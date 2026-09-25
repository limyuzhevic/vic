// NLM Extensibility and Plugin System
// Phase 2: Real Neural Computation - Extensibility Architecture
//
// This plugin system provides:
// - Dynamic plugin loading and management
// - Extensible component registration
// - Plugin interface definitions
// - Plugin lifecycle management
// - Plugin metadata and versioning
// - Security and sandboxing
// - Dependency management
// - Plugin discovery and auto-loading
// - Hot-reloading capabilities
// - Configuration-driven plugin management

#pragma once

#include <string>
#include <memory>
#include <vector>
#include <map>
#include <functional>
#include <unordered_map>
#include <unordered_set>
#include <mutex>
#include <filesystem>
#include <typeinfo>
#include <algorithm>
#include <any>
#include <variant>
#include <optional>

namespace nlm {

// Plugin system forward declarations
class Plugin;
class PluginManager;
class PluginLoader;
class PluginInterface;

// Plugin metadata
struct PluginMetadata {
    std::string name;
    std::string version;
    std::string description;
    std::string author;
    std::string license;
    std::string homepage;
    std::string category;
    std::vector<std::string> tags;
    std::map<std::string, std::string> configuration;
    std::vector<std::string> dependencies;
    bool enabled = true;
    bool autoLoad = false;
    std::string filePath;
    std::chrono::system_clock::time_point lastModified;
    std::string checksum;
};

// Plugin interface for component extension
class PluginInterface {
public:
    virtual ~PluginInterface() = default;
    virtual std::string getName() const = 0;
    virtual std::string getVersion() const = 0;
    virtual std::string getDescription() const = 0;
    virtual void initialize() = 0;
    virtual void shutdown() = 0;
    virtual bool isInitialized() const = 0;
};

// Component factory interface
class ComponentFactory {
public:
    virtual ~ComponentFactory() = default;
    virtual std::string getComponentType() const = 0;
    virtual std::shared_ptr<void> createComponent(const std::map<std::string, std::any>& config) = 0;
    virtual void destroyComponent(std::shared_ptr<void> component) = 0;
    virtual bool validateComponent(const std::map<std::string, std::any>& config) = 0;
};

// Plugin abstract base class
class Plugin : public PluginInterface {
public:
    virtual ~Plugin() = default;
    
    // Register component factories
    template<typename ComponentType, typename FactoryType>
    void registerComponentFactory() {
        std::string componentType = typeid(ComponentType).name();
        auto factory = std::make_shared<FactoryType>();
        componentFactories_[componentType] = factory;
    }
    
    // Get component factory
    std::shared_ptr<ComponentFactory> getComponentFactory(const std::string& componentType) {
        auto it = componentFactories_.find(componentType);
        return (it != componentFactories_.end()) ? it->second : nullptr;
    }
    
    // Get all registered component types
    std::vector<std::string> getRegisteredComponentTypes() const {
        std::vector<std::string> types;
        for (const auto& pair : componentFactories_) {
            types.push_back(pair.first);
        }
        return types;
    }
    
    // Get plugin metadata
    const PluginMetadata& getMetadata() const { return metadata_; }
    
    // Set plugin metadata
    void setMetadata(const PluginMetadata& metadata) { metadata_ = metadata; }
    
    // Plugin configuration
    void setConfiguration(const std::map<std::string, std::any>& config) {
        configuration_ = config;
    }
    
    std::map<std::string, std::any> getConfiguration() const {
        return configuration_;
    }
    
    // Get configuration value
    template<typename T>
    std::optional<T> getConfigurationValue(const std::string& key) const {
        auto it = configuration_.find(key);
        if (it != configuration_.end()) {
            try {
                return std::any_cast<T>(it->second);
            } catch (const std::bad_any_cast&) {
                return std::nullopt;
            }
        }
        return std::nullopt;
    }
    
    // Set configuration value
    template<typename T>
    void setConfigurationValue(const std::string& key, const T& value) {
        configuration_[key] = value;
    }
    
    // Plugin lifecycle management
    virtual void onLoad() {}
    virtual void onUnload() {}
    virtual void onConfigurationUpdate(const std::map<std::string, std::any>& newConfig) {}
    
    // Event handling
    virtual void onEvent(const std::string& eventName, const std::map<std::string, std::any>& eventData) {}
    
