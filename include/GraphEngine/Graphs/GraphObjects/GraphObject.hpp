#ifndef __GRAPHOBJECT_HPP__
#define __GRAPHOBJECT_HPP__

#include <iostream>
#include <vector>
#include <cmath>
#include <algorithm>
#include <limits>
#include <stdlib.h>
#include <functional>

#include "../../Utils/GraphColor.hpp"
#include "../../Utils/Config.hpp"

#include "../../Core/Shader.hpp"
#include "../../Core/GraphApp.hpp"

// glm library header to handle matrix
#include "../../../glm/glm.hpp"
#include "../../../glm/gtc/matrix_transform.hpp"

//third party lirary to handle the earcut of fill points
#include "../../../earcut.hpp"
#include "../../Utils/intersection_helper.hpp"

using Point2D = std::pair<float, float>;
using Polygonn = std::vector<std::vector<Point2D>>;

class GraphApp;

class GraphObject : public GraphMathObject
{
public:
    std::pair<float, float> range = {-1, 1};
    int resolution = 500;

public:
    int drawStart = 0, drawSize = 0;

    //specific to this class
    GLuint StrokeVAO, StrokeVBO;
    GLuint FillVAO, FillVBO;
    GLuint CoverVAO, CoverVBO;

    bool stroke_data_initialized = false;
    bool fill_data_initialized = false;

    float use_bezier_always = false;

    Var func_var;

    glm::vec3 (*func)(float, Var) = nullptr;
    glm::vec3 (*updater_func)(float, float) = nullptr;

public:
    std::string vertexShaderPath = "./shaders/quadratic_shaders/vertex.vs";
    std::string fragmentShaderPath = "./shaders/quadratic_shaders/fragment.fs";
    std::string geometricShaderPath = "./shaders/quadratic_shaders/geometry.gs";

    std::string fillVertexShaderPath = "./shaders/quadratic_fill/vertex.vs";
    std::string fillFragmentShaderPath = "./shaders/quadratic_fill/fragment.fs";
    std::string fillGeometricShaderPath = "./shaders/quadratic_fill/geometry.gs";

    std::string stencilVertexPath = "./shaders/stencil_fill/vertex.vs";
    std::string stencilFragPath = "./shaders/stencil_fill/stencil_pass.fs";
    std::string coverFragPath = "./shaders/stencil_fill/cover_pass.fs";


    std::vector<GraphColor> colors;
    std::vector<GraphColor> stroke_colors;
    std::vector<GraphColor> fill_colors;
    // contains points to draw a stroke
    std::vector<glm::vec3> stroke_prev_points;
    std::vector<glm::vec3> stroke_current_points;
    std::vector<glm::vec3> stroke_next_points;
    std::vector<glm::vec3> stroke_color_array;

    // contains points to draw the fill
    std::vector<glm::vec3> current_fill_points;
    std::vector<uint32_t> fillvertices;

    // contains sub graph of one graph
    std::vector<float> updateStartTime;
    std::vector<float> updateEndTime;

    std::vector<int> sub_path_start_indices;

    Shader *stencil_shader = nullptr;
    Shader *cover_shader = nullptr;

protected:
    void update(float dt) override;
    void updateStroke(float dt) override;
    void updateFill(float dt) override;

    void applyUpdaterFunction(float dt) override;

    void updateStrokePoints() override;
    void updateFillPoints() override;
    void updatePoints() override;

public:
    GraphObject();
    ~GraphObject() {}

    void Init(float dt = 0) override;

    void InitStrokeData() override;
    void InitFillData() override;

    
    void setStrokeData() override;
    void setFillData() override;
    
    void initializeStrokeShader() override;
    void initializeFillShader() override;
    
    void uploadStrokeDataToShader() override;
    void uploadFillDataToShader() override;

    void InitSubObject(float dt);
    
    void setColor()
    {
        colors.push_back(GraphColor());
    }

    void setColor(GraphColor color)
    {

        colors.push_back(color);
    }

    void setColor(std::vector<GraphColor> color_set)
    {
        this->colors.clear();
        this->colors = color_set;
    }

    void draw(float dt) override
    {
        update(dt);
    }

    void add(GraphMathObject *sub_object) override
    {
        subGraphObjects.push_back(sub_object );
    }

    void interpolate(const GraphMathObject *, float t = 0) override;
    void alignPoints(GraphMathObject*) override;
    
    std::vector<glm::vec3> getAllBezierPoints() override;
    void setAllBezierPoints(const std::vector<glm::vec3>& pts) override;

    void linearInterpolate(int);
    std::vector<glm::vec3> linearInterpolate(std::vector<glm::vec3>, int );
    void functionalInterpolate(int);

    void interpolateColor(int);

    void start_bezier_path(glm::vec3 start_point) override;
    void add_cubic_bezier_curve_to(glm::vec3 control1, glm::vec3 control2, glm::vec3 end_anchor) override;
    void add_quadratic_bezier_curve_to(glm::vec3 control, glm::vec3 end_anchor) override;
    void add_line_to(glm::vec3 end_anchor) override;
    void close_path();
    void build_points_from_bezier() override;
    void subdivide_bezier_curves() override;

    void generatePoints(glm::vec3 (*func)(float, Var), Var v);
    void makeSmooth();
    void UpdateGraphWithFunction(float);
    void setUpdater(glm::vec3 (*updater_function)(float, float), float s_t, float d)
    {
        this->updater_func = updater_function;
        this->updateStartTime.push_back(s_t);
        this->updateEndTime.push_back(s_t + d );
    }
    void applyColorToVertex() override;

    void setPoints(glm::vec3 p)
    {
        points.push_back(p);
    }

    void setDimension(float, float, float, float);

    // contains all the helper functions
    int getSize()
    {
        return (int)points.size();
    }

    int getFillSize(){
        return current_fill_points.size();
    }

    void setDrawStart(int index)
    {
        drawStart = index;
    }
    void setDrawSize(int size)
    {
        drawSize = size;
    }

    void scaleTo(float scale_ratio);
    void fit(float width, float height, float buff = 0.0f);

    //positioning function related to this class not the parent class
    glm::vec3 getPosition(Position pos = Position::NONE);
    glm::vec3 getRelativePosition(glm::vec3 targetPosition, Position pos, glm::vec3 buffer = glm::vec3(0, 0, 0));
    void nextTo(GraphObject *target, Position pos = Position::NONE, float buffer = 0.0f);
};


#endif