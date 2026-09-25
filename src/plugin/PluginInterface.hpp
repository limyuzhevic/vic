#pragma once

#include <string>
#include <memory>
#include <map>
#include <vector>
#include <functional>
#include <unordered_map>
#include <algorithm>
#include "core/Config/Config.hpp"
#include "brain/Brain.hpp"
#include "agent/AgentBrain.hpp"
#include "environment/Environment.hpp"

namespace nlm {

// Plugin interface types
using PluginId = std::string;
using PluginEvent = std::string;
using PluginParameter = std::map<std::string, std::string>;

// Plugin lifecycle stages
enum class PluginLifecycleStage {
    UNLOADED,
    LOADING,
    LOADED,
    INITIALIZING,
    INITIALIZED,
    RUNNING,
    SHUTTING_DOWN,
    UNLOADED_FINAL
};

// Plugin capabilities
struct PluginCapabilities {
    bool providesBrainModification = false;
    bool providesNeuromodulation = false;
    bool providesSensoryProcessing = false;
    bool providesMotorControl = false;
    bool providesMemoryModification = false;
    bool providesCognitiveFunctions = false;
    bool providesDevelopmentalEffects = false;
    bool supportsRealTimeUpdates = true;
    bool supportsEventDriven = true;
    bool threadSafe = true;
    std::vector<std::string> requiredPlugins;
    std::vector<std::string> optionalPlugins;
};

// Plugin metadata
struct PluginMetadata {
    PluginId id;
    std::string name;
    std::string version;
    std::string description;
    std::string author;
    std::string email;
    std::string website;
    PluginCapabilities capabilities;
    std::map<std::string, std::string> dependencies;
    std::vector<std::string> tags;
    bool enabledByDefault = true;
    std::string license;
    std::string contactInfo;
    std::chrono::system_clock::time_point lastModified;
};

// Plugin event data
struct PluginEventData {
    PluginEvent eventName;
    PluginId sourcePlugin;
    PluginParameter parameters;
    std::chrono::system_clock::time_point timestamp;
    bool processed = false;
};

// Plugin statistics
struct PluginStatistics {
    size_t loadCount = 0;
    size_t unloadCount = 0;
    size_t errorCount = 0;
    size_t warningCount = 0;
    double totalExecutionTime = 0.0;
    size_t eventCount = 0;
    size_t updateCount = 0;
    std::chrono::system_clock::time_point firstLoad;
    std::chrono::system_clock::time_point lastLoad;
    std::chrono::system_clock::time_point lastExecution;
};

// Base plugin interface
class PluginInterface {
public:
    virtual ~PluginInterface() = default;
    
    // Plugin identification
    virtual PluginId getId() const = 0;
    virtual std::string getName() const = 0;
    virtual std::string getVersion() const = 0;
    virtual std::string getDescription() const = 0;
    
    // Lifecycle management
    virtual bool initialize(std::shared_ptr<Brain> brain, 
                           std::shared_ptr<AgentBrain> agentBrain,
                           std::shared_ptr<Config> config) = 0;
    virtual void shutdown() = 0;
    virtual void update(double timestep) = 0;
    
    // Event handling
    virtual void onEvent(const PluginEventData& event) = 0;
    
    // Plugin state
    virtual bool isEnabled() const = 0;
    virtual void enable() = 0;
    virtual void disable() = 0;
    
    // Plugin management
    virtual bool canLoad() const = 0;
    virtual bool canUnload() const = 0;
    
    // Plugin capabilities
    virtual PluginCapabilities getCapabilities() const = 0;
    
    // Statistics
    virtual PluginStatistics getStatistics() const = 0;
    
    // Configuration
    virtual void setParameter(const std::string& key, const std::string& value) = 0;
    virtual std::string getParameter(const std::string& key) const = 0;
    virtual std::map<std::string, std::string> getParameters() const = 0;
    
    // Plugin discovery
    virtual std::vector<std::string> getRequiredPlugins() const = 0;
    virtual std::vector<std::string> getOptionalPlugins() const = 0;
};

// Plugin factory function type
using PluginFactory = std::function<std::unique_ptr<PluginInterface>()>;

// Plugin registry interface
class PluginRegistry {
public:
    virtual ~PluginRegistry() = default;
    
    // Plugin registration
    virtual bool registerPlugin(const PluginMetadata& metadata, 
                               const PluginFactory& factory) = 0;
    virtual bool unregisterPlugin(const PluginId& id) = 0;
    virtual bool updatePlugin(const PluginId& id, const PluginMetadata& metadata) = 0;
    
    // Plugin discovery
    virtual std::vector<PluginId> getAvailablePlugins() const = 0;
    virtual std::vector<PluginId> getEnabledPlugins() const = 0;
    virtual std::vector<PluginId> getRequiredPlugins(const PluginId& pluginId) const = 0;
    
    // Plugin loading
    virtual bool loadPlugin(const PluginId& id) = 0;
    virtual bool unloadPlugin(const PluginId& id) = 0;
    virtual bool isPluginLoaded(const PluginId& id) const = 0;
    
    // Plugin lifecycle
    virtual bool initializePlugin(const PluginId& id) = 0;
    virtual bool shutdownPlugin(const PluginId& id) = 0;
    virtual bool updatePlugin(const PluginId& id, double timestep) = 0;
    
    // Event handling
    virtual void broadcastEvent(const PluginEventData& event) = 0;
    virtual void registerEventHandler(const PluginId& pluginId,
                                     const PluginEvent& event,
                                     std::function<void(const PluginEventData&)> handler) = 0;
    
    // Configuration
    virtual void setGlobalParameter(const std::string& key, const std::string& value) = 0;
    virtual std::string getGlobalParameter(const std::string& key) const = 0;
    
    // Statistics
    virtual PluginStatistics getPluginStatistics(const PluginId& id) const = 0;
    virtual std::map<PluginId, PluginStatistics> getAllStatistics() const = 0;
    
    // Plugin management
    virtual bool enablePlugin(const PluginId& id) = 0;
    virtual bool disablePlugin(const PluginId& id) = 0;
    virtual bool togglePlugin(const PluginId& id) = 0;
    virtual bool isPluginEnabled(const PluginId& id) const = 0;
    
    // Plugin discovery
    virtual PluginMetadata getPluginMetadata(const PluginId& id) const = 0;
    virtual std::vector<PluginMetadata> getAllPluginMetadata() const = 0;
    
    // File I/O
    virtual bool loadPluginsFromDirectory(const std::string& directory) = 0;
    virtual bool savePluginMetadata(const std::string& filename) const = 0;
    virtual bool loadPluginMetadata(const std::string& filename) = 0;
};

// Default plugin registry implementation
class DefaultPluginRegistry : public PluginRegistry {
public:
    DefaultPluginRegistry() {
        setupDefaultConfiguration();
    }
    
    ~DefaultPluginRegistry() override {
        shutdownAllPlugins();
    }
    
    // Plugin registration
    bool registerPlugin(const PluginMetadata& metadata,
                       const PluginFactory& factory) override {
        auto id = metadata.id;
        
        // Check if plugin already exists
        if (pluginFactories_.find(id) != pluginFactories_.end()) {
            logWarning("Plugin already registered: " + id);
            return false;
        }
        
        // Store plugin metadata and factory
        pluginMetadata_[id] = metadata;
        pluginFactories_[id] = factory;
        pluginInstances_[id] = nullptr;
        
        // Initialize plugin lifecycle
        pluginLifecycle_[id] = PluginLifecycleStage::UNLOADED;
        pluginEnabled_[id] = metadata.enabledByDefault;
        
        // Add to dependency graph
        for (const auto& dependency : metadata.dependencies) {
            pluginDependencies_[dependency].push_back(id);
        }
        
        logInfo("Registered plugin: " + id + " (" + metadata.name + ")");
        return true;
    }
    
    bool unregisterPlugin(const PluginId& id) override {
        auto it = pluginMetadata_.find(id);
        if (it == pluginMetadata_.end()) {
            logWarning("Plugin not found: " + id);
            return false;
        }
        
        // Check if plugin is loaded
        if (pluginInstances_.find(id) != pluginInstances_.end() && 
            pluginInstances_[id] != nullptr) {
            logWarning("Cannot unregister loaded plugin: " + id);
            return false;
        }
        
        // Remove from dependency graph
        for (auto& depIt : pluginDependencies_) {
            depIt.second.erase(
                std::remove(depIt.second.begin(), depIt.second.end(), id),
                depIt.second.end()
            );
        }
        
        // Remove plugin from maps
        pluginMetadata_.erase(it);
        pluginFactories_.erase(id);
        pluginInstances_.erase(id);
        pluginLifecycle_.erase(id);
        pluginEnabled_.erase(id);
        pluginStatistics_.erase(id);
        
        logInfo("Unregistered plugin: " + id);
        return true;
    }
    
    bool updatePlugin(const PluginId& id, const PluginMetadata& metadata) override {
        auto it = pluginMetadata_.find(id);
        if (it == pluginMetadata_.end()) {
            logWarning("Plugin not found for update: " + id);
            return false;
        }
        
        // Update metadata
        it->second = metadata;
        
        // Update enabled state
        pluginEnabled_[id] = metadata.enabledByDefault;
        
        // Update dependencies
        // Remove old dependencies
        for (auto& depIt : pluginDependencies_) {
            depIt.second.erase(
                std::remove(depIt.second.begin(), depIt.second.end(), id),
                depIt.second.end()
            );
        }
        
        // Add new dependencies
        for (const auto& dependency : metadata.dependencies) {
            pluginDependencies_[dependency].push_back(id);
        }
        
        logInfo("Updated plugin: " + id);
        return true;
    }
    
    // Plugin discovery
    std::vector<PluginId> getAvailablePlugins() const override {
        std::vector<PluginId> available;
        for (const auto& pair : pluginMetadata_) {
            available.push_back(pair.first);
        }
        return available;
    }
    
    std::vector<PluginId> getEnabledPlugins() const override {
        std::vector<PluginId> enabled;
        for (const auto& pair : pluginEnabled_) {
            if (pair.second) {
                enabled.push_back(pair.first);
            }
        }
        return enabled;
    }
    
