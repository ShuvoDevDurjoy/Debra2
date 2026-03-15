#include "../include/GraphEngine/Graphs/Graph.hpp"

#include "../include/GraphEngine/Graphs/Coordinates/CylindricalCoordinate.hpp"
#include "../include/GraphEngine/Graphs/Coordinates/CartesianCoordinate.hpp"
#include "../include/GraphEngine/Graphs/GraphObjects/ThreeDAxes.hpp"
#include "../include/GraphEngine/Objects/Rectangle.hpp"
#include "../include/GraphEngine/Objects/Polygon.hpp"
#include "../include/GraphEngine/Graphs/GraphObjects/TestObject.hpp"
#include "../include/GraphEngine/Graphs/GraphObjects/GraphMathObject.hpp"
#include "../include/GraphEngine/Graphs/GraphObjects/ThreeDObject.hpp"
#include "../include/GraphEngine/Graphs/GraphObjects/Graphing/GridBox.hpp"
#include "../include/GraphEngine/Graphs/GraphObjects/Graphing/Dot.hpp"
#include "../include/GraphEngine/Graphs/GraphObjects/Shapes/Circle.hpp"
#include "../include/GraphEngine/Graphs/GraphObjects/Shapes/Text.hpp"
#include "../include/GraphEngine/Graphs/GraphObjects/Shapes/Rectangle.hpp"
#include "../include/GraphEngine/Graphs/GraphObjects/Shapes/Arrow.hpp"
#include "../include/GraphEngine/Graphs/GraphObjects/Shapes/Star.hpp"
#include "../include/GraphEngine/Graphs/GraphObjects/Graphing/NumberLine.hpp"


#include "../include/GraphEngine/Graphs/GraphObjects/Shapes/BasePolygon.hpp"
#include "../include/GraphEngine/Animations/Creation.hpp"
#include "../include/GraphEngine/Animations/Translate.hpp"
#include "../include/GraphEngine/Animations/Transition.hpp"

#include "../include/glm/glm.hpp"
#include "../include/glm/gtc/matrix_transform.hpp"

glm::vec3 generate(float r, float t, Var v)
{
    float x = r * 20.0f;
    float y = x * x;
    float z = x * cos(r);
    return glm::vec3(x, y, z);
}

glm::vec3 torus(float u, float v, Var vars)
{
    float R = vars[0]; // major radius
    float r = vars[1]; // minor radius

    float x = (R + r * cos(u)) * cos(v);
    float y = r * sin(u);
    float z = (R + r * cos(u)) * sin(v);

    return glm::vec3(x, y, z);
}

glm::vec3 paraboloid(float r, float theta, Var vars)
{
    float x = 10.0f * cos(theta);
    float y = 10.0f * sin(theta);
    float z = r; // height proportional to r^2
    return glm::vec3(x, y, z);
}

glm::vec3 sphere(float r, float theta, Var vars)
{
    float R = 20;

    // r is polar angle φ
    float phi = r;

    float x = R * sin(phi) * cos(theta);
    float z = R * sin(phi) * sin(theta);
    float y = R * cos(phi);

    return glm::vec3(x, y, z);
}

glm::vec3 brachistochrone(float t, Var v)
{
    float a = v[0];
    float x0 = v[1];
    float y0 = v[2];

    float x = x0 + a * (t - sin(t));
    float y = y0 - a * (1 - cos(t));

    return glm::vec3(x, y, 0);
}

glm::vec3 butterflyCurve(float t, Var v)
{
    float a = std::exp(std::cos(t)) - 2.0f * std::cos(4.0f * t) - std::pow(std::sin(t / 12.0f), 5.0f);

    float x = v[0] * std::sin(t) * a;
    float y = v[1] * std::cos(t) * a;
    // float z = v[0] * std::cos(t) * std::sin(t) * a;
    float z = 0;

    return glm::vec3(x, y, z);
}

glm::vec3 circleEquation(float t, Var v){
    float x = 23.0f * cos(t);
    float y = 23.0f * sin(t);
    return glm::vec3(x, y, 0);
}

glm::vec3 PetalSpiral(float t, float dt)
{
    float speed = 1.0;
    float z = t * 50.0f;
    float angle = t * 50.0f + dt * speed;
    float radius = 10.0f * sin(2.0f * t + dt * speed);
    float y = radius * sin(angle);
    float x = radius * cos(angle);
    return glm::vec3(x, y, z);
};

