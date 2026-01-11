// #ifndef __INTERSECTION_HELPER_HPP__
// #define __INTERSECTION_HELPER_HPP__

// #include "../../glm/glm.hpp"
// #include "../../glm/gtc/matrix_transform.hpp"

// float crosssecond(const glm::vec2 &a, const glm::vec2 &b)
// {
//     return a.x * b.y - a.y * b.x;
// }

// bool intersectLines(
//     const glm::vec2 &P, const glm::vec2 &P2,
//     const glm::vec2 &Q, const glm::vec2 &Q2,
//     glm::vec2 &intersection)
// {
//     glm::vec2 R = P2 - P;
//     glm::vec2 S = Q2 - Q;

//     float rxs = crosssecond(R, S);
//     float q_pxr = crosssecond(Q - P, R);

//     // Parallel or collinear
//     if (std::abs(rxs) < 1e-8f)
//         return false;

//     float t = crosssecond(Q - P, S) / rxs;
//     float u = q_pxr / rxs;

//     intersection = P + t * R;
//     return true;
// }

// #endif