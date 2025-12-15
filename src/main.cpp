#include "../include/GraphEngine/Graphs/Graph.hpp"

#include "../include/GraphEngine/Graphs/Coordinates/CylindricalCoordinate.hpp"
#include "../include/GraphEngine/Graphs/Coordinates/CartesianCoordinate.hpp"
#include "../include/GraphEngine/Graphs/ThreeDAxes.hpp"
// Paraboloid function
Vec3 hollowParaboloid(float r, float phi, Var v)
{
    float y = r * r * r; // height along Y-axis
    float x = r * cos(phi);
    float z = r * sin(phi);

    return Vec3(x, y, z);
}

// Define scaling constants for the noise input
const float NOISE_SCALE = 0.05f;
const float HEIGHT_FACTOR = 10.0f; // Multiplier to make the hills taller

#include <cmath>
#include <iostream>

// --- Perlin Noise Constants and Utility Functions ---

// Array of pre-computed random gradient vectors at the 16 unique corners (4x4 grid)
// These define the slope/direction of the "randomness" at each integer corner.
// Only 12 are needed for 2D, but 16 simplifies indexing.
const Vec2 GRADIENTS[] = {
    {1, 1}, {-1, 1}, {1, -1}, {-1, -1}, {1, 0}, {-1, 0}, {0, 1}, {0, -1}, {1, 1}, {-1, 1}, {1, -1}, {-1, -1}, {1, 0}, {-1, 0}, {0, 1}, {0, -1}};

// A permutation array to create "random" lookups from integer coordinates
// Size must be 256 or 512 (512 allows seamless wrapping).
int P[512];

// Smoothstep function (6t^5 - 15t^4 + 10t^3) for better interpolation curve
float fade(float t)
{
    return t * t * t * (t * (t * 6.0f - 15.0f) + 10.0f);
}

// Linear interpolation between a and b based on weight t
float lerp_n(float a, float b, float t)
{
    return (1.0f - t) * a + t * b;
}

// Dot product of a gradient vector and a distance vector
float dotGridGradient(int ix, int iy, float x, float y)
{
    // 1. Get the random gradient index based on the grid coordinates
    // We use a simplified hash based on P[] to get a stable random vector
    int index = P[P[ix & 255] + (iy & 255)] & 15; // & 15 limits the index to 0-15

    // 2. Calculate the distance vector from the grid point (ix, iy) to the sample point (x, y)
    float dx = x - (float)ix;
    float dy = y - (float)iy;

    // 3. Return the dot product
    return (dx * GRADIENTS[index].x + dy * GRADIENTS[index].y);
}

// --- Perlin Noise Function ---

float PerlinNoise2D(float x, float y)
{
    // 1. Determine grid cell coordinates
    int x0 = (int)std::floor(x);
    int y0 = (int)std::floor(y);
    int x1 = x0 + 1;
    int y1 = y0 + 1;

    // 2. Determine interpolation welerights (t)
    float tx = x - (float)x0; // Fractional part of X
    float ty = y - (float)y0; // Fractional part of Y

    // 3. Apply the smoothstep function to weights
    float u = fade(tx);
    float v = fade(ty);

    // 4. Calculate the four dot products between the gradient and distance vectors
    // These give the influence of each corner on the sample point.
    float n0 = dotGridGradient(x0, y0, x, y);
    float n1 = dotGridGradient(x1, y0, x, y);
    float n2 = dotGridGradient(x0, y1, x, y);
    float n3 = dotGridGradient(x1, y1, x, y);

    // 5. Interpolate (Lerp_n) the results
    // Interpolate along X:
    float ix1 = lerp_n(n0, n1, u);
    float ix2 = lerp_n(n2, n3, u);

    // Interpolate along Y:
    float finalValue = lerp_n(ix1, ix2, v);

    // The output is typically in the range [-1.0, 1.0]
    return finalValue;
}

// --- Initialization and Octave/FBM Function ---

