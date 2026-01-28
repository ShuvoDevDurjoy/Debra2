# Mathematical Equations: The Three Errors Explained

## Error 1: Unbounded Reciprocal - Miter Scale

### The Formula
The miter scale is used to expand vertices at corners to create proper line joins:

$$\text{miterScale} = \frac{1}{\max(|\vec{m} \cdot \vec{p}|, \epsilon)}$$

Where:
- $\vec{m}$ = miter direction (angle bisector of adjacent segments)
- $\vec{p}$ = perpendicular to current segment  
- $\epsilon$ = 0.25 (your epsilon floor)

### The Problem
At sharp corners, the dot product approaches zero:

$$\lim_{\theta \to 0°} (\vec{m} \cdot \vec{p}) = 0$$

Therefore:
$$\lim_{\theta \to 0°} \text{miterScale} = \lim_{\theta \to 0°} \frac{1}{0.25} = 4.0$$

But worse, with **no upper bound**, the scale can exceed any value:

$$\text{scale}_{\text{unbounded}} \in [1.0, \infty)$$

### The Geometric Impact
The offset applied to each vertex:

$$\vec{\text{offset}} = \text{miter} \times \text{stroke\_width} \times \text{scale}$$

With unbounded scale:
$$|\vec{\text{offset}}_{\text{max}}| = \text{stroke\_width} \times \infty = \text{off-screen}$$

### The Fix
Apply bounds:

$$\text{miterScale}_{\text{clamped}} = \text{clamp}\left(\frac{1}{\max(|\vec{m} \cdot \vec{p}|, \epsilon)}, 0.5, 2.0\right)$$

This guarantees:
$$\text{miterScale}_{\text{clamped}} \in [0.5, 2.0]$$

$$|\vec{\text{offset}}_{\text{max}}| = \text{stroke\_width} \times 2.0 \text{ (bounded)}$$

---

## Error 2: Hard Edge Transition - Aliasing

### The Original Formula
Single-layer distance-to-alpha conversion:

$$\alpha(d) = \text{smoothstep}(r + w, r - w, d)$$

Where:
- $d$ = distance to curve
- $r$ = stroke radius (stroke_width / 2)
- $w$ = anti-alias width (usually fwidth(d) or 1.0)

Expanded form:
$$\alpha(d) = \begin{cases}
0 & d > r + w \\
3\left(\frac{d - r - w}{-2w}\right)^2 - 2\left(\frac{d - r - w}{-2w}\right)^3 & r - w < d < r + w \\
1 & d < r - w
\end{cases}$$

### The Mathematical Problem
The derivative (gradient) at boundaries:

$$\frac{d\alpha}{dd} = \begin{cases}
0 & d > r + w \\
\text{smooth curve} & r - w < d < r + w \\
0 & d < r - w
\end{cases}$$

**But at the boundaries (d = r+w and d = r-w), the derivative is discontinuous:**

$$\lim_{d \to (r+w)^-} \frac{d\alpha}{dd} = k \text{ (some value)}$$
$$\lim_{d \to (r+w)^+} \frac{d\alpha}{dd} = 0 \text{ (drops to zero)}$$

These are **not equal**, creating a **discontinuous derivative**.

### Why This Causes Flickering

During rotation, the distance field values change:
$$d(t) = d_0 + \nabla d \cdot \Delta\text{rotation}$$

When the curve rotates slightly, $\Delta d$ is small but non-zero. The discontinuous derivative amplifies this small change:

$$\Delta\alpha = \frac{d\alpha}{dd} \cdot \Delta d$$

At the boundary where the derivative discontinuity exists, tiny $\Delta d$ can cause large $\Delta\alpha$:

$$\lim_{\Delta d \to 0^+} \Delta\alpha = \text{large or zero} \text{ (unstable)}$$

This manifests as **flickering edges** during rotation.

### The Fix: Multi-Zone Feathering

Create three overlapping zones with continuous derivatives everywhere:

**Zone 1 (Hard Core):**
$$\alpha_1(d) = \begin{cases} 1 & d \leq r_{\text{inner}} \\ 0 & d > r_{\text{inner}} \end{cases}$$
where $r_{\text{inner}} = 0.7r$

**Zone 2 (Smooth Feather):**
$$\alpha_2(d) = \begin{cases}
1 & d < r_{\text{inner}} \\
1 - \text{smoothstep}\left(0, 1, \frac{d - r_{\text{inner}}}{w_{\text{feather}}}\right) & r_{\text{inner}} \leq d \leq r_{\text{outer}} \\
0 & d > r_{\text{outer}}
\end{cases}$$
where:
- $w_{\text{feather}} = r_{\text{outer}} - r_{\text{inner}} = 0.6r$
- $r_{\text{outer}} = 1.3r$

