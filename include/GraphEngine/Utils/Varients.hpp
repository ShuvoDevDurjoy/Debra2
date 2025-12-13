#ifndef __VARIENTS__HPP__
#define __VARIENTS__HPP__

#include <variant>
#include <vector>
#include "../Math/Var.hpp"

// variant that let the user to input both (float) or (float, Var) function reference
// for cartesian curve
using CartisanFunctionTypeVariant = std::variant<float (*)(float), float (*)(float, Var)>;
// variant that let the user to input both (float) or (float, Var) function reference
// for Polar curve
using RadianFunctionTypeVariant = std::variant<float (*)(float), float (*)(float, Var)>;
// variant that let the user to input both (float) or (float, Var) function reference
// for Parametric curve
using ParametricFunctionTypeVariant = std::variant<std::pair<float, float> (*)(float), std::pair<float, float> (*)(float, Var)>;

struct Ticks
{
    int tickStart;
    int tickEnd;
};

// Structure to convert Cartisian Function to Cartisain Type Variant
// which convert initializer_list to CartesianFunctionTypeVariant
struct CartesianFunctionList
{
private:
    std::vector<CartisanFunctionTypeVariant> funcs;
    friend class Graph;

public:
    template <typename... Fns>
    CartesianFunctionList(Fns... fns) : funcs{CartisanFunctionTypeVariant(fns)...} {}
};

// Structure to convert Radian Function to Radian Type Variant
// which convert initializer_list to RadianFunctionTypeVariant
struct RadianFunctionList
{
private:
    std::vector<RadianFunctionTypeVariant> funcs;
    friend class Graph;

public:
    template <typename... Fns>
    RadianFunctionList(Fns... fns) : funcs{RadianFunctionTypeVariant(fns)...} {}
};

// Structure to convert Parametric Function to Parametric Type Variant
// which convert initializer_list to ParametricFunctionTypeVariant
struct ParametricFunctionList
{
private:
    std::vector<RadianFunctionTypeVariant> funcs;
    friend class Graph;

public:
    template <typename... Fns>
    ParametricFunctionList(Fns... fns) : funcs{ParametricFunctionTypeVariant(fns)...} {}
};

// parametric object for parametric curve
struct parametricObject
{
    std::pair<float, float> (*function)(float);
    float minRange;
    float maxRange;
};

#endif