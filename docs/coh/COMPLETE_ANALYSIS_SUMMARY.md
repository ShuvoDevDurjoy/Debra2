# Complete Analysis: Your Bezier Curve Issues - Summary

## What Was Wrong

You had **three mathematical errors** in your bezier curve stroke rendering:

### 1. **Breaking Point in Sine Curve** 
- **Root Cause:** Unbounded miter scale calculation
- **Math:** `miterScale = 1.0 / dot_product` with no upper limit
- **Problem:** When dot_product → 0, scale → ∞, vertices move infinitely far
- **Fix:** Add clamp: `clamp(scale, 0.5, 2.0)`
- **Why it happened:** You forgot to add bounds to a reciprocal operation

### 2. **Flickering Edges During Rotation**
- **Root Cause:** Single hard transition + insufficient adaptive anti-aliasing
- **Math:** Single `smoothstep()` function creates discontinuous gradient
- **Problem:** Rotating changes distance values slightly → alpha suddenly jumps 0→1
- **Fix:** Use three-zone feathering with exponential falloff
- **Why it happened:** You used a sharp mathematical function for a smooth visual effect

### 3. **Green Rounded Cutoff Artifacts**
- **Root Cause:** Over-expansion of bounding quad geometry
- **Math:** Linear expansion formula without upper bound
- **Problem:** Quad expands 2.0-2.3x while curve only needs 1.2-1.3x → unfilled areas
- **Fix:** Conservative formula with hard cap at 1.4x
- **Why it happened:** You didn't consider the actual geometric bounds of a Bezier curve

---

## Mathematical Deep Dive

### Issue #1: The Reciprocal Problem

**The Formula:**
$$\text{miterScale} = \frac{1}{\max(|\vec{m} \cdot \vec{p}|, 0.25)}$$

Where $\vec{m}$ is the miter direction (bisector) and $\vec{p}$ is the perpendicular to current segment.

**When does this break?**

The dot product $\vec{m} \cdot \vec{p}$ measures how "perpendicular" the miter is to the stroke direction. For different angles:

| Angle | Dot Product | Scale | Status |
|-------|------------|-------|--------|
| 180° (straight) | 1.0 | 1.0 | ✓ Perfect |
| 90° (right angle) | 0.707 | 1.41 | ✓ Good |
| 30° (sharp) | 0.49 | 2.04 | ✓ Acceptable |
| 5° (very sharp) | 0.25 | 4.0 | ⚠️ Large |
| 0.1° (nearly parallel) | 0.001 | 1000 | ✗ **EXPLODES** |

Your code hit the case where the curve is nearly parallel (like a smooth sine curve), causing infinite expansion.

**The mathematical principle:** Never compute $\frac{1}{x}$ without:
1. Checking that $x \neq 0$
2. Adding epsilon: $\frac{1}{\max(|x|, \epsilon)}$
3. Bounding the result: $\text{clamp}(\frac{1}{\max(|x|, \epsilon)}, \text{min}, \text{max})$

---

### Issue #2: The Aliasing Problem

**The Theory:**

Aliasing occurs when a **discontinuous function** is sampled on a discrete pixel grid.

Your original function:
$$\alpha(d) = \text{smoothstep}(r + w, r - w, d) = \begin{cases} 
0 & d > r + w \\
\text{smooth} & r - w < d < r + w \\
1 & d < r - w
\end{cases}$$

This has a discontinuous **derivative** at the boundaries, creating:
- Sharp transitions in alpha (0→1 jump)
- Sensitive to subpixel variations
- Different fragments sample different parts of the jump during rotation

**The Solution:**

Use multiple **smooth, overlapping functions**:

**Zone 1 (Core):** 
$$\alpha_{\text{core}}(d) = \begin{cases} 1 & d < r_{\text{inner}} \\ 0 & d > r_{\text{inner}} \end{cases}$$

**Zone 2 (Feather):**
$$\alpha_{\text{feather}}(d) = 1 - \text{smoothstep}(0, 1, \frac{d - r_{\text{inner}}}{w})$$

**Zone 3 (Blur):**
$$\alpha_{\text{blur}}(d) = e^{-\frac{(d - r_{\text{outer}})^2}{w^2}}$$

**Combined (all smooth with no discontinuities):**
$$\alpha_{\text{final}}(d) = \begin{cases}
\alpha_{\text{core}}(d) & d < r_{\text{inner}} \\
\alpha_{\text{feather}}(d) & r_{\text{inner}} < d < r_{\text{outer}} \\
\alpha_{\text{blur}}(d) & d > r_{\text{outer}}
\end{cases}$$

