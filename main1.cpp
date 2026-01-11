// #include "../include/GraphEngine/Graphs/Graph.hpp"

// #include "../include/GraphEngine/Graphs/Coordinates/CylindricalCoordinate.hpp"
// #include "../include/GraphEngine/Graphs/Coordinates/CartesianCoordinate.hpp"
// #include "../include/GraphEngine/Graphs/ThreeDAxes.hpp"
// #include "../include/GraphEngine/Objects/Rectangle.hpp"
// #include "../include/GraphEngine/Objects/Polygon.hpp"
// #include "../include/GraphEngine/Graphs/GraphObjects/TestObject.hpp"
// #include "../include/GraphEngine/Graphs/GraphObjects/GraphMathObject.hpp"
// // #include "../include/GraphEngine/Graphs/GraphObjects/Graphing/Line.hpp"
// #include "../include/GraphEngine/Graphs/GraphObjects/Graphing/GridBox.hpp"
// #include "../include/GraphEngine/Graphs/GraphObjects/Graphing/Dot.hpp"
// #include "../include/GraphEngine/Graphs/GraphObjects/Shapes/Circle.hpp"
// #include "../include/GraphEngine/Graphs/GraphObjects/Shapes/BasePolygon.hpp"
// #include "../include/GraphEngine/Animations/Creation.hpp"
// #include "../include/GraphEngine/Animations/Translate.hpp"

// // Paraboloid function
// Vec3 hollowParaboloid(float r, float phi, Var v)
// {
//     float y = r * r * r; // height along Y-axis
//     float x = r * cos(phi);
//     float z = r * sin(phi);

//     return Vec3(x, y, z);
// }

// // Define scaling constants for the noise input
// const float NOISE_SCALE = 0.05f;
// const float HEIGHT_FACTOR = 10.0f; // Multiplier to make the hills taller

// #include <cmath>
// #include <iostream>

// // --- Perlin Noise Constants and Utility Functions ---

// // Array of pre-computed random gradient vectors at the 16 unique corners (4x4 grid)
// // These define the slope/direction of the "randomness" at each integer corner.
// // Only 12 are needed for 2D, but 16 simplifies indexing.
// const Vec2 GRADIENTS[] = {
//     {1, 1}, {-1, 1}, {1, -1}, {-1, -1}, {1, 0}, {-1, 0}, {0, 1}, {0, -1}, {1, 1}, {-1, 1}, {1, -1}, {-1, -1}, {1, 0}, {-1, 0}, {0, 1}, {0, -1}};

// // A permutation array to create "random" lookups from integer coordinates
// // Size must be 256 or 512 (512 allows seamless wrapping).
// int P[512];

// // Smoothstep function (6t^5 - 15t^4 + 10t^3) for better interpolation curve
// float fade(float t)
// {
//     return t * t * t * (t * (t * 6.0f - 15.0f) + 10.0f);
// }

// // Linear interpolation between a and b based on weight t
// float lerp_n(float a, float b, float t)
// {
//     return (1.0f - t) * a + t * b;
// }

// // Dot product of a gradient vector and a distance vector
// float dotGridGradient(int ix, int iy, float x, float y)
// {
//     // 1. Get the random gradient index based on the grid coordinates
//     // We use a simplified hash based on P[] to get a stable random vector
//     int index = P[P[ix & 255] + (iy & 255)] & 15; // & 15 limits the index to 0-15

//     // 2. Calculate the distance vector from the grid point (ix, iy) to the sample point (x, y)
//     float dx = x - (float)ix;
//     float dy = y - (float)iy;

//     // 3. Return the dot product
//     return (dx * GRADIENTS[index].x + dy * GRADIENTS[index].y);
// }

// // --- Perlin Noise Function ---

// float PerlinNoise2D(float x, float y)
// {
//     // 1. Determine grid cell coordinates
//     int x0 = (int)std::floor(x);
//     int y0 = (int)std::floor(y);
//     int x1 = x0 + 1;
//     int y1 = y0 + 1;

//     // 2. Determine interpolation welerights (t)
//     float tx = x - (float)x0; // Fractional part of X
//     float ty = y - (float)y0; // Fractional part of Y

//     // 3. Apply the smoothstep function to weights
//     float u = fade(tx);
//     float v = fade(ty);

//     // 4. Calculate the four dot products between the gradient and distance vectors
//     // These give the influence of each corner on the sample point.
//     float n0 = dotGridGradient(x0, y0, x, y);
//     float n1 = dotGridGradient(x1, y0, x, y);
//     float n2 = dotGridGradient(x0, y1, x, y);
//     float n3 = dotGridGradient(x1, y1, x, y);

//     // 5. Interpolate (Lerp_n) the results
//     // Interpolate along X:
//     float ix1 = lerp_n(n0, n1, u);
//     float ix2 = lerp_n(n2, n3, u);

//     // Interpolate along Y:
//     float finalValue = lerp_n(ix1, ix2, v);

//     // The output is typically in the range [-1.0, 1.0]
//     return finalValue;
// }

// // --- Initialization and Octave/FBM Function ---

// // Fills the permutation array P[] with a predictable random sequence
// void initializePerlin()
// {
//     // ... (P[i] = i setup is fine)

//     // FIX: Use a safe, standard way to generate a repeatable random index.
//     for (int i = 0; i < 256; i++)
//     {
//         int swap_index_range = 256 - i;

//         // Use a simple, non-overflowing hash to get a 'random' value
//         // The simple hash avoids the large integer multiplication and casting issues.
//         float hash_val = std::sin(i * 12.9898f + 78.233f); // Add offset for better spread

//         // Map hash_val ([-1, 1]) to [0, swap_index_range - 1]
//         int target_offset = (int)((hash_val * 0.5f + 0.5f) * (float)swap_index_range);

//         // Ensure index is within the valid range
//         int target_index = target_offset + i;

//         // --- SAFE SWAP ---
//         int temp = P[i];
//         P[i] = P[target_index];
//         P[target_index] = temp;
//     }

