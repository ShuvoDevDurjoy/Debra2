#include "../../include/GraphEngine/Graphs/ThreeDAxes.hpp"

ThreeDAxes::ThreeDAxes()
{
    this->shader = new Shader(vertexShaderPath, fragmentShaderPath);

    color = GraphColor(0.67, 0.67, 0.67);

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
}

void ThreeDAxes::generate(Vec3 start, Vec3 end)
{
    glm::vec3 dir = glm::normalize(glm::vec3(end.x - start.x, end.y - start.y, end.z - start.z));
    float length = glm::length(glm::vec3(end.x - start.x, end.y - start.y, end.z - start.z));

    float headLength = 1.5f;
    float shaftLength = length;

    float shaftRadius = 0.1f;
    float headRadius = 1.0f;

    glm::vec3 base = glm::vec3(start.x, start.y, start.z);
    glm::vec3 shaftEnd = base + dir * shaftLength;
    glm::vec3 apex = base + dir * (length + headLength);

    // --- Universal perpendicular basis ---
    glm::vec3 u;
    if (fabs(dir.x) > fabs(dir.y))
        u = glm::normalize(glm::vec3(-dir.z, 0, dir.x));
    else
        u = glm::normalize(glm::vec3(0, dir.z, -dir.y));
    glm::vec3 v = glm::cross(dir, u);
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
        glm::vec3 n0 = glm::normalize(glm::cross(p1 - apex, p0 - apex));
        glm::vec3 n1 = glm::normalize(glm::cross(p0 - apex, p1 - apex));

        // float d = glm::pi<float>() / 4;

        float degreeCos = cos(glm::pi<float>() / 3);
        float degreeSin = sin(glm::pi<float>() / 6);

        glm::vec3 r3 = r0 * degreeCos + dir * degreeSin;
        glm::vec3 r4 = r1 * degreeCos + dir * degreeSin;

        n0 = glm::normalize(r3);
        n1 = glm::normalize(r4);

        add(p0);
        add(-glm::normalize(dir));
        add(shaftEnd);
        add(-glm::normalize(dir));
        add(p1);
        add(-glm::normalize(dir));

        add(p0);
        add(n0);
        add(p1);
        add(n1);
        add(apex); // Tip normal along axis
        add(glm::normalize(dir));
    }

    // std::cout << "Done here" << std::endl;
}

void ThreeDAxes::init()
{

    const float Rx = 50.0f; // axis extent
    const float Ry = 30.0f; // axis extent
    const float Rz = 30.0f; // axis extent

    Vec3 origin(0, 0, 0);
    generate(origin, Vec3(Rx, 0.0f, 0.0f));
    generate(origin, Vec3(-Rx, 0.0f, 0.0f));
    generate(origin, Vec3(0.0f, Ry, 0.0f));
    generate(origin, Vec3(0.0f, -Ry, 0.0f));
    generate(origin, Vec3(0.0f, 0.0f, Rz));
    generate(origin, Vec3(0.0f, 0.0f, -Rz));

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

    glBufferData(GL_ARRAY_BUFFER, getSize() * sizeof(float), points.data(), GL_STATIC_DRAW);

    // // Vertex layout: 3 floats per vertex (x, y, z)
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    initialized = true;
}

void ThreeDAxes::drawTick(float tick)
{
    if (!initialized)
    {
        init();
    }

    if (!shader)
    return;

    shader->use();

    view = glm::lookAt(
        glm::vec3(cameraPos.x, cameraPos.y, cameraPos.z),
        glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec3(0.0f, 1.0f, 1.0f));
    // std::cout << "here we are 1" << std::endl;
    // Pass the matrices to the shader
    shader->setMat4("model", model);
    // // std::cout << "here we are 2" << std::endl;
    shader->setMat4("view", view);
    // // std::cout << "here we are 3" << std::endl;
    shader->setMat4("projection", projection);
    // // std::cout << "here we are 4" << std::endl;

    shader->setVec3("objectColor", color.RED, color.GREEN, color.BLUE);
    // // std::cout << "here we are 5" << std::endl;
    shader->setVec3("lightPos", cameraPos.x, cameraPos.y, cameraPos.z);
    // // std::cout << "here we are 6" << std::endl;
    shader->setVec3("viewPos", cameraPos.x, cameraPos.y, cameraPos.z);
    // // std::cout << "here we are 7" << std::endl;
    shader->setVec3("lightColor", 1.0f, 0.2f, 0.5f);
    // // std::cout << "here we are 8" << std::endl;

    glBindVertexArray(VAO);

    // // std::cout << "here we are 9" << std::endl;
    // Draw triangles (assumes points are arranged for triangle mesh)
    glDrawArrays(GL_TRIANGLES, 0, getSize() / 3);

    // // std::cout << "here we are 10" << std::endl;

    glBindVertexArray(0);
    // // std::cout << "here we are 11" << std::endl;
}

void ThreeDAxes::onMouseMoveCallback(MouseEvent event)
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

void ThreeDAxes::onKeyPressedOnceCallback(const KeyEvent &event)
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