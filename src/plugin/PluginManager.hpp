// NLM Plugin Architecture - Extensible plugin system for NLM
//
// This module provides a plugin architecture for NLM, allowing users to add:
// - New experiments and experiment types
// - Custom neuromodulators
// - Special plasticity rules
// - Novel observation models
// - Custom world behaviors
// - Visualization extensions
// - Analysis tools
//
// The architecture uses a dynamic plugin system with:
// - Plugin interface base class
// - Plugin factory for dynamic loading
// - Plugin registry for management
// - Configuration integration for plugin settings
// - Thread-safe plugin lifecycle management

#pragma once

#include <string>
#include <memory>
#include <vector>
#include <map>
#include <functional>
#include <unordered_map>
#include <type_traits>
#include <stdexcept>

namespace nlm {

// Plugin interface version
struct PluginVersion {
    int major;
    int minor;
    int patch;
    
    PluginVersion(int major = 1, int minor = 0, int patch = 0) 
        : major(major), minor(minor), patch(patch) {}
    
    bool operator==(const PluginVersion& other) const {
        return major == other.major && minor == other.minor && patch == other.patch;
    }
};

// Plugin type enumeration
enum class PluginType {
    Experiment,
    Neuromodulator,
    PlasticityRule,
    Sensor,
    World,
    Visualizer,
    Analyzer,
    Utility
};

// Plugin capabilities
struct PluginCapabilities {
    std::vector<PluginType> supportedTypes;
    std::vector<std::string> dependencies;
    std::map<std::string, std::string> requiredConfigParams;
    bool threadSafe;
    bool persistent;
    double memoryUsageMB;
    std::string description;
    
    PluginCapabilities() : threadSafe(false), persistent(false), memoryUsageMB(0.0) {}
};

// Plugin interface base class
class IPlugin {
public:
    virtual ~IPlugin() = default;
    
    // Plugin metadata
    virtual std::string getName() const = 0;
    virtual std::string getDescription() const = 0;
    virtual PluginVersion getVersion() const = 0;
    virtual PluginType getType() const = 0;
    virtual PluginCapabilities getCapabilities() const = 0;
    
    // Plugin lifecycle
    virtual bool initialize(const std::map<std::string, std::string>& config) = 0;
    virtual void shutdown() = 0;
    virtual bool isInitialized() const = 0;
    
    // Plugin functionality
    virtual void update(double deltaTime) {}
    virtual void reset() {}
    virtual std::string getStatus() const { return "Ready"; }
    
    // Configuration management
    virtual bool configure(const std::string& key, const std::string& value) = 0;
    virtual std::string getConfig(const std::string& key) const = 0;
    
    // Plugin-specific methods (to be overridden by subclasses)
    virtual void onExperimentStart() {}
    virtual void onExperimentEnd() {}
    virtual void onStep(double time) {}
    virtual void onStimulus(const std::string& stimulus) {}
    virtual void onReward(float reward, float predictionError) {}
    
    // Extension points for plugin developers
    virtual std::vector<std::string> getExtensionPoints() const {
        return {};
    }
    
    virtual bool supportsExtensionPoint(const std::string& extensionPoint) const {
        return false;
    }
    
    virtual std::string extend(const std::string& extensionPoint, 
                              const std::map<std::string, std::string>& parameters) {
        return "";
    }
};

// Plugin factory for creating plugins
template<typename T>
class PluginFactory {
public:
    using PluginCreateFunc = std::function<std::unique_ptr<IPlugin>()>;
    
    static void registerPlugin(const std::string& name, PluginCreateFunc factory) {
        getRegistry()[name] = factory;
    }
    
    static std::unique_ptr<IPlugin> createPlugin(const std::string& name) {
        auto& registry = getRegistry();
        auto it = registry.find(name);
        if (it != registry.end()) {
            return it->second();
        }
        return nullptr;
    }
    
    static std::vector<std::string> getRegisteredPlugins() {
        std::vector<std::string> names;
        auto& registry = getRegistry();
        for (const auto& pair : registry) {
            names.push_back(pair.first);
        }
        return names;
    }
    
