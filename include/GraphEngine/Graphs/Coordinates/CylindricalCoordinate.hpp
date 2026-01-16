// #ifndef __CYLINDRICALCOORDINATE_HPP__
// #define __CYLINDRICALCOORDINATE_HPP__

// #include <cmath>

// #include "../GraphObject.hpp"
// #include "../../Math/Vec3.hpp"
// #include "../../Math/Var.hpp"
// #include "../Surface.hpp"
// #include "../../../glad.h"
// #include "../../glm/glm.hpp"
// #include "../../glm/gtc/matrix_transform.hpp"

// class CylindricalCoordinate
// {

// private:
//     float inner_radius = 0.0f;
//     float outer_radius = 10.0f;
//     float phi_start = 0.0f;
//     float phi_end = 2 * M_PI;
//     float shi_start = 0.0f;
//     float shi_end = M_PI;

//     int resolutionX = 10;
//     int resolutionY = 10;

//     float ortho[16] = {
//         0.2f, 0.0f, 0.0f, 0.0f,  // first column
//         0.0f, 0.2f, 0.0f, 0.0f,  // second column
//         0.0f, 0.0f, -0.2f, 0.0f, // third column
//         0.0f, 0.0f, 0.0f, 1.0f   // fourth column
//     };

// public:
//     CylindricalCoordinate() {}
//     CylindricalCoordinate(std::pair<float, float> radius, std::pair<float, float> phi)
//     {
//         inner_radius = radius.first;
//         outer_radius = radius.second;

//         phi_start = phi.first;
//         phi_end = phi.second;
//     }

//     void setResolutionR(float r){
//         resolutionX = r;
//     }

//     void setResolutionPhi(float phi){
//         resolutionY = phi;
//     }

//     Surface *drawSurface(Vec3 (*func)(float, float, Var), Var v);
// };

// #endif