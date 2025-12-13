#include "../../include/GraphEngine/Core/Shader.hpp"

Shader::Shader(const std::string &vertexShaderFile, const std::string &fragmentShaderFile)
{

    // vertex and fragment shaders
    GLuint vertex, fragment;

    compileShader(vertex, shaderTypes::VERTEX_SHADER, vertexShaderFile);
    compileShader(fragment, shaderTypes::FRAGMENT_SHADER, fragmentShaderFile);


    compileProgram(vertex, fragment);

    glDeleteShader(vertex);
    glDeleteShader(fragment);
}
Shader::Shader(const std::string &vertexShaderFile, const std::string &geometricShader, const std::string &fragmentShaderFile)
{

    // vertex and fragment shaders
    GLuint vertex, geometric, fragment;

    compileShader(vertex, shaderTypes::VERTEX_SHADER, vertexShaderFile);
    compileShader(geometric, shaderTypes::GEOMETRIC_SHADER, geometricShader);
    compileShader(fragment, shaderTypes::FRAGMENT_SHADER, fragmentShaderFile);

    compileProgram(vertex, fragment);

    glDeleteShader(vertex);
    glDeleteShader(fragment);
}

void Shader::compileShader(GLuint &shader, GLenum shader_type, const std::string shaderFileName)
{

    std::string shaderContent = readFile(shaderFileName);
    
    const char *shader_code = shaderContent.c_str();
    
    shader = glCreateShader(shader_type);

    glShaderSource(shader, 1, &shader_code, NULL);

    glCompileShader(shader);

    int success;
    GLchar infoLog[512];

    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

    if (!success)
    {
        glGetShaderInfoLog(shader, sizeof(infoLog), NULL, infoLog);
        std::cout << "Error with compiling shader" << infoLog << std::endl;
    }
}

void Shader::compileProgram(GLuint vertex, GLuint fragment)
{
    ProgramID = glCreateProgram();

    glAttachShader(ProgramID, vertex);
    glAttachShader(ProgramID, fragment);

    glLinkProgram(ProgramID);

    int success;
    GLchar infoLog[512];

    glGetProgramiv(ProgramID, GL_LINK_STATUS, &success);
    if (!success)
    {
        glGetProgramInfoLog(ProgramID, sizeof(infoLog), NULL, infoLog);
        std::cout << "Error with Comiling Program " << infoLog << std::endl;
    }
}

void Shader::use()
{
    glUseProgram(ProgramID);
}

GLuint Shader::getID(){
    return ProgramID;
}

void Shader::setFloat(const std::string& name, float v) const {
    glUniform1f(glGetUniformLocation(ProgramID, name.c_str()), v);
}
void Shader::setVec2 (const std::string& name, float x, float y) const {
    glUniform2f(glGetUniformLocation(ProgramID, name.c_str()), x, y);
}
void Shader::setVec3 (const std::string &name, float x, float y, float z) const {
    glUniform3f(glGetUniformLocation(ProgramID, name.c_str()), x, y, z);
}

void Shader::setInt(const std::string& name, int v)   const {
    glUniform1i(glGetUniformLocation(ProgramID, name.c_str()), v);
}

void Shader::setBool(const std::string &name, bool value) const
{
    glUniform1i(glGetUniformLocation(ProgramID, name.c_str()), (int)value);
}
