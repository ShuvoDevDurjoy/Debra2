// #include "../include/GraphEngine/Graphs/Graph.hpp"
// #include "./Utils/functions.cpp"

// float line(float x, Var v)
// {
//     return x * tan(GraphUtilities::toRadians(v[0]));
// }
// float line3(float x, Var v)
// {
//     return x * tan(GraphUtilities::toRadians(v[0]));
// }
// float line2(float x, Var v)
// {
//     return v[0] + tan(GraphUtilities::toRadians(-45.0f)) * x;
// }

// float roseCurve(float theta)
// {
//     return cos(theta);
// }

// float cardroid(float theta, Var v)
// {
//     return v[0] * (1.0f + 2.0f * cos(theta));
// }
// float cardroid2(float theta, Var v)
// {
//     return v[0] * (1.0f - 2.0f * cos(theta));
// }
// float cardroid3(float theta, Var v)
// {
//     return v[0] * (1.0f + 2.0f * sin(theta));
// }
// float cardroid4(float theta, Var v)
// {
//     return v[0] * (1.0f - 2.0f * sin(theta));
// }

// std::pair<float, float> sunflowerPoint(float i)
// {
//     float c = 1.0f;
//     constexpr float GOLDEN_ANGLE = 2.39996323f; // radians
//     float theta = GraphUtilities::toRadians(i * 16.0f * GOLDEN_ANGLE);
//     float r = c * std::sqrt(i);
//     float x = r * std::cos(theta);
//     float y = r * std::sin(theta);
//     return {x, y};
// }

// float randomFunction(float t)
// {
//     return 2.0f * sin(t) + 2.0f * cos(t) + 2.0f * tan(t) + t * cos(t) + tan(t) * t - cos(t) / tan(t);
// }

// float para2(float t)
// {
//     return (cos(t) * tan(t) * sin(t) + t) / (t / 4.0f);
// }

// std::pair<float, float> para(float t)
// {
//     float x = (cos(t) * tan(t) * sin(t) * t);
//     float y = 5.0f + ((sin(t) + tan(t) / cos(t) - tan(t)) / 600.0f * tan(t) * sin(t) * cos(t) + cos(t) + tan(t)) * 6.0f / 5.0f - 6.0f + 9.0f / tan(t);
//     return {x, y};
// }

// // // float a = 27.28f;

// float para3(float x, Var v)
// {
//     // return ( x / 4.0f) +  10.0f * pow(cos((1.0f - (1.0f / a)) * a * x), 2.0f);
//     return v[2] + v[1] * pow(cos((1.0f - (1.0f / v[0])) * x), 2.0f);
// }

// float fn(float x)
// {
//     float y = sqrt(10.0f - pow(x, 2.0f));
//     return y;
// }

// std::pair<float, float> flowerCircle(float t, Var v)
// {
//     float R = v[0]; // base circle radius
//     float A = v[1]; // amplitude of petals
//     float k = v[2]; // number of petals

//     float r = R + A * cos(k * t);

//     float x = r * cos(t) + v[3];
//     float y = r * sin(t) + v[4];

//     return {x, y};
// }

// std::pair<float, float> flowerRose(float t, Var v)
// {
//     float a = v[0]; // size
//     float k = v[1]; // number of petals

//     float r = a * cos(k * t);

//     float x = r * cos(t) + v[2];
//     float y = r * sin(t) + v[3];

//     return {x, y};
// }

// void animation1(Graph *graph)
// {
//     graph->StartTime(0.0f);
//     graph->Duration(2.0f);
//     graph->Delay(2.0f);
//     graph->LoopTime(16.0f);
//     graph->MorphDuration(4.0f);
//     graph->insertVerticesRadians(para3, 0.0f, 30.0f, 30.0f, 13.0f);
//     graph->StartTime(6.0f);
//     graph->insertVerticesRadians(Functions::graphButterfly, 0.0f, 30.0f, -3.5f);
//     graph->StartTime(12.0f);
//     graph->insertVerticesRadians([](float x)
//                                  { return 14.0f; }, 0.0f, 30.0f);
//     graph->StartTime(18.0f);
//     graph->insertVerticesRadians([](float x)
//                                  { return 2.0f + 5.0f * cos(x); }, 0.0f, 30.0f);
//     graph->morph(0, 1);
//     graph->morph(1, 2);
//     graph->morph(2, 3);
//     graph->morph(3, 0);
// }

