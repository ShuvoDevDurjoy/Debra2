#pragma once

#include <Event/Event.hpp>
#include <glfw3.h>

class KeyEvent : public Event {
public:
    int key;
    int scancode;
    int action;
    int mods;
    GLFWwindow* window;

    KeyEvent(int k, int s, int a, int m, GLFWwindow* w)
        : key(k), scancode(s), action(a), mods(m), window(w) {}
};
