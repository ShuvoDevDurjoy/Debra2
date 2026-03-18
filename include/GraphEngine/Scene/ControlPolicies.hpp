#pragma once

#include <Event/KeyEventListener.hpp>
#include <Event/MouseEventListener.hpp>
#include <memory>

class CameraAnimation;
class TouchpadControlStrategy;

/**
 * Capability interface for scenes that support camera animations.
 */
class ICameraAnimatable {
public:
    virtual ~ICameraAnimatable() = default;
    void play(CameraAnimation* camera_animation, float start_time, float duration);
};

/**
 * Policy for Manual Control:
 * Supports Keyboard and Mouse events.
 */
class ManualControl : public KeyEventListener, public MouseEventListener {
private:
    std::unique_ptr<TouchpadControlStrategy> manualStrategy;

    public:
    ManualControl();
    virtual ~ManualControl();
    void onEvent(KeyEvent& event) override;
    void onEvent(MouseEvent& event) override;
};

/**
 * Policy for Camera Control:
 * Supports Camera Animations.
 */
class CameraControl : public ICameraAnimatable {
public:
    virtual ~CameraControl() = default;
};
