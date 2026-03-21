#ifndef __RENDERER_HPP__
#define __RENDERER_HPP__

#include <vector>
#include <glad.h>
#include <glfw3.h>
#include <GraphEngine/Camera/Camera.hpp>
#include <GraphEngine/Graphs/GraphObjects/GraphMathObject.hpp>
#include <GraphEngine/Graphs/GraphObjects/GraphObject.hpp>
#include <GraphEngine/Graphs/GraphObjects/ThreeDObject.hpp>

class Renderer {
public:
    Renderer() = default;
    ~Renderer() = default;

    /**
     * @brief Dispatches drawing commands for a list of graph objects.
     * Manages global variables (view, projection, light, etc.) and avoids 
     * redundant state changes.
     */
    void Draw(const std::vector<GraphMathObject*>& objects, Camera* camera, float dt);

private:
    void renderGraphObjectFill(GraphObject* gObj, Camera* camera, float dt);
    void renderGraphObjectStroke(GraphObject* gObj, Camera* camera, float dt);

    void renderThreeDObjectFill(ThreeDObject* tObj, Camera* camera, float dt);
    void renderThreeDObjectStroke(ThreeDObject* tObj, Camera* camera, float dt);
};

#endif // __RENDERER_HPP__