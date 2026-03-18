#pragma once

#include <glfw3.h>
#include <glm/glm.hpp>
#include <GraphEngine/Scene/Scene.hpp>
#include <GraphEngine/Camera/Camera.hpp>
#include <GraphEngine/Scene/ControlStrategy.hpp>

#include <Event/KeyEventListener.hpp>
#include <Event/MouseEventListener.hpp>

class TouchpadControlStrategy : public ControlStrategy, public KeyEventListener, public MouseEventListener {
private:
    float lastX = -1, lastY = -1;

public:
    using ControlStrategy::ControlStrategy;

    void onEvent(KeyEvent& event) override {
        Camera* camera = scene->getCamera();
        if (!camera) return;

        glm::vec3 pos = camera->getPosition();
        glm::vec3 center = camera->getCenter();

        if (event.key == GLFW_KEY_UP) {
            center.y += 2.0f; pos.y += 2.0f;
        } else if (event.key == GLFW_KEY_DOWN) {
            center.y -= 2.0f; pos.y -= 2.0f;
        } else if (event.key == GLFW_KEY_LEFT) {
            center.x -= 2.0f; pos.x -= 2.0f;
        } else if (event.key == GLFW_KEY_RIGHT) {
            center.x += 2.0f; pos.x += 2.0f;
        } else if (event.key == GLFW_KEY_F || event.key == GLFW_KEY_W) {
            camera->zoom(0.9f); return;
        } else if (event.key == GLFW_KEY_B || event.key == GLFW_KEY_S) {
            camera->zoom(1.1f); return;
        }

        camera->setPosition(pos);
        camera->setCenter(center);
    }

    void onEvent(MouseEvent& event) override {
        Camera* camera = scene->getCamera();
        if (!camera) return;

        if (event.type == MouseEvent::Type::MOVE) {
            if (event.button == GLFW_MOUSE_BUTTON_LEFT && event.action == GLFW_PRESS) {
                if (lastX < 0 || lastY < 0) { lastX = event.xpos; lastY = event.ypos; return; }

                float dx = event.xpos - lastX;
                float dy = event.ypos - lastY;
                lastX = event.xpos;
                lastY = event.ypos;

                int width, height;
                glfwGetWindowSize(glfwGetCurrentContext(), &width, &height);
                float zDist = glm::length(camera->getPosition() - camera->getCenter());
                float panScale = zDist / (float)width;

                glm::vec3 center = camera->getCenter();
                center.x -= dx * panScale;
                center.y += dy * panScale;
                glm::vec3 pos = camera->getPosition();
                pos.x -= dx * panScale;
                pos.y += dy * panScale;
                camera->setCenter(center);
                camera->setPosition(pos);
            } else if (event.button == GLFW_MOUSE_BUTTON_RIGHT && event.action == GLFW_PRESS) {
                if (lastX < 0 || lastY < 0) { lastX = event.xpos; lastY = event.ypos; return; }
                float dx = event.xpos - lastX;
                float dy = event.ypos - lastY;
                lastX = event.xpos;
                lastY = event.ypos;

                float sensitivity = 0.5f;
                camera->rotateAround(camera->getCenter(), -dx * sensitivity, glm::vec3(0, 1, 0));
                camera->rotateAround(camera->getCenter(), -dy * sensitivity, glm::vec3(1, 0, 0));
            } else {
                lastX = -1;
                lastY = -1;
            }
        } else if (event.type == MouseEvent::Type::SCROLL) {
            float zoomFactor = (event.yoffset > 0) ? 0.9f : 1.1f;
            camera->zoom(zoomFactor);
        }
    }
};