Each piece has **continuous derivative everywhere**, so rotating causes smooth changes, never jumps.

---

### Issue #3: The Over-Expansion Problem

**The Geometry:**

A quadratic Bezier curve $B(t) = (1-t)^2 P_0 + 2(1-t)t P_1 + t^2 P_2$ has a key property:

**Maximum deviation from the line segment:**
$$h_{\max} = \frac{1}{4} \left| P_1 - \frac{P_0 + P_2}{2} \right|$$

This is the furthest the curve can bulge perpendicular to the straight line.

Your expansion formula:
$$\text{expand} = 0.5 + \frac{|\text{controlY}|}{w}$$

With controlY ≈ h (the computed control point offset):
$$\text{expand} = 0.5 + \frac{h}{w} = 0.5 + \frac{h}{0.25} = 0.5 + 4h$$

For $h = 0.3$: expand = 0.5 + 1.2 = 1.7x

But the quad only needs to expand by:
$$\text{expansion needed} = \frac{w + h_{\max}}{w} = 1.0 + \frac{h_{\max}}{w} \approx 1.1 \text{ to } 1.2\text{x}$$

You're expanding **1.7x when only 1.2x is needed**, leaving unfilled corners.

**The correct formula:**
$$\text{expand} = 1.0 + \min\left(\frac{h}{2.5w}, 0.5\right)$$

For $h = 0.3$: expand = 1.0 + min(0.48, 0.5) = 1.48x

Still covers the curve (1.2x needed) with some buffer, but doesn't waste GPU resources.

---

## Key Principles Learned

### 1. Always Bound Reciprocals
```glsl
// ✗ BAD
result = 1.0 / denominator;

// ✓ GOOD
result = clamp(1.0 / max(abs(denominator), epsilon), min_value, max_value);
```

### 2. Avoid Sharp Transitions in Smooth Visuals
```glsl
// ✗ BAD
alpha = step(threshold, value);  // Binary jump

// ✓ GOOD
alpha = smoothstep(threshold - 0.1, threshold + 0.1, value);  // Smooth
// OR (best)
alpha = exponential_decay(value);  // Naturally smooth
```

### 3. Consider Geometric Bounds
```glsl
// ✗ BAD
expansion = base_value + linear_factor * input;  // Can grow forever

// ✓ GOOD
expansion = clamp(base_value + linear_factor * input, min_bound, max_bound);
// where min_bound and max_bound come from geometric analysis
```

### 4. Test with Animation
```
❌ WRONG: Testing only with static screenshots
✓ RIGHT: Testing with:
  - Rotation at all angles (catches aliasing issues)
  - Zooming in/out (catches expansion issues)
  - Smooth curves at all resolutions (catches precision issues)
```

---

## How to Verify the Fixes

### Test 1: Sharp Corners
```cpp
// Create a curve with a 10° sharp turn
// BEFORE: Visible gap in rendering ✗
// AFTER: Smooth corner with proper miter ✓
```

### Test 2: Rotation Test
```cpp
// Render sine curve
// Rotate continuously: model = rotate(model, θ)
// BEFORE: Edges flicker/shimmer during rotation ✗
// AFTER: Smooth, stable edges at all angles ✓
```

### Test 3: Zoom Test
```cpp
// Render sine curve
// Zoom in to 200%, 400%, 1600%
// BEFORE: Green artifacts at high zoom ✗
// AFTER: Properly shaded at all zoom levels ✓
```

---

## The Universal Graphics Lesson

These three bugs are **common in all graphics work** because they represent fundamental misunderstandings:

1. **Unbounded Reciprocals** - Used in lighting, depth fog, falloff calculations
2. **Hard Transitions** - Used in many shaders for simple effects
3. **Over-expansion** - Used in geometry amplification (parallax mapping, extrusion, etc.)

The fixes are **universal patterns** that apply to 90% of graphics programming bugs:

```
When you compute any formula that:
  □ Divides by something
  □ Has a step/transition
  □ Expands geometry
  
Always add:
  □ Epsilon to prevent division by zero
  □ Smooth interpolation instead of hard steps
  □ Bounds based on actual geometric constraints
```

---

## Recommended Reading

1. **"Robust Graphics Programming"** - Focus on defensive programming
2. **GPU Gems - Chapter on Distance Field Rendering** - How to properly shade distance fields
3. **Valve's SDF papers** - Industrial-strength text rendering with SDFs

The key insight: **Graphics programming rewards defensive, paranoid code**. Always assume the worst case and bound it.

