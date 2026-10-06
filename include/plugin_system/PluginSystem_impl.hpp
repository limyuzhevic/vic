#pragma once

#include "PluginSystem.hpp"

namespace nlm {

class PluginSystem {
public:
    explicit PluginSystem(std::shared_ptr<Config> config);
    ~PluginSystem();
    
    // Delete copy and move operations
    PluginSystem(const PluginSystem&) = delete;
    PluginSystem& operator=(const PluginSystem&) = delete;
    PluginSystem(PluginSystem&&) = delete;
    PluginSystem& operator=(PluginSystem&&) = delete;
    
    bool initialize();
    bool loadPlugin(const std::string& pluginPath);
    bool unloadPlugin(const std::string& pluginName);
    bool enablePlugin(const std::string& pluginName);
    bool disablePlugin(const std::string& pluginName);
    bool isPluginLoaded(const std::string& pluginName) const;
    bool isPluginEnabled(const std::string& pluginName) const;
    std::vector<std::string> getLoadedPlugins() const;
    std::vector<std::string> getEnabledPlugins() const;
    std::vector<ServiceInfo> getAvailableServices() const;
    
    template<typename T>
    std::shared_ptr<T> getService(const std::string& serviceName);
    
    template<typename T>
    bool callService(const std::string& serviceName, const T& request, T& response);
    
    bool executePlugin(const std::string& pluginName, const ExecutionContext& context);
    bool generatePluginTemplate(const std::string& templateType, const std::string& outputPath);
    void setBrainReference(Brain* brain);
    
    void addLoadListener(const PluginLoadListener& listener);
    void addEventListener(const PluginEventListener& listener);
    
    void logStatus() const;
    
private:
    bool validatePluginSignature(const std::string& pluginPath);
    bool loadPluginLibrary(const std::string& pluginPath, void*& handle);
    void* getPluginSymbol(void* handle, const std::string& symbolName);
    bool cleanupPluginResources(const std::string& pluginName);
    void updatePluginState(const std::string& pluginName, PluginState newState);
    void broadcastPluginEvent(const PluginEvent& event);
    
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