auto SpiralRise = [](float t, float dt) -> glm::vec3
{
    float x = t * 50.0f;
    float radius = 3.0f + 2.0f * sin(dt * 0.5f);
    float angle = x * 0.3f + dt;
    float y = radius * sin(angle) + 0.5f * sin(dt);
    float z = radius * cos(angle) + 0.5f * cos(dt);
    return glm::vec3(x, y, z);
};

// 9️⃣ Waving sine tube
auto WavingTube = [](float t, float dt) -> glm::vec3
{
    float x = t * 50.0f;
    float baseRadius = 4.0f;
    float y = (baseRadius + sin(dt * 2.0f)) * sin(x - dt * 1.5f);
    float z = (baseRadius + cos(dt * 2.5f)) * cos(x - dt * 1.5f);
    return glm::vec3(x, y, z);
};

// 🔟 Twisting ribbon with oscillation
auto TwistingRibbon = [](float t, float dt) -> glm::vec3
{
    float x = t * 50.0f;
    float y = 5.0f * sin(x - dt * 1.8f) + 2.0f * sin(dt);
    float z = 10.0f * cos(x - dt * 2.0f) + 1.0f * sin(dt * 1.5f);
    return glm::vec3(x, y, z);
};

// Growing sine wave from origin
auto GrowingSine = [](float t, float dt) -> glm::vec3
{
    // Controls
    const float growSpeed = 2.0f; // units per second along X
    const float amplitude = 5.0f;
    const float frequency = 1.0f; // cycles per unit

    // How much of the curve exists at this time
    float maxX = growSpeed * dt;

    // If this part of curve is not yet revealed, clamp to origin
    if (t > maxX)
        return glm::vec3(maxX, 0.0f, 0.0f);

    float x = t;
    float y = amplitude * sin(2.0f * M_PI * frequency * t);

    return glm::vec3(x, y, 0.0f);
};

// 12️⃣ Rolling sine wave
auto RollingWave = [](float t, float dt) -> glm::vec3
{
    float x = t * 50.0f;
    float y = 5.0f * sin(x - dt * 1.5f) + 0.5f * cos(x * 0.3f + dt);
    float z = 5.0f * cos(x - dt * 1.5f) + 0.5f * sin(x * 0.2f + dt);
    return glm::vec3(x, y, z);
};

// 13️⃣ Corkscrew motion
auto Corkscrew = [](float t, float dt) -> glm::vec3
{
    float z = t * 30;
    float angle = z * 0.5f + dt * 2.0f;
    float x = z * 0.5 * cos(angle);
    // float y = radius * cos(angle);
    float y = z * 0.5 * sin(angle);
    // float z = radius * sin(angle);
    return glm::vec3(x, y, z);
};

// 14️⃣ Ocean wave ripple
auto OceanRipple = [](float t, float dt) -> glm::vec3
{
    float x = t * 50.0f;
    float y = 3.0f * sin(x * 0.5f - dt) + 2.0f * sin(dt * 0.8f);
    float z = 3.0f * cos(x * 0.4f - dt) + 2.0f * cos(dt * 0.6f);
    return glm::vec3(x, y, z);
};

// 15️⃣ Infinity loop wave
auto InfinityWave = [](float t, float dt) -> glm::vec3
{
    float x = t * 50.0f;
    float y = 5.0f * sin(x - dt * 1.2f) + 5.0f * sin(x * 0.5f - dt * 0.8f);
    float z = 5.0f * cos(x - dt * 1.2f) - 5.0f * cos(x * 0.5f - dt * 0.8f);
    return glm::vec3(x, y, z);
};

glm::vec3 chaoticFractal(float t, Var v)
{
    float x = sin(t) * cos(3 * t) + cos(t * t);
    float y = cos(t) * sin(5 * t) - sin(t * t);
    return {x * v[0], y * v[0], 0};
}

float amplitude = 6.0f;
float sin_offset_left = -10.0f;
float sin_offset_right = 10.0f;
float circle_x_center = -40.0f;
float duration = 30.0f;

glm::vec3 sinGraph(float t, Var v){
    float x = t;
    float y = amplitude * sin(x);
    return glm::vec3(x, y, 0);
}

