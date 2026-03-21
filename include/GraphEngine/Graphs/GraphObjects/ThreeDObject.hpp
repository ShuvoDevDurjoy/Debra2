#ifndef __THREEDOBJECT_HPP__
#define __THREEDOBJECT_HPP__

#include "GraphObject.hpp"
#include "../../Core/GraphApp.hpp"

#include "../../Math/Var.hpp"


class ThreeDObject: public GraphMathObject{
    protected:
        std::string vertexShaderPath = "./shaders/threed_shaders/stroke_vertex.vs";
        std::string fragmentShaderPath = "./shaders/threed_shaders/stroke_fragment.fs";
        std::string geometricShaderPath = "./shaders/threed_shaders/stroke_geometry.gs";

        std::string fillVertexShaderPath = "./shaders/threed_shaders/fill_vertex.vs";
        std::string fillFragmentShaderPath = "./shaders/threed_shaders/fill_fragment.fs";
        std::string fillGeometricShaderPath = "./shaders/threed_shaders/fill_geometry.gs";

        std::pair<float, float> range = {-1, 1};

        GLuint StrokeVAO = 0, StrokeVBO = 0, StrokeEBO = 0;
        GLuint FillVAO = 0, FillVBO = 0, FillEBO = 0;
        GLuint NormalsVBO = 0;

        std::vector<uint32_t> stroke_indices;
        std::vector<uint32_t> fill_indices;
        std::vector<glm::vec3> normals;

        std::vector<ThreeDObject *> sub_graphs;
        std::vector<void (*)(ThreeDObject *, float)> updaters;
        std::vector<float> updateStartTime;
        std::vector<float> updateEndTime;

        std::vector<glm::vec3> m_original_points;
        std::vector<glm::vec3> m_original_normals;

        // bool showFill = true;

    public:
        enum class GenerationDirection { ALONG_THETA, ALONG_RHO };
        GenerationDirection generation_dir = GenerationDirection::ALONG_THETA;

        std::vector<std::pair<int, int>> sub_surface_ranges;

        glm::vec3 (*graph_func)(float, float, Var) = nullptr;
        Var graph_var;
        std::pair<float, float> rho_range = {0, 1};
        std::pair<float, float> theta_range = {0, 2.0f * M_PI};
        std::pair<float, float> resolution = {32, 32};
        bool showGrid = true;
        float gloss = 0.2f;
        float shadow = 0.2f;

        bool is_periodic_rho = false;
        bool is_periodic_theta = true;

    public:
        ThreeDObject() {}
        ~ThreeDObject() {}

        void Init() override;

        void InitStrokeData() override;
        void InitFillData() override;

        void setStrokeData() override;
        void setFillData() override;

        void initializeStrokeShader() override;
        void initializeFillShader() override;

        void uploadStrokeDataToShader() override;
        void uploadFillDataToShader() override;

        void interpolate(const GraphMathObject*, float t) override;
        void become(GraphMathObject *target) override;

        void setUpdater(void (*updater_function)(ThreeDObject *, float), float s_t, float d)
        {
            this->updateStartTime.push_back(s_t);
            this->updateEndTime.push_back(d == -1.0f ? d : s_t + d);
            this->updaters.push_back(updater_function);
        }

        void draw(float dt)
        {
            update(dt);
        };

    void setPoints(std::vector<glm::vec3> points);

    void setFillProgress(float progress);

    void addPoints(glm::vec3 point);

    void generatePoints() override;

    int getSize();

    int getFillsize(){
        return (int)fill_indices.size();
    }

    int getStrokeSize(){
        return (int)stroke_indices.size();
    }

    void update(float dt) override;
    void drawStroke(float dt) override;
    void drawFill(float dt) override;
    void applyUpdaterFunction(float dt) override;
    void updateStrokePoints() override;
    void updateFillPoints() override;

    void updatePoints();

    void setShowFill(bool show_fill){
        showFill = show_fill;
    }

    void start_bezier_path(glm::vec3 start_point) override {}
    void add_cubic_bezier_curve_to(glm::vec3 control1, glm::vec3 control2, glm::vec3 end_anchor) override {}
    void add_quadratic_bezier_curve_to(glm::vec3 control, glm::vec3 end_anchor) override {}
    void add_line_to(glm::vec3 end_anchor) override {}
    void build_points_from_bezier() override {}
    void subdivide_bezier_curves() override {}
    // void add(GraphMathObject *) override {}
    void alignPoints(GraphMathObject*) override;
    void applyColorToVertex() override {}
};

#endif