/*
 * NLM Plugin System - Core Architecture
 * Provides registry, plugin management, and factory patterns for extensible neural system
 */

#pragma once

#include <string>
#include <memory>
#include <vector>
#include <map>
#include <unordered_map>
#include <functional>
#include <typeindex>
#include <any>
#include <stdexcept>

namespace nlm {

// Plugin Version
struct PluginVersion {
    int major;
    int minor;
    int patch;
    
    PluginVersion(int major = 0, int minor = 0, int patch = 0)
        : major(major), minor(minor), patch(patch) {}
    
    bool operator==(const PluginVersion& other) const {
        return major == other.major && minor == other.minor && patch == other.patch;
    }
    
    bool operator!=(const PluginVersion& other) const {
        return !(*this == other);
    }
};

// Plugin Metadata
struct PluginMetadata {
    std::string name;
    std::string description;
    std::string author;
    std::string version;
    PluginVersion versionInfo;
    std::string category;
    std::vector<std::string> dependencies;
    bool enabled = true;
    std::map<std::string, std::string> metadata;
    
    PluginMetadata() = default;
    PluginMetadata(const std::string& name, const std::string& desc, 
                   const std::string& author, const std::string& version)
        : name(name), description(desc), author(author), version(version),
          enabled(true) {}
};

// Plugin Status
enum class PluginStatus {
    Unknown,
    Loading,
    Loaded,
    Initializing,
    Ready,
    Error,
    Disabled,
    Unloaded
};

// Plugin Interface (Base for all plugins)
class IPlugin {
public:
    virtual ~IPlugin() = default;
    
    virtual const PluginMetadata& getMetadata() const = 0;
    virtual PluginStatus getStatus() const = 0;
    
    virtual bool load() = 0;
    virtual bool unload() = 0;
    virtual bool initialize() = 0;
    virtual void shutdown() = 0;
    
    virtual bool isLoaded() const { return getStatus() == PluginStatus::Loaded; }
    virtual bool isInitialized() const { return getStatus() == PluginStatus::Ready; }
    
    // Plugin-specific configuration
    virtual void configure(const std::map<std::string, std::any>& config) {}
    
    // Get extension points provided by this plugin
    virtual std::vector<std::string> getExtensionPoints() const { return {}; }
    
    // Get services provided by this plugin
    virtual std::map<std::string, std::any> getServices() const { return {}; }
};

// Extension Point Interface
class IExtensionPoint {
public:
    virtual ~IExtensionPoint() = default;
    virtual std::string getName() const = 0;
    virtual std::type_index getExtensionType() const = 0;
    virtual bool isOptional() const = 0;
};

// Extension Point Implementation
class ExtensionPoint : public IExtensionPoint {
public:
    ExtensionPoint(const std::string& name, const std::type_index& type, 
                   bool optional = false)
        : name_(name), extensionType_(type), optional_(optional) {}
    
    std::string getName() const override { return name_; }
    std::type_index getExtensionType() const override { return extensionType_; }
    bool isOptional() const override { return optional_; }
    
private:
    std::string name_;
    std::type_index extensionType_;
    bool optional_;
};

// Extension Interface
class IExtension {
public:
    virtual ~IExtension() = default;
    virtual std::string getExtensionPoint() const = 0;
    virtual std::any getExtension() = 0;
};

// Extension Implementation
class Extension : public IExtension {
public:
    Extension(const std::string& extensionPoint, std::any extension)
        : extensionPoint_(extensionPoint), extension_(extension) {}
    
    std::string getExtensionPoint() const override { return extensionPoint_; }
    std::any getExtension() override { return extension_; }
    
private:
    std::string extensionPoint_;
    std::any extension_;
};

// Plugin Registry - Manages all plugins
class IPluginRegistry {
public:
    virtual ~IPluginRegistry() = default;
    
    // Plugin lifecycle management
    virtual bool loadPlugin(const std::string& pluginName) = 0;
    virtual bool unloadPlugin(const std::string& pluginName) = 0;
    virtual bool enablePlugin(const std::string& pluginName) = 0;
    virtual bool disablePlugin(const std::string& pluginName) = 0;
    
    // Plugin discovery and registration
    virtual bool registerPlugin(std::shared_ptr<IPlugin> plugin) = 0;
    virtual bool unregisterPlugin(const std::string& pluginName) = 0;
    
