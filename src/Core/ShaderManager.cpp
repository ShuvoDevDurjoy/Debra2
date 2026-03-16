#include "../../include/GraphEngine/Core/ShaderManager.hpp"
#include <iostream>

ShaderManager* ShaderManager::instance = nullptr;

ShaderManager* ShaderManager::getInstance() {
    if (!instance) {
        instance = new ShaderManager();
    }
    return instance;
}

ShaderManager::~ShaderManager() {
    cleanup();
}

Shader* ShaderManager::getShader(const std::string& vertex, const std::string& fragment) {
    std::string key = vertex + "|" + fragment;
    if (shaderCache.find(key) == shaderCache.end()) {
        std::cout << "Loading Shader: " << vertex << ", " << fragment << std::endl;
        shaderCache[key] = new Shader(vertex, fragment);
    }
    return shaderCache[key];
}

Shader* ShaderManager::getShader(const std::string& vertex, const std::string& geometry, const std::string& fragment) {
    std::string key = vertex + "|" + geometry + "|" + fragment;
    if (shaderCache.find(key) == shaderCache.end()) {
        std::cout << "Loading Shader: " << vertex << ", " << geometry << ", " << fragment << std::endl;
        shaderCache[key] = new Shader(vertex, geometry, fragment);
    }
    return shaderCache[key];
}

void ShaderManager::cleanup() {
    for (auto& pair : shaderCache) {
        delete pair.second;
    }
    shaderCache.clear();
}
