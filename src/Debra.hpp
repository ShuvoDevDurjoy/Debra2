#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <GraphEngine/Scene/Graph.hpp>

#include <GraphEngine/Graphs/GraphObjects/TestObject.hpp>
#include <GraphEngine/Graphs/GraphObjects/GraphObject.hpp>
#include <GraphEngine/Graphs/GraphObjects/ThreeDObject.hpp>
#include <GraphEngine/Graphs/GraphObjects/GraphMathObject.hpp>

#include <GraphEngine/Graphs/GraphObjects/Graphing/Line.hpp>
#include <GraphEngine/Graphs/GraphObjects/Graphing/GridBox.hpp>
#include <GraphEngine/Graphs/GraphObjects/Graphing/NumberLine.hpp>
#include <GraphEngine/Graphs/GraphObjects/Graphing/NumberPlane.hpp>

#include <GraphEngine/Graphs/GraphObjects/Shapes/Star.hpp>
#include <GraphEngine/Graphs/GraphObjects/Shapes/Text.hpp>
#include <GraphEngine/Graphs/GraphObjects/Shapes/Arrow.hpp>
#include <GraphEngine/Graphs/GraphObjects/Shapes/Circle.hpp>
#include <GraphEngine/Graphs/GraphObjects/Shapes/Polygon.hpp>
#include <GraphEngine/Graphs/GraphObjects/Shapes/Rectangle.hpp>
#include <GraphEngine/Graphs/GraphObjects/Shapes/BasePolygon.hpp>

#include <GraphEngine/Animations/Fade.hpp>
#include <GraphEngine/Animations/Creation.hpp>
#include <GraphEngine/Animations/Translate.hpp>
#include <GraphEngine/Animations/Animation.hpp>
#include <GraphEngine/Animations/Transition.hpp>
#include <GraphEngine/Animations/CameraAnimation.hpp>

#include <GraphEngine/Camera/Camera.hpp>

#include <GraphEngine/Math/Var.hpp>
#include <GraphEngine/Math/Vec2.hpp>
#include <GraphEngine/Math/Vec3.hpp>