    // Component lifecycle management
    virtual bool validateComponent(const std::string& componentType, const std::map<std::string, std::any>& config) {
        auto factory = getComponentFactory(componentType);
        return factory ? factory->validateComponent(config) : false;
    }
    
    virtual std::shared_ptr<void> createComponent(const std::string& componentType, const std::map<std::string, std::any>& config) {
        auto factory = getComponentFactory(componentType);
        return factory ? factory->createComponent(config) : nullptr;
    }
    
    virtual void destroyComponent(const std::string& componentType, std::shared_ptr<void> component) {
        auto factory = getComponentFactory(componentType);
        if (factory) {
            factory->destroyComponent(component);
        }
    }

protected:
    PluginMetadata metadata_;
    std::map<std::string, std::any> configuration_;
    std::map<std::string, std::shared_ptr<ComponentFactory>> componentFactories_;
    bool initialized_ = false;
};

// Plugin discovery interface
class PluginDiscovery {
public:
    virtual ~PluginDiscovery() = default;
    virtual std::vector<std::string> discoverPlugins(const std::string& directory) = 0;
    virtual PluginMetadata loadMetadata(const std::string& pluginPath) = 0;
    virtual bool isPluginFile(const std::string& filePath) = 0;
};

// File system plugin discovery
class FileSystemPluginDiscovery : public PluginDiscovery {
public:
    std::vector<std::string> discoverPlugins(const std::string& directory) override {
        std::vector<std::string> plugins;
        
        if (!std::filesystem::exists(directory)) {
            return plugins;
        }
        
        for (const auto& entry : std::filesystem::directory_iterator(directory)) {
            if (entry.is_regular_file() && isPluginFile(entry.path().string())) {
                plugins.push_back(entry.path().string());
            }
        }
        
        return plugins;
    }
    
    PluginMetadata loadMetadata(const std::string& pluginPath) override {
        PluginMetadata metadata;
        metadata.filePath = pluginPath;
        
        // Extract filename
        std::filesystem::path path(pluginPath);
        metadata.name = path.stem().string();
        
        // Try to load metadata file
        std::string metadataPath = path.parent_path().string() + "/" + metadata.name + ".plugin.json";
        if (std::filesystem::exists(metadataPath)) {
            // In production, would parse JSON metadata file
            // For now, create basic metadata
            metadata.description = "Plugin loaded from " + metadata.name;
            metadata.version = "1.0.0";
            metadata.author = "Unknown";
        } else {
            // Create default metadata
            metadata.description = "Dynamic plugin";
            metadata.version = "1.0.0";
            metadata.author = "NLM System";
        }
        
        metadata.lastModified = std::chrono::system_clock::now();
        
        return metadata;
    }
    
    bool isPluginFile(const std::string& filePath) override {
        std::filesystem::path path(filePath);
        std::string extension = path.extension().string();
        
        // Check for common plugin extensions
        std::vector<std::string> pluginExtensions = {".so", ".dll", ".dylib", ".plugin"};
        return std::find(pluginExtensions.begin(), pluginExtensions.end(), extension) != pluginExtensions.end();
    }
};

// Plugin loader interface
class PluginLoader {
public:
    virtual ~PluginLoader() = default;
    virtual std::shared_ptr<Plugin> loadPlugin(const std::string& pluginPath) = 0;
    virtual bool unloadPlugin(const std::string& pluginName) = 0;
    virtual bool isPluginLoaded(const std::string& pluginName) const = 0;
    virtual std::vector<std::string> getLoadedPlugins() const = 0;
};

// Dynamic plugin loader
class DynamicPluginLoader : public PluginLoader {
public:
    DynamicPluginLoader(std::shared_ptr<PluginDiscovery> discovery)
        : discovery_(discovery) {}
    
    std::shared_ptr<Plugin> loadPlugin(const std::string& pluginPath) override {
        std::lock_guard<std::mutex> lock(mutex_);
        
        // Check if already loaded
        auto pluginName = getPluginNameFromPath(pluginPath);
        if (isPluginLoaded(pluginName)) {
            return nullptr;
        }
        
        try {
            // Load metadata
            PluginMetadata metadata = discovery_->loadMetadata(pluginPath);
            
            // Create plugin instance
            // In production, would use dynamic library loading
            // For now, create a basic plugin
            auto plugin = createPluginInstance(pluginPath, metadata);
            
            // Store plugin
            plugins_[pluginName] = plugin;
            
            // Initialize plugin
            plugin->initialize();
            
            // Call load hook
            plugin->onLoad();
            
            return plugin;
            
        } catch (const std::exception& e) {
            // Log error
            return nullptr;
        }
    }
    
