# Phase 6 Integration Commands
# Commands for Phase 6 integration and plugin management

#ifndef NLM_PHASE6_INTEGRATION_COMMANDS_H
#define NLM_PHASE6_INTEGRATION_COMMANDS_H

#include "plugin/Phase6PluginCommands.hpp"
#include <string>
#include <vector>
#include <memory>
#include <nlohmann/json.hpp>
#include "core/Config/Config.hpp"
#include "brain/Brain.hpp"
#include "agent/AgentBrain.hpp"
#include "neuromodulation/Neuromodulator.hpp"
#include "plasticity/STDP.hpp"
#include "plasticity/Hebbian.hpp"
#include "plasticity/StructuralPlasticity.hpp"
#include "memory/NeuralWorkingMemory.hpp"
#include "memory/NeuralEpisodicMemory.hpp"
#include "prediction/PredictionSystem.hpp"
#include "cognition/NeuralPlanner.hpp"
#include "cognition/ConceptFormation.hpp"
#include "cognition/SelfModel.hpp"
#include "cognition/SocialLearning.hpp"
#include "development/DevelopmentSystem.hpp"

namespace nlm {

// Phase 6 Integration Commands
// These commands implement the easy wins and advanced integrations for Phase 6

class Phase6IntegrationCommands {
public:
    Phase6IntegrationCommands(std::shared_ptr<Brain> brain,
                             std::shared_ptr<AgentBrain> agentBrain,
                             std::shared_ptr<PluginManager> pluginManager,
                             std::shared_ptr<Config> config);
    ~Phase6IntegrationCommands() = default;
    
    // Easy Wins - Priority 1
    bool connectPlannerToActionSelection();
    bool enhanceRecurrentConnectivity(float strength, float delay);
    
    // Integration Commands - Priority 2
    bool integrateWorkingMemory();
    bool integrateEpisodicMemory();
    bool integrateNeuromodulation();
    bool integratePredictionSystem();
    bool integrateConceptFormation();
    bool integrateSelfModel();
    bool integrateSocialLearning();
    bool integrateDevelopmentSystem();
    
    // Advanced Plasticity - Priority 3
    bool implementSynapticScaling(float targetRatio);
    bool implementMetaplasticity(float rate, const nlohmann::json& learningHistory);
    bool enhanceNeuromodulatoryPlasticity();
    
    // Enhanced Connectivity
    bool enhanceWorkingMemoryRecurrent(float strength);
    bool enhanceEpisodicMemoryReplay(float replayStrength);
    bool enhanceAttentionConnectivity(float attentionStrength);
    bool enhanceMemoryCrossTalk(float crossTalkStrength);
    
    // Developmental Enhancements
    bool enhanceDevelopmentalStages(const std::vector<std::string>& stageOrder);
    bool addPubertyTransition(float pubertyAge, const nlohmann::json& pubertyParams);
    bool enhanceDevelopmentalPlasticity(float plasticityRate);
    
    // System Integration
    bool enableFullNeuromodulatorSet();
    bool integrateFullPredictionSystem();
    bool connectAllCognitionSystems();
    bool enableCompleteDevelopmentPath();
    
    // Plugin Integration
    bool loadPhase6PluginSystem();
    bool initializeAllPhase6Plugins();
    bool configurePhase6Integration();
    
    // Monitoring and Validation
    bool validateIntegrationComplete() const;
    nlohmann::json getIntegrationStatus() const;
    nlohmann::json getIntegrationReport() const;
    bool monitorIntegrationHealth() const;
    
    // Command execution
    bool executeIntegrationCommand(const std::string& command, const std::vector<std::string>& args);
    bool executePluginCommand(const std::string& pluginId, const std::string& command, const std::vector<std::string>& args);
    
    // Configuration management
    bool saveIntegrationConfiguration(const std::string& filepath) const;
    bool loadIntegrationConfiguration(const std::string& filepath);
    bool exportIntegrationConfiguration(const std::string& filepath) const;
    bool importIntegrationConfiguration(const std::string& filepath);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Advanced Configuration Schema for Phase 6
struct Phase6AdvancedConfiguration {
    // Easy Wins Configuration
    struct EasyWinsConfig {
        bool enablePlannerActionSelection = false;
        bool enhanceRecurrentConnectivity = false;
        float recurrentStrength = 0.5f;
        float recurrentDelay = 5.0f;
        
        EasyWinsConfig() = default;
    } easyWins;
    
    // Memory Integration Configuration
    struct MemoryIntegrationConfig {
        bool enableWorkingMemoryIntegration = false;
        bool enableEpisodicMemoryIntegration = false;
        bool enableAssociativeMemoryIntegration = false;
        
        float workingMemoryDecayRate = 0.1f;
        float workingMemoryMaxCapacity = 50.0f;
        
