#pragma once

/**
 * Phase 6 Enhanced Configuration System
 * 
 * Provides comprehensive configuration management for Phase 6 integration experiments.
 * Extends the existing Config class with Phase 6 specific features and validation.
 */

#include "src/core/Config/Config.hpp"
#include <string>
#include <memory>
#include <vector>
#include <unordered_map>
#include <set>

namespace nlm {

/**
 * Phase 6 Experiment Configuration
 * 
 * Comprehensive configuration for Phase 6 integration experiments.
 * Provides validation, defaults, and organized configuration groups.
 */
class Phase6Config {
public:
    // Configuration groups for better organization
    enum class ConfigGroup {
        Experiment,      // Core experiment parameters
        Performance,     // Performance and optimization settings
        Memory,          // Memory system configuration
        Neuromodulation, // Neuromodulation system configuration
        Development,     // Developmental process configuration
        Learning,        // Learning and plasticity configuration
        Output,          // Output and logging configuration
        Advanced         // Advanced/experimental settings
    };
    
    // Experiment configuration
    struct ExperimentConfig {
        uint64_t maxSteps;
        size_t neuronCount;
        size_t regionCount;
        float connectionProbability;
        uint64_t randomSeed;
        double simulationTimestep;
        bool enableCheckpointing;
        bool enableReplay;
        bool enableDevelopment;
        std::string checkpointPath;
        std::string experimentName;
        
        ExperimentConfig() 
            : maxSteps(10000), neuronCount(1000), regionCount(1), 
              connectionProbability(0.1f), randomSeed(42), simulationTimestep(0.001),
              enableCheckpointing(true), enableReplay(true), enableDevelopment(true),
              checkpointPath("./checkpoint_test.bin"), experimentName("Phase6Experiment") {}
    };
    
    // Performance configuration
    struct PerformanceConfig {
        bool enableVectorization;
        bool enableParallelProcessing;
        int maxThreads;
        bool enableSIMD;
        bool enableMemoryPools;
        bool enableEventQueues;
        bool optimizeForSpeed;
        bool optimizeForMemory;
        
        PerformanceConfig()
            : enableVectorization(true), enableParallelProcessing(false), maxThreads(4),
              enableSIMD(true), enableMemoryPools(true), enableEventQueues(true),
              optimizeForSpeed(true), optimizeForMemory(false) {}
    };
    
    // Memory configuration
    struct MemoryConfig {
        bool enableWorkingMemory;
        bool enableEpisodicMemory;
        bool enableSemanticMemory;
        bool enableProceduralMemory;
        bool enableAssociativeMemory;
        size_t workingMemoryCapacity;
        size_t episodicMemoryCapacity;
        double replayFrequency;
        double consolidationThreshold;
        bool enableSleepReplay;
        
        MemoryConfig()
            : enableWorkingMemory(true), enableEpisodicMemory(true), 
              enableSemanticMemory(true), enableProceduralMemory(true), enableAssociativeMemory(true),
              workingMemoryCapacity(1000), episodicMemoryCapacity(100), replayFrequency(3600.0),
              consolidationThreshold(0.8), enableSleepReplay(true) {}
    };
    
    // Neuromodulation configuration
    struct NeuromodulationConfig {
        bool enableDopamine;
        bool enableCuriosity;
        bool enableNovelty;
        bool enablePredictionError;
        float dopamineBaseline;
        float curiosityBaseline;
        float noveltyBaseline;
        float predictionErrorBaseline;
        double neuromodulationTimeScale;
        bool enableRewardPredictionError;
        bool enableRiskAssessment;
        
        NeuromodulationConfig()
            : enableDopamine(true), enableCuriosity(true), enableNovelty(true),
              enablePredictionError(true), dopamineBaseline(0.5f), curiosityBaseline(0.5f),
              noveltyBaseline(0.5f), predictionErrorBaseline(0.5f), neuromodulationTimeScale(1.0),
              enableRewardPredictionError(true), enableRiskAssessment(false) {}
    };
    
