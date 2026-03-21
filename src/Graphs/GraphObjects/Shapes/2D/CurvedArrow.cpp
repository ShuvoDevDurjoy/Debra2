#include <GraphEngine/Graphs/GraphObjects/Shapes/2D/CurvedArrow.hpp>
#include <GraphEngine/Graphs/GraphObjects/Shapes/2D/Polygon.hpp>

CurvedArrow::CurvedArrow(glm::vec3 start, glm::vec3 control, glm::vec3 end,
                         float tip_size, float shaft_width, float buff)
    : start_anchor(start), control1(control), end_anchor(end),
      tip_size(tip_size), shaft_width(shaft_width), buff(buff), is_cubic(false)
{
    setStrokeWidth(shaft_width);
    Init();
}

CurvedArrow::CurvedArrow(glm::vec3 start, glm::vec3 c1, glm::vec3 c2, glm::vec3 end,
                         float tip_size, float shaft_width, float buff)
    : start_anchor(start), control1(c1), control2(c2), end_anchor(end),
      tip_size(tip_size), shaft_width(shaft_width), buff(buff), is_cubic(true)
{
    setStrokeWidth(shaft_width);
    Init();
}

void CurvedArrow::generatePoints() {
    subGraphObjects.clear();

    // 1. Shaft (as a Bezier path)
    class Shaft : public GraphObject {
    public:
        Shaft() { Init(); }
        void generatePoints() override {} // Points added manually via bezier points
    };

    Shaft* shaft = new Shaft();
    shaft->setStrokeWidth(this->line_width);
    shaft->start_bezier_path(start_anchor);
    if (is_cubic) {
        shaft->add_cubic_bezier_curve_to(control1, control2, end_anchor);
    } else {
        shaft->add_quadratic_bezier_curve_to(control1, end_anchor);
    }
    shaft->build_points_from_bezier();
    add(shaft);

    // 2. Head (Triangle at the end)
    // Calculate tangent at end_anchor
    glm::vec3 tangent;
    if (is_cubic) {
        tangent = 3.0f * (end_anchor - control2);
    } else {
        tangent = 2.0f * (end_anchor - control1);
    }

    if (glm::length(tangent) < 0.0001f) tangent = glm::vec3(1, 0, 0);
    glm::vec3 dir = glm::normalize(tangent);
    glm::vec3 perp = glm::vec3(-dir.y, dir.x, 0);

    float h_half = tip_size * 0.6f;
    glm::vec3 head_base = end_anchor - dir * tip_size;

    std::vector<std::vector<glm::vec3>> head_pts = {
        {end_anchor, head_base + perp * h_half, head_base - perp * h_half, end_anchor}
    };
    Polygon* head = new Polygon(head_pts);
    head->showFill = true;
    head->showStroke = false;
    head->setFillOpacity(1.0f);
    add(head);

    updateDimensions();
}
