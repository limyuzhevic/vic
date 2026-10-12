#pragma once

#include <memory>
#include <string>
#include <vector>
#include <unordered_map>
#include <functional>
#include <type_traits>
#include <algorithm>
#include <stdexcept>

namespace nlm {

// Plugin system for NLM extensibility
namespace PluginSystem {

// Plugin metadata for discovery and loading
struct PluginMetadata {
    std::string name;
    std::string version;
    std::string description;
    std::string author;
    std::string license;
    std::vector<std::string> dependencies;
    std::vector<std::string> tags;
    std::string pluginType;
    bool isEnabled;
    std::string filePath;
    
    PluginMetadata()
        : isEnabled(true), version("1.0.0") {}
};

// Plugin interface - base class for all plugins
class PluginInterface {
public:
    virtual ~PluginInterface() = default;
    
    // Plugin identification
    virtual std::string getName() const = 0;
    virtual std::string getVersion() const = 0;
    virtual std::string getDescription() const = 0;
    
    // Plugin lifecycle
    virtual bool initialize() = 0;
    virtual bool shutdown() = 0;
    virtual bool isInitialized() const = 0;
    
    // Plugin functionality
    virtual void update(float deltaTime) = 0;
    virtual void render() = 0;
    
    // Configuration
    virtual bool loadConfiguration(const std::string& config) = 0;
    virtual std::string getConfiguration() const = 0;
    
    // Event handling
    virtual void onEvent(const std::string& eventType, const std::string& eventData) = 0;
    
    // Extension points
    virtual std::vector<std::string> getExtensionPoints() const = 0;
    virtual bool provideExtension(const std::string& extensionPoint, const std::string& extensionData) = 0;
};

// Plugin factory interface for creating plugins
class PluginFactory {
public:
    virtual ~PluginFactory() = default;
    virtual std::unique_ptr<PluginInterface> createPlugin() = 0;
    virtual PluginMetadata getMetadata() const = 0;
};

// Plugin loader for dynamic loading
class PluginLoader {
public:
    PluginLoader() = default;
    virtual ~PluginLoader() = default;
    
    // Load plugin from file
    virtual bool loadPlugin(const std::string& filePath) = 0;
    
    // Unload plugin
    virtual bool unloadPlugin(const std::string& pluginName) = 0;
    
    // Get loaded plugin
    virtual std::shared_ptr<PluginInterface> getPlugin(const std::string& pluginName) = 0;
    
    // List all loaded plugins
    virtual std::vector<std::string> getLoadedPlugins() const = 0;
    
    // Check if plugin is loaded
    virtual bool isPluginLoaded(const std::string& pluginName) const = 0;
};

// Plugin manager for coordinating plugin lifecycle
class PluginManager {
public:
    PluginManager() = default;
    ~PluginManager() {
        shutdownAllPlugins();
    }
    
    // Register plugin factory
    void registerPluginFactory(std::unique_ptr<PluginFactory> factory) {
        auto metadata = factory->getMetadata();
        pluginFactories_[metadata.name] = std::move(factory);
    }
    
    // Load plugin by name
    bool loadPlugin(const std::string& pluginName) {
        auto it = pluginFactories_.find(pluginName);
        if (it == pluginFactories_.end()) {
            return false;
        }
        
        auto factory = it->second.get();
        auto plugin = factory->createPlugin();
        
        if (!plugin) {
            return false;
        }
        
        // Initialize plugin
        if (!plugin->initialize()) {
            return false;
        }
        
        // Store plugin
        plugins_[pluginName] = std::move(plugin);
        
        // Notify other plugins
        onPluginLoaded(pluginName);
        
        return true;
    }
    
    // Unload plugin
    bool unloadPlugin(const std::string& pluginName) {
        auto it = plugins_.find(pluginName);
        if (it == plugins_.end()) {
            return false;
        }
        
        // Shutdown plugin
        it->second->shutdown();
        
        // Remove plugin
        plugins_.erase(it);
        
        // Notify other plugins
        onPluginUnloaded(pluginName);
        
        return true;
    }
    
    // Get plugin
    std::shared_ptr<PluginInterface> getPlugin(const std::string& pluginName) {
        auto it = plugins_.find(pluginName);
        if (it != plugins_.end()) {
            return it->second;
        }
        return nullptr;
    }
    
    // Update all plugins
    void updateAllPlugins(float deltaTime) {
        for (auto& [name, plugin] : plugins_) {
            if (plugin->isInitialized()) {
                try {
                    plugin->update(deltaTime);
                } catch (const std::exception& e) {
                    // Log error
                }
            }
        }
    }
    
    // Get all plugin names
    std::vector<std::string> getAllPluginNames() const {
        std::vector<std::string> names;
        names.reserve(plugins_.size());
        for (const auto& pair : plugins_) {
            names.push_back(pair.first);
        }
        return names;
    }
    
    // Get plugin metadata
    PluginMetadata getPluginMetadata(const std::string& pluginName) const {
        auto it = pluginFactories_.find(pluginName);
        if (it != pluginFactories_.end()) {
            return it->second->getMetadata();
        }
        return PluginMetadata();
    }
    
    // Get all available plugin factories
    std::vector<std::string> getAvailablePlugins() const {
        std::vector<std::string> names;
        names.reserve(pluginFactories_.size());
        for (const auto& pair : pluginFactories_) {
            names.push_back(pair.first);
        }
        return names;
    }
    
    // Shutdown all plugins
    void shutdownAllPlugins() {
        for (auto& [name, plugin] : plugins_) {
            if (plugin->isInitialized()) {
                plugin->shutdown();
            }
        }
        plugins_.clear();
    }
    