    std::vector<PluginId> getRequiredPlugins(const PluginId& pluginId) const override {
        auto it = pluginDependencies_.find(pluginId);
        if (it == pluginDependencies_.end()) {
            return std::vector<PluginId>();
        }
        return it->second;
    }
    
    // Plugin loading
    bool loadPlugin(const PluginId& id) override {
        auto it = pluginMetadata_.find(id);
        if (it == pluginMetadata_.end()) {
            logError("Plugin not found: " + id);
            return false;
        }
        
        auto factoryIt = pluginFactories_.find(id);
        if (factoryIt == pluginFactories_.end()) {
            logError("Plugin factory not found: " + id);
            return false;
        }
        
        // Check if plugin is already loaded
        if (pluginInstances_.find(id) != pluginInstances_.end() && 
            pluginInstances_[id] != nullptr) {
            logWarning("Plugin already loaded: " + id);
            return true;
        }
        
        // Set lifecycle stage to loading
        pluginLifecycle_[id] = PluginLifecycleStage::LOADING;
        
        try {
            // Create plugin instance
            auto instance = factoryIt->second();
            pluginInstances_[id] = instance;
            
            // Set lifecycle stage to loaded
            pluginLifecycle_[id] = PluginLifecycleStage::LOADED;
            
            logInfo("Loaded plugin: " + id);
            
            // Check and load dependencies
            auto dependencies = getRequiredPlugins(id);
            for (const auto& dependency : dependencies) {
                if (!isPluginEnabled(dependency)) {
                    logWarning("Required plugin not enabled: " + dependency + " (required by " + id + ")");
                } else if (!isPluginLoaded(dependency)) {
                    loadPlugin(dependency);
                }
            }
            
            // Update statistics
            pluginStatistics_[id].loadCount++;
            pluginStatistics_[id].firstLoad = std::chrono::system_clock::now();
            
            return true;
        } catch (const std::exception& e) {
            logError("Failed to load plugin " + id + ": " + e.what());
            
            // Clean up
            pluginInstances_[id] = nullptr;
            pluginLifecycle_[id] = PluginLifecycleStage::UNLOADED;
            
            return false;
        }
    }
    
    bool unloadPlugin(const PluginId& id) override {
        auto it = pluginMetadata_.find(id);
        if (it == pluginMetadata_.end()) {
            logWarning("Plugin not found: " + id);
            return false;
        }
        
        // Check if plugin is loaded
        if (pluginInstances_.find(id) == pluginInstances_.end() || 
            pluginInstances_[id] == nullptr) {
            logWarning("Plugin not loaded: " + id);
            return false;
        }
        
        // Shutdown plugin
        if (!shutdownPlugin(id)) {
            return false;
        }
        
        // Remove plugin instance
        pluginInstances_[id] = nullptr;
        pluginLifecycle_[id] = PluginLifecycleStage::UNLOADED;
        
        logInfo("Unloaded plugin: " + id);
        
        // Update statistics
        pluginStatistics_[id].unloadCount++;
        
        return true;
    }
    
    bool isPluginLoaded(const PluginId& id) const override {
        auto it = pluginInstances_.find(id);
        if (it == pluginInstances_.end()) {
            return false;
        }
        return it->second != nullptr;
    }
    
    // Plugin lifecycle
    bool initializePlugin(const PluginId& id) override {
        auto it = pluginInstances_.find(id);
        if (it == pluginInstances_.end() || it->second == nullptr) {
            logError("Plugin not loaded: " + id);
            return false;
        }
        
        // Check lifecycle stage
        if (pluginLifecycle_[id] != PluginLifecycleStage::LOADED) {
            logError("Plugin not in LOADED state: " + id);
            return false;
        }
        
        try {
            // Get config
            auto config = getGlobalConfig();
            
            // Get brain (would need to be set elsewhere)
            auto brain = getBrain();
            auto agentBrain = getAgentBrain();
            
            // Initialize plugin
            bool success = it->second->initialize(brain, agentBrain, config);
            
            if (success) {
                pluginLifecycle_[id] = PluginLifecycleStage::INITIALIZED;
                logInfo("Initialized plugin: " + id);
            } else {
                logError("Failed to initialize plugin: " + id);
            }
            
            return success;
        } catch (const std::exception& e) {
            logError("Exception during plugin initialization " + id + ": " + e.what());
            return false;
        }
    }
    
    bool shutdownPlugin(const PluginId& id) override {
        auto it = pluginInstances_.find(id);
        if (it == pluginInstances_.end() || it->second == nullptr) {
            logError("Plugin not loaded: " + id);
            return false;
        }
        
        try {
            // Shutdown plugin
            it->second->shutdown();
            
            pluginLifecycle_[id] = PluginLifecycleStage::SHUTTING_DOWN;
            logInfo("Shut down plugin: " + id);
            
            return true;
        } catch (const std::exception& e) {
            logError("Exception during plugin shutdown " + id + ": " + e.what());
            return false;
        }
    }
    
    bool updatePlugin(const PluginId& id, double timestep) override {
        auto it = pluginInstances_.find(id);
        if (it == pluginInstances_.end() || it->second == nullptr) {
            logError("Plugin not loaded: " + id);
            return false;
        }
        
        try {
            // Update plugin
            it->second->update(timestep);
            
            // Update statistics
            pluginStatistics_[id].updateCount++;
            pluginStatistics_[id].lastExecution = std::chrono::system_clock::now();
            
            return true;
        } catch (const std::exception& e) {
            logError("Exception during plugin update " + id + ": " + e.what());
            return false;
        }
    }
    
    // Event handling
    void broadcastEvent(const PluginEventData& event) override {
        // Process event for all loaded plugins
        for (auto& pair : pluginInstances_) {
            if (pair.second != nullptr && isPluginEnabled(pair.first)) {
                try {
                    pair.second->onEvent(event);
                    
                    // Update statistics
                    pluginStatistics_[pair.first].eventCount++;
                } catch (const std::exception& e) {
                    logError("Exception during event processing " + pair.first + ": " + e.what());
                    pluginStatistics_[pair.first].errorCount++;
                }
            }
        }
        
        // Process event handlers
        auto handlersIt = eventHandlers_.find(event.eventName);
        if (handlersIt != eventHandlers_.end()) {
            for (const auto& handler : handlersIt->second) {
                try {
                    handler(event);
                } catch (const std::exception& e) {
                    logError("Exception during event handler: " + e.what());
                }
            }
        }
    }
    
    void registerEventHandler(const PluginId& pluginId,
                             const PluginEvent& event,
                             std::function<void(const PluginEventData&)> handler) override {
        eventHandlers_[event].push_back(handler);
        logInfo("Registered event handler for plugin " + pluginId + " event: " + event);
    }
    
    // Configuration
    void setGlobalParameter(const std::string& key, const std::string& value) override {
        globalParameters_[key] = value;
        logInfo("Set global parameter: " + key + " = " + value);
        
        // Broadcast parameter change event
        PluginEventData eventData;
        eventData.eventName = "parameter_changed";
        eventData.sourcePlugin = "system";
        eventData.parameters["key"] = key;
        eventData.parameters["value"] = value;
        broadcastEvent(eventData);
    }
    
    std::string getGlobalParameter(const std::string& key) const override {
        auto it = globalParameters_.find(key);
        if (it == globalParameters_.end()) {
            return "";
        }
        return it->second;
    }
    
    // Statistics
    PluginStatistics getPluginStatistics(const PluginId& id) const override {
        auto it = pluginStatistics_.find(id);
        if (it == pluginStatistics_.end()) {
            return PluginStatistics();
        }
        return it->second;
    }
    
    std::map<PluginId, PluginStatistics> getAllStatistics() const override {
        return pluginStatistics_;
    }
    
    // Plugin management
    bool enablePlugin(const PluginId& id) override {
        auto it = pluginEnabled_.find(id);
        if (it == pluginEnabled_.end()) {
            logError("Plugin not found: " + id);
            return false;
        }
        
        if (it->second) {
            logWarning("Plugin already enabled: " + id);
            return true;
        }
        
        it->second = true;
        logInfo("Enabled plugin: " + id);
        return true;
    }
    
    bool disablePlugin(const PluginId& id) override {
        auto it = pluginEnabled_.find(id);
        if (it == pluginEnabled_.end()) {
            logError("Plugin not found: " + id);
            return false;
        }
        
        if (!it->second) {
            logWarning("Plugin already disabled: " + id);
            return true;
        }
        
        it->second = false;
        logInfo("Disabled plugin: " + id);
        return true;
    }
    
    bool togglePlugin(const PluginId& id) override {
        auto it = pluginEnabled_.find(id);
        if (it == pluginEnabled_.end()) {
            logError("Plugin not found: " + id);
            return false;
        }
        
        it->second = !it->second;
        logInfo("Toggled plugin " + id + " to " + 
               (it->second ? "enabled" : "disabled"));
        return true;
    }
    
    bool isPluginEnabled(const PluginId& id) const override {
        auto it = pluginEnabled_.find(id);
        if (it == pluginEnabled_.end()) {
            return false;
        }
        return it->second;
    }
    
    // Plugin discovery
    PluginMetadata getPluginMetadata(const PluginId& id) const override {
        auto it = pluginMetadata_.find(id);
        if (it == pluginMetadata_.end()) {
            throw std::runtime_error("Plugin not found: " + id);
        }
        return it->second;
    }
    
    std::vector<PluginMetadata> getAllPluginMetadata() const override {
        std::vector<PluginMetadata> metadata;
        for (const auto& pair : pluginMetadata_) {
            metadata.push_back(pair.second);
        }
        return metadata;
    }
    
    // File I/O
    bool loadPluginsFromDirectory(const std::string& directory) override {
        // In a real implementation, this would scan the directory for plugin libraries
        // For now, we'll return false
        logWarning("loadPluginsFromDirectory not implemented");
        return false;
    }
    
    bool savePluginMetadata(const std::string& filename) const override {
        // In a real implementation, this would save plugin metadata to file
        // For now, we'll return false
        logWarning("savePluginMetadata not implemented");
        return false;
    }
    
    bool loadPluginMetadata(const std::string& filename) override {
        // In a real implementation, this would load plugin metadata from file
        // For now, we'll return false
        logWarning("loadPluginMetadata not implemented");
        return false;
    }
    
