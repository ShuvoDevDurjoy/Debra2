#pragma once
#ifndef __VAR_HPP__
#define __VAR_HPP__

#include <vector>

struct Var
{
private:
    std::vector<float> args;

    void add(float x)
    {
        args.push_back(x);
    }

    friend class Graph;

public:
    // Single argument constructor
    Var(float v)
    {
        add(v);
    }

    // Variadic constructor
    template <typename... Ts>
    Var(Ts... values)
    {
        (args.push_back(values), ...); // fold expression (C++17)
    }

    int size() const { return args.size(); }

    float operator[](int idx) const
    {
        if (idx < size())
            return args[idx];
        return 0.0f;
    }

    void addVar(float v)
    {
        args.push_back(v);
    }
};

#endif