    static bool hasPlugin(const std::string& name) {
        return getRegistry().find(name) != getRegistry().end();
    }
    
    template<typename U>
    static void registerTypedPlugin(const std::string& name) {
        static_assert(std::is_base_of<IPlugin, U>::value, "U must derive from IPlugin");
        registerPlugin(name, []() { return std::make_unique<U>(); });
    }
    
private:
    static std::map<std::string, PluginCreateFunc>& getRegistry() {
        static std::map<std::string, PluginCreateFunc> registry;
        return registry;
    }
};

// Plugin registry for managing loaded plugins
class PluginRegistry {
private:
    struct PluginInfo {
        std::unique_ptr<IPlugin> plugin;
        PluginCapabilities capabilities;
        std::map<std::string, std::string> config;
        bool initialized;
        std::chrono::system_clock::time_point loadTime;
        std::chrono::system_clock::time_point lastUpdate;
        
        PluginInfo(std::unique_ptr<IPlugin> p)
            : plugin(std::move(p)), initialized(false), loadTime(std::chrono::system_clock::now()) {}
    };
    
    std::map<std::string, PluginInfo> plugins;
    std::mutex registryMutex;
    
public:
    PluginRegistry() = default;
    
    // Plugin registration
    bool registerPlugin(std::unique_ptr<IPlugin> plugin) {
        std::lock_guard<std::mutex> lock(registryMutex);
        
        std::string name = plugin->getName();
        if (plugins.find(name) != plugins.end()) {
            return false; // Plugin already registered
        }
        
        plugins[name] = PluginInfo(std::move(plugin));
        return true;
    }
    
    // Plugin loading and initialization
    bool loadPlugin(const std::string& name, const std::map<std::string, std::string>& config = {}) {
        std::lock_guard<std::mutex> lock(registryMutex);
        
        auto it = plugins.find(name);
        if (it == plugins.end()) {
            return false; // Plugin not found
        }
        
        auto& pluginInfo = it->second;
        
        // Initialize plugin
        if (!pluginInfo.plugin->initialize(config)) {
            return false;
        }
        
        pluginInfo.config = config;
        pluginInfo.initialized = true;
        pluginInfo.lastUpdate = std::chrono::system_clock::now();
        
        return true;
    }
    
    // Plugin management
    bool unloadPlugin(const std::string& name) {
        std::lock_guard<std::mutex> lock(registryMutex);
        
        auto it = plugins.find(name);
        if (it == plugins.end()) {
            return false;
        }
        
        auto& pluginInfo = it->second;
        
        if (pluginInfo.initialized) {
            pluginInfo.plugin->shutdown();
        }
        
        plugins.erase(it);
        return true;
    }
    
    // Plugin queries
    IPlugin* getPlugin(const std::string& name) {
        std::lock_guard<std::mutex> lock(registryMutex);
        
        auto it = plugins.find(name);
        if (it != plugins.end()) {
            return it->second.plugin.get();
        }
        return nullptr;
    }
    
    const PluginInfo* getPluginInfo(const std::string& name) const {
        std::lock_guard<std::mutex> lock(registryMutex);
        
        auto it = plugins.find(name);
        if (it != plugins.end()) {
            return &it->second;
        }
        return nullptr;
    }
    
    std::vector<std::string> getPluginNames() const {
        std::lock_guard<std::mutex> lock(registryMutex);
        
        std::vector<std::string> names;
        for (const auto& pair : plugins) {
            names.push_back(pair.first);
        }
        return names;
    }
    
    std::vector<std::string> getPluginNamesByType(PluginType type) const {
        std::lock_guard<std::mutex> lock(registryMutex);
        
        std::vector<std::string> names;
        for (const auto& pair : plugins) {
            const auto& plugin = pair.second.plugin;
            if (plugin->getType() == type) {
                names.push_back(pair.first);
            }
        }
        return names;
    }
    