    // Global state management
    void setBrain(std::shared_ptr<Brain> brain) {
        brain_ = brain;
    }
    
    void setAgentBrain(std::shared_ptr<AgentBrain> agentBrain) {
        agentBrain_ = agentBrain;
    }
    
    void setGlobalConfig(std::shared_ptr<Config> config) {
        globalConfig_ = config;
    }
    
    std::shared_ptr<Brain> getBrain() const {
        return brain_;
    }
    
    std::shared_ptr<AgentBrain> getAgentBrain() const {
        return agentBrain_;
    }
    
    std::shared_ptr<Config> getGlobalConfig() const {
        return globalConfig_;
    }
    
    // Plugin lifecycle management
    void shutdownAllPlugins() {
        // Shutdown all plugins in reverse order
        std::vector<PluginId> pluginIds;
        for (const auto& pair : pluginInstances_) {
            if (pair.second != nullptr) {
                pluginIds.push_back(pair.first);
            }
        }
        
        // Shutdown in reverse order
        for (auto it = pluginIds.rbegin(); it != pluginIds.rend(); ++it) {
            shutdownPlugin(*it);
        }
    }
    
    // Debug and logging
    void setDebugMode(bool enabled) {
        debugMode_ = enabled;
    }
    
    void logInfo(const std::string& message) {
        if (debugMode_) {
            std::cout << "[INFO] " << message << std::endl;
        }
    }
    
    void logWarning(const std::string& message) {
        std::cout << "[WARN] " << message << std::endl;
    }
    
    void logError(const std::string& message) {
        std::cerr << "[ERROR] " << message << std::endl;
    }
    
    // Utility methods
    std::vector<PluginId> getPluginsWithCapability(const std::string& capability) const {
        std::vector<PluginId> plugins;
        for (const auto& pair : pluginMetadata_) {
            if (hasCapability(pair.first, capability)) {
                plugins.push_back(pair.first);
            }
        }
        return plugins;
    }
    
    bool hasCapability(const PluginId& pluginId, const std::string& capability) const {
        auto it = pluginMetadata_.find(pluginId);
        if (it == pluginMetadata_.end()) {
            return false;
        }
        
        const auto& caps = it->second.capabilities;
        
        if (capability == "brain_modification") return caps.providesBrainModification;
        if (capability == "neuromodulation") return caps.providesNeuromodulation;
        if (capability == "sensory_processing") return caps.providesSensoryProcessing;
        if (capability == "motor_control") return caps.providesMotorControl;
        if (capability == "memory_modification") return caps.providesMemoryModification;
        if (capability == "cognitive_functions") return caps.providesCognitiveFunctions;
        if (capability == "developmental_effects") return caps.providesDevelopmentalEffects;
        if (capability == "real_time_updates") return caps.supportsRealTimeUpdates;
        if (capability == "event_driven") return caps.supportsEventDriven;
        if (capability == "thread_safe") return caps.threadSafe;
        
        return false;
    }
    
protected:
    // Setup default configuration
    void setupDefaultConfiguration() {
        // Set default debug mode
        debugMode_ = false;
        
        // Set default event handlers
        eventHandlers_["brain_initialized"] = std::vector<std::function<void(const PluginEventData&)>>();
        eventHandlers_["simulation_step"] = std::vector<std::function<void(const PluginEventData&)>>();
        eventHandlers_["experiment_complete"] = std::vector<std::function<void(const PluginEventData&)>>();
        
        // Set default global parameters
        globalParameters_["version"] = "2.0.0";
        globalParameters_["debug"] = "false";
        globalParameters_["verbose"] = "false";
        
        // Initialize plugin lifecycle
        pluginLifecycle_["system"] = PluginLifecycleStage::INITIALIZED;
        pluginEnabled_["system"] = true;
    }
    
private:
    // Plugin registry
    std::map<PluginId, PluginMetadata> pluginMetadata_;
    std::map<PluginId, PluginFactory> pluginFactories_;
    std::map<PluginId, std::unique_ptr<PluginInterface>> pluginInstances_;
    std::map<PluginId, PluginLifecycleStage> pluginLifecycle_;
    std::map<PluginId, bool> pluginEnabled_;
    std::map<PluginId, PluginStatistics> pluginStatistics_;
    
    // Dependency management
    std::map<std::string, std::vector<PluginId>> pluginDependencies_;
    
    // Event handling
    std::map<PluginEvent, std::vector<std::function<void(const PluginEventData&)>> eventHandlers_;
    
    // Global state
    std::shared_ptr<Brain> brain_;
    std::shared_ptr<AgentBrain> agentBrain_;
    std::shared_ptr<Config> globalConfig_;
    
    // Global parameters
    std::map<std::string, std::string> globalParameters_;
    
    // Debug and logging
    bool debugMode_ = false;
    
    // Debug logging methods
    void logInfo(const std::string& message, bool enabled = true) {
        if (debugMode_ || enabled) {
            std::cout << "[INFO] " << message << std::endl;
        }
    }
    
    void logWarning(const std::string& message) {
        std::cout << "[WARN] " << message << std::endl;
    }
    
    void logError(const std::string& message) {
        std::cerr << "[ERROR] " << message << std::endl;
    }
};

// Built-in curiosity plugin
class CuriosityPlugin : public PluginInterface {
public:
    CuriosityPlugin() {
        metadata_.id = "curiosity";
        metadata_.name = "Curiosity Plugin";
        metadata_.version = "1.0.0";
        metadata_.description = "Implements curiosity-driven exploration behavior";
        metadata_.author = "NLM Development Team";
        metadata_.email = "support@nlm.org";
        metadata_.website = "https://nlm.org";
        metadata_.license = "MIT";
        
        metadata_.capabilities.providesNeuromodulation = true;
        metadata_.capabilities.providesCognitiveFunctions = true;
        metadata_.capabilities.supportsEventDriven = true;
        
        metadata_.enabledByDefault = true;
        
        // Dependencies
        metadata_.dependencies["brain"] = "core";
        metadata_.dependencies["agent_brain"] = "core";
    }
    
    ~CuriosityPlugin() override {
        shutdown();
    }
    
    PluginId getId() const override {
        return metadata_.id;
    }
    
    std::string getName() const override {
        return metadata_.name;
    }
    
    std::string getVersion() const override {
        return metadata_.version;
    }
    
    std::string getDescription() const override {
        return metadata_.description;
    }
    
    bool initialize(std::shared_ptr<Brain> brain,
                   std::shared_ptr<AgentBrain> agentBrain,
                   std::shared_ptr<Config> config) override {
        brain_ = brain;
        agentBrain_ = agentBrain;
        config_ = config;
        
        // Initialize parameters
        explorationRate_ = config_->getOr<float>("plugin.curiosity.exploration_rate", 0.5f);
        noveltyThreshold_ = config_->getOr<float>("plugin.curiosity.novelty_threshold", 0.3f);
        curiosityEnabled_ = config_->getOr<bool>("plugin.curiosity.enabled", true);
        
        // Register configuration parameters
        config_->set("plugin.curiosity.exploration_rate", explorationRate_, 
                    nlm::ConfigSource::Runtime);
        config_->set("plugin.curiosity.novelty_threshold", noveltyThreshold_, 
                    nlm::ConfigSource::Runtime);
        config_->set("plugin.curiosity.enabled", curiosityEnabled_, 
                    nlm::ConfigSource::Runtime);
        
        // Register event handlers
        registerEventHandler("brain_initialized", 
                           [this](const PluginEventData& event) { onBrainInitialized(event); });
        registerEventHandler("simulation_step",
                           [this](const PluginEventData& event) { onSimulationStep(event); });
        
        // Initialize curiosity system
        initializeCuriositySystem();
        
        return true;
    }
    
    void shutdown() override {
        curiositySystem_.reset();
        brain_.reset();
        agentBrain_.reset();
        config_.reset();
    }
    
    void update(double timestep) override {
        if (!enabled_ || !brain_ || !agentBrain_) return;
        
        // Update curiosity system
        updateCuriositySystem(timestep);
        
        // Apply curiosity to agent brain
        applyCuriosityToAgentBrain();
        
        // Log activity
        if (debug_mode_) {
            logDebug("Curiosity plugin updated: " + 
                    std::to_string(curiosityLevel_) + " (exploration: " +
                    std::to_string(explorationRate_) + ")");
        }
    }
    
    void onEvent(const PluginEventData& event) override {
        if (event.eventName == "brain_initialized") {
            onBrainInitialized(event);
        } else if (event.eventName == "simulation_step") {
            onSimulationStep(event);
        }
    }
    
    bool isEnabled() const override {
        return enabled_;
    }
    
    void enable() override {
        enabled_ = true;
        logDebug("Curiosity plugin enabled");
    }
    
    void disable() override {
        enabled_ = false;
        logDebug("Curiosity plugin disabled");
    }
    
    bool canLoad() const override {
        return true;
    }
    
    bool canUnload() const override {
        return true;
    }
    
    PluginCapabilities getCapabilities() const override {
        return metadata_.capabilities;
    }
    
    PluginStatistics getStatistics() const override {
        return statistics_;
    }
    
    void setParameter(const std::string& key, const std::string& value) override {
        if (key == "plugin.curiosity.exploration_rate") {
            explorationRate_ = std::stof(value);
            config_->set(key, explorationRate_, nlm::ConfigSource::Runtime);
        } else if (key == "plugin.curiosity.novelty_threshold") {
            noveltyThreshold_ = std::stof(value);
            config_->set(key, noveltyThreshold_, nlm::ConfigSource::Runtime);
        } else if (key == "plugin.curiosity.enabled") {
            curiosityEnabled_ = (value == "true" || value == "1");
            config_->set(key, curiosityEnabled_, nlm::ConfigSource::Runtime);
        }
    }
    
    std::string getParameter(const std::string& key) const override {
        if (key == "plugin.curiosity.exploration_rate") {
            return std::to_string(explorationRate_);
        } else if (key == "plugin.curiosity.novelty_threshold") {
            return std::to_string(noveltyThreshold_);
        } else if (key == "plugin.curiosity.enabled") {
            return curiosityEnabled_ ? "true" : "false";
        }
        return "";
    }
    
