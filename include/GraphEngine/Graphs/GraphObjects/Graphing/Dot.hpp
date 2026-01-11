// #ifndef __DOT_HPP__
// #define __DOT_HPP__

// #include "../GraphObject.hpp"

// class Dot: public GraphObject{
//     public:
//         float radius;
//         glm::vec2 center;

//     public:
//         Dot(glm::vec2 center = glm::vec2(0, 0), float radius = 1){
//             this->center = center;
//             this->radius = radius;
//             generatePoints();
//         }

//     public:
//         void generatePoints(){
//             for (int i = 0; i <= 360; ++i){
//                 float x = radius * cos(glm::radians(i * 1.0f));
//                 float y = radius * sin(glm::radians(i * 1.0f));
//                 setPoints(glm::vec3(x, y, 0));
//             }
//             setDimension();
//         }
// };

// #endif