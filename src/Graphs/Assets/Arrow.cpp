#include "../../../include/GraphEngine/Graphs/Assets/Arrow.hpp"
Arrow3D::Arrow3D()
{
    this->shader = new Shader(vertexShaderPath, fragmentShaderPath);

    // rotationAxis = randomUnitVector();
    rotationSpeed = glm::radians(2.0f); // 20° per second
    startTime = glfwGetTime();

    cameraPos = Vec3(0.0f, 0.0f, 100.0f);
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
    (GraphApp::keyManager)->registerListener(this);

    generate();
}

Arrow3D::Arrow3D(Vec3 start, Vec3 end){
    this->start = start;
    this->end = end;

    this->shader = new Shader(vertexShaderPath, fragmentShaderPath);

    // rotationAxis = randomUnitVector();
    rotationSpeed = glm::radians(2.0f); // 20° per second
    startTime = glfwGetTime();

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
    (GraphApp::keyManager)->registerListener(this);

    generate();
}

void Arrow3D::init()
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

void Arrow3D::drawTick(float tick)
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

    shader->setVec3("objectColor", 1.0f, 0.0f, 0.0f);
    shader->setVec3("lightPos", cameraPos.x, cameraPos.y, cameraPos.z);
    shader->setVec3("viewPos", cameraPos.x, cameraPos.y, cameraPos.z);
    shader->setVec3("lightColor", 1.0f, 1.0f, 1.0f);

    glBindVertexArray(VAO);

    // Draw triangles (assumes points are arranged for triangle mesh)
    glDrawArrays(GL_TRIANGLES, 0, getSize() / 6);

    glBindVertexArray(0);
}

void Arrow3D::onMouseMoveCallback(MouseEvent event)
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

        // std::cout << "roty is: " << rotY << std::endl;

        model = glm::mat4(1.0f);
        model = glm::rotate(model, glm::radians(rotX), glm::vec3(1, 0, 0));
        model = glm::rotate(model, glm::radians(rotY), glm::vec3(0, 1, 0));
    }
    else
    {
        lastX = -1;
        lastY = -1;
    }
}

void Arrow3D::onKeyPressedOnceCallback(const KeyEvent &event)
{
    if (event.key == GLFW_KEY_F && cameraPos.z > 0)
    {
        cameraPos.z -= 5.0f;
    }
    else if (event.key == GLFW_KEY_B)
    {
        cameraPos.z += 5.0f;
    }
}

void Arrow3D::generate()
{

    glm::vec3 dir = glm::normalize(glm::vec3(end.x - start.x, end.y - start.y, end.z - start.z));
    float length = glm::length(glm::vec3(end.x - start.x, end.y - start.y, end.z - start.z));

    float headLength = 1.5f;
    float shaftLength = length - headLength;

    float shaftRadius = 0.1f;
    float headRadius = 1.0f;

    glm::vec3 base = glm::vec3(start.x, start.y, start.z);
    glm::vec3 shaftEnd = base + dir * shaftLength;
    glm::vec3 apex = base + dir * length;

    // --- Universal perpendicular basis ---
    glm::vec3 u;
    if (fabs(dir.x) > fabs(dir.y))
        u = glm::normalize(glm::vec3(-dir.z, 0, dir.x));
    else
        u = glm::normalize(glm::vec3(0, dir.z, -dir.y));
    glm::vec3 v = glm::cross(dir, u);

    std::cout << "u is: " << std::endl;
    std::cout << u[0] << ", " << u[1] << ", " << u[2] << std::endl;
    std::cout << "v is: " << std::endl;
    std::cout << v[0] << ", " << v[1] << ", " << v[2] << std::endl;
    std::cout << "div is: " << std::endl;
    std::cout << dir[0] << ", " << dir[1] << ", " << dir[2] << std::endl;

    int segments = 64;
    float step = 2.0f * glm::pi<float>() / segments;

    // --- Shaft (cylinder) ---
    for (int i = 0; i < segments; ++i)
    {
        float t0 = i * step;
        float t1 = (i + 1) * step;

        glm::vec3 r0 = u * (shaftRadius * cos(t0)) + v * (shaftRadius * sin(t0));
        glm::vec3 r1 = u * (shaftRadius * cos(t1)) + v * (shaftRadius * sin(t1));

        // Normals for cylinder: radial
        glm::vec3 n0 = glm::normalize(r0);
        glm::vec3 n1 = glm::normalize(r1);

        // First triangle
        add(base + r0);
        add(n0);
        add(base + r1);
        add(n1);
        add(shaftEnd + r1);
        add(n1);

        // Second triangle
        add(base + r0);
        add(n0);
        add(shaftEnd + r1);
        add(n1);
        add(shaftEnd + r0);
        add(n0);
    }

    // --- Cone (arrowhead) ---
    for (int i = 0; i < segments; ++i)
    {
        float t0 = i * step;
        float t1 = (i + 1) * step;

        glm::vec3 r0 = u * (headRadius * cos(t0)) + v * (headRadius * sin(t0));
        glm::vec3 r1 = u * (headRadius * cos(t1)) + v * (headRadius * sin(t1));

        glm::vec3 p0 = shaftEnd + r0;
        glm::vec3 p1 = shaftEnd + r1;

        // Normals for cone: perpendicular to cone surface
        // glm::vec3 n0 = glm::normalize(glm::cross(p1 - apex, p0 - apex));
        // glm::vec3 n1 = glm::normalize(glm::cross(p0 - apex, p1 - apex));

        add(p0);
        add(-glm::normalize(dir));
        add(shaftEnd);
        add(-glm::normalize(dir));
        add(p1);
        add(-glm::normalize(dir));

        add(p0);
        add(glm::normalize(dir));
        add(p1);
        add(glm::normalize(dir));
        add(apex); // Tip normal along axis
        add(glm::normalize(dir));
    }

    // std::cout << "Done here" << std::endl;
}