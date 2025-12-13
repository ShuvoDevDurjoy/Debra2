#include "../../include/GraphEngine/Graphs/Graph.hpp"
#include <cmath>

Graph *Graph::instance = nullptr;

namespace GraphUtilities
{
    float DEFAULT_FRAG = 1.0f;
    float DEFAULT_YRATIO = 1.0f;
    float DEFAULT_PIXEL_PER_UNIT = 10.0f;
    float DEFAULT_PAN_Y_OFFSET = 0.0f;
    float DEFAULT_PAN_X_OFFSET = 0.0f;
    float MAX_PIXEL_PER_UNIT = 400.0f;
    float MIN_PIXEL_PER_UNIT = 50.0f;
    float PIXEL_INCREMENT_STEP = 5.0f;
    float DEFAULT_PAN_INCREMENT_STEP = 1.0f;
    int DEFAULT_STEPS = 10.0f;

    float genSin(float x)
    {
        float radians = x * M_PI / 2.0f;
        return sin(radians);
    }
    float genCos(float x)
    {
        float radians = x * M_PI / 2.0f;
        return cos(radians);
    }
    float genTan(float x)
    {
        float radians = x * M_PI / 2.0f;
        return tan(radians);
    }
    float genCosec(float x)
    {
        float radians = x * M_PI / 2.0f;
        return (1.0f / sin(radians));
    }
    float genSec(float x)
    {
        float radians = x * M_PI / 2.0f;
        return (1.0f / cos(radians));
    }
    float genCot(float x)
    {
        float radians = x * M_PI / 2.0f;
        return (1.0f / tan(radians));
    }

    float toRadians(float degree)
    {
        return (degree * M_PI) / 180.0f;
    }

};

Graph::Graph(float s)
{
    app = new GraphApp();
    this->window = app->getWindow();
    this->shader = app->getShader();

    glfwGetWindowSize(window, &windowWidth, &windowHeight);

    this->prevHeight = windowHeight;
    this->prevWidth = windowWidth;

    this->stepsx = windowWidth / (s * 1.0f);
    this->stepsy = windowHeight / (s * 1.0f);

    this->unitX = normalizeX(1.0f);
    this->unitY = normalizeY(1.0f);
    this->speed = s <= 0.0f ? 0 : s * 60.0f;

    initBox();
    generateGrid();
    glfwSetFramebufferSizeCallback(window, frame_size_buffer_callback);

    (GraphApp::keyManager)->registerListener(this);
}

void Graph::frame_size_buffer_callback(GLFWwindow *window, int width, int height)
{
    glViewport(0, 0, width, height);
    instance->windowWidth = width;
    instance->windowHeight = height;
}

void Graph::initBox()
{
    boxVerticess.setColor(new GraphColor(0.40f, 0.40f, 0.70f));
    boxVerticess.add({-1.0f, -1.0f});
    boxVerticess.add({1.0f, -1.0f});
    boxVerticess.add({1.0f, -1.0f});
    boxVerticess.add({1.0f, 1.0f});
    boxVerticess.add({1.0f, 1.0f});
    boxVerticess.add({-1.0f, 1.0f});
    boxVerticess.add({-1.0f, 1.0f});
    boxVerticess.add({-1.0f, -1.0f});
    boxVerticess.setRangeSize(boxVerticess.getSize());
}

void Graph::toggleFullscreen()
{
    if (!window)
        return;

    GLFWmonitor *monitor = glfwGetPrimaryMonitor();
    const GLFWvidmode *mode = glfwGetVideoMode(monitor);

    if (!isFullscreen)
    {

        glfwGetWindowPos(window, &prevPosX, &prevPosY);
        prevPosY -= 10.0f;

        glfwGetFramebufferSize(window, &prevWidth, &prevHeight);

        glfwSetWindowMonitor(window, monitor, 0, 0, mode->width, mode->height, mode->refreshRate);
        isFullscreen = true;

        glfwPollEvents();

        glfwGetFramebufferSize(window, &windowWidth, &windowHeight);
    }
    else
    {

        glfwSetWindowMonitor(window, nullptr, prevPosX, prevPosY, prevWidth, prevHeight, 0);
        isFullscreen = false;

        windowWidth = prevWidth;
        windowHeight = prevHeight;
    }

    glViewport(0, 0, windowWidth, windowHeight);
}

inline float Graph::maxPanXX() const
{
    //  (steps/2) * (scale-1)/scale   →  0 when scale==1, grows as you zoom‑in
    return (stepsx * 0.5f) * (scale - 1.0f) / scale;
}

