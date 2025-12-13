#ifndef __UTILS_H__
#define __UTILS_H__

#include "../include/glad.h"
#include <string>
#include <sstream>
#include <fstream>
#include <iostream>
#include <vector>
#include <math.h>
#include <filesystem>

std::string readFile(std::string s);
void generateVertices(std::vector<float> &, float );

#endif