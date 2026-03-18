#ifndef __FADE_HPP__
#define __FADE_HPP__

#include <GraphEngine/Animations/Animation.hpp>

// FadeIn: animates fillOpacity and strokeOpacity from 0 to 1 over the duration.
class FadeIn : public Animation {
public:
    FadeIn(GraphMathObject* object): Animation(object){
        // Start invisible
        object->setFillOpacity(0.0f);
        object->setStrokeOpacity(0.0f);
    }

    void play(float dt) override {
        if (dt < start_time) return;
        if (dt > end_time) {
            targetObject->setFillOpacity(1.0f);
            targetObject->setStrokeOpacity(1.0f);
            return;
        }
        float t = anim_timing_func((dt - start_time), duration);
        targetObject->setFillOpacity(t);
        targetObject->setStrokeOpacity(t);
    }
};

// FadeOut: animates fillOpacity and strokeOpacity from 1 to 0 over the duration.
class FadeOut : public Animation {
public:
    FadeOut(GraphMathObject *object)
        : Animation(object) { is_initialized = true; }

    void play(float dt) override {
        if (dt < start_time) return;
        if (dt > end_time) {
            targetObject->setFillOpacity(0.0f);
            targetObject->setStrokeOpacity(0.0f);
            return;
        }
        float t = 1.0f - anim_timing_func((dt - start_time), duration);
        targetObject->setFillOpacity(t);
        targetObject->setStrokeOpacity(t);
    }
};

// FadeToColor: cross-fades between two colors.
// class FadeToColor : public Animation {
//     GraphColor from_color, to_color;
// public:
//     FadeToColor(GraphMathObject* object, GraphColor to,
//                 float start_time = 0.0f, float duration = 1.0f)
//         : Animation(object, start_time, duration), to_color(to)
//     {
//         if (!object->colors.empty())
//             from_color = object->colors[0];
//     }

//     void play(float dt) override {
//         if (dt < start_time) return;
//         float t = (dt > end_time) ? 1.0f : anim_timing_func((dt - start_time), duration);
//         GraphColor blended;
//         blended.RED   = from_color.RED   + t * (to_color.RED   - from_color.RED);
//         blended.GREEN = from_color.GREEN + t * (to_color.GREEN - from_color.GREEN);
//         blended.BLUE  = from_color.BLUE  + t * (to_color.BLUE  - from_color.BLUE);
//         targetObject->setColor(blended);
//     }
// };

#endif
