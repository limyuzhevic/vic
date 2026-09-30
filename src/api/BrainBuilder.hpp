#pragma once

#include "Brain.hpp"
#include "../core/Config/Config.hpp"
#include <memory>
#include <string>
#include <format>
#include <vector>
#include <unordered_map>

namespace nlm {

// Forward declarations
class Brain;
class Neuron;
class Synapse;

// Brain builder class for fluent API configuration
class BrainBuilder {
public:
    BrainBuilder();
    ~BrainBuilder();
    
    // Disable copying, enable moving
    BrainBuilder(const BrainBuilder&) = delete;
    BrainBuilder& operator=(const BrainBuilder&) = delete;
    BrainBuilder(BrainBuilder&&) noexcept = default;
    BrainBuilder& operator=(BrainBuilder&&) noexcept = default;
    
    // Fluent configuration methods
    
    // Basic brain parameters
    BrainBuilder& withNeuronCount(size_t count);
    BrainBuilder& withRegionCount(size_t count);
    BrainBuilder& withConnectionProbability(float probability);
    BrainBuilder& withTimestep(double timestep);
    
    // Memory system configuration
    BrainBuilder& withWorkingMemoryCapacity(size_t capacity);
    BrainBuilder& withEpisodicMemoryCapacity(size_t capacity);
    BrainBuilder& withAssociativeMemoryEnabled(bool enabled = true);
    
    // Neuromodulation configuration
    BrainBuilder& withDopamineLevel(float level);
    BrainBuilder& withCuriosityLevel(float level);
    BrainBuilder& withNoveltyLevel(float level);
    BrainBuilder& withPredictionErrorLevel(float level);
    
    // Development configuration
    BrainBuilder& withDevelopmentStages(const std::string& stages);
    BrainBuilder& withCriticalPeriodEnabled(bool enabled = true);
    
    // Cognitive architecture configuration
    BrainBuilder& withPlanningEnabled(bool enabled = true);
    BrainBuilder& withConceptFormationEnabled(bool enabled = true);
    BrainBuilder& withAttentionEnabled(bool enabled = true);
    BrainBuilder& withPredictionEnabled(bool enabled = true);
    
    // Population type configuration
    BrainBuilder& withSensoryNeuronCount(size_t count);
    BrainBuilder& withInternalNeuronCount(size_t count);
    BrainBuilder& withMotorNeuronCount(size_t count);
    BrainBuilder& withExcitatoryNeuronRatio(float ratio);
    BrainBuilder& withInhibitoryNeuronRatio(float ratio);
    
    // Plasticity configuration
    BrainBuilder& withSTDPWeight(float ltp, float ltd, float tau);
    BrainBuilder& withHebbianLearningEnabled(bool enabled = true);
    BrainBuilder& withStructuralPlasticityEnabled(bool enabled = true);
    BrainBuilder& withSynaptogenesisRate(float rate);
    BrainBuilder& withPruningRate(float rate);
    
    // Checkpoint and persistence
    BrainBuilder& withCheckpointDir(const std::string& dir);
    BrainBuilder& withCheckpointInterval(size_t steps);
    BrainBuilder& withCheckpointCompression(bool enabled);
    
    // Performance configuration
    BrainBuilder& withPerformanceMode(PerformanceMode mode);
    BrainBuilder& withSIMDAcceleration(bool enabled);
    BrainBuilder& withParallelProcessing(bool enabled);
    BrainBuilder& withMemoryPoolEnabled(bool enabled);
    
    // Presets for common brain architectures
    
    // Simple control system
    BrainBuilder& createControlSystem();
    
    // Sensory processing system
    BrainBuilder& createSensorySystem();
    
    // Cognitive architecture
    BrainBuilder& createCognitiveArchitecture();
    
    // Memory-dominated system
    BrainBuilder& createMemoryDominant();
    
    // Motor control system
    BrainBuilder& createMotorControl();
    
    // Developmental system
    BrainBuilder& createDevelopmentalBrain();
    
    // Social learning system
    BrainBuilder& createSocialLearning();
    
    // Advanced feature presets
    
    // Attention-focused system
    BrainBuilder& createAttentionFocused();
    
    // Prediction-heavy system
    BrainBuilder& createPredictionHeavy();
    
    // Reward-based learning system
    BrainBuilder& createRewardBased();
    
    // Exploration-focused system
    BrainBuilder& createExplorationFocused();
    
    // Curiosity-driven system
    BrainBuilder& createCuriosityDriven();
    
    // Task-specific presets
    