// void animation2(Graph *graph)
// {
//     graph->Duration(2.0f);
//     graph->MorphDuration(2.0f);
//     graph->Delay(2.0f);
//     graph->LoopTime(18.0f);
//     graph->StartTime(0.0f);
//     graph->insertVertices([](float x)
//                           { return 5.0f * sin(x); });
//     graph->StartTime(4.0f);
//     graph->insertVertices([](float x, Var v)
//                           { return line(x ,v); }, 27.0f);
//     graph->StartTime(8.0f);
//     graph->insertVertices([](float x, Var v)
//                           { return 5.0f + Functions::explodedTrigFusion(x, v); }, 10.0f, 0.05f);
//     graph->StartTime(12.0f);
//     graph->insertVertices([](float x)
//                           {
//                             return 5.0f * cos(x);});
//     graph->StartTime(16.0f);
//     graph->insertVertices([](float x, Var v)
//                           { return line(x, v); }, -27.0f);
//     graph->StartTime(20.0f);
//     graph->insertVertices([](float x, Var v)
//                           { return -5.0f + Functions::explodedTrigFusion(x, v); }, 10.0f, 0.05f);
//     graph->morph(0, 1);
//     graph->morph(1, 2);
//     graph->morph(2, 3);
//     graph->morph(3, 4);
//     graph->morph(4, 5);
//     graph->morph(5, 0);
// }

// void animation3(Graph *graph, float s, float start){
//     float alltime = 3.0f;
//     graph->setAnimationMode(AnimationMode::ONCE_AND_LOOP_BACK);
//     graph->StartTime(start);
//     graph->Duration(start + alltime);
//     graph->Delay(start + alltime);
//     graph->MorphDuration(start + alltime);
//     graph->LoopTime(start + alltime);

//     graph->insertVerticesParametric(Functions::graphButterflyParam, -15.0f, 15.0f, -4.0f, -16.0f, 0.0f);
//     graph->insertVerticesParametric(Functions::graphButterflyParam, -15.0f, 15.0f, -4.0f, 16.0f, 0.0f);

//     graph->StartTime(start + 2 * (start + alltime));
//     // graph->insertVerticesParametric(flowerRose, 0.0f, 2.0f, 10.0f, 10.0f, 16.0f, 0.0f);
//     // graph->insertVerticesParametric(flowerRose, 0.0f, 2.0f, 10.0f, 10.0f, -16.0f, 0.0f);

//     // graph->insertVerticesParametric(flowerCircle, 0.0f, 2.0f, 3.0f, 5.0f, 12.0f, 16.0f, 0.0f);
//     // graph->insertVerticesParametric(flowerCircle, 0.0f, 2.0f, 3.0f, 5.0f, 12.0f, -16.0f, 0.0f);

//     graph->insertVerticesParametric(Functions::heartCurve, -15.0f, 15.0f, 0.6f, 30.0f, 3.0f);
//     graph->insertVerticesParametric(Functions::heartCurve, -15.0f, 15.0f, 0.6f, -30.0f, 3.0f);
//     graph->morph(s + 0, s + 2);
//     graph->morph(s + 1, s + 3);
//     graph->morph(s + 2, s + 0);
//     graph->morph(s + 3, s + 1);
// }

// std::pair<float, float> randomly(float t, Var v){
//     float x = v[0] * cos(t) + v[2];
//     float y = v[1] * sin(0.98f * t);
//     return {x  , y};
// }

// std::pair<float, float> arka(float t){
//     float x = 5.0f + (sin(t) + log(t) / cos(t) / t) / 60.0f * tan(t) * sin(t) * cos(t) - cos(t) + 6.0f / 5.0f - 6.0f + 9.0f / tan(t) + 500.0f;
//     float y = 5.0f + sin(t) + tan(t) / 600.0f * 50.0f;
//     return {x, y};
// }

