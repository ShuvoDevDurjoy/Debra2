#ifndef __CIRCLE_HPP__
#define __CIRCLE_HPP__

#include "BasePolygon.hpp"

class Circle : public BasePolygon
{
public:
    Circle(float radius = 5, float centerX = 0, float centerY = 0)
        : BasePolygon(0, radius, centerX, centerY)
    { // 0 sides so BasePolygon generates nothing
        // Increase subdivision for smoother circle
        bezier_subdivision_resolution = 128;
        generateBezierCircle();
    }

private:
    void generateBezierCircle()
    {
        // Magic number for a bezier circle approximation
        float k = 0.552284749831f * radius;

        glm::vec3 c = glm::vec3(centerX, centerY, 0);

        // Right point
        glm::vec3 r = c + glm::vec3(radius, 0, 0);
        // Top point
        glm::vec3 t = c + glm::vec3(0, radius, 0);
        // Left point
        glm::vec3 l = c + glm::vec3(-radius, 0, 0);
        // Bottom point
        glm::vec3 b = c + glm::vec3(0, -radius, 0);

        start_bezier_path(r);
        // Right to Top
        add_cubic_bezier_curve_to(r + glm::vec3(0, k, 0), t + glm::vec3(k, 0, 0), t);
        // Top to Left
        add_cubic_bezier_curve_to(t + glm::vec3(-k, 0, 0), l + glm::vec3(0, k, 0), l);
        // Left to Bottom
        add_cubic_bezier_curve_to(l + glm::vec3(0, -k, 0), b + glm::vec3(-k, 0, 0), b);
        // Bottom to Right
        add_cubic_bezier_curve_to(b + glm::vec3(k, 0, 0), r + glm::vec3(0, -k, 0), r);

        makeSmooth();
        setDimension(centerX - radius, centerX + radius, centerY - radius, centerY + radius);
        build_points_from_bezier();
    }
};

class Dot : public BasePolygon
{
public:
    Dot(float centerX = 0, float centerY = 0, float radius = 0.5f, float res = 32) : BasePolygon(res, radius, centerX, centerY) {}
};

#endif