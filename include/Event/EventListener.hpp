#pragma once

#include <Event/Event.hpp>
#include <vector>

template <typename E>
class EventListener {
public:
    virtual ~EventListener() = default;
    virtual void onEvent(E& event) = 0;
};