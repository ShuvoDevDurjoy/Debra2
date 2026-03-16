#include "Debra.hpp"

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


class MainScene: public Scene{

    public: 
        MainScene(): Scene(1200, 600){
            Circle* circ = new Circle(10, 0, 0);
            add(circ);
        }
};

int main(){
    MainScene* scene = new MainScene();
    scene->run();
}