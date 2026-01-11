    #ifndef __GRAPHMATHOBJECT_HPP__
    #define __GRAPHMATHOBJECT_HPP__

    #include <iostream>
    #include <algorithm>
    #include <vector>
    #include <climits>
    #include <cmath>
    #include <unordered_map>

    #include "../../Core/Shader.hpp"
    #include "../../../glm/glm.hpp"
    #include "../../../glm/gtc/matrix_transform.hpp"

    class GraphMathObject{
        public:
            virtual void Init(float) = 0;
            virtual void draw(float) = 0;
    };

    #endif