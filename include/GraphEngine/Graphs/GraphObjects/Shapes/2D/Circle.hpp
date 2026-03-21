#ifndef __CIRCLE_HPP__
#define __CIRCLE_HPP__

#include <GraphEngine/Graphs/GraphObjects/GraphObject.hpp>

class Circle : public GraphObject
{
    public:
        float radius, centerX, centerY, centerZ;

    public:
        Circle(float radius, float x, float y, float z = 0.0f)
        {
            this->radius = radius;
            this->centerX = x;
            this->centerY = y;
            this->centerZ = z;
            makeSmooth();
            Init();
        }

        Circle(float radius = 5, glm::vec3 pos = glm::vec3(0.0f))
        { // 0 sides so BasePolygon generates nothing
            // Increase subdivision for smoother circle
            this->radius = radius;
            this->centerX = pos.x;
            this->centerY = pos.y;
            this->centerZ = pos.z;
            // bezier_subdivision_resolution = 128;
            makeSmooth();
            Init();
        }

        void generatePoints() override
        {
            // Magic number for a bezier circle approximation
            float k = 0.552284749831f * radius;

            glm::vec3 c = glm::vec3(centerX, centerY, centerZ);

            // Right point
            glm::vec3 r = c + glm::vec3(radius, 0, centerZ);
            // Top point
            glm::vec3 t = c + glm::vec3(0, radius, centerZ);
            // Left point
            glm::vec3 l = c + glm::vec3(-radius, 0, centerZ);
            // Bottom point
            glm::vec3 b = c + glm::vec3(0, -radius, centerZ);

            start_bezier_path(r);
            // Right to Top
            add_cubic_bezier_curve_to(r + glm::vec3(0, k, 0), t + glm::vec3(k, 0, 0), t);
            // Top to Left
            add_cubic_bezier_curve_to(t + glm::vec3(-k, 0, 0), l + glm::vec3(0, k, 0), l);
            // Left to Bottom
            add_cubic_bezier_curve_to(l + glm::vec3(0, -k, 0), b + glm::vec3(-k, 0, 0), b);
            // Bottom to Right
            add_cubic_bezier_curve_to(b + glm::vec3(k, 0, 0), r + glm::vec3(0, -k, 0), r);

            setDimension(centerX - radius, centerX + radius, centerY - radius, centerY + radius);
        }
};

class Dot: public Circle{
    public: 
        Dot(float r = 4.0f, float centerX = 0, float centerY = 0, float centerZ = 0): Circle(r, centerX, centerY, centerZ){
            showStroke = false;
            showFill = true;
            makeSmooth();
        }
};

#endif