//     // ... (P[i + 256] = P[i] duplication is fine)
// }

// // Fractal Brownian Motion (FBM) - combines multiple layers (octaves) of noise
// // to create greater detail and smoothness.
// extern float getPerlinHeight(float x, float z)
// {
//     static bool initialized = false;
//     if (!initialized)
//     {
//         initializePerlin();
//         initialized = true;
//     }

//     const int OCTAVES = 4;
//     const float PERSISTENCE = 0.5f; // How much each octave contributes
//     const float LACUNARITY = 2.0f;  // How much frequency increases per octave

//     float total = 0.0f;
//     float amplitude = 1.0f;
//     float frequency = 1.0f;
//     float maxValue = 0.0f; // Used for normalizing the final output

//     for (int i = 0; i < OCTAVES; i++)
//     {
//         total += PerlinNoise2D(x * frequency, z * frequency) * amplitude;

//         maxValue += amplitude;
//         amplitude *= PERSISTENCE;
//         frequency *= LACUNARITY;
//     }

//     // Normalize the output to be in the range of approximately [-1.0, 1.0]
//     return total / maxValue;
// }

// Vec3 generateHillsPolar(float r, float phi, Var v)
// {
//     // 1. Convert polar coordinates (r, phi) to Cartesian coordinates (x, z)
//     float x = r * cos(phi);
//     float z = r * sin(phi);

//     // 2. Calculate the height (y) based on the (x, z) position
//     // Use the noise function for smooth, natural hill displacement.
//     // The noise input (x * NOISE_SCALE) controls the size/frequency of the hills.
//     float rawNoise = getPerlinHeight(x * NOISE_SCALE, z * NOISE_SCALE);

//     // Scale the noise output to control the maximum hill height
//     // Noise typically returns a value between -1.0 and 1.0.
//     float y = rawNoise * HEIGHT_FACTOR;

//     // 3. Return the 3D vector (x, y, z)
//     return Vec3(x, y, z);
// }

// Vec3 hollowParaboloidX(float r, float phi, Var v)
// {
//     float x = r * r - 15.0f; // height along X-axis
//     float y = r * cos(phi);
//     float z = r * sin(phi);

//     return Vec3(x, y, z);
// }

// glm::vec3 hollowCylinder(float h, float phi, Var v)
// {
//     float r = 5.0f;
//     float x = r * cos(phi);
//     float y = h;
//     float z = r * sin(phi);
//     return glm::vec3(x, y, z);
// }

// Vec3 torusLike(float r, float phi, Var v)
// {
//     float R = 1.0f; // major radius
//     float t = phi;  // sweep

//     float x = (R + r * cos(t)) * cos(t) - 5.0f;
//     float y = r * sin(phi);
//     float z = (R + r * cos(t)) * sin(t);

//     return Vec3(x, y, z);
// }

// Vec3 torus(float u, float v, Var vars)
// {
//     float R = vars[0]; // major radius
//     float r = vars[1]; // minor radius

//     float x = (R + r * cos(u)) * cos(v);
//     float y = r * sin(u);
//     float z = (R + r * cos(u)) * sin(v);

//     return Vec3(x, y, z);
// }

// Vec3 mobius(float u, float v, Var)
// {
//     // u in [0, 2π], v in [-1, 1]
//     float R = 10.0f;
//     float x = (R + v * cos(u / 2)) * cos(u);
//     float y = (R + v * cos(u / 2)) * sin(u);
//     float z = v * sin(u / 2);
//     return Vec3(x, y, z);
// }

// Vec3 sphere(float theta, float phi, Var)
// {
//     // theta: 0 → π, phi: 0 → 2π
//     float r = 3.0f;
//     float x = r * sin(theta) * cos(phi);
//     float y = r * sin(theta) * sin(phi);
//     float z = r * cos(theta);
//     return Vec3(x, y, z);
// }

// Vec3 saddle(float x, float y, Var)
// {
//     float z = x * x - y * y; // f(x, y) = x^2 - y^2
//     return Vec3(x, y, z);
// }

// Vec3 ripple(float x, float y, Var v)
// {
//     float t = v[0]; // time for animation
//     float z = 5.0f * sin(sqrt(x * x + y * y) - t);
//     return Vec3(x, y, z);
// }

// Vec3 flower(float u, float v, Var)
// {
//     float R = 2.0f + 5.0f * sin(6 * u) * cos(4 * v);
//     float x = R * cos(u) * sin(v);
//     float y = R * sin(u) * sin(v);
//     float z = R * cos(v);
//     return Vec3(x, y, z);
// }

// #include <cmath>

// Vec3 cross(const Vec3 &a, const Vec3 &b)
// {
//     return {
//         a.y * b.z - a.z * b.y,
//         a.z * b.x - a.x * b.z,
//         a.x * b.y - a.y * b.x};
// }

// Vec3 normalize(const Vec3 &v)
// {
//     float len = sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
//     return {v.x / len, v.y / len, v.z / len};
// }

// void makeBasis(
//     const Vec3 &axis,
//     Vec3 &u,
//     Vec3 &v)
// {
//     Vec3 ref = fabs(axis.y) < 0.99f
//                    ? Vec3{0, 1, 0}
//                    : Vec3{1, 0, 0};

//     u = normalize(cross(axis, ref));
//     v = cross(axis, u);
// }

// void generateCone(
//     const Vec3 &apex,
//     const Vec3 &direction,
//     float height,
//     float radius,
//     int segments)
// {
//     Vec3 axis = normalize(direction);

//     Vec3 u, v;
//     makeBasis(axis, u, v);

//     Vec3 baseCenter = {
//         apex.x + axis.x * height,
//         apex.y + axis.y * height,
//         apex.z + axis.z * height};

//     float step = 2.0f * M_PI / segments;

//     for (int i = 0; i < segments; ++i)
//     {
//         float t0 = i * step;
//         float t1 = (i + 1) * step;