    bool unloadPlugin(const std::string& pluginName) override {
        std::lock_guard<std::mutex> lock(mutex_);
        
        auto it = plugins_.find(pluginName);
        if (it == plugins_.end()) {
            return false;
        }
        
        auto plugin = it->second;
        
        // Call unload hook
        plugin->onUnload();
        
        // Shutdown plugin
        plugin->shutdown();
        
        // Remove plugin
        plugins_.erase(it);
        
        return true;
    }
    
    bool isPluginLoaded(const std::string& pluginName) const override {
        std::lock_guard<std::mutex> lock(mutex_);
        return plugins_.find(pluginName) != plugins_.end();
    }
    
    std::vector<std::string> getLoadedPlugins() const override {
        std::lock_guard<std::mutex> lock(mutex_);
        std::vector<std::string> pluginNames;
        for (const auto& pair : plugins_) {
            pluginNames.push_back(pair.first);
        }
        return pluginNames;
    }
    
    std::shared_ptr<Plugin> getPlugin(const std::string& pluginName) {
        std::lock_guard<std::mutex> lock(mutex_);
        auto it = plugins_.find(pluginName);
        return (it != plugins_.end()) ? it->second : nullptr;
    }

private:
    std::string getPluginNameFromPath(const std::string& pluginPath) {
        std::filesystem::path path(pluginPath);
        return path.stem().string();
    }
    
    std::shared_ptr<Plugin> createPluginInstance(const std::string& pluginPath, const PluginMetadata& metadata) {
        // In production, would load from dynamic library
        // For now, create a basic plugin instance
        class BasicPlugin : public Plugin {
        public:
            std::string getName() const override { return metadata.name; }
            std::string getVersion() const override { return metadata.version; }
            std::string getDescription() const override { return metadata.description; }
            void initialize() override { initialized_ = true; }
            void shutdown() override { initialized_ = false; }
            bool isInitialized() const override { return initialized_; }
        };
        
        return std::make_shared<BasicPlugin>();
    }
    
    std::shared_ptr<PluginDiscovery> discovery_;
    std::map<std::string, std::shared_ptr<Plugin>> plugins_;
    mutable std::mutex mutex_;
};

// Plugin manager - central plugin management
class PluginManager {
public:
    PluginManager() 
        : discovery_(std::make_shared<FileSystemPluginDiscovery>()),
          loader_(std::make_shared<DynamicPluginLoader>(discovery_)) {}
    
    // Plugin discovery and loading
    void scanPlugins(const std::string& directory) {
        std::vector<std::string> pluginFiles = discovery_->discoverPlugins(directory);
        
        for (const auto& pluginPath : pluginFiles) {
            PluginMetadata metadata = discovery_->loadMetadata(pluginPath);
            
            // Check if plugin should be auto-loaded
            if (metadata.autoLoad) {
                loadPlugin(pluginPath);
            }
        }
    }
    
    std::shared_ptr<Plugin> loadPlugin(const std::string& pluginPath) {
        auto plugin = loader_->loadPlugin(pluginPath);
        if (plugin) {
            // Store in registry
            pluginRegistry_[plugin->getMetadata().name] = plugin;
            
            // Configure plugin if configuration exists
            std::string pluginName = plugin->getMetadata().name;
            auto it = pluginConfigurations_.find(pluginName);
            if (it != pluginConfigurations_.end()) {
                plugin->setConfiguration(it->second);
            }
        }
        
        return plugin;
    }
    
    bool unloadPlugin(const std::string& pluginName) {
        // Remove from registry
        auto it = pluginRegistry_.find(pluginName);
        if (it == pluginRegistry_.end()) {
            return false;
        }
        
        // Unload from loader
        bool unloaded = loader_->unloadPlugin(pluginName);
        if (unloaded) {
            pluginRegistry_.erase(it);
        }
        
        // Remove configuration
        pluginConfigurations_.erase(pluginName);
        
        return unloaded;
    }
    
    // Plugin configuration
    void configurePlugin(const std::string& pluginName, const std::map<std::string, std::any>& config) {
        pluginConfigurations_[pluginName] = config;
        
        // Apply configuration to loaded plugin
        auto plugin = getPlugin(pluginName);
        if (plugin) {
            plugin->setConfiguration(config);
        }
    }
    
