#ifndef __CAMERA_HPP__
#define __CAMERA_HPP__

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class GraphMathObject;

class Camera {
private:
    glm::vec3 position;
    glm::vec3 center;
    glm::vec3 up;
    
    float fov;
    float aspect;
    float nearPlane;
    float farPlane;

    glm::mat4 viewMatrix;
    glm::mat4 projectionMatrix;

public:
    Camera(glm::vec3 pos = glm::vec3(0, 0, 100), 
           glm::vec3 target = glm::vec3(0, 0, 0), 
           glm::vec3 upDir = glm::vec3(0, 1, 0));

    void updateMatrices();
    
    // Projection settings
    void setPerspective(float fovDeg, float aspectRatio, float nearP, float farP);
    void setAspect(float aspectRatio);

    // View manipulation
    void setPosition(glm::vec3 pos) { position = pos; updateMatrices(); }
    void setCenter(glm::vec3 target) { center = target; updateMatrices(); }
    
    glm::vec3 getPosition() const { return position; }
    glm::vec3 getCenter() const { return center; }
    glm::mat4 getViewMatrix() const { return viewMatrix; }
    glm::mat4 getProjectionMatrix() const { return projectionMatrix; }

    // Specialized Logic
    void focusOn(GraphMathObject* obj, float padding = 1.2f);
    void rotateAround(glm::vec3 target, float angleDeg, glm::vec3 axis);
};

#endif