//         Vec3 p0 = {
//             baseCenter.x + u.x * (radius * cos(t0)) + v.x * (radius * sin(t0)),
//             baseCenter.y + u.y * (radius * cos(t0)) + v.y * (radius * sin(t0)),
//             baseCenter.z + u.z * (radius * cos(t0)) + v.z * (radius * sin(t0))};

//         Vec3 p1 = {
//             baseCenter.x + u.x * (radius * cos(t1)) + v.x * (radius * sin(t1)),
//             baseCenter.y + u.y * (radius * cos(t1)) + v.y * (radius * sin(t1)),
//             baseCenter.z + u.z * (radius * cos(t1)) + v.z * (radius * sin(t1))};

//         // drawTriangle(apex, p0, p1);
//     }
// }

// Vec2 cardroid(float theta, Var v)
// {
//     float t = 10.0f * (1.0f - 2.0f * cos(theta));
//     float x = t * cos(theta);
//     float y = t * sin(theta);

//     return Vec2(x, y);
// }
// glm::vec2 cardroid2(float theta, Var v)
// {
//     float t = 10.0f * (1.0f + 2.0f * cos(theta));
//     float x = t * cos(theta);
//     float y = t * sin(theta);

//     return glm::vec2(x, y);
// }
// Vec2 cardroid3(float theta, Var v)
// {
//     float t = 10.0f * (1.0f + 2.0f * sin(theta));
//     float x = t * cos(theta);
//     float y = t * sin(theta);

//     return Vec2(x, y);
// }
// Vec2 cardroid4(float theta, Var v)
// {
//     float t = 10.0f * (1.0f - 2.0f * sin(theta));
//     float x = t * cos(theta);
//     float y = t * sin(theta);

//     return Vec2(x, y);
// }

// #include <cmath>
// #include <numbers> // Use for M_PI_2, M_PI, etc. if compiling with C++20 standard

// // Assuming Vec3 is a structure like:
// // struct Vec3 { float x, y, z; };
// // Assuming Var is your custom variable struct (unused here but kept for signature consistency)

// // The Butterfly Curve is typically drawn from theta = 0 to theta = 12*PI for a complete shape.
// // The angle input 'theta' must be in radians.

// Vec2 polarButterflyCurve(float theta, Var v)
// {
//     float term1 = exp(sin(theta));

//     // 2. Calculate the Primary Lobes Term
//     float term2 = 2.0f * cos(4.0f * theta);

//     float argument = (2.0f * theta - M_PI) / 24.0f;

//     // Calculate sin(argument) raised to the 5th power
//     float term3 = pow(sin(argument), 5.0f);

//     // 4. Calculate the Final Radius (r)
//     float r = term1 - term2 + term3;

//     // --- Convert Polar Coordinates (r, theta) to Cartesian (x, y, z) ---

//     // For a 2D polar curve, we place it on the XY plane:
//     float x = 10.0f * r * cos(theta);
//     float y = 10.0f * r * sin(theta);
//     float z = 0.0f;

//     // Return the 3D position vector
//     return Vec2(x, y);
// }

// #include <functional>
// #include <glm/glm.hpp>
// #include <cmath>

// // 1️⃣ Classic moving helix
// auto WaveHelix = [](float t, float dt) -> glm::vec3
// {
//     float x = t * 50.0f;
//     float speed = 2.0f;
//     float phase = x - dt * speed;
//     float y = 5.0f * sin(phase);
//     float z = 5.0f * cos(phase);
//     return glm::vec3(x, y, z);
// };

// // 2️⃣ Rising pulsating wave
// auto RisingWave = [](float t, float dt) -> glm::vec3
// {
//     float x = t * 50.0f;
//     float speed = 1.5f;
//     float phase = x - dt * speed;
//     float amplitude = 2.0f + 3.0f * sin(dt * 0.5f);
//     float y = amplitude * sin(phase);
//     float z = amplitude * cos(phase);
//     return glm::vec3(x, y, z);
// };

// // 3️⃣ Twisting helix
// auto TwistingHelix = [](float t, float dt) -> glm::vec3
// {
//     float x = t * 50.0f;
//     float speed = 2.0f;
//     float phase = x - dt * speed;
//     float radius = 5.0f;
//     float angle = phase + dt; // twist over time
//     float y = radius * sin(angle);
//     float z = radius * cos(angle);
//     return glm::vec3(x, y, z);
// };

// // 4️⃣ Ripple wave
// auto RippleWave = [](float t, float dt) -> glm::vec3
// {
//     float x = t * 50.0f;
//     float speed = 3.0f;
//     float phase = x - dt * speed;
//     float ripple = 1.0f + 0.5f * sin(x * 0.5f - dt * 2.0f);
//     float y = ripple * sin(phase);
//     float z = ripple * cos(phase);
//     return glm::vec3(x, y, z);
// };

// // 5️⃣ Double oscillating wave
// auto DoubleWave = [](float t, float dt) -> glm::vec3
// {
//     float x = t * 50.0f;
//     float speed1 = 2.0f;
//     float speed2 = 1.0f;
//     float y = 5.0f * sin(x - dt * speed1) + 2.0f * sin(x + dt * speed2);
//     float z = 5.0f * cos(x - dt * speed1) + 2.0f * cos(x + dt * speed2);
//     return glm::vec3(x, y, z);
// };

// // 6️⃣ Flower-like Lissajous curve
// auto LissajousCurve = [](float t, float dt) -> glm::vec3
// {
//     float a = 10.0f, b = 5.0f;
//     float freqX = 2.0f, freqY = 3.0f;
//     float y = t * 30.0f;
//     float x = a * sin(freqX * t + dt);
//     float z = b * cos(freqY * t + dt);
//     return glm::vec3(x, y, z);
// };

