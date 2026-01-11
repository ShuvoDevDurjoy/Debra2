// #include "../../include/GraphEngine/Objects/Polygon.hpp"
// Polygon::Polygon()
// {
//     this->shader = new Shader("./shaders/cubic_bezier_shader_stroke/vertex.vs", "./shaders/cubic_bezier_shader_stroke/geometry.gs", "./shaders/cubic_bezier_shader_stroke/fragment.fs");

//     rotationSpeed = glm::radians(2.0f); // 20° per second
//     startTime = glfwGetTime();

//     color = new GraphColor(1.0f, 0.0f, 0.0f);

//     this->x = -10;
//     this->y = 10;
//     this->width = 20;
//     this->height = 20;

//     cameraPos = Vec3(0.0f, 0.0f, 100.0f);
//     // Perspective camera
//     view = glm::lookAt(
//         glm::vec3(cameraPos.x, cameraPos.y, cameraPos.z),
//         glm::vec3(0.0f, 0.0f, 0.0f),
//         glm::vec3(0.0f, 1.0f, 0.0f));

//     // Projection
//     projection = glm::perspective(
//         glm::radians(45.0f),
//         1.6f, // replace with window ratio if needed
//         0.1f,
//         500.0f);

//     // Model rotation
//     model = glm::mat4(1.0f);

//     (GraphApp::mouseEventMangager)->registerListener(this);
//     (GraphApp::keyManager)->registerListener(this);

//     // generate();
// }

// // Add to Polygon.cpp

// Polygon::Polygon(int sides, float radius)
// {

//     // this->shader = new Shader(vertexShaderPath, geometricShaderPath, fragmentShaderPath);

//     this->shader = new Shader("./shaders/cubic_bezier_shader_stroke/vertex.vs", "./shaders/cubic_bezier_shader_stroke/geometry.gs", "./shaders/cubic_bezier_shader_stroke/fragment.fs");
//     // this->shader = new Shader("./shaders/cubic_bezier_shader_stroke/vertex.vs", "./shaders/cubic_bezier_shader_stroke/fragment.fs");

//     rotationSpeed = glm::radians(2.0f); // 20° per second
//     startTime = glfwGetTime();

//     color = new GraphColor();

//     this->x = -10;
//     this->y = 10;
//     this->width = 20;
//     this->height = 20;

//     cameraPos = Vec3(0.0f, 0.0f, 100.0f);
//     // Perspective camera
//     view = glm::lookAt(
//         glm::vec3(cameraPos.x, cameraPos.y, cameraPos.z),
//         glm::vec3(0.0f, 0.0f, 0.0f),
//         glm::vec3(0.0f, 1.0f, 0.0f));

//     // Projection
//     projection = glm::perspective(
//         glm::radians(45.0f),
//         1.6f, // replace with window ratio if needed
//         0.1f,
//         500.0f);

//     // Model rotation
//     model = glm::mat4(1.0f);

//     (GraphApp::mouseEventMangager)->registerListener(this);
//     (GraphApp::keyManager)->registerListener(this);

//     this->sides = sides; // Store sides in a class member
//     generateRegularPolygon(sides, radius);
// }

// void Polygon::generateRegularPolygon(int sides, float radius)
// {
//     points.clear();
//     float angleStep = glm::two_pi<float>() / sides;

//     // add(glm::vec3(-10, 10, 0));
//     // add(glm::vec3(10, 10, 0));

//     for (int i = 0; i <= sides; i++)
//     {
//         float angle = i * angleStep;
//         // Position
//         glm::vec3 pos(round(radius * cos(angle) * 100) / 100, round(radius * sin(angle) * 100) / 100, 0.0f);
//         // Normal
//         glm::vec3 normal(0.0f, 0.0f, 1.0f);

//         add(pos);
//         // add(normal);
//     }
//     // add(Vec3(points[0], points[1], points[2]));

//     for (int i = 0; i < getSize(); i += 3){
//         std::cout << "[(" << points[i] << ", " << points[i + 1] << ", " << points[i + 2] << ")]";
//     }
//     std::cout << std::endl;

// }

// void Polygon::init()
// {
//     if (getSize() == 0) // Corrected the check
//     {
//         std::cerr << "No points to initialize!\n";
//         return;
//     }