    // Plugin operations
    bool updatePlugins(double deltaTime) {
        std::lock_guard<std::mutex> lock(registryMutex);
        
        bool allUpdated = true;
        for (auto& pair : plugins) {
            auto& pluginInfo = pair.second;
            
            if (pluginInfo.initialized) {
                try {
                    pluginInfo.plugin->update(deltaTime);
                    pluginInfo.lastUpdate = std::chrono::system_clock::now();
                } catch (const std::exception& e) {
                    // Plugin update failed
                    allUpdated = false;
                }
            }
        }
        
        return allUpdated;
    }
    
    bool resetPlugins() {
        std::lock_guard<std::mutex> lock(registryMutex);
        
        bool allReset = true;
        for (auto& pair : plugins) {
            auto& pluginInfo = pair.second;
            
            if (pluginInfo.initialized) {
                try {
                    pluginInfo.plugin->reset();
                } catch (const std::exception& e) {
                    allReset = false;
                }
            }
        }
        
        return allReset;
    }
    
    // Configuration management
    bool configurePlugin(const std::string& name, const std::string& key, const std::string& value) {
        std::lock_guard<std::mutex> lock(registryMutex);
        
        auto it = plugins.find(name);
        if (it == plugins.end()) {
            return false;
        }
        
        auto& pluginInfo = it->second;
        
        if (pluginInfo.initialized) {
            return pluginInfo.plugin->configure(key, value);
        }
        
        return false;
    }
    
    std::string getPluginConfig(const std::string& name, const std::string& key) const {
        std::lock_guard<std::mutex> lock(registryMutex);
        
        auto it = plugins.find(name);
        if (it == plugins.end()) {
            return "";
        }
        
        const auto& pluginInfo = it->second;
        return pluginInfo.plugin->getConfig(key);
    }
    
    // Plugin lifecycle events
    void onExperimentStart() {
        std::lock_guard<std::mutex> lock(registryMutex);
        
        for (auto& pair : plugins) {
            auto& pluginInfo = pair.second;
            
            if (pluginInfo.initialized) {
                try {
                    pluginInfo.plugin->onExperimentStart();
                } catch (const std::exception& e) {
                    // Handle plugin error
                }
            }
        }
    }
    
    void onExperimentEnd() {
        std::lock_guard<std::mutex> lock(registryMutex);
        
        for (auto& pair : plugins) {
            auto& pluginInfo = pair.second;
            
            if (pluginInfo.initialized) {
                try {
                    pluginInfo.plugin->onExperimentEnd();
                } catch (const std::exception& e) {
                    // Handle plugin error
                }
            }
        }
    }
    
    void onStep(double time) {
        std::lock_guard<std::mutex> lock(registryMutex);
        
        for (auto& pair : plugins) {
            auto& pluginInfo = pair.second;
            
            if (pluginInfo.initialized) {
                try {
                    pluginInfo.plugin->onStep(time);
                } catch (const std::exception& e) {
                    // Handle plugin error
                }
            }
        }
    }
    
    void onStimulus(const std::string& stimulus) {
        std::lock_guard<std::mutex> lock(registryMutex);
        
        for (auto& pair : plugins) {
            auto& pluginInfo = pair.second;
            
            if (pluginInfo.initialized) {
                try {
                    pluginInfo.plugin->onStimulus(stimulus);
                } catch (const std::exception& e) {
                    // Handle plugin error
                }
            }
        }
    }
    
    void onReward(float reward, float predictionError) {
        std::lock_guard<std::mutex> lock(registryMutex);
        
        for (auto& pair : plugins) {
            auto& pluginInfo = pair.second;
            
            if (pluginInfo.initialized) {
                try {
                    pluginInfo.plugin->onReward(reward, predictionError);
                } catch (const std::exception& e) {
                    // Handle plugin error
                }
            }
        }
    }
    
    // Plugin statistics and monitoring
    struct PluginStats {
        std::string name;
        std::string status;
        PluginVersion version;
        PluginCapabilities capabilities;
        std::chrono::system_clock::time_point lastUpdate;
        std::chrono::duration<double> uptime;
        std::string configStatus;
    };
    
