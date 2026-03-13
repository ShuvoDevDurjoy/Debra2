#pragma once

#include <GraphEngine/Graphs/GraphObjects/GraphMathObject.hpp>


class Renderer{

    public: 
        void drawLineStrip(GraphMathObject* GMObject, float dt){
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            GMObject->updateStroke(dt);
        }
        void drawFilledPolygon(GraphMathObject* GMObject){
            glEnable(GL_STENCIL_TEST);
            glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE); // no colour output
            glDepthMask(GL_FALSE);
            glStencilMask(0xFF);
            glClear(GL_STENCIL_BUFFER_BIT);
            glStencilFunc(GL_ALWAYS, 0, 0xFF);
            // Increment for CW, decrement for CCW → non-zero fill rule
            glStencilOpSeparate(GL_FRONT, GL_KEEP, GL_KEEP, GL_INCR_WRAP);
            glStencilOpSeparate(GL_BACK, GL_KEEP, GL_KEEP, GL_DECR_WRAP);
            glDisable(GL_CULL_FACE);
        }

};