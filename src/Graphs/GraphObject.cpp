#include "../../include/GraphEngine/Graphs/GraphObject.hpp"

void singletonGraph::init()
{
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, getSize() * sizeof(float), points.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);

    glGenBuffers(1, &MBO);
    glBindBuffer(GL_ARRAY_BUFFER, MBO);
    glBufferData(GL_ARRAY_BUFFER, getSize() * sizeof(float), morphPoints.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(1);

    std::cout << "start time: " << startTime << " duration: " << duration << " morph start: " << morphStartTime << " and morph end: " << (morphStartTime + morphDuration) << std::endl;
}

// void singletonGraph::draw(float tick)
// {
//     // Not time to start yet
//     if (tick < startTime)
//         return;

//     remove_after_draw = false;
//     shader->setVec3("color", color->RED, color->GREEN, color->BLUE);

//     drawVertices = points.size() / 2;
//     glBindVertexArray(VAO);
//     if (tick >= startTime && tick < (startTime + duration))
//     {
//         // Compute drawing progress
//         float progress = float(tick - startTime) / float(duration);
//         progress = std::clamp(progress, 0.0f, 1.0f);

//         // Number of vertices to draw
//         drawVertices = int(progress * drawVertices);
//         // std::cout << "initializing: " << drawVertices << std::endl;

//         if (drawVertices <= 1)
//             return; // avoids GL crash on LINE_STRIP

//         draw();
//     }
//     else {
//         if(! remove_after_draw){
//             draw();
//         }
//     }
//     if (morphGraphs.size())
//     {
//         for (int i = 0; i < int(morphGraphs.size()); ++i)
//         {
//             if (morphStart[i] <= tick && tick <= morphEnd[i])
//             {
//                 glBindBuffer(GL_ARRAY_BUFFER, MBO);
//                 glBufferData(GL_ARRAY_BUFFER, getSize() * sizeof(float), morphGraphs[i]->points.data(), GL_STATIC_DRAW);
//                 glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void *)0);
//                 glEnableVertexAttribArray(1);

//                 float morph_progress = float(tick - morphStart[i]) / float(morphEnd[i] - morphStart[i]);
//                 morph_progress = std::clamp(morph_progress, 0.0f, 1.0f);

//                 // std::cout << "Morphing: " << morph_progress << std::endl;
//                 shader->setBool("is_morphing", true);
//                 shader->setFloat("morph_progress", morph_progress);
//                 draw();
//             }
//         }
//     }
// }

// void singletonGraph::draw()
// {
//     glDrawArrays(GL_LINE_STRIP, 0, drawVertices);
//     shader->setFloat("morph_progress", 0.0);
//     shader->setBool("is_morphing", false);
// }