    std::vector<PluginStats> getPluginStats() const {
        std::lock_guard<std::mutex> lock(registryMutex);
        
        std::vector<PluginStats> stats;
        auto now = std::chrono::system_clock::now();
        
        for (const auto& pair : plugins) {
            const auto& pluginInfo = pair.second;
            
            PluginStats stat;
            stat.name = pair.first;
            stat.status = pluginInfo.initialized ? "Initialized" : "Not Initialized";
            stat.version = pluginInfo.plugin->getVersion();
            stat.capabilities = pluginInfo.capabilities;
            stat.lastUpdate = pluginInfo.lastUpdate;
            stat.uptime = now - pluginInfo.loadTime;
            
            // Check configuration status
            std::vector<std::string> missingParams;
            for (const auto& param : stat.capabilities.requiredConfigParams) {
                if (pluginInfo.config.find(param) == pluginInfo.config.end()) {
                    missingParams.push_back(param);
                }
            }
            
            if (missingParams.empty()) {
                stat.configStatus = "Complete";
            } else {
                stat.configStatus = "Missing: " + joinStrings(missingParams, ", ");
            }
            
            stats.push_back(stat);
        }
        
        return stats;
    }
    
private:
    static std::string joinStrings(const std::vector<std::string>& strings, const std::string& delimiter) {
        if (strings.empty()) return "";
        std::string result = strings[0];
        for (size_t i = 1; i < strings.size(); ++i) {
            result += delimiter + strings[i];
        }
        return result;
    }
};

// Predefined experiment plugins
class TemporalPredictionExperimentPlugin : public IPlugin {
public:
    std::string getName() const override { return "TemporalPredictionExperiment"; }
    std::string getDescription() const override { return "Temporal prediction learning experiment"; }
    PluginVersion getVersion() const override { return PluginVersion(1, 0, 0); }
    PluginType getType() const override { return PluginType::Experiment; }
    PluginCapabilities getCapabilities() const override {
        PluginCapabilities caps;
        caps.supportedTypes = {PluginType::Experiment, PluginType::Analyzer};
        caps.dependencies = {"Brain", "Memory"};
        caps.requiredConfigParams = {"max_steps", "learning_rate"};
        caps.threadSafe = true;
        caps.description = "Implements temporal prediction learning through neural dynamics";
        return caps;
    }
    
    bool initialize(const std::map<std::string, std::string>& config) override {
        // Initialize temporal prediction experiment
        return true;
    }
    
    void shutdown() override {
        // Clean up experiment resources
    }
    
    bool isInitialized() const override {
        return initialized;
    }
    
    bool configure(const std::string& key, const std::string& value) override {
        config[key] = value;
        return true;
    }
    
    std::string getConfig(const std::string& key) const override {
        auto it = config.find(key);
        return (it != config.end()) ? it->second : "";
    }
    
    void onExperimentStart() override {
        // Start temporal prediction experiment
    }
    
    void onStep(double time) override {
        // Update temporal prediction
    }
    
    std::string getStatus() const override {
        return "Temporal prediction experiment active";
    }
    
private:
    bool initialized = false;
    std::map<std::string, std::string> config;
};

// Plugin manager for integration with NLM core
class PluginManager {
private:
    PluginRegistry pluginRegistry;
    std::map<std::string, std::function<void(IPlugin*)>> extensionPointHandlers;
    
public:
    PluginManager() = default;
    
    // Plugin lifecycle management
    bool registerPlugin(std::unique_ptr<IPlugin> plugin) {
        return pluginRegistry.registerPlugin(std::move(plugin));
    }
    
    bool loadPlugin(const std::string& name, const std::map<std::string, std::string>& config = {}) {
        return pluginRegistry.loadPlugin(name, config);
    }
    
    bool unloadPlugin(const std::string& name) {
        return pluginRegistry.unloadPlugin(name);
    }
    
    // Plugin discovery and queries
    std::vector<std::string> getAvailablePlugins() const {
        return PluginFactory<IPlugin>::getRegisteredPlugins();
    }
    
