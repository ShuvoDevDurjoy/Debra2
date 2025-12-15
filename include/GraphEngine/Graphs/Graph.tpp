// #include "Graph.hpp"
// #ifndef M_PI
// #define M_PI 3.14159265358979323846
// #endif

// template <typename... T>
// singletonGraph* Graph::insertVertices(CartisanFunctionTypeVariant fn, T... t)
// {
//     GraphColor *gc = new GraphColor();
//     singletonGraph *graph = new singletonGraph(shader, getStart(), getDuration(), getDelay(), getLoopTime(), getMorphDuration());
//     graph->setColor(gc);
//     graph->setAnimationMode(Graph::ANIMATION_MODE);
//     Var v;
//     addToVar(v, t...);
//     float i;
//     float from = round((-(stepsx / 2.0f) - 1.0f) * 100.0f) / 100.0f;
//     float to = round(((stepsx / 2.0f) + 1.0f) * 100.0f) / 100.0f;
//     for (i = from; i <= to; i = round((i + 0.1f) * 100.0f) / 100.0f)
//     {
//         float x = normalizeX(i);
//         float y;
//         std::visit([&](auto &&fn_inner)
//                    {
//             using FnType = std::decay_t<decltype(fn_inner)>;
//             if constexpr (std::is_invocable_v<FnType, float, Var>)
//             {
//                 y = fn_inner(i, v);
//             }
//             else if constexpr (std::is_invocable_v<FnType, float>)
//             {
//                 y = fn_inner(i);
//             }
//             else
//             {
//                 static_assert(false, "Function must be callable with float or (float, Var)");
//             } }, fn);
//         y = normalizeY(y);
//         y = y > 1.0f ? 1.0f : y < -1.0f ? -1.0f
//                                         : y;
//         graph->add({x, y});
//     }
//     return graph;
// }

// template <typename... T>
// singletonGraph* Graph::insertVerticesRadians(RadianFunctionTypeVariant fn, float s, float f, T... t)
// {
//     GraphColor *gc = new GraphColor();
//     singletonGraph* graph = new singletonGraph(shader, getStart(), getDuration(), getDelay(), getLoopTime(), getMorphDuration());
//     graph->setColor(gc);
//     graph->setAnimationMode(Graph::ANIMATION_MODE);
//     Var v;
//     addToVar(v, t...);
//     float step = GraphUtilities::toRadians(2.0f);
//     for (float i = s * M_PI; i <= f * M_PI; i += step)
//     {
//         float radius;

//         std::visit([&](auto &&fn_inner)
//                    {
//             using FnType = std::decay_t<decltype(fn_inner)>;
//             if constexpr (std::is_invocable_v<FnType, float, Var>)
//             {
//                 radius = fn_inner(i, v);
//             }
//             else if constexpr (std::is_invocable_v<FnType, float>)
//             {
//                 radius = fn_inner(i);
//             }
//             else
//             {
//                 static_assert(false, "Function must be callable with float or (float, Var)");
//             } }, fn);

//         float x = normalizeX(radius * cos(i));
//         float y = normalizeY(radius * sin(i));
//         graph->add({x, y});
//     }
//     return graph;
// }

// template <typename... T>
// singletonGraph* Graph::insertVerticesParametric(ParametricFunctionTypeVariant fn, float minRange, float maxRange, T... t)
// {
//     GraphColor *gc = new GraphColor();
//     singletonGraph* graph = new singletonGraph(shader, getStart(), getDuration(), getDelay(), getLoopTime(), getMorphDuration());
//     graph->setColor(gc);
//     graph->setAnimationMode(Graph::ANIMATION_MODE);
//     Var v;
//     addToVar(v, t...);

//     float step = GraphUtilities::toRadians(0.5f);
//     for (float t = minRange * M_PI; t <= maxRange * M_PI; t += step)
//     {
//         std::pair<float, float> pt;
//         std::visit([&](auto &&fn_inner)
//                    {
//             using FnType = std::decay_t<decltype(fn_inner)>;
//             if constexpr (std::is_invocable_v<FnType, float, Var>)
//             {
//                 pt = fn_inner(t, v);
//             }
//             else if constexpr (std::is_invocable_v<FnType, float>)
//             {
//                 pt = fn_inner(t);
//             }
//             else
//             {
//                 static_assert(false, "Function must be callable with float or (float, Var)");
//             } }, fn);
//         float x = normalizeX(pt.first);
//         float y = normalizeY(pt.second);
//         graph->add({x, y});
//     }

//     return graph;
// }

// template <typename... T>
// void Graph::insertVerticesList(CartesianFunctionList fns, T... t)
// {
//     for (auto &fn : fns.funcs)
//     {
//         insertVertices(fn, t...);
//     }
// }

// template <typename... T>
// void Graph::insertVerticesRadiansList(RadianFunctionList fns, float minRange, float maxRange, T... t)
// {
//     for (auto &fn : fns.funcs)
//     {
//         insertVerticesRadians(fn, minRange, maxRange, t...);
//     }
// }

// template <typename... T>
// void Graph::insertVerticesParametricList(ParametricFunctionList fns, float minRange, float maxRange, T... t)
// {
//     for (auto &fn : fns.funcs)
//     {
//         insertVerticesParametric(fn, minRange, maxRange, t...);
//     }
// }
