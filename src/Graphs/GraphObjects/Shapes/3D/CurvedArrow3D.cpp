#include <GraphEngine/Graphs/GraphObjects/Shapes/3D/CurvedArrow3D.hpp>
#include <iostream>

CurvedArrow3D::CurvedArrow3D(glm::vec3 start, glm::vec3 control, glm::vec3 end,
                             float shaft_radius, float head_length)
    : start_anchor(start), control1(control), end_anchor(end),
      shaft_radius(shaft_radius), head_length(head_length), is_cubic(false)
{
    this->resolution = {100, 32};
    this->showStroke = false;
    this->setFillOpacity(1.0f);
    this->showGrid = false;
    Init();
}

CurvedArrow3D::CurvedArrow3D(glm::vec3 start, glm::vec3 c1, glm::vec3 c2, glm::vec3 end,
                             float shaft_radius, float head_length)
    : start_anchor(start), control1(c1), control2(c2), end_anchor(end),
      shaft_radius(shaft_radius), head_length(head_length), is_cubic(true)
{
    this->resolution = {100, 32};
    this->showStroke = false;
    this->setFillOpacity(1.0f);
    this->showGrid = false;
    Init();
}

void CurvedArrow3D::generatePoints() {
    points.clear();
    normals.clear();
    fill_indices.clear();
    sub_surface_ranges.clear();

    auto eval_bezier = [&](float t) {
        float omt = 1.0f - t;
        if (is_cubic) {
            return (omt * omt * omt) * start_anchor +
                   3.0f * (omt * omt) * t * control1 +
                   3.0f * omt * (t * t) * control2 +
                   (t * t * t) * end_anchor;
        } else {
            return (omt * omt) * start_anchor +
                   2.0f * omt * t * control1 +
                   (t * t) * end_anchor;
        }
    };

    auto eval_derivative = [&](float t) {
        float omt = 1.0f - t;
        if (is_cubic) {
            return 3.0f * (omt * omt) * (control1 - start_anchor) +
                   6.0f * omt * t * (control2 - control1) +
                   3.0f * (t * t) * (end_anchor - control2);
        } else {
            return 2.0f * (omt) * (control1 - start_anchor) +
                   2.0f * t * (end_anchor - control1);
        }
    };

    const int steps = 40;
    const int T = 16; // Radial resolution
    float head_radius = shaft_radius * 3.0f;

    // 1. Shaft (Tube)
    int shaftBaseIndex = points.size();
    for (int i = 0; i <= steps; ++i) {
        float t = (float)i / steps;
        glm::vec3 pos = eval_bezier(t);
        glm::vec3 tangent = glm::normalize(eval_derivative(t));
        
        // Find normal and binormal (Parallel Transport Frame or simple approach)
        glm::vec3 up = (std::abs(tangent.y) < 0.99f) ? glm::vec3(0, 1, 0) : glm::vec3(1, 0, 0);
        glm::vec3 normal = glm::normalize(glm::cross(up, tangent));
        glm::vec3 binormal = glm::normalize(glm::cross(tangent, normal));

        for (int j = 0; j < T; j++) {
            float theta = (float(j) / T) * 2.0f * M_PI;
            glm::vec3 p = pos + shaft_radius * (cos(theta) * normal + sin(theta) * binormal);
            points.push_back(p);
            normals.push_back(glm::normalize(p - pos));
        }
    }
    sub_surface_ranges.push_back({shaftBaseIndex, (steps + 1) * T});

    for (int i = 0; i < steps; i++) {
        for (int j = 0; j < T; j++) {
            int p00 = shaftBaseIndex + i * T + j;
            int p10 = shaftBaseIndex + (i + 1) * T + j;
            int p01 = shaftBaseIndex + i * T + ((j + 1) % T);
            int p11 = shaftBaseIndex + (i + 1) * T + ((j + 1) % T);
            fill_indices.push_back(p00); fill_indices.push_back(p10); fill_indices.push_back(p01);
            fill_indices.push_back(p10); fill_indices.push_back(p11); fill_indices.push_back(p01);
        }
    }

    // 2. Head (Cone at end_anchor)
    glm::vec3 tip_tangent = glm::normalize(eval_derivative(1.0f));
    glm::vec3 up = (std::abs(tip_tangent.y) < 0.99f) ? glm::vec3(0, 1, 0) : glm::vec3(1, 0, 0);
    glm::vec3 normal = glm::normalize(glm::cross(up, tip_tangent));
    glm::vec3 binormal = glm::normalize(glm::cross(tip_tangent, normal));

    int headBaseIndex = points.size();
    const int R = 5; // Head levels
    for (int i = 0; i < R; i++) {
        float s = (float)i / (R - 1); // 0 at base, 1 at tip
        float r = head_radius * (1.0f - s);
        glm::vec3 pos = end_anchor + tip_tangent * (s * head_length);
        
        for (int j = 0; j < T; j++) {
            float theta = (float(j) / T) * 2.0f * M_PI;
            glm::vec3 p = pos + r * (cos(theta) * normal + sin(theta) * binormal);
            points.push_back(p);
            // Normal is slightly tilted towards tip
            glm::vec3 n = glm::normalize(cos(theta) * normal + sin(theta) * binormal + (head_radius/head_length) * tip_tangent);
            normals.push_back(n);
        }
    }
    sub_surface_ranges.push_back({headBaseIndex, R * T});
    for (int i = 0; i < R - 1; i++) {
        for (int j = 0; j < T; j++) {
            int p00 = headBaseIndex + i * T + j;
            int p10 = headBaseIndex + (i + 1) * T + j;
            int p01 = headBaseIndex + i * T + ((j + 1) % T);
            int p11 = headBaseIndex + (i + 1) * T + ((j + 1) % T);
            fill_indices.push_back(p00); fill_indices.push_back(p10); fill_indices.push_back(p01);
            fill_indices.push_back(p10); fill_indices.push_back(p11); fill_indices.push_back(p01);
        }
    }

    // 3. Junction Ring (Connecting shaft ending to head base)
    // For simplicity, we just overlap them or draw a disk. 
    // The current approach has them slightly overlapping at end_anchor.

    updateDimensions();
}
