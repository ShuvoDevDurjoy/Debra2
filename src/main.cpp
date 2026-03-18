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


class MainScene: public TwoDScene<CameraControl>{

    public: 
        using Scene::play;
        using CameraControl::play;

        MainScene(): TwoDScene<CameraControl>(1200, 600){

            Circle* circ = new Circle(30, 0, 0);
            add(circ);

            Line *line = new Line(glm::vec3(0, 0, 0), glm::vec3(30, 30, 0));
            add(line);

            Rectangle *rect = new Rectangle(30, 20, 0, 0);
            add(rect);

            Star *star = new Star(5, 5, 30, 0, 0);
            add(star);

            TestObject *test = new TestObject();
            Var v;
            v.addVar(8.0f);
            v.addVar(8.0f);
            test->range = {-6.5 * M_PI, 6.5 * M_PI};
            test->generatePoints(butterflyCurve, v);
            add(test);

            timeProgress(5.0f);
            Animation *trans = new Transition(line, test);
            play(trans, 2.0f);
            play(new FadeOut(line), 3);
            play(new Transition(test, circ), 3);

            CameraAnimation* anim = CameraAnimation::CreateOrbit(getCamera(), 360.0f, glm::vec3(0, 1, 0));
            CameraAnimation *anim1 = new CameraAnimation(getCamera(), glm::vec3(0, 0, 30), glm::vec3(0, 0, 0));
            // this->play(anim1, 5.0f, 5.0f);
            this->play(anim, 0.0f, 5.0f);
        }
};

int main(){
    MainScene* scene = new MainScene();
    scene->run();
}