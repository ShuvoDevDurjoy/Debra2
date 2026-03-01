#include "../include/GraphEngine/Graphs/GraphObjects/Shapes/Text.hpp"
#include "../include/GraphEngine/Core/GraphApp.hpp"

struct DecomposeData {
    GraphMathObject* obj;
    float x, y, scale;
};

static int move_to(const FT_Vector* to, void* user) {
    auto data = (DecomposeData*)user;
    data->obj->start_bezier_path(glm::vec3(data->x + to->x * data->scale, data->y + to->y * data->scale, 0));
    return 0;
}

static int line_to(const FT_Vector* to, void* user) {
    auto data = (DecomposeData*)user;
    data->obj->add_line_to(glm::vec3(data->x + to->x * data->scale, data->y + to->y * data->scale, 0));
    return 0;
}

static int conic_to(const FT_Vector* control, const FT_Vector* to, void* user) {
    auto data = (DecomposeData*)user;
    data->obj->add_quadratic_bezier_curve_to(
        glm::vec3(data->x + control->x * data->scale, data->y + control->y * data->scale, 0),
        glm::vec3(data->x + to->x * data->scale, data->y + to->y * data->scale, 0));
    return 0;
}

static int cubic_to(const FT_Vector* c1, const FT_Vector* c2, const FT_Vector* to, void* user) {
    auto data = (DecomposeData*)user;
    data->obj->add_cubic_bezier_curve_to(
        glm::vec3(data->x + c1->x * data->scale, data->y + c1->y * data->scale, 0),
        glm::vec3(data->x + c2->x * data->scale, data->y + c2->y * data->scale, 0),
        glm::vec3(data->x + to->x * data->scale, data->y + to->y * data->scale, 0));
    return 0;
}

Char::Char(char c, float x, float y, float scale) {
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

    FT_Set_Pixel_Sizes(face, 0, 100); // Base size for extraction

    if (FT_Load_Char(face, c, FT_LOAD_NO_BITMAP)) {
        FT_Done_Face(face);
        FT_Done_FreeType(ft);
        return;
    }

    FT_Outline_Funcs funcs;
    funcs.move_to = move_to;
    funcs.line_to = line_to;
    funcs.conic_to = conic_to;
    funcs.cubic_to = cubic_to;
    funcs.shift = 0;
    funcs.delta = 0;

    DecomposeData data = {this, x, y, scale / 64.0f}; // FreeType uses 1/64 pixels
    FT_Outline_Decompose(&face->glyph->outline, &funcs, &data);

    FT_Done_Face(face);
    FT_Done_FreeType(ft);
    
    build_points_from_bezier();
}

Text::Text(const std::string& textStr, float x, float y, float scale) {
    float currentX = x;
    
    // We need a face to calculate advances
    FT_Library ft;
    FT_Init_FreeType(&ft);
    FT_Face face;
    FT_New_Face(ft, FONT_FAMILY_PATH, 0, &face);
    FT_Set_Pixel_Sizes(face, 0, 100);

    for (char c : textStr) {
        Char* ch = new Char(c, currentX, y, scale);
        add(ch);
        
        if (!FT_Load_Char(face, c, FT_LOAD_NO_BITMAP)) {
            currentX += (face->glyph->advance.x >> 6) * (scale / 64.0f) * 100.0f; // Simplified advance tracking
        }
    }
    
    FT_Done_Face(face);
    FT_Done_FreeType(ft);
}
