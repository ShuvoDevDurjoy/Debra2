#ifndef __GRAPHAPP_HPP__
#define __GRAPHAPP_HPP__
#include <iostream>
#include <vector>
#include "../include/glad.h"
#include "../include/glfw3.h"
#include <map>
#include "../Graphs/Graph.hpp"
#include "../Core/Shader.hpp"
#include "../../Event/KeyEventManager.hpp"
#include "../../Event/MouseEventListener.hpp"
#include "../../../glm/glm.hpp"
#include "../../../glm/gtc/matrix_transform.hpp"
#include "../../Event/KeyClicked.hpp"
#include "../../Event/MouseEvents.hpp"

#define VERTEX_SHADER_FILE_NAME "./shaders/vertex_shader.vs"
#define FRAGMENT_SHADER_FILE_NAME "./shaders/fragment_shader.fs"
#define GEOMETRIC_SHADER "./shaders/geometry.gs"
#define TEXT_VERTEX_SHADER_FILE_NAME "./shaders/text.vs"
#define TEXT_FRAGMENT_SHADER_FILE_NAME "./shaders/text.fs"
#define FONT_FAMILY_PATH "./fonts/NunitoSans.ttf"

class Graph;

class GraphApp : public MouseEvents, public KeyClicked
{
private:
    int window_width = 1200;
    int window_height = 600;
    std::string vertex_shader_file = VERTEX_SHADER_FILE_NAME;
    std::string fragment_shader_file = FRAGMENT_SHADER_FILE_NAME;
    GLuint VBO, VAO;
    Shader *shader, *text_shader;
    GLFWwindow *window;
    
    public:
    static float lastX , lastY, rotX, rotY;
    static KeyEventManager *keyManager;
    static MouseEventListener *mouseEventMangager;
    static glm::mat4 projection, view;
    static glm::vec3 cameraPos;
    static glm::vec3 camera_center;
    static int drawCount;
    static bool isAlive;

private:
    int InitWindow();
    void InitTextRenderer();
    int loadGLAD();
    static void frame_buffer_size_callback(GLFWwindow *, int, int);
    void process_input();
    void setCallback();
    void mainLoop(Graph *);
    void cleanUp(Graph *);

    void LoadFont(const std::string &path);

    static void mouseMoveCallback(GLFWwindow *window, double xpos, double ypos);
    static void keyClickCallback(GLFWwindow *window, int key, int scancode, int action, int mode);
    static void mouseClickCallback(GLFWwindow *window, int button, int action, int mode);

public:
    GraphApp();
    ~GraphApp() {};
    GLFWwindow *getWindow() { return window; };
    Shader *getShader() { return shader; };
    void refreshOpenGL(std::vector<float> &, int, int);
    void setColor(float, float, float);
    void run(Graph *);

    void onMouseClickCallback(MouseEvent) override{}

    void onMouseMoveCallback(MouseEvent) override;
    void onKeyPressedOnceCallback(const KeyEvent &event) override;
};

#endif