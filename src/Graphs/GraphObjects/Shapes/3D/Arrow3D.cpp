#include <GraphEngine/Graphs/GraphObjects/Shapes/3D/Arrow3D.hpp>

Arrow3D::Arrow3D(glm::vec3 from, glm::vec3 to, float shaft_radius, float head_length, float buff){
    this->from = from;
    this->to = to;
    this->buff = buff;
    this->shaft_radius = shaft_radius;
    this->head_length = head_length;

    float dist = glm::distance(from, to);
    this->length = std::max(0.0f, dist - 2.0f * buff);
    
    this->graph_func = nullptr;
    this->resolution = {100, 32};
    this->showStroke = false;
    this->setFillOpacity(1.0f);
    this->showGrid = false;
}

void Arrow3D::generatePoints() {
    points.clear();
    normals.clear();
    sub_surface_ranges.clear();
    fill_indices.clear();
    stroke_indices.clear();

    if (length < 0.0001f) return;

    glm::vec3 direction = glm::normalize(to - from);
    glm::vec3 from_real = from + direction * buff;
    // to_real is actually not needed as we build along Z, but we need the translated origin
    
    float head_radius = shaft_radius * 3.0f;
    float actual_head_length = std::min(head_length, length * 0.5f);
    float shaft_length = length - actual_head_length;
    
    int R = 5;  // Radial resolution
    int T = 32; // Tangential resolution
    
    glm::vec3 z_axis(0, 0, 1);
    glm::mat4 rotation(1.0f);

    if (glm::length(glm::cross(z_axis, direction)) > 0.0001f) {
        glm::vec3 axis = glm::normalize(glm::cross(z_axis, direction));
        float angle = acos(glm::dot(z_axis, direction));
        rotation = glm::rotate(glm::mat4(1.0f), angle, axis);
    } else if (glm::dot(z_axis, direction) < -0.9999f) {
        // Opposite direction
        rotation = glm::rotate(glm::mat4(1.0f), (float)M_PI, glm::vec3(1, 0, 0));
    }
    
    auto transform = [&](glm::vec3 p) {
        return glm::vec3(rotation * glm::vec4(p, 1.0f)) + from_real;
    };
    auto transform_n = [&](glm::vec3 n) {
        return glm::normalize(glm::vec3(rotation * glm::vec4(n, 0.0f)));
    };

    // 1. Shaft Base Cap (Disk at z=0)
    int shaftBaseCapIndex = points.size();
    points.push_back(transform(glm::vec3(0, 0, 0))); 
    normals.push_back(transform_n(glm::vec3(0, 0, -1)));
    for (int j = 0; j < T; j++) {
        float theta = (float(j) / T) * 2.0f * M_PI;
        points.push_back(transform(glm::vec3(shaft_radius * cos(theta), shaft_radius * sin(theta), 0)));
        normals.push_back(transform_n(glm::vec3(0, 0, -1)));
    }
    sub_surface_ranges.push_back({shaftBaseCapIndex, T + 1});
    for (int j = 0; j < T; j++) {
        fill_indices.push_back(shaftBaseCapIndex);
        fill_indices.push_back(shaftBaseCapIndex + j + 1);
        fill_indices.push_back(shaftBaseCapIndex + ((j + 1) % T) + 1);
    }

    // 2. Shaft (Cylinder)
    int shaftBaseIndex = points.size();
    for (int i = 0; i < R; i++) {
        float z = (float(i) / (R - 1)) * shaft_length;
        for (int j = 0; j < T; j++) {
            float theta = (float(j) / T) * 2.0f * M_PI;
            points.push_back(transform(glm::vec3(shaft_radius * cos(theta), shaft_radius * sin(theta), z)));
            normals.push_back(transform_n(glm::vec3(cos(theta), sin(theta), 0.0f)));
        }
    }
    sub_surface_ranges.push_back({shaftBaseIndex, R * T});
    
    for (int i = 0; i < R - 1; i++) {
        for (int j = 0; j < T; j++) {
            int p00 = shaftBaseIndex + i * T + j;
            int p10 = shaftBaseIndex + (i + 1) * T + j;
            int p01 = shaftBaseIndex + i * T + ((j + 1) % T);
            int p11 = shaftBaseIndex + (i + 1) * T + ((j + 1) % T);
            
            fill_indices.push_back(p00); fill_indices.push_back(p10); fill_indices.push_back(p01);
            fill_indices.push_back(p10); fill_indices.push_back(p11); fill_indices.push_back(p01);
        }
    }

    // 3. Junction Ring (Connecting shaft ending to head base)
    int ringBaseIndex = points.size();
    for (int j = 0; j < T; j++) {
        float theta = (float(j) / T) * 2.0f * M_PI;
        // Inner point (on shaft edge)
        points.push_back(transform(glm::vec3(shaft_radius * cos(theta), shaft_radius * sin(theta), shaft_length)));
        normals.push_back(transform_n(glm::vec3(0, 0, 1)));
        // Outer point (on head base edge)
        points.push_back(transform(glm::vec3(head_radius * cos(theta), head_radius * sin(theta), shaft_length)));
        normals.push_back(transform_n(glm::vec3(0, 0, 1)));
    }
    sub_surface_ranges.push_back({ringBaseIndex, 2 * T});
    for (int j = 0; j < T; j++) {
        int p00 = ringBaseIndex + 2 * j;
        int p01 = ringBaseIndex + 2 * j + 1;
        int p10 = ringBaseIndex + 2 * ((j + 1) % T);
        int p11 = ringBaseIndex + 2 * ((j + 1) % T) + 1;
        
        fill_indices.push_back(p00); fill_indices.push_back(p01); fill_indices.push_back(p11);
        fill_indices.push_back(p00); fill_indices.push_back(p11); fill_indices.push_back(p10);
    }
    
    // 4. Head (Cone)
    int headBaseIndex = points.size();
    for (int i = 0; i < R; i++) {
        float t = float(i) / (R - 1);
        float z = shaft_length + t * head_length;
        float r = head_radius * (1.0f - t); 
        for (int j = 0; j < T; j++) {
            float theta = (float(j) / T) * 2.0f * M_PI;
            points.push_back(transform(glm::vec3(r * cos(theta), r * sin(theta), z)));
            glm::vec3 n = glm::normalize(glm::vec3(cos(theta) * head_length / head_radius, sin(theta) * head_length / head_radius, head_radius / head_length));
            normals.push_back(transform_n(n));
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
    updateDimensions();
}

void Arrow3D::setFillData() {}
void Arrow3D::setStrokeData() {}
