#pragma once

#include <Event/KeyEvent.hpp>
#include <Event/EventListener.hpp>

class KeyEventListener : public EventListener<KeyEvent> {
public:
    virtual ~KeyEventListener() = default;
    virtual void onEvent(KeyEvent& event) = 0;
};
