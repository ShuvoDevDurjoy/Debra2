#include "../../include/GraphEngine/Animations/CameraAnimation.hpp"

float easeInOutSine(float t) {
    return -0.5f * (cos(3.14159265359f * t) - 1.0f);
}

CameraAnimation::CameraAnimation(Camera* cam, glm::vec3 tPos, glm::vec3 tCenter)
    : camera(cam), targetPos(tPos), targetCenter(tCenter) {
    startPos = cam->getPosition();
    startCenter = cam->getCenter();
    isOrbit = false;
}

CameraAnimation* CameraAnimation::CreateOrbit(Camera* cam, float angle, glm::vec3 axis) {
    CameraAnimation* anim = new CameraAnimation(cam, cam->getPosition(), cam->getCenter());
    anim->isOrbit = true;
    anim->orbitAngle = angle;
    anim->orbitAxis = axis;
    return anim;
}

void CameraAnimation::play(float dt) {
    if (dt < start_time) return;
    
    float progress = (dt - start_time) / duration;
    if (progress > 1.0f) progress = 1.0f;
    
    float t = easeInOutSine(progress);
    if (isOrbit) {
        camera->rotateAround(targetCenter, orbitAngle * progress - lastOrbitAngle, orbitAxis);
        lastOrbitAngle = orbitAngle * progress;
    } else {
        camera->setPosition(glm::mix(startPos, targetPos, t));
        camera->setCenter(glm::mix(startCenter, targetCenter, t));
    }
}