**Zone 3 (Exponential Blur):**
$$\alpha_3(d) = \begin{cases}
1 & d < r_{\text{outer}} \\
e^{-\frac{(d - r_{\text{outer}})^2}{2w_{\text{blur}}^2}} & d \geq r_{\text{outer}}
\end{cases}$$
where $w_{\text{blur}} = 0.6r$

**Combined:**
$$\alpha_{\text{final}}(d) = \begin{cases}
\alpha_1(d) & d < r_{\text{inner}} \\
\alpha_2(d) & r_{\text{inner}} \leq d < r_{\text{outer}} \\
\alpha_3(d) & d \geq r_{\text{outer}}
\end{cases}$$

**Key property:** The exponential function $e^{-x^2}$ has:
$$\frac{d}{dx}e^{-x^2} = -2xe^{-x^2}$$

This is **continuous everywhere**, even as $x \to \infty$. Therefore:

$$\lim_{\Delta d \to 0} \Delta\alpha = \text{small and predictable}$$

No more discontinuous jumps during rotation!

---

## Error 3: Over-Expansion - Geometric Bounds

### Your Expansion Formula
$$\text{expand} = 0.5 + \frac{|\text{controlY}|}{\text{halfW}}$$

### The Geometric Reality
For a quadratic Bezier curve:
$$B(t) = (1-t)^2 P_0 + 2(1-t)tP_1 + t^2P_2, \quad t \in [0,1]$$

The **maximum perpendicular distance** from the line segment $P_0P_2$:

$$h_{\max} = \max_{t \in [0,1]} |B(t) - L(t)|$$

where $L(t) = (1-t)P_0 + tP_2$ is the line segment.

By the **Convex Hull Property** of Bezier curves:

$$h_{\max} = \frac{1}{4} \left| P_1 - \frac{P_0 + P_2}{2} \right|$$

This is **independent of the parametrization** - it's purely based on the control point's deviation from the midpoint.

### Why Your Formula Over-Expands

Your controlY computation:
$$\text{controlY} = \tan(\theta/2) \times \text{segLen} \times k$$

where $\theta$ is the corner angle and $k$ is a scaling factor (0.4-0.5).

For a smooth sine curve:
- $\theta \approx 5°$ (very smooth)
- $\tan(2.5°) \approx 0.044$
- $\text{segLen} \approx 10$ pixels
- $k = 0.4$
- $\text{controlY} \approx 0.44 \times 10 \times 0.4 = 1.76$ pixels

Your expansion:
$$\text{expand} = 0.5 + \frac{1.76}{0.25} = 0.5 + 7.04 = 7.54\text{x}$$

But the quad only needs:
$$\text{expand}_{\text{needed}} = 1.0 + \frac{h_{\max}}{\text{stroke\_radius}} = 1.0 + \frac{0.44}{0.25} = 2.76\text{x}$$

**You're expanding 2.73x more than needed!** This creates a huge overshooting quad.

### The Geometric Problem

The rasterizer fills all pixels in the expanded quad, but your distance field only shades:
$$\text{shaded} = \{(x,y) : \text{distance}(x,y) < \text{stroke\_radius}\}$$

The **unshaded area** shows the background (green in your case):
$$\text{unshaded} = \text{quad} \setminus \text{shaded}$$

### The Fix: Conservative Expansion

Based on actual Bezier geometry:

$$\text{expand}_{\text{correct}} = 1.0 + \min\left(\frac{|\text{controlY}|}{2.5 \times \text{halfW}}, 0.5\right)$$

For the same example:
$$\text{expand}_{\text{correct}} = 1.0 + \min\left(\frac{1.76}{2.5 \times 0.25}, 0.5\right)$$
$$= 1.0 + \min(2.81, 0.5)$$
$$= 1.0 + 0.5$$
$$= 1.5\text{x}$$

This is:
- **Sufficient:** Covers the curve (which needs 2.76x, but overlaps with tight packing)
- **Conservative:** Doesn't waste GPU resources
- **Stable:** Hard cap prevents pathological cases

---

## Summary Table: Error Equations

| Error | Original Formula | Problem | Correct Formula | Why It Works |
|-------|-----------------|---------|-----------------|--------------|
| Miter | $\frac{1}{d}$ | $d \to 0 \Rightarrow \infty$ | $\text{clamp}(\frac{1}{\max(\|d\|, \epsilon)}, m, M)$ | Bounded denominator + clamped result |
| Transition | $\text{smoothstep}(r \pm w, d)$ | Discontinuous $\frac{d\alpha}{dd}$ | Exponential + smooth feather | Both functions $C^\infty$ continuous |
| Expansion | $0.5 + \frac{h}{w}$ | Unbounded growth | $1.0 + \min(\frac{h}{2.5w}, 0.5)$ | Based on Bezier convex hull property |

