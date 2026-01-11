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
        setDimension();
        generatePoints();
    }

    void generatePoints()
    {

        float angleStep = glm::two_pi<float>() / sides;
        float minx = INT_MAX, maxX = INT_MIN, minY = INT_MAX, maxY = INT_MIN;
        for (int i = 0; i <= sides; i++)
        {
            float angle = i * angleStep;
            // Position
            float x = round(radius * cos(angle) * 100) / 100;
            float y = round(radius * sin(angle) * 100) / 100;
            glm::vec3 point = glm::vec3(x, y, 0);
            minx = std::min(minx * 1.0f, (point[0]));
            maxX = std::max(maxX * 1.0f, (point[0]));
            minY = std::min(minY * 1.0f, (point[1]));
            maxY = std::max(maxY * 1.0f, (point[1]));
            setPoints(point + glm::vec3(centerX, centerY, 0));
        }

        setDimension(minx, maxX, minY, maxY);
    }
};

#endif