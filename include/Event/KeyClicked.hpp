#ifndef __KEY_CLICKED_HPP__
#define __KEY_CLICKED_HPP__

#include "../include/glfw3.h"
#include <unordered_map>

struct KeyEvent
{
    int key;
    GLFWwindow *window;
};

struct ModifiedKeyEvent
{
    int key;
    int mods; // Bitmask using GLFW_MOD_SHIFT, GLFW_MOD_CONTROL, GLFW_MOD_ALT, etc.
    GLFWwindow *window;
};

class KeyClicked
{
private:
    std::unordered_map<int, bool> keyStates;

    bool isModifierKey(int key)
    {
        return key == GLFW_KEY_LEFT_SHIFT || key == GLFW_KEY_RIGHT_SHIFT ||
               key == GLFW_KEY_LEFT_CONTROL || key == GLFW_KEY_RIGHT_CONTROL ||
               key == GLFW_KEY_LEFT_ALT || key == GLFW_KEY_RIGHT_ALT;
    }

    int getModifierMask(GLFWwindow *window)
    {
        int mods = 0;
        if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS ||
            glfwGetKey(window, GLFW_KEY_RIGHT_SHIFT) == GLFW_PRESS)
            mods |= GLFW_MOD_SHIFT;

        if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS ||
            glfwGetKey(window, GLFW_KEY_RIGHT_CONTROL) == GLFW_PRESS)
            mods |= GLFW_MOD_CONTROL;

        if (glfwGetKey(window, GLFW_KEY_LEFT_ALT) == GLFW_PRESS ||
            glfwGetKey(window, GLFW_KEY_RIGHT_ALT) == GLFW_PRESS)
            mods |= GLFW_MOD_ALT;

        return mods;
    }

public:
    virtual ~KeyClicked() = default;

    virtual void onKeyPressedOnceCallback(const KeyEvent &event) = 0;
    virtual void onModifiedKeyPressedOnceCallback(const ModifiedKeyEvent &event) {}

    // Handles single key presses (not modifiers), and only when no other key is pressed
    void onceKeyPressedPollEvents(GLFWwindow *window)
    {
        int pressedKey = -1;
        int pressedCount = 0;

        for (int key = GLFW_KEY_SPACE; key <= GLFW_KEY_LAST; ++key)
        {
            if (glfwGetKey(window, key) == GLFW_PRESS && !isModifierKey(key))
            {
                pressedKey = key;
                pressedCount++;
            }
        }

        if (pressedCount == 1 && !keyStates[pressedKey])
        {
            onKeyPressedOnceCallback({pressedKey, window});
        }

        for (int key = GLFW_KEY_SPACE; key <= GLFW_KEY_LAST; ++key)
        {
            keyStates[key] = (glfwGetKey(window, key) == GLFW_PRESS);
        }
    }
};

#endif