    std::map<std::string, std::string> getParameters() const override {
        std::map<std::string, std::string> params;
        params["plugin.curiosity.exploration_rate"] = std::to_string(explorationRate_);
        params["plugin.curiosity.novelty_threshold"] = std::to_string(noveltyThreshold_);
        params["plugin.curiosity.enabled"] = curiosityEnabled_ ? "true" : "false";
        return params;
    }
    
    std::vector<std::string> getRequiredPlugins() const override {
        return metadata_.dependencies;
    }
    
    std::vector<std::string> getOptionalPlugins() const override {
        return std::vector<std::string>(); // No optional plugins for now
    }
    
private:
    void initializeCuriositySystem() {
        // Initialize curiosity system parameters
        curiositySystem_.reset(new CuriositySystem());
        
        curiositySystem_->setExplorationRate(explorationRate_);
        curiositySystem_->setNoveltyThreshold(noveltyThreshold_);
        curiositySystem_->setEnabled(curiosityEnabled_);
        
        // Set up curiosity monitoring
        curiosityMonitoringActive_ = true;
    }
    
    void updateCuriositySystem(double timestep) {
        if (!curiositySystem_) return;
        
        // Update curiosity based on novelty
        float novelty = calculateNovelty();
        curiositySystem_->updateCuriosityLevel(novelty, timestep);
        
        // Store curiosity level for external access
        curiosityLevel_ = curiositySystem_->getCuriosityLevel();
    }
    
    float calculateNovelty() {
        if (!agentBrain_) return 0.0f;
        
        // Calculate novelty based on sensory input changes
        // This is a simplified implementation
        // In a real system, this would compare current input with historical input
        
        // Get novelty from agent brain
        float novelty = agentBrain_->getNoveltyLevel();
        
        // Apply threshold
        if (novelty < noveltyThreshold_) {
            return 0.0f;
        }
        
        // Scale novelty to appropriate range
        return (novelty - noveltyThreshold_) / (1.0f - noveltyThreshold_);
    }
    
    void applyCuriosityToAgentBrain() {
        if (!agentBrain_ || !curiositySystem_) return;
        
        // Apply curiosity-based exploration
        float curiosityLevel = curiositySystem_->getCuriosityLevel();
        
        // Modify agent's curiosity behavior
        if (curiosityEnabled_) {
            // In a real implementation, this would modify:
            // 1. AgentBrain's decision-making process
            // 2. Exploration vs. exploitation balance
            // 3. Action selection criteria
            
            // For demonstration, we'll just log the effect
            if (debug_mode_) {
                logDebug("Applying curiosity: " + std::to_string(curiosityLevel) +
                        " (exploration rate: " + std::to_string(explorationRate_) + ")");
            }
        }
    }
    
    void onBrainInitialized(const PluginEventData& event) {
        logDebug("Curiosity plugin: Brain initialized event received");
        
        // Initialize curiosity monitoring
        curiosityMonitoringActive_ = true;
        
        // Calculate initial curiosity level
        curiosityLevel_ = 0.0f;
    }
    
    void onSimulationStep(const PluginEventData& event) {
        // Process simulation step event
        // Update curiosity system
        if (curiosityMonitoringActive_) {
            updateCuriositySystem(0.001); // Default timestep
        }
    }
    
    void logDebug(const std::string& message) {
        if (debug_mode_) {
            std::cout << "[CURIOSITY] " << message << std::endl;
        }
    }
    
    struct CuriositySystem {
        void setExplorationRate(float rate) {
            explorationRate_ = rate;
        }
        
        void setNoveltyThreshold(float threshold) {
            noveltyThreshold_ = threshold;
        }
        
        void setEnabled(bool enabled) {
            enabled_ = enabled;
        }
        
        void updateCuriosityLevel(float novelty, double timestep) {
            if (!enabled_) {
                currentLevel_ = 0.0f;
                return;
            }
            
            // Update curiosity level based on novelty
            float noveltyEffect = std::min(1.0f, novelty * 2.0f); // Scale novelty
            
            // Apply decay
            currentLevel_ = currentLevel_ * (1.0f - decayRate_ * timestep) +
                          noveltyEffect * curiosityGain_ * timestep;
            
            // Clamp to valid range
            currentLevel_ = std::max(0.0f, std::min(1.0f, currentLevel_));
        }
        
        float getCuriosityLevel() const {
            return currentLevel_;
        }
        
    private:
        float explorationRate_ = 0.5f;
        float noveltyThreshold_ = 0.3f;
        bool enabled_ = true;
        float currentLevel_ = 0.0f;
        float decayRate_ = 0.1f;
        float curiosityGain_ = 0.5f;
    };
    
    PluginMetadata metadata_;
    std::shared_ptr<Brain> brain_;
    std::shared_ptr<AgentBrain> agentBrain_;
    std::shared_ptr<Config> config_;
    
    // Plugin state
    bool enabled_ = false;
    bool debug_mode_ = false;
    bool curiosityMonitoringActive_ = false;
    
    // Plugin parameters
    float explorationRate_ = 0.5f;
    float noveltyThreshold_ = 0.3f;
    
    // Plugin statistics
    PluginStatistics statistics_;
    
    // Internal systems
    std::unique_ptr<CuriositySystem> curiositySystem_;
    float curiosityLevel_ = 0.0f;
};

// Built-in reward plugin
class RewardPlugin : public PluginInterface {
public:
    RewardPlugin() {
        metadata_.id = "reward";
        metadata_.name = "Reward Plugin";
        metadata_.version = "1.0.0";
        metadata_.description = "Implements reward-based learning and reinforcement";
        metadata_.author = "NLM Development Team";
        metadata_.email = "support@nlm.org";
        metadata_.website = "https://nlm.org";
        metadata_.license = "MIT";
        
        metadata_.capabilities.providesNeuromodulation = true;
        metadata_.capabilities.providesBrainModification = true;
        
        metadata_.enabledByDefault = true;
        
        // Dependencies
        metadata_.dependencies["brain"] = "core";
        metadata_.dependencies["agent_brain"] = "core";
    }
    
    ~RewardPlugin() override {
        shutdown();
    }
    
    // Plugin interface implementations (similar to CuriosityPlugin)
    PluginId getId() const override { return metadata_.id; }
    std::string getName() const override { return metadata_.name; }
    std::string getVersion() const override { return metadata_.version; }
    std::string getDescription() const override { return metadata_.description; }
    
    bool initialize(std::shared_ptr<Brain> brain,
                   std::shared_ptr<AgentBrain> agentBrain,
                   std::shared_ptr<Config> config) override {
        brain_ = brain;
        agentBrain_ = agentBrain;
        config_ = config;
        
        // Initialize parameters
        rewardBaseline_ = config_->getOr<float>("plugin.reward.baseline", 0.0f);
        rewardDecay_ = config_->getOr<float>("plugin.reward.decay", 0.95f);
        rewardEnabled_ = config_->getOr<bool>("plugin.reward.enabled", true);
        
        // Register configuration parameters
        config_->set("plugin.reward.baseline", rewardBaseline_, nlm::ConfigSource::Runtime);
        config_->set("plugin.reward.decay", rewardDecay_, nlm::ConfigSource::Runtime);
        config_->set("plugin.reward.enabled", rewardEnabled_, nlm::ConfigSource::Runtime);
        
        // Register event handlers
        registerEventHandler("brain_initialized",
                           [this](const PluginEventData& event) { onBrainInitialized(event); });
        registerEventHandler("simulation_step",
                           [this](const PluginEventData& event) { onSimulationStep(event); });
        
        // Initialize reward system
        initializeRewardSystem();
        
        return true;
    }
    
    void shutdown() override {
        rewardSystem_.reset();
        brain_.reset();
        agentBrain_.reset();
        config_.reset();
    }
    
    void update(double timestep) override {
        if (!enabled_ || !brain_ || !agentBrain_) return;
        
        // Update reward system
        updateRewardSystem(timestep);
        
        // Apply reward to agent brain
        applyRewardToAgentBrain();
        
        // Log activity
        if (debug_mode_) {
            logDebug("Reward plugin updated: " + std::to_string(rewardLevel_) +
                    " (decay: " + std::to_string(rewardDecay_) + ")");
        }
    }
    
    void onEvent(const PluginEventData& event) override {
        if (event.eventName == "brain_initialized") {
            onBrainInitialized(event);
        } else if (event.eventName == "simulation_step") {
            onSimulationStep(event);
        }
    }
    
    bool isEnabled() const override {
        return enabled_;
    }
    
    void enable() override {
        enabled_ = true;
        logDebug("Reward plugin enabled");
    }
    
    void disable() override {
        enabled_ = false;
        logDebug("Reward plugin disabled");
    }
    
    bool canLoad() const override {
        return true;
    }
    
    bool canUnload() const override {
        return true;
    }
    
    PluginCapabilities getCapabilities() const override {
        return metadata_.capabilities;
    }
    
    PluginStatistics getStatistics() const override {
        return statistics_;
    }
    
    void setParameter(const std::string& key, const std::string& value) override {
        if (key == "plugin.reward.baseline") {
            rewardBaseline_ = std::stof(value);
            config_->set(key, rewardBaseline_, nlm::ConfigSource::Runtime);
        } else if (key == "plugin.reward.decay") {
            rewardDecay_ = std::stof(value);
            config_->set(key, rewardDecay_, nlm::ConfigSource::Runtime);
        } else if (key == "plugin.reward.enabled") {
            rewardEnabled_ = (value == "true" || value == "1");
            config_->set(key, rewardEnabled_, nlm::ConfigSource::Runtime);
        }
    }
    
    std::string getParameter(const std::string& key) const override {
        if (key == "plugin.reward.baseline") {
            return std::to_string(rewardBaseline_);
        } else if (key == "plugin.reward.decay") {
            return std::to_string(rewardDecay_);
        } else if (key == "plugin.reward.enabled") {
            return rewardEnabled_ ? "true" : "false";
        }
        return "";
    }
    
    std::map<std::string, std::string> getParameters() const override {
        std::map<std::string, std::string> params;
        params["plugin.reward.baseline"] = std::to_string(rewardBaseline_);
        params["plugin.reward.decay"] = std::to_string(rewardDecay_);
        params["plugin.reward.enabled"] = rewardEnabled_ ? "true" : "false";
        return params;
    }
    
