// Plugin manager implementation for plugin lifecycle management
#pragma once

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <algorithm>
#include <chrono>
#include <thread>
#include "PluginBase.hpp"
#include "PluginManifest.hpp"
#include "PluginLoader.hpp"

namespace nlm {

class PluginManager {
private:
    std::shared_ptr<PluginRegistry> registry;
    std::shared_ptr<IPluginLoader> loader;
    
    // Lifecycle tracking
    std::map<std::string, std::chrono::system_clock::time_point> pluginLoadTimes;
    std::map<std::string, std::chrono::system_clock::time_point> pluginShutdownTimes;
    std::map<std::string, std::vector<std::thread::id>> pluginThreads;
    
    // Dependency resolution
    bool resolveDependencies(const PluginManifest& manifest) const;
    std::vector<std::string> getSortedPluginNames(const std::vector<PluginManifest>& manifests) const;
    
    // Event handling
    void onPluginLoaded(const std::string& pluginName);
    void onPluginShutdown(const std::string& pluginName);
    void onPluginError(const std::string& pluginName, const std::string& error);
    
    // Configuration management
    void updatePluginConfiguration(const std::string& pluginName, const PluginConfig& config);
    
public:
    PluginManager(std::shared_ptr<IPluginLoader> loader = nullptr);
    
    // Plugin loading and management
    bool loadPlugin(const std::string& pluginPath);
    bool loadPlugin(const PluginManifest& manifest);
    
    bool unloadPlugin(const std::string& pluginName);
    bool reloadPlugin(const std::string& pluginName);
    
    // Plugin state management
    bool initializePlugin(const std::string& pluginName, const PluginConfig& config = PluginConfig{});
    void shutdownPlugin(const std::string& pluginName);
    bool isPluginLoaded(const std::string& pluginName) const;
    bool isPluginInitialized(const std::string& pluginName) const;
    
    // Plugin discovery
    std::vector<std::string> discoverPlugins(const std::string& directory) const;
    std::vector<PluginManifest> loadAllPlugins(const std::string& directory);
    
    // Plugin access
    std::shared_ptr<IPlugin> getPlugin(const std::string& pluginName) const;
    PluginManifest getPluginManifest(const std::string& pluginName) const;
    std::vector<std::string> getLoadedPlugins() const;
    std::vector<std::string> getInitializedPlugins() const;
    
    // Plugin information
    PluginCategory getPluginCategory(const std::string& pluginName) const;
    std::vector<std::string> getPluginDependencies(const std::string& pluginName) const;
    std::string getPluginVersion(const std::string& pluginName) const;
    std::string getPluginAuthor(const std::string& pluginName) const;
    std::string getPluginDescription(const std::string& pluginName) const;
    
    // Configuration management
    bool updatePluginConfig(const std::string& pluginName, const PluginConfig& config);
    PluginConfig getPluginConfig(const std::string& pluginName) const;
    
    // Batch operations
    std::vector<bool> loadPlugins(const std::vector<std::string>& pluginPaths);
    std::vector<bool> unloadPlugins(const std::vector<std::string>& pluginNames);
    std::vector<bool> initializePlugins(const std::vector<std::string>& pluginNames);
    std::vector<bool> shutdownPlugins(const std::vector<std::string>& pluginNames);
    
    // Event handling
    void publishEvent(const PluginEvent& event);
    void subscribeToPluginEvents(const std::string& pluginName, IEventSubscriber* subscriber);
    
    // Error handling
    std::string getLastError() const;
    void clearErrorCache();
    
    // Statistics and monitoring
    double getPluginUptime(const std::string& pluginName) const;
    size_t getPluginMemoryUsage(const std::string& pluginName) const;
    std::map<std::string, std::chrono::system_clock::time_point> getLoadTimes() const { return pluginLoadTimes; }
    std::map<std::string, std::chrono::system_clock::time_point> getShutdownTimes() const { return pluginShutdownTimes; }
    
    // Plugin validation
    bool validatePlugin(const PluginManifest& manifest) const;
    bool validatePluginCompatibility(const PluginManifest& manifest) const;
    
    // Cleanup
    void unloadAllPlugins();
    void shutdownAllPlugins();
};