// std::pair<float, float> fouriarTransformCartesian(float t, Var v){
//     float x = v[1] * cos(t);
//     float y = (pow(sin(v[0] * x), 3.0f)) / (v[0] * 1.0f);
//     return {x, y * v[1]};
// }

// float maths(float t){
//     float x = t;
//     float y = pow(2.0f, x);
//     return y;
// }

// float maths2(float x){
//     return pow(2.0f, -1.0f * x);
// }

// void drawMatrixTranspose1(Graph *graph){
//     GraphColor *gc_1 = new GraphColor(0.0f, 1.0f, 0.0f);
//     GraphColor *gc_2 = new GraphColor(0.0f, 1.0f, 1.0f);
//     GraphColor *gc_3 = new GraphColor(0.0f, 0.0f, 1.0f);

//     graph->StartTime(0.0f);
//     graph->Duration(1.0f);
//     graph->drawPoints(1.0f, 4.0f, 0.1f, gc_1);
//     graph->StartTime(1.0f);
//     graph->drawPoints(3.0f, 2.0f, 0.1f, gc_2);
//     graph->StartTime(2.0f);
//     graph->drawPoints(1.0f, 3.0f, 0.1f, gc_3);
//     graph->StartTime(3.0f);
//     graph->drawPoints(4.0f, 2.0f, 0.1f, gc_3);
//     graph->StartTime(4.0f);
//     graph->drawPoints(17.0f, 11.0f, 0.2f, gc_1);
//     graph->StartTime(5.0f);
//     graph->drawPoints(11.0f, 13.0f, 0.2f, gc_2);

//     graph->StartTime(1.0f);
//     graph->drawLines({{0.0f, 0.0f}, {1.0f, 4.0f}}, gc_1);
//     graph->StartTime(2.0f);
//     graph->drawLines({{0.0f, 0.0f}, {3.0f, 2.0f}}, gc_2);
//     graph->StartTime(3.0f);
//     graph->drawLines({{0.0f, 0.0f}, {1.0f, 3.0f}}, gc_3);
//     graph->StartTime(4.0f);
//     graph->drawLines({{0.0f, 0.0f}, {4.0f, 2.0f}}, gc_3);

//     graph->StartTime(5.0f);
//     graph->drawLines({{0.0f, 0.0f}, {17.0f, 11.0f}}, gc_1);
//     graph->StartTime(6.0f);
//     graph->drawLines({{0.0f, 0.0f}, {11.0f, 13.0f}}, gc_2);
// }

// void drawMatrixTranspose2(Graph *graph){
//     GraphColor *gc_1 = new GraphColor(0.0f, 1.0f, 0.0f);
//     GraphColor *gc_2 = new GraphColor(0.0f, 1.0f, 1.0f);
//     GraphColor *gc_3 = new GraphColor(0.0f, 0.0f, 1.0f);

//     graph->StartTime(0.0f);
//     graph->Duration(1.0f);
//     graph->drawPoints(5.0f, 4.0f, 0.1f, gc_1);
//     graph->StartTime(1.0f);
//     graph->drawPoints(9.0f, 3.0f, 0.1f, gc_2);
//     graph->StartTime(2.0f);
//     graph->drawPoints(5.0f, 9.0f, 0.1f, gc_3);
//     graph->StartTime(3.0f);
//     graph->drawPoints(4.0f, 3.0f, 0.1f, gc_3);
//     graph->StartTime(4.0f);
//     graph->drawPoints(17.0f, 11.0f, 0.2f, gc_1);
//     graph->StartTime(5.0f);
//     graph->drawPoints(11.0f, 13.0f, 0.2f, gc_2);

