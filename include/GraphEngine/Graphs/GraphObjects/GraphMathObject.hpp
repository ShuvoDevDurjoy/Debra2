#ifndef __GRAPHMATHOBJECT_HPP__
#define __GRAPHMATHOBJECT_HPP__

#include <iostream>
#include <algorithm>
#include <vector>
#include <climits>
#include <cmath>
#include <unordered_map>
#include <functional>

#include "../../Utils/Config.hpp"
#include "../../Utils/GraphColor.hpp"
#include "../../Core/Shader.hpp"
#include "../../../glm/glm.hpp"
#include "../../../glm/gtc/matrix_transform.hpp"

class Scene;

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
    Shader *stroke_shader = nullptr, *fill_shader = nullptr;

    glm::mat4 model = glm::mat4(1.0f);
    glm::mat4 scale = glm::mat4(1.0f);

    glm::mat4 baseModel = glm::mat4(1.0f);
    glm::vec3 translate = glm::vec3(0, 0, 0);
    glm::vec3 scale_factor = glm::vec3(1.0f, 1.0f, 1.0f);
    glm::vec3 rotation_vec = glm::vec3(0, 0, 0);
    glm::vec3 rotation_pivot = glm::vec3(0, 0, 0);
    float phi = 0, theta = 0;

    int layer = 0;
    long long id;
    GraphMathObject* parent = nullptr;
    Scene* scene = nullptr;
    static long long next_id;

    bool showStroke = true;
    bool showGraph = true;
    bool showFill = false;

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
    
    // Modern Manim-like Bezier path storage
    bool is_bezier_path = false;
    std::vector<glm::vec3> bezier_points; // Stores cubic bezier points: anchor, control1, control2, anchor ...
    int bezier_subdivision_resolution = 64;

    // Sub-path tracking (Manim-style anchor-coincidence detection)
    // Each entry is the index into bezier_points where a new sub-path begins.
    // Populated by start_bezier_path() for subsequent calls.
    std::vector<int> bezier_sub_path_starts;

    // After subdivide_bezier_curves(), this holds (firstVertex, vertexCount) per sub-path
    // for use with glMultiDrawArrays(GL_LINE_STRIP, ...).
    std::vector<std::pair<int,int>> point_sub_path_ranges;

    std::vector<GraphMathObject *> subGraphObjects;

    // Adaptive subdivision parameters (used by subdivide_bezier_curves)
    int adaptive_max_depth = 8;
    float adaptive_tolerance = 0.01f;
    float adaptive_min_distance = 0.001f;

    
    std::vector<GraphColor> stroke_colors;
    std::vector<GraphColor> fill_colors;

    bool stroke_data_initialized = false;
    bool fill_data_initialized = false;

    float x, y, width, height;
    Dimension *dim;

public:
    GraphMathObject();
    virtual void on_added_to_scene(Scene* target_scene);
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
    virtual void alignPoints(GraphMathObject*) = 0;
    virtual void interpolate(const GraphMathObject*, float t = 0) = 0;
    virtual void updateDimensions();

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
    void setStrokeWidth(float, bool to_sub_graphs = true);
    void setSubdivisions(int res) { bezier_subdivision_resolution = res; bezier_dirty = true; }
    void setSmooth(bool smooth = true) { is_smooth = smooth; bezier_dirty = true; }
    void moveTo(Position pos = Position::NONE);
    void moveTo(glm::vec3);
    void setMoveTo(glm::vec3);
    void scaleTo(glm::vec3);
    void rotate(glm::vec3 rotation_amount = glm::vec3(0, 0, 0), glm::vec3 rotation_axis = glm::vec3(0, 0, 0));

public:
    void setRotation(glm::vec3 rot_amount, glm::vec3 rot_pivot);
    virtual void updatePoints() = 0;
    virtual void applyColorToVertex() = 0;

    // Bezier Path Methods
    virtual void start_bezier_path(glm::vec3 start_point) = 0;
    virtual void add_cubic_bezier_curve_to(glm::vec3 control1, glm::vec3 control2, glm::vec3 end_anchor) = 0;
    virtual void add_quadratic_bezier_curve_to(glm::vec3 control, glm::vec3 end_anchor) = 0;
    virtual void add_line_to(glm::vec3 end_anchor) = 0;
    virtual void build_points_from_bezier() = 0;
    virtual void subdivide_bezier_curves() = 0;
    virtual std::vector<glm::vec3> getAllBezierPoints();
    virtual void setAllBezierPoints(const std::vector<glm::vec3>& pts);
    virtual void add(GraphMathObject *);

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
        return glm::distance(a, b) < 0.0001f;
    }


public:
    bool bezier_dirty = true;
    bool stroke_dirty = true;
    bool is_smooth = false;
};

#endif