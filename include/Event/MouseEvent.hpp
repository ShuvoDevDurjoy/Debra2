#pragma once

#include <Event/Event.hpp>

class MouseEvent : public Event {
public:
    enum class Type {
        MOVE,
        CLICK,
        SCROLL
    };

    Type type;
    double xpos, ypos;
    int button, action, mods;
    double xoffset, yoffset;

    // constructor for Move
    MouseEvent(Type t, double x, double y)
        : type(t), xpos(x), ypos(y), button(-1), action(-1), mods(-1), xoffset(0), yoffset(0) {}

    // constructor for Click
    MouseEvent(int b, int a, int m, double x, double y)
        : type(Type::CLICK), xpos(x), ypos(y), button(b), action(a), mods(m), xoffset(0), yoffset(0) {}

    // constructor for Scroll
    MouseEvent(double xo, double yo, Type t)
        : type(t), xpos(0), ypos(0), button(-1), action(-1), mods(-1), xoffset(xo), yoffset(yo) {}
};
