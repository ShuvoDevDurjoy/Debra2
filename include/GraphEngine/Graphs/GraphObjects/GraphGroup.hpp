#ifndef __GRAPHGROUP_HPP__
#define __GRAPHGROUP_HPP__

#include "GraphObject.hpp"

class GraphGroup: public GraphObject
{
public:
    GraphGroup() {}

    void synchronizeWithDelay(float delay){
        // float total_duration = 0;
        float begin_time = startTime;
        int subGraphSize = subGraphObjects.size();
        float ind_duration = 1;
        if (delay * subGraphSize >= duration)
        {
            delay = (duration) / (subGraphSize);
            ind_duration = delay;
        }
        else{
            ind_duration = (duration - delay * (subGraphSize - 1)) / subGraphSize;
        }
        if(ind_duration * subGraphSize + (subGraphSize - 1) * delay > duration){

        }
        for (GraphObject *graph : subGraphObjects)
        {
            graph->setStartTime(begin_time);
            graph->setDuration(ind_duration);
            begin_time += delay;
        }
        setDuration(subGraphObjects.size() * ind_duration - (subGraphObjects.size()-1) * delay);
    }

    void synchronize(float run_time){
        setDuration(run_time);
        synchronizeWithDelay(duration / subGraphObjects.size());
    }
};

#endif