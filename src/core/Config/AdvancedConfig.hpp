#pragma once

#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <map>
#include <unordered_map>
#include "Config.hpp"

namespace nlm {

// Forward declarations for configuration parameter classes
class NeuralParameters;
class MemoryParameters;
class PredictionParameters;
class NeuromodulationParameters;
class CognitiveParameters;
class DevelopmentParameters;
class IntegrationParameters;

// Configuration presets
enum class PresetType {
    Research,
    Learning,
    Simulation,
    Production
};

// Advanced configuration system
class AdvancedConfig {
public:
    AdvancedConfig();
    ~AdvancedConfig();
    
    // Disable copying, enable moving
    AdvancedConfig(const AdvancedConfig&) = delete;
    AdvancedConfig& operator=(const AdvancedConfig&) = delete;
    AdvancedConfig(AdvancedConfig&&) noexcept;
    AdvancedConfig& operator=(AdvancedConfig&&) noexcept;
    
    // Core advanced configuration methods
    void loadAdvancedConfig(const std::string& filename);
    void saveAdvancedConfig(const std::string& filename, const std::string& description = "") const;
    void applyPreset(const std::string& presetName);
    void optimizeParameters();
    void validateConfiguration();
    void exportConfiguration(const std::string& filename) const;
    void importConfiguration(const std::string& filename);
    void compareConfigurations(const AdvancedConfig& other) const;
    
    // Parameter setting methods
    void setNeuralParameters(const NeuralParameters& params);
    void setMemoryParameters(const MemoryParameters& params);
    void setPredictionParameters(const PredictionParameters& params);
    void setNeuromodulationParameters(const NeuromodulationParameters& params);
    void setCognitiveParameters(const CognitiveParameters& params);
    void setDevelopmentParameters(const DevelopmentParameters& params);
    void setIntegrationParameters(const IntegrationParameters& params);
    
    // Parameter getter methods
    const NeuralParameters& getNeuralParameters() const;
    const MemoryParameters& getMemoryParameters() const;
    const PredictionParameters& getPredictionParameters() const;
    const NeuromodulationParameters& getNeuromodulationParameters() const;
    const CognitiveParameters& getCognitiveParameters() const;
    const DevelopmentParameters& getDevelopmentParameters() const;
    const IntegrationParameters& getIntegrationParameters() const;
    
    // Advanced features
    bool isValid() const;
    std::string getValidationErrors() const;
    std::string getSummary() const;
    
    // Preset management
    static std::vector<std::string> getAvailablePresets();
    static std::map<std::string, std::string> getPresetDescriptions();
    
    // Dynamic configuration
    void setParameter(const std::string& category, const std::string& key, const ConfigValue& value);
    std::optional<ConfigValue> getParameter(const std::string& category, const std::string& key) const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helpers
    static bool validateParameterCombination(const std::string& category, const std::string& key, const ConfigValue& value);
    static std::string getPresetFilePath(const std::string& presetName);
};

// Neural configuration parameters
class NeuralParameters {
public:
    NeuralParameters();
    ~NeuralParameters();
    
    // Neuron parameters
    double neuronThreshold;
    double neuronRefractoryPeriod;
    double neuronMembraneTimeConstant;
    double neuronResetPotential;
    
    // Synaptic parameters
    double synapticWeight;
    double synapticDelay;
    double synapticDecay;
    double synapticModulation;
    
    // Learning parameters
    double learningRate;
    double momentum;
    double forgettingRate;
    double eligibilityTrace;
    
    // Network parameters
    size_t neuronCount;
    size_t synapseCount;
    double connectionDensity;
    
    std::string summary() const;
};

// Memory configuration parameters
class MemoryParameters {
public:
    MemoryParameters();
    
    // Working memory parameters
    size_t workingMemoryCapacity;
    double workingMemoryDecayRate;
    size_t workingMemoryUpdateInterval;
    
    // Episodic memory parameters
    size_t episodicMemoryCapacity;
    double episodicMemoryConsolidationRate;
    size_t episodicMemoryRetrievalWindow;
    
    // Associative memory parameters
    size_t associativeMemoryCapacity;
    double associativeMemoryStrength;
    size_t associativeMemoryUpdateInterval;
    
    std::string summary() const;
};

// Prediction configuration parameters
class PredictionParameters {
public:
    PredictionParameters();
    
    // Prediction accuracy parameters
    double predictionAccuracyThreshold;
    double predictionConfidenceThreshold;
    
    // Prediction horizon parameters
    size_t predictionHorizonSteps;
    double predictionHorizonDuration;
    
    // Training parameters
    size_t predictionTrainingEpochs;
    double predictionTrainingLearningRate;
    double predictionTrainingRegularization;
    
    // Model parameters
    size_t predictionModelOrder;
    double predictionModelComplexity;
    
    std::string summary() const;
};

// Neuromodulation configuration parameters
class NeuromodulationParameters {
public:
    NeuromodulationParameters();
    
    // Neuromodulator strength parameters
    double rewardStrength;
    double errorStrength;
    double noveltyStrength;
    double curiosityStrength;
    
    // Response dynamics parameters
    double neuromodulatorResponseTime;
    double neuromodulatorDecayTime;
    double neuromodulatorSaturation;
    
    // Interaction parameters
    double neuromodulatorInteractionStrength;
    size_t neuromodulatorInteractionWindow;
    
    std::string summary() const;
};

// Cognitive configuration parameters
class CognitiveParameters {
public:
    CognitiveParameters();
    
    // Planning parameters
    size_t planningDepth;
    double planningTimeHorizon;
    size_t planningExpansionSteps;
    
    // Concept formation parameters
    size_t conceptFormationWindow;
    double conceptFormationThreshold;
    size_t conceptFormationUpdateInterval;
    
    // Attention parameters
    double attentionFocusStrength;
    size_t attentionFocusDuration;
    double attentionSaliencyThreshold;
    
    std::string summary() const;
};

// Development configuration parameters
class DevelopmentParameters {
public:
    DevelopmentParameters();
    
    // Developmental stages
    double initialPlasticity;
    double criticalPeriodPlasticity;
    double adultPlasticity;
    
    // Maturation parameters
    size_t maturationSteps;
    double maturationRate;
    double maturationThreshold;
    
    // Plasticity modulation
    double structuralPlasticityRate;
    double functionalPlasticityRate;
    size_t plasticityUpdateInterval;
    
    std::string summary() const;
};

// Integration configuration parameters
class IntegrationParameters {
public:
    IntegrationParameters();
    
    // System coupling parameters
    double systemCouplingStrength;
    size_t systemCouplingLatency;
    double systemCommunicationEfficiency;
    
    // Coordination parameters
    size_t coordinationCycleInterval;
    double coordinationSmoothing;
    double coordinationThreshold;
    
    // Communication protocols
    std::string communicationProtocol;
    size_t communicationBufferSize;
    double communicationReliability;
    
    std::string summary() const;
};

} // namespace nlm