//     graph->StartTime(1.0f);
//     graph->drawLines({{0.0f, 0.0f}, {1.0f, 4.0f}}, gc_1);
//     graph->StartTime(2.0f);
//     graph->drawLines({{0.0f, 0.0f}, {3.0f, 2.0f}}, gc_2);
//     graph->StartTime(3.0f);
//     graph->drawLines({{0.0f, 0.0f}, {1.0f, 3.0f}}, gc_3);
//     graph->StartTime(4.0f);
//     graph->drawLines({{0.0f, 0.0f}, {4.0f, 2.0f}}, gc_3);
//     graph->StartTime(5.0f);
//     graph->drawLines({{0.0f, 0.0f}, {17.0f, 11.0f}}, gc_1);
//     graph->StartTime(6.0f);
//     graph->drawLines({{0.0f, 0.0f}, {11.0f, 13.0f}}, gc_2);
// }

// float circle(float x){
//     return 14.0f;
// }

// // int main(){
// //     Graph *graph = Graph::getInstance(20.0f);

// //     graph->setAnimationMode(AnimationMode::INFINITE);

// //     graph->StartTime(0.0f);
// //     graph->Duration(4.0f);
// //     graph->Delay(2.0f);
// //     RadianFunctionList rt{cardroid, cardroid2, cardroid3, cardroid4};
// //     float radius = 4.0f;
// //     GraphColor *color1 = new GraphColor(1.0f, 0.0f, 0.0f);
// //     graph->insertVerticesRadiansList(rt, 0.0f, 2.0f, radius);
// //     graph->insertVerticesRadians(circle);
// //     // graph->setAnimationMode(AnimationMode::ONCE_AND_LOOP_BACK);

// //     // animation1(graph);

// //     graph->run();
// // }

// void animate(Graph* graph){
//     graph->StartTime(0.0f);
//     graph->Duration(5.0f);
//     graph->Delay(2.0f);
//     graph->MorphDuration(5.0f);
//     singletonGraph *graph1 = graph->insertVerticesParametric(randomly, -25.0f, 25.0f, 7.0f, 7.0f, 0.0f);
//         // graph->StartTime(4.0f);
//     singletonGraph *graph2 = graph->insertVerticesParametric(randomly, -25.0f, 25.0f, 7.0f, 7.0f, -15.0f);
//     // graph->StartTime(8.0f);
//     singletonGraph *graph3 = graph->insertVerticesParametric(randomly, -25.0f, 25.0f, 7.0f, 7.0f, 15.0f);

//     graph1->morphGraph(graph2, 5.0f, 5.0f);
//     graph1->morphGraph(graph3, 5.0f, 5.0f);
//     // graph1->morphGraph(graph4, 5.0f, 5.0f);

//     graph->play(graph1);
//     graph->play(graph2);
//     graph->play(graph3);
// }

// int main()
// {
//     Graph *graph = Graph::getInstance(20.0f);

//     // Graph *graph2 = Graph::getInstance(4.0f);
//     // graph->speed = 200;
//     // Graph *graph2 = Graph::getInstance();

//     //**************************************Logic to Draw Functions Starts Here**********************************************/

//     // Functions to draw in Cartisian Coordinate System
//     // graph->insertVertices(fn, 2.0f);
//     // graph->insertVertices({TO_CARTISAN_VARIANT(line2), TO_CARTISAN_VARIANT(line3)});

//     // graph->insertVertices([](float x){return 15.0f;});
//     // graph->insertVertices([](float x){return -15.0f;});
//     // graph->setAnimationMode(AnimationMode::ONCE);
//     // graph->StartTime(0.0f);
//     // graph->LoopTime(2.0f);
//     // graph->Duration(5.0f);
//     // graph->MorphDuration(3.0f);
//     // graph->StartTime(0.0f);
//     // graph->setAnimationMode(AnimationMode::ONCE);
//     // graph->insertVertices([](float x){ return sin(x) * 3.0f + 5.0f;});
//     // graph->setAnimationMode(AnimationMode::ONCE_AND_REMOVE);
//     // singletonGraph *graph1 = graph->insertVertices(Functions::explodedTrigFusion, 10.0f, 0.09f);
//     // singletonGraph *graph2 = graph->insertVertices([](float t){return sin(t);});
//     // animate(graph);
//     // graph->play(graph1);
//     // graph1->
//     // graph1->morphGraph(graph2);

