// NLM Module Manager Implementation
#include "NLMModuleManager.hpp"
#include <fstream>
#include <sstream>
#include <iostream>

namespace nlm {

NLMModuleManager::NLMModuleManager() {
    NLM_LOG_INFO("Initializing NLM Module Manager");
}

NLMModuleManager::~NLMModuleManager() {
    NLM_LOG_INFO("Shutting down NLM Module Manager");
}

bool NLMModuleManager::loadModule(const std::string& filePath) {
    NLM_LOG_INFO("Loading module: " + filePath);
    
    // In a real implementation, this would load the module from a shared library
    // For simplicity, we'll just create a dummy module
    std::string moduleName = "Module_" + filePath;
    
    auto module = std::make_unique<NLMModule>(moduleName, "core", "1.0.0");
    
    if (module->initialize()) {
        _modules[moduleName] = std::move(module);
        NLM_LOG_INFO("Module loaded successfully: " + moduleName);
        return true;
    }
    
    NLM_LOG_ERROR("Failed to load module: " + filePath);
    return false;
}

bool NLMModuleManager::unloadModule(const std::string& name) {
    NLM_LOG_INFO("Unloading module: " + name);
    
    auto it = _modules.find(name);
    if (it == _modules.end()) {
        NLM_LOG_ERROR("Module not found: " + name);
        return false;
    }
    
    it->second->shutdown();
    _modules.erase(it);
    
    NLM_LOG_INFO("Module unloaded successfully: " + name);
    return true;
}

bool NLMModuleManager::enableModule(const std::string& name) {
    NLM_LOG_INFO("Enabling module: " + name);
    
    auto it = _modules.find(name);
    if (it == _modules.end()) {
        NLM_LOG_ERROR("Module not found: " + name);
        return false;
    }
    
    if (it->second->isEnabled()) {
        NLM_LOG_INFO("Module already enabled: " + name);
        return true;
    }
    
    if (it->second->enable()) {
        NLM_LOG_INFO("Module enabled successfully: " + name);
        return true;
    }
    
    NLM_LOG_ERROR("Failed to enable module: " + name);
    return false;
}

bool NLMModuleManager::disableModule(const std::string& name) {
    NLM_LOG_INFO("Disabling module: " + name);
    
    auto it = _modules.find(name);
    if (it == _modules.end()) {
        NLM_LOG_ERROR("Module not found: " + name);
        return false;
    }
    
    if (!it->second->isEnabled()) {
        NLM_LOG_INFO("Module not enabled: " + name);
        return true;
    }
    
    it->second->disable();
    
    NLM_LOG_INFO("Module disabled successfully: " + name);
    return true;
}

NLMModule* NLMModuleManager::getModule(const std::string& name) {
    auto it = _modules.find(name);
    if (it != _modules.end()) {
        return it->second.get();
    }
    return nullptr;
}

const std::map<std::string, std::unique_ptr<NLMModule>>& NLMModuleManager::getAllModules() const {
    return _modules;
}

void NLMModuleManager::saveModuleConfig(const std::string& name, const ModuleConfig& config) {
    // In a real implementation, this would save the module configuration
    NLM_LOG_INFO("Module config saved for: " + name);
}

} // namespace nlm