// // 7️⃣ Floating sine ribbon
// auto FloatingRibbon = [](float t, float dt) -> glm::vec3
// {
//     float x = t * 50.0f;
//     float speed = 1.2f;
//     float phase = x - dt * speed;
//     float y = 5.0f * sin(phase) + 2.0f * sin(dt * 0.7f);
//     float z = 3.0f * cos(phase) + 1.5f * sin(dt * 1.3f);
//     return glm::vec3(x, y, z);
// };

// #include <glm/glm.hpp>
// #include <cmath>

// // 8️⃣ Spiral rising motion
// auto SpiralRise = [](float t, float dt) -> glm::vec3
// {
//     float x = t * 50.0f;
//     float radius = 3.0f + 2.0f * sin(dt * 0.5f);
//     float angle = x * 0.3f + dt;
//     float y = radius * sin(angle) + 0.5f * sin(dt);
//     float z = radius * cos(angle) + 0.5f * cos(dt);
//     return glm::vec3(x, y, z);
// };

// // 9️⃣ Waving sine tube
// auto WavingTube = [](float t, float dt) -> glm::vec3
// {
//     float x = t * 50.0f;
//     float baseRadius = 4.0f;
//     float y = (baseRadius + sin(dt * 2.0f)) * sin(x - dt * 1.5f);
//     float z = (baseRadius + cos(dt * 2.5f)) * cos(x - dt * 1.5f);
//     return glm::vec3(x, y, z);
// };

// // 🔟 Twisting ribbon with oscillation
// auto TwistingRibbon = [](float t, float dt) -> glm::vec3
// {
//     float x = t * 50.0f;
//     float y = 5.0f * sin(x - dt * 1.8f) + 2.0f * sin(dt);
//     float z = 3.0f * cos(x - dt * 2.0f) + 1.0f * sin(dt * 1.5f);
//     return glm::vec3(x, y, z);
// };

// // 11️⃣ Flower petal spiral
// auto PetalSpiral = [](float t, float dt) -> glm::vec3
// {
//     float speed = 1.0;
//     float x = t * 70.0f;
//     float angle = t * 90.0f + dt * speed;
//     float radius = 5.0f * sin(2.0f * t + dt * speed);
//     float y = radius * sin(angle);
//     float z = radius * cos(angle);
//     return glm::vec3(x, y, 0);
// };

// // Growing sine wave from origin
// auto GrowingSine = [](float t, float dt) -> glm::vec3
// {
//     // Controls
//     const float growSpeed = 2.0f; // units per second along X
//     const float amplitude = 5.0f;
//     const float frequency = 1.0f; // cycles per unit

//     // How much of the curve exists at this time
//     float maxX = growSpeed * dt;

//     // If this part of curve is not yet revealed, clamp to origin
//     if (t > maxX)
//         return glm::vec3(maxX, 0.0f, 0.0f);

//     float x = t;
//     float y = amplitude * sin(2.0f * M_PI * frequency * t);

//     return glm::vec3(x, y, 0.0f);
// };

// // 12️⃣ Rolling sine wave
// auto RollingWave = [](float t, float dt) -> glm::vec3
// {
//     float x = t * 50.0f;
//     float y = 5.0f * sin(x - dt * 1.5f) + 0.5f * cos(x * 0.3f + dt);
//     float z = 5.0f * cos(x - dt * 1.5f) + 0.5f * sin(x * 0.2f + dt);
//     return glm::vec3(x, y, z);
// };

// // 13️⃣ Corkscrew motion
// auto Corkscrew = [](float t, float dt) -> glm::vec3
// {
//     float z = t * 30;
//     float radius = 4.0f + 2.0f * sin(dt * 0.7f);
//     float angle = z * 0.5f + dt * 2.0f;
//     float x = z * 0.5 * cos(angle);
//     // float y = radius * cos(angle);
//     float y = z * 0.5 * sin(angle);
//     // float z = radius * sin(angle);
//     return glm::vec3(x, y, z);
// };

// // 14️⃣ Ocean wave ripple
// auto OceanRipple = [](float t, float dt) -> glm::vec3
// {
//     float x = t * 50.0f;
//     float y = 3.0f * sin(x * 0.5f - dt) + 2.0f * sin(dt * 0.8f);
//     float z = 3.0f * cos(x * 0.4f - dt) + 2.0f * cos(dt * 0.6f);
//     return glm::vec3(x, y, z);
// };

// // 15️⃣ Infinity loop wave
// auto InfinityWave = [](float t, float dt) -> glm::vec3
// {
//     float x = t * 50.0f;
//     float y = 5.0f * sin(x - dt * 1.2f) + 5.0f * sin(x * 0.5f - dt * 0.8f);
//     float z = 5.0f * cos(x - dt * 1.2f) - 5.0f * cos(x * 0.5f - dt * 0.8f);
//     return glm::vec3(x, y, z);
// };

// void mainSecondary(Graph *graph);

// int main()
// {
//     Graph *graph = Graph::getInstance(25);

//     mainSecondary(graph);
//     Var v;
//     // GraphObject *g = new GraphObject();
//     // g->generatePoints([](float t, Var v)
//     //                   { float x = t * 50;
//     //     float y = 5 * sin(x);
//     // return glm::vec3(x, y, 0); }, v);

//     // TestObject *g1 = new TestObject();
//     // g1->generatePoints([](float t, Var v)
//     //                   { float x = t * 50;
//     //     float y = 10 * cos(x);
//     // return glm::vec2(x, y); }, v);
//     // GraphObject *g2 = new GraphObject();
//     // g2->generatePoints(cardroid2,v);
//     // g->add(g1);
//     // g->add(g2);
//     // g->synchronizeSubObjects(-1.0f, 5.0f);
//     // graph->play(g);

//     // g1->setPoints(glm::vec3(-40, -40, 0));
//     // g1->setPoints(glm::vec3(-20, 0, 0));
//     // g1->setPoints(glm::vec3(0, 20, 0));
//     // g1->setPoints(glm::vec3(20, 0, 0));
//     // g1->setPoints(glm::vec3(40, 40, 0));

