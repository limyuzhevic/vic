#pragma once

#include <string>
#include <memory>
#include <vector>
#include <unordered_map>
#include <functional>
#include <any>
#include "../core/Types/Types.hpp"
#include "../brain/Brain.hpp"

namespace nlm {

// Plugin interface base class
class IPlugin {
public:
    virtual ~IPlugin() = default;
    
    // Get plugin name
    virtual const std::string& getName() const = 0;
    
    // Get plugin version
    virtual const std::string& getVersion() const = 0;
    
    // Get plugin description
    virtual const std::string& getDescription() const = 0;
    
    // Get plugin author
    virtual const std::string& getAuthor() const = 0;
    
    // Check if plugin is enabled
    virtual bool isEnabled() const = 0;
    
    // Enable/disable plugin
    virtual void setEnabled(bool enabled) = 0;
    
    // Initialize plugin with brain reference
    virtual bool initialize(Brain* brain) = 0;
    
    // Shutdown plugin
    virtual void shutdown() = 0;
    
    // Get plugin capabilities
    virtual const std::vector<std::string>& getCapabilities() const = 0;
    
    // Check if plugin has specific capability
    virtual bool hasCapability(const std::string& capability) const = 0;
    
    // Get configuration options
    virtual const std::unordered_map<std::string, std::any>& getConfigOptions() const = 0;
    
    // Set configuration option
    virtual bool setConfigOption(const std::string& key, const std::any& value) = 0;
    
    // Get configuration option
    virtual std::any getConfigOption(const std::string& key) const = 0;
    
    // Plugin-specific update method
    virtual void update(double dt) = 0;
    
    // Plugin-specific event handler
    virtual bool handleEvent(const std::string& eventType, const std::any& eventData) = 0;
};

// Plugin manifest for plugin metadata
struct PluginManifest {
    std::string name;
    std::string version;
    std::string description;
    std::string author;
    std::string email;
    std::string website;
    std::string license;
    std::string platform;
    std::vector<std::string> dependencies;
    std::vector<std::string> capabilities;
    std::unordered_map<std::string, std::any> defaultConfig;
    bool isBuiltIn;
    bool isThirdParty;
};

// Plugin factory interface
class IPluginFactory {
public:
    virtual ~IPluginFactory() = default;
    
    virtual std::unique_ptr<IPlugin> createPlugin() = 0;
    virtual PluginManifest getManifest() const = 0;
    virtual bool canCreatePlugin() const = 0;
};

// Plugin loader interface
class IPluginLoader {
public:
    virtual ~IPluginLoader() = default;
    
    // Load plugin from file
    virtual std::unique_ptr<IPlugin> loadPlugin(const std::string& path) = 0;
    
    // Load plugin from memory
    virtual std::unique_ptr<IPlugin> loadPluginFromMemory(const std::vector<uint8_t>& data) = 0;
    
    // Get list of available plugins
    virtual std::vector<std::string> getAvailablePlugins() const = 0;
    
    // Check if plugin can be loaded
    virtual bool canLoadPlugin(const std::string& path) const = 0;
};

// Plugin manager for managing plugins
class PluginManager {
public:
    PluginManager();
    ~PluginManager();
    
    // Disable copying
    PluginManager(const PluginManager&) = delete;
    PluginManager& operator=(const PluginManager&) = delete;
    
    // Enable moving
    PluginManager(PluginManager&&) noexcept;
    PluginManager& operator=(PluginManager&&) noexcept;
    
    // Add plugin factory
    void addPluginFactory(std::unique_ptr<IPluginFactory> factory);
    
    // Remove plugin factory
    void removePluginFactory(const std::string& pluginName);
    
    // Get all plugin names
    std::vector<std::string> getPluginNames() const;
    
    // Get plugin by name
    IPlugin* getPlugin(const std::string& name);
    const IPlugin* getPlugin(const std::string& name) const;
    
    // Create plugin instance
    bool createPlugin(const std::string& name);
    
    // Destroy plugin instance
    void destroyPlugin(const std::string& name);
    
    // Initialize all plugins
    bool initializeAll(Brain* brain);
    
    // Shutdown all plugins
    void shutdownAll();
    
    // Update all plugins
    void updateAll(double dt);
    
    // Handle event for all plugins
    bool handleEvent(const std::string& eventType, const std::any& eventData);
    
    // Get plugin configuration
    const std::any getPluginConfig(const std::string& pluginName, const std::string& key) const;
    
    // Set plugin configuration
    bool setPluginConfig(const std::string& pluginName, const std::string& key, const std::any& value);
    
    // Check if plugin is enabled
    bool isPluginEnabled(const std::string& name) const;
    
    // Enable/disable plugin
    void setPluginEnabled(const std::string& name, bool enabled);
    
