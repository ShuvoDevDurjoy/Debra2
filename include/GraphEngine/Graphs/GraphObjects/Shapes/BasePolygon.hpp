#ifndef __BASE_POLYGON_HPP__
#define __BASE_POLYGON_HPP__

#include "../GraphObject.hpp"

class BasePolygon : public GraphObject
{
public:
    int sides = 4;
    float radius = 5.0f;
    float centerX = 0.0f, centerY = 0.0f;

public:
    BasePolygon(int sides = 4, float radius = 5.0f, float centerX = 0.0f, float centerY = 0.0f)
        : sides(sides), radius(radius), centerX(centerX), centerY(centerY)
    {
        resolution = sides;
        // setDimension();
        generatePoints();
    }

    void generatePoints()
    {
        if (sides <= 0) return;

        float angleStep = glm::two_pi<float>() / sides;
        float minx = INT_MAX, maxX = INT_MIN, minY = INT_MAX, maxY = INT_MIN;
        
        std::vector<glm::vec3> polygon_corners;
        for (int i = 0; i < sides; i++)
        {
            float angle = i * angleStep;
            float x = round(radius * cos(angle) * 100) / 100;
            float y = round(radius * sin(angle) * 100) / 100;
            glm::vec3 point = glm::vec3(x, y, 0) + glm::vec3(centerX, centerY, 0);
            polygon_corners.push_back(point);
            
            minx = std::min(minx, point.x);
            maxX = std::max(maxX, point.x);
            minY = std::min(minY, point.y);
            maxY = std::max(maxY, point.y);
        }
        
        // Build bezier paths around the polygon corners
        start_bezier_path(polygon_corners[0]);
        for (size_t i = 1; i <= polygon_corners.size(); ++i) {
            glm::vec3 next_corner = polygon_corners[i % polygon_corners.size()];
            add_line_to(next_corner); // This generates a cubic representation of a line
        }

        close_path();

        setDimension(minx, maxX, minY, maxY);
        build_points_from_bezier();
    }
};

#endif