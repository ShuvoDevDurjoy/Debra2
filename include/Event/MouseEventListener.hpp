#pragma once

#include <Event/MouseEvent.hpp>
#include <Event/EventListener.hpp>

class MouseEventListener : public EventListener<MouseEvent> {
public:
    virtual ~MouseEventListener() = default;
    virtual void onEvent(MouseEvent& event) = 0;
};