#include <GraphEngine/Graphs/GraphObjects/Shapes/3D/Line3D.hpp>

Line3D::Line3D(glm::vec3 from, glm::vec3 to, float thickness) {
    this->from = from;
    this->to = to;
    this->thickness = thickness;
    this->resolution = {100, 32};
    this->showStroke = false;
    this->setFillOpacity(1.0f);
    this->showGrid = false;
    Init();
}

void Line3D::generatePoints() {
    points.clear();
    normals.clear();
    sub_surface_ranges.clear();
    fill_indices.clear();
    stroke_indices.clear();

    float length = glm::distance(from, to);
    if (length < 0.0001f) return;

    glm::vec3 direction = glm::normalize(to - from);
    
    int R = 5;  // Radial resolution
    int T = 32; // Tangential resolution
    
    glm::vec3 z_axis(0, 0, 1);
    glm::mat4 rotation(1.0f);

    if (glm::length(glm::cross(z_axis, direction)) > 0.0001f) {
        glm::vec3 axis = glm::normalize(glm::cross(z_axis, direction));
        float angle = acos(glm::dot(z_axis, direction));
        rotation = glm::rotate(glm::mat4(1.0f), angle, axis);
    } else if (glm::dot(z_axis, direction) < -0.9999f) {
        rotation = glm::rotate(glm::mat4(1.0f), (float)M_PI, glm::vec3(1, 0, 0));
    }
    
    auto transform = [&](glm::vec3 p) {
        return glm::vec3(rotation * glm::vec4(p, 1.0f)) + from;
    };
    auto transform_n = [&](glm::vec3 n) {
        return glm::normalize(glm::vec3(rotation * glm::vec4(n, 0.0f)));
    };

    // Shaft Base Cap (Disk at z=0)
    int baseCapIndex = points.size();
    points.push_back(transform(glm::vec3(0, 0, 0))); 
    normals.push_back(transform_n(glm::vec3(0, 0, -1)));
    for (int j = 0; j < T; j++) {
        float theta = (float(j) / T) * 2.0f * M_PI;
        points.push_back(transform(glm::vec3(thickness * cos(theta), thickness * sin(theta), 0)));
        normals.push_back(transform_n(glm::vec3(0, 0, -1)));
    }
    sub_surface_ranges.push_back({baseCapIndex, T + 1});
    for (int j = 0; j < T; j++) {
        fill_indices.push_back(baseCapIndex);
        fill_indices.push_back(baseCapIndex + j + 1);
        fill_indices.push_back(baseCapIndex + ((j + 1) % T) + 1);
    }

    // Cylinder Body
    int bodyBaseIndex = points.size();
    for (int i = 0; i < R; i++) {
        float z = (float(i) / (R - 1)) * length;
        for (int j = 0; j < T; j++) {
            float theta = (float(j) / T) * 2.0f * M_PI;
            points.push_back(transform(glm::vec3(thickness * cos(theta), thickness * sin(theta), z)));
            normals.push_back(transform_n(glm::vec3(cos(theta), sin(theta), 0.0f)));
        }
    }
    sub_surface_ranges.push_back({bodyBaseIndex, R * T});
    for (int i = 0; i < R - 1; i++) {
        for (int j = 0; j < T; j++) {
            int p00 = bodyBaseIndex + i * T + j;
            int p10 = bodyBaseIndex + (i + 1) * T + j;
            int p01 = bodyBaseIndex + i * T + ((j + 1) % T);
            int p11 = bodyBaseIndex + (i + 1) * T + ((j + 1) % T);
            fill_indices.push_back(p00); fill_indices.push_back(p10); fill_indices.push_back(p01);
            fill_indices.push_back(p10); fill_indices.push_back(p11); fill_indices.push_back(p01);
        }
    }

    // Shaft Top Cap (Disk at z=length)
    int topCapIndex = points.size();
    points.push_back(transform(glm::vec3(0, 0, length))); 
    normals.push_back(transform_n(glm::vec3(0, 0, 1)));
    for (int j = 0; j < T; j++) {
        float theta = (float(j) / T) * 2.0f * M_PI;
        points.push_back(transform(glm::vec3(thickness * cos(theta), thickness * sin(theta), length)));
        normals.push_back(transform_n(glm::vec3(0, 0, 1)));
    }
    sub_surface_ranges.push_back({topCapIndex, T + 1});
    for (int j = 0; j < T; j++) {
        fill_indices.push_back(topCapIndex);
        fill_indices.push_back(topCapIndex + ((j + 1) % T) + 1);
        fill_indices.push_back(topCapIndex + j + 1);
    }

    updateDimensions();
}
