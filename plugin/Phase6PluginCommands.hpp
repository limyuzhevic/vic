# NLM/熙然 Phase 6 Commands and Configuration

This file contains Phase 6 specific commands for integrating plugins and managing advanced configuration.

// Phase 6 Plugin Commands
// Commands for managing plugins in Phase 6 integration

#ifndef NLM_PHASE6_COMMANDS_H
#define NLM_PHASE6_COMMANDS_H

#include <string>
#include <vector>
#include <memory>
#include <nlohmann/json.hpp>
#include "plugin/PluginSystem.hpp"

namespace nlm {

// Phase 6 Plugin Commands
class Phase6PluginCommands {
public:
    Phase6PluginCommands(std::shared_ptr<PluginManager> pluginManager, std::shared_ptr<Config> config);
    ~Phase6PluginCommands() = default;
    
    // Plugin management commands
    bool loadPhase6Plugin(const std::string& pluginId, const nlohmann::json& config);
    bool unloadPhase6Plugin(const std::string& pluginId);
    bool initializePhase6Plugins();
    bool shutdownPhase6Plugins();
    
    // Plugin discovery and registration
    std::vector<nlohmann::json> discoverPhase6Plugins() const;
    bool registerPhase6PluginFactory(const std::string& pluginId, const nlohmann::json& factoryConfig);
    
    // Integration commands
    bool integrateWorkingMemory() const;
    bool integrateEpisodicMemory() const;
    bool integrateNeuromodulation() const;
    bool integratePredictionSystem() const;
    bool connectPlannerToActionSelection() const;
    bool enhanceRecurrentConnectivity(float strength, float delay) const;
    bool implementSynapticScaling() const;
    bool implementMetaplasticity(float rate, const nlohmann::json& history) const;
    
    // Configuration commands
    bool configurePhase6(const nlohmann::json& phase6Config);
    bool getPhase6Status() const;
    bool exportPhase6Configuration(const std::string& filepath) const;
    bool importPhase6Configuration(const std::string& filepath);
    
    // Statistics and monitoring
    nlohmann::json getPluginStatistics() const;
    nlohmann::json getIntegrationStatus() const;
    bool monitorPluginPerformance(const std::string& pluginId) const;
    
    // Plugin lifecycle
    bool restartPlugin(const std::string& pluginId);
    bool pausePlugin(const std::string& pluginId);
    bool resumePlugin(const std::string& pluginId);
    std::vector<std::string> getActivePlugins() const;
    
    // Event handling
    void registerEventHandler(const std::string& eventName, std::function<void(const std::string&, const nlohmann::json&)> handler);
    bool triggerPluginEvent(const std::string& eventName, const std::string& sourcePluginId, const nlohmann::json& data);
    
    // Validation and health checks
    bool validatePluginConfiguration(const std::string& pluginId, const nlohmann::json& config) const;
    bool validatePluginHealth(const std::string& pluginId) const;
    std::vector<std::string> getPluginHealthIssues() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Phase 6 Configuration Schema
struct Phase6ConfigurationSchema {
    // Memory System Configuration
    struct MemoryConfig {
        float workingMemoryDecayRate = 0.1f;
        float workingMemoryMaxCapacity = 50.0f;
        size_t episodicMemoryCaptureInterval = 10;
        size_t episodicMemoryReplayInterval = 100;
        size_t episodicMemoryConsolidationInterval = 1000;
        float memoryConsolidationRate = 0.5f;
        float memoryRetentionThreshold = 0.3f;
    } memory;
    
    // Neuromodulation Configuration
    struct NeuromodulationConfig {
        struct DopamineConfig {
            float learningRate = 0.01f;
            float predictionErrorSensitivity = 1.0f;
            float baselineLevel = 0.1f;
            float phasicStrength = 1.0f;
        } dopamine;
        
        struct NoveltyConfig {
            float sensitivity = 0.5f;
            float decayRate = 0.1f;
            float baseline = 0.05f;
        } novelty;
        
        struct CuriosityConfig {
            float explorationDrive = 0.5f;
            float noveltyWeight = 0.7f;
            float predictionErrorWeight = 0.3f;
            float decayRate = 0.05f;
        } curiosity;
    } neuromodulation;
    
    // Synaptic Plasticity Configuration
    struct PlasticityConfig {
        float stdpLTPWeight = 1.0f;
        float stdpLTDWeight = 0.8f;
        float stdpTimeConstant = 20.0f;  // ms
        float hebbianLearningRate = 0.01f;
        float synapticScalingTargetRatio = 1.0f;
        float metaplasticChangeRate = 0.001f;
    } plasticity;
    
