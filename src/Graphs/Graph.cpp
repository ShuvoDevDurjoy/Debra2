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
}

void Graph::InitGraphs()
{
    for (GraphMathObject *graph : graphs)
    {
        graph->Init(0);
    }
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

float Graph::normalizeX(float x)
{
    return (x * 2.0f) / (stepsx * 1.0f);
}

float Graph::normalizeY(float y)
{
    return (y * 2.0f) / (stepsy * 1.0f);
}

void Graph::draw(float tick)
{
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    try
    {
        for (GraphMathObject *graph : graphs)
        {
            graph->draw(tick);
        }

        AnimationManager::Run(tick);
    }
    catch (const std::exception &e)
    {
        std::cerr << "Caught standard exception: " << e.what() << std::endl;
    }
    catch (...)
    {
        std::cerr << "Caught an UNKNOWN exception." << std::endl;
    }
    glLineWidth(3.0f);
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