glm::vec3 lineUpdater(float t, float dt){
    float dis = (sin_offset_right - sin_offset_left) * M_PI;
    if (t == 1)
    {
        float angle = 2.0f * M_PI * dt * (dis / (2.0f * M_PI));
        float x = circle_x_center + amplitude * cos(angle);
        float y = amplitude * sin(angle);

        return glm::vec3(x, y, 0);
    }
    else{
        return glm::vec3(circle_x_center, 0, 0);
    }
}

glm::vec3 trackingLineUpdater(float t, float dt){
    float dis = (sin_offset_right - sin_offset_left) * M_PI;
    if(t==1){
        float x = sin_offset_left * M_PI + dis * dt;
        float y = amplitude * sin(x);
        return glm::vec3(x, y, 0);
    }
    else{
        float angle = 2.0f * M_PI * dt * ( dis / (2.0f * M_PI));
        float x = circle_x_center + amplitude * cos(angle);
        float y = amplitude * sin(angle);
        return glm::vec3(x, y, 0);
    }
}

void animation1(Graph* graph){

    //Initialize the lines
    Line *x_axis = new Line(glm::vec3(-50, 0, 0), glm::vec3(50, 0, 0));
    Line *y_axis = new Line(glm::vec3(0, 30, 0), glm::vec3(0, -30, 0));
    Line *z_axis = new Line(glm::vec3(0, 0, -50), glm::vec3(0, 0, 50));

    //set stroke for the lines
    x_axis->setStrokeWidth(0.5f);
    y_axis->setStrokeWidth(0.5f);
    z_axis->setStrokeWidth(0.5f);

    //draw the circle
    Circle *circle = new Circle(amplitude, circle_x_center, 0);
    circle->setStrokeWidth(0.5f);

    Line *radius = new Line(glm::vec3(-45, 0, 0), glm::vec3(-40, 0, 0));
    radius->setStrokeWidth(0.5f);

    float start_time = 0.0f;

    radius->setUpdater(lineUpdater, start_time, duration);

    Line *tracking_line = new Line(glm::vec3(-40, 0, 0), glm::vec3(-12.0f * M_PI, 0, 0));
    tracking_line->setStrokeWidth(0.5f);
    tracking_line->setUpdater(trackingLineUpdater, start_time, duration);

    Var v;

    TestObject *sin_graph = new TestObject();
    sin_graph->range = {sin_offset_left * M_PI, sin_offset_right * M_PI};
    sin_graph->generatePoints(sinGraph, v);
    sin_graph->setStrokeWidth(0.5f);

    Animation *sin_graph_anim = new ShowCreation(sin_graph, start_time, duration);
    sin_graph_anim->anim_timing_func = AnimationTimmingFunction::linearProgress;

    
    //add the lines to the scene
    graph->play(x_axis);
    graph->play(y_axis);
    graph->play(z_axis);

    graph->play(circle);
    graph->play(sin_graph);
    graph->play(radius);
    graph->play(tracking_line);
}

void animation2(Graph* graph){
    // Initialize the lines
    Line *x_axis = new Line(glm::vec3(-50, 0, 0), glm::vec3(50, 0, 0));
    Line *y_axis = new Line(glm::vec3(0, 30, 0), glm::vec3(0, -30, 0));
    Line *z_axis = new Line(glm::vec3(0, 0, -50), glm::vec3(0, 0, 50));

    // set stroke for the lines
    x_axis->setStrokeWidth(0.5f);
    y_axis->setStrokeWidth(0.5f);
    z_axis->setStrokeWidth(0.5f);

    Var v;

    TestObject *sin_graph = new TestObject();
    sin_graph->generatePoints(sinGraph, v);
    sin_graph->setStrokeWidth(0.5f);
    sin_graph->setColor({GraphColor(), GraphColor(), GraphColor(), GraphColor(), GraphColor()});
    sin_graph->setUpdater(PetalSpiral, 0, -1);

    graph->play(sin_graph);

    // add the lines to the scene
    graph->play(x_axis);
    graph->play(y_axis);
    graph->play(z_axis);
}