    std::vector<std::string> getRequiredPlugins() const override {
        return metadata_.dependencies;
    }
    
    std::vector<std::string> getOptionalPlugins() const override {
        return std::vector<std::string>();
    }
    
private:
    void initializeRewardSystem() {
        // Initialize reward system parameters
        rewardSystem_.reset(new RewardSystem());
        
        rewardSystem_->setBaseline(rewardBaseline_);
        rewardSystem_->setDecay(rewardDecay_);
        rewardSystem_->setEnabled(rewardEnabled_);
        
        // Set up reward monitoring
        rewardMonitoringActive_ = true;
    }
    
    void updateRewardSystem(double timestep) {
        if (!rewardSystem_) return;
        
        // Update reward system based on external signals
        // In a real implementation, this would process reward signals from the environment
        
        // Calculate current reward level
        rewardLevel_ = rewardSystem_->getRewardLevel();
        
        // Apply to agent brain
        applyRewardToAgentBrain();
    }
    
    void applyRewardToAgentBrain() {
        if (!agentBrain_ || !rewardSystem_) return;
        
        // Apply reward to agent's neuromodulation system
        if (rewardEnabled_) {
            // Get current reward
            float reward = rewardSystem_->getRewardLevel();
            
            // Apply to agent brain (this would modify dopamine levels, etc.)
            if (debug_mode_) {
                logDebug("Applying reward: " + std::to_string(reward) +
                        " (baseline: " + std::to_string(rewardBaseline_) + ")");
            }
        }
    }
    
    void onBrainInitialized(const PluginEventData& event) {
        logDebug("Reward plugin: Brain initialized event received");
        
        // Initialize reward monitoring
        rewardMonitoringActive_ = true;
        
        // Calculate initial reward level
        rewardLevel_ = rewardBaseline_;
    }
    
    void onSimulationStep(const PluginEventData& event) {
        // Process simulation step event
        if (rewardMonitoringActive_) {
            updateRewardSystem(0.001); // Default timestep
        }
    }
    
    void logDebug(const std::string& message) {
        if (debug_mode_) {
            std::cout << "[REWARD] " << message << std::endl;
        }
    }
    
    struct RewardSystem {
        void setBaseline(float baseline) {
            baseline_ = baseline;
            currentLevel_ = baseline;
        }
        
        void setDecay(float decay) {
            decay_ = decay;
        }
        
        void setEnabled(bool enabled) {
            enabled_ = enabled;
        }
        
        void updateRewardLevel(double timestep) {
            if (!enabled_) {
                currentLevel_ = baseline_;
                return;
            }
            
            // Apply decay
            currentLevel_ = currentLevel_ * decay_ + timestep * rewardInput_;
            
            // Clamp to reasonable range
            currentLevel_ = std::max(0.0f, std::min(1.0f, currentLevel_));
        }
        
        float getRewardLevel() const {
            return currentLevel_;
        }
        
        void addReward(float reward) {
            rewardInput_ += reward;
        }
        
        void clearReward() {
            rewardInput_ = 0.0f;
        }
        
    private:
        float baseline_ = 0.0f;
        float decay_ = 0.95f;
        bool enabled_ = true;
        float currentLevel_ = 0.0f;
        float rewardInput_ = 0.0f;
    };
    
    PluginMetadata metadata_;
    std::shared_ptr<Brain> brain_;
    std::shared_ptr<AgentBrain> agentBrain_;
    std::shared_ptr<Config> config_;
    
    // Plugin state
    bool enabled_ = false;
    bool debug_mode_ = false;
    bool rewardMonitoringActive_ = false;
    
    // Plugin parameters
    float rewardBaseline_ = 0.0f;
    float rewardDecay_ = 0.95f;
    
    // Plugin statistics
    PluginStatistics statistics_;
    
    // Internal systems
    std::unique_ptr<RewardSystem> rewardSystem_;
    float rewardLevel_ = 0.0f;
};

// Built-in developmental plugin
class DevelopmentalPlugin : public PluginInterface {
public:
    DevelopmentalPlugin() {
        metadata_.id = "developmental";
        metadata_.name = "Developmental Plugin";
        metadata_.version = "1.0.0";
        metadata_.description = "Implements developmental stages and maturation";
        metadata_.author = "NLM Development Team";
        metadata_.email = "support@nlm.org";
        metadata_.website = "https://nlm.org";
        metadata_.license = "MIT";
        
        metadata_.capabilities.providesDevelopmentalEffects = true;
        metadata_.capabilities.providesBrainModification = true;
        metadata_.capabilities.supportsEventDriven = true;
        
        metadata_.enabledByDefault = true;
        
        // Dependencies
        metadata_.dependencies["brain"] = "core";
    }
    
    ~DevelopmentalPlugin() override {
        shutdown();
    }
    
    // Plugin interface implementations (similar to previous plugins)
    PluginId getId() const override { return metadata_.id; }
    std::string getName() const override { return metadata_.name; }
    std::string getVersion() const override { return metadata_.version; }
    std::string getDescription() const override { return metadata_.description; }
    
    bool initialize(std::shared_ptr<Brain> brain,
                   std::shared_ptr<AgentBrain> agentBrain,
                   std::shared_ptr<Config> config) override {
        brain_ = brain;
        agentBrain_ = agentBrain;
        config_ = config;
        
        // Initialize parameters
        developmentalStage_ = config_->getOr<int>("plugin.developmental.stage", 0);
        plasticityReductionRate_ = config_->getOr<float>("plugin.developmental.plasticity_reduction_rate", 0.01f);
        developmentalEnabled_ = config_->getOr<bool>("plugin.developmental.enabled", true);
        
        // Register configuration parameters
        config_->set("plugin.developmental.stage", developmentalStage_, nlm::ConfigSource::Runtime);
        config_->set("plugin.developmental.plasticity_reduction_rate", plasticityReductionRate_, nlm::ConfigSource::Runtime);
        config_->set("plugin.developmental.enabled", developmentalEnabled_, nlm::ConfigSource::Runtime);
        
        // Register event handlers
        registerEventHandler("brain_initialized",
                           [this](const PluginEventData& event) { onBrainInitialized(event); });
        registerEventHandler("simulation_step",
                           [this](const PluginEventData& event) { onSimulationStep(event); });
        
        // Initialize developmental system
        initializeDevelopmentalSystem();
        
        return true;
    }
    
    void shutdown() override {
        developmentalSystem_.reset();
        brain_.reset();
        agentBrain_.reset();
        config_.reset();
    }
    
    void update(double timestep) override {
        if (!enabled_ || !brain_ || !agentBrain_) return;
        
        // Update developmental system
        updateDevelopmentalSystem(timestep);
        
        // Apply developmental effects
        applyDevelopmentalEffects();
        
        // Log activity
        if (debug_mode_) {
            logDebug("Developmental plugin updated: stage " + std::to_string(developmentalStage_) +
                    " (reduction rate: " + std::to_string(plasticityReductionRate_) + ")");
        }
    }
    
    void onEvent(const PluginEventData& event) override {
        if (event.eventName == "brain_initialized") {
            onBrainInitialized(event);
        } else if (event.eventName == "simulation_step") {
            onSimulationStep(event);
        }
    }
    
    bool isEnabled() const override {
        return enabled_;
    }
    
    void enable() override {
        enabled_ = true;
        logDebug("Developmental plugin enabled");
    }
    
    void disable() override {
        enabled_ = false;
        logDebug("Developmental plugin disabled");
    }
    
    bool canLoad() const override {
        return true;
    }
    
    bool canUnload() const override {
        return true;
    }
    
    PluginCapabilities getCapabilities() const override {
        return metadata_.capabilities;
    }
    
    PluginStatistics getStatistics() const override {
        return statistics_;
    }
    
    void setParameter(const std::string& key, const std::string& value) override {
        if (key == "plugin.developmental.stage") {
            developmentalStage_ = std::stoi(value);
            config_->set(key, developmentalStage_, nlm::ConfigSource::Runtime);
        } else if (key == "plugin.developmental.plasticity_reduction_rate") {
            plasticityReductionRate_ = std::stof(value);
            config_->set(key, plasticityReductionRate_, nlm::ConfigSource::Runtime);
        } else if (key == "plugin.developmental.enabled") {
            developmentalEnabled_ = (value == "true" || value == "1");
            config_->set(key, developmentalEnabled_, nlm::ConfigSource::Runtime);
        }
    }
    
    std::string getParameter(const std::string& key) const override {
        if (key == "plugin.developmental.stage") {
            return std::to_string(developmentalStage_);
        } else if (key == "plugin.developmental.plasticity_reduction_rate") {
            return std::to_string(plasticityReductionRate_);
        } else if (key == "plugin.developmental.enabled") {
            return developmentalEnabled_ ? "true" : "false";
        }
        return "";
    }
    
    std::map<std::string, std::string> getParameters() const override {
        std::map<std::string, std::string> params;
        params["plugin.developmental.stage"] = std::to_string(developmentalStage_);
        params["plugin.developmental.plasticity_reduction_rate"] = std::to_string(plasticityReductionRate_);
        params["plugin.developmental.enabled"] = developmentalEnabled_ ? "true" : "false";
        return params;
    }
    
    std::vector<std::string> getRequiredPlugins() const override {
        return metadata_.dependencies;
    }
    
    std::vector<std::string> getOptionalPlugins() const override {
        return std::vector<std::string>();
    }
    
private:
    void initializeDevelopmentalSystem() {
        // Initialize developmental system parameters
        developmentalSystem_.reset(new DevelopmentalSystem());
        
        developmentalSystem_->setStage(developmentalStage_);
        developmentalSystem_->setPlasticityReductionRate(plasticityReductionRate_);
        developmentalSystem_->setEnabled(developmentalEnabled_);
        
        // Set up developmental monitoring
        developmentalMonitoringActive_ = true;
    }
    
    void updateDevelopmentalSystem(double timestep) {
        if (!developmentalSystem_) return;
        
        // Update developmental system
        developmentalSystem_->updateStage(timestep);
        
        // Get current stage
        developmentalStage_ = developmentalSystem_->getStage();
        
        // Apply developmental effects
        applyDevelopmentalEffects();
    }
    