    // Cognition Configuration
    struct CognitionConfig {
        bool plannerEnabled = false;
        bool conceptFormationEnabled = false;
        bool selfModelEnabled = false;
        bool socialLearningEnabled = false;
        float planningDepth = 3;
        float conceptStabilityThreshold = 0.7f;
    } cognition;
    
    // Development Configuration
    struct DevelopmentConfig {
        bool developmentEnabled = false;
        std::string developmentalStage = "adult";
        float plasticityRateModulation = 1.0f;
        float structuralPlasticityRate = 0.01f;
        float pubertyAge = 100.0f;  // simulation steps
    } development;
    
    // Integration Configuration
    struct IntegrationConfig {
        bool enableMemoryIntegration = true;
        bool enableNeuromodulationIntegration = true;
        bool enablePredictionIntegration = true;
        bool enableCognitionIntegration = true;
        bool enableDevelopmentIntegration = true;
        float integrationStrength = 1.0f;
        size_t integrationCheckInterval = 100;
    } integration;
    
    // Performance Configuration
    struct PerformanceConfig {
        bool enablePerformanceMonitoring = true;
        size_t pluginCheckInterval = 1000;
        float performanceThreshold = 0.8f;
        bool enableAutoRecovery = true;
        float recoveryThreshold = 0.5f;
    } performance;
    
    // Plugin Configuration
    struct PluginConfig {
        std::string pluginDirectory = "plugins/";
        bool enablePluginHotReload = true;
        bool validatePluginManifests = true;
        size_t maxConcurrentPlugins = 10;
        float pluginLoadTimeout = 30.0f;  // seconds
        bool enablePluginLogging = true;
        std::string pluginLogLevel = "INFO";
    } plugin;
    
    // Command Configuration
    struct CommandConfig {
        bool enableCommandLogging = true;
        bool enableCommandValidation = true;
        size_t commandTimeout = 5000;  // ms
        std::string commandLogFile = "phase6_commands.log";
    } command;
    
    // Statistics Configuration
    struct StatisticsConfig {
        bool enableStatisticsCollection = true;
        size_t statisticsInterval = 1000;
        bool exportStatistics = true;
        std::string statisticsFile = "phase6_statistics.json";
        std::string statisticsFormat = "json";
    } statistics;
    
    // Health Check Configuration
    struct HealthCheckConfig {
        bool enableHealthChecks = true;
        size_t healthCheckInterval = 500;
        float healthCheckTimeout = 100.0f;  // ms
        bool enableAutoRestart = false;
        float restartThreshold = 0.8f;
    } health;
};

// Phase 6 Command Line Interface
class Phase6CLI {
public:
    Phase6CLI(std::shared_ptr<Phase6PluginCommands> commands, std::shared_ptr<Config> config);
    ~Phase6CLI() = default;
    
    // Parse and execute command line arguments
    bool execute(int argc, char** argv);
    
    // Command handlers
    bool handlePluginCommands(const std::vector<std::string>& args);
    bool handleIntegrationCommands(const std::vector<std::string>& args);
    bool handleConfigurationCommands(const std::vector<std::string>& args);
    bool handleStatisticsCommands(const std::vector<std::string>& args);
    bool handleHealthCommands(const std::vector<std::string>& args);
    
    // Configuration file handling
    bool loadPhase6Configuration(const std::string& filepath);
    bool savePhase6Configuration(const std::string& filepath) const;
    
    // Plugin discovery and listing
    void listPlugins() const;
    void listPluginFactories() const;
    
    // Integration status
    void showIntegrationStatus() const;
    void showPluginStatus(const std::string& pluginId) const;
    
    // Help and documentation
    void showHelp() const;
    void showPluginHelp() const;
    void showIntegrationHelp() const;
    void showConfigurationHelp() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Plugin Manifest Generator
class PluginManifestGenerator {
public:
    static nlohmann::json generateManifest(const PluginManifest& manifest);
    static std::string generatePluginDescription(const PluginManifest& manifest);
    static nlohmann::json generatePluginSchema(const PluginManifest& manifest);
    static std::string generatePluginCode(const PluginManifest& manifest, const nlohmann::json& config);
    
    static PluginManifest createWorkingMemoryEnhancementPlugin();
    static PluginManifest createNeuromodulationIntegrationPlugin();
    static PluginManifest createSynapticScalingPlugin();
    static PluginManifest createAdvancedCognitionPlugin();
    static PluginManifest createDevelopmentEnhancementPlugin();
    static PluginManifest createMetaplasticityPlugin();
    static PluginManifest createRecurrentConnectivityPlugin();
    static PluginManifest createPlannerIntegrationPlugin();
    static PluginManifest createPredictionIntegrationPlugin();
    
    static std::vector<PluginManifest> getDefaultPhase6Plugins();
};

} // namespace nlm

#endif // NLM_PHASE6_COMMANDS_H
