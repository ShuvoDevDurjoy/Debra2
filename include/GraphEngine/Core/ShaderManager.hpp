#ifndef __SHADER_MANAGER_HPP__
#define __SHADER_MANAGER_HPP__

#include <string>
#include <unordered_map>
#include <GraphEngine/Core/Shader.hpp>

class ShaderManager {
private:
    static ShaderManager* instance;
    std::unordered_map<std::string, Shader*> shaderCache;

    ShaderManager() {}
    ~ShaderManager();

public:
    static ShaderManager* getInstance();
    
    Shader* getShader(const std::string& vertex, const std::string& fragment);
    Shader* getShader(const std::string& vertex, const std::string& geometry, const std::string& fragment);
    
    void cleanup();
};

#endif
