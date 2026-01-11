#include "../../../include/GraphEngine/Graphs/GraphObjects/ThreeDObject.hpp"

void ThreeDObject::Init(float dt)
{

    // this->stroke_shader = new Shader(vertexShaderPath, geometricShaderPath, fragmentShaderPath);
    this->fill_shader = new Shader(fillVertexShaderPath, fillGeometricShaderPath, fillFragmentShaderPath);
    // fill_color_array = {glm::vec3(1, 0, 1), glm::vec3(1, 1, 0)};
    fill_color_array = {glm::vec3(1, 0, 1), glm::vec3(1, 1, 0), glm::vec3(1, 0, 1), glm::vec3(1, 1, 0), glm::vec3(1, 0, 1)};

    fill_progress = 1;
    std::cout << "Graph point size while initialization is: " << getSize() << std::endl;
    if (!getSize())
    {
        generatePoints();
        std::cout << "here" << std::endl;
    }

    if (!getSize())
    {
        glGenVertexArrays(1, &FillVAO);
        glGenBuffers(1, &FillVBO);
        std::cerr << "No points in the graph to initialize" << std::endl;
    }
    else
    {
        // Initialize OpenGL Buffer and Array
        glGenVertexArrays(1, &StrokeVAO);
        glGenBuffers(1, &StrokeVBO);

        // initialize the stroke data
        // InitStrokeData();

        glGenVertexArrays(1, &FillVAO);
        glGenBuffers(1, &FillVBO);
        // initialize the fill data
        std::cout << "This is here" << std::endl;
        InitFillData();
    }

    is_initialized = true;
}

void ThreeDObject::setPoints(std::vector<glm::vec3> points)
{
    this->points = points;
}

void ThreeDObject::generatePoints()
{
    if (graph_func == nullptr)
    {
        std::cout << "This is here in the graph function checkf" << std::endl;
        return;
    }
    float increment_r = (r_range.second - r_range.first) / (resolution.first - 1);
    float increment_t = (t_range.second - t_range.first) / (resolution.second - 1);
    float t = 0, r = 0;
    for (int i = 0; i < resolution.first; ++i)
    {
        r = r_range.first + i * increment_r;
        t = 0;
        for (int j = 0; j < resolution.second; ++j)
        {
            t = t_range.first + j * increment_t;
            glm::vec3 point = graph_func(r, t, graph_var);
            points.push_back(point);
        }
    }

    std::cout << "point size is: " << points.size() << std::endl;
}

void ThreeDObject::InitStrokeData() {}
void ThreeDObject::InitFillData()
{
    setFillData();
    glBindVertexArray(FillVAO);
    glBindBuffer(GL_ARRAY_BUFFER, FillVBO);
    InitializeFillShader();
    UploadFillShaderData();
}

void ThreeDObject::InitializeFillShader()
{
    glBindVertexArray(FillVAO);
    glBindBuffer(GL_ARRAY_BUFFER, FillVBO);
    int buffer_size = getFillsize() * sizeof(glm::vec3) + fill_color_array.size() * sizeof(glm::vec3);
    glBufferData(GL_ARRAY_BUFFER, buffer_size, nullptr, GL_DYNAMIC_DRAW);
}

void ThreeDObject::UploadFillShaderData()
{
    size_t buffer_size = getFillsize() * sizeof(glm::vec3);
    glBindVertexArray(FillVAO);
    glBindBuffer(GL_ARRAY_BUFFER, FillVBO);
    glBufferSubData(GL_ARRAY_BUFFER, 0, buffer_size, fill_points.data());
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void *)0);
    glEnableVertexAttribArray(0);
    glBufferSubData(GL_ARRAY_BUFFER, buffer_size, fill_color_array.size() * sizeof(glm::vec3), fill_color_array.data());
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void *)(buffer_size));
    glEnableVertexAttribArray(1);
}

void ThreeDObject::setStrokeData()
{
}

// void ThreeDObject::setFillData()
// {
//     if (getSize() != (resolution.first * resolution.second))
//         return;

//     std::vector<glm::vec3> color_palette = fill_color_array;
//     fill_color_array.clear();

//     int num_colors = color_palette.size();
//     // Calculate how many grid steps per color segment
//     float segments = static_cast<float>(resolution.first - 1) / (num_colors - 1);

//     for (int i = 0; i < resolution.first - 1; i++)
//     {
//         // Find which two colors in the palette we are between
//         int color_idx = static_cast<int>(i / segments);
//         float t_a_lin = (i - (color_idx * segments)) / segments;
//         float t_b_lin = ((i + 1) - (color_idx * segments)) / segments;

//         float t_a = (1.0f - cos(t_a_lin * 3.1415927f)) * 0.5f;
//         float t_b = (1.0f - cos(t_b_lin * 3.1415927f)) * 0.5f;

//         // colora is the exact color for index i, colorb for index i+1
//         glm::vec3 colora = glm::mix(color_palette[color_idx], color_palette[color_idx + 1], t_a);
//         glm::vec3 colorb = glm::mix(color_palette[color_idx], color_palette[color_idx + 1], t_b);

