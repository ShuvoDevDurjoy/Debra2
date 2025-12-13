#define STB_IMAGE_IMPLEMENTATION
#include "../../include/GraphEngine/Utils/Utils.hpp"
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

std::string readFile(std::string filename)
{

    // stream to open the file
    std::ifstream file;
    // stream to contain the details of file
    std::stringstream fileStream;

    file.exceptions(std::ifstream::failbit | std::ifstream::badbit);

    try
    {
        // opening the file with the stream
        file.open(filename);

        // reading the file into the string stream at once
        fileStream << file.rdbuf();

        // close the file after reading
        file.close();
    }
    catch (std::ifstream::failure &e)
    {
        std::cout << "Error with Opening " << filename << " file" << std::endl;
        throw e;
    }

    // reding the file into a string
    std::string fileContent = fileStream.str();

    // returning the file content in form of string
    return fileContent;
}

void generateVertices(std::vector<float> &ver_arr, float r)
{
    // Center vertex (for triangle fan)
    ver_arr.push_back(0.0f); // x
    ver_arr.push_back(0.0f); // y
    ver_arr.push_back(0.0f); // z
    ver_arr.push_back(1.0f); // r (color)
    ver_arr.push_back(1.0f); // g (color)
    ver_arr.push_back(1.0f); // b (color)
    ver_arr.push_back(0.5f); // u (texture)
    ver_arr.push_back(0.5f); // v (texture)

    float angle = M_PI / 180.0f; // Convert degrees to radians
    float radius = r;

    for (int i = 0; i <= 360; ++i)
    {
        float x = radius * cos(i * angle);
        float y = radius * sin(i * angle);

        // Vertex position
        ver_arr.push_back(x);    // x
        ver_arr.push_back(y);    // y
        ver_arr.push_back(0.0f); // z

        // Color (you can set this to whatever color you want)
        ver_arr.push_back(1.0f); // r
        ver_arr.push_back(1.0f); // g
        ver_arr.push_back(1.0f); // b

        // Texture coordinates
        float u = (x / radius + 1.0f) / 2.0f; // Normalize x to [0, 1]
        float v = (y / radius + 1.0f) / 2.0f; // Normalize y to [0, 1]
        ver_arr.push_back(u);                 // u
        ver_arr.push_back(v);                 // v
    }
}
