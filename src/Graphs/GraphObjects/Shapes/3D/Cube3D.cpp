#include <GraphEngine/Graphs/GraphObjects/Shapes/3D/Cube3D.hpp>

Cube3D::Cube3D(float s) : side_length(s) {
    this->graph_func = nullptr; // Manual point generation
}

void Cube3D::generatePoints() {
    points.clear();
    normals.clear();
    sub_surface_ranges.clear();
    fill_indices.clear();
    stroke_indices.clear();

    float r = side_length / 2.0f;
    
    glm::vec3 faces[6][4] = {
        { {-r, -r,  r}, { r, -r,  r}, {-r,  r,  r}, { r,  r,  r} }, // +Z
        { { r, -r, -r}, {-r, -r, -r}, { r,  r, -r}, {-r,  r, -r} }, // -Z
        { { r, -r,  r}, { r, -r, -r}, { r,  r,  r}, { r,  r, -r} }, // +X
        { {-r, -r, -r}, {-r, -r,  r}, {-r,  r, -r}, {-r,  r,  r} }, // -X
        { {-r,  r,  r}, { r,  r,  r}, {-r,  r, -r}, { r,  r, -r} }, // +Y
        { {-r, -r, -r}, { r, -r, -r}, {-r, -r,  r}, { r, -r,  r} }  // -Y
    };

    glm::vec3 normals_faces[6] = {
        {0, 0, 1}, {0, 0, -1}, {1, 0, 0}, {-1, 0, 0}, {0, 1, 0}, {0, -1, 0}
    };

    for(int f = 0; f < 6; ++f) {
        int baseIndex = points.size();
        for(int v = 0; v < 4; ++v) {
            points.push_back(faces[f][v]);
            normals.push_back(normals_faces[f]);
        }
        
        int p00 = baseIndex;
        int p10 = baseIndex + 1;
        int p01 = baseIndex + 2;
        int p11 = baseIndex + 3;
        
        fill_indices.push_back(p00);
        fill_indices.push_back(p01);
        fill_indices.push_back(p10);
        
        fill_indices.push_back(p10);
        fill_indices.push_back(p01);
        fill_indices.push_back(p11);
        
        stroke_indices.push_back(p00); stroke_indices.push_back(p10);
        stroke_indices.push_back(p10); stroke_indices.push_back(p11);
        stroke_indices.push_back(p11); stroke_indices.push_back(p01);
        stroke_indices.push_back(p01); stroke_indices.push_back(p00);
        
        sub_surface_ranges.push_back({baseIndex, 4});
    }
}

void Cube3D::setFillData() {
    // Overridden to prevent ThreeDObject from clearing fill_indices 
    // since we calculate them optimally in generatePoints().
}

void Cube3D::setStrokeData() {
    // Overridden to prevent ThreeDObject from clearing stroke_indices
}
