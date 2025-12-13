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

    void pollAll(GLFWwindow *window)
    {
        for (KeyClicked *k : listeners)
        {
            k->onceKeyPressedPollEvents(window);
        }
    }
};

#endif