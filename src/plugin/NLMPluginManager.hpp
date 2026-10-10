// Plugin Manager Header
#pragma once

#include <string>
#include <map>
#include <memory>
#include "NLMPlugin.hpp"

namespace nlm {

/**
 * NLM Plugin Manager
 */
class NLMPluginManager {
public:
    NLMPluginManager();
    ~NLMPluginManager();
    
    /**
     * Load plugin from file
     */
    bool loadPlugin(const std::string& filePath);
    
    /**
     * Unload plugin
     */
    bool unloadPlugin(const std::string& name);
    
    /**
     * Enable plugin
     */
    bool enablePlugin(const std::string& name);
    
    /**
     * Disable plugin
     */
    bool disablePlugin(const std::string& name);
    
    /**
     * Get plugin
     */
    NLMPlugin* getPlugin(const std::string& name);
    
    /**
     * Get all plugins
     */
    const std::map<std::string, std::unique_ptr<NLMPlugin>>& getAllPlugins() const;
    
    /**
     * Execute plugin extension
     */
    void executePluginExtension(const std::string& name, const std::string& extension);
    
    /**
     * Save plugin configuration
     */
    void savePluginConfig(const std::string& name, const PluginConfig& config);
    
private:
    std::map<std::string, std::unique_ptr<NLMPlugin>> _plugins;
};

} // namespace nlm