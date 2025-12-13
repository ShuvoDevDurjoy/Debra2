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

#define VERTEX_SHADER_FILE_NAME "./shaders/vertex_shader.vs"
#define FRAGMENT_SHADER_FILE_NAME "./shaders/fragment_shader.fs"
#define GEOMETRIC_SHADER "./shaders/geometry.gs"
#define TEXT_VERTEX_SHADER_FILE_NAME "./shaders/text.vs"
#define TEXT_FRAGMENT_SHADER_FILE_NAME "./shaders/text.fs"
#define FONT_FAMILY_PATH "./fonts/NunitoSans.ttf"

class Graph;

class GraphApp
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
    static KeyEventManager *keyManager;
    static MouseEventListener *mouseEventMangager;

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

public:
    GraphApp();
    ~GraphApp() {};
    GLFWwindow *getWindow() { return window; };
    Shader *getShader() { return shader; };
    void refreshOpenGL(std::vector<float> &, int, int);
    void setColor(float, float, float);
    void run(Graph *);
};

#endif