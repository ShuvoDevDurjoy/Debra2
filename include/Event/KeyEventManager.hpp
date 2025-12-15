#ifndef __KEYEVENTMANAGER_HPP__
#define __KEYEVENTMANAGER_HPP__

#include "KeyClicked.hpp"
#include <vector>

class KeyEventManager
{

private:
    std::vector<KeyClicked *> listeners;

public:
    void registerListener(KeyClicked *keyclicked)
    {
        listeners.push_back(keyclicked);
    }

    void pollAll(GLFWwindow *window, int key, int scancode, int action, int mode)
    {
        for (KeyClicked *k : listeners)
        {
            k->onceKeyPressedPollEvents(window);
        }
    }
};

#endif