    // Query plugins
    virtual std::vector<std::string> getPluginNames() const = 0;
    virtual std::vector<std::string> getPluginNames(const std::string& category) const = 0;
    virtual PluginMetadata getPluginMetadata(const std::string& pluginName) const = 0;
    virtual PluginStatus getPluginStatus(const std::string& pluginName) const = 0;
    
    // Extension management
    virtual bool registerExtension(const std::string& extensionPointName, std::shared_ptr<IExtension> extension) = 0;
    virtual std::vector<std::shared_ptr<IExtension>> getExtensions(const std::string& extensionPointName) const = 0;
    virtual std::vector<std::string> getExtensionPoints() const = 0;
    
    // Service management
    virtual bool registerService(const std::string& serviceName, std::any service) = 0;
    virtual std::any getService(const std::string& serviceName) = 0;
    virtual bool hasService(const std::string& serviceName) const = 0;
    
    // Plugin factories
    virtual bool registerFactory(const std::string& pluginName, std::function<std::shared_ptr<IPlugin>()> factory) = 0;
    virtual std::shared_ptr<IPlugin> createPlugin(const std::string& pluginName) = 0;
};

// Abstract factory for plugin creation
class IPluginFactory {
public:
    virtual ~IPluginFactory() = default;
    virtual std::shared_ptr<IPlugin> createPlugin() = 0;
    virtual PluginMetadata getMetadata() const = 0;
};

// Default implementation of the registry
class PluginRegistry : public IPluginRegistry {
public:
    PluginRegistry();
    ~PluginRegistry() override;
    
    // Plugin lifecycle management
    bool loadPlugin(const std::string& pluginName) override;
    bool unloadPlugin(const std::string& pluginName) override;
    bool enablePlugin(const std::string& pluginName) override;
    bool disablePlugin(const std::string& pluginName) override;
    
    // Plugin discovery and registration
    bool registerPlugin(std::shared_ptr<IPlugin> plugin) override;
    bool unregisterPlugin(const std::string& pluginName) override;
    
    // Query plugins
    std::vector<std::string> getPluginNames() const override;
    std::vector<std::string> getPluginNames(const std::string& category) const override;
    PluginMetadata getPluginMetadata(const std::string& pluginName) const override;
    PluginStatus getPluginStatus(const std::string& pluginName) const override;
    
    // Extension management
    bool registerExtension(const std::string& extensionPointName, std::shared_ptr<IExtension> extension) override;
    std::vector<std::shared_ptr<IExtension>> getExtensions(const std::string& extensionPointName) const override;
    std::vector<std::string> getExtensionPoints() const override;
    
    // Service management
    bool registerService(const std::string& serviceName, std::any service) override;
    std::any getService(const std::string& serviceName) override;
    bool hasService(const std::string& serviceName) const override;
    
    // Plugin factories
    bool registerFactory(const std::string& pluginName, std::function<std::shared_ptr<IPlugin>()> factory) override;
    std::shared_ptr<IPlugin> createPlugin(const std::string& pluginName) override;
    
    // Global instance access
    static PluginRegistry& getInstance();
    static void reset();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Plugin loader for dynamic plugin loading from modules
class IPluginLoader {
public:
    virtual ~IPluginLoader() = default;
    virtual bool loadFromFile(const std::string& filepath) = 0;
    virtual bool loadFromDirectory(const std::string& directory) = 0;
    virtual bool loadFromResource(const std::string& resourceName) = 0;
};

// Module interface for plugin modules
class IModule {
public:
    virtual ~IModule() = default;
    virtual const PluginMetadata& getMetadata() const = 0;
    virtual bool initialize(IPluginRegistry& registry) = 0;
    virtual void shutdown() = 0;
    
    // Get exported symbols/functions
    virtual std::map<std::string, std::any> getSymbols() const = 0;
    
    // Check if module is initialized
    virtual bool isInitialized() const { return initialized_; }
    
protected:
    bool initialized_ = false;
};

// Plugin system initialization
class PluginSystem {
public:
    PluginSystem();
    ~PluginSystem();
    
    // Initialize the plugin system
    bool initialize();
    
    // Shutdown the plugin system
    void shutdown();
    
    // Get the global registry
    IPluginRegistry& getRegistry() { return registry_; }
    
    // Load all plugins from configuration
    bool loadPluginsFromConfig(const std::string& configFile);
    
    // Save plugin configuration
    bool savePluginConfig(const std::string& configFile) const;
    
    // Get system status
    std::string getSystemStatus() const;
    
    // Global instance access
    static PluginSystem& getInstance();
    static void reset();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    IPluginRegistry& registry_;
};

} // namespace nlm