//     TestObject *t1 = new TestObject();
//     t1->setLineWidth(2.0f);
//     // TestObject *t2 = new TestObject();
//     // TestObject *t3 = new TestObject();
//     // TestObject *t4 = new TestObject();
//     // TestObject *t5 = new TestObject();
//     // TestObject *t6 = new TestObject();
//     // TestObject *t7 = new TestObject();
//     // TestObject *t8 = new TestObject();
//     t1->generatePoints([](float t, Var v)
//                        { float x = t * 40.0f;
//         float angle = t * 10.0f;
//         float radius = 10.0f * sin(2.0f * t);
//         float y = radius * sin(angle);
//         float z = radius * cos(angle);
//         return glm::vec3(x, y, 0); }, v);
//     // t2->generatePoints([](float t, Var v)
//     //                    { float x = 5 * t * M_PI * cos(t);
//     //                 float y = 5 * sin(x);
//     //                 float z = 5 * cos(x);
//     //                 return glm::vec3(x, y, 0); }, v);
//     // t3->generatePoints([](float t, Var v)
//     //                    { float x = t * 10.0f;
//     //     float y = x * x ;
//     //     return glm::vec3(x, y, 0); }, v);
//     // t4->generatePoints([](float t, Var v)
//     //                    { float x = t * 50.0f;
//     //                 float y = 5 * sin(x);
//     //                 float z = 5 * cos(x);
//     //                 return glm::vec3(x, 0, 0); }, v);
//     // t5->generatePoints([](float t, Var v)
//     //                    { float x = t * 50.0f;
//     //                 float y = 5 * sin(x);
//     //                 float z = 5 * cos(x);
//     //                 return glm::vec3(x, 0, 0); }, v);
//     // t1->setUpdater([](float t, float dt)
//     //                {
//     //     float x = t * 50.0f;
//     //     float speed = 2.0f; // units per second
//     //     float phase = x - dt * speed;
//     //     float y = 5 * sin(0.2 * ((int)(x) % 20) * dt - dt);
//     //     float z = 5 * cos(0.2 * ((int)(x) % 20) * dt - dt) ;
//     //     return glm::vec3(x, y, z); });

//     // t1->setUpdater(WaveHelix);      // simple helix
//     // t1->setUpdater(RisingWave);     // pulsating wave
//     // t1->setUpdater(TwistingHelix);  // twisting helix
//     // t1->setUpdater(RippleWave);     // ripple motion
//     // t1->setUpdater(DoubleWave);     // overlapping waves
//     // t2->setUpdater(LissajousCurve); // flower-like Lissajous
//     // t1->setUpdater(FloatingRibbon); // floating ribbon motion
//     // t1->setUpdater(SpiralRise);
//     // t1->setUpdater(WavingTube);
//     // t1->setUpdater(TwistingRibbon);
//     // t1->setUpdater(PetalSpiral);
//     // t1->setUpdater(PetalSpiral);
//     // t2->setUpdater(RollingWave);
//     // t3->setUpdater(Corkscrew);
//     // t4->setUpdater(OceanRipple);
//     // t5->setUpdater(InfinityWave);

//     // t1->setShader("./shaders/cubic_bezier_shader_stroke/vertex.vs", "./shaders/cubic_bezier_shader_stroke/geometry.gs", "./shaders/cubic_bezier_shader_stroke/fragment.fs");

//     // t1->setDuration(2.0f);
//     // t1->setColor({GraphColor(1, 1, 0), GraphColor(0, 1, 0), GraphColor(1, 0, 1)});
//     t1->setColor({GraphColor(0.67, 0.67, .67), GraphColor(0.67, 0.67, 0.67), GraphColor(0, 0.75, 0.67)});
//     // t2->setColor({GraphColor(0.67, 0.67, .67), GraphColor(0.67, 0.67, 0.67), GraphColor(0, 0.75, 0.67)});
//     // t3->setColor({GraphColor(0.67, 0.67, .67), GraphColor(0.67, 0.67, 0.67), GraphColor(0, 0.75, 0.67)});
//     GraphMathObject *td = (GraphMathObject *)new ThreeDAxes();
//     GridBox *gb = new GridBox(14, 20);
//     gb->setColor({GraphColor(0.2, 0.2, 0.2)});
//     gb->setDimension(-80, 40, 160, 80);
//     // gb->moveTo(Position::BOTTOM_LEFT);
//     graph->play(td);
//     // graph->play((GraphMathObject*)g1);
//     // graph->play(t1);
//     // graph->play((GraphMathObject*)t2);
//     // graph->play(t3);
//     // graph->play((GraphMathObject*)t4);
//     // graph->play((GraphMathObject*)t5);
//     // t2->setDuration(5.0f);
//     // g1->setDuration(5.0f);
//     // Dot *d1 = new Dot({0, 0}, 1.0f);
//     // graph->play(d1);
//     // graph->play(new ShowCreation(t1, 0, 3));
//     // graph->play(new ShowCreation(t2, 0, 3));
//     // graph->play(new ShowFillCreation(t3, 0, 10));
//     // graph->play(new Translate(t1, glm::vec3(-50, 30, 0), 3, 2));
//     // graph->play(new Scale(t1, glm::vec3(0.2, 0.5, 0), 3, 2));
//     // graph->play(g1);

//     // p->Duration(10.0f);

//     // graph->play(p);
//     // Line *l1 = new Line(glm::vec3(-50, 30, 0), glm::vec3(50, 30, 0));
//     // graph->play(l1);
//     // graph->play(new ShowCreation(l1, 0, 3));
//     // graph->play(gb);
//     // gb->setLineWidth(1.0f);
//     graph->play(new ShowCreation(gb, 0, 5));
//     mainSecondary(graph);

//     graph->run();
// }

// float tt = 1;

// glm::vec3 updatePoints(float t, float dt)
// {