    std::vector<std::string> getLoadedPlugins() const {
        return pluginRegistry.getPluginNames();
    }
    
    std::vector<std::string> getPluginsByType(PluginType type) const {
        return pluginRegistry.getPluginNamesByType(type);
    }
    
    IPlugin* getPlugin(const std::string& name) {
        return pluginRegistry.getPlugin(name);
    }
    
    // Plugin configuration
    bool configurePlugin(const std::string& name, const std::string& key, const std::string& value) {
        return pluginRegistry.configurePlugin(name, key, value);
    }
    
    std::string getPluginConfig(const std::string& name, const std::string& key) const {
        return pluginRegistry.getPluginConfig(name, key);
    }
    
    // Core integration
    void updatePlugins(double deltaTime) {
        pluginRegistry.updatePlugins(deltaTime);
    }
    
    void resetPlugins() {
        pluginRegistry.resetPlugins();
    }
    
    void onExperimentStart() {
        pluginRegistry.onExperimentStart();
    }
    
    void onExperimentEnd() {
        pluginRegistry.onExperimentEnd();
    }
    
    void onStep(double time) {
        pluginRegistry.onStep(time);
    }
    
    void onStimulus(const std::string& stimulus) {
        pluginRegistry.onStimulus(stimulus);
    }
    
    void onReward(float reward, float predictionError) {
        pluginRegistry.onReward(reward, predictionError);
    }
    
    // Extension points
    void registerExtensionPoint(const std::string& extensionPoint,
                               std::function<void(IPlugin*)> handler) {
        extensionPointHandlers[extensionPoint] = handler;
    }
    
    bool extend(const std::string& extensionPoint, 
                const std::map<std::string, std::string>& parameters) {
        auto it = extensionPointHandlers.find(extensionPoint);
        if (it != extensionPointHandlers.end()) {
            // Find plugins that support this extension point
            for (const auto& name : getLoadedPlugins()) {
                IPlugin* plugin = getPlugin(name);
                if (plugin && plugin->supportsExtensionPoint(extensionPoint)) {
                    try {
                        it->second(plugin);
                    } catch (const std::exception& e) {
                        // Handle plugin error
                    }
                }
            }
            return true;
        }
        return false;
    }
    
    // Plugin statistics and monitoring
    std::vector<PluginRegistry::PluginStats> getPluginStats() const {
        return pluginRegistry.getPluginStats();
    }
    
    std::string getPluginInfoReport() const {
        std::ostringstream oss;
        oss << "=== NLM Plugin System ===" << std::endl;
        oss << "Loaded Plugins: " << getLoadedPlugins().size() << std::endl;
        oss << "Available Plugins: " << getAvailablePlugins().size() << std::endl;
        oss << std::endl;
        
        auto stats = getPluginStats();
        for (const auto& stat : stats) {
            oss << "Plugin: " << stat.name << std::endl;
            oss << "  Status: " << stat.status << std::endl;
            oss << "  Version: " << stat.version.major << "." 
                << stat.version.minor << "." << stat.version.patch << std::endl;
            oss << "  Type: " << static_cast<int>(stat.capabilities.supportedTypes[0]) << std::endl;
            oss << "  Uptime: " << std::chrono::duration_cast<std::chrono::seconds>(stat.uptime).count() << "s" << std::endl;
            oss << "  Config Status: " << stat.configStatus << std::endl;
            oss << std::endl;
        }
        
        return oss.str();
    }
};

// Convenience functions for plugin registration
template<typename T>
void registerPlugin(const std::string& name) {
    PluginFactory<T>::registerTypedPlugin<T>(name);
}

// Plugin system initialization
template<typename... Args>
PluginManager createPluginManager(PluginRegistry& registry, Args&&... args) {
    return PluginManager(std::forward<Args>(args)...);
}

// Global plugin manager instance
inline PluginManager& getPluginManager() {
    static PluginManager manager;
    return manager;
}

} // namespace nlm
