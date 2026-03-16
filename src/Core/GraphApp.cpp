#include "../../include/GraphEngine/Core/GraphApp.hpp"

KeyEventManager *GraphApp::keyManager;
MouseEventListener *GraphApp::mouseEventMangager;
float GraphApp::lastX = 0, GraphApp::lastY = 0, GraphApp::rotX = 0, GraphApp::rotY = 0;
int GraphApp::drawCount = 0;
int GraphApp::window_width = 1200;
int GraphApp::window_height = 600;

GraphApp::GraphApp(int width, int height)
{
    // window_height = 570;
    window_width = width;
    window_height = height;
    InitWindow();
    loadGLAD();
    InitTextRenderer();
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    shader = new Shader(vertex_shader_file, fragment_shader_file);
    process_input();
    setCallback();

    keyManager = new KeyEventManager();
    mouseEventMangager = new MouseEventListener();

    keyManager->registerListener(this);
    mouseEventMangager->registerListener(this);
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

void GraphApp::InitTextRenderer()
{

    text_shader = new Shader(TEXT_VERTEX_SHADER_FILE_NAME, TEXT_FRAGMENT_SHADER_FILE_NAME);

    Font::Load(FONT_FAMILY_PATH, 48);

    Font::UpdateProjection(window_width, window_height);

    text_shader->use();
    text_shader->setInt("text", 0);
    text_shader->setMat4("projection", Font::projection);
}

void GraphApp::frame_buffer_size_callback(GLFWwindow *window, int w, int h)
{
    glViewport(0, 0, w, h);
    window_width = w;
    window_height = h;
}

void GraphApp::mouseMoveCallback(GLFWwindow *window, double xpos, double ypos)
{
    mouseEventMangager->mouseMoved(window, xpos, ypos);
}

void GraphApp::keyClickCallback(GLFWwindow *window, int key, int scancode, int action, int mode)
{
    keyManager->pollAll(window, key, scancode, action, mode);
}

void GraphApp::mouseClickCallback(GLFWwindow *window, int button, int action, int mode)
{
    mouseEventMangager->mouseClicked(window, button, action, mode);
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
}

bool GraphApp::isAlive = true;

Scene* currentGraph = nullptr;

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
    glDeleteBuffers(1, &VBO);
    glDeleteVertexArrays(1, &VAO);
    glDeleteProgram(shader->getID());
    delete graph;
    delete keyManager;
    glfwTerminate();
}

void GraphApp::refreshOpenGL(std::vector<float> &vertices, int initial, int size)
{
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    glBufferData(GL_ARRAY_BUFFER, (size - initial) * sizeof(float), vertices.data() + initial, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);
}

void GraphApp::setColor(float r, float g, float b)
{
    shader->setVec3("color", r, g, b);
}

void GraphApp::run(Scene *graph)
{
    mainLoop(graph);
    cleanUp(graph);
}

void GraphApp::onMouseMoveCallback(MouseEvent event) 
{
    // Touchpad control: left-drag pans the camera
    if (currentGraph && currentGraph->getControlMode() == Scene::ControlMode::TOUCHPAD_CONTROL)
    {
        if (event.key == GLFW_MOUSE_BUTTON_LEFT && event.action == GLFW_PRESS)
        {
            if (lastX < 0 || lastY < 0) { lastX = event.positionX; lastY = event.positionY; return; }

            float dx = event.positionX - lastX;
            float dy = event.positionY - lastY;
            lastX = event.positionX;
            lastY = event.positionY;

            // Scale drag pixels → world units proportional to zoom distance
            Camera* cam = currentGraph->getCamera();
            float zDist = glm::length(cam->getPosition() - cam->getCenter());
            float panScale = zDist / (float)window_width;

            glm::vec3 center = cam->getCenter();
            center.x -= dx * panScale;
            center.y += dy * panScale;
            glm::vec3 pos = cam->getPosition();
            pos.x -= dx * panScale;
            pos.y += dy * panScale;
            cam->setCenter(center);
            cam->setPosition(pos);
        }
        else { lastX = -1; lastY = -1; }
        return; // don't fall through to camera orbit logic
    }

    // Camera control: legacy orbit via drag (rotX/rotY still stored but not used by new Camera)
    if (event.key == GLFW_MOUSE_BUTTON_LEFT && event.action == GLFW_PRESS)
    {
        if (lastX < 0 || lastY < 0) { lastX = event.positionX; lastY = event.positionY; return; }
        float dx = event.positionX - lastX;
        float dy = event.positionY - lastY;
        lastX = event.positionX;
        lastY = event.positionY;
        rotX = fmod(rotX + dy * (180.0f / event.windowHeight), 180.0f);
        rotY = fmod(rotY + dx * (180.0f / event.windowWidth),  180.0f);
    }
    else { lastX = -1; lastY = -1; }
}

void GraphApp::onKeyPressedOnceCallback(const KeyEvent &event)
{
    if (!currentGraph || !currentGraph->getCamera()) return;

    // Keyboard navigation only applies in CAMERA_CONTROL mode
    if (currentGraph->getControlMode() != Scene::ControlMode::CAMERA_CONTROL) return;

    Camera* cam = currentGraph->getCamera();
    glm::vec3 pos = cam->getPosition();
    glm::vec3 center = cam->getCenter();

    if      (event.key == GLFW_KEY_F)     { pos.z -= 5.0f; }
    else if (event.key == GLFW_KEY_B)     { pos.z += 5.0f; }
    else if (event.key == GLFW_KEY_UP)    { center.y += 2.0f; }
    else if (event.key == GLFW_KEY_DOWN)  { center.y -= 2.0f; }
    else if (event.key == GLFW_KEY_LEFT)  { center.x -= 2.0f; }
    else if (event.key == GLFW_KEY_RIGHT) { center.x += 2.0f; }
    else if (event.key == GLFW_KEY_R)     { pos = glm::vec3(0,0,100); center = glm::vec3(0,0,0); }

    cam->setPosition(pos);
    cam->setCenter(center);
}