//     if (!shader)
//     {
//         std::cerr << "Shader not initialized!\n";
//         return;
//     }

//     glGenVertexArrays(1, &VAO);
//     glGenBuffers(1, &VBO);

//     glBindVertexArray(VAO);
//     glBindBuffer(GL_ARRAY_BUFFER, VBO);

//     int size = getSize();

//     if(points[0] == points[size-3] && points[1] == points[size-2] && points[2] == points[size-1] && size > 6){
//         add(Vec3(points[size - 6], points[size - 5], points[size - 4]));
//     }
//     else{
//         add(Vec3(points[0], points[1], points[2]));
//     }

//     for (int i = 0; i < size - 3; i += 3)
//     {
//         add(Vec3(points[i], points[i + 1], points[i + 2]));
//     }

//     for (int i = 3; i < size; i += 3)
//     {
//         add(Vec3(points[i], points[i + 1], points[i + 2]));
//     }

//     if (points[0] == points[size - 3] && points[1] == points[size - 2] && points[2] == points[size - 1] && size > 6)
//     {
//         add(Vec3(points[3], points[4], points[5]));
//     }
//     else
//     {
//         add(Vec3(points[0], points[1], points[2]));
//     }

//     for (int i = size; i < 2 * size; i += 3){
//         std::cout << "[(" << points[i] << ", " << points[i + 1] << ", " << points[i + 2] << ")]";
//     }
//     std::cout<<std::endl;
//     for (int i = 0; i < size; i += 3){
//         std::cout << "[(" << points[i] << ", " << points[i + 1] << ", " << points[i + 2] << ")]";
//     }
//     std::cout<<std::endl;

//     for (int i = 2 * size; i < getSize(); i += 3){
//         std::cout << "[(" << points[i] << ", " << points[i + 1] << ", " << points[i + 2] << ")]";
//     }
//     std::cout << std::endl;

//     glBufferData(GL_ARRAY_BUFFER, getSize() * sizeof(float),
//                  points.data(), GL_STATIC_DRAW);

//     std::cout << "Size is: " << getSize() << std::endl;

//     // Vertex layout: 3 floats per vertex (x, y, z)
//     glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)(0 * sizeof(float)));
//     glEnableVertexAttribArray(0);
//     glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)(size * sizeof(float)));
//     glEnableVertexAttribArray(1);
//     glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)(size * 2 * sizeof(float)));
//     glEnableVertexAttribArray(2);

//     glBindBuffer(GL_ARRAY_BUFFER, 0);
//     glBindVertexArray(0);

//     remove_after_draw = false;

//     initialized = true;

//     initQuad();
// }

// void Polygon::initQuad()
// {
//     // Position (x, y, z) and Normal (x, y, z) to match your shader layout
//     float quadVertices[] = {
//         -20.0f, 20.0f, 0.0f, 0.0f, 0.0f, 1.0f,  // Top Left
//         -20.0f, -20.0f, 0.0f, 0.0f, 0.0f, 1.0f, // Bottom Left
//         20.0f, 20.0f, 0.0f, 0.0f, 0.0f, 1.0f,   // Top Right
//         20.0f, -20.0f, 0.0f, 0.0f, 0.0f, 1.0f   // Bottom Right
//     };

//     glGenVertexArrays(1, &quadVAO);
//     glGenBuffers(1, &quadVBO);

//     glBindVertexArray(quadVAO);
//     glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
//     glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);

//     // Position attribute
//     glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
//     glEnableVertexAttribArray(0);
    // Normal attribute
    // glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)(3 * sizeof(float)));
    // glEnableVertexAttribArray(1);
// }

// void Polygon::draw(float tick)
// {
//     if (!initialized)
//     {
//         init();
//     }

//     if (!shader)
//         return;

//     shader->use();

//     view = glm::lookAt(
//         glm::vec3(cameraPos.x, cameraPos.y, cameraPos.z),
//         glm::vec3(0.0f, 0.0f, 0.0f),
//         glm::vec3(0.0f, 1.0f, 1.0f));

//     shader->setFloat("iTime", tick);

