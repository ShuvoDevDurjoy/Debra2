#ifndef __THREEDOBJECT_HPP__
#define __THREEDOBJECT_HPP__

#include "GraphObject.hpp"
#include "../../Core/GraphApp.hpp"

#include "../../Math/Var.hpp"

class ThreeDObject: public GraphObject{
    private:
        std::string vertexShaderPath = "./shaders/quadratic_shaders/vertex.vs";
        std::string fragmentShaderPath = "./shaders/quadratic_shaders/fragment.fs";
        std::string geometricShaderPath = "./shaders/quadratic_shaders/geometry.gs";

        std::string fillVertexShaderPath = "./shaders/surface_shaders/vertex.vs";
        std::string fillFragmentShaderPath = "./shaders/surface_shaders/fragment.fs";
        std::string fillGeometricShaderPath = "./shaders/surface_shaders/geometry.gs";

        Shader *stroke_shader, *fill_shader;

        glm::mat4 baseModel = glm::mat4(1.0f);
        glm::vec3 translate = glm::vec3(0, 0, 0);
        glm::vec3 scale_factor = glm::vec3(1.0f, 1.0f, 1.0f);
        glm::mat4 model = glm::mat4(1.0f), view, projection;
        float phi = 0, theta = 0;
        glm::mat4 scale = glm::mat4(1.0f);
        float startTime = 0, duration = 1.0f;
        float line_width = 1.5f;
        std::pair<float, float> range = {-1, 1};

        float scale_x = 1, scale_y = 1, scale_z = 1;
        glm::vec3 rotation_vec = glm::vec3(0, 0, 0), rotation_pivot = glm::vec3(0, 0, 0);

        float sub_obj_show_interval = 0.1f;
        bool sub_sync = true;

        bool is_initialized = false;

        float stroke_progress;
        float fill_progress = 1;
        float fill_Opacity = 1;

        GLuint StrokeVAO, StrokeVBO;
        GLuint FillVAO, FillVBO;

        float x, y, width, height;

        std::vector<glm::vec3> points;

        std::vector<glm::vec3> stroke_points;
        std::vector<glm::vec3> fill_points;

        std::vector<glm::vec3> stroke_color_array;
        std::vector<glm::vec3> fill_color_array;

        std::vector<ThreeDObject *> sub_graphs;
public:
    glm::vec3 (*graph_func)(float, float, Var) = nullptr;
    Var graph_var;
    std::pair<float, float> r_range = {0, 1};
    std::pair<float, float> t_range = {0, 2.0f * M_PI};
    std::pair<float, float> resolution = {32, 32};

public:
    ThreeDObject() {}
    ~ThreeDObject() {}

    void Init(float dt = 0) override;

    void draw(float dt) override;

    void drawFill(float dt);


    void InitStrokeData();
    void InitFillData();

    void InitializeFillShader();
    void UploadFillShaderData();

    void setPoints(std::vector<glm::vec3> points);

    void setStrokeData();
    void setFillData();

    void setFillProgress(float progress);
    void setColorToFillVertices();

    void addPoints(glm::vec3 point);

    void generatePoints();

    int getSize();

    int getFillsize(){
        return (int)fill_points.size();
    }

    void updatePoints();
};

#endif