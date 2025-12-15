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
    

        void mouseMoved(GLFWwindow* window, double xpos, double ypos){
            int width = 0, height = 0;
            glfwGetWindowSize(window, &width, &height);
            glfwGetCursorPos(window, &xpos, &ypos);
            for (MouseEvents *listener : listeners)
            {
                listener->onMouseMoved(xpos, ypos, width, height);
            }
        }

        void mouseClicked(GLFWwindow* window, int key, int action, int mode){
            for(MouseEvents *listener : listeners){
                listener->onMouseClick(key, action, mode);
            }
        }
};