    void applyDevelopmentalEffects() {
        if (!brain_ || !developmentalSystem_) return;
        
        // Apply developmental effects based on current stage
        switch (developmentalStage_) {
            case 0: // Initial stage
                // High plasticity, rapid learning
                applyHighPlasticityEffects();
                break;
            case 1: // Critical period
                // Moderate plasticity, specialized learning
                applyCriticalPeriodEffects();
                break;
            case 2: // Maturation
                // Reduced plasticity, stable behavior
                applyMaturationEffects();
                break;
            case 3: // Adult
                // Low plasticity, stable behavior
                applyAdultEffects();
                break;
        }
        
        if (debug_mode_) {
            logDebug("Applied developmental effects for stage: " + std::to_string(developmentalStage_));
        }
    }
    
    void applyHighPlasticityEffects() {
        // In a real implementation, this would:
        // 1. Increase synaptic plasticity
        // 2. Enhance learning capacity
        // 3. Promote structural changes
        // 4. Apply high learning rates
        
        // For demonstration, we'll just log the effect
        std::cout << "[DEVELOPMENTAL] Applying high plasticity effects" << std::endl;
    }
    
    void applyCriticalPeriodEffects() {
        // In a real implementation, this would:
        // 1. Apply sensitive period learning
        // 2. Enhance pattern learning
        // 3. Promote specialization
        // 4. Apply moderate learning rates
        
        std::cout << "[DEVELOPMENTAL] Applying critical period effects" << std::endl;
    }
    
    void applyMaturationEffects() {
        // In a real implementation, this would:
        // 1. Stabilize neural connections
        // 2. Reduce plasticity
        // 3. Consolidate learned behaviors
        // 4. Apply low learning rates
        
        std::cout << "[DEVELOPMENTAL] Applying maturation effects" << std::endl;
    }
    
    void applyAdultEffects() {
        // In a real implementation, this would:
        // 1. Maintain stable neural connections
        // 2. Minimize plasticity changes
        // 3. Preserve learned behaviors
        // 4. Apply minimal learning rates
        
        std::cout << "[DEVELOPMENTAL] Applying adult effects" << std::endl;
    }
    
    void onBrainInitialized(const PluginEventData& event) {
        logDebug("Developmental plugin: Brain initialized event received");
        
        // Initialize developmental monitoring
        developmentalMonitoringActive_ = true;
        
        // Calculate initial developmental stage
        developmentalStage_ = 0; // Start with initial stage
    }
    
    void onSimulationStep(const PluginEventData& event) {
        // Process simulation step event
        if (developmentalMonitoringActive_) {
            updateDevelopmentalSystem(0.001); // Default timestep
        }
    }
    
    void logDebug(const std::string& message) {
        if (debug_mode_) {
            std::cout << "[DEVELOPMENTAL] " << message << std::endl;
        }
    }
    
    struct DevelopmentalSystem {
        void setStage(int stage) {
            currentStage_ = stage;
        }
        
        void setPlasticityReductionRate(float rate) {
            plasticityReductionRate_ = rate;
        }
        
        void setEnabled(bool enabled) {
            enabled_ = enabled;
        }
        
        void updateStage(double timestep) {
            if (!enabled_) return;
            
            // Update developmental stage based on age
            // This is a simplified implementation
            // In a real system, this would use actual developmental timing
            
            // For demonstration, we'll increment stage based on accumulated time
            accumulatedTime_ += timestep;
            
            // Change stage every 1000 timesteps (simplified developmental timeline)
            if (accumulatedTime_ >= 1000.0) {
                currentStage_++;
                if (currentStage_ > 3) {
                    currentStage_ = 3; // Cap at adult stage
                }
                accumulatedTime_ = 0.0;
                
                // Apply stage transition effects
                onStageTransition(currentStage_);
            }
        }
        
        int getStage() const {
            return currentStage_;
        }
        
        void onStageTransition(int newStage) {
            // Handle stage transition effects
            // This would modify brain properties based on stage
            std::cout << "[DEVELOPMENTAL] Transitioned to developmental stage: " << newStage << std::endl;
        }
        
    private:
        int currentStage_ = 0;
        float plasticityReductionRate_ = 0.01f;
        bool enabled_ = true;
        double accumulatedTime_ = 0.0;
    };
    
    PluginMetadata metadata_;
    std::shared_ptr<Brain> brain_;
    std::shared_ptr<AgentBrain> agentBrain_;
    std::shared_ptr<Config> config_;
    
    // Plugin state
    bool enabled_ = false;
    bool debug_mode_ = false;
    bool developmentalMonitoringActive_ = false;
    
    // Plugin parameters
    int developmentalStage_ = 0;
    float plasticityReductionRate_ = 0.01f;
    
    // Plugin statistics
    PluginStatistics statistics_;
    
    // Internal systems
    std::unique_ptr<DevelopmentalSystem> developmentalSystem_;
    double accumulatedTime_ = 0.0;
};

// Built-in structural plasticity plugin
class StructuralPlasticityPlugin : public PluginInterface {
public:
    StructuralPlasticityPlugin() {
        metadata_.id = "structural_plasticity";
        metadata_.name = "Structural Plasticity Plugin";
        metadata_.version = "1.0.0";
        metadata_.description = "Implements synaptogenesis and pruning";
        metadata_.author = "NLM Development Team";
        metadata_.email = "support@nlm.org";
        metadata_.website = "https://nlm.org";
        metadata_.license = "MIT";
        
        metadata_.capabilities.providesBrainModification = true;
        metadata_.capabilities.supportsEventDriven = true;
        
        metadata_.enabledByDefault = true;
        
        // Dependencies
        metadata_.dependencies["brain"] = "core";
    }
    
    ~StructuralPlasticityPlugin() override {
        shutdown();
    }
    
    // Plugin interface implementations (similar to previous plugins)
    PluginId getId() const override { return metadata_.id; }
    std::string getName() const override { return metadata_.name; }
    std::string getVersion() const override { return metadata_.version; }
    std::string getDescription() const override { return metadata_.description; }
    
    bool initialize(std::shared_ptr<Brain> brain,
                   std::shared_ptr<AgentBrain> agentBrain,
                   std::shared_ptr<Config> config) override {
        brain_ = brain;
        agentBrain_ = agentBrain;
        config_ = config;
        
        // Initialize parameters
        synaptogenesisRate_ = config_->getOr<float>("plugin.structural_plasticity.synaptogenesis_rate", 0.0001f);
        pruningRate_ = config_->getOr<float>("plugin.structural_plasticity.pruning_rate", 0.00001f);
        structuralPlasticityEnabled_ = config_->getOr<bool>("plugin.structural_plasticity.enabled", true);
        
        // Register configuration parameters
        config_->set("plugin.structural_plasticity.synaptogenesis_rate", synaptogenesisRate_, nlm::ConfigSource::Runtime);
        config_->set("plugin.structural_plasticity.pruning_rate", pruningRate_, nlm::ConfigSource::Runtime);
        config_->set("plugin.structural_plasticity.enabled", structuralPlasticityEnabled_, nlm::ConfigSource::Runtime);
        
        // Register event handlers
        registerEventHandler("brain_initialized",
                           [this](const PluginEventData& event) { onBrainInitialized(event); });
        registerEventHandler("simulation_step",
                           [this](const PluginEventData& event) { onSimulationStep(event); });
        
        // Initialize structural plasticity system
        initializeStructuralPlasticitySystem();
        
        return true;
    }
    
    void shutdown() override {
        structuralPlasticitySystem_.reset();
        brain_.reset();
        agentBrain_.reset();
        config_.reset();
    }
    
    void update(double timestep) override {
        if (!enabled_ || !brain_ || !agentBrain_) return;
        
        // Update structural plasticity system
        updateStructuralPlasticitySystem(timestep);
        
        // Apply structural changes
        applyStructuralChanges();
        
        // Log activity
        if (debug_mode_) {
            logDebug("Structural plasticity plugin updated: synaptogenesis " +
                    std::to_string(synaptogenesisRate_) + ", pruning " +
                    std::to_string(pruningRate_) + ")");
        }
    }
    
    void onEvent(const PluginEventData& event) override {
        if (event.eventName == "brain_initialized") {
            onBrainInitialized(event);
        } else if (event.eventName == "simulation_step") {
            onSimulationStep(event);
        }
    }
    
    bool isEnabled() const override {
        return enabled_;
    }
    
    void enable() override {
        enabled_ = true;
        logDebug("Structural plasticity plugin enabled");
    }
    
    void disable() override {
        enabled_ = false;
        logDebug("Structural plasticity plugin disabled");
    }
    
    bool canLoad() const override {
        return true;
    }
    
    bool canUnload() const override {
        return true;
    }
    
    PluginCapabilities getCapabilities() const override {
        return metadata_.capabilities;
    }
    
    PluginStatistics getStatistics() const override {
        return statistics_;
    }
    
    void setParameter(const std::string& key, const std::string& value) override {
        if (key == "plugin.structural_plasticity.synaptogenesis_rate") {
            synaptogenesisRate_ = std::stof(value);
            config_->set(key, synaptogenesisRate_, nlm::ConfigSource::Runtime);
        } else if (key == "plugin.structural_plasticity.pruning_rate") {
            pruningRate_ = std::stof(value);
            config_->set(key, pruningRate_, nlm::ConfigSource::Runtime);
        } else if (key == "plugin.structural_plasticity.enabled") {
            structuralPlasticityEnabled_ = (value == "true" || value == "1");
            config_->set(key, structuralPlasticityEnabled_, nlm::ConfigSource::Runtime);
        }
    }
    
    std::string getParameter(const std::string& key) const override {
        if (key == "plugin.structural_plasticity.synaptogenesis_rate") {
            return std::to_string(synaptogenesisRate_);
        } else if (key == "plugin.structural_plasticity.pruning_rate") {
            return std::to_string(pruningRate_);
        } else if (key == "plugin.structural_plasticity.enabled") {
            return structuralPlasticityEnabled_ ? "true" : "false";
        }
        return "";
    }
    
    std::map<std::string, std::string> getParameters() const override {
        std::map<std::string, std::string> params;
        params["plugin.structural_plasticity.synaptogenesis_rate"] = std::to_string(synaptogenesisRate_);
        params["plugin.structural_plasticity.pruning_rate"] = std::to_string(pruningRate_);
        params["plugin.structural_plasticity.enabled"] = structuralPlasticityEnabled_ ? "true" : "false";
        return params;
    }
    
