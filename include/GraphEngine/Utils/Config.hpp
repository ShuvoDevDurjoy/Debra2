#ifndef __CONFIG_HPP__
#define __CONFIG_HPP__

enum Position
{
    NONE,
    LEFT,
    RIGHT,
    TOP,
    BOTTOM,
    TOP_LEFT,
    TOP_RIGHT,
    BOTTOM_LEFT,
    BOTTOM_RIGHT,  
    CENTER,
};

#include <glm/glm.hpp>

extern const glm::vec3 ORIGIN;
extern const glm::vec3 UP;
extern const glm::vec3 DOWN;
extern const glm::vec3 LEFT_VEC;
extern const glm::vec3 RIGHT_VEC;
extern const glm::vec3 UL;
extern const glm::vec3 UR;
extern const glm::vec3 DL;
extern const glm::vec3 DR;

// Edge/Screen positions (updated by Scene based on xUnits)
extern glm::vec3 TOP_VEC;
extern glm::vec3 BOTTOM_VEC;
extern glm::vec3 LEFT_VEC_SIDE;
extern glm::vec3 RIGHT_VEC_SIDE;
extern glm::vec3 UL_VEC;
extern glm::vec3 UR_VEC;
extern glm::vec3 DL_VEC;
extern glm::vec3 DR_VEC;

#endif