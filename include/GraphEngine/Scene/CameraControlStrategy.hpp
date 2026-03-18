#pragma once

#include <GraphEngine/Scene/ControlStrategy.hpp>

class CameraControlStrategy : public ControlStrategy {
public:
    using ControlStrategy::ControlStrategy;
    
    // Camera Control Strategy does not implement Key or Mouse listeners
    // as animations are added manually to the scene.
};
