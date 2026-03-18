#include <GraphEngine/Scene/Scene.hpp>
#include <GraphEngine/Core/GraphApp.hpp>
#include <GraphEngine/Scene/TouchpadControlStrategy.hpp>

// --- Scene Implementation ---

Scene::Scene(int width, int height) {
    if(scene_instantiated){
        throw std::runtime_error("Only one Scene instance allowed!");
    }
    app = new GraphApp(width, height);
    this->window = app->getWindow();
    camera = new Camera(glm::vec3(0, 0, 100), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
    camera->setAspect((float)width / (float)height);
    scene_instantiated = true;
}

Scene::~Scene() {
    delete app;
    delete camera;
}

void Scene::InitGraphs(){
    for (GraphMathObject *graph : flat_scene_registry)
    {
        if (!graph->is_initialized)
        {
            graph->Init(GraphApp::drawCount);
            GraphApp::drawCount++;
        }
    }
}

void Scene::draw(float tick)
{
    try
    {
        auto cam_it = camera_animations.begin();
        while (cam_it != camera_animations.end())
        {
            CameraAnimation *anim = *cam_it;
            anim->play(tick);
            if (anim->isDone(tick))
            {
                cam_it = camera_animations.erase(cam_it);
                delete anim;
            }
            else { ++cam_it; }
        }

        camera->updateMatrices();
        AnimationManager::Run(tick);

        for (GraphMathObject *graph : flat_scene_registry)
        {
            if (graph->showGraph && graph->showFill)
                graph->updateFill(tick);
        }

        for (GraphMathObject *graph : flat_scene_registry)
        {
            if (graph->showGraph && graph->showStroke)
                graph->updateStroke(tick);
        }
    }
    catch (const std::exception &e) { std::cerr << e.what() << std::endl; }
    catch (...) { std::cerr << "Caught an UNKNOWN exception." << std::endl; }
}

void Scene::run() { app->run(this); }

void Scene::register_object(GraphMathObject *obj)
{
    auto it = std::find(flat_scene_registry.begin(), flat_scene_registry.end(), obj);
    if (it == flat_scene_registry.end())
    {
        flat_scene_registry.push_back(obj);
        if (!obj->is_initialized) obj->Init(GraphApp::drawCount++);
    }
}

void Scene::play(Animation *animation, float duration)
{
    float t_duration = std::max(0.0f, duration);
    animation->start_time = time_stamp;
    animation->duration = t_duration;
    animation->end_time = animation->start_time + animation->duration;
    animations.push_back(animation);
    this->time_stamp += t_duration;
}

void Scene::play(std::vector<Animation *> anims, float duration)
{
    float t_duration = std::max(0.0f, duration);
    for (Animation *anim : anims)
    {
        anim->start_time = time_stamp;
        anim->duration = t_duration;
        anim->end_time = anim->start_time + anim->duration;
        animations.push_back(anim);
    }
    this->time_stamp += t_duration;
}

void Scene::add(GraphMathObject *graph) { graph->on_added_to_scene(this); }

// --- Policy Implementations ---

void ICameraAnimatable::play(CameraAnimation* camera_animation, float start_time, float duration) {
    Scene* scene = dynamic_cast<Scene*>(this);
    if (!scene) return;
    
    float t_duration = std::max(0.0f, duration);
    float t_start = std::max(0.0f, start_time);
    camera_animation->start_time = t_start;
    camera_animation->end_time = t_start + t_duration;
    camera_animation->duration = t_duration;
    scene->camera_animations.push_back(camera_animation);
}

ManualControl::ManualControl() {}
ManualControl::~ManualControl() {}

void ManualControl::onEvent(KeyEvent& event) {
    Scene* scene = dynamic_cast<Scene*>(this);
    if (!scene) return;
    if (!manualStrategy) manualStrategy = std::make_unique<TouchpadControlStrategy>(scene);
    manualStrategy->onEvent(event);
}

void ManualControl::onEvent(MouseEvent& event) {
    Scene* scene = dynamic_cast<Scene*>(this);
    if (!scene) return;
    if (!manualStrategy) manualStrategy = std::make_unique<TouchpadControlStrategy>(scene);
    manualStrategy->onEvent(event);
}
