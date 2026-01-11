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

    /*
            Data Fields for the graph
    */
public:
    Shader *stroke_shader, *fill_shader;
    glm::mat4 baseModel = glm::mat4(1.0f);
    glm::vec3 translate = glm::vec3(0, 0, 0);
    glm::vec3 scale_factor = glm::vec3(1.0f, 1.0f, 1.0f);
    glm::mat4 model = glm::mat4(1.0f), view, projection;
    float phi = 0, theta = 0;
    glm::mat4 scale = glm::mat4(1.0f);
    float startTime = 0, duration = 1.0f;
    //in base class this is stroke_line_width
    float line_width = 1.5f;
    //specific to this class
    std::pair<float, float> range = {-1, 1};
    int resolution = 500;

    float scale_x = 1, scale_y = 1, scale_z = 1;
    glm::vec3 rotation_vec = glm::vec3(0, 0, 0), rotation_pivot = glm::vec3(0, 0, 0);

    static int count;

public:
    float sub_obj_show_interval = 0.1f;
    bool sub_sync = true;

public:
    bool i_initialized = false;

public:
    int drawStart = 0, drawSize = 0;
    //in base class these are stroke_progress, fill_progress, fill_opacity
    float progress = 1.0f;
    float fillProgress = 1.0f;
    float fillOpacity = 1.0f;

    //this is not in base class
    GraphColor *color = new GraphColor();

    //specific to this class
    GLuint StrokeVAO, StrokeVBO;
    GLuint FillVAO, FillVBO;

    bool stroke_data_initialized = false;
    bool fill_data_initialized = false;

    glm::vec3 *(func)(float, Var);
    glm::vec3 (*updater_func)(float, float) = nullptr;

public:
    std::string vertexShaderPath = "./shaders/quadratic_shaders/vertex.vs";
    std::string fragmentShaderPath = "./shaders/quadratic_shaders/fragment.fs";
    std::string geometricShaderPath = "./shaders/quadratic_shaders/geometry.gs";

    std::string fillVertexShaderPath = "./shaders/quadratic_fill/vertex.vs";
    std::string fillFragmentShaderPath = "./shaders/quadratic_fill/fragment.fs";
    std::string fillGeometricShaderPath = "./shaders/quadratic_fill/geometry.gs";
    // bounding box of the graph
    float x, y, width, height;

    // factor to scale and rotate the graph as a whole model

    std::vector<glm::vec3> points;
    std::vector<GraphColor> colors;
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
    /*
            Function Fields for the graph
    */

private:
protected:
    void update(float dt);
    void updateStroke(float dt);
    void updateFill(float dt);


    void updatePoints(float dt);

    void updateStrokePoints();
    void updateFillPoints();

    void setDimension()
    {
    }

    void moveTo()
    {
    }
    
    void nextTo(GraphObject *target)
    {
    }
    
    public:
    void synchronizeSubObjects(float s_time = -1.0f, float r_time = -1.0f, float delay = 0.0f)
    {
        if (s_time == -1.0f)
        {
            s_time = this->startTime + this->duration;
        }
        if (r_time == -1.0f)
        {
            r_time = this->duration;
        }
        for (GraphObject *subObj : subGraphObjects)
        {
            subObj->setStartTime(s_time);
            subObj->setDuration(r_time);
            s_time += r_time + delay;
        }
    }
    void updatePoints();

public:
    GraphObject();
    ~GraphObject() {}

    void Init(float dt = 0) override;

    void InitStrokeData();
    void InitFillData();

    void
    InitSelf(float dt);
    void InitSubObject(float dt);

    void setStrokeData();
    void setFillData(char axis = 'y');

    void initializeStrokeShader();
    void initializeFillShader();

    void uploadStrokeDataToShader();
    void uploadFillDataToShader();

    void setStartTime(float s_time = 0.0f)
    {
        this->startTime = s_time;
    }
    void setDuration(float duration = 1.0f)
    {
        this->duration = duration;
    }

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

    void interpolate(int number);

    void prepareSubObjects(float start_time, float duration);

    // void generatePoints(glm::vec2 (*func)(float, Var), Var v);
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

    void setFillProgress(float prog){
        this->fillProgress = prog * 1.0f;
    }

    int getFillSize(){
        return current_fill_points.size();
    }

    bool isEqual(glm::vec3 a, glm::vec3 b)
    {
        return a[0] == b[0] && a[1] == b[1] && a[2] == b[2];
    }

    void setDrawStart(int index)
    {
        drawStart = index;
    }
    void setDrawSize(int size)
    {
        drawSize = size;
    }

    void setProgress(float prog)
    {
        this->progress = prog * 1.0f;
    }

    void setBaseModel(glm::mat4 base)
    {
        baseModel = base;
    }

    void setScale(glm::vec3 scale_factor)
    {
        this->scale_factor = scale_factor;
        this->scale_x = scale_factor.x;
        this->scale_y = scale_factor.y;
        this->scale_z = scale_factor.z;
    }

    void setLineWidth(float width){
        line_width = width;
    }

    void setTranslate(glm::vec3);

    // transition operation
    void moveTo(Position pos = Position::NONE);
    void moveTo(glm::vec3 translate);

    //transition scaling
    void scaleTo(float scale_ratio);
    void scaleTo(float x_scale, float y_scale);

    void setRotation(glm::vec3 rot_amount, glm::vec3 rot_pivot);

    //transition rotating
    void rotate(glm::vec3 rotation_amount = glm::vec3(45, 0, 0), glm::vec3 pivot = glm::vec3(0, 0, 0));

    void fit(float width, float height, float buff = 0.0f);
};

#endif