//     float time = dt * 10 * M_PI * 2;
//     if (t == -1)
//     {
//         float x = 10 * cos(time) - 65;
//         float y = 10 * sin(time);
//         return glm::vec3(x, y, 0);
//     }
//     else
//     {
//         float x = -16 * M_PI + (40 * M_PI) * dt;
//         float y = 10 * sin(x / 2);
//         return glm::vec3(x, y, 0);
//     }
// }

// void mainSecondary(Graph *graph)
// {
//     Var v;
//     Position p = Position::TOP_LEFT;
//     float time = 3;
//     Circle *c1 = new Circle(10, 0, 0);
//     // c1->resolution = 32;
//     // c1->setLineWidth(2.0f);
//     // // c1->moveTo(glm::vec3(10, 10, 0));
//     // c1->setColor(GraphColor(1, 1, 0));
//     graph->play(c1);
//     Dot *d = new Dot(-50.0, 0.0, 1);
//     graph->play(d);
//     graph->play(new Translate(c1, glm::vec3(-50, 30, 0), 0, 10, p));
//     // graph->play(new Scale(c1, glm::vec3(2, 2, 0), 1, time));
//     // // graph->play(new Scale(c1, glm::vec3(0.5f, 0.5f, 0), 1 + time * 3, 2));
//     // graph->play(new Translate(c1, glm::vec3(-70, 30, 0), 1 + time, time, p));
//     // graph->play(new Translate(c1, glm::vec3(-30, 30, 0), 1 + time * 2, time, p));
//     // graph->play(new Translate(c1, glm::vec3(-50, 10, 0), 1 + time * 3, time, p));
//     // graph->play(new Translate(c1, glm::vec3(-50, 50, 0), 1 + time * 4, time, p));
//     // graph->play(new Translate(c1, glm::vec3(0, 0, 0), 1 + time * 5, time, p));
//     // graph->play(new Translate(c1, glm::vec3(-15, 0, 0), 1 + time * 6, time, p));
//     c1->fillOpacity = 0;

//     // Circle *c2 = new Circle(10, 0, 0);
//     // c2->setLineWidth(2.0f);
//     // c2->resolution = 32;

//     // c2->setColor(GraphColor(1, 1, 0));
//     // graph->play(c2);
//     // graph->play(new Translate(c2, glm::vec3(30, 30, 0), 1, time, p));
//     // graph->play(new Translate(c2, glm::vec3(10, 30, 0), 1 + time, time, p));
//     // graph->play(new Translate(c2, glm::vec3(50, 30, 0), 1 + time * 2, time, p));
//     // graph->play(new Translate(c2, glm::vec3(30, 10, 0), 1 + time * 3, time, p));
//     // graph->play(new Translate(c2, glm::vec3(30, 50, 0), 1 + time * 4, time, p));
//     // graph->play(new Translate(c2, glm::vec3(-20, 0, 0), 1 + time * 5, time, p));
//     // graph->play(new Translate(c2, glm::vec3(-5, 0, 0), 1 + time * 6, time, p));
//     // c2->fillOpacity = 0;
//     // c1->scaleTo(1, 2);
//     // graph->play(new ShowCreation(c1, 0, 5));
//     // c1->moveTo(glm::vec3(10, 10, 0));
//     // c1->moveTo(glm::vec3(-50, 20, 0));
//     // c1->moveTo(glm::vec3(5 - c1->x, 0, 0));
//     // graph->play(new Scale(c1, glm::vec3(0.5, 0.5, 0), 5, 1));

//     Line *l1 = new Line(glm::vec3(-30, -30, 0), glm::vec3(30, 30, 0));
//     Line *l2 = new Line(glm::vec3(10, 30, 0), glm::vec3(30, 30, 0));
//     Line *l3 = new Line(glm::vec3(10, 5, 0), glm::vec3(30, 30, 0));
//     l1->setColor({GraphColor(1, 0, 0)});
//     l2->setColor({GraphColor(0, 1, 0)});
//     std::cout << "l2 resolution: " << l2->resolution << std::endl;
//     l1->resolution = 1;
//     l1->setLineWidth(2.0f);
//     graph->play(l1);
//     graph->play(l2);
//     graph->play(l3);
//     graph->play(new Translate(l1, glm::vec3(10, 30, 0), 0, 5, Position::BOTTOM_LEFT));
//     graph->play(new Rotation(l1, glm::vec3(0, 0, 45), Position::BOTTOM_LEFT, 3, 5));
//     graph->play(new Rotation(l3, glm::vec3(0, 0, 45), Position::BOTTOM_LEFT, 0, 5));
//     // graph->play(new Scale(l1, glm::vec3(5, 0, 0), 0, 2));
//     // TestObject *t1 = new TestObject();
//     // t1->range = {-1, 1};
//     // t1->setLineWidth(2.0f);
//     // t1->generatePoints([](float t, Var v)
//     //                    {
//     //                     float x = t * 20 * M_PI + 4 * M_PI;
//     //                     float y = 10 * sin(x / 2);
//     //                     return glm::vec3(x, y, 0);
//     //                     }, v);
//     // graph->play(t1);
//     // t1->fillOpacity = 0;
//     // graph->play(new ShowCreation(t1, 0, tt * 12));

//     // // graph->play(c1);

//     // Line *lTest = new Line();
//     // // graph->play(lTest);

//     // // graph->play(new ShowCreation(lTest, 0, 3));

//     // TestObject* tTest = new TestObject();
//     // tTest->x = -50;
//     // tTest->y = 30;
//     // tTest->width = 20;
//     // tTest->height = 20;
//     // tTest->setLineWidth(2.0f);
//     // tTest->setPoints({-50, 30, 0});
//     // tTest->setPoints({-50, 10, 0});
//     // tTest->setPoints({-30, 10, 0});
//     // tTest->setPoints({-30, 30, 0});
//     // tTest->setPoints({-50, 30, 0});
//     // tTest->fillOpacity = 0;
//     // graph->play(tTest);

