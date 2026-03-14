#include "../include/GraphEngine/Graphs/GraphObjects/Shapes/Text.hpp"
#include "../include/GraphEngine/Core/GraphApp.hpp"

// FreeType loads the font at this pixel size:
// glyph coordinates are in 1/64th-pixel units, so the full range is
// BASE_PIXEL_SIZE * 64 units per em (roughly).
// We derive world-space units as: world = ft_unit / (64 * BASE_PIXEL_SIZE) * scale
// This makes scale == "desired cap-height in graph world units".
static const float BASE_PIXEL_SIZE = 100.0f;

struct DecomposeData {
    GraphMathObject* obj;
    float world_x;    // pen x in world coords (baseline-left origin)
    float world_y;    // pen y in world coords (baseline)
    float unit_scale; // ft_unit → world unit factor = scale / (64 * BASE_PIXEL_SIZE)
};

static int move_to(const FT_Vector* to, void* user) {
    auto data = (DecomposeData*)user;
    float wx = data->world_x + to->x * data->unit_scale;
    float wy = data->world_y + to->y * data->unit_scale;
    data->obj->start_bezier_path(glm::vec3(wx, wy, 0));
    return 0;
}

static int line_to(const FT_Vector* to, void* user) {
    auto data = (DecomposeData*)user;
    float wx = data->world_x + to->x * data->unit_scale;
    float wy = data->world_y + to->y * data->unit_scale;
    data->obj->add_line_to(glm::vec3(wx, wy, 0));
    return 0;
}

static int conic_to(const FT_Vector* control, const FT_Vector* to, void* user) {
    auto data = (DecomposeData*)user;
    data->obj->add_quadratic_bezier_curve_to(
        glm::vec3(data->world_x + control->x * data->unit_scale,
                  data->world_y + control->y * data->unit_scale, 0),
        glm::vec3(data->world_x + to->x * data->unit_scale,
                  data->world_y + to->y * data->unit_scale, 0));
    return 0;
}

static int cubic_to(const FT_Vector* c1, const FT_Vector* c2, const FT_Vector* to, void* user) {
    auto data = (DecomposeData*)user;
    data->obj->add_cubic_bezier_curve_to(
        glm::vec3(data->world_x + c1->x * data->unit_scale,
                  data->world_y + c1->y * data->unit_scale, 0),
        glm::vec3(data->world_x + c2->x * data->unit_scale,
                  data->world_y + c2->y * data->unit_scale, 0),
        glm::vec3(data->world_x + to->x * data->unit_scale,
                  data->world_y + to->y * data->unit_scale, 0));
    return 0;
}

Char::Char(char c, float x, float y, float scale) {
    this->adaptive_tolerance = 0.0005f;
    this->adaptive_max_depth = 10;
    this->adaptive_min_distance = 0.0001f;
    generateBezierOutline(c, x, y, scale);
}

void Char::generateBezierOutline(char c, float x, float y, float scale) {
    FT_Library ft;
    if (FT_Init_FreeType(&ft)) return;

    FT_Face face;
    if (FT_New_Face(ft, FONT_FAMILY_PATH, 0, &face)) {
        FT_Done_FreeType(ft);
        return;
    }

    // Load at a fixed reference pixel size.
    // All coordinates from FreeType will be in (1/64 of a pixel) units.
    FT_Set_Pixel_Sizes(face, 0, static_cast<FT_UInt>(BASE_PIXEL_SIZE));

    if (FT_Load_Char(face, c, FT_LOAD_NO_BITMAP)) {
        FT_Done_Face(face);
        FT_Done_FreeType(ft);
        return;
    }

    // unit_scale converts FreeType's 1/64-pixel integers → graph world units.
    // scale = desired character height in world units.
    // At BASE_PIXEL_SIZE, a cap-height g≈ BASE_PIXEL_SIZE pixels,
    // and FT coordinates span BASE_PIXEL_SIZE * 64 units vertically.
    // So: world = ft_unit / (64 * BASE_PIXEL_SIZE) * scale
    float unit_scale = scale / (64.0f * BASE_PIXEL_SIZE);

    FT_Outline_Funcs funcs;
    funcs.move_to  = move_to;
    funcs.line_to  = line_to;
    funcs.conic_to = conic_to;
    funcs.cubic_to = cubic_to;
    funcs.shift = 0;
    funcs.delta = 0;

    DecomposeData data = { this, x, y, unit_scale };
    FT_Outline_Decompose(&face->glyph->outline, &funcs, &data);

    FT_Done_Face(face);
    FT_Done_FreeType(ft);
    close_path();
    build_points_from_bezier();

    // Update the bounding box for this character in world units
    if (!points.empty()) {
        float minx = points[0].x, maxx = points[0].x;
        float miny = points[0].y, maxy = points[0].y;
        for (auto& p : points) {
            minx = std::min(minx, p.x); maxx = std::max(maxx, p.x);
            miny = std::min(miny, p.y); maxy = std::max(maxy, p.y);
        }
        setDimension(minx, maxx, miny, maxy);
    }
}

Text::Text(const std::string& textStr, float x, float y, float scale) {
    float pen_x = x;

    // We need a FreeType face once to measure glyph advances for kerning.
    FT_Library ft;
    FT_Init_FreeType(&ft);
    FT_Face face;
    FT_New_Face(ft, FONT_FAMILY_PATH, 0, &face);
    FT_Set_Pixel_Sizes(face, 0, static_cast<FT_UInt>(BASE_PIXEL_SIZE));

    float unit_scale = scale / (64.0f * BASE_PIXEL_SIZE);

    for (char c : textStr) {

        if(c == ' '){
            // Advance by a fixed width for spaces (e.g., 1/4 em)
            pen_x += 0.25f * scale;
            continue;
        }
        Char* ch = new Char(c, pen_x, y, scale);
        ch->setStrokeWidth(1.0f);
        add(ch);

        if (!FT_Load_Char(face, c, FT_LOAD_NO_BITMAP)) {
            // advance.x is in 1/64-pixel units; convert to world units
            pen_x += face->glyph->advance.x * unit_scale;
        }
    }

    FT_Done_Face(face);
    FT_Done_FreeType(ft);
}
