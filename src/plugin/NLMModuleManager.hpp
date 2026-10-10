// Module Manager Header
#pragma once

#include <string>
#include <map>
#include <memory>
#include "NLMModule.hpp"

namespace nlm {

/**
 * NLM Module Manager
 */
class NLMModuleManager {
public:
    NLMModuleManager();
    ~NLMModuleManager();
    
    /**
     * Load module from file
     */
    bool loadModule(const std::string& filePath);
    
    /**
     * Unload module
     */
    bool unloadModule(const std::string& name);
    
    /**
     * Enable module
     */
    bool enableModule(const std::string& name);
    
    /**
     * Disable module
     */
    bool disableModule(const std::string& name);
    
    /**
     * Get module
     */
    NLMModule* getModule(const std::string& name);
    
    /**
     * Get all modules
     */
    const std::map<std::string, std::unique_ptr<NLMModule>>& getAllModules() const;
    
    /**
     * Save module configuration
     */
    void saveModuleConfig(const std::string& name, const ModuleConfig& config);
    
private:
    std::map<std::string, std::unique_ptr<NLMModule>> _modules;
};

} // namespace nlm