//     // graph->StartTime(0.0f);
//     // graph->Duration(2.0f);
//     // graph->insertVerticesParametric(para, -30.0f, 30.0f);

//     // graph->setAnimationMode(AnimationMode::INFINITE);
//     // graph->StartTime(0.0f);
//     // graph->Duration(2.0f);
//     // graph->Delay(2.0f);
//     // graph->MorphDuration(4.0f);
//     // graph->LoopTime(0.0f);
//     // CartesianFunctionList t{line3, line};
//     // graph->insertVerticesList(t);
//     // graph->morph(6, 7);
//     // graph->morph(7, 6);
//     // graph->insertVertices([](float x){return x * x * x;});
//     // graph->insertVertices([](float x){return 2.0f;});
//     // graph->insertVertices([](float x){return Functions::sineModulated(x);});
//     // graph->setAnimationMode(AnimationMode::ONCE_AND_REMOVE);
//     // graph->insertVertices([](float x){return 1.0f / (3.0f * tan(x));});
//     // graph->insertVertices([](float x){return tan(x);});

//     // Functions to draw in Radia Coordinate System
//     //  graph->insertVerticesRadians([](float r){return 9.0f;});
//     // for (float a = 10.0f; a <= 70.0f; a+= 10.0f){
//     // }
//     // graph->StartTime(14.0f);
//     // graph->setAnimationMode(AnimationMode::ONCE_AND_LOOP_BACK);
//     // graph->Duration(2.0f);
//     // graph->Delay(2.0f);
//     // graph->MorphDuration(2.0f);
//     // graph->LoopTime(2.0f);
//     // graph->insertVerticesRadians([](float t)
//     //                              { return 15.0f; }, 0.0, 1.0, 30.0f, 4.0f, 1.0f);
//     // graph->StartTime(4.0f);
//     // graph->insertVerticesRadians(para3, 0.0f, 10.0f, 10.0f, 13.0f);
//     // graph->insertVerticesRadians(para3, 0.0f, 30.0f, 30.0f, 8.0f, 7.0f);
//     // graph->morph(1, 2);
//     // graph->morph(2, 1);
//     // graph->StartTime(0.0f);
//     // graph->Duration(6.0f);
//     // graph->Delay(6.0f);
//     // graph->MorphDuration(6.0f);
//     // graph->LoopTime(18.0f);
//     // graph->insertVerticesParametric(randomly, -25.0f, 25.0f, 14.0f, 14.0f, -15.0f);
//     // graph->insertVerticesParametric(randomly, 0.0f, 1.0f, 14.0f, 14.0f,  15.0f);
//     // graph->insertVerticesParametric(randomly, -25.0f, 25.0f, 14.0f, 14.0f);
//     // graph->insertVerticesParametric(randomly, -25.0f, 25.0f, 12.0f, 12.0f);
//     // graph->insertVerticesParametric(randomly, -25.0f, 25.0f, 10.0f, 10.0f);
//     // graph->insertVerticesParametric(randomly, -25.0f, 25.0f, 8.0f, 8.0f);
//     // graph->insertVerticesParametric(randomly, -25.0f, 25.0f, 6.0f, 6.0f);
//     // graph->insertVerticesParametric(randomly, -25.0f, 25.0f, 4.0f, 4.0f);
//     // graph->insertVerticesParametric(randomly, -25.0f, 25.0f, 2.0f, 2.0f);
//     // graph->insertVerticesRadians(para3, 0.0f, 50.0f, 50.0f, 13.0f);
//     // graph->insertVerticesRadians(para3, 0.0f, 60.0f, 60.0f, 13.0f);
//     // graph->insertVerticesRadians(para3, 0.0f, 70.0f, 70.0f, 13.0f);
//     // graph->insertVerticesParametric(randomly, -25.0f, 25.0f, 14.0f, 14.0f, -15.0f);
//     // graph->insertVerticesParametric(randomly, 0.0f, 1.0f, 14.0f, 14.0f,  15.0f);
//     // graph->insertVerticesParametric(randomly, -25.0f, 25.0f, 14.0f, 14.0f);
//     // graph->insertVerticesParametric(randomly, -25.0f, 25.0f, 12.0f, 12.0f);
//     // graph->insertVerticesParametric(randomly, -25.0f, 25.0f, 10.0f, 10.0f);
//     // graph->insertVerticesParametric(randomly, -25.0f, 25.0f, 8.0f, 8.0f);
//     // graph->insertVerticesParametric(randomly, -25.0f, 25.0f, 6.0f, 6.0f);
//     // graph->insertVerticesParametric(randomly, -25.0f, 25.0f, 4.0f, 4.0f);
//     // graph->insertVerticesParametric(randomly, -25.0f, 25.0f, 2.0f, 2.0f);
//     // graph->insertVerticesRadians(para3, 0.0f, 50.0f, 50.0f, 13.0f);
//     // graph->insertVerticesRadians(para3, 0.0f, 60.0f, 60.0f, 13.0f);
//     // graph->insertVerticesRadians(para3, 0.0f, 70.0f, 70.0f, 13.0f);
//     // graph->StartTime(10.0f);
//     // RadianFunctionList rt{cardroid, cardroid2, cardroid3, cardroid4};
//     // float a = 30.0f;
//     // graph->StartTime(21.0f);
//     // animation1(graph);
//     // graph->insertVerticesRadiansList(rt, 0.0f, 2.0f, 3.0f);
//     // graph->StartTime(28.0f);
//     // graph->insertVertices({Functions::explodedTrigFusion2});
//     // graph->insertVerticesRadians(para3, 0.0f, a, a, 13.0f);
//     // graph->StartTime(35.0f);
//     // graph->insertVertices(roseCurve);
//     // graph->StartTime(12.0f);
//     // graph->insertVerticesParametric(Functions::heartCurve, -25.0f, 25.0f, 0.8f,  20.0f, 3.0f);
//     // graph->insertVerticesParametric(Functions::heartCurve, -25.0f, 25.0f, 0.8f, -20.0f, 3.0f);
//     // graph->insertVerticesParametric(para, -25.0f, 25.0f);
//     //   graph->insertVerticesParametric(sunflowerPoint, 0.0f, 50.0f);
//     //   graph->insertVerticesParametric(Functions::hypotrochoid, 0.0f, 50.0f);
//     // graph->insertVerticesParametric(Functions::chaoticFractal, 0.0f, 50.0f, 8.0f);
//     //    graph->insertVerticesParametric(Functions::harmonograph, 0.0f, 50.0f);
//     // graph->StartTime(24.0f);
//     // graph->insertVerticesParametric(Functions::spirograph, -2.0f, 2.0f, -14.0f, 10.0f, 3.0f);
//     // graph->insertVerticesParametric(Functions::spirograph, -25.0f, 25.0f,  14.0f, 10.0f, 3.0f);
//     // graph->morph(0, 2);
//     // graph->morph(1, 3);
//     // graph->morph(2, 4);
//     // graph->morph(3, 5);
//     // graph->morph(4, 1);
//     // graph->morph(5, 0);

//     // Functions to draw in Parametric Coordinate System
//     //  graph->insertVerticesParametric(sunflowerPoint, 0.0f, 1.0f);
//     // graph->StartTime(42.0f);
//     //  graph->insertVerticesParametric(Functions::riemannProjection, 0.0f, 10.0f);
//     // graph->setAnimationMode(AnimationMode::INFINITE);
//     // graph->StartTime(49.0f);
//     // graph->Duration(5.0f);
//     // graph->Delay(2.0f);
//     // graph->insertVerticesRadians(Functions::graphButterfly, 0.0f, 2.0f, -4.0f);
//     // graph->insertVerticesParametric(Functions::lissajous, 0.0f, 2.0f);

//     //***************************************Logic to draw Functions Ends Here*************************************************/

//     graph->run();

//     return 0;
// }