    // Check if plugin is loaded
    bool isPluginLoaded(const std::string& pluginName) const {
        return plugins_.find(pluginName) != plugins_.end();
    }

protected:
    virtual void onPluginLoaded(const std::string& pluginName) {
        // Notify other plugins
        for (auto& [name, plugin] : plugins_) {
            if (plugin->isInitialized()) {
                plugin->onEvent("plugin_loaded", pluginName);
            }
        }
    }
    
    virtual void onPluginUnloaded(const std::string& pluginName) {
        // Notify other plugins
        for (auto& [name, plugin] : plugins_) {
            if (plugin->isInitialized()) {
                plugin->onEvent("plugin_unloaded", pluginName);
            }
        }
    }

private:
    std::unordered_map<std::string, std::unique_ptr<PluginFactory>> pluginFactories_;
    std::unordered_map<std::string, std::shared_ptr<PluginInterface>> plugins_;
};

// Template for creating plugins
template<typename T>
class PluginTemplate : public PluginInterface {
public:
    PluginTemplate(const std::string& name, const std::string& version, const std::string& description)
        : name_(name), version_(version), description_(description), initialized_(false) {}
    
    virtual std::string getName() const override { return name_; }
    virtual std::string getVersion() const override { return version_; }
    virtual std::string getDescription() const override { return description_; }
    
    virtual bool initialize() override {
        initialized_ = onInitialize();
        return initialized_;
    }
    
    virtual bool shutdown() override {
        bool success = onShutdown();
        initialized_ = false;
        return success;
    }
    
    virtual bool isInitialized() const override { return initialized_; }
    
    virtual void update(float deltaTime) override {
        onUpdate(deltaTime);
    }
    
    virtual void render() override {
        onRender();
    }
    
    virtual bool loadConfiguration(const std::string& config) override {
        return onLoadConfiguration(config);
    }
    
    virtual std::string getConfiguration() const override {
        return onGetConfiguration();
    }
    
    virtual void onEvent(const std::string& eventType, const std::string& eventData) override {
        onPluginEvent(eventType, eventData);
    }
    
    virtual std::vector<std::string> getExtensionPoints() const override {
        return onGetExtensionPoints();
    }
    
    virtual bool provideExtension(const std::string& extensionPoint, const std::string& extensionData) override {
        return onProvideExtension(extensionPoint, extensionData);
    }

protected:
    virtual bool onInitialize() = 0;
    virtual bool onShutdown() = 0;
    virtual void onUpdate(float deltaTime) = 0;
    virtual void onRender() = 0;
    virtual bool onLoadConfiguration(const std::string& config) = 0;
    virtual std::string onGetConfiguration() const = 0;
    virtual void onPluginEvent(const std::string& eventType, const std::string& eventData) = 0;
    virtual std::vector<std::string> onGetExtensionPoints() const = 0;
    virtual bool onProvideExtension(const std::string& extensionPoint, const std::string& extensionData) = 0;

private:
    std::string name_;
    std::string version_;
    std::string description_;
    bool initialized_;
};

// Plugin registry for managing plugin factories
class PluginRegistry {
public:
    static PluginRegistry& getInstance() {
        static PluginRegistry instance;
        return instance;
    }
    
    // Register a plugin factory
    void registerPluginFactory(std::unique_ptr<PluginFactory> factory) {
        auto metadata = factory->getMetadata();
        pluginFactories_[metadata.name] = std::move(factory);
    }
    
    // Get plugin factory
    std::unique_ptr<PluginFactory> getPluginFactory(const std::string& pluginName) {
        auto it = pluginFactories_.find(pluginName);
        if (it != pluginFactories_.end()) {
            return std::make_unique<PluginFactory>(*it->second);
        }
        return nullptr;
    }
    
    // Get all registered plugin names
    std::vector<std::string> getRegisteredPluginNames() const {
        std::vector<std::string> names;
        names.reserve(pluginFactories_.size());
        for (const auto& pair : pluginFactories_) {
            names.push_back(pair.first);
        }
        return names;
    }
    
    // Clear all registered plugins
    void clear() {
        pluginFactories_.clear();
    }

private:
    PluginRegistry() = default;
    ~PluginRegistry() = default;
    PluginRegistry(const PluginRegistry&) = delete;
    PluginRegistry& operator=(const PluginRegistry&) = delete;
    
    std::unordered_map<std::string, std::unique_ptr<PluginFactory>> pluginFactories_;
};

// Plugin system utilities
namespace PluginUtils {

// Create a simple plugin factory
template<typename T>
class SimplePluginFactory : public PluginFactory {
public:
    SimplePluginFactory(const std::string& name, const std::string& version, const std::string& description)
        : metadata_{name, version, description, "", "", {}, {}, "", true} {}
    
    std::unique_ptr<PluginInterface> createPlugin() override {
        return std::make_unique<T>(name_, version_, description_);
    }
    
    PluginMetadata getMetadata() const override {
        return metadata_;
    }

private:
    PluginMetadata metadata_;
    std::string name_;
    std::string version_;
    std::string description_;
};

// Plugin system initialization
template<typename... Args>
void initializePluginSystem(PluginManager& manager, Args&&... args) {
    // Initialize default plugins
    // This can be extended to load plugins from configuration files
}

// Plugin system shutdown
template<typename... Args>
void shutdownPluginSystem(PluginManager& manager, Args&&... args) {
    manager.shutdownAllPlugins();
}

} // namespace PluginUtils

} // namespace PluginSystem

} // namespace nlm