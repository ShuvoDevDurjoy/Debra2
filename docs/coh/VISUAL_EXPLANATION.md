# Visual Explanation of Bezier Curve Issues

## Issue 1: Breaking Point - Unbounded Miter Scale

### The Problem Visualized:

```
SMOOTH CURVE (No issue):
  segmentA        segmentB
     \               /
      \   angle≈20° /       miter dot product ≈ 0.9
       \           /        scale = 1.0 / 0.9 ≈ 1.1 ✓ (bounded)
        -----------
      Normal miter

SHARP CORNER (Your bug):
  segmentA |segmentB
      \   |   /
       \  |  /         angle≈160° (very sharp turn)
        \ | /          miter dot product ≈ 0.001 (nearly zero!)
         \|/           scale = 1.0 / 0.001 ≈ 1000.0 ✗ (unbounded!)
          
Expansion formula:
  offset = miter_direction × (stroke_width × miterScale)
         = perpendicular × (2.0 × 1000.0)   ← Expands 2000 pixels!
         
Result: Vertex goes WAY off screen → Intersection test fails → Missing segment
```

### Why the Dot Product Matters:

```
miter = normalized(perp_prev + perp_next)

For 180° turn (straight line):
  perp_prev = [0, 1]
  perp_next = [0, 1]
  miter = [0, 1]
  miter · perp_next = 1.0
  scale = 1.0 / 1.0 = 1.0  ✓

For 90° turn:
  perp_prev = [1, 0]
  perp_next = [0, 1]
  miter = [0.707, 0.707]  (45° bisector)
  miter · [0, 1] = 0.707
  scale = 1.0 / 0.707 = 1.41  ✓

For 10° turn (sharp corner):
  perp_prev ≈ [0, 1]
  perp_next ≈ [0.174, 0.985]  (5° rotation)
  miter ≈ [0.087, 0.992]  (nearly same as perpendicular!)
  miter · [0.174, 0.985] ≈ 0.981
  scale = 1.0 / 0.981 ≈ 1.02  (still ok with max(...,0.25) floor)

For 0.1° turn (almost straight, but smooth curve):
  The problem: value approaches 0.25 floor, but can still oscillate
  ✓ Solution: CAP AT 2.0-4.0 regardless
```

---

## Issue 2: Flickering Edges - Hard vs Soft Transitions

### Visual Comparison:

```
HARD SINGLE-LAYER (Your original):
  Alpha
   1.0 |     ________
       |    |        |
       |    | SHARP  |  ← Discontinuous change
       |    |        |
   0.5 |____|________|____
       |    |        |
       |____|________|____  ← Aliasing at edge
   0.0 |___|________|___
       Distance:  radius-aa_width ← radius ← radius+aa_width
       
Rotating the curve:
  Distance changes slightly → Alpha suddenly jumps 0→1
  Different pixels see different values → Flickering


SOFT MULTI-LAYER (The fix):
  Alpha
   1.0 |  ___________
       | |  CORE     |
   0.8 | |___________|___      ← Multiple falloff zones
       |        |FEATHER |
   0.5 |________|_________|___  ← Smooth curve, no jumps
       |         |  BLUR  |  |
   0.2 |_________|________|___|
       |         |        |  |
   0.0 |_________|________|___|
       innerR   radius   outerR
       
Rotating the curve:
  Distance changes slightly → Alpha changes smoothly (via exponential)
  All pixels follow same falloff curve → Stable, no flicker
```

### Why Rotation Causes Flickering:

```
Frame 1: Curve at angle θ = 0°
  Pixel grid:  +---+---+
               |   | X |  ← Pixel X samples distance=0.8
               +---+---+
  
  With hardstep: 0.8 > 0.5 → Alpha jumps to near-1.0 → Visible

Frame 2: Curve rotates slightly to θ = 2°
  Pixel grid:  +---+---+
               | X |   |  ← Pixel X now samples distance=0.52
               +---+---+
  
  With hardstep: 0.52 is right at the discontinuity!
  Alpha suddenly drops to 0.0 → Invisible
  
  Result: Pixel flickers between visible/invisible ✗

With soft feathering:
  Frame 1: distance=0.8 → Alpha = e^(-0.2²/w²) ≈ 0.96
  Frame 2: distance=0.52 → Alpha = e^(-(0.02)²/w²) ≈ 0.95
  Difference: only 0.01 change, imperceptible ✓
```