// Fills the permutation array P[] with a predictable random sequence
void initializePerlin()
{
    // ... (P[i] = i setup is fine)

    // FIX: Use a safe, standard way to generate a repeatable random index.
    for (int i = 0; i < 256; i++)
    {
        int swap_index_range = 256 - i;

        // Use a simple, non-overflowing hash to get a 'random' value
        // The simple hash avoids the large integer multiplication and casting issues.
        float hash_val = std::sin(i * 12.9898f + 78.233f); // Add offset for better spread

        // Map hash_val ([-1, 1]) to [0, swap_index_range - 1]
        int target_offset = (int)((hash_val * 0.5f + 0.5f) * (float)swap_index_range);

        // Ensure index is within the valid range
        int target_index = target_offset + i;

        // --- SAFE SWAP ---
        int temp = P[i];
        P[i] = P[target_index];
        P[target_index] = temp;
    }

    // ... (P[i + 256] = P[i] duplication is fine)
}

// Fractal Brownian Motion (FBM) - combines multiple layers (octaves) of noise
// to create greater detail and smoothness.
extern float getPerlinHeight(float x, float z)
{
    static bool initialized = false;
    if (!initialized)
    {
        initializePerlin();
        initialized = true;
    }

    const int OCTAVES = 4;
    const float PERSISTENCE = 0.5f; // How much each octave contributes
    const float LACUNARITY = 2.0f;  // How much frequency increases per octave

    float total = 0.0f;
    float amplitude = 1.0f;
    float frequency = 1.0f;
    float maxValue = 0.0f; // Used for normalizing the final output

    for (int i = 0; i < OCTAVES; i++)
    {
        total += PerlinNoise2D(x * frequency, z * frequency) * amplitude;

        maxValue += amplitude;
        amplitude *= PERSISTENCE;
        frequency *= LACUNARITY;
    }

    // Normalize the output to be in the range of approximately [-1.0, 1.0]
    return total / maxValue;
}

Vec3 generateHillsPolar(float r, float phi, Var v)
{
    // 1. Convert polar coordinates (r, phi) to Cartesian coordinates (x, z)
    float x = r * cos(phi);
    float z = r * sin(phi);

    // 2. Calculate the height (y) based on the (x, z) position
    // Use the noise function for smooth, natural hill displacement.
    // The noise input (x * NOISE_SCALE) controls the size/frequency of the hills.
    float rawNoise = getPerlinHeight(x * NOISE_SCALE, z * NOISE_SCALE);

    // Scale the noise output to control the maximum hill height
    // Noise typically returns a value between -1.0 and 1.0.
    float y = rawNoise * HEIGHT_FACTOR;

    // 3. Return the 3D vector (x, y, z)
    return Vec3(x, y, z);
}

Vec3 hollowParaboloidX(float r, float phi, Var v)
{
    float x = r * r - 15.0f; // height along X-axis
    float y = r * cos(phi);
    float z = r * sin(phi);

    return Vec3(x, y, z);
}

Vec3 hollowCylinder(float h, float phi, Var v)
{
    float r = 5.0f;
    float x = r * cos(phi);
    float y = h;
    float z = r * sin(phi);
    return Vec3(x, y, z);
}

Vec3 torusLike(float r, float phi, Var v)
{
    float R = 1.0f; // major radius
    float t = phi;  // sweep

    float x = (R + r * cos(t)) * cos(t) - 5.0f;
    float y = r * sin(phi);
    float z = (R + r * cos(t)) * sin(t);

    return Vec3(x, y, z);
}

Vec3 torus(float u, float v, Var vars)
{
    float R = vars[0]; // major radius
    float r = vars[1]; // minor radius

    float x = (R + r * cos(u)) * cos(v);
    float y = r * sin(u);
    float z = (R + r * cos(u)) * sin(v);

    return Vec3(x, y, z);
}

Vec3 mobius(float u, float v, Var)
{
    // u in [0, 2π], v in [-1, 1]
    float R = 10.0f;
    float x = (R + v * cos(u / 2)) * cos(u);
    float y = (R + v * cos(u / 2)) * sin(u);
    float z = v * sin(u / 2);
    return Vec3(x, y, z);
}

