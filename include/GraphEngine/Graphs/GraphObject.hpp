// // include/GraphEngine/Graphs/GraphObject.hpp
// #pragma once

// // Small note: always include glad BEFORE glfw
// // so glad provides GL prototypes and loader macros.
// #include "../include/glad.h"
// #include "../include/glfw3.h"

// #include "../Utils/GraphUtilities.hpp"
// #include "../Utils/GraphColor.hpp"
// #include "../Core/Shader.hpp"
// #include "../Utils/AnimationMode.hpp"
// #include "../Math/Vec3.hpp"

// #include <vector>
// #include <algorithm>
// #include <utility>
// #include <iostream>
// class singletonGraph
// {
// private:
//     std::vector<float> morphPoints;
//     std::vector<singletonGraph *> morphGraphs;
//     std::vector<float> morphStart;
//     std::vector<float> morphEnd;
//     int rangeSize = 0;

// protected:
//     float x, y, width, height;
//     GraphColor *color;
//     GLuint VAO = 0, VBO = 0, MBO = 0;
//     bool remove_after_draw = true;
//     std::vector<float> points;
//     Shader *shader = nullptr;
//     float startTime = 0;
//     float duration = 1.0f;
//     float delay = 1.0f;
//     float loopTime = 1.0f;
//     float morphStartTime = 5.0f;
//     float morphDelay = 1.0f;
//     float morphDuration = 3.0f;
//     int drawVertices = 0;
//     glm::vec3 color_fade_factor = glm::vec3(1.0f, 1.0f, 1.0f);

// private:
//     float line_width = 2.0f;
//     float scale_factor = 1.0f;
//     bool scale_with_zoom = true;

// public:
//     AnimationMode ANIMATION_MODE = AnimationMode::INFINITE;

//     int startIndex = 0, endIndex = 0;

//     virtual void init();

//     void morphGraph(singletonGraph *mGraph, float start, float duration)
//     {
//         // morphPoints = mGraph->points;
//         // morphStartTime = start;
//         // morphDuration = duration;

//         morphGraphs.push_back(mGraph);
//         morphStart.push_back(start);
//         morphEnd.push_back(start + duration);
//     }

//     void setMorphPoints(const std::vector<float> &target)
//     {
//         morphPoints = target;
//         if (morphPoints.size() != points.size())
//         {
//             std::cout << "Size is: " << morphPoints.size() << " and " << points.size() << std::endl;
//             std::cerr << "Morph points size mismatch!\n";
//         }
//     }
//     bool hasMorph() const { return !morphPoints.empty(); }

//     std::pair<float, float> interpolateVertex(int idx, float alpha) const
//     {
//         float x1 = points[2 * idx], y1 = points[2 * idx + 1];
//         float x2 = morphPoints[2 * idx], y2 = morphPoints[2 * idx + 1];
//         return {(1.0f - alpha) * x1 + alpha * x2,
//                 (1.0f - alpha) * y1 + alpha * y2};
//     }

// private:
//     friend class Graph;

// public:
//     singletonGraph() {}
//     singletonGraph(Shader *shader)
//     {
//         this->shader = shader;
//     }
//     singletonGraph(Shader *shader, float startTime, float duration, float delay, float loopTime, float morphDuration)
//     {
//         this->startTime = startTime;
//         this->duration = duration;
//         this->delay = delay;
//         this->loopTime = loopTime;
//         this->morphDuration = morphDuration;
//         this->shader = shader;
//     }

//     Shader *getShader()
//     {
//         return this->shader;
//     }

//     void setShader(Shader *shader)
//     {
//         this->shader = shader;
//     }

//     int getSize()
//     {
//         return points.size();
//     }

//     void setColor(GraphColor *c)
//     {
//         this->color = c;
//     }

//     GraphColor *getColor()
//     {
//         return color;
//     }

//     void setRangeSize(int rs)
//     {
//         this->rangeSize = rs;
//     }

//     int getRangeSize()
//     {
//         return rangeSize;
//     }

//     void add(std::pair<float, float> p)
//     {
//         points.push_back(p.first);
//         points.push_back(p.second);
//     }

//     void add(float x)
//     {
//         points.push_back(x);
//     }

//     void add(glm::vec3 v)
//     {
//         points.push_back(v[0]);
//         points.push_back(v[1]);
//         points.push_back(v[2]);
//     }

//     void add(Vec3 vec)
//     {
//         points.push_back(vec.x);
//         points.push_back(vec.y);
//         points.push_back(vec.z);
//     }

//     void setAnimationMode(AnimationMode M)
//     {
//         this->ANIMATION_MODE = M;
//     }

//     void StartTime(float startTime)
//     {
//         this->startTime = startTime;
//     }
//     void Duration(float duration)
//     {
//         this->duration = duration;
//     }
//     void Delay(float delay)
//     {
//         this->delay = delay;
//     }

//     float getStartTime() { return this->startTime; }
//     float getDuration() { return this->duration; }
//     float getDelay() { return this->delay; }
//     float getLoopTime() { return this->loopTime; }
//     float getMorphDuration() { return hasMorph() ? this->morphDuration : 0; }
//     float getTotalDuration() { return getDuration() + getMorphDuration() + this->delay; }

//     void setColorFadeFactor(glm::vec3 color_factor)
//     {
//         this->color_fade_factor = color_factor;
//     }

//     virtual void draw(float tick) = 0;

//     virtual void drawTick(float tick) = 0;
// };