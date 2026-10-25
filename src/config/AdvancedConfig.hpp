# Advanced Configuration System for NLM
// Comprehensive configuration options for neural parameters, cognitive systems,
// memory management, and simulation settings.

#pragma once

#include "../core/Config/Config.hpp"
#include <string>
#include <vector>
#include <memory>
#include <map>

namespace nlm {

// Advanced neural configuration
class AdvancedConfig {
public:
    // Neural architecture parameters
    struct NeuralParameters {
        size_t neuronCount = 1000;
        size_t regionCount = 1;
        float simulationTimestep = 0.001f;
        float connectionProbability = 0.1f;
        float initialExcitatoryWeight = 0.2f;
        float initialInhibitoryWeight = 0.1f;
        size_t maxSynapsesPerNeuron = 100;
    };

    // Memory system configuration
    struct MemoryParameters {
        // Working memory
        size_t workingMemoryCapacity = 100;
        float workingMemoryDecayRate = 0.01f;
        float workingMemoryThreshold = 0.3f;
        size_t workingMemoryMaxTraces = 1000;
        
        // Episodic memory
        size_t episodicMemoryMaxEpisodes = 1000;
        float episodicMemoryRelevanceDecay = 0.001f;
        size_t episodicMemoryReplayInterval = 100;
        size_t episodicMemoryConsolidationInterval = 1000;
        
        // Associative memory
        size_t associativeMemoryMaxAssociations = 5000;
        float associativeMemoryMinStrength = 0.1f;
        float associativeMemoryMaxStrength = 1.0f;
        size_t associativeMemorySpreadSteps = 2;
    };

    // Cognitive system configuration
    struct CognitiveParameters {
        // Prediction system
        float predictionConfidenceThreshold = 0.5f;
        float predictionErrorTolerance = 0.1f;
        size_t predictionHistorySize = 1000;
        float predictionLearningRate = 0.01f;
        
        // Planning system
        size_t planningDepth = 5;
        float planningExploration = 0.3f;
        float planningExploitation = 0.7f;
        
        // Attention system
        float attentionInhibitionStrength = 0.5f;
        float attentionExcitationStrength = 1.5f;
        float attentionCompetitionThreshold = 0.3f;
        float attentionDecayRate = 0.95f;
        
        // Concept formation
        float conceptFormationLearningRate = 0.01f;
        size_t conceptFormationMaxConcepts = 100;
        float conceptFormationSimilarityThreshold = 0.7f;
    };

    // Neuromodulation parameters
    struct NeuromodulationParameters {
        // Dopamine system
        float dopamineBaseline = 0.1f;
        float dopamineLearningRate = 0.001f;
        float dopamineTimeConstant = 100.0f;
        
        // Curiosity system
        float curiosityBaseline = 0.1f;
        float curiosityLearningRate = 0.001f;
        float curiosityNoveltySensitivity = 0.5f;
        
        // Novelty detection
        float noveltyThreshold = 0.5f;
        float noveltyDecayRate = 0.9f;
        
        // Prediction error
        float predictionErrorGain = 1.0f;
        float predictionErrorTimeConstant = 50.0f;
    };

    // Development system configuration
    struct DevelopmentParameters {
        // Developmental stages
        size_t criticalPeriodStart = 1000;
        size_t criticalPeriodEnd = 5000;
        size_t maturationStart = 5000;
        
        // Plasticity modulation
        float initialPlasticity = 1.0f;
        float criticalPeriodPlasticity = 0.8f;
        float maturationPlasticity = 0.5f;
        float adultPlasticity = 0.2f;
        
        // Synaptogenesis
        float synaptogenesisRate = 0.0001f;
        float pruningRate = 0.00001f;
        size_t synaptogenesisMaxAge = 1000;
    };

    // Plasticity system parameters
    struct PlasticityParameters {
        // STDP
        float stdpLTPWeight = 0.01f;
        float stdpLTDWeight = 0.012f;
        float stdpTimeConstant = 20.0f;
        float stdpLearningRate = 0.001f;
        
        // Hebbian
        float hebbianLearningRate = 0.01f;
        float hebbianTimeConstant = 10.0f;
        
        // Structural
        float structuralPlasticityRate = 0.001f;
        float structuralTimeConstant = 100.0f;
    };

    // Simulation and monitoring parameters
    struct SimulationParameters {
        size_t simulationSteps = 10000;
        size_t checkpointInterval = 1000;
        size_t maxCheckpoints = 10;
        size_t monitoringInterval = 100;
        bool enableProfiling = false;
        bool enableLogging = true;
        
        // Visualization
        bool enableVisualization = false;
        std::string visualizationOutput = "";
        float visualizationScale = 1.0f;
        
        // Performance
        size_t parallelThreads = 1;
        size_t maxMemoryUsage = 1024;  // MB
        float timeLimit = 0.0f;  // 0 = unlimited
    };

    // Configuration groups
    struct ConfigurationGroups {
        NeuralParameters neural;
        MemoryParameters memory;
        CognitiveParameters cognitive;
        NeuromodulationParameters neuromodulation;
        DevelopmentParameters development;
        PlasticityParameters plasticity;
        SimulationParameters simulation;
    };

    AdvancedConfig() = default;
    ~AdvancedConfig() = default;
    
    // Initialize from standard config
    void fromStandardConfig(const Config& standardConfig);
    
    // Apply to standard config
    void applyToConfig(Config& config) const;
    
