#include "../../../include/GraphEngine/Graphs/GraphObjects/GraphMathObject.hpp"

int GraphMathObject::getPointsSize()
{
    return (int)(points.size());
}

void GraphMathObject::setBaseModel(glm::mat4 model)
{
    this->model = model;
}

void GraphMathObject::setFillProgress(float fill_progress)
{
    this->fillProgress = fill_progress;
}

void GraphMathObject::setStrokeProgress(float stroke_progress)
{
    this->progress = stroke_progress;
}

void GraphMathObject::setFillOpacity(float fill_opacity)
{
    this->fillOpacity = fill_opacity;
}

void GraphMathObject::setStrokeOpacity(float stroke_opacity)
{
    this->stroke_opacity = stroke_opacity;
}

void GraphMathObject::setStrokeWidth(float stroke_width)
{
    this->line_width = stroke_width * 0.5f;
}

void GraphMathObject::setScale(glm::vec3 scale_factor)
{
    this->scale_factor = scale_factor;
    this->scale_x = scale_factor.x;
    this->scale_y = scale_factor.y;
    this->scale_z = scale_factor.z;
}

void GraphMathObject::moveTo(glm::vec3 newPos)
{
    translate = newPos - glm::vec3(x, y, 0);
    //<<<<< This must be implement later for animation purpose ***
    updatePoints();
}

void GraphMathObject::moveTo(Position pos)
{
    glm::vec3 translate = glm::vec3(0, 0, 0);
    switch (pos)
    {
    case Position::LEFT:
        translate = glm::vec3(x - width, y, 0);
        break;
    case Position::RIGHT:
        translate = glm::vec3(x + width, y, 0);
        break;
    case Position::TOP:
        translate = glm::vec3(x, y + height, 0);
        break;
    case Position::BOTTOM:
        translate = glm::vec3(x, y - height, 0);
        break;
    case Position::TOP_LEFT:
        translate = glm::vec3(x - width, y + height, 0);
        break;
    case Position::TOP_RIGHT:
        translate = glm::vec3(x + width, y + height, 0);
        break;
    case Position::BOTTOM_LEFT:
        translate = glm::vec3(x - width, y - height, 0);
        break;
    case Position::BOTTOM_RIGHT:
        translate = glm::vec3(x + width, y - height, 0);
        break;
    case Position::NONE:
        translate = glm::vec3(x + width / 2, y - height / 2, 0);
        break;
    }

    moveTo(translate);
}

void GraphMathObject::setMoveTo(glm::vec3 pos){
    translate = pos - glm::vec3(x, y, 0);
    updatePoints();
}

void GraphMathObject::scaleTo(glm::vec3 scale_factor)
{
    this->scale_x = scale_factor.x;
    this->scale_y = scale_factor.y;
    this->scale_z = scale_factor.z;
    // ***
    // updatePoints();
    // setStrokeData();
    // uploadStrokeDataToShader();
}

void GraphMathObject::setRotation(glm::vec3 rot_amount, glm::vec3 rot_pivot)
{
    rotation_vec = rot_amount;
    rotation_pivot = rot_pivot;
}

void GraphMathObject::rotate(glm::vec3 rotation_amount, glm::vec3 pivot)
{
    setRotation(rotation_amount, pivot);
    // ***
    updatePoints();
}

void GraphMathObject::setTranslate(glm::vec3 trans)
{
    this->translate = trans - glm::vec3(x, y, 0);
}

void GraphMathObject::setStrokeColors(std::vector<GraphColor> stroke_colors)
{
    this->stroke_colors = stroke_colors;
}

void GraphMathObject::setFillColors(std::vector<GraphColor> fill_colors)
{
    this->fill_colors = fill_colors;
}

void GraphMathObject::setStrokeColorLinearInterpolation(bool interpolate)
{
    this->stroke_color_interpolate = interpolate;
}

void GraphMathObject::setFillColorLinearInterpolation(bool interpolate)
{
    this->fill_color_interpolate = interpolate;
}

std::pair<glm::vec3, glm::vec3> GraphMathObject::getStrokeColorIndex(int index)
{
    int color_array_size = stroke_colors.size();
    float segments = static_cast<float>(stroke_color_resolution) / (color_array_size - 1);
    if (stroke_color_interpolate)
    {
        int color_idx = static_cast<int>(index / segments);
        float t_a_lin = (index - (color_idx * segments)) / segments;
        float t_b_lin = ((index + 1) - (color_idx * segments)) / segments;
        float t_a = (1.0f - cos(t_a_lin * M_PI)) * 0.5f;
        float t_b = (1.0f - cos(t_b_lin * M_PI)) * 0.5f;

        glm::vec3 color_a = glm::mix(GraphColor::colorToVec(stroke_colors[color_idx]), GraphColor::colorToVec(stroke_colors[color_idx + 1]), t_a);
        glm::vec3 color_b = glm::mix(GraphColor::colorToVec(stroke_colors[color_idx]), GraphColor::colorToVec(stroke_colors[color_idx + 1]), t_b);

        return {color_a, color_b};
    }
    else
    {
        int color_idx = static_cast<int>(index % static_cast<int>(segments));
        return {GraphColor::colorToVec(stroke_colors[color_idx]), GraphColor::colorToVec(stroke_colors[color_idx])};
    }
}