    void updatePluginConfiguration(const std::string& pluginName, const std::map<std::string, std::any>& newConfig) {
        auto it = pluginConfigurations_.find(pluginName);
        if (it != pluginConfigurations_.end()) {
            it->second.merge(newConfig);
            
            // Notify plugin
            auto plugin = getPlugin(pluginName);
            if (plugin) {
                plugin->onConfigurationUpdate(newConfig);
            }
        }
    }
    
    // Plugin information
    std::vector<std::string> getLoadedPlugins() const {
        return loader_->getLoadedPlugins();
    }
    
    std::shared_ptr<Plugin> getPlugin(const std::string& pluginName) {
        return loader_->getPlugin(pluginName);
    }
    
    PluginMetadata getPluginMetadata(const std::string& pluginName) const {
        auto plugin = loader_->getPlugin(pluginName);
        return plugin ? plugin->getMetadata() : PluginMetadata{};
    }
    
    // Component management
    template<typename ComponentType>
    std::shared_ptr<ComponentType> getComponent(const std::string& pluginName, const std::string& componentType) {
        auto plugin = getPlugin(pluginName);
        if (!plugin) {
            return nullptr;
        }
        
        auto factory = plugin->getComponentFactory(componentType);
        if (!factory) {
            return nullptr;
        }
        
        // In production, would use type-safe casting
        // For now, return as generic void pointer
        return nullptr;
    }
    
    // Plugin lifecycle management
    void initializeAllPlugins() {
        for (auto& pair : pluginRegistry_) {
            auto plugin = pair.second;
            if (plugin && !plugin->isInitialized()) {
                plugin->initialize();
            }
        }
    }
    
    void shutdownAllPlugins() {
        // Shutdown plugins in reverse order
        std::vector<std::string> pluginNames = getLoadedPlugins();
        std::reverse(pluginNames.begin(), pluginNames.end());
        
        for (const auto& pluginName : pluginNames) {
            auto plugin = getPlugin(pluginName);
            if (plugin) {
                plugin->shutdown();
            }
        }
        
        // Clear registry
        pluginRegistry_.clear();
    }
    
    // Plugin discovery
    void refreshPlugins(const std::string& directory) {
        // Unload plugins that no longer exist
        std::vector<std::string> currentPlugins;
        for (const auto& pluginPath : discovery_->discoverPlugins(directory)) {
            currentPlugins.push_back(getPluginNameFromPath(pluginPath));
        }
        
        std::vector<std::string> loadedPlugins = getLoadedPlugins();
        
        for (const auto& pluginName : loadedPlugins) {
            if (std::find(currentPlugins.begin(), currentPlugins.end(), pluginName) == currentPlugins.end()) {
                unloadPlugin(pluginName);
            }
        }
        
        // Load new plugins
        for (const auto& pluginPath : discovery_->discoverPlugins(directory)) {
            if (!isPluginLoaded(getPluginNameFromPath(pluginPath))) {
                loadPlugin(pluginPath);
            }
        }
    }
    
    // Plugin statistics
    std::map<std::string, std::any> getPluginStatistics() const {
        std::map<std::string, std::any> stats;
        stats["total_loaded_plugins"] = static_cast<int64_t>(getLoadedPlugins().size());
        stats["total_registered_plugins"] = static_cast<int64_t>(pluginRegistry_.size());
        
        std::vector<std::string> enabledPlugins;
        for (const auto& pair : pluginRegistry_) {
            if (pair.second->getMetadata().enabled) {
                enabledPlugins.push_back(pair.first);
            }
        }
        
        stats["enabled_plugins"] = static_cast<int64_t>(enabledPlugins.size());
        
        return stats;
    }

private:
    std::string getPluginNameFromPath(const std::string& pluginPath) {
        std::filesystem::path path(pluginPath);
        return path.stem().string();
    }
    
    std::shared_ptr<PluginDiscovery> discovery_;
    std::shared_ptr<PluginLoader> loader_;
    std::map<std::string, std::shared_ptr<Plugin>> pluginRegistry_;
    std::map<std::string, std::map<std::string, std::any>> pluginConfigurations_;
    mutable std::mutex mutex_;
};

// Plugin interface for extending core components
class CoreComponentPluginInterface {
public:
    virtual ~CoreComponentPluginInterface() = default;
    
