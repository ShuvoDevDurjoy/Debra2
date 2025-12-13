#include "../../include/GraphEngine/Graphs/Surface.hpp"

Surface::Surface()
{
    this->shader = new Shader(vertexShaderPath, fragmentShaderPath);

    cameraPos = Vec3(0.0f, 45.0f, 50.0f);
    // Perspective camera
    view = glm::lookAt(
        glm::vec3(cameraPos.x, cameraPos.y, cameraPos.z),
        glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec3(0.0f, 1.0f, 1.0f));

    // Projection
    projection = glm::perspective(
        glm::radians(45.0f),
        1.6f, // replace with window ratio if needed
        0.1f,
        500.0f);

    // Model rotation
    model = glm::mat4(1.0f);

    (GraphApp::mouseEventMangager)->registerListener(this);
}

// Initialize VAO, VBO and upload points
void Surface::init()
{
    if (getSize() == 0) // Corrected the check
    {
        std::cerr << "No points to initialize!\n";
        return;
    }

    if (!shader)
    {
        std::cerr << "Shader not initialized!\n";
        return;
    }

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    glBufferData(GL_ARRAY_BUFFER, getSize() * sizeof(float),
                 points.data(), GL_STATIC_DRAW);

    // Vertex layout: 3 floats per vertex (x, y, z)
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);


    initialized = true;
}

void Surface::drawTick()
{
    if (!initialized)
    {
        init();
    }

    if (!shader)
        return;

    shader->use();

    // Pass the matrices to the shader
    shader->setMat4("model", model);
    shader->setMat4("view", view);
    shader->setMat4("projection", projection);

    shader->setVec3("objectColor", color.RED, color.GREEN, color.BLUE);
    shader->setVec3("lightPos", cameraPos.x, cameraPos.y, cameraPos.z);
    shader->setVec3("viewPos", cameraPos.x, cameraPos.y, cameraPos.z);
    shader->setVec3("lightColor", 1.0f, 1.0f, 1.0f);

    glBindVertexArray(VAO);

    // Draw triangles (assumes points are arranged for triangle mesh)
    glDrawArrays(GL_TRIANGLES, 0, getSize() / 3);

    glBindVertexArray(0);
}


void Surface::onMouseMoveCallback(MouseEvent event){

    int width = event.windowWidth;
    int height = event.windowHeight;

    float nx = 1.0f - (2.0f * event.positionX) / (width * 1.0f);
    float ny = 1.0f - (2.0f * event.positionY) / (height * 1.0f); // invert Y

    float camX = nx * (50); // camRadius = max distance along X
    float camY = ny * (200); // same for Y
    float camZ = 100.0f;         // keep a fixed Z distance from origin

    cameraPos = Vec3(camX, camY, camZ);
    view = glm::lookAt(
        glm::vec3(camX, camY, camZ),
        glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec3(0.0f, 1.0f, 0.0f));
}