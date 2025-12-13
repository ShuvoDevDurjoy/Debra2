#ifndef SHADER_HPP
#define SHADER_HPP

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include "../include/glad.h"
#include "../Utils/Utils.hpp"
#include "../../glm/glm.hpp"

enum shaderTypes
{
    VERTEX_SHADER = GL_VERTEX_SHADER,
    FRAGMENT_SHADER = GL_FRAGMENT_SHADER,
    GEOMETRIC_SHADER = GL_GEOMETRY_SHADER
};

class Shader
{
private:
    GLuint ProgramID;
    /// @brief This function reads the shader file and returns its content as a string.
    /// @param shader
    /// @param shader_type
    /// @param shaderFileName
    void compileShader(GLuint &shader, GLenum shader_type, const std::string shaderFileName);
    void compileProgram(GLuint vertex, GLuint fragment);

public:
    Shader(const std::string &vertexShaderFile, const std::string &fragmentShaderFile);
    Shader(const std::string &vertexShaderFile, const std::string &geometricShader, const std::string &fragmentShaderFile);
    void use();
    GLuint getID();
    void setFloat(const std::string &, float) const;
    void setVec2(const std::string &, float, float) const;
    void setVec3(const std::string &, float, float, float) const;
    void setInt(const std::string &, int) const;
    void setBool(const std::string &, bool) const;
    void setMat4(const std::string &name, const float *value)
    {
        GLint loc = glGetUniformLocation(ProgramID, name.c_str());
        glUniformMatrix4fv(loc, 1, GL_FALSE, value);
    }

    void setMat4(const std::string &name, const glm::mat4 &mat)
    {
        GLint loc = glGetUniformLocation(ProgramID, name.c_str());
        glUniformMatrix4fv(loc, 1, GL_FALSE, &mat[0][0]);
    }
};

#endif