#include <GraphEngine/Core/GraphApp.hpp>

Scene* currentGraph = nullptr;

int GraphApp::drawCount = 0;
int GraphApp::window_width = 1200;
int GraphApp::window_height = 600;
float GraphApp::xUnits = 14.22f;
std::string GraphApp::font_path = "./fonts/playful_normal.ttf";

GraphApp::GraphApp(int width, int height, float xUnits)
{
    window_width = width;
    window_height = height;
    GraphApp::xUnits = xUnits;
    InitWindow();
    loadGLAD();
    process_input();
    setCallback();
}

int GraphApp::InitWindow()
{
    // initialize glfw
    glfwInit();

    // Enable MSAA
    glfwWindowHint(GLFW_SAMPLES, 4);
    
    // define window hints
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_STENCIL_BITS, 8); // Request stencil buffer for zero-overlap logic

    // create window
    window = glfwCreateWindow(window_width, window_height, "Debra", nullptr, nullptr);

    // Initial setup handled by Graph constructor

    // check if the window is created successfully or return out of the programe terminating the glfw
    if (!window)
    {
        std::cout << "Window Creation Failed" << std::endl;
        glfwTerminate();
        return -1;
    }

    // if window creating is successfull then head to creating current context
    glfwMakeContextCurrent(window);
    glfwSetWindowSizeLimits(window, 600, 400, GLFW_DONT_CARE, GLFW_DONT_CARE);
    return 0;
}

int GraphApp::loadGLAD()
{
    // load glad to load opengl function and pointers
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Error with Loading the GLAD" << std::endl;
        return -1;
    }

    glEnable(GL_MULTISAMPLE);
    return 0;
}

void GraphApp::frame_buffer_size_callback(GLFWwindow *window, int w, int h)
{
    glViewport(0, 0, w, h);
    window_width = w;
    window_height = h;
}

void GraphApp::mouseMoveCallback(GLFWwindow *window, double xpos, double ypos)
{
    if (currentGraph)
    {
        MouseEvent event(MouseEvent::Type::MOVE, (float)xpos, (float)ypos);
        if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
            event.button = GLFW_MOUSE_BUTTON_LEFT;
            event.action = GLFW_PRESS;
        } else if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS) {
            event.button = GLFW_MOUSE_BUTTON_RIGHT;
            event.action = GLFW_PRESS;
        }
        
        auto* listener = dynamic_cast<MouseEventListener*>(currentGraph);
        if (listener) listener->onEvent(event);
    }
}

void GraphApp::scrollCallback(GLFWwindow *window, double xoffset, double yoffset)
{
    if (currentGraph)
    {
        MouseEvent event((float)xoffset, (float)yoffset, MouseEvent::Type::SCROLL);
        auto* listener = dynamic_cast<MouseEventListener*>(currentGraph);
        if (listener) listener->onEvent(event);
    }
}

void GraphApp::keyClickCallback(GLFWwindow *window, int key, int scancode, int action, int mode)
{
    if (currentGraph && action == GLFW_PRESS)
    {
        KeyEvent event(key, scancode, action, mode, window);
        auto* listener = dynamic_cast<KeyEventListener*>(currentGraph);
        if (listener) listener->onEvent(event);
    }
}

void GraphApp::mouseClickCallback(GLFWwindow *window, int button, int action, int mode)
{
    if (currentGraph)
    {
        double xpos, ypos;
        glfwGetCursorPos(window, &xpos, &ypos);
        MouseEvent event(button, action, mode, (float)xpos, (float)ypos);
        auto* listener = dynamic_cast<MouseEventListener*>(currentGraph);
        if (listener) listener->onEvent(event);
    }
}

void GraphApp::process_input()
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}
void GraphApp::setCallback()
{
    // callback for frame buffer size
    glfwSetFramebufferSizeCallback(window, frame_buffer_size_callback);
    glfwSetCursorPosCallback(window, mouseMoveCallback);
    glfwSetKeyCallback(window, keyClickCallback);
    glfwSetMouseButtonCallback(window, mouseClickCallback);
    glfwSetScrollCallback(window, scrollCallback);
}

bool GraphApp::isAlive = true;


void GraphApp::mainLoop(Scene *graph)
{
    currentGraph = graph;
    glfwSwapInterval(1); // Enable V-Sync
    // glEnable(GL_DEPTH_TEST);
    double lastFrameTime = glfwGetTime();
    graph->InitGraphs();

    while (!glfwWindowShouldClose(window))
    {
        double currentTime = glfwGetTime();
        float deltaTime = float(currentTime - lastFrameTime);

        process_input();

        glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
        
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
        graph->draw(deltaTime); // use interpolated positions
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
}

void GraphApp::cleanUp(Scene *graph)
{
    delete graph;
    glfwTerminate();
}

void GraphApp::run(Scene *graph)
{
    mainLoop(graph);
    cleanUp(graph);
}