//     // TestObject *tTest2 = new TestObject();
//     // tTest2->x = 30;
//     // tTest2->y = 30;
//     // tTest2->setPoints({30, 30, 0});
//     // tTest2->setPoints({30, 10, 0});
//     // tTest2->setPoints({50, 10, 0});
//     // tTest2->setPoints({50, 30, 0});
//     // tTest2->setPoints({30, 30, 0});
//     // tTest2->fillOpacity = 0;
//     // graph->play(tTest2);
//     // tTest2->setColor({GraphColor(1, 0, 1)});
//     // tTest->setColor({GraphColor(0, 1, 1)});
//     // graph->play(new Translate(tTest, glm::vec3(30, -25, 0), 1 + time * 5, time, p));
//     // graph->play(new Translate(tTest2, glm::vec3(-50, -25, 0), 1 + time * 5, time, p));
//     // graph->play(new Translate(tTest2, glm::vec3(30, -25, 0), 1 + time * 6, time, p));
//     // graph->play(new Translate(tTest, glm::vec3(-50, -25, 0), 1 + time * 6, time, p));
//     // graph->play(new ShowCreation(tTest, 1 + 8 * time, 3));
//     // graph->play(new ShowCreation(tTest, 0, 3));
//     // graph->play(new UnCreate(tTest, 3, 3));
// }

// float para(float x)
// {
//     return x * x;
// }

// // function to draw a parabola
// // glm::vec3 para2(float t, Var v)
// // {
// //     float x = t * 6;
// //     float y = para(x);

// //     return glm::vec3(x, y, 0);
// // }

// glm::vec3 lineUpdater(float t, float dt)
// {
//     // 1. Current position on parabola (x0, y0)
//     // float speed = 2.0f; // x moves 2 units per second
//     float x0 = -6 + 12 * dt * 12;
//     float y0 = x0 * x0;
//     glm::vec3 contactPoint(x0, y0, 0);

//     // 2. Calculate the Tangent Vector
//     // The derivative of (x, x^2) is (1, 2x)
//     glm::vec3 tangentDir(1.0f, 2.0f * x0, 0.0f);

//     // 3. Normalize the vector so the line length is constant
//     // This prevents the line from "stretching" as it gets steeper
//     glm::vec3 unitTangent = glm::normalize(tangentDir);

//     // 4. Calculate the vertex position
//     // t comes from line->range {-1, 1}.
//     // This places vertices at exactly 1 unit away from the contact point
//     // along the direction of the tangent.
//     float lineHalfLength = 20.0f;
//     glm::vec3 vertexPos = contactPoint + (unitTangent * (t * lineHalfLength));

//     return vertexPos;
// }

// glm::vec3 sinGraphLineUpdater(float t, float dt)
// {
//     // 1. Calculate the current X position of the sine wave 'tip'
//     // The sine wave grows from -12*PI to 18*PI (Total width = 30*PI) over 10 seconds.
//     float startX = -1.0f * 12.0f * M_PI;
//     float totalWidth = 30.0f * M_PI;
//     float currentX = startX + (totalWidth * dt);

//     // 2. Calculate the shared Y value (This makes the line horizontal)
//     // Both points must have this Y.
//     float sharedY = 15.0f * sin(currentX / 3.0f);

//     // End 1 (t = 0): The point on the Circle
//     if (t == 0)
//     {
//         // We use the same angle (currentX / 3) to find the X of the circle point.
//         // x = center_x + radius * cos(angle)
//         float circleX = (-25.0f * M_PI) + 15.0f * cos(currentX / 3.0f);
//         return glm::vec3(circleX, sharedY, 0);
//     }
//     // End 2 (t = 1): The point on the Sine Wave
//     else
//     {
//         return glm::vec3(currentX, sharedY, 0);
//     }
// }

// void animation1(Graph *graph)
// {

//     Circle *c1 = new Circle(15, -25 * M_PI, 0);
//     c1->setLineWidth(2.0f);
//     c1->fillOpacity = 0;

//     TestObject *sinGraph = new TestObject();
//     Var v;
//     sinGraph->range = {-1, 1.5f};
//     sinGraph->fillOpacity = 0;
//     sinGraph->setLineWidth(2.0f);
//     sinGraph->generatePoints([](float t, Var v)
//                              {
//         float x = t * 12 * M_PI;
//         float y = 15 * sin(x / 3);
//         return glm::vec3(x, y, 0); },
//                              v);

//     graph->play(sinGraph);
//     int time = 20;
//     graph->play(new ShowCreation(sinGraph, 0, time));

//     sinGraph->interpolate(sinGraph->resolution * 2);

//     Line *line = new Line();
//     line->range = {0, 1};
//     line->resolution = 1;
//     line->setUpdater(sinGraphLineUpdater, 0, time);
//     graph->play(line);

//     graph->play(c1);
//     // graph->play(new ShowCreation(c1, 0, 5));
// }

// void animation2(Graph *graph)
// {
//     ThreeDAxes *td = new ThreeDAxes();
//     graph->play(td);

//     TestObject *para = new TestObject();
//     Var v;
//     // para->generatePoints(para2, v);
//     // graph->play(para2);
//     para->fillOpacity = 0;

//     Line *line = new Line();
//     line->range = {-1, 1};
//     line->resolution = 1;
//     line->setUpdater(lineUpdater, 0, 3);

//     graph->play(line);
// }

// float x_range = 24.0f * M_PI;
// float y_amplitude = 10.0f;

// glm::vec3 sinGraph(float t, Var v)
// {
//     float x = x_range * t;
//     float y = y_amplitude * sin(x / 2);
//     return glm::vec3(x, y, 0);
// }

// glm::vec3 cosGraph(float t, Var v)
// {
//     float x = 21.0f * M_PI * t;
//     float y = y_amplitude * cos(x / 2);
//     return glm::vec3(x, y, 0);
// }

// glm::vec3 para3(float t, Var v)
// {
//     float x = 5.0f * 4.0f * t;
//     float y = (3.5f * t) * (3.5f * t) * (3.5f * t);
//     return glm::vec3(x, y, 0);
// }

