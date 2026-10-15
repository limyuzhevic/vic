#pragma once

#include "IPluginManager.hpp"
#include <unordered_map>
#include <vector>
#include <string>

namespace nlm {

// Plugin registry implementation
class PluginRegistry {
public:
    PluginRegistry();
    ~PluginRegistry();
    
    // Get instance
    static PluginRegistry& getInstance();
    
    // Register plugin
    void registerPlugin(const std::string& name, const std::string& version,
                       const std::string& description, PluginFactory factory,
                       const std::string& type);
    
    // Unregister plugin
    void unregisterPlugin(const std::string& name);
    
    // Get plugin factory
    PluginFactory getPluginFactory(const std::string& name) const;
    
    // Get all registered plugin names
    std::vector<std::string> getPluginNames() const;
    
    // Get plugin info
    PluginInfo getPluginInfo(const std::string& name) const;
    
    // Check if plugin is registered
    bool isPluginRegistered(const std::string& name) const;
    
    // Clear registry
    void clear();
    
private:
    std::unordered_map<std::string, PluginFactory> pluginFactories;
    std::unordered_map<std::string, PluginInfo> pluginInfos;
};

}} // namespace nlm