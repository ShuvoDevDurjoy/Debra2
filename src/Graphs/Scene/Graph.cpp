// #include "../../../include/GraphEngine/Scene/Graph.hpp"
// #include <cmath>

// Graph *Graph::instance = nullptr;



// Graph::Graph(int width, int height)
//     : controlMode(mode)
// {
//     app = new GraphApp();
//     this->window = app->getWindow();
//     camera = new Camera(glm::vec3(0, 0, 100), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
    
//     int width, height;
//     glfwGetFramebufferSize(window, &width, &height);
//     camera->setAspect((float)width / (float)height);
// }

// void Graph::InitGraphs()
// {
//     is_running = true;
//     // Registry is already populated via play() -> on_added_to_scene() -> register_object()
//     for (GraphMathObject *graph : flat_scene_registry)
//     {
//         if (!graph->is_initialized) {
//             graph->Init(GraphApp::drawCount);
//             GraphApp::drawCount++;
//         }
//     }
// }


// void Graph::draw(float tick)
// {
//     try
//     {
//         // Execute camera animations first
//         auto cam_it = camera_animations.begin();
//         while (cam_it != camera_animations.end()) {
//             CameraAnimation* anim = *cam_it;
//             anim->play(tick);
//             if (anim->isDone(tick)) {
//                 cam_it = camera_animations.erase(cam_it);
//                 delete anim;
//             } else {
//                 ++cam_it;
//             }
//         }

//         camera->updateMatrices();
//         AnimationManager::Run(tick);

//         // --- Pass 1: Fill Pass ---
//         for (GraphMathObject *graph : flat_scene_registry)
//         {
//             if (graph->showGraph && graph->showFill)
//                 graph->updateFill(tick);
//         }

//         // --- Pass 2: Stroke Pass ---
//         for (GraphMathObject *graph : flat_scene_registry)
//         {
//             if (graph->showGraph && graph->showStroke)
//                 graph->updateStroke(tick);
//         }
        
//     }
//     catch (const std::exception &e)
//     {
//         std::cerr << "Caught standard exception: " << e.what() << std::endl;
//     }
//     catch (...)
//     {
//         std::cerr << "Caught an UNKNOWN exception." << std::endl;
//     }
// }

// Graph *Graph::getInstance(float speed, Graph::ControlMode mode)
// {
//     if (instance)
//     {
//         std::cerr << "\033[31mError: Graph instance already exists! Only one instance of Graph is allowed.\033[0m\n";
//         std::exit(EXIT_FAILURE);
//     }

//     instance = new Graph(speed, mode);
//     return instance;
// }

// void Graph::run()
// {
//     app->run(instance);
// }

// void Graph::register_object(GraphMathObject *obj)
// {
//     // Avoid duplicate registration
//     auto it = std::find(flat_scene_registry.begin(), flat_scene_registry.end(), obj);
//     if (it == flat_scene_registry.end())
//     {
//         flat_scene_registry.push_back(obj);

//         // If the scene is already running, initialize the object immediately
//         if (is_running && !obj->is_initialized)
//         {
//             obj->Init(GraphApp::drawCount++);
//         }
//     }
// }

// void Graph::play(Animation *animation, float duration)
// {
//     float t_duration = std::max(0.0f, duration);
//     animation->start_time = time_stamp;
//     animation->duration = t_duration;
//     animation->end_time = animation->start_time + animation->end_time;
//     animations.push_back(animation);
// }

// void Graph::play(std::vector<Animation*> anims, float duration){
//     float t_duration = std::max(0.0f, duration);
//     for (Animation *anim : anims)
//     {
//         anim->start_time = time_stamp;
//         anim->duration = t_duration;
//         animations.push_back(anim);
//     }

//     this->time_stamp += t_duration;
// }

// void Graph::play(CameraAnimation *camera_animation, float start_time, float duration)
// {
//     float t_duration = std::max(0.0f, duration);
//     float t_start = std::max(0.0f, start_time);
//     camera_animation->start_time = t_start;
//     camera_animation->end_time = t_start + t_duration;
//     camera_animation->duration = t_duration;
//     camera_animations.push_back(camera_animation);
// }

// void Graph::add(GraphMathObject *graph)
// {
//     graph->on_added_to_scene(this);
// }
