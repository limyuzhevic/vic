// PluginSystem.hpp
#pragma once

#include "PluginManager.hpp"
#include "PluginRegistry.hpp"
#include "PluginFactory.hpp"
#include "PluginLoader.hpp"

namespace nlm {

class PluginSystem {
public:
    PluginSystem();
    ~PluginSystem();

    // Initialize the plugin system
    bool initialize();

    // Shutdown the plugin system
    void shutdown();

    // Get the plugin manager
    static PluginManager& getManager();

    // Get the plugin registry
    static PluginRegistry& getRegistry();

    // Get the plugin factory
    static PluginFactory& getFactory();

    // Get the plugin loader
    static PluginLoader& getLoader();

    // Load plugins from directory
    bool loadPluginsFromDirectory(const std::string& directory);

    // Get plugin information
    std::vector<std::string> getAvailablePlugins() const;

    // Check if plugin is available
    bool isPluginAvailable(const std::string& name) const;

    // Get plugin status
    std::string getPluginStatus(const std::string& name) const;

    // Set plugin configuration
    bool configurePlugin(const std::string& name, const Config& config);

    // Get plugin configuration
    Config getPluginConfiguration(const std::string& name) const;

    // Initialize all plugins
    bool initializeAllPlugins();

    // Enable all plugins
    bool enableAllPlugins();

    // Disable all plugins
    bool disableAllPlugins();

    // Shutdown all plugins
    bool shutdownAllPlugins();

    // Get system statistics
    size_t getTotalPluginCount() const;
    size_t getInitializedPluginCount() const;
    size_t getEnabledPluginCount() const;

    // Get last error
    std::string getLastError() const;

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm