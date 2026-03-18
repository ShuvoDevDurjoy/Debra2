#include <cmath>
#include <algorithm>
#include <GraphEngine/Camera/Camera.hpp>
#include <GraphEngine/Graphs/GraphObjects/GraphMathObject.hpp>

Camera::Camera(glm::vec3 pos, glm::vec3 target, glm::vec3 upDir)
    : position(pos), center(target), up(upDir), 
      fov(45.0f), aspect(2.0f), nearPlane(0.1f), farPlane(1000.0f) {
    updateMatrices();
}

void Camera::updateMatrices() {
    viewMatrix = glm::lookAt(position, center, up);
    projectionMatrix = glm::perspective(glm::radians(fov), aspect, nearPlane, farPlane);
}

void Camera::setPerspective(float fovDeg, float aspectRatio, float nearP, float farP) {
    this->fov = fovDeg;
    this->aspect = aspectRatio;
    this->nearPlane = nearP;
    this->farPlane = farP;
    updateMatrices();
}

void Camera::setAspect(float aspectRatio) {
    this->aspect = aspectRatio;
    updateMatrices();
}

void Camera::focusOn(GraphMathObject* obj, float padding) {
    float objCenterX = obj->x + obj->width / 2.0f;
    float objCenterY = obj->y - obj->height / 2.0f;
    center = glm::vec3(objCenterX, objCenterY, 0.0f);
    
    float maxDim = std::max(obj->width, obj->height);
    if (maxDim < 0.1f) maxDim = 10.0f; 
    
    float dist = (maxDim * padding) / (2.0f * tan(glm::radians(fov) * 0.5f));
    position = glm::vec3(objCenterX, objCenterY, dist);
    
    updateMatrices();
}

void Camera::rotateAround(glm::vec3 target, float angleDeg, glm::vec3 axis) {
    glm::mat4 rotation = glm::rotate(glm::mat4(1.0f), glm::radians(angleDeg), axis);
    glm::vec3 relativePos = position - target;
    position = glm::vec3(rotation * glm::vec4(relativePos, 1.0f)) + target;
    center = target;
    updateMatrices();
}

void Camera::zoom(float factor) {
    glm::vec3 relativePos = position - center;
    position = center + relativePos * factor;
    updateMatrices();
}