void animation3(Graph* graph){
    // Initialize the lines
    Line *x_axis = new Line(glm::vec3(-50, 0, 0), glm::vec3(50, 0, 0));
    Line *y_axis = new Line(glm::vec3(0, 30, 0), glm::vec3(0, -30, 0));
    Line *z_axis = new Line(glm::vec3(0, 0, -50), glm::vec3(0, 0, 50));

    // set stroke for the lines
    x_axis->setStrokeWidth(0.5f);
    y_axis->setStrokeWidth(0.5f);
    z_axis->setStrokeWidth(0.5f);

    Var v;
    v.addVar(8.0f);
    v.addVar(8.0f);
    v.addVar(8.0f);

    TestObject *sin_graph = new TestObject();
    sin_graph->range = {sin_offset_left * M_PI, sin_offset_right * M_PI};
    sin_graph->generatePoints(sinGraph, v);
    sin_graph->setStrokeWidth(0.5f);
    
    TestObject *sin_graph2 = new TestObject();
    sin_graph2->range = {-2.0f * M_PI, 2.0f * M_PI};
    sin_graph2->generatePoints(chaoticFractal, v);
    sin_graph2->setStrokeWidth(0.5f);
    
    TestObject *buttfly_curve = new TestObject();
    buttfly_curve->range = {sin_offset_left * M_PI, sin_offset_right * M_PI};
    buttfly_curve->resolution = 3000;
    buttfly_curve->generatePoints(butterflyCurve, v);
    buttfly_curve->setStrokeWidth(0.5f);
    Circle* circ = new Circle(15, 0, 0);
    Circle* circ2 = new Circle(10, 0, 0);

    graph->play(circ);
    // new ShowCreation(sin_graph, 0, 5.0f);
    graph->play(new Transition(circ, circ2, 0.0f, 5.0f));
    // graph->play(new Transition(sin_graph, sin_graph2, 10.0f, 5.0f));


    // add the lines to the scene
    graph->play(x_axis);
    graph->play(y_axis);
    graph->play(z_axis);
}

glm::vec3 random(float t, Var v){
    float x = 50.0f * t;
    float y = sin(x) + cos(x);
    float z = cos(y);
    return {x, y, z};
}

// int main(){
    // Graph *graph = Graph::getInstance();

    // animation1(graph);
    // animation2(graph);
    // animation3(graph);

    // TestObject *test = new TestObject();
    // Var v;
    // test->resolution = 2000;
    // test->setStrokeWidth(0.5f);
    // test->generatePoints(random, v);
    // graph->play(test);

    // ThreeDObject *td = new ThreeDObject();
    // td->r_range = {0, 2 * M_PI};
    // td->t_range = {0, 2 * M_PI};
    // td->resolution = {50, 50};
    // td->graph_func = torus;
    // Var var;
    // var.addVar(20.0f);
    // var.addVar(10.0f);
    // td->graph_var = var;
    // td->setFillColors({GraphColor(0.5, 0, 0.5), GraphColor(0.5, 0.5, 0), GraphColor(0.5, 0, 0.5)});
    // td->setShowFill(false);
    // td->setFillOpacity(0.0f);
    // td->setStrokeWidth(5.0f);
    // Animation* anim1 = new ShowFillCreation(td, 0, 10);
    // anim1->anim_timing_func = AnimationTimmingFunction::linearProgress;

    // graph->play(td);

    // graph->run();
// }

int main(){
    Graph *graph = Graph::getInstance(0);

   
    // Text *t1 = new Text("Title", -20, 20, 10.0f);
    // Text *t2 = new Text("Shuvo", 20, 20, 10.0f);
    // graph->play(t1);
    // graph->play(t2);

    // new Transition(t1, t2, 2, 5);

    TestObject *test1 = new TestObject();
    test1->start_bezier_path(glm::vec3(0, 0, 0));
    test1->add_line_to(glm::vec3(10, 10, 0));
    test1->add_line_to(glm::vec3(20, 0, 0));

    test1->start_bezier_path(glm::vec3(0, 20, 0));
    test1->add_line_to(glm::vec3(10, 10, 0));
    test1->add_line_to(glm::vec3(20, 20, 0));

    TestObject *test2 = new TestObject();
    test2->start_bezier_path(glm::vec3(0, 5, 0));
    test2->add_line_to(glm::vec3(-10, 0, 0));
    test2->add_line_to(glm::vec3(-20, 5, 0));
    
    test2->start_bezier_path(glm::vec3(0, 10, 0));
    test2->add_line_to(glm::vec3(-10, 20, 0));
    test2->add_line_to(glm::vec3(-20, 10, 0));
    test2->add_line_to(glm::vec3(-30, 20, 0));

    test1->makeSmooth();
    // test2->makeSmooth();

    Text *c1 = new Text("tit", -20, 10, 10.0f);
    Text *c2 = new Text("shu", 20, 10, 10.0f);

    graph->play(c1);
    graph->play(c2);

    new Transition(c1, c2, 1, 10);

    graph->play(test1);
    graph->play(test2);

    // new Transition(test1, test2, 1, 3);

    graph->run();
}