inline float Graph::maxPanYY() const
{
    // Steps in Y direction are derived from visible vertical units,
    // which depend on aspect ratio and pixel_per_unit.
    // int visibleYUnits = windowHeight / (2.0f * GraphUtilities::MIN_PIXEL_PER_UNIT);
    return (stepsy * 0.5f) * (scale - 1.0f) / scale;
}

void Graph::onKeyPressedOnceCallback(const KeyEvent &event)
{
    float maxPanX = (stepsx / 2.0f) * (scale - 1.0f);

    float maxPanY = (stepsy / 2.0f) * (scale - 1.0f);

    std::cout << "event: " << event.key << std::endl;

    switch (event.key)
    {
    case GLFW_KEY_HOME:
        scale = 1.0f;
        panOffsetX = GraphUtilities::DEFAULT_PAN_X_OFFSET;
        panOffsetY = GraphUtilities::DEFAULT_PAN_Y_OFFSET;
        pixel_per_unit = GraphUtilities::DEFAULT_PIXEL_PER_UNIT;
        break;
    case GLFW_KEY_EQUAL:
        if (scale < 50.0f)
        {
            scale += 0.25;
            float maxpanx = maxPanXX();
            panOffsetX = std::clamp(panOffsetX, -maxpanx, maxpanx);
        }
        break;
    case GLFW_KEY_MINUS:
        if (scale > 1.0f)
        {
            scale -= 0.25;
            float maxpanx = maxPanXX();
            panOffsetX = std::clamp(panOffsetX, -maxpanx, maxpanx);
        }
        break;
    case GLFW_KEY_F11:
    {
        toggleFullscreen();
    }
    break;
    case GLFW_KEY_LEFT:
        if ((panOffsetX * scale) > -maxPanX)
            panOffsetX -= GraphUtilities::DEFAULT_PAN_INCREMENT_STEP;
        break;

    case GLFW_KEY_RIGHT:
        if ((panOffsetX * scale) < maxPanX)
            panOffsetX += GraphUtilities::DEFAULT_PAN_INCREMENT_STEP;
        break;

    case GLFW_KEY_UP:
        if ((panOffsetY * scale) < maxPanY)
        {
            panOffsetY += GraphUtilities::DEFAULT_PAN_INCREMENT_STEP;
        }
        break;

    case GLFW_KEY_DOWN:
        if ((panOffsetY * scale) > -maxPanY)
        {
            panOffsetY -= GraphUtilities::DEFAULT_PAN_INCREMENT_STEP;
        }
        break;

    default:
        break;
    }
}

void Graph::generateGrid()
{

    gridLines.setColor(new GraphColor(0.12f, 0.13f, 0.25f));
    for (int x = -stepsx / 2; x <= stepsx / 2; x += 1)
    {
        gridLines.add({normalizeX(x), 1.0f});
        gridLines.add({normalizeX(x), -1.0f});
    }
    for (int y = -stepsy / 2; y <= stepsy / 2; y += 1)
    {
        gridLines.add({-1.0f, normalizeY(y)});
        gridLines.add({1.0f, normalizeY(y)});
    }
    gridLines.setRangeSize(gridLines.getSize());
}

float Graph::normalizeX(float x)
{
    return (x * 2.0f) / (stepsx * 1.0f);
}

float Graph::normalizeY(float y)
{
    return (y * 2.0f) / (stepsy * 1.0f);
}

void Graph::drawBox()
{
    GraphColor *gc = boxVerticess.getColor();
    app->setColor(gc->RED, gc->GREEN, gc->BLUE);
    app->refreshOpenGL(boxVerticess.points, 0, boxVerticess.getSize());
    glLineWidth(6.0f);
    glDrawArrays(GL_LINES, 0, boxVerticess.getSize() / 2);
}

void Graph::drawGrid()
{
    std::vector<float> axis = {
        -1.0f, normalizeY(0.0f),
        1.0f, normalizeY(0.0f),
        normalizeX(0.0f), -1.0f,
        normalizeX(0.0f), 1.0f};

    shader->setVec2("position", scale, scale);
    shader->setVec2("translate", 0.0f, 0.0f);
    app->refreshOpenGL(gridLines.points, 0, gridLines.getSize());
    glLineWidth(2.0f);
    glDrawArrays(GL_LINES, 0, gridLines.getSize() / 2);

    shader->setVec2("position", 1.0f, 1.0f);
    shader->setVec2("translate", 0.0f, 0.0f);
    drawAxis();

    shader->setVec2("position", scale, scale);
    shader->setVec2("translate", panOffsetX * scale * unitX, panOffsetY * scale * unitY);

    app->refreshOpenGL(axis, 0, axis.size());
    glLineWidth(3.0f);
    app->setColor(0.30f, 0.34f, 0.50f);
    glDrawArrays(GL_LINES, 0, axis.size() / 2);
}