    // Development configuration
    struct DevelopmentConfig {
        bool enableDevelopmentalStages;
        DevelopmentalStage initialStage;
        double criticalPeriodStart;
        double criticalPeriodEnd;
        double maturationAge;
        double agingStart;
        float plasticityRate;
        float plasticityMaturationRate;
        float plasticityAgingRate;
        bool enableExperienceDependentDevelopment;
        bool enableGeneticConstraints;
        
        DevelopmentConfig()
            : enableDevelopmentalStages(true), initialStage(DevelopmentalStage::Initial),
              criticalPeriodStart(10.0), criticalPeriodEnd(100.0), maturationAge(300.0),
              agingStart(600.0), plasticityRate(1.0f), plasticityMaturationRate(0.5f),
              plasticityAgingRate(0.2f), enableExperienceDependentDevelopment(true),
              enableGeneticConstraints(true) {}
    };
    
    // Learning configuration
    struct LearningConfig {
        float hebbianLearningRate;
        float stdpLTPWeight;
        float stdpLTDWeight;
        float stdpTimeConstant;
        float rewardModulatedLearningRate;
        bool enableSTDP;
        bool enableHebbian;
        bool enableRewardModulated;
        float eligibilityTraceDecay;
        float synapticWeightBoundsMin;
        float synapticWeightBoundsMax;
        bool enableStructuralPlasticity;
        float synaptogenesisRate;
        float pruningRate;
        
        LearningConfig()
            : hebbianLearningRate(0.01f), stdpLTPWeight(0.02f), stdpLTDWeight(0.015f),
              stdpTimeConstant(20.0f), rewardModulatedLearningRate(0.005f),
              enableSTDP(true), enableHebbian(true), enableRewardModulated(true),
              eligibilityTraceDecay(0.95f), synapticWeightBoundsMin(0.0f),
              synapticWeightBoundsMax(1.0f), enableStructuralPlasticity(true),
              synaptogenesisRate(0.0001f), pruningRate(0.00001f) {}
    };
    
    // Output configuration
    struct OutputConfig {
        bool enableConsoleLogging;
        bool enableFileLogging;
        bool enableNetworkLogging;
        std::string logFile;
        std::string resultsFile;
        std::string checkpointFile;
        bool saveDetailedMetrics;
        bool saveNeuralTrajectories;
        bool enableVisualizationOutput;
        
        OutputConfig()
            : enableConsoleLogging(true), enableFileLogging(true), enableNetworkLogging(false),
              logFile("nlm_phase6.log"), resultsFile("nlm_phase6_results.json"),
              checkpointFile("nlm_phase6_checkpoint.bin"), saveDetailedMetrics(true),
              saveNeuralTrajectories(true), enableVisualizationOutput(false) {}
    };
    
    // Advanced configuration
    struct AdvancedConfig {
        bool enableExperimentalFeatures;
        bool enableDebugMode;
        bool enableValidation;
        bool enableProfiling;
        bool enableBenchmarking;
        std::vector<std::string> enabledExperiments;
        std::unordered_map<std::string, double> customParameters;
        bool enableAblationStudies;
        bool enableParameterTuning;
        
        AdvancedConfig()
            : enableExperimentalFeatures(false), enableDebugMode(false),
              enableValidation(true), enableProfiling(false), enableBenchmarking(false),
              enableAblationStudies(false), enableParameterTuning(false) {}
    };
    
    // Main configuration class
    Phase6Config();
    ~Phase6Config();
    
    // Disable copying, enable moving
    Phase6Config(const Phase6Config&) = delete;
    Phase6Config& operator=(const Phase6Config&) = delete;
    Phase6Config(Phase6Config&&) noexcept;
    Phase6Config& operator=(Phase6Config&&) noexcept;
    
    // Configuration management
    void loadFromFile(const std::string& filepath);
    void saveToFile(const std::string& filepath) const;
    
    // Validation
    bool validate() const;
    std::vector<std::string> getValidationErrors() const;
    
    // Configuration access
    template<typename T>
    T get(const std::string& key) const;
    
    template<typename T>
    T getOr(const std::string& key, const T& defaultValue) const;
    
    // Configuration modification
    void set(const std::string& key, const ConfigValue& value, ConfigSource source = ConfigSource::Runtime);
    