    std::vector<std::string> getRequiredPlugins() const override {
        return metadata_.dependencies;
    }
    
    std::vector<std::string> getOptionalPlugins() const override {
        return std::vector<std::string>();
    }
    
private:
    void initializeStructuralPlasticitySystem() {
        // Initialize structural plasticity system parameters
        structuralPlasticitySystem_.reset(new StructuralPlasticitySystem());
        
        structuralPlasticitySystem_->setSynaptogenesisRate(synaptogenesisRate_);
        structuralPlasticitySystem_->setPruningRate(pruningRate_);
        structuralPlasticitySystem_->setEnabled(structuralPlasticityEnabled_);
        
        // Set up structural plasticity monitoring
        structuralPlasticityMonitoringActive_ = true;
    }
    
    void updateStructuralPlasticitySystem(double timestep) {
        if (!structuralPlasticitySystem_) return;
        
        // Update structural plasticity system
        structuralPlasticitySystem_->update(timestep);
        
        // Apply structural changes
        applyStructuralChanges();
    }
    
    void applyStructuralChanges() {
        if (!brain_ || !structuralPlasticitySystem_) return;
        
        // Get structural change statistics
        auto stats = structuralPlasticitySystem_->getStatistics();
        
        // Apply synaptogenesis (new synapse formation)
        if (stats.synaptogenesisEvents > 0) {
            std::cout << "[STRUCTURAL PLASTICITY] Synaptogenesis: " 
                      << stats.synaptogenesisEvents << " new synapses formed" << std::endl;
        }
        
        // Apply pruning (synapse elimination)
        if (stats.pruningEvents > 0) {
            std::cout << "[STRUCTURAL PLASTICITY] Pruning: " 
                      << stats.pruningEvents << " synapses eliminated" << std::endl;
        }
        
        // Update brain based on structural changes
        // In a real implementation, this would:
        // 1. Modify neural connectivity
        // 2. Adjust plasticity parameters
        // 3. Update brain state
        
        if (debug_mode_) {
            logDebug("Structural changes applied: synaptogenesis=" +
                    std::to_string(stats.synaptogenesisEvents) +
                    ", pruning=" + std::to_string(stats.pruningEvents));
        }
    }
    
    void onBrainInitialized(const PluginEventData& event) {
        logDebug("Structural plasticity plugin: Brain initialized event received");
        
        // Initialize structural plasticity monitoring
        structuralPlasticityMonitoringActive_ = true;
        
        // Initialize statistics
        statistics_.loadCount++;
        statistics_.lastLoad = std::chrono::system_clock::now();
    }
    
    void onSimulationStep(const PluginEventData& event) {
        // Process simulation step event
        if (structuralPlasticityMonitoringActive_) {
            updateStructuralPlasticitySystem(0.001); // Default timestep
        }
    }
    
    void logDebug(const std::string& message) {
        if (debug_mode_) {
            std::cout << "[STRUCTURAL PLASTICITY] " << message << std::endl;
        }
    }
    
    struct StructuralPlasticitySystem {
        void setSynaptogenesisRate(float rate) {
            synaptogenesisRate_ = rate;
        }
        
        void setPruningRate(float rate) {
            pruningRate_ = rate;
        }
        
        void setEnabled(bool enabled) {
            enabled_ = enabled;
        }
        
        void update(double timestep) {
            if (!enabled_) return;
            
            // Update synaptogenesis
            synaptogenesisEvents_ += static_cast<size_t>(synaptogenesisRate_ * timestep);
            
            // Update pruning
            pruningEvents_ += static_cast<size_t>(pruningRate_ * timestep);
            
            // Apply changes to brain
            // This is a simplified implementation
            // In a real system, this would modify actual neural connections
        }
        
        void addSynaptogenesisEvent() {
            synaptogenesisEvents_++;
        }
        
        void addPruningEvent() {
            pruningEvents_++;
        }
        
        struct Statistics {
            size_t synaptogenesisEvents = 0;
            size_t pruningEvents = 0;
            double totalPlasticityChange = 0.0;
            double averageSynapticStrength = 0.5;
        };
        
        Statistics getStatistics() const {
            return stats_;
        }
        
        void onSynaptogenesis() {
            stats_.synaptogenesisEvents++;
            stats_.totalPlasticityChange += synaptogenesisRate_;
            stats_.averageSynapticStrength = (stats_.averageSynapticStrength * (synaptogenesisEvents_ - 1) + 1.0) / synaptogenesisEvents_;
        }
        
        void onPruning() {
            stats_.pruningEvents++;
            stats_.totalPlasticityChange -= pruningRate_;
            stats_.averageSynapticStrength = (stats_.averageSynapticStrength * (pruningEvents_ - 1) + 0.1) / pruningEvents_;
        }
        
    private:
        float synaptogenesisRate_ = 0.0001f;
        float pruningRate_ = 0.00001f;
        bool enabled_ = true;
        Statistics stats_;
    };
    
    PluginMetadata metadata_;
    std::shared_ptr<Brain> brain_;
    std::shared_ptr<AgentBrain> agentBrain_;
    std::shared_ptr<Config> config_;
    
    // Plugin state
    bool enabled_ = false;
    bool debug_mode_ = false;
    bool structuralPlasticityMonitoringActive_ = false;
    
    // Plugin parameters
    float synaptogenesisRate_ = 0.0001f;
    float pruningRate_ = 0.00001f;
    
    // Plugin statistics
    PluginStatistics statistics_;
    
    // Internal systems
    std::unique_ptr<StructuralPlasticitySystem> structuralPlasticitySystem_;
};

// System plugin (core functionality)
class SystemPlugin : public PluginInterface {
public:
    SystemPlugin() {
        metadata_.id = "system";
        metadata_.name = "System Plugin";
        metadata_.version = "1.0.0";
        metadata_.description = "Core system plugin for NLM functionality";
        metadata_.author = "NLM Development Team";
        metadata_.email = "support@nlm.org";
        metadata_.website = "https://nlm.org";
        metadata_.license = "MIT";
        
        metadata_.capabilities.providesBrainModification = true;
        metadata_.capabilities.providesNeuromodulation = true;
        metadata_.capabilities.providesSensoryProcessing = true;
        metadata_.capabilities.providesMotorControl = true;
        metadata_.capabilities.supportsRealTimeUpdates = true;
        metadata_.capabilities.threadSafe = true;
        
        metadata_.enabledByDefault = true;
        
        // No dependencies for system plugin
    }
    
    ~SystemPlugin() override {
        shutdown();
    }
    
    // Plugin interface implementations (similar to previous plugins)
    PluginId getId() const override { return metadata_.id; }
    std::string getName() const override { return metadata_.name; }
    std::string getVersion() const override { return metadata_.version; }
    std::string getDescription() const override { return metadata_.description; }
    
    bool initialize(std::shared_ptr<Brain> brain,
                   std::shared_ptr<AgentBrain> agentBrain,
                   std::shared_ptr<Config> config) override {
        brain_ = brain;
        agentBrain_ = agentBrain;
        config_ = config;
        
        // Register event handlers
        registerEventHandler("brain_initialized",
                           [this](const PluginEventData& event) { onBrainInitialized(event); });
        registerEventHandler("simulation_step",
                           [this](const PluginEventData& event) { onSimulationStep(event); });
        
        // Initialize system functionality
        initializeSystemFunctionality();
        
        return true;
    }
    
    void shutdown() override {
        systemFunctionality_.reset();
        brain_.reset();
        agentBrain_.reset();
        config_.reset();
    }
    
    void update(double timestep) override {
        if (!enabled_ || !brain_ || !agentBrain_) return;
        
        // Update system functionality
        updateSystemFunctionality(timestep);
        
        // Log activity
        if (debug_mode_) {
            logDebug("System plugin updated");
        }
    }
    
    void onEvent(const PluginEventData& event) override {
        if (event.eventName == "brain_initialized") {
            onBrainInitialized(event);
        } else if (event.eventName == "simulation_step") {
            onSimulationStep(event);
        }
    }
    
    bool isEnabled() const override {
        return enabled_;
    }
    
    void enable() override {
        enabled_ = true;
        logDebug("System plugin enabled");
    }
    
    void disable() override {
        enabled_ = false;
        logDebug("System plugin disabled");
    }
    
    bool canLoad() const override {
        return true;
    }
    
    bool canUnload() const override {
        return true;
    }
    
    PluginCapabilities getCapabilities() const override {
        return metadata_.capabilities;
    }
    
    PluginStatistics getStatistics() const override {
        return statistics_;
    }
    
    void setParameter(const std::string& key, const std::string& value) override {
        // System plugin handles configuration
        if (key == "system.version") {
            metadata_.version = value;
        } else if (key == "system.debug") {
            debug_mode_ = (value == "true" || value == "1");
        }
        
        // Update config
        config_->set(key, value, nlm::ConfigSource::Runtime);
    }
    
    std::string getParameter(const std::string& key) const override {
        if (key == "system.version") {
            return metadata_.version;
        } else if (key == "system.debug") {
            return debug_mode_ ? "true" : "false";
        }
        return "";
    }
    
    std::map<std::string, std::string> getParameters() const override {
        std::map<std::string, std::string> params;
        params["system.version"] = metadata_.version;
        params["system.debug"] = debug_mode_ ? "true" : "false";
        return params;
    }
    
    std::vector<std::string> getRequiredPlugins() const override {
        return std::vector<std::string>();
    }
    