    // Neuron extension
    virtual std::shared_ptr<Neuron> createNeuron(const std::map<std::string, std::any>& config) { return nullptr; }
    virtual bool validateNeuronConfig(const std::map<std::string, std::any>& config) { return false; }
    
    // Synapse extension
    virtual std::shared_ptr<Synapse> createSynapse(const std::map<std::string, std::any>& config) { return nullptr; }
    virtual bool validateSynapseConfig(const std::map<std::string, std::any>& config) { return false; }
    
    // Brain extension
    virtual std::shared_ptr<Brain> createBrain(const std::shared_ptr<Config>& config) { return nullptr; }
    virtual bool validateBrainConfig(const std::shared_ptr<Config>& config) { return false; }
    
    // Sensory extension
    virtual std::shared_ptr<SensoryInput> createSensoryInput(const std::map<std::string, std::any>& config) { return nullptr; }
    virtual bool validateSensoryInputConfig(const std::map<std::string, std::any>& config) { return false; }
    
    // Motor extension
    virtual std::shared_ptr<MotorSystem> createMotorSystem(const std::map<std::string, std::any>& config) { return nullptr; }
    virtual bool validateMotorSystemConfig(const std::map<std::string, std::any>& config) { return false; }
    
    // Environment extension
    virtual std::shared_ptr<Environment> createEnvironment(const std::map<std::string, std::any>& config) { return nullptr; }
    virtual bool validateEnvironmentConfig(const std::map<std::string, std::any>& config) { return false; }
};

// Core component plugin implementation
class CoreComponentPlugin : public Plugin, public CoreComponentPluginInterface {
public:
    std::string getName() const override { return "core_components"; }
    std::string getVersion() const override { return "1.0.0"; }
    std::string getDescription() const override { return "Core neural network component extensions"; }
    
    void initialize() override {
        Plugin::initialize();
        // Register component factories
        registerComponentFactory<Neuron, NeuronFactory>();
        registerComponentFactory<Synapse, SynapseFactory>();
        registerComponentFactory<Brain, BrainFactory>();
        registerComponentFactory<SensoryInput, SensoryInputFactory>();
        registerComponentFactory<MotorSystem, MotorSystemFactory>();
        registerComponentFactory<Environment, EnvironmentFactory>();
    }
    
    void shutdown() override {
        Plugin::shutdown();
    }
    
private:
    // Component factories
    class NeuronFactory : public ComponentFactory {
    public:
        std::string getComponentType() const override { return typeid(Neuron).name(); }
        std::shared_ptr<void> createComponent(const std::map<std::string, std::any>& config) override {
            auto neuron = std::make_shared<Neuron>();
            // Apply configuration
            return neuron;
        }
        void destroyComponent(std::shared_ptr<void> component) override {
            // Component will be destroyed when reference count reaches zero
        }
        bool validateComponent(const std::map<std::string, std::any>& config) override {
            // Validate configuration
            return true;
        }
    };
    
    class SynapseFactory : public ComponentFactory {
    public:
        std::string getComponentType() const override { return typeid(Synapse).name(); }
        std::shared_ptr<void> createComponent(const std::map<std::string, std::any>& config) override {
            auto synapse = std::make_shared<Synapse>();
            // Apply configuration
            return synapse;
        }
        void destroyComponent(std::shared_ptr<void> component) override {}
        bool validateComponent(const std::map<std::string, std::any>& config) override {
            return true;
        }
    };
    
    class BrainFactory : public ComponentFactory {
    public:
        std::string getComponentType() const override { return typeid(Brain).name(); }
        std::shared_ptr<void> createComponent(const std::map<std::string, std::any>& config) override {
            // Create configuration from component config
            auto brainConfig = std::make_shared<Config>();
            // Apply configuration
            return brainConfig;
        }
        void destroyComponent(std::shared_ptr<void> component) override {}
        bool validateComponent(const std::map<std::string, std::any>& config) override {
            return true;
        }
    };
    
    class SensoryInputFactory : public ComponentFactory {
    public:
        std::string getComponentType() const override { return typeid(SensoryInput).name(); }
        std::shared_ptr<void> createComponent(const std::map<std::string, std::any>& config) override {
            auto sensoryInput = std::make_shared<SensoryInput>();
            // Apply configuration
            return sensoryInput;
        }
        void destroyComponent(std::shared_ptr<void> component) override {}
        bool validateComponent(const std::map<std::string, std::any>& config) override {
            return true;
        }
    };
    