// glm::vec3 para2(float t, Var v)
// {
//     float x = 5.0f * 4.0f * t;
//     float y = (6.0f * t) * (6.0f * t);
//     return glm::vec3(x, y, 0);
// }

// void animation3(Graph *graph)
// {
//     Var v;

//     ThreeDAxes *td = new ThreeDAxes();
//     graph->play(td);

//     TestObject *sine = new TestObject();
//     sine->range = {-1, 1};
//     sine->fillOpacity = 1;
//     // sine->resolution = 400;
//     sine->generatePoints(sinGraph, v);

//     TestObject *cose = new TestObject();
//     cose->range = {-1, 1};
//     cose->generatePoints(cosGraph, v);
//     cose->fillOpacity = 1;
//     // TestObject* par3 = new TestObject();
//     // par3->range = {-1, 1};
//     // cose->fillOpacity = 0;
//     // par3->generatePoints(para3, v);
//     // sine->moveTo(Position::BOTTOM_RIGHT);

//     graph->play(sine);
//     graph->play(cose);
//     cose->moveTo(Position::TOP);

//     graph->play(new Translate(sine, Position::LEFT, 0, 3));
//     graph->play(new Translate(sine, Position::TOP, 3, 3));
//     // graph->play(new Translate(sine, Position::BOTTOM, 6, 3));
//     // graph->play(new Translate(sine, Position::RIGHT, 9, 3));
//     // graph->play(new Translate(sine, Position::TOP_LEFT, 12, 3));
//     // graph->play(new Translate(sine, Position::TOP_RIGHT, 15, 3));
//     // graph->play(new Translate(sine, Position::BOTTOM_LEFT, 18, 3));
//     // graph->play(new Translate(sine, Position::BOTTOM_RIGHT, 21, 3));
//     // graph->play(new Translate(sine, Position::BOTTOM, 24, 3));

//     graph->play(new Transition(sine, cose, 3, 3));
//     // graph->play(new Translate(sine, Position::TOP, 27, 5));

//     // graph->play(new Transition(sine, cose, 3, 3));

//     // graph->play(sine);
//     // graph->play(new ShowCreation(sine, 0, 2));

//     Line *line = new Line(glm::vec3(-24, -24, 0), glm::vec3(24, 24, 0));
//     line->setLineWidth(3.0f);
//     // graph->play(line);
//     // graph->play(par2);
//     // graph->play(new ShowCreation(par2, 0, 3));

//     // graph->play(new Transition(par2, par3, 4, 3));
//     // graph->play(new Transition(line, par3, 8, 3));
//     // graph->play(new Rotation(line, glm::vec3(0, 180.0f, 0), Position::NONE, 12, 3));
//     // graph->play(new Transition(line, par2, 15, 3));
//     // graph->play(new Rotation(line, glm::vec3(0, 180.0f, 0), Position::NONE, 18, 3));
//     // graph->play(new Transition(line, par3, 21, 3));
// }

// glm::vec3 butterflyCurve(float t, Var v)
// {
//     float a = std::exp(std::cos(t)) - 2.0f * std::cos(4.0f * t) - std::pow(std::sin(t / 12.0f), 5.0f);

//     float z = v[0] * std::sin(t) * a;
//     float y = v[0] * std::cos(t) * a;
//     // float z = v[0] * std::cos(t) * std::sin(t) * a;
//     float x = 0;

//     return glm::vec3(x, y, z);
// }

// // int main()
// // {
// //     Graph *graph = Graph::getInstance();

// //     ThreeDAxes *td = new ThreeDAxes();

// //     Line *line = new Line(glm::vec3(-50, -30, 0), glm::vec3(-50, 30, 0));
// //     Line *line2 = new Line(glm::vec3(10, 0.01, -30), glm::vec3(10, 0, 30));
// //     Circle *circle1 = new Circle(30, 0, 0);
// //     line->setColor({GraphColor(1, 0, 1), GraphColor(0, 1, 1), GraphColor(1, 1, 0), GraphColor(1, 0, 1)});
// //     line2->setColor({GraphColor(1, 0, 1), GraphColor(0, 1, 1), GraphColor(1, 1, 0), GraphColor(1, 0, 1)});
// //     circle1->setColor({GraphColor(1, 0, 1), GraphColor(0, 1, 1), GraphColor(1, 1, 0), GraphColor(1, 0, 1)});
// //     graph->play(line);
// //     graph->play(line2);

// //     // graph->play(new ShowCreation(line, 3, 5));

// //     TestObject *t = new TestObject();
// //     Var v = Var();
// //     v.addVar(10.0f);
// //     t->range = {-1.0f * M_PI, 1.0f * M_PI};
// //     t->setLineWidth(3.0f);
// //     t->resolution = 1000;
// //     t->fillOpacity = 0;
// //     t->setColor({GraphColor(1, 0, 1), GraphColor(0, 1, 1), GraphColor(1, 1, 0), GraphColor(1, 0, 1)});
// //     t->generatePoints(butterflyCurve, v);
// //     // graph->play(new ShowCreation(t, 0, 5));
// //     // graph->play(new Rotation(t, glm::vec3(0, 0, 45.0f), glm::vec3(0, 0, 0), 0, 5));
// //     // graph->play(new Translate(t, Position::NONE, 0, 5, Position::TOP_LEFT));
// //     // graph->play(new Rotation(t, glm::vec3(0, 0, -90.0f), glm::vec3(0, 0, 0), 6, 5));
// //     // graph->play(new Rotation(t, glm::vec3(0, 0, 90.0f), glm::vec3(0, 0, 0), 12, 5));
// //     // graph->play(new Transition(line, circle1, 5, 5));
// //     // graph->play(new Transition(line, t,11, 5));
// //     // graph->play(new Transition(line, line2, 17, 5));
// //     graph->play(t);
// //     graph->play(td);
// //     // animation1(graph);
// //     graph->run();
// //     // draw the parabola
// // }
