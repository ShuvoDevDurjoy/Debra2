// #include "../../include/GraphEngine/Animations/Translate.hpp"

// Translate::Translate(GraphObject *object, glm::vec3 to, float start_time, float duration, Position pos) : Animation(object, start_time, duration)
// {
//     this->to = to;
//     this->pivot = pos;
// }

// inline glm::vec3 getPosition(float x, float y, float width, float height, Position pos)
// {
//     glm::vec3 position;
//     switch (pos)
//     {
//     case Position::LEFT:
//         position = glm::vec3(x - width, y, 0);
//         break;
//     case Position::RIGHT:
//         position = glm::vec3(x + width, y, 0);
//         break;
//     case Position::TOP:
//         position = glm::vec3(x, y + height, 0);
//         break;
//     case Position::BOTTOM:
//         position = glm::vec3(x, y - height, 0);
//         break;
//     case Position::TOP_LEFT:
//         position = glm::vec3(x - width, y + height, 0);
//         break;
//     case Position::TOP_RIGHT:
//         position = glm::vec3(x + width, y + height, 0);
//         break;
//     case Position::BOTTOM_LEFT:
//         position = glm::vec3(x - width, y - height, 0);
//         break;
//     case Position::BOTTOM_RIGHT:
//         position = glm::vec3(x + width, y - height, 0);
//         break;
//     case Position::NONE:
//         position = glm::vec3(x + width / 2, y - height / 2, 0);
//         break;
//     }

//     return position;
// }

// Translate::Translate(GraphObject *object, Position to, float start_time, float duration, Position pos) : Animation(object, start_time, duration)
// {
//     this->to = getPosition(object->x, object->y, object->width, object->height, to);
//     this->pivot = pos;
// }

// void Translate::Init(){
//     x = targetObject->x;
//     y = targetObject->y;
//     width = targetObject->width;
//     height = targetObject->height;

//     offset = glm::vec3(x, y, 0);
//     this->to = to - offset + getPivotPosition(pivot);

//     this->is_initialized = true;
// }

// glm::vec3 Translate::getPivotPosition(Position pos)
// {
//     glm::vec3 position;
//     switch (pos)
//     {
//     case LEFT:
//         position = glm::vec3(0, height / 2.0f, 0);
//         break;
//     case RIGHT:
//         position = glm::vec3(-width, height / 2.0f, 0);
//         break;
//     case TOP_LEFT:
//         position = glm::vec3(0, 0, 0);
//         break;
//     case BOTTOM_LEFT:
//         position = glm::vec3(0, height, 0);
//         break;
//     case TOP_RIGHT:
//         position = glm::vec3(-width, 0, 0);
//         break;
//     case BOTTOM_RIGHT:
//         position = glm::vec3(-width, height, 0);
//         break;
//     case TOP:
//         position = glm::vec3(-width / 2.0f, 0, 0);
//         break;
//     case BOTTOM:
//         position = glm::vec3(-width / 2.0f, height, 0);
//         break;
//     case NONE:
//         position = glm::vec3(-width / 2.0f, height / 2.0f, 0);
//     }

//     return position;
// }

// void Translate::play(float dt)
// {
//     if (dt < start_time)
//     {
//         progress = 0;
//         updatingPos = false;
//         return;
//     }

//     if (dt >= end_time)
//     {
//         progress = 1;
//         updatingPos = false;
//         return;
//     }

//     if(! is_initialized){
//         Init();
//     }

//     updatingPos = true;
//     float cur_prog = AnimationTimmingFunction::easeInOutExpo( dt - start_time, duration);
//     pos = offset + to * cur_prog;
//     if(GraphApp::isAlive == false)
//         return;
//     // targetObject->moveTo(pos);
//     // targetObject->translate = pos;
//     targetObject->setTranslate(pos);
// }

// // Scale Animation constructor
// Scale::Scale(GraphObject *object, glm::vec3 targetScale, float start_time, float duration)
//     : Animation(object, start_time, duration)
// {
//     this->targetScale = targetScale;
// }

// // Play function
// void Scale::play(float dt)
// {
//     if (dt < start_time)
//         return;
//     if (dt >= end_time)
//     {
//         targetObject->setScale(glm::vec3(1, 1, 1)); // ensure exact final scale at end
//         return;
//     }