std::pair<glm::vec3, glm::vec3> GraphMathObject::getFillColorIndex(int index)
{
    int color_array_size = fill_colors.size();
    float segments = static_cast<float>(fill_color_resolution * 1.0f) / (color_array_size - 1.0f);
    if (fill_color_interpolate)
    {
        int color_idx = static_cast<int>(index / (segments * 1.0f));
        float t_a_lin = (index - color_idx * segments) / segments;
        float t_b_lin = ((index + 1) - (color_idx * segments)) / segments;
        float t_a = (1.0f - cos(t_a_lin * M_PI)) * 0.5f;
        float t_b = (1.0f - cos(t_b_lin * M_PI)) * 0.5f;

        glm::vec3 color_a = glm::mix(GraphColor::colorToVec(fill_colors[color_idx]), GraphColor::colorToVec(fill_colors[color_idx + 1]), t_a);
        glm::vec3 color_b = glm::mix(GraphColor::colorToVec(fill_colors[color_idx]), GraphColor::colorToVec(fill_colors[color_idx + 1]), t_b);

        return {color_a, color_b};
    }
    else
    {
        int color_idx = static_cast<int>(index % static_cast<int>(segments));
        return {GraphColor::colorToVec(fill_colors[color_idx]), GraphColor::colorToVec(fill_colors[color_idx])};
    }
}

// --- Bezier Path Methods ---

void GraphMathObject::start_bezier_path(glm::vec3 start_point)
{
    is_bezier_path = true;
    bezier_points.clear();
    bezier_points.push_back(start_point);
}

void GraphMathObject::add_cubic_bezier_curve_to(glm::vec3 control1, glm::vec3 control2, glm::vec3 end_anchor)
{
    if (bezier_points.empty()) {
        start_bezier_path(glm::vec3(0, 0, 0));
    }
    bezier_points.push_back(control1);
    bezier_points.push_back(control2);
    bezier_points.push_back(end_anchor);
}

void GraphMathObject::add_quadratic_bezier_curve_to(glm::vec3 control, glm::vec3 end_anchor)
{
    // Convert quadratic to cubic bezier for unified storage
    if (bezier_points.empty()) {
        start_bezier_path(glm::vec3(0, 0, 0));
    }
    glm::vec3 start_anchor = bezier_points.back();
    
    // Control1 = Start + 2/3 * (Control - Start)
    glm::vec3 control1 = start_anchor + (2.0f / 3.0f) * (control - start_anchor);
    // Control2 = End + 2/3 * (Control - End)
    glm::vec3 control2 = end_anchor + (2.0f / 3.0f) * (control - end_anchor);
    
    add_cubic_bezier_curve_to(control1, control2, end_anchor);
}

void GraphMathObject::add_line_to(glm::vec3 end_anchor)
{
    if (bezier_points.empty()) {
        start_bezier_path(glm::vec3(0, 0, 0));
    }
    glm::vec3 start_anchor = bezier_points.back();
    
    // For a straight line, controls are 1/3 and 2/3 along the line
    glm::vec3 control1 = start_anchor + (1.0f / 3.0f) * (end_anchor - start_anchor);
    glm::vec3 control2 = start_anchor + (2.0f / 3.0f) * (end_anchor - start_anchor);
    
    add_cubic_bezier_curve_to(control1, control2, end_anchor);
}

void GraphMathObject::subdivide_bezier_curves()
{
    if (!is_bezier_path || bezier_points.size() < 4) return;
    
    std::vector<glm::vec3> new_points;
    new_points.push_back(bezier_points[0]);
    
    // Iterate through cubic curve segments
    // Sequence is: start_anchor, c1, c2, end_anchor, c1, c2, end_anchor...
    for (size_t i = 1; i + 2 < bezier_points.size(); i += 3) {
        glm::vec3 p0 = bezier_points[i - 1];
        glm::vec3 p1 = bezier_points[i];
        glm::vec3 p2 = bezier_points[i + 1];
        glm::vec3 p3 = bezier_points[i + 2];
        
        for (int step = 1; step <= bezier_subdivision_resolution; ++step) {
            float t = static_cast<float>(step) / bezier_subdivision_resolution;
            float omt = 1.0f - t;
            
            // Cubic Bezier Formula
            glm::vec3 point = (omt * omt * omt) * p0 + 
                              3.0f * (omt * omt) * t * p1 + 
                              3.0f * omt * (t * t) * p2 + 
                              (t * t * t) * p3;
                              
            new_points.push_back(point);
        }
    }
    
    points = new_points;
}

void GraphMathObject::build_points_from_bezier()
{
    if (is_bezier_path && bezier_dirty) {
        subdivide_bezier_curves();
        bezier_dirty = false;
        stroke_dirty = true;
    }
}

std::vector<glm::vec3> GraphMathObject::getAllBezierPoints() {
    return bezier_points;
}

void GraphMathObject::setAllBezierPoints(const std::vector<glm::vec3>& pts) {
    if (pts.size() != bezier_points.size() || (pts.size() > 0 && glm::distance(pts[0], bezier_points[0]) > 0.0f)) {
        bezier_points = pts;
        is_bezier_path = !bezier_points.empty();
        bezier_dirty = true;
        build_points_from_bezier();
    }
}