        size_t episodicCaptureInterval = 10;
        size_t episodicReplayInterval = 100;
        size_t episodicConsolidationInterval = 1000;
        
        float memoryConsolidationRate = 0.5f;
        float memoryRetentionThreshold = 0.3f;
        
        MemoryIntegrationConfig() = default;
    } memoryIntegration;
    
    // Neuromodulation Integration Configuration
    struct NeuromodulationIntegrationConfig {
        bool enableDopamineIntegration = false;
        bool enableAcetylcholineIntegration = false;
        bool enableNorepinephrineIntegration = false;
        bool enableSerotoninIntegration = false;
        bool enableCuriosityIntegration = false;
        bool enableNoveltyIntegration = false;
        
        struct DopamineParams {
            float learningRate = 0.01f;
            float predictionErrorSensitivity = 1.0f;
            float baselineLevel = 0.1f;
            float phasicStrength = 1.0f;
        } dopamine;
        
        struct AcetylcholineParams {
            float attentionModulation = 1.0f;
            float memoryConsolidationModulation = 0.5f;
            float baselineLevel = 0.1f;
        } acetylcholine;
        
        NeuromodulationIntegrationConfig() = default;
    } neuromodulationIntegration;
    
    // Plasticity Configuration
    struct PlasticityConfig {
        bool enableSTDPIntegration = false;
        bool enableHebbianIntegration = false;
        bool enableStructuralPlasticityIntegration = false;
        bool enableSynapticScaling = false;
        bool enableMetaplasticity = false;
        
        float stdpLTPWeight = 1.0f;
        float stdpLTDWeight = 0.8f;
        float stdpTimeConstant = 20.0f;
        
        float hebbianLearningRate = 0.01f;
        float structuralPlasticityRate = 0.01f;
        
        float synapticScalingTargetRatio = 1.0f;
        float metaplasticChangeRate = 0.001f;
        
        PlasticityConfig() = default;
    } plasticity;
    
    // Cognition Integration Configuration
    struct CognitionIntegrationConfig {
        bool enableNeuralPlannerIntegration = false;
        bool enableConceptFormationIntegration = false;
        bool enableSelfModelIntegration = false;
        bool enableSocialLearningIntegration = false;
        
        struct PlannerParams {
            bool enablePlanning = false;
            float planningDepth = 3;
            float actionQualityThreshold = 0.5f;
        } planner;
        
        struct ConceptFormationParams {
            float conceptStabilityThreshold = 0.7f;
            float patternClarityThreshold = 0.6f;
        } conceptFormation;
        
        struct SelfModelParams {
            float selfModelConfidenceThreshold = 0.5f;
            float agencyThreshold = 0.3f;
        } selfModel;
        
        struct SocialLearningParams {
            float imitationThreshold = 0.5f;
            float theoryOfMindThreshold = 0.3f;
        } socialLearning;
        
        CognitionIntegrationConfig() = default;
    } cognitionIntegration;
    
    // Development Integration Configuration
    struct DevelopmentIntegrationConfig {
        bool enableDevelopmentalStages = false;
        bool enableMaturationPath = false;
        bool enablePubertyTransition = false;
        bool enableAgeDependentPlasticity = false;
        
        std::vector<std::string> developmentalStageOrder = {"initial", "critical_period", "maturation", "adult", "aging"};
        
        float plasticityRateModulation = 1.0f;
        float structuralPlasticityRate = 0.01f;
        
        float pubertyAge = 100.0f;
        nlohmann::json pubertyParams;
        
        DevelopmentIntegrationConfig() = default;
    } developmentIntegration;
    
    // Integration System Configuration
    struct IntegrationSystemConfig {
        bool enableMemoryPredictionIntegration = false;
        bool enableNeuromodulationPlasticityIntegration = false;
        bool enableCognitionAttentionIntegration = false;
        bool enableDevelopmentBehaviorIntegration = false;
        
        float integrationStrength = 1.0f;
        size_t integrationCheckInterval = 100;
        float crossSystemInfluence = 0.5f;
        
        IntegrationSystemConfig() = default;
    } integrationSystem;
    
    // Performance Configuration
    struct PerformanceConfig {
        bool enablePerformanceMonitoring = true;
        bool enableRealTimeMetrics = true;
        size_t metricsInterval = 1000;
        
        bool enableAutoOptimization = true;
        float optimizationThreshold = 0.8f;
        
        PerformanceConfig() = default;
    } performance;
    
    // Plugin System Configuration
    struct PluginSystemConfig {
        std::string pluginDirectory = "plugins/";
        bool enablePluginHotReload = true;
        bool validatePluginManifests = true;
        
        size_t maxConcurrentPlugins = 10;
        float pluginLoadTimeout = 30.0f;
        
        bool enablePluginLogging = true;
        std::string pluginLogLevel = "INFO";
        