    // Simple navigation task
    BrainBuilder& forNavigationTask();
    
    // Object recognition task
    BrainBuilder& forObjectRecognition();
    
    // Sequential learning task
    BrainBuilder& forSequentialLearning();
    
    // Decision making task
    BrainBuilder& forDecisionMaking();
    
    // Social interaction task
    BrainBuilder& forSocialInteraction();
    
    // Build the brain
    std::shared_ptr<Brain> build();
    
    // Build and initialize
    std::shared_ptr<Brain> buildAndInitialize();
    
    // Get current configuration summary
    std::string getConfigurationSummary() const;
    
    // Validate current configuration
    bool validateConfiguration() const;
    
    // Reset builder to default state
    void reset();
    
    // Export configuration to JSON
    bool exportToJSON(const std::string& filepath) const;
    
    // Import configuration from JSON
    bool importFromJSON(const std::string& filepath);
    
    // Clone configuration
    std::unique_ptr<BrainBuilder> clone() const;
    
    // Get default configuration values
    static std::unordered_map<std::string, std::string> getDefaultConfiguration();
    
    // Create a builder with default configuration
    static BrainBuilder createDefault();
    
    // Create a builder with minimal configuration
    static BrainBuilder createMinimal();

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helper methods
    void applyPreset(PresetType preset);
    void setupDefaultConfiguration();
    bool validateConfigurationInternal() const;
    std::shared_ptr<Config> createConfigFromBuilder() const;
    
    // Configuration validation
    bool isNeuronCountValid(size_t count) const;
    bool isRegionCountValid(size_t count) const;
    bool isConnectionProbabilityValid(float prob) const;
    bool isTimestepValid(double timestep) const;
    bool arePopulationCountsValid() const;
    bool isRatioValid(float ratio) const;
};

// Convenience factory functions
namespace BrainFactory {
    
    // Create a simple brain with default configuration
    std::shared_ptr<Brain> createSimpleBrain(size_t neuronCount = 1000);
    
    // Create a cognitive brain with advanced features
    std::shared_ptr<Brain> createCognitiveBrain(size_t neuronCount = 5000);
    
    // Create a memory-focused brain
    std::shared_ptr<Brain> createMemoryBrain(size_t neuronCount = 2000);
    
    // Create a developmental brain
    std::shared_ptr<Brain> createDevelopmentalBrain(size_t neuronCount = 3000);
    
    // Create a motor control brain
    std::shared_ptr<Brain> createMotorControlBrain(size_t neuronCount = 1500);
    
    // Create a social learning brain
    std::shared_ptr<Brain> createSocialLearningBrain(size_t neuronCount = 4000);
    
    // Create a brain for a specific task
    std::shared_ptr<Brain> createBrainForTask(TaskType task, size_t neuronCount = 2000);
    
    // Create a brain with custom configuration
    std::shared_ptr<Brain> createCustomBrain(const std::string& configFile);
    
    // Create a brain from JSON configuration
    std::shared_ptr<Brain> createBrainFromJSON(const std::string& jsonConfig);
}

// Brain configuration presets
enum class PresetType {
    Default,
    SimpleControl,
    SensoryProcessing,
    CognitiveArchitecture,
    MemoryDominant,
    MotorControl,
    Developmental,
    SocialLearning,
    AttentionFocused,
    PredictionHeavy,
    RewardBased,
    ExplorationFocused,
    CuriosityDriven
};

// Task types for specialized brain creation
enum class TaskType {
    Navigation,
    ObjectRecognition,
    SequentialLearning,
    DecisionMaking,
    SocialInteraction,
    MotorControl,
    SensoryProcessing,
    CognitiveReasoning
};

// Performance modes
enum class PerformanceMode {
    Default,
    Speed,
    Accuracy,
    Balanced,
    MemoryOptimized,
    CPUOptimized,
    GPUOptimized
};

// Builder configuration utility
class BuilderConfig {
public:
    BuilderConfig() = default;
    ~BuilderConfig() = default;
    
    // Get global builder configuration
    static const BuilderConfig& getInstance();
    
    // Set configuration values
    void setNeuronCount(size_t count);
    void setRegionCount(size_t count);
    void setDefaultConnectionProbability(float prob);
    void setDefaultTimestep(double timestep);
    
    // Get configuration values
    size_t getNeuronCount() const;
    size_t getRegionCount() const;
    float getDefaultConnectionProbability() const;
    double getDefaultTimestep() const;
    
    // Validation
    bool isValid() const;
    
    // Reset to defaults
    void resetToDefaults();
    