void Graph::drawAxis()
{
    std::vector<float> axisfixed = {
        -1.0f, 0.0f,
        1.0f, 0.0f,
        0.0f, -1.0f,
        0.0f, 1.0f};

    app->refreshOpenGL(axisfixed, 0, axisfixed.size());
    glLineWidth(3.0f);
    app->setColor(0.6f, 0.34f, 0.50f);
    glDrawArrays(GL_LINES, 0, axisfixed.size() / 2);
}

void Graph::drawLines(std::vector<std::pair<float, float>> vertices, GraphColor *graph_color)
{
    GraphColor *gc = graph_color;
    singletonGraph* graph = new singletonGraph(shader, getStart(), getDuration(), getDelay(), getLoopTime(), getMorphDuration());
    graph->setColor(gc);
    graph->setAnimationMode(Graph::ANIMATION_MODE);
    int vertices_size = vertices.size();
    float x = Graph::normalizeX(vertices[0].first);
    float y = Graph::normalizeY(vertices[0].second);
    graph->add({x, y});
    float steps = 0.01f;
    for (int i = 1; i < vertices_size; ++i)
    {
        std::pair<float, float> previous = vertices[i - 1];
        std::pair<float, float> current = vertices[i];

        float slope = (previous.first - current.first) == 0 ? 1.0f : ((previous.second - current.second) / (previous.first - current.first));
        steps = (abs(current.first - previous.first)) / 100.0f;
        steps = steps == 0 ? 1.0f : steps;
        float c = 0.0f;
        if (previous.first < current.first)
        {
            if (current.first < 0.0f)
            {
                c = (-1.0f * current.first * slope) + current.second;
            }
            else
            {
                c = (-1.0f * previous.first * slope) + previous.second;
            }

            for (float a = previous.first; a <= current.first; a = round((a + steps) * 100.0f) / 100.0f)
            {
                float x = normalizeX(a);
                float y = normalizeY(slope * a) + normalizeY(c);
                graph->add({x, y});
            }
        }
        else
        {
            if (previous.first < 0.0f)
            {
                c = (-1.0f * previous.first * slope) + previous.second;
            }
            else
            {
                c = (-1.0f * current.first * slope) + current.second;
            }

            for (float a = previous.first; a >= current.first; a = round((a - steps) * 100.0f) / 100.0f)
            {
                float x = normalizeX(a);
                float y = normalizeY(slope * a) + normalizeY(c);
                graph->add({x, y});
            }
        }
    }
    // int cr = graph.getSize() / (getDuration() == 0 ? 60 : (getDuration() - 60 == 0 ? 60 : getDuration() - 60));
    // cr = cr < 1 ? 1 : cr;
    // graph.setRangeSize(cr);
    graphs.push_back(graph);
}

void Graph::drawPoints(float x, float y, float radius, GraphColor *g_color)
{
    GraphColor *gc = g_color;
    singletonGraph* graph = new singletonGraph(shader, getStart(), getDuration(), getDelay(), getLoopTime(), getMorphDuration());
    graph->setColor(gc);
    graph->setAnimationMode(Graph::ANIMATION_MODE);
    float step = GraphUtilities::toRadians(1.0f);
    float centerX = normalizeX(x);
    float centerY = normalizeY(y);
    float to = round(2.0f * M_PI * 100.0f) / 100.0f;

    float prevI = -1.0f;
    for (float i = 0.0f; i <= to && i != prevI; i = round((i + step) * 1000.0f) / 1000.0f)
    {
        graph->add({centerX, centerY});
        float posX = normalizeX(x + radius * cos(i));
        float posY = normalizeY(y + radius * sin(i));
        graph->add({posX, posY});
        prevI = i;
    }
    // int radianSteps = graph.getSize() / (getDuration() == 0 ? 60 : (getDuration() - 60 == 0 ? 60 : getDuration() - 60));
    // radianSteps = radianSteps < 1 ? 1 : radianSteps;
    // graph.setRangeSize(radianSteps);
    graphs.push_back(graph);
}

// void Graph::draw(int tick)
// {
//     glEnable(GL_BLEND);
//     glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

