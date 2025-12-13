#include "../include/GraphEngine/Graphs/Graph.hpp"

#include "../include/GraphEngine/Graphs/Coordinates/CylindricalCoordinate.hpp"

// Paraboloid function
Vec3 hollowParaboloid(float r, float phi, Var v)
{
    float y = r * r * r; // height along Y-axis
    float x = r * cos(phi);
    float z = r * sin(phi);

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

int main(){
    Graph *graph = Graph::getInstance(25);

    CylindricalCoordinate* cor = new CylindricalCoordinate();
    Var v;
    v.addVar(20.0f);
    v.addVar(10.0f);

    Surface* graph1 = cor->drawSurface(torus, v);
    // Surface* graph2 = cor->drawSurface(mobius, v);
    Surface* graph3 = cor->drawSurface(sphere, v);
    // Surface* graph4 = cor->drawSurface(ripple, v);
    // Surface* graph5 = cor->drawSurface(saddle, v);
    Surface* graph6 = cor->drawSurface(flower, v);

    std::cout << "here we are" << std::endl;

    graph->play(graph1);
    // graph->play(graph2);
    graph->play(graph3);
    // graph->play(graph4);
    // graph->play(graph5);
    graph->play(graph6);

    graph->run();
}