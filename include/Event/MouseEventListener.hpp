#pragma once

#include "MouseEvents.hpp"
#include <vector>

class MouseEventListener{

    private:
        std::vector<MouseEvents *> listeners;

    public: 
        void registerListener(MouseEvents* listener){
            listeners.push_back(listener);
        }
    

        void pullAll(GLFWwindow* window){
            double xpos = 0, ypos = 0;
            int width = 0, height = 0;
            glfwGetWindowSize(window, &width, &height);
            glfwGetCursorPos(window, &xpos, &ypos);

            for (MouseEvents *listener : listeners)
            {
                listener->onMouseMoved(xpos, ypos, width, height);
            }
        }
};