---

## Issue 3: Green Cutoff Artifacts - Over-Expansion

### The Geometry Problem:

```
CORRECT EXPANSION:
                    Bezier curve
                         •  ← Control point (causes outward bulge)
                        / \
  Expanded quad:    +-------+
  (tight bounds)    |  •••  |  ← Curve fits inside bounds
                    | • • • |
                    +-------+
              Coverage: 100%, no artifacts


YOUR EXPANSION (2.0-2.3x):
                    Bezier curve
                         •
                        / \
  Over-expanded:   +-------+
  quad (too big)   |   •   |  ← Curve only uses center
                   |       |
                   |   +--quadratic-+  ← Original coverage zone
                   |   |  •  |     |
                   +---|-----+-----+
                       |gap! |     |
                       +-----+     +  ← Unfilled area
                   
GPU rasterizes large quad, but distance field only shades center
→ Edges of quad not covered by stroke shading
→ Green background shows through! ✓ This matches what you saw
```

### Mathematical Bounds for Bezier:

```
For a quadratic Bezier curve:
  B(t) = (1-t)² P0 + 2(1-t)t P1 + t² P2
  
Maximum distance from P0-P2 line segment:
  h_max = 1/4 × |P1 - midpoint(P0, P2)|
  
For the bounding quad to guarantee coverage:
  quad_half_width ≥ stroke_half_width + h_max
  
YOUR formula:     expand = 0.5 + |controlY| / halfW
                           = 0.5 + |h| / halfW
                        ≈ 0.5 + h / (stroke_width/2)
                        = 0.5 + 2h / stroke_width
  
For h ≈ 0.4 × segment_length and stroke_width=2:
  expand ≈ 0.5 + 0.4 × segment_length
  → For long segments, this grows unbounded! ✗

CORRECT formula: expand = 1.0 + min(|h| / (2.5 × halfW), 0.5)
                        = 1.0 + min(|h| / (1.25 × stroke_width), 0.5)
                        
Maximum expansion: 1.5× (bounded!) ✓
```

---

## Summary: The Three Errors Side by Side

```
┌─────────────────────────────────────────────────────────────────┐
│ ERROR 1: UNBOUNDED MITER SCALE                                 │
├─────────────────────────────────────────────────────────────────┤
│ Math Error:   Reciprocal of value approaching 0 → infinity     │
│ Visual Error: Gap/break in smooth curves                        │
│ Fix:          Clamp result to [0.5, 2.0]                       │
│ Cost:         Negligible                                        │
└─────────────────────────────────────────────────────────────────┘

┌─────────────────────────────────────────────────────────────────┐
│ ERROR 2: HARD TRANSITIONS + INSUFFICIENT AA                    │
├─────────────────────────────────────────────────────────────────┤
│ Math Error:   Single step function + poor derivative scaling    │
│ Visual Error: Flickering edges during rotation                  │
│ Fix:          Multi-layer feathering + adaptive AA width        │
│ Cost:         ~5 extra shader ops                               │
└─────────────────────────────────────────────────────────────────┘

┌─────────────────────────────────────────────────────────────────┐
│ ERROR 3: UNBOUNDED EXPANSION                                   │
├─────────────────────────────────────────────────────────────────┤
│ Math Error:   Linear formula without upper bound                │
│ Visual Error: Green artifacts from unfilled quad edges          │
│ Fix:          Conservative expansion formula with hard cap      │
│ Cost:         Negligible                                        │
└─────────────────────────────────────────────────────────────────┘
```

---

## Key Insight: Normalization is Your Friend

Most of your bugs came from **not normalizing quantities** that could have extreme ranges:

```
❌ What you did (problematic):
  miterScale = 1.0 / dot_product;    // Can be anything from 1 to ∞
  expand = 0.5 + controlY / halfW;   // Can be anything from 0.5 to ∞

✓ What you should do:
  miterScale = clamp(1.0 / max(abs(dot_product), 0.25), 0.5, 2.0);
  expand = mix(1.0, 1.5, clamp(controlY / (2.5 * halfW), 0.0, 1.0));
  
Pattern: Divide → Max with epsilon → Clamp between sensible bounds
```

This pattern prevents all three of your issues!