//     // Pass the matrices to the shader
//     shader->setMat4("model", model);
//     shader->setMat4("view", view);
//     shader->setMat4("projection", projection);

//     GraphColor *color = this->getColor();

//     shader->setVec3("objectColor", color->RED, color->GREEN, color->BLUE);
//     shader->setVec3("lightPos", cameraPos.x, cameraPos.y, cameraPos.z + 100);
//     shader->setVec3("viewPos", cameraPos.x, cameraPos.y, cameraPos.z);
//     shader->setVec3("lightColor", 1.0f, 0.8f, 0.8f);

//     // updateColorFactor();

//     std::cout << "Here we are" << std::endl;
//     if (tick < startTime)
//         return;

//     drawVertices = points.size() / 6;
//     glBindVertexArray(VAO);
//     if (tick >= startTime && tick < (startTime + duration))
//     {
//         // Compute drawing progress
//         float progress = float(tick - startTime) / float(duration);
//         progress = std::clamp(progress, 0.0f, 1.0f);

//         // Number of vertices to draw
//         drawVertices = int(progress * drawVertices);
//         // std::cout << "initializing: " << drawVertices << std::endl;

//         if (drawVertices <= 1)
//             return; // avoids GL crash on LINE_STRIP

//         drawTick(tick);
//     }
//     else
//     {
//         if (!remove_after_draw)
//         {
//             drawTick(tick);
//         }
//     }
// }

// void Polygon::draw(float tick)
// {
//     // ... existing shader setup ...

//     float progress = std::clamp((tick - startTime) / duration, 0.0f, 1.0f);
//     shader->setFloat("uProgress", progress);

//     glBindVertexArray(VAO);
//     // We draw as lines so the Geometry Shader can process each edge
//     glDrawArrays(GL_LINE_STRIP, 0, sides + 1);
// }

// void Polygon::draw(float tick)
// {
//     if (!initialized)
//         init();
//     if (!shader)
//         return;

//     shader->use();

//     // 1. Update Matrices (Crucial for GS to work)
//     view = glm::lookAt(glm::vec3(cameraPos.x, cameraPos.y, cameraPos.z),
//                        glm::vec3(0.0f, 0.0f, 0.0f),
//                        glm::vec3(0.0f, 1.0f, 0.0f));

//     shader->setMat4("model", model);
//     shader->setMat4("view", view);
//     shader->setMat4("projection", projection);

//     // 2. Calculate Progress
//     float progress = std::clamp((tick - startTime) / duration, 0.0f, 1.0f);
//     shader->setFloat("uProgress", progress);
//     shader->setInt("vertexCount", (getSize() / 9));

//     // std::cout << "progress: " << progress << std::endl;

//     // 3. Set Lighting/Color Uniforms
//     GraphColor *color = this->getColor();
//     shader->setVec3("objectColor", color->RED, color->GREEN, color->BLUE);
//     shader->setVec3("lightColor", 1.0f, 0.8f, 0.8f);
//     shader->setVec3("lightPos", cameraPos.x, cameraPos.y, cameraPos.z);
//     shader->setVec3("viewPos", cameraPos.x, cameraPos.y, cameraPos.z);
//     shader->setFloat("u_thickness", 1.0f);
//     shader->setFloat("u_radius", 2.0f);
//     shader->setVec2("uViewportSize", 1200, 600);

//     // 4. DRAW
//     glBindVertexArray(VAO);
//     // Use GL_LINE_STRIP so GS receives segm
//     glDrawArrays(GL_LINE_STRIP, 0, getSize() / 9);
// }

// void Polygon::drawTick(float tick)
// {
//     glEnable(GL_STENCIL_TEST);
//     glClear(GL_STENCIL_BUFFER_BIT);

//     // --- PASS 1: CUT THE MASK ---
//     // Use the complex, overlapping concave points
//     glDepthMask(GL_FALSE);
//     glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);

//     glStencilFunc(GL_ALWAYS, 0, 0xFF);
//     glStencilOp(GL_KEEP, GL_KEEP, GL_INVERT); // The "Even-Odd" magic

//     glBindVertexArray(this->VAO);
//     glDrawArrays(GL_TRIANGLE_FAN, 0, drawVertices);

