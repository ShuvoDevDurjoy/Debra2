// #include "../../include/GraphEngine/Objects/Rectangle.hpp"

// Rectangle::Rectangle()
// {
//     this->shader = new Shader(vertexShaderPath, geometricShaderPath, fragmentShaderPath);

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
//         glm::vec3(0.0f, 1.0f, 1.0f));

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

//     generate();
// }

// void Rectangle::init()
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

//     glBufferData(GL_ARRAY_BUFFER, getSize() * sizeof(float),
//                  points.data(), GL_STATIC_DRAW);

//     // Vertex layout: 3 floats per vertex (x, y, z)
//     glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)0);
//     glEnableVertexAttribArray(0);
//     glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)(3 * sizeof(float)));
//     glEnableVertexAttribArray(1);

//     glBindBuffer(GL_ARRAY_BUFFER, 0);
//     glBindVertexArray(0);

//     remove_after_draw = false;

//     initialized = true;
// }

// void Rectangle::draw(float tick)
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

// void Rectangle::drawTick(float tick)
// {
//     glDrawArrays(GL_LINE_STRIP, 0, drawVertices);
// }

// void Rectangle::onMouseMoveCallback(MouseEvent event)
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

// void Rectangle::onKeyPressedOnceCallback(const KeyEvent &event)
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

// void Rectangle::updateColorFactor()
// {
//     this->shader->setVec3("color_factor", color_fade_factor);
// }

// void Rectangle::generate()
// {
//     // int perSide = fragments / 4;
//     // points.clear();

//     // // 1. Top side (Left to Right)
//     // for (int i = 0; i < perSide; ++i)
//     // {
//     //     float t = (float)i / perSide;
//     //     add(glm::vec3(x + (t * width), y, 0));
//     //     add(glm::vec3(0, 0, 1));
//     // }
//     // // 2. Right side (Top to Bottom)
//     // for (int i = 0; i < perSide; ++i)
//     // {
//     //     float t = (float)i / perSide;
//     //     add(glm::vec3(x + width, y - (t * height), 0));
//     //     add(glm::vec3(0, 0, 1));
//     // }
//     // // 3. Bottom side (Right to Left)
//     // for (int i = 0; i < perSide; ++i)
//     // {
//     //     float t = (float)i / perSide;
//     //     add(glm::vec3(x + width - (t * width), y - height, 0));
//     //     add(glm::vec3(0, 0, 1));
//     // }
//     // // 4. Left side (Bottom to Top)
//     // for (int i = 0; i < perSide; ++i)
//     // {
//     //     float t = (float)i / perSide;
//     //     add(glm::vec3(x, y - height + (t * height), 0));
//     //     add(glm::vec3(0, 0, 1));
//     // }
//     // // Final point to close the gap
//     // add(glm::vec3(x, y, 0));
//     // add(glm::vec3(0, 0, 1));

//     add(glm::vec3(-10, 10, 0));
//     add(glm::vec3(0, 0, 1));
//     add(glm::vec3(10, 10, 0));
//     add(glm::vec3(0, 0, 1));
//     add(glm::vec3(0, -10, 0));
//     add(glm::vec3(0, 0, 1));
//     add(glm::vec3(-10, -15, 0));
//     add(glm::vec3(0, 0, 1));
//     add(glm::vec3(15, 5, 0));
//     add(glm::vec3(0, 0, 1));
//     add(glm::vec3(-15, 3, 0));
//     add(glm::vec3(0, 0, 1));
//     add(glm::vec3(-10, 10, 0));
//     add(glm::vec3(0, 0, 1));
// }