        PluginSystemConfig() = default;
    } pluginSystem;
    
    // Command System Configuration
    struct CommandSystemConfig {
        bool enableCommandLogging = true;
        bool enableCommandValidation = true;
        
        size_t commandTimeout = 5000;
        std::string commandLogFile = "phase6_commands.log";
        
        bool enableCommandHistory = true;
        size_t commandHistorySize = 100;
        
        CommandSystemConfig() = default;
    } commandSystem;
    
    // Statistics Configuration
    struct StatisticsConfig {
        bool enableStatisticsCollection = true;
        bool enableRealTimeStatistics = true;
        
        size_t statisticsInterval = 1000;
        
        bool exportStatistics = true;
        std::string statisticsFile = "phase6_statistics.json";
        std::string statisticsFormat = "json";
        
        bool enableSystemMetrics = true;
        bool enablePluginMetrics = true;
        bool enableIntegrationMetrics = true;
        
        StatisticsConfig() = default;
    } statistics;
    
    // Health Check Configuration
    struct HealthCheckConfig {
        bool enableHealthChecks = true;
        bool enableProactiveHealthChecks = true;
        
        size_t healthCheckInterval = 500;
        float healthCheckTimeout = 100.0f;
        
        bool enableAutoRestart = false;
        float restartThreshold = 0.8f;
        
        bool enablePerformanceMonitoring = true;
        float performanceThreshold = 0.8f;
        
        bool enableMemoryMonitoring = true;
        float memoryThreshold = 0.9f;
        
        HealthCheckConfig() = default;
    } healthCheck;
    
    // Command Line Interface Configuration
    struct CLIConfig {
        bool enableHelpCommands = true;
        bool enableTabCompletion = true;
        bool enableCommandSuggestions = true;
        
        bool colorOutput = true;
        std::string outputFormat = "pretty";
        
        bool enableInteractiveMode = true;
        bool enableBatchMode = true;
        
        CLIConfig() = default;
    } cli;
    
    // Documentation Configuration
    struct DocumentationConfig {
        bool enableAutoDocumentation = true;
        bool generateIntegrationDocs = true;
        bool generatePluginDocs = true;
        
        std::string documentationDirectory = "docs/phase6/";
        std::string documentationFormat = "markdown";
        
        DocumentationConfig() = default;
    } documentation;
    
    Phase6AdvancedConfiguration() = default;
};

// Phase 6 Command Line Interface
class Phase6CLI {
public:
    Phase6CLI(std::shared_ptr<Phase6IntegrationCommands> integrationCommands,
             std::shared_ptr<Phase6PluginCommands> pluginCommands,
             std::shared_ptr<Config> config);
    ~Phase6CLI() = default;
    
    // Execute command line interface
    bool execute(int argc, char** argv);
    
    // Command handlers
    bool handleEasyWinsCommands(const std::vector<std::string>& args);
    bool handleIntegrationCommands(const std::vector<std::string>& args);
    bool handlePluginCommands(const std::vector<std::string>& args);
    bool handleConfigurationCommands(const std::vector<std::string>& args);
    bool handleStatusCommands(const std::vector<std::string>& args);
    bool handlePluginSystemCommands(const std::vector<std::string>& args);
    
    // Help and documentation
    void showHelp() const;
    void showEasyWinsHelp() const;
    void showIntegrationHelp() const;
    void showPluginHelp() const;
    void showConfigurationHelp() const;
    void showStatusHelp() const;
    void showPluginSystemHelp() const;
    
    // Configuration file handling
    bool loadPhase6Configuration(const std::string& filepath);
    bool savePhase6Configuration(const std::string& filepath) const;
    
    // Integration status and reporting
    void showIntegrationStatus() const;
    void showIntegrationReport() const;
    void showPluginStatus(const std::string& pluginId) const;
    void showPluginSystemStatus() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Integration Command Registry
class IntegrationCommandRegistry {
public:
    IntegrationCommandRegistry();
    ~IntegrationCommandRegistry() = default;
    
    // Register integration commands
    void registerEasyWinsCommands();
    void registerMemoryIntegrationCommands();
    void registerNeuromodulationIntegrationCommands();
    void registerPlasticityIntegrationCommands();
    void registerCognitionIntegrationCommands();
    void registerDevelopmentIntegrationCommands();
    void registerPluginSystemCommands();
    
    // Get command descriptions
    std::vector<std::string> getAvailableCommands() const;
    std::string getCommandDescription(const std::string& command) const;
    
    // Validate command
    bool validateCommand(const std::string& command, const std::vector<std::string>& args) const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Global integration commands instance
extern Phase6IntegrationCommands& getGlobalPhase6IntegrationCommands();

} // namespace nlm

#endif // NLM_PHASE6_INTEGRATION_COMMANDS_H