int main2()
{
    Graph *graph = Graph::getInstance(0);

    // ThreeDAxes *td = new ThreeDAxes();
    // graph->play(td);

    Dot *dot0 = new Dot(0, 0, 15, 32);
    Dot *dot1 = new Dot(0, 0, 5, 32);
    Dot *dot2 = new Dot(0, 0, 30, 5);
    Dot *dot3 = new Dot(0, 0, 15, 5);
    Dot *dot4 = new Dot(0, 0, 20, 6);
    Dot *dot5 = new Dot(0, 0, 25, 13);
    dot0->setStrokeWidth(0.5f);
    dot1->setStrokeWidth(0.5f);
    dot2->setStrokeWidth(0.5f);
    dot3->setStrokeWidth(3.0f);
    dot4->setStrokeWidth(4.0f);
    dot5->setStrokeWidth(5.0f);
    graph->play(dot0);
    graph->play(dot1);
    graph->play(dot2);

    std::cout << dot0->x << ", " << dot0->y << ", " << dot0->width << ", " << dot0->height << std::endl;
    // graph->play(new ShowCreation(dot0, 0, 5));
    // graph->play(new ShowCreation(dot1, 0, 5));
    // graph->play(new ShowCreation(dot2, 0, 5));
    // graph->play(new ShowCreation(dot3, 9, 5));
    // graph->play(new ShowCreation(dot4, 12, 5));
    // graph->play(new ShowCreation(dot5, 15, 5));
    // dot1->nextTo(dot2, Position::LEFT);
    // dot1->setMoveTo(glm::vec3(0, 0, 0));
    // graph->play(dot3);
    // graph->play(dot4);
    // graph->play(dot5);
    dot0->fillOpacity = 0;
    dot1->fillOpacity = 0;
    // dot0->moveTo(Position::RIGHT);
    // dot2->fillOpacity = 0;
    dot3->fillOpacity = 0;
    dot4->fillOpacity = 0;
    dot5->fillOpacity = 0;

    int start_time = 0;
    std::vector<Position> positions = {LEFT, TOP, RIGHT, BOTTOM, TOP_LEFT, BOTTOM_LEFT, TOP_RIGHT, BOTTOM_RIGHT};

    for (int i = 0; i < positions.size(); ++i){
        (new Translate(dot1, dot0->getPosition(positions[i]), start_time + i * 3, 3, Position::TOP_LEFT))->anim_timing_func = AnimationTimmingFunction::linearProgress;
        std::cout << "Start time is: " << start_time + i * 3 << std::endl;
    }

    glm::vec3 pos = dot0->getPosition(Position::NONE);
    glm::vec3 pos1 = dot0->getPosition(Position::BOTTOM_RIGHT);

    std::cout << "x is: " << pos[0] << " y is: " << pos[1] << " z is: " << pos[2] << std::endl;
    std::cout << "x is: " << pos1[0] << " y is: " << pos1[1] << " z is: " << pos1[2] << std::endl;

    dot1->nextTo(dot0, Position::RIGHT, 5.0f);

    Var v;
    v.addVar(8.0f);
    v.addVar(8.0f);
    v.addVar(8.0f);

    TestObject *test = new TestObject();
    // test->range = {0.0f, 6.0f * M_PI};
    // test->resolution = 100;
    // test->generatePoints(butterflyCurve, v);
    // test->setPoints(glm::vec3(-30, -30, 0));
    // test->setPoints(glm::vec3(-30, 30, 0));
    // test->setPoints(glm::vec3(-40, 10, 0));
    // test->setPoints(glm::vec3(-45, 30, 0));
    test->setStrokeWidth(3.0f);
    // test->setPoints(glm::vec3(-30, -30, 0));
    // test->setPoints(glm::vec3(30, 14, 0));
    // test->setPoints(glm::vec3(50, 8, 0));
    // test->setPoints(glm::vec3(55, 20, 0));
    TestObject *test1 = new TestObject();
    test1->range = {0.0f, 6.0f * M_PI};
    test1->resolution = 1000;
    test->resolution = 2000;
    test->generatePoints([](float t, Var v)
                         { float x = 60.0f * t; float y = 5.0f * sin(x * 0.3f);
                        float z = 0;
                    return glm::vec3(x, y, z); }, v);
    // test->use_bezier_always = 0.0;

    // Use BEVEL join style for smooth curves to avoid artifacts
    // test1->setStrokeJoinStyle(GraphMathObject::StrokeJoinStyle::BEVEL);
    // test1->setMiterLimit(1.5f);  // Tight limit for smooth rendering
    // test->resolution = 500;
    test1->generatePoints(chaoticFractal, v);
    // test1->use_bezier_always = 0.0;
    // test1->setUpdater(WavingTube, 0, -1);
    // test1->setUpdater(TwistingRibbon, 0, -1);
    // test1->setUpdater(RollingWave, 0, -1);
    // test1->setUpdater(Corkscrew, 0, -1);
    // test1->setUpdater(OceanRipple, 0, -1);
    // test->setUpdater(PetalSpiral, 0, -1);

    test1->fillOpacity = 0;
    test->setStrokeWidth(1.0f);
    graph->play(test);
    test->fillOpacity = 0;
    test1->setStrokeWidth(0.5f);
    // graph->play(test1);
    Animation *anim1 = new ShowCreation(test, 0, 10);
    // Animation *transitionAnim1 = new Transition(test1, test, 2, 5);
    // Animation *anim2 = new ShowCreation(test1, 0, 5);
    anim1->anim_timing_func = AnimationTimmingFunction::linearProgress;
    // anim2->anim_timing_func = AnimationTimmingFunction::easeInOutExpo;
    // transitionAnim1->anim_timing_func = AnimationTimmingFunction::easeInOutExpo;
    float L = 30.0f;
    Line *x_pos_to_neg = new Line(glm::vec3(L, 0, 0), glm::vec3(-L, 0, 0));
    Line *x_neg_to_pos = new Line(glm::vec3(-L, 5, 0), glm::vec3(L, 5, 0));

    Line *y_pos_to_neg = new Line(glm::vec3(0, L, 0), glm::vec3(0, -L, 0));
    Line *y_neg_to_pos = new Line(glm::vec3(5, -L, 0), glm::vec3(5, L, 0));

    Line *z_offset1 = new Line(glm::vec3(0, 10, L), glm::vec3(0, 10, -L));
    Line *z_offset2 = new Line(glm::vec3(0, 5, -L), glm::vec3(0, 5, L));

    GraphColor color1 = GraphColor(0, 1, 1);
    GraphColor color2 = GraphColor(1, 1, 0);

    // Line *ro = new Line(glm::vec3(0, 0, 0), glm::vec3(25, 0, 0));
    // graph->play(ro);
    // Animation *anim1 = new Rotation(ro, glm::vec3(0, 0, 360), glm::vec3(0, 0, 0), 0, 5);
    // anim1->anim_timing_func = AnimationTimmingFunction::linearProgress;
    // graph->play(new Rotation(ro, glm::vec3(0, 0, 360), glm::vec3(10, 0, 0), 6, 5));
    // graph->play(new Rotation(ro, glm::vec3(0, 0, 360), glm::vec3(20, 0, 0), 12, 5));

    x_pos_to_neg->setColor(color1);
    y_pos_to_neg->setColor(color2);

    int offset = 15;
    for (int i = 0; i < 2; ++i)
    {
        Line *line = new Line(glm::vec3(-offset + i * offset * 2, offset * 3, 0), glm::vec3(-offset + i * offset * 2, -offset * 3, 0));
        line->setColor({color1, color2});
        graph->play(line);
        line->showGraph = false;
        line->setStrokeWidth(0.5f);
        graph->play(new ShowCreation(line, 0, 5));

        Line *liney = new Line(glm::vec3(-offset * 3, -offset + i * offset * 2, 0), glm::vec3(offset * 3, -offset + i * offset * 2, 0));
        liney->setColor({color1, color2});
        graph->play(liney);
        liney->showGraph = false;
        liney->setStrokeWidth(0.5f);
        graph->play(new ShowCreation(liney, 0, 5));
    }



    graph->run();
}