#ifndef __TRANSLATE_HPP__
#define __TRANSLATE_HPP__

#include "Animation.hpp"

class Translate: public Animation{
    public:
        glm::vec3 to;
        float x;
        float y;
        float width;
        float height;
        glm::vec3 offset;
        Position pivot;
        glm::vec3 current = glm::vec3(0, 0, 0);
        float lastProgress = 0;

    public:
        Translate(GraphObject *object, glm::vec3 to, float start_time = 0.0f, float duration = 1.0f, Position pos = Position::TOP_LEFT);
        Translate(GraphObject *object, Position to = Position::NONE, float start_time = 0.0f, float duration = 1.0f, Position pos = Position::TOP_LEFT);

        void Init();

        void play(float dt);

        glm::vec3 getPivotPosition(Position);
};

class Scale: public Animation{
    public:
        glm::vec3 targetScale;
        glm::vec3 current_scale = glm::vec3(1, 1, 1);
        glm::vec3 startScale = glm::vec3(1, 1, 1);

    public:
        Scale(GraphObject *object, glm::vec3 scale_factor, float start_time = 0.0f, float duration = 1.0f);
        void play(float dt);
};


class Rotation: public Animation{
    public :
        glm::vec3 rot_pivot;
        glm::vec3 rot_amount;
        float x, y, width, height;
        glm::vec3 startRot = glm::vec3(0, 0, 0);
        glm::vec3 currentRot = glm::vec3(0, 0, 0);
        glm::vec3 total = glm::vec3(0, 0, 0);
        Position pos;
        bool is_initialized = false;
        bool calc_pivot = true;

    public:
        Rotation(GraphObject *object, glm::vec3 rot_amount = glm::vec3(0, 0, 45), Position pos = Position::BOTTOM_LEFT, float start_time = 0.0f, float duration = 3.0f);
        Rotation(GraphObject *object, glm::vec3 rot_amount = glm::vec3(0, 0, 45), glm::vec3 pivot = glm::vec3(0, 0, 0), float start_time = 0.0f, float duration = 3.0f);
        void Init();
        glm::vec3 getPivotPosition(Position pos);
        void play(float dt);
};

#endif