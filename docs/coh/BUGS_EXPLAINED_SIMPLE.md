# Quick Summary: Your Three Bezier Bugs Explained

## TL;DR - What You Did Wrong

Your implementation had **three cascading numerical errors** that are common in graphics programming:

### Bug #1: Unbounded Reciprocal (Breaking Point)
```glsl
// WRONG:
float miterScale = 1.0 / dot_product;  // Can explode to infinity!

// RIGHT:
float miterScale = clamp(1.0 / max(abs(dot_product), 0.25), 0.5, 2.0);
```
**Why it matters:** At sharp corners, the dot product approaches zero, making the reciprocal explode. This pushes vertices infinitely far, breaking the segment rendering.

---

### Bug #2: Single Hard Transition (Flickering)
```glsl
// WRONG:
float alpha = smoothstep(r + w, r - w, distance);  // Binary jump

// RIGHT:
// Three zones: hard core → smooth feather → soft blur
// Use exponential falloff in outer zones to avoid discontinuities
```
**Why it matters:** A single sharp transition aliases badly. When you rotate, the curve's distance values shift slightly, causing the alpha to snap between visible/invisible. Multi-layer feathering ensures smooth transitions regardless of rotation angle.

---

### Bug #3: Unbounded Expansion (Green Artifacts)
```glsl
// WRONG:
float expand = 0.5 + abs(controlY) / halfW;  // No upper limit!

// RIGHT:
float expand = 1.0 + min(abs(controlY) / (2.5 * halfW), 0.5);  // Capped at 1.5x
```
**Why it matters:** Over-expanding the quad creates unfilled areas that show the green background. The formula needs a hard cap based on the maximum possible Bezier curve deviation.

---

## The Underlying Pattern

All three bugs follow the same **numerical antipattern**:

```
❌ Pattern (What you did):
   result = unconstrained_calculation;  // Can be anything!
   
✓ Pattern (What's correct):
   result = clamp(calculation, min_sensible, max_sensible);
```

In graphics, **never trust unbounded formulas**. Always ask:
- "What's the minimum this could be?"
- "What's the maximum this should be?"
- "What happens if the input approaches zero?"

---

## Mathematical Root Causes

| Bug | Formula Problem | Mathematical Issue | Visual Symptom |
|-----|-----------------|-------------------|-----------------|
| Miter Scale | $\frac{1}{x}$ with $x \to 0$ | Division by small numbers causes infinity | Vertices explode off-screen, segments vanish |
| Edge Transition | Hard step function | Discontinuous gradient, aliasing at subpixel level | Flickers during rotation/camera movement |
| Expansion | Linear growth without bound | Formula grows faster than geometric properties | Quad overshoots coverage zone |

---

## Why These Are Hard to Find

1. **Breaking Point:** Happens at specific frame indices (depends on curve complexity and angle). Hard to reproduce consistently.

2. **Flickering:** Only visible during animation/rotation. Static screenshots look fine, creating false sense of correctness.

3. **Green Artifacts:** Looks like a clipping issue, not a math issue. Easy to blame the graphics pipeline instead of the shader logic.

---

## The General Lesson

When implementing geometric algorithms in shaders, follow this checklist:

```
□ Are you dividing? Add epsilon to denominator: max(x, 1e-5)
□ Is the result a scale factor? Clamp it: clamp(x, 0.1, 10.0)
□ Are you using smoothstep? Consider if discontinuities exist at boundaries
□ Expanding geometry? Put an upper bound on expansion
□ Using fwidth for AA? Combine with derivative magnitude for stability
□ Testing hard-to-see artifacts? Use rotation and zoom heavily in testing
```

---

## References in Your Code

Looking at your actual implementation, the correct patterns are already there:

**✓ You DID this right:**
```glsl
max(abs(dot_product), 0.25)  // Epsilon to prevent division by zero
clamp(bias, 0.1, 0.9)         // Scale factor clamping
```

**✗ You missed these:**
```glsl
min(miterScale, MITER_LIMIT)   // ← You added this in fix, was missing before
max(pixelSize, fwidth(d))      // ← Combine both metrics for AA
min(expand, 1.5)               // ← Cap the expansion, was missing before
```

The fixes I provided add the **missing bounds checks** that prevent the numerical explosions.

---

## Testing to Verify the Fixes

To confirm each fix works:

1. **Miter Scale Fix:**
   - Render a sharp corner (>60°)
   - Should not create a gap
   - No explosion of geometry

2. **Edge Transition Fix:**
   - Rotate the model continuously
   - Edges should appear smooth, never flickering
   - Same visual quality at all angles

3. **Expansion Fix:**
   - Render smooth sine curve
   - No green artifacts at the edges
   - All pixels properly shaded

