#ifndef __GRAPHOBJECT_HPP__
#define __GRAPHOBJECT_HPP__

#include <iostream>
#include <vector>
#include <cmath>
#include <algorithm>
#include <limits>
#include <stdlib.h>

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

    std::vector<GraphColor> colors;
    std::vector<GraphColor> stroke_colors;
    std::vector<GraphColor> fill_colors;
    // contains points to draw a stroke
    std::vector<glm::vec3> stroke_prev_points;
    std::vector<glm::vec3> stroke_current_points;
    std::vector<glm::vec3> stroke_next_points;
    std::vector<glm::vec3> stroke_color_array;

    // contains points to draw the fill
    std::vector<float> prev_fill_points;
    std::vector<glm::vec3> current_fill_points;
    std::vector<uint32_t> fillvertices;
    std::vector<float> next_fill_points;
    std::vector<float> fill_color_array;

    // contains sub graph of one graph
    std::vector<GraphObject *> subGraphObjects;
    std::vector<float> updateStartTime;
    std::vector<float> updateEndTime;

protected:
    void update(float dt) override;
    void updateStroke(float dt) override;
    void updateFill(float dt) override;

    void applyUpdaterFunction(float dt) override;

    void updateStrokePoints() override;
    void updateFillPoints() override;
    void updatePoints();

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

    void add(GraphObject *sub_object)
    {
        sub_object->setColor(this->colors);
        subGraphObjects.push_back(sub_object);
    }

    void interpolate(int) override;

    void generatePoints(glm::vec3 (*func)(float, Var), Var v);
    void UpdateGraphWithFunction(float);
    void setUpdater(glm::vec3 (*updater_function)(float, float), float s_t, float d)
    {
        this->updater_func = updater_function;
        this->updateStartTime.push_back(s_t);
        this->updateEndTime.push_back(s_t + d );
    }
    void applyColorToVertex();

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
};

#endif