    // Get configuration as standard config
    Config toStandardConfig() const;
    
    // Validate configuration
    bool validate() const;
    std::vector<std::string> validateAndGetErrors() const;
    
    // Merge with another configuration
    void merge(const AdvancedConfig& other);
    
    // Get default configuration
    static AdvancedConfig getDefaultConfig();
    
    // Load from file
    bool loadFromFile(const std::string& filepath);
    
    // Save to file
    bool saveToFile(const std::string& filepath) const;
    
    // Get configuration groups
    const ConfigurationGroups& getGroups() const { return groups; }
    ConfigurationGroups& getGroups() { return groups; }
    
    // Individual parameter accessors for convenience
    const NeuralParameters& getNeural() const { return groups.neural; }
    NeuralParameters& getNeural() { return groups.neural; }
    
    const MemoryParameters& getMemory() const { return groups.memory; }
    MemoryParameters& getMemory() { return groups.memory; }
    
    const CognitiveParameters& getCognitive() const { return groups.cognitive; }
    CognitiveParameters& getCognitive() { return groups.cognitive; }
    
    const NeuromodulationParameters& getNeuromodulation() const { return groups.neuromodulation; }
    NeuromodulationParameters& getNeuromodulation() { return groups.neuromodulation; }
    
    const DevelopmentParameters& getDevelopment() const { return groups.development; }
    DevelopmentParameters& getDevelopment() { return groups.development; }
    
    const PlasticityParameters& getPlasticity() const { return groups.plasticity; }
    PlasticityParameters& getPlasticity() { return groups.plasticity; }
    
    const SimulationParameters& getSimulation() const { return groups.simulation; }
    SimulationParameters& getSimulation() { return groups.simulation; }
    
    // Quick setup methods
    void setupBasicResearch() {
        getNeural().neuronCount = 500;
        getMemory().workingMemoryCapacity = 50;
        getMemory().episodicMemoryMaxEpisodes = 500;
        getCognitive().planningDepth = 3;
        getSimulation().simulationSteps = 5000;
    }
    
    void setupAdvancedResearch() {
        getNeural().neuronCount = 5000;
        getMemory().workingMemoryCapacity = 500;
        getMemory().episodicMemoryMaxEpisodes = 5000;
        getMemory().episodicMemoryConsolidationInterval = 500;
        getCognitive().planningDepth = 7;
        getCognitive().predictionConfidenceThreshold = 0.7f;
        getSimulation().simulationSteps = 20000;
        getSimulation().enableVisualization = true;
    }
    
    void setupRealTime() {
        getNeural().neuronCount = 100;
        getMemory().workingMemoryCapacity = 20;
        getSimulation().simulationSteps = 100000;
        getSimulation().checkpointInterval = 1000;
        getSimulation().monitoringInterval = 10;
        getSimulation().enableProfiling = true;
        getSimulation().enableLogging = false;
    }
    
    void setupMemoryIntensive() {
        getMemory().workingMemoryCapacity = 1000;
        getMemory().episodicMemoryMaxEpisodes = 10000;
        getMemory().associativeMemoryMaxAssociations = 50000;
        getSimulation().maxMemoryUsage = 2048;
        getSimulation().parallelThreads = 4;
    }
    
    // Configuration presets
    enum class Preset {
        Basic,
        Advanced,
        RealTime,
        MemoryIntensive,
        Research,
        Custom
    };
    
    void setPreset(Preset preset);
    
    // String-based parameter access
    template<typename T>
    void setParameter(const std::string& group, const std::string& parameter, const T& value);
    
    template<typename T>
    T getParameter(const std::string& group, const std::string& parameter) const;
    
    // Configuration validation and error handling
    class ValidationError {
    public:
        enum class ErrorType {
            InvalidRange,
            MissingDependency,
            InvalidCombination,
            ResourceLimitExceeded,
            Unknown
        };
        
        ValidationError(ErrorType type, const std::string& message,
                       const std::string& parameter = "")
            : type(type), message(message), parameter(parameter) {}
        
        ErrorType type;
        std::string message;
        std::string parameter;
    };

private:
    ConfigurationGroups groups;
    
    // Parameter name mappings
    static const std::map<std::string, std::map<std::string, std::string>> parameterNames;
    
    // Configuration validation
    std::vector<ValidationError> validateConfiguration() const;
    
    // Apply parameter constraints
    void applyParameterConstraints();
    
    // Convert to standard config value names
    static std::string toStandardParameterName(const std::string& group, const std::string& parameter);
    
    // Get from standard config value name
    static bool fromStandardParameterName(const std::string& standardName, std::string& group, std::string& parameter);
};

// Factory function for advanced configuration
inline AdvancedConfig createAdvancedConfig() {
    return AdvancedConfig::getDefaultConfig();
}

// Quick configuration presets
namespace Presets {
    inline AdvancedConfig getResearchBasic() {
        AdvancedConfig config;
        config.setupBasicResearch();
        return config;
    }
    
    inline AdvancedConfig getResearchAdvanced() {
        AdvancedConfig config;
        config.setupAdvancedResearch();
        return config;
    }
    
    inline AdvancedConfig getRealTime() {
        AdvancedConfig config;
        config.setupRealTime();
        return config;
    }
    
    inline AdvancedConfig getMemoryIntensive() {
        AdvancedConfig config;
        config.setupMemoryIntensive();
        return config;
    }
}

} // namespace nlm