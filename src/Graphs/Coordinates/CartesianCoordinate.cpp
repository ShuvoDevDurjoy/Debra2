#include "../../../include/GraphEngine/Graphs/Coordinates/CartesianCoordinate.hpp"

CartesianGraph* CartesianCoordinate::generateFunction(Vec2 (*func)(float, Var), Var v){

    CartesianGraph *graph = new CartesianGraph();
    int i;
    float t = t_start;
    float step = (t_end - t_start) / resolution;
    float yPrev = 0;
    for (i = 0; i < resolution; i++)
    {
        // std::cout << "i: " << i << std::endl;
        t = t + step;
        Vec2 vec = func(t, v);

        yPrev = vec.y;
        graph->add(vec.x);
        graph->add(vec.y);
        graph->add(0.0f);

        graph->add(0);
        graph->add(0);
        graph->add(1.0f);
    }


        return graph;
}

Arrow3D *CartesianCoordinate::drawArrow(Vec3 start, Vec3 end)
{
    Arrow3D *arrow = new Arrow3D();

    glm::vec3 dir = glm::normalize(glm::vec3(end.x - start.x, end.y - start.y, end.z - start.z));
    float length = glm::length(glm::vec3(end.x - start.x, end.y - start.y, end.z - start.z));

    float headLength = 1.5f;
    float shaftLength = length - headLength;

    float shaftRadius = 0.1f;
    float headRadius = 1.0f;

    glm::vec3 base = glm::vec3(start.x, start.y, start.z);
    glm::vec3 shaftEnd = base + dir * shaftLength;
    glm::vec3 apex = base + dir * length;

    // --- Universal perpendicular basis ---
    glm::vec3 u;
    if (fabs(dir.x) > fabs(dir.y))
        u = glm::normalize(glm::vec3(-dir.z, 0, dir.x));
    else
        u = glm::normalize(glm::vec3(0, dir.z, -dir.y));
    glm::vec3 v = glm::cross(dir, u);
    int segments = 64;
    float step = 2.0f * glm::pi<float>() / segments;

    // --- Shaft (cylinder) ---
    for (int i = 0; i < segments; ++i)
    {
        float t0 = i * step;
        float t1 = (i + 1) * step;

        glm::vec3 r0 = u * (shaftRadius * cos(t0)) + v * (shaftRadius * sin(t0));
        glm::vec3 r1 = u * (shaftRadius * cos(t1)) + v * (shaftRadius * sin(t1));

        // Normals for cylinder: radial
        glm::vec3 n0 = glm::normalize(r0);
        glm::vec3 n1 = glm::normalize(r1);

        // First triangle
        arrow->add(base + r0);
        arrow->add(n0);
        arrow->add(base + r1);
        arrow->add(n1);
        arrow->add(shaftEnd + r1);
        arrow->add(n1);

        // Second triangle
        arrow->add(base + r0);
        arrow->add(n0);
        arrow->add(shaftEnd + r1);
        arrow->add(n1);
        arrow->add(shaftEnd + r0);
        arrow->add(n0);
    }

    // --- Cone (arrowhead) ---
    for (int i = 0; i < segments; ++i)
    {
        float t0 = i * step;
        float t1 = (i + 1) * step;

        glm::vec3 r0 = u * (headRadius * cos(t0)) + v * (headRadius * sin(t0));
        glm::vec3 r1 = u * (headRadius * cos(t1)) + v * (headRadius * sin(t1));

        glm::vec3 p0 = shaftEnd + r0;
        glm::vec3 p1 = shaftEnd + r1;

        // Normals for cone: perpend

        arrow->add(p0);
        arrow->add(-glm::normalize(dir));
        arrow->add(shaftEnd);
        arrow->add(-glm::normalize(dir));
        arrow->add(p1);
        arrow->add(-glm::normalize(dir));

        arrow->add(p0);
        arrow->add(glm::normalize(dir));
        arrow->add(p1);
        arrow->add(glm::normalize(dir));
        arrow->add(apex); // Tip normal along axis
        arrow->add(glm::normalize(dir));
    }

    return arrow;
}