Vec3 sphere(float theta, float phi, Var)
{
    // theta: 0 → π, phi: 0 → 2π
    float r = 3.0f;
    float x = r * sin(theta) * cos(phi);
    float y = r * sin(theta) * sin(phi);
    float z = r * cos(theta);
    return Vec3(x, y, z);
}

Vec3 saddle(float x, float y, Var)
{
    float z = x * x - y * y; // f(x, y) = x^2 - y^2
    return Vec3(x, y, z);
}

Vec3 ripple(float x, float y, Var v)
{
    float t = v[0]; // time for animation
    float z = 5.0f * sin(sqrt(x * x + y * y) - t);
    return Vec3(x, y, z);
}

Vec3 flower(float u, float v, Var)
{
    float R = 2.0f + 5.0f * sin(6 * u) * cos(4 * v);
    float x = R * cos(u) * sin(v);
    float y = R * sin(u) * sin(v);
    float z = R * cos(v);
    return Vec3(x, y, z);
}

#include <cmath>

Vec3 cross(const Vec3 &a, const Vec3 &b)
{
    return {
        a.y * b.z - a.z * b.y,
        a.z * b.x - a.x * b.z,
        a.x * b.y - a.y * b.x};
}

Vec3 normalize(const Vec3 &v)
{
    float len = sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
    return {v.x / len, v.y / len, v.z / len};
}

void makeBasis(
    const Vec3 &axis,
    Vec3 &u,
    Vec3 &v)
{
    Vec3 ref = fabs(axis.y) < 0.99f
                   ? Vec3{0, 1, 0}
                   : Vec3{1, 0, 0};

    u = normalize(cross(axis, ref));
    v = cross(axis, u);
}

void generateCone(
    const Vec3 &apex,
    const Vec3 &direction,
    float height,
    float radius,
    int segments)
{
    Vec3 axis = normalize(direction);

    Vec3 u, v;
    makeBasis(axis, u, v);

    Vec3 baseCenter = {
        apex.x + axis.x * height,
        apex.y + axis.y * height,
        apex.z + axis.z * height};

    float step = 2.0f * M_PI / segments;

    for (int i = 0; i < segments; ++i)
    {
        float t0 = i * step;
        float t1 = (i + 1) * step;

        Vec3 p0 = {
            baseCenter.x + u.x * (radius * cos(t0)) + v.x * (radius * sin(t0)),
            baseCenter.y + u.y * (radius * cos(t0)) + v.y * (radius * sin(t0)),
            baseCenter.z + u.z * (radius * cos(t0)) + v.z * (radius * sin(t0))};

        Vec3 p1 = {
            baseCenter.x + u.x * (radius * cos(t1)) + v.x * (radius * sin(t1)),
            baseCenter.y + u.y * (radius * cos(t1)) + v.y * (radius * sin(t1)),
            baseCenter.z + u.z * (radius * cos(t1)) + v.z * (radius * sin(t1))};

        // drawTriangle(apex, p0, p1);
    }
}

Vec2 cardroid(float theta, Var v)
{
    float t = 10.0f * (1.0f - 2.0f * cos(theta));
    float x = t * cos(theta);
    float y = t * sin(theta);

    return Vec2(x, y);
}
Vec2 cardroid2(float theta, Var v)
{
    float t = 10.0f * (1.0f + 2.0f * cos(theta));
    float x = t * cos(theta);
    float y = t * sin(theta);

    return Vec2(x, y);
}
Vec2 cardroid3(float theta, Var v)
{
    float t = 10.0f * (1.0f + 2.0f * sin(theta));
    float x = t * cos(theta);
    float y = t * sin(theta);

    return Vec2(x, y);
}
Vec2 cardroid4(float theta, Var v)
{
    float t = 10.0f * (1.0f - 2.0f * sin(theta));
    float x = t * cos(theta);
    float y = t * sin(theta);

    return Vec2(x, y);
}

#include <cmath>
#include <numbers> // Use for M_PI_2, M_PI, etc. if compiling with C++20 standard

