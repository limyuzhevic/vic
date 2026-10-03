#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <memory>
#include <functional>
#include <typeindex>
#include <mutex>
#include <atomic>
#include <dlfcn.h>
#include <filesystem>
#include "IBrainPlugin.hpp"

namespace nlm {

class Brain;

// Plugin factory function type
using PluginFactory = std::function<std::unique_ptr<IBrainPlugin>()>;

// Plugin registration interface
struct IPluginRegistration {
    virtual ~IPluginRegistration() = default;
    virtual PluginMetadata getMetadata() const = 0;
    virtual PluginFactory getFactory() = 0;
};

// Dynamic plugin loader
class DynamicPluginLoader {
public:
    DynamicPluginLoader(const std::string& libraryPath);
    ~DynamicPluginLoader();

    bool load();
    void unload();
    bool isLoaded() const { return loaded; }

    void* getSymbol(const std::string& symbolName);
    std::string getLibraryPath() const { return libraryPath; }

private:
    std::string libraryPath;
    void* handle;
    bool loaded;
};

// Plugin discovery utility
class PluginDiscovery {
public:
    static std::vector<std::string> discoverPlugins(const std::string& directory);
    static std::vector<std::string> discoverBuiltInPlugins();
};

// Plugin registry for managing discovered plugins
class PluginRegistry {
public:
    PluginRegistry();
    ~PluginRegistry();

    // Register a plugin factory
    void registerPlugin(std::unique_ptr<IPluginRegistration> registration);

    // Discover and load plugins from directory
    bool loadPluginsFromDirectory(const std::string& directory);

    // Get plugin metadata by name
    PluginMetadata getPluginMetadata(const std::string& pluginName) const;

    // Check if plugin is available
    bool isPluginAvailable(const std::string& pluginName) const;

    // Get all available plugin names
    std::vector<std::string> getAvailablePlugins() const;

    // Create plugin instance
    std::unique_ptr<IBrainPlugin> createPlugin(const std::string& pluginName);

    // Get all plugins in a category
    std::vector<std::string> getPluginsInCategory(const std::string& category) const;

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;

    // Prevent copying
    PluginRegistry(const PluginRegistry&) = delete;
    PluginRegistry& operator=(const PluginRegistry&) = delete;
};

// Core plugin system manager
class PluginSystem {
public:
    PluginSystem();
    ~PluginSystem();

    // Initialize plugin system with brain reference
    bool initialize(Brain* brain, std::shared_ptr<const Config> config);
    void update(SimulationStep step);
    void shutdown();

    // Plugin management
    bool loadPlugin(const std::string& pluginName);
    bool unloadPlugin(const std::string& pluginName);
    bool isPluginLoaded(const std::string& pluginName) const;

    // Configuration forwarding
    bool configurePlugin(const std::string& pluginName, const std::string& key, const std::any& value);
    std::any getPluginConfig(const std::string& pluginName, const std::string& key) const;

    // Plugin queries
    PluginMetadata getPluginMetadata(const std::string& pluginName) const;
    std::vector<std::string> getLoadedPlugins() const;
    std::vector<std::string> getAvailablePlugins() const;

    // Event system
    void publishEvent(const std::string& eventType, const std::any& eventData);

    // Statistics and debugging
    size_t getActivePluginCount() const;
    float getAveragePluginLoadTime() const;

    // Singleton access
    static PluginSystem& getInstance();

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;

    // Prevent copying
    PluginSystem(const PluginSystem&) = delete;
    PluginSystem& operator=(const PluginSystem&) = delete;
};

} // namespace nlm

// Plugin registration macro for easy plugin registration
#define REGISTER_PLUGIN(PluginClass) \\
namespace { \\
    class PluginRegistration_##PluginClass : public nlm::IPluginRegistration { \\
    public: \\
        PluginRegistration_##PluginClass() {} \\
        nlm::PluginMetadata getMetadata() const override { \\
            return PluginClass::getStaticMetadata(); \\
        } \\
        nlm::PluginFactory getFactory() override { \\
            return []() { return std::make_unique<PluginClass>(); }; \\
        } \\
    }; \\
    \\
    extern "C" { \\
        // Export plugin registration \\
        nlm::IPluginRegistration* getPluginRegistration() { \\
            static PluginRegistration_##PluginClass reg; \\
            return &reg; \\
        } \\
    } \\
} // namespace

// Macro to define plugin metadata
#define PLUGIN_METADATA(name, version, description, author, category, isCore) \\
    nlm::PluginMetadata nlm::PluginClass::getStaticMetadata() { \\
        nlm::PluginMetadata metadata; \\
        metadata.name = name; \\
        metadata.version = version; \\
        metadata.description = description; \\
        metadata.author = author; \\
        metadata.category = category; \\
        metadata.isCore = isCore; \\
        return metadata; \\
    }