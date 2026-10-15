#pragma once

#include <string>
#include <memory>
#include <map>
#include <vector>
#include <functional>
#include <unordered_map>
#include <mutex>
#include <atomic>
#include <filesystem>
#include "../core/Config/Config.hpp"
#include "PluginInterface.hpp"

namespace nlm {

class PluginManager {
public:
    // Plugin manager lifecycle
    PluginManager();
    ~PluginManager();
    
    // Disable copying, enable moving
    PluginManager(const PluginManager&) = delete;
    PluginManager& operator=(const PluginManager&) = delete;
    PluginManager(PluginManager&&) noexcept;
    PluginManager& operator=(PluginManager&&) noexcept;
    
    // Plugin loading and management
    bool loadPlugin(const std::string& pluginPath);
    bool loadPluginFromDirectory(const std::string& directoryPath);
    bool unloadPlugin(const std::string& pluginName);
    bool isPluginLoaded(const std::string& pluginName) const;
    
    // Plugin discovery and registration
    std::vector<std::string> getLoadedPlugins() const;
    std::vector<std::string> discoverPlugins(const std::string& directory) const;
    PluginMetadata getPluginMetadata(const std::string& pluginName) const;
    
    // Plugin interface access
    std::shared_ptr<INeuronModelPlugin> getNeuronModelPlugin(const std::string& name) const;
    std::shared_ptr<IPlasticityRulePlugin> getPlasticityRulePlugin(const std::string& name) const;
    std::shared_ptr<ISynapticDynamicsPlugin> getSynapticDynamicsPlugin(const std::string& name) const;
    std::shared_ptr<IPlugin> getPlugin(const std::string& name) const;
    
    // Plugin management
    void setConfig(const Config& config);
    void update(double timestep);
    void shutdownAllPlugins();
    
    // Dynamic plugin management
    void enablePlugin(const std::string& name);
    void disablePlugin(const std::string& name);
    bool isPluginEnabled(const std::string& name) const;
    
    // Plugin statistics
    size_t getLoadedPluginCount() const;
    std::string getPluginStats() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