//     app->setColor(0.12f, 0.13f, 0.25f);
//     drawGrid();

//     shader->setVec2("position", scale, scale);
//     shader->setVec2("translate", panOffsetX * scale * unitX, panOffsetY * scale * unitY);

//     for (auto &g : graphs)
//     {
//         if (tick < g.getStartTime())
//         continue;
//         if(g.getRangeSize() == 0)
//         continue;

//         int localTick = (tick - g.getStartTime()) % (int)(g.getTotalDuration()) + 1;
//         int drawEnd = g.getDuration() + 1;
//         int morphEnd = g.getDuration() + g.getMorphDuration() + 1;

//         std::vector<float> drawPoints;

//         if (localTick < drawEnd)
//         {
//             // --- Draw phase ---
//             int size = std::min(g.getSize(), g.getRangeSize() * localTick);
//             drawPoints.reserve(size);
//             drawPoints.assign(g.points.begin(), g.points.begin() + size);
//         }
//         else if (localTick < morphEnd && g.hasMorph())
//         {
//             // --- Morph phase ---
//             float alpha = float(localTick - drawEnd) / g.morphDuration;
//             for (int i = 0; i < g.getSize() / 2; i++)
//             {
//                 auto [x, y] = g.interpolateVertex(i, alpha);
//                 drawPoints.push_back(x);
//                 drawPoints.push_back(y);
//             }
//         }
//         else
//         {
//             // --- Hold phase ---
//             if (g.hasMorph())
//                 drawPoints = g.morphPoints;
//             else
//                 drawPoints = g.points;

//             // Handle AnimationMode here
//             if ((localTick + 1) == g.getTotalDuration())
//             {
//                 switch (g.ANIMATION_MODE)
//                 {
//                 case AnimationMode::ONCE_AND_REMOVE:
//                     g.setRangeSize(0);
//                     break;
//                 case AnimationMode::ONCE_AND_LOOP_BACK:
//                     g.StartTime((tick + g.loopTime + 1) / 60.0f);
//                     break;
//                 case AnimationMode::ONCE:
//                     g.setRangeSize(g.getSize());
//                     break;
//                 case AnimationMode::INFINITE:
//                 default:
//                     break;
//                 }
//             }
//         }
//         app->refreshOpenGL(drawPoints, 0, drawPoints.size());
//         glLineWidth(2.5f);
//         GraphColor *c = g.getColor();
//         app->setColor(c->RED, c->GREEN, c->BLUE);
//         glDrawArrays(GL_LINE_STRIP, 0, drawPoints.size() / 2);
//         drawPoints.clear();
//     }

//     app->setColor(0.40f, 0.40f, 0.70f);
//     shader->setVec2("translate", 0.0f, 0.0f);
//     shader->setVec2("position", 1.0f, 1.0f);
//     drawBox();
// }

void Graph::draw(float tick)
{
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // app->setColor(0.12f, 0.13f, 0.25f);
    // drawGrid();

    // shader->setVec2("position", scale, scale);
    // shader->setVec2("translate", panOffsetX * scale * unitX, panOffsetY * scale * unitY);
    // shader->setFloat("pointSize", 3.0f);
    // int size = graphs.size();
    // for(int i = 0; i < size; ++i){
    //     graphs[i].draw(tick);
    // }

    for(singletonGraph* graph: graphs){
        graph->draw(tick);
    }
    glLineWidth(3.0f);
    // app->setColor(0.12f, 0.13f, 0.25f);
    // drawBox();
}

void Graph::addText(const std::string &text, float x, float y, float scale, GraphColor *color)
{
    // float duration = getDuration();
    // int size = text.size();

    // float eachCharDuration = duration / (size == 0 ? 1 : size);
}

void Graph::drawText(int tick)
{
}

void Graph::morph(int index1, int index2)
{
    if (index1 < 0 || index1 > (int)graphs.size() || index2 < 0 || index2 > (int)graphs.size())
    {
        std::cerr << "Invalid graph index!" << std::endl;
        return;
    }
    else
    {
        graphs[index1]->setMorphPoints(graphs[index2]->points);
    }
}

Graph *Graph::getInstance(float speed)
{
    if (instance)
    {
        std::cerr << "\033[31mError: Graph instance already exists! Only one instance of Graph is allowed.\033[0m\n";
        std::exit(EXIT_FAILURE); // Exit immediately
    }

    instance = new Graph(speed);
    return instance;
}

void Graph::run()
{
    app->run(instance);
}
