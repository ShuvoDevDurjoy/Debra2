#pragma once

class Scene;

class ControlStrategy {
protected:
    Scene* scene;

public:
    ControlStrategy(Scene* s) : scene(s) {}
    virtual ~ControlStrategy() = default;
};
