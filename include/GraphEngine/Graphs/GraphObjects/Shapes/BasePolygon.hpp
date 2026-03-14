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
    float b_radius = 0.0f;

public:
    BasePolygon(int sides = 4, float radius = 5.0f, float centerX = 0.0f, float centerY = 0.0f)
        : sides(sides), radius(radius), centerX(centerX), centerY(centerY)
    {
        resolution = sides;
        // setDimension();
        generatePoints();
    }

    BasePolygon& border_radius(float r) {
        b_radius = r;
        generatePoints();
        return *this;
    }

    void generatePoints()
    {
        if (sides <= 0) return;

        bezier_points.clear();
        bezier_sub_path_starts.clear();
        points.clear();
        point_sub_path_ranges.clear();
        current_fill_points.clear();

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
        
        if (b_radius <= 0.0001f) {
            // Build bezier paths around the polygon corners
            start_bezier_path(polygon_corners[0]);
            for (size_t i = 1; i <= polygon_corners.size(); ++i) {
                glm::vec3 next_corner = polygon_corners[i % polygon_corners.size()];
                add_line_to(next_corner); // This generates a cubic representation of a line
            }
        } else {
            int n = polygon_corners.size();
            float interior_angle = glm::pi<float>() * (sides - 2) / sides;
            float arc_angle = glm::pi<float>() - interior_angle;

            float d_tan = b_radius / tan(interior_angle / 2.0f);
            float d_ctrl = b_radius * (4.0f / 3.0f) * tan(arc_angle / 4.0f);

            // Cap the tangent distance to prevent overlapping corners
            float max_d = 0.0f;
            for (int i = 0; i < n; i++) {
                float d1 = glm::length(polygon_corners[i] - polygon_corners[(i + 1) % n]);
                if (max_d == 0.0f || d1 < max_d) max_d = d1;
            }
            if (d_tan > max_d / 2.05f) {
                float ratio = (max_d / 2.05f) / d_tan;
                d_tan *= ratio;
                d_ctrl *= ratio;
            }
            
            for (int i = 0; i < n; i++) {
                glm::vec3 prev = polygon_corners[(i - 1 + n) % n];
                glm::vec3 curr = polygon_corners[i];
                glm::vec3 next = polygon_corners[(i + 1) % n];

                glm::vec3 dir_prev = glm::normalize(prev - curr);
                glm::vec3 dir_next = glm::normalize(next - curr);

                glm::vec3 pt_tan1 = curr + dir_prev * d_tan;
                glm::vec3 pt_tan2 = curr + dir_next * d_tan;

                glm::vec3 c1 = pt_tan1 - dir_prev * d_ctrl; // towards corner
                glm::vec3 c2 = pt_tan2 - dir_next * d_ctrl; // towards corner

                if (i == 0) {
                    start_bezier_path(pt_tan1);
                } else {
                    add_line_to(pt_tan1);
                }

                add_cubic_bezier_curve_to(c1, c2, pt_tan2);
            }
        }

        close_path();

        setDimension(minx, maxX, minY, maxY);
        build_points_from_bezier();
    }
};

#endif