//         for (int j = 0; j < resolution.second - 1; j++)
//         {
//             glm::vec3 p00 = points[i * resolution.second + j];
//             glm::vec3 p10 = points[(i + 1) * resolution.second + j];
//             glm::vec3 p01 = points[i * resolution.second + (j + 1)];
//             glm::vec3 p11 = points[(i + 1) * resolution.second + (j + 1)];

//             // Triangle 1: p00, p01, p10
//             fill_points.push_back(p00);
//             fill_color_array.push_back(colora);
//             fill_points.push_back(p10);
//             fill_color_array.push_back(colora); // Fix: use colorb
//             fill_points.push_back(p01);
//             fill_color_array.push_back(colorb); // Fix: use colora

//             // Triangle 2: p10, p01, p11
//             fill_points.push_back(p01);
//             fill_color_array.push_back(colorb); // Fix: use colora
//             fill_points.push_back(p10);
//             fill_color_array.push_back(colora); // Fix: use colorb
//             fill_points.push_back(p11);
//             fill_color_array.push_back(colorb);
//         }
//     }
// }

void ThreeDObject::setFillData()
{
    if (getSize() != (resolution.first * resolution.second))
        return;

    std::vector<glm::vec3> color_palette = fill_color_array;
    fill_color_array.clear();

    int num_colors = color_palette.size();
    // Calculate how many grid steps per color segment
    float segments = static_cast<float>(resolution.second - 1) / (num_colors - 1);

    for (int i = 0; i < resolution.second - 1; i++)
    {
        // Find which two colors in the palette we are between
        int color_idx = static_cast<int>(i / segments);
        float t_a_lin = (i - (color_idx * segments)) / segments;
        float t_b_lin = ((i + 1) - (color_idx * segments)) / segments;

        float t_a = (1.0f - cos(t_a_lin * 3.1415927f)) * 0.5f;
        float t_b = (1.0f - cos(t_b_lin * 3.1415927f)) * 0.5f;

        // colora is the exact color for index i, colorb for index i+1
        glm::vec3 colora = glm::mix(color_palette[color_idx], color_palette[color_idx + 1], t_a);
        glm::vec3 colorb = glm::mix(color_palette[color_idx], color_palette[color_idx + 1], t_b);

        for (int j = 0; j < resolution.first - 1; j++)
        {
            glm::vec3 p00 = points[j * resolution.second + i];
            glm::vec3 p10 = points[(j + 1) * resolution.second + i];
            glm::vec3 p01 = points[j * resolution.second + (i + 1)];
            glm::vec3 p11 = points[(j + 1) * resolution.second + (i + 1)];

            // Triangle 1: p00, p01, p10
            fill_points.push_back(p10);
            fill_color_array.push_back(colora);
            fill_points.push_back(p00);
            fill_color_array.push_back(colora); // Fix: use colorb
            fill_points.push_back(p01);
            fill_color_array.push_back(colorb); // Fix: use colora

            // Triangle 2: p10, p01, p11
            fill_points.push_back(p10);
            fill_color_array.push_back(colora); // Fix: use colora
            fill_points.push_back(p11);
            fill_color_array.push_back(colorb); // Fix: use colorb
            fill_points.push_back(p01);
            fill_color_array.push_back(colorb);
        }
    }

    std::cout << "fill size is: " << (getFillsize() / 3) << std::endl;
    std::cout << "resolution that is being drawn, size is: " << (resolution.first - 1) * 2 << std::endl;
}

void ThreeDObject::draw(float dt)
{
    drawFill(dt);
}

void ThreeDObject::drawFill(float dt)
{
    // --- Save previous depth state ---
    GLboolean depthWasEnabled = glIsEnabled(GL_DEPTH_TEST);

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    fill_shader->use();

    projection = GraphApp::projection;
    view = GraphApp::view;

    model = glm::mat4(1.0f);
    model = glm::rotate(model, glm::radians(GraphApp::rotX), glm::vec3(1, 0, 0));
    model = glm::rotate(model, glm::radians(GraphApp::rotY), glm::vec3(0, 1, 0));

    fill_shader->setMat4("model", model);
    fill_shader->setMat4("view", view);
    fill_shader->setMat4("projection", projection);

    fill_shader->setFloat("u_progress", fillProgress);
    fill_shader->setInt("triangle_count", getFillsize() / 3);

    fill_shader->setInt("vertices_increment_count", ((resolution.first - 1) * 2));

    glm::vec3 cameraPos(0.0f, 0.0f, 100.0f);

    fill_shader->setVec3("objectColor", 1, 0.67, 0.67);
    fill_shader->setVec3("lightPos", cameraPos);
    fill_shader->setVec3("viewPos", cameraPos);
    fill_shader->setVec3("lightColor", 0.5, 0.5, 0.5);

    glBindVertexArray(FillVAO);
    glDrawArrays(GL_TRIANGLES, 0, getFillsize());

    // --- Restore previous depth state ---
    if (!depthWasEnabled)
        glDisable(GL_DEPTH_TEST);
}

int ThreeDObject::getSize()
{
    return (int)points.size();
}

void ThreeDObject::addPoints(glm::vec3 point)
{
    points.push_back(point);
}

void ThreeDObject::setFillProgress(float progress)
{
    std::cout << "This is setting up fill progress" << std::endl;
    fillProgress = progress;
}

void ThreeDObject::updatePoints() {}

void ThreeDObject::setColorToFillVertices()
{
}