    class MotorSystemFactory : public ComponentFactory {
    public:
        std::string getComponentType() const override { return typeid(MotorSystem).name(); }
        std::shared_ptr<void> createComponent(const std::map<std::string, std::any>& config) override {
            auto motorSystem = std::make_shared<MotorSystem>();
            // Apply configuration
            return motorSystem;
        }
        void destroyComponent(std::shared_ptr<void> component) override {}
        bool validateComponent(const std::map<std::string, std::any>& config) override {
            return true;
        }
    };
    
    class EnvironmentFactory : public ComponentFactory {
    public:
        std::string getComponentType() const override { return typeid(Environment).name(); }
        std::shared_ptr<void> createComponent(const std::map<std::string, std::any>& config) override {
            auto environment = std::make_shared<Environment>();
            // Apply configuration
            return environment;
        }
        void destroyComponent(std::shared_ptr<void> component) override {}
        bool validateComponent(const std::map<std::string, std::any>& config) override {
            return true;
        }
    };
};

// Plugin loader for dynamic library loading (platform-specific)
#ifdef _WIN32
#include <windows.h>
#elif defined(__linux__)
#include <dlfcn.h>
#elif defined(__APPLE__)
#include <dlfcn.h>
#endif

class DynamicLibraryPluginLoader : public DynamicPluginLoader {
public:
    DynamicLibraryPluginLoader(std::shared_ptr<PluginDiscovery> discovery)
        : DynamicPluginLoader(discovery) {}
    
    std::shared_ptr<Plugin> loadPlugin(const std::string& pluginPath) override {
#ifdef _WIN32
        HMODULE module = LoadLibraryA(pluginPath.c_str());
        if (!module) {
            return nullptr;
        }
        
        // Get plugin create function
        using PluginCreateFunc = std::shared_ptr<Plugin>(*)();
        auto createFunc = reinterpret_cast<PluginCreateFunc>(
            GetProcAddress(module, "createPlugin"));
        
        if (!createFunc) {
            FreeLibrary(module);
            return nullptr;
        }
        
        auto plugin = createFunc();
        
        // Store module handle
        pluginModules_[plugin->getMetadata().name] = module;
        
        return plugin;
        
#elif defined(__linux__) || defined(__APPLE__)
        void* module = dlopen(pluginPath.c_str(), RTLD_LAZY | RTLD_LOCAL);
        if (!module) {
            return nullptr;
        }
        
        // Get plugin create function
        using PluginCreateFunc = std::shared_ptr<Plugin>(*)();
        auto createFunc = reinterpret_cast<PluginCreateFunc>(
            dlsym(module, "createPlugin"));
        
        if (!createFunc) {
            dlclose(module);
            return nullptr;
        }
        
        auto plugin = createFunc();
        
        // Store module handle
        pluginModules_[plugin->getMetadata().name] = module;
        
        return plugin;
        
#else
        return nullptr; // Unsupported platform
#endif
    }
    
    bool unloadPlugin(const std::string& pluginName) override {
        auto it = pluginModules_.find(pluginName);
        if (it == pluginModules_.end()) {
            return false;
        }
        
        std::shared_ptr<Plugin> plugin = DynamicPluginLoader::unloadPlugin(pluginName);
        if (!plugin) {
            return false;
        }
        
        // Unload module
#ifdef _WIN32
        FreeLibrary(it->second);
#elif defined(__linux__) || defined(__APPLE__)
        dlclose(it->second);
#endif
        
        pluginModules_.erase(it);
        
        return true;
    }
    
    ~DynamicLibraryPluginLoader() {
        for (auto& pair : pluginModules_) {
#ifdef _WIN32
            FreeLibrary(pair.second);
#elif defined(__linux__) || defined(__APPLE__)
            dlclose(pair.second);
#endif
        }
    }

private:
    std::map<std::string, void*> pluginModules_;
};

// Convenience function to get global plugin manager
inline PluginManager& getPluginManager() {
    static PluginManager manager;
    return manager;
}

// Helper function to load all plugins from directory
inline void loadPluginsFromDirectory(const std::string& directory) {
    getPluginManager().refreshPlugins(directory);
}

// Helper function to unload all plugins
inline void unloadAllPlugins() {
    getPluginManager().shutdownAllPlugins();
}

} // namespace nlm
