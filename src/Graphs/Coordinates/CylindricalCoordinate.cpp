#include "../../../include/GraphEngine/Graphs/Coordinates/CylindricalCoordinate.hpp"

Surface *CylindricalCoordinate::drawSurface(Vec3 (*func)(float, float, Var), Var v)
{
    Surface *surface = new Surface();

    resolutionY = 100;
    resolutionX = 100;

    float step_r = (outer_radius - inner_radius) / (resolutionX - 1);
    float step_phi = (phi_end - phi_start) / (resolutionY - 1);

    std::vector<Vec3> grid(resolutionX * resolutionY);

    // Build grid of points
    for (int i = 0; i < resolutionX; i++)
    {
        float r = inner_radius + i * step_r;

        for (int j = 0; j < resolutionY; j++)
        {
            float phi = phi_start + j * step_phi;
            grid[i * resolutionY + j] = func(r, phi, v);
        }
    }

    // Generate triangle mesh
    for (int i = 0; i < resolutionX - 1; i++)
    {
        for (int j = 0; j < resolutionY - 1; j++)
        {
            Vec3 A = grid[i * resolutionY + j];
            Vec3 B = grid[(i + 1) * resolutionY + j];
            Vec3 C = grid[i * resolutionY + (j + 1)];
            Vec3 D = grid[(i + 1) * resolutionY + (j + 1)];

            
            glm::vec3 a(A.x, A.y, A.z);
            glm::vec3 b(B.x, B.y, B.z);
            glm::vec3 c(C.x, C.y, C.z);
            glm::vec3 d(D.x, D.y, D.z);
            
            glm::vec3 normal = glm::normalize(glm::cross(b - a, d - a));
            Vec3 normalV1 = Vec3(normal[0], normal[1], normal[2]);

            // Triangle 1: A, B, C
            surface->add(A);
            surface->add(normalV1);
            surface->add(B);
            surface->add(normalV1);
            surface->add(C);
            surface->add(normalV1);
            
            
            // normal = glm::normalize(glm::cross(c - d, b - d));
            normalV1 = Vec3(normal[0], normal[1], normal[2]);
            
            surface->add(C);
            surface->add(normalV1);
            surface->add(B);
            surface->add(normalV1);
            surface->add(D);
            surface->add(normalV1);
        }
    }

    return surface;
}
