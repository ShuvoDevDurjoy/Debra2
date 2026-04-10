#ifndef __GRAPHAPP_HPP__
#define __GRAPHAPP_HPP__
#include <map>
#include <vector>
#include <iostream>

#include <glad.h>
#include <glfw3.h>
#include <glm/glm.hpp>
#include <Event/KeyEvent.hpp>
#include <Event/MouseEvent.hpp>
#include <GraphEngine/Core/Shader.hpp>
#include <GraphEngine/Scene/Scene.hpp>
#include <glm/gtc/matrix_transform.hpp>


class Scene;

class GraphApp
{
private:
public:
    static int window_width;
    static int window_height;
    static float xUnits;
    GLFWwindow *window = nullptr;
    
    public:
    static int drawCount;
    static bool isAlive;
    public:
        static std::string font_path;

    private:
        int InitWindow();
        int loadGLAD();
        static void frame_buffer_size_callback(GLFWwindow *, int, int);
        void process_input();
        void setCallback();
        void mainLoop(Scene *);
        void cleanUp(Scene *);

        static void mouseMoveCallback(GLFWwindow *window, double xpos, double ypos);
        static void keyClickCallback(GLFWwindow *window, int key, int scancode, int action, int mode);
        static void mouseClickCallback(GLFWwindow *window, int button, int action, int mode);
        static void scrollCallback(GLFWwindow *window, double xoffset, double yoffset);

    public:
        GraphApp(int width = 1200, int height = 600, float xUnits = 14.22f);
        ~GraphApp() {};
        GLFWwindow *getWindow() { return window; };
        void run(Scene *);
};

#endif