    std::vector<std::string> getOptionalPlugins() const override {
        return std::vector<std::string>();\n    private:
    void initializeSystemFunctionality() {
        // Initialize system functionality
        systemFunctionality_.reset(new SystemFunctionality());
        
        systemFunctionality_->setDebugMode(debug_mode_);
        systemFunctionality_->initialize(brain_, agentBrain_, config_);
        
        systemFunctionalityMonitoringActive_ = true;
    }
    
    void updateSystemFunctionality(double timestep) {
        if (!systemFunctionality_) return;
        
        // Update system functionality
        systemFunctionality_->update(timestep);
        
        // Apply system functionality to brain
        applySystemFunctionality();
    }
    
    void applySystemFunctionality() {
        if (!brain_ || !systemFunctionality_) return;
        
        // Apply system functionality to brain
        // This would include:
        // 1. System-wide configuration
        // 2. Global statistics
        // 3. System monitoring
        // 4. Error handling
        
        if (debug_mode_) {
            logDebug("System functionality applied");
        }
    }
    
    void onBrainInitialized(const PluginEventData& event) {
        logDebug("System plugin: Brain initialized event received");
        
        // Initialize system monitoring
        systemFunctionalityMonitoringActive_ = true;
        
        // Initialize statistics
        statistics_.loadCount++;
        statistics_.lastLoad = std::chrono::system_clock::now();
    }
    
    void onSimulationStep(const PluginEventData& event) {
        // Process simulation step event
        if (systemFunctionalityMonitoringActive_) {
            updateSystemFunctionality(0.001); // Default timestep
        }
    }
    
    void logDebug(const std::string& message) {
        if (debug_mode_) {
            std::cout << "[SYSTEM] " << message << std::endl;
        }
    }
    
    struct SystemFunctionality {
        void setDebugMode(bool enabled) {
            debugMode_ = enabled;
        }
        
        void initialize(std::shared_ptr<Brain> brain,
                       std::shared_ptr<AgentBrain> agentBrain,
                       std::shared_ptr<Config> config) {
            brain_ = brain;
            agentBrain_ = agentBrain;
            config_ = config;
            
            // Initialize system state
            initialized_ = true;
        }
        
        void update(double timestep) {
            if (!initialized_) return;
            
            // Update system state
            // This would include:
            // 1. System monitoring
            // 2. Performance tracking
            // 3. Resource management
            // 4. Error handling
            
            // For demonstration, we'll just log the update
            if (debugMode_) {
                std::cout << "[SYSTEM FUNCTIONALITY] Updated at timestep " 
                          << timestep << std::endl;
            }
        }
        
        bool isInitialized() const {
            return initialized_;
        }
        
    private:
        bool debugMode_ = false;
        bool initialized_ = false;
        std::shared_ptr<Brain> brain_;
        std::shared_ptr<AgentBrain> agentBrain_;
        std::shared_ptr<Config> config_;
    };
    
    PluginMetadata metadata_;
    std::shared_ptr<Brain> brain_;
    std::shared_ptr<AgentBrain> agentBrain_;
    std::shared_ptr<Config> config_;
    
    // Plugin state
    bool enabled_ = false;
    bool debug_mode_ = false;
    bool systemFunctionalityMonitoringActive_ = false;
    
    // Plugin statistics
    PluginStatistics statistics_;
    
    // Internal systems
    std::unique_ptr<SystemFunctionality> systemFunctionality_;
};

// Default plugin registry implementation
DefaultPluginRegistry::DefaultPluginRegistry() {
    setupDefaultConfiguration();
    
    // Register built-in plugins
    registerBuiltInPlugins();
}

void DefaultPluginRegistry::registerBuiltInPlugins() {
    // Create plugin metadata
    PluginMetadata curiosityMetadata;
    curiosityMetadata.id = "curiosity";
    curiosityMetadata.name = "Curiosity Plugin";
    curiosityMetadata.version = "1.0.0";
    curiosityMetadata.description = "Implements curiosity-driven exploration behavior";
    curiosityMetadata.author = "NLM Development Team";
    curiosityMetadata.email = "support@nlm.org";
    curiosityMetadata.website = "https://nlm.org";
    curiosityMetadata.license = "MIT";
    curiosityMetadata.enabledByDefault = true;
    curiosityMetadata.dependencies = {"brain", "agent_brain"};
    curiosityMetadata.capabilities.providesNeuromodulation = true;
    curiosityMetadata.capabilities.providesCognitiveFunctions = true;
    curiosityMetadata.capabilities.supportsEventDriven = true;
    
    // Register curiosity plugin factory
    auto curiosityFactory = []() -> std::unique_ptr<PluginInterface> {
        return std::make_unique<CuriosityPlugin>();
    };
    registerPlugin(curiosityMetadata, curiosityFactory);
    
    // Create reward plugin metadata
    PluginMetadata rewardMetadata;
    rewardMetadata.id = "reward";
    rewardMetadata.name = "Reward Plugin";
    rewardMetadata.version = "1.0.0";
    rewardMetadata.description = "Implements reward-based learning and reinforcement";
    rewardMetadata.author = "NLM Development Team";
    rewardMetadata.email = "support@nlm.org";
    rewardMetadata.website = "https://nlm.org";
    rewardMetadata.license = "MIT";
    rewardMetadata.enabledByDefault = true;
    rewardMetadata.dependencies = {"brain", "agent_brain"};
    rewardMetadata.capabilities.providesNeuromodulation = true;
    rewardMetadata.capabilities.providesBrainModification = true;
    
    // Register reward plugin factory
    auto rewardFactory = []() -> std::unique_ptr<PluginInterface> {
        return std::make_unique<RewardPlugin>();
    };
    registerPlugin(rewardMetadata, rewardFactory);
    
    // Create developmental plugin metadata
    PluginMetadata developmentalMetadata;
    developmentalMetadata.id = "developmental";
    developmentalMetadata.name = "Developmental Plugin";
    developmentalMetadata.version = "1.0.0";
    developmentalMetadata.description = "Implements developmental stages and maturation";
    developmentalMetadata.author = "NLM Development Team";
    developmentalMetadata.email = "support@nlm.org";
    developmentalMetadata.website = "https://nlm.org";
    developmentalMetadata.license = "MIT";
    developmentalMetadata.enabledByDefault = true;
    developmentalMetadata.dependencies = {"brain"};
    developmentalMetadata.capabilities.providesDevelopmentalEffects = true;
    developmentalMetadata.capabilities.providesBrainModification = true;
    developmentalMetadata.capabilities.supportsEventDriven = true;
    
    // Register developmental plugin factory
    auto developmentalFactory = []() -> std::unique_ptr<PluginInterface> {
        return std::make_unique<DevelopmentalPlugin>();
    };
    registerPlugin(developmentalMetadata, developmentalFactory);
    
    // Create structural plasticity plugin metadata
    PluginMetadata structuralMetadata;
    structuralMetadata.id = "structural_plasticity";
    structuralMetadata.name = "Structural Plasticity Plugin";
    structuralMetadata.version = "1.0.0";
    structuralMetadata.description = "Implements synaptogenesis and pruning";
    structuralMetadata.author = "NLM Development Team";
    structuralMetadata.email = "support@nlm.org";
    structuralMetadata.website = "https://nlm.org";
    structuralMetadata.license = "MIT";
    structuralMetadata.enabledByDefault = true;
    structuralMetadata.dependencies = {"brain"};
    structuralMetadata.capabilities.providesBrainModification = true;
    structuralMetadata.capabilities.supportsEventDriven = true;
    
    // Register structural plasticity plugin factory
    auto structuralFactory = []() -> std::unique_ptr<PluginInterface> {
        return std::make_unique<StructuralPlasticityPlugin>();
    };
    registerPlugin(structuralMetadata, structuralFactory);
    
    // Create system plugin metadata
    PluginMetadata systemMetadata;
    systemMetadata.id = "system";
    systemMetadata.name = "System Plugin";
    systemMetadata.version = "1.0.0";
    systemMetadata.description = "Core system plugin for NLM functionality";
    systemMetadata.author = "NLM Development Team";
    systemMetadata.email = "support@nlm.org";
    systemMetadata.website = "https://nlm.org";
    systemMetadata.license = "MIT";
    systemMetadata.enabledByDefault = true;
    systemMetadata.dependencies = {};
    systemMetadata.capabilities.providesBrainModification = true;
    systemMetadata.capabilities.providesNeuromodulation = true;
    systemMetadata.capabilities.providesSensoryProcessing = true;
    systemMetadata.capabilities.providesMotorControl = true;
    systemMetadata.capabilities.supportsRealTimeUpdates = true;
    systemMetadata.capabilities.threadSafe = true;
    
    // Register system plugin factory
    auto systemFactory = []() -> std::unique_ptr<PluginInterface> {
        return std::make_unique<SystemPlugin>();
    };
    registerPlugin(systemMetadata, systemFactory);
}

// Singleton instance
PluginRegistry& getPluginRegistry() {
    static DefaultPluginRegistry registry;
    return registry;
}

// Convenience functions
bool loadPlugin(const std::string& pluginId) {
    return getPluginRegistry().loadPlugin(pluginId);
}

bool unloadPlugin(const std::string& pluginId) {
    return getPluginRegistry().unloadPlugin(pluginId);
}

bool enablePlugin(const std::string& pluginId) {
    return getPluginRegistry().enablePlugin(pluginId);
}

bool disablePlugin(const std::string& pluginId) {
    return getPluginRegistry().disablePlugin(pluginId);
}

bool togglePlugin(const std::string& pluginId) {
    return getPluginRegistry().togglePlugin(pluginId);
}

std::vector<std::string> getAvailablePlugins() {
    return getPluginRegistry().getAvailablePlugins();
}

std::vector<std::string> getEnabledPlugins() {
    return getPluginRegistry().getEnabledPlugins();
}

bool broadcastEvent(const PluginEventData& event) {
    getPluginRegistry().broadcastEvent(event);
    return true;
}

void setBrain(std::shared_ptr<Brain> brain) {
    getPluginRegistry().setBrain(brain);
}

void setAgentBrain(std::shared_ptr<AgentBrain> agentBrain) {
    getPluginRegistry().setAgentBrain(agentBrain);
}

void setGlobalConfig(std::shared_ptr<Config> config) {
    getPluginRegistry().setGlobalConfig(config);
}

void initializePlugins() {
    // Initialize all enabled plugins
    auto enabledPlugins = getPluginRegistry().getEnabledPlugins();
    for (const auto& pluginId : enabledPlugins) {
        getPluginRegistry().initializePlugin(pluginId);
    }
}

void shutdownPlugins() {
    getPluginRegistry().shutdownAllPlugins();
}

void updatePlugins(double timestep) {
    // Update all plugins
    auto enabledPlugins = getPluginRegistry().getEnabledPlugins();
    for (const auto& pluginId : enabledPlugins) {
        getPluginRegistry().updatePlugin(pluginId, timestep);
    }
}

} // namespace nlm
