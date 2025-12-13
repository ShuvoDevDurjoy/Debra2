#pragma once
#ifndef __VAR_HPP__
#define __VAR_HPP__


#include <vector>

// Variable for Varags to be accessible
// via this Variable
struct Var
{
private:
    // argument for the Varags that will be
    // accessible through array like indexing via
    // overloaded operator
    std::vector<float> args;
    void add(float x)
    {
        args.push_back(x);
    }

    friend class Graph;

public:
    // return size of the Var Object
    int size() { return args.size(); }

    // method overfload for array like indexing
    float operator[](int idx)
    {
        if (idx < size())
        {
            return args[idx];
        }
        return 0.0f;
    }

    void addVar(float v){
        args.push_back(v);
    }
};

#endif