#ifndef __GRAPHMATHOBJECT_HPP__
#define __GRAPHMATHOBJECT_HPP__

#include <iostream>
#include <algorithm>
#include <vector>
#include <climits>
#include <cmath>
#include <unordered_map>

#include "../../Utils/Config.hpp"
#include "../../Utils/GraphColor.hpp"

#include "../../Core/Shader.hpp"
#include "../../../glm/glm.hpp"
#include "../../../glm/gtc/matrix_transform.hpp"

class Dimension
{
    float x, y, width, height;

public:
    Dimension(float min_x, float max_x, float min_y, float max_y)
    {
        this->x = min_x;
        this->y = max_y;
        this->width = (max_x - min_x);
        this->height = (max_y - min_y);
    }
};

class GraphMathObject
{
public:
    Shader *stroke_shader, *fill_shader;

    glm::mat4 model = glm::mat4(1.0f), view, projection;
    glm::mat4 scale = glm::mat4(1.0f);

    glm::mat4 baseModel = glm::mat4(1.0f);
    glm::vec3 translate = glm::vec3(0, 0, 0);
    glm::vec3 scale_factor = glm::vec3(1.0f, 1.0f, 1.0f);
    glm::vec3 rotation_vec = glm::vec3(0, 0, 0);
    glm::vec3 rotation_pivot = glm::vec3(0, 0, 0);
    float phi = 0, theta = 0;

    int layer = 0;

    bool showGraph = true;

    // need to be changed to stroke_line_width
    float line_width = 3.0f;
    float scale_x = 1, scale_y = 1, scale_z = 1;

    bool is_initialized = false;
    
    // Industry-standard stroke join styles
    enum class StrokeJoinStyle {
        MITER = 0,      // Sharp corners (default)
        BEVEL = 1,      // Beveled/cut corners
        ROUND = 2       // Rounded corners
    };
    
    StrokeJoinStyle stroke_join_style = StrokeJoinStyle::MITER;
    float miter_limit = 2.0f;  // Industry standard: 2.0-4.0

public:
    // parameters for animation controlling
    // float stroke_progress = 1.0f;
    // float fill_progress = 1.0f;
    // float fill_opacity = 1.0f;
    float stroke_opacity = 1.0f;

    // should be changed to the above format after testing
    float progress = 1.0f;
    float fillProgress = 1.0f;
    float fillOpacity = 1.0f;

    bool stroke_color_interpolate = false;
    bool fill_color_interpolate = false;

    int stroke_color_resolution;
    int fill_color_resolution;

public:
    std::vector<glm::vec3> points;
    std::vector<GraphColor> stroke_colors;
    std::vector<GraphColor> fill_colors;

    bool stroke_data_initialized = false;
    bool fill_data_initialized = false;

    float x, y, width, height;
    Dimension *dim;

public:
    virtual void Init(float) = 0;

public:
    virtual void update(float dt) = 0;
    virtual void updateStroke(float dt) = 0;
    virtual void updateFill(float dt) = 0;
    virtual void applyUpdaterFunction(float dt) = 0;
    virtual void updateStrokePoints() = 0;
    virtual void updateFillPoints() = 0;

public:
    virtual void InitStrokeData() = 0;
    virtual void InitFillData() = 0;
    virtual void setStrokeData() = 0;
    virtual void setFillData() = 0;
    virtual void initializeStrokeShader() = 0;
    virtual void initializeFillShader() = 0;
    virtual void uploadStrokeDataToShader() = 0;
    virtual void uploadFillDataToShader() = 0;

public:
    virtual void draw(float dt) = 0;

public:
    virtual void interpolate(int) = 0;

public:
    int getPointsSize();

public:
    void setBaseModel(glm::mat4);
    void setScale(glm::vec3);
    void setTranslate(glm::vec3);

public:
    void setFillProgress(float);
    void setStrokeProgress(float);
    void setFillOpacity(float);
    void setStrokeOpacity(float);
    void setStrokeWidth(float);
    void moveTo(Position pos = Position::NONE);
    void moveTo(glm::vec3);
    void setMoveTo(glm::vec3);
    void scaleTo(glm::vec3);
    void rotate(glm::vec3 rotation_amount = glm::vec3(0, 0, 0), glm::vec3 rotation_axis = glm::vec3(0, 0, 0));

public:
    void setRotation(glm::vec3 rot_amount, glm::vec3 rot_pivot);
    virtual void updatePoints() = 0;

public:
    void setStrokeColors(std::vector<GraphColor> stroke_colors);
    void setFillColors(std::vector<GraphColor> fill_colors);
    std::pair<glm::vec3, glm::vec3> getStrokeColorIndex(int);
    std::pair<glm::vec3, glm::vec3> getFillColorIndex(int);
    void setStrokeColorLinearInterpolation(bool interpolate = true);
    void setFillColorLinearInterpolation(bool interpolate = true);
    
public:
    // Industry-standard stroke styling methods
    void setStrokeJoinStyle(StrokeJoinStyle style) { stroke_join_style = style; }
    void setMiterLimit(float limit) { miter_limit = glm::clamp(limit, 1.0f, 10.0f); }
    StrokeJoinStyle getStrokeJoinStyle() const { return stroke_join_style; }
    float getMiterLimit() const { return miter_limit; }

    bool isEqual(glm::vec3 a, glm::vec3 b)
    {
        return a[0] == b[0] && a[1] == b[1] && a[2] == b[2];
    }
};

#endif