    // Get plugin capabilities
    const std::vector<std::string>& getPluginCapabilities(const std::string& name) const;
    
    // Check if plugin has capability
    bool pluginHasCapability(const std::string& pluginName, const std::string& capability) const;
    
    // Get all plugins with specific capability
    std::vector<std::string> getPluginsWithCapability(const std::string& capability) const;
    
    // Save plugin configuration to file
    bool savePluginConfig(const std::string& filename) const;
    
    // Load plugin configuration from file
    bool loadPluginConfig(const std::string& filename);
    
    // Get plugin manifest
    PluginManifest getPluginManifest(const std::string& name) const;
    
    // Get all plugin manifests
    std::vector<PluginManifest> getAllPluginManifests() const;
    
    // Install plugin from file
    bool installPlugin(const std::string& path, const std::string& targetDir = "");
    
    // Uninstall plugin
    bool uninstallPlugin(const std::string& name);
    
    // Check if plugin is installed
    bool isPluginInstalled(const std::string& name) const;
    
    // Get plugin installation directory
    std::string getPluginInstallDirectory(const std::string& name) const;
    
    // Get plugin cache directory
    static std::string getPluginCacheDirectory();
    
    // Clear plugin cache
    static void clearPluginCache();
    
    // Get plugin manager statistics
    struct PluginStats {
        size_t totalPlugins;
        size_t enabledPlugins;
        size_t disabledPlugins;
        size_t builtInPlugins;
        size_t thirdPartyPlugins;
        size_t totalUpdates;
        double averageUpdateTime;
    };
    
    PluginStats getStats() const;
    
    void resetStats() {
        stats_ = PluginStats{};
    }
    
    // Export plugin as package
    bool exportPlugin(const std::string& pluginName, const std::string& outputPath);
    
    // Import plugin from package
    bool importPlugin(const std::string& packagePath);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    PluginStats stats_;
    
    // Plugin registration and discovery
    std::unordered_map<std::string, std::unique_ptr<IPluginFactory>> pluginFactories_;
    std::unordered_map<std::string, std::unique_ptr<IPlugin>> pluginInstances_;
    std::unordered_map<std::string, PluginManifest> pluginManifests_;
    
    // Plugin configuration
    std::unordered_map<std::string, std::unordered_map<std::string, std::any>> pluginConfigs_;
    
    // Brain reference for plugin initialization
    Brain* brain_;
    
    // Event system
    std::unordered_map<std::string, std::vector<std::function<bool(const std::any&)>>> eventHandlers_;
};

// Built-in plugin factories
namespace BuiltInPlugins {
    class MemoryPluginFactory : public IPluginFactory {
    public:
        std::unique_ptr<IPlugin> createPlugin() override;
        PluginManifest getManifest() const override;
        bool canCreatePlugin() const override;
    };
    
    class PredictionPluginFactory : public IPluginFactory {
    public:
        std::unique_ptr<IPlugin> createPlugin() override;
        PluginManifest getManifest() const override;
        bool canCreatePlugin() const override;
    };
    
    class CognitivePluginFactory : public IPluginFactory {
    public:
        std::unique_ptr<IPlugin> createPlugin() override;
        PluginManifest getManifest() const override;
        bool canCreatePlugin() const override;
    };
    
    class NeuromodulationPluginFactory : public IPluginFactory {
    public:
        std::unique_ptr<IPlugin> createPlugin() override;
        PluginManifest getManifest() const override;
        bool canCreatePlugin() const override;
    };
}

// Plugin utilities
namespace PluginUtils {
    // Get default plugin directories
    std::vector<std::string> getDefaultPluginDirectories();
    
    // Get system plugin directory
    static std::string getSystemPluginDirectory() {
        return "/usr/local/share/nlm/plugins";
    }
    
    static std::string getUserPluginDirectory() {
        return "~/.config/nlm/plugins";
    }
    
    static std::string getTempPluginDirectory() {
        return "/tmp/nlm-plugins";
    }
    
    // Plugin versioning utilities
    bool isVersionCompatible(const std::string& pluginVersion, const std::string& requiredVersion);
    
    // Plugin dependency resolution
    bool resolvePluginDependencies(const PluginManifest& plugin, 
                                  const std::unordered_map<std::string, PluginManifest>& availablePlugins);
    
    // Plugin configuration validation
    bool validatePluginConfig(const PluginManifest& manifest, 
                            const std::unordered_map<std::string, std::any>& config);
    
    // Plugin hash and verification
    std::string generatePluginHash(const std::vector<uint8_t>& data);
    bool verifyPluginSignature(const std::vector<uint8_t>& data, const std::string& signature);
}

} // namespace nlm
