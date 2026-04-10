#include "Debra.hpp"
#include <AudioEngine/Sound.hpp>

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

glm::vec3 PetalSpiral(float t, Var v)
{
    float speed = 5.0;
    float z = t;
    float angle = t + v[0] * speed;
    float radius = 10.0f * sin(2.0f * t + v[0] * speed);
    float y = radius * sin(angle);
    float x = radius * cos(angle);
    return glm::vec3(x, y, z);
};

glm::vec3 sinGraph(float t, Var v){
    float x = t;
    float speed = 5.0f;
    float phase = speed * v[0];
    float y = 5.0f * sin(x * 0.5f + phase);
    return glm::vec3(x, y, 0);
}

void sinGraphUpdater(GraphObject* obj, float dt){
    Var v(dt);
    FunctionGraph *func = new FunctionGraph(sinGraph, {-20.0f * M_PI, 20.0f * M_PI, 0.2f}, v);
    obj->become(func);
}

class MainScene: public TwoDScene<ManualControl>{

public:
        using Scene::play;
        // using CameraControl::play;

        MainScene(): TwoDScene<ManualControl>(1200, 600){

            std::vector<std::vector<glm::vec3>> p = {
                {glm::vec3(-60, 20, 0),
                 glm::vec3(-30, 10, 0),
                 glm::vec3(0, 20, 0),
                 glm::vec3(30, 10, 0),
                 glm::vec3(60, 20, 0)},
                {glm::vec3(-60, -20, 0),
                 glm::vec3(-30, -10, 0),
                 glm::vec3(0, -20, 0),
                 glm::vec3(30, -10, 0),
                 glm::vec3(60, -20, 0)}};

            Polygon *test = new Polygon(p);

            std::cout << test->getPointsSize() << std::endl;
            Var v;
            FunctionGraph *func = new FunctionGraph(sinGraph, {-20 * M_PI, 20 * M_PI, 0.2f}, v);
            // func->setUpdater(sinGraphUpdater, 3.0f, -1.0f);

            Rectangle *rect = new Rectangle();
            Circle *circ = new Circle(20, 0, 0);
            add(test);
            add(func);
            add(rect);
            add(circ);
            circ->setColor({GraphColor(1, 0, 0)});
            std::cout << circ->layer << std::endl;

            Text *text = new Text("Shuvo Dev", -30, -30, 10.0f);
            add(text);
            Text *text2 = new Text("This is Debra Graph Engine", -60, 30, 10.0f);
            add(text2);

            NumberLine *num = new NumberLine(-20.0f * M_PI, 20.0f * M_PI, M_PI * 2.0f, 2.0f);
            // num->add_label(1.5f);
            add(num);

            // Transition *trans = new Transition(test, func);
            // timeProgress(1);
            // play(trans, 2);

        }
};

glm::vec3 sinSurface(float rho, float theta, Var var)
{
    float time = var[0];

    float x = rho * cos(theta) * theta * rho;
    float z = rho * sin(theta) ;

    float y = 5.0f * sin(rho * 0.2f + time);

    return glm::vec3(x, y, z);
}

void sinSurfaceUpdater(ThreeDObject* obj, float dt){
    obj->graph_var = Var(dt * 5.0f);
    obj->generatePoints();
    obj->updatePoints();
}

class Demo3DScene : public TwoDScene<ManualControl> {
public:
    Demo3DScene() : TwoDScene<ManualControl>(1200, 600, 50.0f) {
        // 1. Circle with new constructor
        Circle *c1 = new Circle(8, 0, 0, 0);
        c1->setColor({GraphColor(1, 0, 0)});
        c1->setStrokeWidth(100.0f);
        c1->showFill = false;
        add(c1);

        Rectangle *rect = new Rectangle(20, 10, 0, 0);
        rect->setStrokeWidth(20.0f);
        add(rect);
        FunctionGraph *sin_graph = new FunctionGraph(sinGraph, {-50.0f, 50.0f, 1.0f});
        // add(sin_graph);

        std::cout << "point size: " << c1->getPointsSize() << std::endl;

        // for(auto &p: sin_graph->stroke_points){
        //     Dot *d = new Dot(0.2f, p.x, p.y, p.z);
        //     d->showStroke = true;
        //     d->showFill = false;
        //     add(d);
        // }

            Polygon* poly = new Polygon({
                {glm::vec3(-20, 0, 0), glm::vec3(-10, 10, 0), glm::vec3(0, 0, 0)},
                {glm::vec3(10, 0, 0), glm::vec3(20, 10, 0), glm::vec3(30, 0, 0)}
            });

            add(poly);
    }
};
int main(){
    // MainScene* scene = new MainScene();
    Demo3DScene* scene = new Demo3DScene();
    scene->run();
}