// Assuming Vec3 is a structure like:
// struct Vec3 { float x, y, z; };
// Assuming Var is your custom variable struct (unused here but kept for signature consistency)

// The Butterfly Curve is typically drawn from theta = 0 to theta = 12*PI for a complete shape.
// The angle input 'theta' must be in radians.

Vec2 polarButterflyCurve(float theta, Var v)
{
    float term1 = exp(sin(theta));

    // 2. Calculate the Primary Lobes Term
    float term2 = 2.0f * cos(4.0f * theta);


    float argument = (2.0f * theta - M_PI) / 24.0f;

    // Calculate sin(argument) raised to the 5th power
    float term3 = pow(sin(argument), 5.0f);

    // 4. Calculate the Final Radius (r)
    float r = term1 - term2 + term3;

    // --- Convert Polar Coordinates (r, theta) to Cartesian (x, y, z) ---

    // For a 2D polar curve, we place it on the XY plane:
    float x = 10.0f * r * cos(theta);
    float y = 10.0f * r * sin(theta);
    float z = 0.0f;

    // Return the 3D position vector
    return Vec2(x, y);
}

int main(){
    Graph *graph = Graph::getInstance(25);

    CylindricalCoordinate* cor = new CylindricalCoordinate({-25.0f, 25.0f}, {0.0f, 2.0f * M_PI});
    cor->setResolutionR(100);
    cor->setResolutionPhi(100);
    Var v;
    v.addVar(10.0f);
    v.addVar(4.0f);

    CartesianCoordinate *cart = new CartesianCoordinate({-10, 10}, 100);

    // std::cout << "Resolution is: " << cart->getResolution() << std::endl;

    CartesianGraph *cGraph = cart->generateFunction([](float t, Var v)
                                                    { return Vec2(t, 10 * sin(t)); }, v);

    CartesianGraph *cGraph2 = cart->generateFunction([](float t, Var v)
                                                     { return Vec2(t, tan(t)); }, v);
    // cGraph->Duration(5.0f);

    // Surface* graph1 = cor->drawSurface(torus, v);
    // Surface* graph2 = cor->drawSurface(hollowParaboloid, v);
    // Surface* graph3 = cor->drawSurface(hollowCylinder, v);
    // Surface* graph4 = cor->drawSurface(ripple, v);
    // Surface* graph5 = cor->drawSurface(saddle, v);
    // Surface* graph6 = cor->drawSurface(flower, v);
    Surface* graph7 = cor->drawSurface(generateHillsPolar, v);

    // std::cout << "here we are" << std::endl;

    // graph->play(graph1);
    // graph->play(graph2);
    // graph->play(graph3);
    // graph->play(graph4);
    // graph->play(graph5);
    // graph->play(graph7);

    // graph->play(cGraph);
    // graph->play(cGraph2);

    ThreeDAxes *axes = new ThreeDAxes();

    CartesianCoordinate *rcord = new CartesianCoordinate({0, 10 * M_PI}, 1000);


    CartesianGraph *rgraph1 = rcord->generateFunction(cardroid, v);
    rgraph1->Duration(5.0f);
    CartesianGraph *rgraph2 = rcord->generateFunction(cardroid2, v);
    rgraph2->Duration(5.0f);
    CartesianGraph *rgraph3 = rcord->generateFunction(cardroid3, v);
    rgraph3->Duration(5.0f);
    CartesianGraph *rgraph4 = rcord->generateFunction(cardroid4, v);
    rgraph4->Duration(5.0f);

    CartesianGraph *rgraph5 = rcord->generateFunction(polarButterflyCurve, v);
    rgraph5->Duration(10.0f);

    // Arrow3D *arrow1 = cart->drawArrow(Vec3(0.0f, 0.0f, 0.0f), Vec3(10.0f, 10.0f, 0.0f));
    // graph->play(arrow1);
    graph->play(axes);
    // graph->play(rgraph1);
    // graph->play(rgraph2);
    // graph->play(rgraph3);
    // graph->play(rgraph4);
    graph->play(rgraph5);

    graph->run();
}