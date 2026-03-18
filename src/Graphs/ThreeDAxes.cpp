#include <GraphEngine/Graphs/GraphObjects/ThreeDAxes.hpp>


ThreeDAxes::ThreeDAxes(){
    generatePoints();
}

void ThreeDAxes::generate(Vec3 start, Vec3 end)
{
    glm::vec3 dir = glm::normalize(glm::vec3(end.x - start.x, end.y - start.y, end.z - start.z));
    float length = glm::length(glm::vec3(end.x - start.x, end.y - start.y, end.z - start.z));

    float headLength = 1.5f;
    float shaftLength = length;

    float shaftRadius = 0.2f;
    float headRadius = 1.0f;

    glm::vec3 base = glm::vec3(start.x, start.y, start.z);
    glm::vec3 shaftEnd = base + dir * shaftLength;
    glm::vec3 apex = base + dir * (length + headLength);

    // --- Universal perpendicular basis ---
    glm::vec3 u;
    if (fabs(dir.x) > fabs(dir.y))
        u = glm::normalize(glm::vec3(-dir.z, 0, dir.x));
    else
        u = glm::normalize(glm::vec3(0, dir.z, -dir.y));
    glm::vec3 v = glm::cross(dir, u);
    int segments = 16;
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
        add(base + r0);
        add(n0);
        add(base + r1);
        add(n1);
        add(shaftEnd + r1);
        add(n1);

        // Second triangle
        add(base + r0);
        add(n0);
        add(shaftEnd + r1);
        add(n1);
        add(shaftEnd + r0);
        add(n0);
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

        // Normals for cone: perpendicular to cone surface
        glm::vec3 n0 = glm::normalize(glm::cross(p1 - apex, p0 - apex));
        glm::vec3 n1 = glm::normalize(glm::cross(p0 - apex, p1 - apex));

        // float d = glm::pi<float>() / 4;

        float degreeCos = cos(glm::pi<float>() / 3);
        float degreeSin = sin(glm::pi<float>() / 6);

        glm::vec3 r3 = r0 * degreeCos + dir * degreeSin;
        glm::vec3 r4 = r1 * degreeCos + dir * degreeSin;

        n0 = glm::normalize(r3);
        n1 = glm::normalize(r4);

        add(p0);
        add(-glm::normalize(dir));
        add(shaftEnd);
        add(-glm::normalize(dir));
        add(p1);
        add(-glm::normalize(dir));

        add(p0);
        add(n0);
        add(p1);
        add(n1);
        add(apex); // Tip normal along axis
        add(glm::normalize(dir));
    }

    // std::cout << "Done here" << std::endl;
}

void ThreeDAxes::generatePoints()
{

    const float Rx = 80.0f; // axis extent
    const float Ry = 40.0f; // axis extent
    const float Rz = 60.0f; // axis extent

    Vec3 origin(0, 0, 0);
    generate(origin, Vec3(Rx, 0.0f, 0.0f));
    // std::cout << "size is: " << getSize() << std::endl;
    generate(origin, Vec3(-Rx, 0.0f, 0.0f));
    // std::cout << "size is: " << getSize() << std::endl;
    generate(origin, Vec3(0.0f, Ry, 0.0f));
    // std::cout << "size is: " << getSize() << std::endl;
    generate(origin, Vec3(0.0f, -Ry, 0.0f));
    // std::cout << "size is: " << getSize() << std::endl;
    generate(origin, Vec3(0.0f, 0.0f, Rz));
    // std::cout << "size is: " << getSize() << std::endl;
    generate(origin, Vec3(0.0f, 0.0f, -Rz));
    std::cout << "size is: " << getSize() << std::endl;

    if (getSize() == 0) // Corrected the check
    {
        std::cerr << "No points to initialize!\n";
        return;
    }
}