//     // --- PASS 2: PAINT THE COLOR ---
//     // Use the simple, non-overlapping Quad
//     glDepthMask(GL_TRUE);
//     glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);

//     // Only paint where the stencil is NOT zero
//     glStencilFunc(GL_NOTEQUAL, 0, 0xFF);
//     glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);

//     glBindVertexArray(quadVAO);
//     // TRIANGLE_STRIP for the 4 points of the quad
//     glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

//     glDisable(GL_STENCIL_TEST);
// }

// void Polygon::onMouseMoveCallback(MouseEvent event)
// {
//     if (event.key == GLFW_MOUSE_BUTTON_LEFT && event.action == GLFW_PRESS)
//     {
//         // First click initialization
//         if (lastX < 0 || lastY < 0)
//         {
//             lastX = event.positionX;
//             lastY = event.positionY;
//             return;
//         }

//         float dx = event.positionX - lastX;
//         float dy = event.positionY - lastY;

//         lastX = event.positionX;
//         lastY = event.positionY;

//         // Map full screen drag → full rotation
//         float rotPerPixelX = 360.0f / event.windowHeight;
//         float rotPerPixelY = 360.0f / event.windowWidth;

//         rotX += dy * rotPerPixelX; // vertical drag → X axis
//         rotY += dx * rotPerPixelY; // horizontal drag → Y axis

//         // Optional: keep values bounded
//         rotX = fmod(rotX, 360.0f);
//         rotY = fmod(rotY, 360.0f);

//         model = glm::mat4(1.0f);
//         model = glm::rotate(model, glm::radians(rotX), glm::vec3(1, 0, 0));
//         model = glm::rotate(model, glm::radians(rotY), glm::vec3(0, 1, 0));
//     }
//     else
//     {
//         lastX = -1;
//         lastY = -1;
//     }
// }

// void Polygon::onKeyPressedOnceCallback(const KeyEvent &event)
// {
//     if (event.key == GLFW_KEY_F && cameraPos.z > 0)
//     {
//         cameraPos.z -= 5.0f;
//     }
//     else if (event.key == GLFW_KEY_B)
//     {
//         cameraPos.z += 5.0f;
//     }
//     else if (event.key == GLFW_KEY_UP)
//     {
//         cameraPos.y += 5.0f;
//     }
//     else if (event.key == GLFW_KEY_DOWN)
//     {
//         cameraPos.y -= 5.0f;
//     }
//     else if (event.key == GLFW_KEY_LEFT)
//     {
//         cameraPos.x -= 5.0f;
//     }
//     else if (event.key == GLFW_KEY_R)
//     {
//         cameraPos.x += 5.0f;
//     }
// }

// void Polygon::updateColorFactor()
// {
//     this->shader->setVec3("color_factor", color_fade_factor);
// }

// void Polygon::generate()
// {
//     add(glm::vec3(-10, 10, 0));
//     add(glm::vec3(0, 0, 1.0f));
//     add(glm::vec3(10, 10, 0));
//     add(glm::vec3(0, 0, 1.0f));
//     add(glm::vec3(10, 10, 0));
//     add(glm::vec3(0, 0, 1.0f));
//     add(glm::vec3(0, -10, 0));
//     add(glm::vec3(0, 0, 1.0f));
//     add(glm::vec3(0, -10, 0));
//     add(glm::vec3(0, 0, 1.0f));
//     add(glm::vec3(-10, -15, 0));
//     add(glm::vec3(0, 0, 1.0f));
//     add(glm::vec3(-10, -15, 0));
//     add(glm::vec3(0, 0, 1.0f));
//     add(glm::vec3(15, 5, 0));
//     add(glm::vec3(0, 0, 1.0f));
//     add(glm::vec3(15, 5, 0));
//     add(glm::vec3(0, 0, 1.0f));
//     add(glm::vec3(-15, 3, 0));
//     add(glm::vec3(0, 0, 1.0f));
//     add(glm::vec3(-15, 3, 0));
//     add(glm::vec3(0, 0, 1.0f));
//     add(glm::vec3(-10, 10, 0));
//     add(glm::vec3(0, 0, 1.0f));
//     add(glm::vec3(-10, 10, 0));
//     add(glm::vec3(0, 0, 1.0f));
// }
