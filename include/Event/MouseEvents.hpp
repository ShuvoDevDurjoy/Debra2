#ifndef __MOUSEEVENTS_HPP__
#define __MOUSEEVENTS_HPP__

#include "../include/glfw3.h"

struct MouseEvent{
    double positionX = 0.0f;
    double positionY = 0.0f;

    int windowWidth;
    int windowHeight;

    MouseEvent(double xpos, double ypos, int width, int height){
        positionX = xpos;
        positionY = ypos;
        windowWidth = width;
        windowHeight = height;
    }
};



class MouseEvents
{

    public:
        virtual ~MouseEvents() {}
        virtual void onMouseMoveCallback(MouseEvent) = 0;
        // virtual void onScrollCallback(GLFWwindow *window, double xOffSet, double yOffSet);

        void onMouseMoved(double xpos, double ypos, int width, int height){
            MouseEvent mouseEvent(xpos, ypos, width, height);
            onMouseMoveCallback(mouseEvent);
        }
};

#endif