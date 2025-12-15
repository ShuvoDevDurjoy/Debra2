#ifndef __MOUSEEVENTS_HPP__
#define __MOUSEEVENTS_HPP__

#include "../include/glfw3.h"

struct MouseEvent{
    double positionX = 0.0f;
    double positionY = 0.0f;

    int windowWidth;
    int windowHeight;

    int key = -1;
    int action = -1;
    int mode = -1;

    MouseEvent(double xpos, double ypos, int width, int height, int k, int a, int m){
        positionX = xpos;
        positionY = ypos;
        windowWidth = width;
        windowHeight = height;
        key = k;
        action = a;
        mode = m;
    }
};



class MouseEvents
{
    private:
        int key = -1;
        int action = -1;
        int mode = -1;
        double xpos = 0;
        double ypos = 0;

        int width = 0;
        int height = 0;

    public:
        virtual ~MouseEvents() {}
        virtual void onMouseMoveCallback(MouseEvent) = 0;
        virtual void onMouseClickCallback(MouseEvent) = 0;
        // virtual void onScrollCallback(GLFWwindow *window, double xOffSet, double yOffSet);

        void onMouseMoved(double xpos, double ypos, int width, int height){
            this->width = width;
            this->height = height;
            this->xpos = xpos;
            this->ypos = ypos;
            MouseEvent mouseEvent(this->xpos, this->ypos, this->width, this->height, this->key, this->action, this->mode);
            onMouseMoveCallback(mouseEvent);
        }

        void onMouseClick(int key, int action, int mode){
            this->key = key;
            this->action = action;
            this->mode = mode;
            MouseEvent mouseEvent(this->xpos, this->ypos, this->width, this->height, this->key, this->action, this->mode);
            onMouseClickCallback(mouseEvent);
        }
};

#endif