# Implementation Plan

## Overview
Fix the broken transition morphing animation in Transition.cpp where point interpolation between GraphObject instances fails to visibly morph due to logic errors, incomplete OpenGL buffer updates, and improper dirty flag handling. The animation system aims to smoothly interpolate between two GraphObjects over a specified duration by aligning their point counts and lerping vertex positions, but current issues prevent visual updates during runtime.

The core problem stems from three main areas:
1. **Logic error in play() timing**: The animation skips morphing when `start_time <= dt < end_time`, executes full morph at `dt == end_time`, and incorrectly early-exits immediately after.
2. **Missing OpenGL buffer updates**: `morphPoints()` modifies `targetObject->points` in memory but fails to propagate changes to stroke/fill VAOs via `setStrokeData()` and `uploadStrokeDataToShader()`.
3. **Stroke data staleness**: Morphing bypasses `stroke_dirty` flag and GPU synchronization needed for shader rendering.

The fix integrates seamlessly with existing `alignPoints()` (which resamples curves to matching resolution), `interpolate()` (vertex-wise lerp), and `updateStroke()` (rendering pipeline), requiring minimal changes while ensuring real-time visual feedback.

## Types
No new types, interfaces, or data structures required. Leverage existing:
- `GraphObject* targetObject` (source points)
- `GraphObject* morphObject` (target points, misnamed but functional)
- `std::vector<glm::vec3> points` (vertex positions)
- `bool stroke_dirty` (triggers GPU sync)
- `GLuint StrokeVAO, StrokeVBO` (render buffers)

## Files
Modify one existing file:
- `src/Animations/Transition.cpp`: Core logic fixes for timing, morphing, and GPU sync.
No new files. No deletions. No config changes.

## Functions
Modify three functions in `src/Animations/Transition.cpp`:
- `Transition::play(float dt)`: Fix timing logic, trigger `morphPoints()` correctly during active duration, remove early exit, call `updateStrokeDataToShader()` post-morph.
- `Transition::morphPoints(float alpha)`: Add `stroke_dirty = true` after interpolation, followed by `setStrokeData()` and `uploadStrokeDataToShader()` to sync GPU.
- `Transition::Init()`: Minor logging cleanup (optional), ensure `morphObject` is valid post-alignment.

No new functions. No removals.

## Classes
Modify `Transition` class behavior in `src/Animations/Transition.cpp`:
- Add GPU buffer sync after point mutation in morphing path.
- No structural changes to constructor, fields, or inheritance from `Animation`.

## Dependencies
No new dependencies. Uses existing GLM, OpenGL, and GraphObject infrastructure.

## Testing
Manual validation via existing GraphApp rendering loop:
1. Create two mismatched GraphObjects (e.g., sine wave → circle via bezier paths).
2. Instantiate `Transition(obj1, obj2, 0.0f, 2.0f)`.
3. Advance `dt` in 16ms increments; verify smooth morphing in real-time.
4. Check console logs pre/post-alignment for point count matching.
5. Validate final state matches `morphObject` points exactly at `alpha=1.0`.

Unit test not implemented (rendering dependencies), but fixes are atomic and regression-safe.

## Implementation Order
1. Fix `play()` timing logic and call sequence.
2. Enhance `morphPoints()` with stroke dirty flag and full GPU upload.
3. Test via manual run (rebuild + execute main with transition demo).
4. Clean up debug prints if confirmed working.

