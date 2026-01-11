#include "../include/GraphEngine/Graphs/Graph.hpp"

#include "../include/GraphEngine/Graphs/Coordinates/CylindricalCoordinate.hpp"
#include "../include/GraphEngine/Graphs/Coordinates/CartesianCoordinate.hpp"
#include "../include/GraphEngine/Graphs/ThreeDAxes.hpp"
#include "../include/GraphEngine/Objects/Rectangle.hpp"
#include "../include/GraphEngine/Objects/Polygon.hpp"
#include "../include/GraphEngine/Graphs/GraphObjects/TestObject.hpp"
#include "../include/GraphEngine/Graphs/GraphObjects/GraphMathObject.hpp"
#include "../include/GraphEngine/Graphs/GraphObjects/ThreeDObject.hpp"
#include "../include/GraphEngine/Graphs/GraphObjects/Graphing/GridBox.hpp"
#include "../include/GraphEngine/Graphs/GraphObjects/Graphing/Dot.hpp"
#include "../include/GraphEngine/Graphs/GraphObjects/Shapes/Circle.hpp"
#include "../include/GraphEngine/Graphs/GraphObjects/Shapes/BasePolygon.hpp"
#include "../include/GraphEngine/Animations/Creation.hpp"
#include "../include/GraphEngine/Animations/Translate.hpp"
#include "../include/GraphEngine/Animations/Transition.hpp"

#include "../include/glm/glm.hpp"
#include "../include/glm/gtc/matrix_transform.hpp"


glm::vec3 generate(float r, float t, Var v){
    float x = r * 20.0f;
    float y = x * x;
    float z = x * cos(r);
    return glm::vec3(x, y, z);
}

glm::vec3 torus(float u, float v, Var vars)
{
    float R = 20; // major radius
    float r = 10; // minor radius

    float x = (R + r * cos(u)) * cos(v);
    float y = r * sin(u);
    float z = (R + r * cos(u)) * sin(v);

    return glm::vec3(x, y, z);
}

glm::vec3 paraboloid(float r, float theta, Var vars)
{
    float x = 10.0f * cos(theta);
    float y = 10.0f * sin(theta);
    float z = r ; // height proportional to r^2
    return glm::vec3(x, y, z);
}

glm::vec3 sphere(float r, float theta, Var vars)
{
    float R = 20;

    // r is polar angle φ
    float phi = r;

    float x = R * sin(phi) * cos(theta);
    float y = R * sin(phi) * sin(theta);
    float z = R * cos(phi);

    return glm::vec3(x, y, z);
}

int main(){
    Graph *graph = Graph::getInstance(0);

    // ThreeDAxes* three_d_axes = new ThreeDAxes();
    // graph->play(three_d_axes);

    ThreeDObject* td = new ThreeDObject();
    td->r_range = {0, 2.0f * M_PI};
    td->t_range = {0, 2.0f * M_PI};
    td->resolution = {50, 50};
    td->graph_func = torus;
    graph->play(new ShowFillCreation(td, 0, 5));

    graph->play(td);

    ThreeDObject *testObject = new ThreeDObject();
    testObject->r_range = {0, 1};
    testObject->t_range = {0, 2};
    testObject->resolution = {2, 5};
    testObject->setPoints({glm::vec3(20, -5, 29), glm::vec3(10, -5, 29), glm::vec3(0, -5, 29), glm::vec3(-10, -5, 29), glm::vec3(-20, -5, 29),
                            glm::vec3(20, 5, 29), glm::vec3(10, 5, 29), glm::vec3(0, 5, 29), glm::vec3(-10, 5, 29), glm::vec3(-20, 5, 29)});
    graph->play(testObject);
    graph->play(new ShowCreation(testObject, 0, 5));

    graph->run();
}