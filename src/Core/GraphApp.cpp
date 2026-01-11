#include "../../include/GraphEngine/Core/GraphApp.hpp"

KeyEventManager *GraphApp::keyManager;
MouseEventListener *GraphApp::mouseEventMangager;
glm::mat4 GraphApp::view, GraphApp::projection;
glm::vec3 GraphApp::cameraPos;
float GraphApp::lastX = 0, GraphApp::lastY = 0, GraphApp::rotX = 0, GraphApp::rotY = 0;

GraphApp::GraphApp()
{
    InitWindow();
    loadGLAD();
    InitTextRenderer();
    window_width = 600;
    window_height = 400;
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

    // glEnable(GL_MULTISAMPLE);

    // #ifdef _WIN32
    // Enable MSAA (multi-sample anti-aliasing) only on Windows
    // glfwWindowHint(GLFW_SAMPLES, 4);
// #endif
    // define window hints
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    // glfwWindowHint(GLFW_MAXIMIZED, GLFW_TRUE);

    // create window
    window = glfwCreateWindow(window_width, window_height, "Shuvo Dev Durjoy", nullptr, nullptr);

    cameraPos = glm::vec3(0.0f, 0.0f, 120.0f);
    view = glm::lookAt(
        glm::vec3(cameraPos[0], cameraPos[1], cameraPos[2]),
        glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec3(0.0f, 1.0f, 0.0f));

    // Projection
    projection = glm::perspective(
        glm::radians(45.0f),
        (window_width / window_height) * 1.0f, // replace with window ratio if needed
        0.1f,
        500.0f);

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

#ifdef _WIN32
    glEnable(GL_MULTISAMPLE);
#endif
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

void GraphApp::mainLoop(Graph *graph)
{
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

void GraphApp::cleanUp(Graph *graph)
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

void GraphApp::run(Graph *graph)
{
    mainLoop(graph);
    cleanUp(graph);
}

void GraphApp::onMouseMoveCallback(MouseEvent event) 
{
    if (event.key == GLFW_MOUSE_BUTTON_LEFT && event.action == GLFW_PRESS)
    {
        // First click initialization
        if (lastX < 0 || lastY < 0)
        {
            lastX = event.positionX;
            lastY = event.positionY;
            return;
        }

        float dx = event.positionX - lastX;
        float dy = event.positionY - lastY;

        lastX = event.positionX;
        lastY = event.positionY;

        // Map full screen drag → full rotation
        float rotPerPixelX = 360.0f / event.windowHeight;
        float rotPerPixelY = 360.0f / event.windowWidth;

        rotX += dy * rotPerPixelX; // vertical drag → X axis
        rotY += dx * rotPerPixelY; // horizontal drag → Y axis

        // Optional: keep values bounded
        rotX = fmod(rotX, 360.0f);
        rotY = fmod(rotY, 360.0f);

    }
    else
    {
        lastX = -1;
        lastY = -1;
    }
}

void GraphApp::onKeyPressedOnceCallback(const KeyEvent &event)
{
    if (event.key == GLFW_KEY_F && cameraPos.z > 0)
    {
        cameraPos.z -= 5.0f;
    }
    else if (event.key == GLFW_KEY_B)
    {
        cameraPos.z += 5.0f;
    }
    else if (event.key == GLFW_KEY_UP)
    {
        cameraPos.y += 5.0f;
    }
    else if (event.key == GLFW_KEY_DOWN)
    {
        cameraPos.y -= 5.0f;
    }
    else if (event.key == GLFW_KEY_LEFT)
    {
        cameraPos.x -= 5.0f;
    }
    else if (event.key == GLFW_KEY_R)
    {
        cameraPos.x += 5.0f;
    }

    view = glm::lookAt(
        glm::vec3(cameraPos[0], cameraPos[1], cameraPos[2]),
        glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec3(0.0f, 1.0f, 0.0f));
}
