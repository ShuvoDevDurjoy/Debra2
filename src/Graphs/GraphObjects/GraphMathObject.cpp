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

void GraphMathObject::setStrokeWidth(float stroke_width, bool to_sub_graphs)
{
    this->line_width = stroke_width;
    if (to_sub_graphs) {
        for (auto subObj : subGraphObjects) {
            subObj->setStrokeWidth(stroke_width, true);
        }
    }
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

void GraphMathObject::setMoveTo(glm::vec3 pos)
{
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

    if (bezier_points.empty())
    {
        // Very first sub-path: clear everything and begin fresh
        bezier_sub_path_starts.clear();
        bezier_sub_path_starts.push_back(0);
        bezier_points.push_back(start_point);
    }
    else
    {
        bezier_points.push_back(bezier_points.back());
        bezier_sub_path_starts.push_back(static_cast<int>(bezier_points.size()));
        bezier_points.push_back(start_point);
    }
    bezier_dirty = true;
}

void GraphMathObject::add_cubic_bezier_curve_to(glm::vec3 control1, glm::vec3 control2, glm::vec3 end_anchor)
{
    if (bezier_points.empty())
    {
        start_bezier_path(glm::vec3(0, 0, 0));
    }
    bezier_points.push_back(control1);
    bezier_points.push_back(control2);
    bezier_points.push_back(end_anchor);
}

void GraphMathObject::add_quadratic_bezier_curve_to(glm::vec3 control, glm::vec3 end_anchor)
{
    // Convert quadratic to cubic bezier for unified storage
    if (bezier_points.empty())
    {
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
    if (bezier_points.empty())
    {
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
    if (!is_bezier_path || bezier_points.size() < 4)
        return;

    // Build a sorted list of every bezier_points index that begins a new sub-path.
    // The very first sub-path always starts at index 0.
    std::vector<int> sub_starts = {0};
    for (int idx : bezier_sub_path_starts)
    {
        sub_starts.push_back(idx);
    }
    std::sort(sub_starts.begin(), sub_starts.end());
    sub_starts.push_back(static_cast<int>(bezier_points.size())); // sentinel

    // Helpers
    auto eval_bezier = [](glm::vec3 p0, glm::vec3 p1, glm::vec3 p2, glm::vec3 p3, float t)
    {
        float omt = 1.0f - t;
        return (omt * omt * omt) * p0 +
               3.0f * (omt * omt) * t * p1 +
               3.0f * omt * (t * t) * p2 +
               (t * t * t) * p3;
    };

    auto distPointToSegment = [](glm::vec3 p, glm::vec3 a, glm::vec3 b) -> float
    {
        glm::vec3 ab = b - a;
        float l2 = glm::dot(ab, ab);
        if (l2 < 1e-8f)
            return glm::distance(p, a);
        float t = glm::clamp(glm::dot(p - a, ab) / l2, 0.0f, 1.0f);
        return glm::distance(p, a + t * ab);
    };

    std::vector<glm::vec3> all_points;
    point_sub_path_ranges.clear();

    // Process each sub-path independently
    for (int s = 0; s + 1 < (int)sub_starts.size(); ++s)
    {
        int beg = sub_starts[s];     // index of start anchor in bezier_points
        int end = sub_starts[s + 1]; // exclusive end

        // A sub-path needs at least anchor + c1 + c2 + end_anchor = 4 points
        if (end - beg < 4)
            continue;

        int sub_path_first_vertex = static_cast<int>(all_points.size());
        std::vector<glm::vec3> sub_points;
        sub_points.push_back(bezier_points[beg]);

        // Iterate cubic segments within this sub-path.
        // Layout: [anchor, c1, c2, anchor, c1, c2, anchor, ...]  (start is already in sub_points)
        for (int i = beg + 1; i + 2 < end; i += 3)
        {
            glm::vec3 p0 = bezier_points[i - 1];
            glm::vec3 p1 = bezier_points[i];
            glm::vec3 p2 = bezier_points[i + 1];
            glm::vec3 p3 = bezier_points[i + 2];

            glm::vec3 c_p0 = p0, c_p1 = p1, c_p2 = p2, c_p3 = p3;
            if (is_smooth)
            {
                float lineDist1 = distPointToSegment(p1, p0, p3);
                float lineDist2 = distPointToSegment(p2, p0, p3);

                if (lineDist1 < 1e-5f && lineDist2 < 1e-5f)
                {
                    // Degenerate (straight) segment — apply Catmull-Rom handles
                    int seg_in_path = (i - beg - 1) / 3;
                    glm::vec3 prev_anchor = (i - 1 >= beg + 3) ? bezier_points[i - 4] : p0;
                    glm::vec3 next_anchor = (i + 5 < end) ? bezier_points[i + 5] : p3;
                    c_p1 = p0 + (p3 - prev_anchor) / 6.0f;
                    c_p2 = p3 - (next_anchor - p0) / 6.0f;
                    (void)seg_in_path;
                }
            }

            struct SampleNode
            {
                float t;
                glm::vec3 p;
            };
            std::vector<SampleNode> final_pts;

            std::function<void(SampleNode, SampleNode, int)> subdivide =
                [&](SampleNode left, SampleNode right, int depth)
            {
                float t_mid = (left.t + right.t) * 0.5f;
                glm::vec3 p_mid = eval_bezier(c_p0, c_p1, c_p2, c_p3, t_mid);
                float dist = distPointToSegment(p_mid, left.p, right.p);
                float chordDist = glm::distance(left.p, right.p);
                if (depth < adaptive_max_depth && dist > adaptive_tolerance && chordDist > adaptive_min_distance)
                {
                    SampleNode mid = {t_mid, p_mid};
                    subdivide(left, mid, depth + 1);
                    subdivide(mid, right, depth + 1);
                }
                else
                {
                    if (final_pts.empty() || glm::distance(final_pts.back().p, left.p) > 1e-7f)
                        final_pts.push_back(left);
                    final_pts.push_back(right);
                }
            };

            SampleNode n1 = {0.0f, p0};
            SampleNode n2 = {1.0f, p3};
            subdivide(n1, n2, 0);

            for (size_t j = 0; j < final_pts.size(); ++j)
            {
                if (j == 0 && glm::distance(sub_points.back(), final_pts[j].p) < 1e-7f)
                    continue;
                sub_points.push_back(final_pts[j].p);
            }
        }

        // Append this sub-path's points and record its range
        int count = static_cast<int>(sub_points.size());
        for (auto &pt : sub_points)
            all_points.push_back(pt);
        point_sub_path_ranges.push_back({sub_path_first_vertex, count});
    }

    points = all_points;
}

void GraphMathObject::build_points_from_bezier()
{
    if (is_bezier_path && bezier_dirty)
    {
        subdivide_bezier_curves();
        bezier_dirty = false;
        stroke_dirty = true;
    }
}

std::vector<glm::vec3> GraphMathObject::getAllBezierPoints()
{
    return bezier_points;
}

void GraphMathObject::setAllBezierPoints(const std::vector<glm::vec3> &pts)
{
    if (pts.size() != bezier_points.size() || (pts.size() > 0 && glm::distance(pts[0], bezier_points[0]) > 0.0f))
    {
        bezier_points = pts;
        is_bezier_path = !bezier_points.empty();
        bezier_dirty = true;
        build_points_from_bezier();
    }
}
