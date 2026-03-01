#include "GraphEngine/Graphs/Graph.hpp"
#include "GraphEngine/Graphs/GraphObjects/ThreeDAxes.hpp"
#include "GraphEngine/Graphs/GraphObjects/Shapes/Circle.hpp"
#include "GraphEngine/Animations/Transition.hpp"

int main()
{
    Graph *graph = Graph::getInstance(20);

    // Create a Circle
    Circle *c1 = new Circle(15, -20, 0);
    c1->setColor(GraphColor(0, 1, 1)); // Cyan

    // Create a Rectangle
    Rectangle *r1 = new Rectangle(15, 20, 15);
    r1->setColor(GraphColor(1, 0, 1)); // Magenta

    // Add axes for reference
    ThreeDAxes *td = new ThreeDAxes();
    graph->play(td);

    // Initial shape
    graph->play(c1);

    // Morph the Circle into the Rectangle
    graph->play(new Transition(c1, r1, 1.0f, 4.0f));

    graph->run();
    return 0;
}
