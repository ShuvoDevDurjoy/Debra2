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
    this->line_width = stroke_width;
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
    // updatePoints();
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
