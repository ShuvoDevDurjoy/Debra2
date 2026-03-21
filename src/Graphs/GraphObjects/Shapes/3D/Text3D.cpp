#include <GraphEngine/Graphs/GraphObjects/Shapes/3D/Text3D.hpp>
#include <GraphEngine/Core/GraphApp.hpp>
#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_OUTLINE_H
#include <earcut.hpp>
#include <iostream>

static const float BASE_PIXEL_SIZE = 100.0f;

struct DecomposeData {
    Char3D* obj;
    float world_x;
    float world_y;
    float world_z;
    float unit_scale;
};

static int move_to(const FT_Vector* to, void* user) {
    auto data = (DecomposeData*)user;
    float wx = data->world_x + to->x * data->unit_scale;
    float wy = data->world_y + to->y * data->unit_scale;
    data->obj->start_bezier_path(glm::vec3(wx, wy, data->world_z));
    return 0;
}

static int line_to(const FT_Vector* to, void* user) {
    auto data = (DecomposeData*)user;
    float wx = data->world_x + to->x * data->unit_scale;
    float wy = data->world_y + to->y * data->unit_scale;
    data->obj->add_line_to(glm::vec3(wx, wy, data->world_z));
    return 0;
}

static int conic_to(const FT_Vector* control, const FT_Vector* to, void* user) {
    auto data = (DecomposeData*)user;
    data->obj->add_quadratic_bezier_curve_to(
        glm::vec3(data->world_x + control->x * data->unit_scale,
                  data->world_y + control->y * data->unit_scale, data->world_z),
        glm::vec3(data->world_x + to->x * data->unit_scale,
                  data->world_y + to->y * data->unit_scale, data->world_z));
    return 0;
}

static int cubic_to(const FT_Vector* c1, const FT_Vector* c2, const FT_Vector* to, void* user) {
    auto data = (DecomposeData*)user;
    data->obj->add_cubic_bezier_curve_to(
        glm::vec3(data->world_x + c1->x * data->unit_scale,
                  data->world_y + c1->y * data->unit_scale, data->world_z),
        glm::vec3(data->world_x + c2->x * data->unit_scale,
                  data->world_y + c2->y * data->unit_scale, data->world_z),
        glm::vec3(data->world_x + to->x * data->unit_scale,
                  data->world_y + to->y * data->unit_scale, data->world_z));
    return 0;
}

Char3D::Char3D(char c, float x, float y, float z, float scale) 
    : c(c), x(x), y(y), z(z), scale(scale)
{
    this->adaptive_tolerance = 0.0005f;
    this->adaptive_max_depth = 10;
    this->adaptive_min_distance = 0.0001f;
    this->showStroke = false;
    this->showFill = true;
    this->setFillOpacity(1.0f);
    Init();
}

void Char3D::generatePoints() {
    points.clear();
    normals.clear();
    fill_indices.clear();
    bezier_points.clear();
    bezier_sub_path_starts.clear();
    point_sub_path_ranges.clear();
    sub_surface_ranges.clear();

    generateBezierOutline(c, x, y, z, scale);
    subdivide_bezier_curves();

    if (point_sub_path_ranges.empty()) return;

    // We treat the set of paths as a single complicated polygon with holes
    // Earcut handles this by taking a vector of rings (outer, hole, hole...)
    // Since glyphs can have multiple sub-paths (some outer, some holes), we pass them all.
    std::vector<std::vector<std::pair<float, float>>> polygon;
    for (auto& range : point_sub_path_ranges) {
        std::vector<std::pair<float, float>> ring;
        for (int i = 0; i < range.second; ++i) {
            glm::vec3 p = points[range.first + i];
            ring.push_back({p.x, p.y});
        }
        // Earcut expects closed rings usually, or at least it doesn't mind if they are.
        polygon.push_back(ring);
    }

    auto indices = mapbox::earcut<uint32_t>(polygon);
    
    // The indices from earcut refer to the vertices in the concatenated list of all rings.
    // Our 'points' vector already contains these vertices in that exact order.
    // So we just copy the indices and add normals.
    fill_indices = indices;
    
    normals.clear();
    normals.resize(points.size(), glm::vec3(0, 0, 1));
    
    sub_surface_ranges.push_back({0, (int)points.size()});

    updateDimensions();
}

void Char3D::generateBezierOutline(char c, float x, float y, float z, float scale) {
    FT_Library ft;
    if (FT_Init_FreeType(&ft)) return;

    FT_Face face;
    if (FT_New_Face(ft, (GraphApp::font_path).c_str(), 0, &face)) {
        FT_Done_FreeType(ft);
        return;
    }

    FT_Set_Pixel_Sizes(face, 0, static_cast<FT_UInt>(BASE_PIXEL_SIZE));

    if (FT_Load_Char(face, c, FT_LOAD_NO_BITMAP)) {
        FT_Done_Face(face);
        FT_Done_FreeType(ft);
        return;
    }

    float unit_scale = scale / (64.0f * BASE_PIXEL_SIZE);

    FT_Outline_Funcs funcs;
    funcs.move_to  = move_to;
    funcs.line_to  = line_to;
    funcs.conic_to = conic_to;
    funcs.cubic_to = cubic_to;
    funcs.shift = 0;
    funcs.delta = 0;

    DecomposeData data = { this, x, y, z, unit_scale };
    FT_Outline_Decompose(&face->glyph->outline, &funcs, &data);

    FT_Done_Face(face);
    FT_Done_FreeType(ft);
}

Text3D::Text3D(const std::string& textStr, float x, float y, float z, float scale)
    : textStr(textStr), x(x), y(y), z(z), scale(scale)
{
    this->showFill = true;
    this->showStroke = false;
    Init();
}

void Text3D::generatePoints() {
    subGraphObjects.clear();
    // We don't have our own points, we just add Char3D sub-objects
    
    FT_Library ft;
    FT_Init_FreeType(&ft);
    FT_Face face;
    FT_New_Face(ft, (GraphApp::font_path).c_str(), 0, &face);
    FT_Set_Pixel_Sizes(face, 0, static_cast<FT_UInt>(BASE_PIXEL_SIZE));

    float unit_scale = scale / (64.0f * BASE_PIXEL_SIZE);
    float pen_x = x;

    for (char c : textStr) {
        if (c == ' ') {
            pen_x += 0.25f * scale;
            continue;
        }

        Char3D* ch = new Char3D(c, pen_x, y, z, scale);
        ch->fill_colors = this->fill_colors;
        add(ch);

        if (!FT_Load_Char(face, c, FT_LOAD_NO_BITMAP)) {
            pen_x += face->glyph->advance.x * unit_scale;
        }
    }

    FT_Done_Face(face);
    FT_Done_FreeType(ft);
    
    updateDimensions();
}