    // Export/import configuration
    bool exportConfig(const std::string& filepath) const;
    bool importConfig(const std::string& filepath);
    
private:
    size_t defaultNeuronCount;
    size_t defaultRegionCount;
    float defaultConnectionProbability;
    double defaultTimestep;
    
    static BuilderConfig instance;
};

// Smart neuron population configurator
class NeuronPopulationConfigurator {
public:
    NeuronPopulationConfigurator(BrainBuilder& builder);
    ~NeuronPopulationConfigurator();
    
    // Configure sensory population
    BrainBuilder& sensory(size_t count);
    
    // Configure internal population
    BrainBuilder& internal(size_t count);
    
    // Configure motor population
    BrainBuilder& motor(size_t count);
    
    // Configure mixed population
    BrainBuilder& mixed(size_t sensory, size_t internal, size_t motor);
    
    // Configure by neuron type ratios
    BrainBuilder& byTypeRatios(float sensory, float internal, float motor);
    
    // Configure for specific task
    BrainBuilder& forTask(TaskType task);
    
    // Get current configuration
    std::string getConfiguration() const;
    
private:
    BrainBuilder& builder;
};

// Memory system configurator
class MemorySystemConfigurator {
public:
    MemorySystemConfigurator(BrainBuilder& builder);
    ~MemorySystemConfigurator();
    
    // Configure working memory
    BrainBuilder& workingMemory(size_t capacity, float recallSpeed = 1.0f);
    
    // Configure episodic memory
    BrainBuilder& episodicMemory(size_t maxEpisodes, float consolidationRate = 0.1f);
    
    // Configure associative memory
    BrainBuilder& associativeMemory(float associationStrength = 0.5f, 
                                    size_t maxAssociations = 10000);
    
    // Configure memory consolidation
    BrainBuilder& memoryConsolidation(float consolidationRate = 0.2f,
                                      size_t consolidationInterval = 1000);
    
    // Configure memory replay
    BrainBuilder& memoryReplay(size_t replayInterval = 100, 
                              float replayStrength = 0.8f);
    
    // Enable/disable memory systems
    BrainBuilder& enableWorkingMemory(bool enabled = true);
    BrainBuilder& enableEpisodicMemory(bool enabled = true);
    BrainBuilder& enableAssociativeMemory(bool enabled = true);
    
    // Get current memory configuration
    std::string getMemoryConfiguration() const;
    
private:
    BrainBuilder& builder;
};

// Plasticity system configurator
class PlasticitySystemConfigurator {
public:
    PlasticitySystemConfigurator(BrainBuilder& builder);
    ~PlasticitySystemConfigurator();
    
    // Configure STDP plasticity
    BrainBuilder& stdp(float ltpWeight = 0.01f, float ltdWeight = 0.012f, 
                      float tau = 20.0f, float learningRate = 0.001f);
    
    // Configure Hebbian plasticity
    BrainBuilder& hebbian(float learningRate = 0.01f, float weightMax = 1.0f);
    
    // Configure structural plasticity
    BrainBuilder& structuralPlasticity(float synaptogenesisRate = 0.0001f,
                                       float pruningRate = 0.00001f,
                                       float maturationRate = 0.001f);
    
    // Configure neuromodulation-dependent plasticity
    BrainBuilder& neuromodulatedPlasticity(float rewardModulation = 0.5f,
                                          float predictionErrorModulation = 0.3f);
    
    // Enable/disable plasticity types
    BrainBuilder& enableSTDP(bool enabled = true);
    BrainBuilder& enableHebbian(bool enabled = true);
    BrainBuilder& enableStructural(bool enabled = true);
    BrainBuilder& enableNeuromodulated(bool enabled = true);
    
    // Get current plasticity configuration
    std::string getPlasticityConfiguration() const;
    
private:
    BrainBuilder& builder;
};

} // namespace nlm

// Implementation includes
#include "BrainBuilder.cpp"

// Convenience macros
#define SIMPLE_BRAIN(name, neurons) auto name = nlm::BrainFactory::createSimpleBrain(neurons)
#define COGNITIVE_BRAIN(name, neurons) auto name = nlm::BrainFactory::createCognitiveBrain(neurons)
#define MEMORY_BRAIN(name, neurons) auto name = nlm::BrainFactory::createMemoryBrain(neurons)
#define DEVELOPMENTAL_BRAIN(name, neurons) auto name = nlm::BrainFactory::createDevelopmentalBrain(neurons)

#define BUILDER brain = nlm::BrainBuilder().createDefault().buildAndInitialize()