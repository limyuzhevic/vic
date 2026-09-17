/*
 * NLM/熙然 Plugin System v6.0
 * 
 * Plugin Architecture for Phase 6 Integration
 * 
 * This plugin system allows users to extend and customize NLM/熙然
 * with modular components following SOLID principles.
 */

#ifndef NLM_PLUGIN_SYSTEM_H
#define NLM_PLUGIN_SYSTEM_H

#include <string>
#include <map>
#include <vector>
#include <memory>
#include <functional>
#include <unordered_map>
#include <mutex>
#include <typeinfo>
#include <functional>
#include <nlohmann/json.hpp>
#include "core/Config/Config.hpp"

namespace nlm {

// Plugin System Version
#define PLUGIN_SYSTEM_VERSION "6.0.0"

// Plugin Interface - Core abstraction for all plugins
class IPlugin {
public:
    virtual ~IPlugin() = default;
    
    // Get plugin name
    virtual std::string getName() const = 0;
    
    // Get plugin version
    virtual std::string getVersion() const = 0;
    
    // Get plugin description
    virtual std::string getDescription() const = 0;
    
    // Get plugin author
    virtual std::string getAuthor() const = 0;
    
    // Get plugin dependencies
    virtual std::vector<std::string> getDependencies() const = 0;
    
    // Initialize plugin
    virtual bool initialize(const std::shared_ptr<Config>& config) = 0;
    
    // Shutdown plugin
    virtual void shutdown() = 0;
    
    // Check if plugin is initialized
    virtual bool isInitialized() const = 0;
    
    // Get plugin capabilities
    virtual std::vector<std::string> getCapabilities() const = 0;
    
    // Execute plugin command
    virtual bool executeCommand(const std::string& command, const std::vector<std::string>& args) = 0;
    
    // Get plugin metadata
    virtual nlohmann::json getMetadata() const = 0;
    
    // Validate plugin configuration
    virtual bool validateConfig(const nlohmann::json& config) const = 0;
};

// PluginFactory creates plugins based on plugin IDs
class IPluginFactory {
public:
    virtual ~IPluginFactory() = default;
    virtual std::unique_ptr<IPlugin> createPlugin() = 0;
    virtual std::string getPluginId() const = 0;
    virtual nlohmann::json getPluginInfo() const = 0;
    virtual bool canCreate(const nlohmann::json& config) const = 0;
};

// PluginManager manages all plugins
class PluginManager {
public:
    PluginManager(std::shared_ptr<Config> config);
    ~PluginManager();
    
    // Disable copying
    PluginManager(const PluginManager&) = delete;
    PluginManager& operator=(const PluginManager&) = delete;
    
    // Plugin loading
    bool loadPlugin(const std::string& pluginPath);
    bool loadPlugin(const std::string& pluginId, const nlohmann::json& config);
    bool unloadPlugin(const std::string& pluginId);
    
    // Plugin management
    bool initializeAllPlugins();
    void shutdownAllPlugins();
    bool isPluginLoaded(const std::string& pluginId) const;
    bool isPluginInitialized(const std::string& pluginId) const;
    
    // Plugin queries
    std::vector<std::string> getLoadedPluginIds() const;
    std::vector<std::string> getInitializedPluginIds() const;
    nlohmann::json getPluginMetadata(const std::string& pluginId) const;
    nlohmann::json getAllPluginMetadata() const;
    
    // Command execution
    bool executePluginCommand(const std::string& pluginId, const std::string& command, const std::vector<std::string>& args);
    bool executeGlobalCommand(const std::string& command, const std::vector<std::string>& args);
    
    // Configuration management
    bool updatePluginConfig(const std::string& pluginId, const nlohmann::json& config);
    nlohmann::json getPluginConfig(const std::string& pluginId) const;
    nlohmann::json getGlobalConfig() const;
    
    // Statistics
    struct PluginStats {
        std::string pluginId;
        std::string name;
        std::string version;
        bool initialized;
        std::chrono::system_clock::time_point loadTime;
        std::chrono::system_clock::time_point lastActivity;
        uint64_t callsExecuted;
        std::vector<std::string> capabilities;
    };
    
    std::vector<PluginStats> getPluginStats() const;
    
    // Event handling
    using EventCallback = std::function<void(const std::string& pluginId, const std::string& eventName, const nlohmann::json& data)>
    
    void registerEventCallback(const std::string& eventName, EventCallback callback);
    bool triggerEvent(const std::string& eventName, const std::string& sourcePluginId, const nlohmann::json& data);
    
    // Discovery
    std::vector<nlohmann::json> discoverAvailablePlugins() const;
    bool registerPluginFactory(std::unique_ptr<IPluginFactory> factory);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Phase 6 specific plugin interfaces

// Memory Integration Plugin
class IMemoryIntegrationPlugin : public virtual IPlugin {
public:
    virtual ~IMemoryIntegrationPlugin() = default;
    
