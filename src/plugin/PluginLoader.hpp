#pragma once

#include <string>
#include <vector>
#include <memory>
#include <unordered_map>
#include <mutex>
#include "PluginBase.hpp"

namespace nlm {

class PluginLoader {
public:
    PluginLoader();
    ~PluginLoader();
    
    // Dynamic loading
    std::shared_ptr<NLMPlugin> loadPlugin(const std::string& pluginPath);
    bool unloadPlugin(const std::string& pluginName);
    
    // Plugin discovery
    std::vector<std::string> discoverPlugins(const std::string& directory);
    bool validatePlugin(const std::string& pluginPath);
    
    // Plugin information
    PluginInfo getPluginInfo(const std::string& pluginPath);
    std::string getPluginSignature(const std::string& pluginPath);
    
    // Error handling
    std::string getLastError() const;
    bool hasError() const;
    void clearError();
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
