#ifndef __CAMERA_ANIMATION_HPP__
#define __CAMERA_ANIMATION_HPP__

#include <GraphEngine/Scene/Scene.hpp>
#include <GraphEngine/Camera/Camera.hpp>
#include <GraphEngine/Animations/Animation.hpp>

class CameraAnimation {
    friend class Scene;
    friend class ICameraAnimatable;

private:
    Camera* camera;
    glm::vec3 startPos, targetPos;
    glm::vec3 startCenter, targetCenter;
    
    float start_time;
    float duration;
    float end_time;
    
    bool isOrbit = false;
    float orbitAngle;
    float lastOrbitAngle = 0.0f;
    glm::vec3 orbitAxis;

public:
    CameraAnimation(Camera* cam, glm::vec3 tPos, glm::vec3 tCenter);
    
    static CameraAnimation* CreateOrbit(Camera* cam, float angle, glm::vec3 axis);
    
    void play(float dt);
    bool isDone(float dt) const { return dt >= end_time; }
};

#endif
