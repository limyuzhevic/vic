#pragma once

#include "PluginBase.hpp"
#include <string>
#include <memory>
#include <unordered_map>
#include <vector>
#include <functional>
#include <concepts>
#include <thread>

namespace nlm {

// Concept for plugin capabilities
template<typename T>
concept PluginCapability = requires(T plugin) {
    { plugin.getMetadata() } -> std::same_as<const PluginMetadata&>;
    { plugin.initialize() } -> std::same_as<void>;
    { plugin.shutdown() } -> std::same_as<void>;
    { plugin.getState() } -> std::same_as<PluginState>;
};

// Plugin manager with thread safety and lifecycle management
class PluginManager : public ThreadSafePluginManager {
public:
    PluginManager() = default;
    ~PluginManager() = default;

    // Disable copying
    PluginManager(const PluginManager&) = delete;
    PluginManager& operator=(const PluginManager&) = delete;

    // Plugin loading and unloading
    std::unique_ptr<PluginBase> loadPlugin(const std::string& path);
    bool unloadPlugin(const std::string& pluginId);
    bool unloadPlugin(std::unique_ptr<PluginBase> plugin);

    // Plugin discovery and registration
    bool registerPluginLoader(std::unique_ptr<PluginLoader> loader);
    std::vector<std::string> discoverPlugins(const std::string& directory) const;

    // Plugin lifecycle management
    bool initializePlugin(const std::string& pluginId);
    bool shutdownPlugin(const std::string& pluginId);
    bool isPluginLoaded(const std::string& pluginId) const;

    // Plugin access
    std::unique_ptr<PluginBase> getPlugin(const std::string& pluginId);
    std::vector<std::string> getLoadedPlugins() const;
    PluginMetadata getPluginMetadata(const std::string& pluginId) const;

    // Global plugin registry access
    static PluginManager& getInstance();
    static void setGlobalRegistry(std::unique_ptr<PluginManager> registry);

    // Utility functions
    static std::string generatePluginId(const std::string& name, const std::string& version);
    static bool validatePluginCompatibility(const PluginMetadata& metadata);

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl_;
};

} // namespace nlm
