#ifndef __THREEDOBJECT_HPP__
#define __THREEDOBJECT_HPP__

#include "GraphObject.hpp"
#include "../../Core/GraphApp.hpp"

#include "../../Math/Var.hpp"

class ThreeDObject: public GraphMathObject{
    private:
        std::string vertexShaderPath = "./shaders/quadratic_shaders/vertex.vs";
        std::string fragmentShaderPath = "./shaders/quadratic_shaders/fragment.fs";
        std::string geometricShaderPath = "./shaders/quadratic_shaders/geometry.gs";

        std::string fillVertexShaderPath = "./shaders/surface_shaders/vertex.vs";
        std::string fillFragmentShaderPath = "./shaders/surface_shaders/fragment.fs";
        std::string fillGeometricShaderPath = "./shaders/surface_shaders/geometry.gs";

        std::pair<float, float> range = {-1, 1};

        GLuint StrokeVAO, StrokeVBO;
        GLuint FillVAO, FillVBO;

        std::vector<glm::vec3> stroke_points;
        std::vector<glm::vec3> stroke_prev_points;
        std::vector<glm::vec3> stroke_current_points;
        std::vector<glm::vec3> stroke_next_points;
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

    void InitStrokeData() override;
    void InitFillData() override;

    void setStrokeData() override;
    void setFillData() override;

    void initializeStrokeShader() override;
    void initializeFillShader() override;

    void uploadStrokeDataToShader() override;
    void uploadFillDataToShader() override;

    void interpolate(int) override;

    void draw(float dt) {
        update(dt);
    };

    void setPoints(std::vector<glm::vec3> points);

    void setFillProgress(float progress);
    void setColorToFillVertices();

    void addPoints(glm::vec3 point);

    void generatePoints();

    int getSize();

    int getFillsize(){
        return (int)fill_points.size();
    }

    int getStrokeSize(){
        return (int)stroke_points.size();
    }

    void update(float dt) override;
    void updateStroke(float dt) override;
    void updateFill(float dt) override;
    void applyUpdaterFunction(float dt) override;
    void updateStrokePoints() override;
    void updateFillPoints() override;

    void updatePoints();
};

#endif