//     float progress = (dt - start_time) / duration;
//     glm::vec3 current = glm::mix(startScale, targetScale, progress);
//     glm::vec3 scaleFactor = current / current_scale;
//     // targetObject->scaleTo(scaleFactor.x, scaleFactor.y);
//     targetObject->scale_x = scaleFactor.x;
//     targetObject->scale_y = scaleFactor.y;
//     current_scale = current;
// }


// Rotation::Rotation(GraphObject* object, glm::vec3 rot_amount, Position pos, float start_time, float duration): Animation(object, start_time, duration){
//     this->x = object->x;
//     this->y = object->y;
//     this->width = object->width;
//     this->height = object->height;
//     this->start_time = start_time;
//     this->end_time = start_time + duration;
//     rot_pivot = glm::vec3(x, y, 0) - getPivotPosition(pos);
//     // std::cout << "pivot position is: " << std::endl;
//     this->pos = pos;
//     // std::cout << "pivot x is: " << rot_pivot.x << " y is : " << rot_pivot.y << " and z is: " << rot_pivot.z << std::endl;
//     this->rot_amount = rot_amount;
//     this->currentRot = glm::vec3(0, 0, 0);
//     calc_pivot = true;
//     // std::cout << "x is: " << x << " y is: " << y << " width is : " << width << " heighth is : " << height << std::endl;
// }
// Rotation::Rotation(GraphObject* object, glm::vec3 rot_amount, glm::vec3 pivot, float start_time, float duration): Animation(object, start_time, duration){
//     this->x = object->x;
//     this->y = object->y;
//     this->width = object->width;
//     this->height = object->height;
//     this->start_time = start_time;
//     this->end_time = start_time + duration;
//     rot_pivot = pivot;
//     // std::cout << "pivot position is: " << std::endl;
//     // std::cout << "pivot x is: " << rot_pivot.x << " y is : " << rot_pivot.y << " and z is: " << rot_pivot.z << std::endl;
//     this->rot_amount = rot_amount;
//     this->currentRot = glm::vec3(0, 0, 0);
//     calc_pivot = false;
//     // std::cout << "x is: " << x << " y is: " << y << " width is : " << width << " heighth is : " << height << std::endl;
// }

// glm::vec3 Rotation::getPivotPosition(Position pos)
// {
//     glm::vec3 position;
//     switch (pos)
//     {
//         case LEFT:
//         position = glm::vec3(0, height / 2, 0);
//         break;
//     case RIGHT:
//         position = glm::vec3(-width, height / 2, 0);
//         break;
//     case TOP_LEFT:
//         position = glm::vec3(0, 0, 0);
//         break;
//     case BOTTOM_LEFT:
//         position = glm::vec3(0, height, 0);
//         break;
//     case TOP_RIGHT:
//         position = glm::vec3(-width, 0, 0);
//         break;
//     case BOTTOM_RIGHT:
//         position = glm::vec3(-width, height, 0);
//         break;
//     case TOP:
//         position = glm::vec3(-width / 2, 0, 0);
//         break;
//     case BOTTOM:
//         position = glm::vec3(-width / 2, height, 0);
//         break;
//     case NONE:
//         position = glm::vec3(-width / 2, height / 2, 0);
//         break;
//     }

//     return position;
// }

// inline glm::vec3 safeDiv(glm::vec3 numerator, glm::vec3 denominator){
//     glm::vec3 result;
//     result.x = (denominator.x == 0 ? 0 : numerator.x / denominator.x);
//     result.y = (denominator.y == 0 ? 0 : numerator.y / denominator.y);
//     result.z = (denominator.z == 0 ? 0 : numerator.z / denominator.z);

//     return result;
// }

// void Rotation::Init(){
//     this->x = targetObject->x;
//     this->y = targetObject->y;
//     this->width = targetObject->width;
//     this->height = targetObject->height;
//     if(calc_pivot)
//     rot_pivot = glm::vec3(x, y, 0) - getPivotPosition(pos);
//     this->rot_amount = rot_amount;
//     is_initialized = true;
// }

// void Rotation::play(float dt){
//     if (dt < start_time)
//     return;
//     if (dt >= end_time)
//     {
//         return;
//     }
//     if(! is_initialized)
//     Init();

//     float progress = AnimationTimmingFunction::easeInOutExpo(dt - start_time, duration);
    
//     glm::vec3 current = glm::mix(startRot, rot_amount, progress);
//     glm::vec3 rotFactor = current - currentRot;
    
//     total += rotFactor;
//     targetObject->setRotation(rotFactor, rot_pivot);
//     currentRot += rotFactor;
// }