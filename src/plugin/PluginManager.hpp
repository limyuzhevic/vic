#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <memory>
#include <mutex>
#include "PluginBase.hpp"

namespace nlm {

class PluginManager {
public:
    PluginManager();
    ~PluginManager();
    
    // Plugin lifecycle management
    bool loadPlugin(const std::string& pluginPath);
    bool unloadPlugin(const std::string& pluginName);
    bool initializePlugin(const std::string& pluginName);
    bool shutdownPlugin(const std::string& pluginName);
    
    // Plugin state management
    void pauseAllPlugins();
    void resumeAllPlugins();
    void stopAllPlugins();
    
    // Plugin discovery
    std::vector<PluginInfo> discoverPlugins(const std::string& directory);
    bool validatePlugin(const std::string& pluginPath);
    
    // Plugin dependency resolution
    bool resolveDependencies(const std::string& pluginName);
    std::vector<std::string> getUnsatisfiedDependencies(const std::string& pluginName) const;
    
    // Plugin communication
    void broadcastMessage(const PluginMessage& message);
    void sendMessage(const std::string& receiver, const PluginMessage& message);
    
    // Plugin configuration
    void configurePlugin(const std::string& pluginName, const PluginConfig& config);
    PluginConfig getPluginConfig(const std::string& pluginName) const;
    
    // Plugin monitoring
    PluginStatus getPluginStatus(const std::string& pluginName) const;
    double getPluginPerformance(const std::string& pluginName) const;
    std::string getPluginError(const std::string& pluginName) const;
    
    // Plugin listing
    std::vector<PluginInfo> listPlugins() const;
    PluginInfo getPluginInfo(const std::string& pluginName) const;
    std::unordered_map<std::string, PluginInfo> getAllPluginInfos() const;
    
    // Plugin execution
    void updatePlugins(double deltaTime);
    void processPluginMessages();
    
    // Plugin statistics
    size_t getPluginCount() const;
    double getTotalMemoryUsage() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