    // Configuration groups
    const ExperimentConfig& getExperimentConfig() const { return experimentConfig_; }
    const PerformanceConfig& getPerformanceConfig() const { return performanceConfig_; }
    const MemoryConfig& getMemoryConfig() const { return memoryConfig_; }
    const NeuromodulationConfig& getNeuromodulationConfig() const { return neuromodulationConfig_; }
    const DevelopmentConfig& getDevelopmentConfig() const { return developmentConfig_; }
    const LearningConfig& getLearningConfig() const { return learningConfig_; }
    const OutputConfig& getOutputConfig() const { return outputConfig_; }
    const AdvancedConfig& getAdvancedConfig() const { return advancedConfig_; }
    
    // Set configuration groups
    void setExperimentConfig(const ExperimentConfig& config) { experimentConfig_ = config; }
    void setPerformanceConfig(const PerformanceConfig& config) { performanceConfig_ = config; }
    void setMemoryConfig(const MemoryConfig& config) { memoryConfig_ = config; }
    void setNeuromodulationConfig(const NeuromodulationConfig& config) { neuromodulationConfig_ = config; }
    void setDevelopmentConfig(const DevelopmentConfig& config) { developmentConfig_ = config; }
    void setLearningConfig(const LearningConfig& config) { learningConfig_ = config; }
    void setOutputConfig(const OutputConfig& config) { outputConfig_ = config; }
    void setAdvancedConfig(const AdvancedConfig& config) { advancedConfig_ = config; }
    
    // Configuration presets
    static Phase6Config getPreset(const std::string& presetName);
    static std::vector<std::string> getAvailablePresets();
    
    // Configuration comparison
    bool isEqual(const Phase6Config& other) const;
    
    // Configuration serialization
    std::string toString() const;
    bool fromString(const std::string& str);
    
    // Configuration migration (for backwards compatibility)
    void migrateFromLegacyConfig(const Config& legacyConfig);
    
    // Configuration profiling
    std::string getConfigurationProfile() const;
    double getEstimatedMemoryUsage() const;
    double getEstimatedRuntime() const;
    
    // Configuration help
    std::string getConfigDescription(const std::string& key) const;
    std::vector<std::string> getConfigKeysByGroup(ConfigGroup group) const;
    std::unordered_map<std::string, ConfigGroup> getKeyToGroupMap() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Configuration storage
    ExperimentConfig experimentConfig_;
    PerformanceConfig performanceConfig_;
    MemoryConfig memoryConfig_;
    NeuromodulationConfig neuromodulationConfig_;
    DevelopmentConfig developmentConfig_;
    LearningConfig learningConfig_;
    OutputConfig outputConfig_;
    AdvancedConfig advancedConfig_;
    
    // Key mapping for configuration access
    std::unordered_map<std::string, ConfigGroup> keyToGroup_;
    std::unordered_map<ConfigGroup, std::set<std::string>> groupToKeys_;
    
    // Validation cache
    mutable bool validationDirty_;
    mutable std::vector<std::string> validationErrors_;
    
    // Helper methods
    void initializeKeyMappings();
    void updateValidationCache() const;
    bool validateGroup(ConfigGroup group) const;
    std::string configValueToString(const ConfigValue& value) const;
    ConfigValue stringToConfigValue(const std::string& str) const;
    
    // Configuration preset definitions
    static void defineStandardPresets(std::vector<std::string>& presets);
    static void defineQuickPresets(std::vector<std::string>& presets);
    static void defineResearchPresets(std::vector<std::string>& presets);
}; 

// Helper functions
inline std::string toString(Phase6Config::ConfigGroup group) {
    switch (group) {
        case Phase6Config::ConfigGroup::Experiment: return "Experiment";
        case Phase6Config::ConfigGroup::Performance: return "Performance";
        case Phase6Config::ConfigGroup::Memory: return "Memory";
        case Phase6Config::ConfigGroup::Neuromodulation: return "Neuromodulation";
        case Phase6Config::ConfigGroup::Development: return "Development";
        case Phase6Config::ConfigGroup::Learning: return "Learning";
        case Phase6Config::ConfigGroup::Output: return "Output";
        case Phase6Config::ConfigGroup::Advanced: return "Advanced";
        default: return "Unknown";
    }
}

} // namespace nlm
