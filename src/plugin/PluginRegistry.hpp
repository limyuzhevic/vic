#pragma once

#include <string>
#include <vector>
#include <memory>
#include <unordered_map>
#include <mutex>
#include "PluginBase.hpp"

namespace nlm {

// Plugin filter for discovery
struct PluginFilter {
    PluginType type = PluginType::UNKNOWN;
    std::string category;
    std::vector<std::string> excludeCategories;
    std::vector<std::string> requiredParameters;
    std::vector<std::string> excludeParameters;
    bool enabledOnly = false;
    int minPriority = 0;
    std::string namePattern;
};

class PluginRegistry {
public:
    PluginRegistry();
    ~PluginRegistry();
    
    // Plugin registration
    bool registerPlugin(const PluginInfo& info);
    bool unregisterPlugin(const std::string& pluginName);
    bool updatePluginInfo(const PluginInfo& info);
    
    // Plugin lookup
    PluginInfo getPluginInfo(const std::string& pluginName) const;
    std::shared_ptr<NLMPlugin> getPluginInstance(const std::string& pluginName);
    
    // Plugin discovery
    std::vector<PluginInfo> discoverPlugins() const;
    std::vector<PluginInfo> getPluginsByType(PluginType type) const;
    std::vector<PluginInfo> getPluginsByCategory(const std::string& category) const;
    
    // Plugin filtering
    std::vector<PluginInfo> filterPlugins(const PluginFilter& filter) const;
    bool matchesFilter(const PluginInfo& plugin, const PluginFilter& filter) const;
    
    // Plugin statistics
    size_t getPluginCount() const;
    size_t getPluginCount(PluginType type) const;
    double getTotalPluginMemoryUsage() const;
    
    // Plugin lifecycle management
    bool activatePlugin(const std::string& pluginName);
    bool deactivatePlugin(const std::string& pluginName);
    bool pausePlugin(const std::string& pluginName);
    bool resumePlugin(const std::string& pluginName);
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
