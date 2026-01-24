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

glm::vec3 generate(float r, float t, Var v)
{
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
    float z = r; // height proportional to r^2
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
    float x = t * 50.0f;
    float angle = t * 50.0f + dt * speed;
    float radius = 10.0f * sin(2.0f * t + dt * speed);
    float y = radius * sin(angle);
    float z = radius * cos(angle);
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
    float radius = 4.0f + 2.0f * sin(dt * 0.7f);
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

int main()
{
    Graph *graph = Graph::getInstance(0);

    // ThreeDAxes *td = new ThreeDAxes();
    // graph->play(td);

    Dot *dot0 = new Dot(0, 0, 25, 32);
    Dot *dot1 = new Dot(0, 0, 30, 3);
    Dot *dot2 = new Dot(0, 0, 30, 4);
    Dot *dot3 = new Dot(0, 0, 15, 5);
    Dot *dot4 = new Dot(0, 0, 20, 6);
    Dot *dot5 = new Dot(0, 0, 25, 13);
    dot0->setStrokeWidth(3.0f);
    dot1->setStrokeWidth(3.0f);
    dot2->setStrokeWidth(2.0f);
    dot3->setStrokeWidth(3.0f);
    dot4->setStrokeWidth(4.0f);
    dot5->setStrokeWidth(5.0f);
    graph->play(dot0);
    // graph->play(new ShowCreation(dot0, 18, 5));
    // graph->play(new ShowCreation(dot1, 3, 5));
    // graph->play(new ShowCreation(dot2, 6, 5));
    // graph->play(new ShowCreation(dot3, 9, 5));
    // graph->play(new ShowCreation(dot4, 12, 5));
    // graph->play(new ShowCreation(dot5, 15, 5));
    // graph->play(dot1);
    // graph->play(dot2);
    // graph->play(dot3);
    // graph->play(dot4);
    // graph->play(dot5);
    dot0->fillOpacity = 0;
    dot1->fillOpacity = 0;
    dot2->fillOpacity = 0;
    dot3->fillOpacity = 0;
    dot4->fillOpacity = 0;
    dot5->fillOpacity = 0;
    Var v;
    v.addVar(10.0f);
    v.addVar(10.0f);

    TestObject *test = new TestObject();
    test->range = {0.0f, 2.0f * M_PI};
    // test->generatePoints(circleEquation, v);
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
    test1->resolution = 1000;
    test1->generatePoints([](float t, Var v)
                         { float x = 50.0f * t; float y = 5.0f + 5.0f * sin(x * 0.5f);
                        float z = 0;
                    return glm::vec3(x, y, z); }, v);
    // test->resolution = 500;
    // test1->generatePoints(butterflyCurve, v);
    // test1->setUpdater(WavingTube, 0, -1);
    // test1->setUpdater(TwistingRibbon, 0, -1);
    // test1->setUpdater(RollingWave, 0, -1);
    test1->setUpdater(Corkscrew, 0, -1);
    test1->setUpdater(OceanRipple, 0, -1);
    // test1->setUpdater(PetalSpiral, 0, -1);

    test1->fillOpacity = 0;
    graph->play(test1);
    test->fillOpacity = 0;
    test1->setStrokeWidth(3.0f);
    test->setStrokeWidth(3.0f);
    graph->play(test);
    // Animation *anim1 = new ShowCreation(test1, 0, 2);
    Animation *anim2 = new ShowCreation(test1, 0, 5);
    // anim1->anim_timing_func = AnimationTimmingFunction::linearProgress;
    anim2->anim_timing_func = AnimationTimmingFunction::easeInOutExpo;

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

    int offset = 10;
    // for (int i = 0; i < 2; ++i)
    // {
    //     Line *line = new Line(glm::vec3(-offset + i * offset * 2, offset * 3, 0), glm::vec3(-offset + i * offset * 2, -offset * 3, 0));
    //     line->setColor({color1, color2});
    //     graph->play(line);
    //     line->showGraph = false;
    //     graph->play(new ShowCreation(line, i * 2, 5));

    //     Line *liney = new Line(glm::vec3(-offset * 3, -offset + i * offset * 2, 0), glm::vec3(offset * 3, -offset + i * offset * 2, 0));
    //     liney->setColor({color1, color2});
    //     graph->play(liney);
    //     liney->showGraph = false;
    //     graph->play(new ShowCreation(liney, i * 2, 5));
    // }


    graph->run();
}