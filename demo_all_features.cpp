#include "GraphEngine/Graphs/Graph.hpp"
#include "GraphEngine/Graphs/GraphObjects/ThreeDAxes.hpp"
#include "GraphEngine/Graphs/GraphObjects/Shapes/Circle.hpp"
#include "GraphEngine/Graphs/GraphObjects/Shapes/BasePolygon.hpp"
#include "GraphEngine/Animations/Creation.hpp"
#include "GraphEngine/Animations/Transition.hpp"
#include "GraphEngine/Animations/Translate.hpp"

int main()
{
    // Initialize the Debra Manim-like Engine
    Graph *graph = Graph::getInstance(20);

    // 1. Create Background Axes
    ThreeDAxes *axes = new ThreeDAxes();
    graph->play(axes); // Background objects are drawn universally

    // Overlay Text
    // graph->addText("Debra Engine", 50, 550, 0.8f, new GraphColor(1, 1, 1));
    // graph->addText("Manim C++ Version", 50, 510, 0.4f, new GraphColor(0.8, 0.8, 0.8));

    // 2. Create a smooth Bezier Circle
    Circle *c1 = new Circle(15, -20, 0); // 15 unit radius, centered at -20, 0
    c1->setColor(GraphColor(0.0f, 1.0f, 1.0f)); // Cyan
    c1->setStrokeWidth(1.0f); // Thick stroke to showcase view-space billboarding
    
    // CRITICAL: Must add the object to the scene to be drawn
    graph->play(c1);

    // target Rectangle for Morphing later
    Rectangle *r1 = new Rectangle(15, 20, 15);
    r1->setColor(GraphColor(1.0f, 0.0f, 1.0f)); // Magenta
    r1->setStrokeWidth(1.0f);

    // ==========================================
    // Sequence of Animations
    // ==========================================

    // Animation 1: Creation (Draw the Circle boundary over 2 seconds)
    graph->play(new ShowCreation(c1, 0.5f, 2.0f));

    // Animation 2: Translation (Move from -20 to 0 on X axis)
    graph->play(new Translate(c1, glm::vec3(0, 0, 0), 3.0f, 2.0f));

    // Animation 3: Scale (Grow to 150%)
    graph->play(new Scale(c1, glm::vec3(.5f, .5f, 1.0f), 5.5f, 2.0f));

    // Animation 4: Rotation (Spin 180 degrees)
    graph->play(new Rotation(c1, glm::vec3(0, 0, 180), glm::vec3(0, 0, 0), 8.0f, 2.0f));

    // Animation 5: Morphing (Transition from Cyan Circle to Magenta Rectangle)
    graph->play(new Transition(c1, r1, 10.5f, 3.0f));
    
    // Animation 6: UnCreate (Erase the stroke backward)
    graph->play(new UnCreate(c1, 14.0f, 2.0f));

    // Start the OpenGL pipeline and timeline
    graph->run();
    
    return 0;
}