    // Called when memory system is being integrated
    virtual bool integrateMemorySystems() = 0;
    
    // Called when working memory needs enhancement
    virtual bool enhanceWorkingMemory(float decayRate, float maxCapacity) = 0;
    
    // Called when episodic memory needs enhancement
    virtual bool enhanceEpisodicMemory(size_t captureInterval, size_t replayInterval) = 0;
    
    // Called when memory consolidation needs enhancement
    virtual bool enhanceMemoryConsolidation(float consolidationRate, float retentionThreshold) = 0;
};

// Neuromodulation Integration Plugin
class INeuromodulationIntegrationPlugin : public virtual IPlugin {
public:
    virtual ~INeuromodulationIntegrationPlugin() = default;
    
    // Called when neuromodulators need integration
    virtual bool integrateNeuromodulators() = 0;
    
    // Called when dopamine system needs enhancement
    virtual bool enhanceDopamineSystem(float learningRate, float predictionErrorSensitivity) = 0;
    
    // Called when other neuromodulators need implementation
    virtual bool implementNeuromodulator(const std::string& name, const nlohmann::json& parameters) = 0;
};

// Synaptic Scaling Plugin
class ISynapticScalingPlugin : public virtual IPlugin {
public:
    virtual ~ISynapticScalingPlugin() = default;
    
    // Called to implement synaptic scaling
    virtual bool implementSynapticScaling() = 0;
    
    // Called to enhance synaptic homeostasis
    virtual bool enhanceSynapticHomeostasis(float targetExcitationInhibitionRatio) = 0;
    
    // Called to implement metaplasticity
    virtual bool implementMetaplasticity(float metaplasticChangeRate, const nlohmann::json& learningHistory) = 0;
};

// Advanced Cognition Plugin
class IAdvancedCognitionPlugin : public virtual IPlugin {
public:
    virtual ~IAdvancedCognitionPlugin() = default;
    
    // Called to connect planner to action selection
    virtual bool connectPlannerToActionSelection() = 0;
    
    // Called to enhance recurrent connectivity
    virtual bool enhanceRecurrentConnectivity(float strength, float delay) = 0;
    
    // Called to connect concept formation
    virtual bool connectConceptFormation() = 0;
    
    // Called to connect self-model
    virtual bool connectSelfModel() = 0;
    
    // Called to connect social learning
    virtual bool connectSocialLearning() = 0;
};

// Development Enhancement Plugin
class IDevelopmentEnhancementPlugin : public virtual IPlugin {
public:
    virtual ~IDevelopmentEnhancementPlugin() = default;
    
    // Called to enhance developmental stages
    virtual bool enhanceDevelopmentStages(const std::vector<std::string>& stageOrder, const nlohmann::json& stageParams) = 0;
    
    // Called to enhance developmental plasticity
    virtual bool enhanceDevelopmentalPlasticity(float plasticityRate, float structuralPlasticityRate) = 0;
    
    // Called to add puberty-like transition
    virtual bool addPubertyTransition(float pubertyAge, const nlohmann::json& pubertyParams) = 0;
};

// Plugin System Utilities

// Plugin manifest for describing plugin metadata
struct PluginManifest {
    std::string id;
    std::string name;
    std::string version;
    std::string description;
    std::string author;
    std::string license;
    std::vector<std::string> dependencies;
    std::vector<std::string> capabilities;
    nlohmann::json configSchema;
    std::string entryPoint;
    std::string platform;
    std::vector<std::string> tags;
    bool requiresRestart;
    
    PluginManifest() = default;
    PluginManifest(const std::string& id, const std::string& name, const std::string& version,
                  const std::string& description, const std::string& author)
        : id(id), name(name), version(version), description(description), author(author),
          license("MIT"), requiresRestart(false) {}
};

// PluginRegistry manages plugin manifests and loading
class PluginRegistry {
public:
    PluginRegistry(const std::string& pluginDirectory);
    ~PluginRegistry() = default;
    
    // Manifest management
    bool registerManifest(const PluginManifest& manifest);
    bool unregisterManifest(const std::string& pluginId);
    PluginManifest getManifest(const std::string& pluginId) const;
    std::vector<PluginManifest> getAllManifests() const;
    
    // Plugin discovery
    std::vector<std::string> findPlugins(const std::string& pattern = "*") const;
    std::vector<nlohmann::json> discoverPlugins() const;
    
    // Validation
    bool validateManifest(const PluginManifest& manifest) const;
    bool validateConfig(const PluginManifest& manifest, const nlohmann::json& config) const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Global plugin registry instance
extern PluginManager& getGlobalPluginManager();

} // namespace nlm

#endif // NLM_PLUGIN_SYSTEM_H
