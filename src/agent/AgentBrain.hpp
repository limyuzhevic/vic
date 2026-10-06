// AgentBrain.hpp - Enhanced AgentBrain with improved architecture and validation

#pragma once

#include "AgentBody.hpp"
#include "SensoryPercept.hpp"
#include "../brain/Brain.hpp"
#include "../world/SimpleWorld.hpp"
#include <memory>
#include <vector>
#include <unordered_map>
#include <algorithm>
#include <cmath>

namespace nlm {

// AgentBrain Configuration Parameters
struct AgentBrainConfig {
    // Sensory configuration
    size_t visionWidth{16};
    size_t visionHeight{16};
    size_t touchNeurons{8};
    size_t internalNeurons{4};
    size_t proprioceptionNeurons{6};
    
    // Motor configuration
    size_t maxMotorGroups{6};
    
    // Novelty detection
    float noveltyDecay{0.99f};
    float noveltyThreshold{0.1f};
    
    // Curiosity parameters
    float curiosityBaseMultiplier{2.0f};
    float curiosityErrorMultiplier{0.5f};
    float curiosityMin{0.0f};
    float curiosityMax{1.0f};
    
    // Exploration parameters
    float explorationThreshold{0.5f};
    float explorationChanceMultiplier{0.3f};
    float minExplorationChance{0.05f};
    float maxExplorationChance{0.3f};
    
    // Activity thresholds
    float activationThreshold{0.5f};
    float sensoryInputScale{5.0f};
    float touchScale{8.0f};
    float internalScale{2.0f};
    float proprioceptionScale{2.0f};
    
    // Developmental stages (in simulation steps)
    double initialDuration{60.0};
    double criticalPeriodDuration{240.0};
    double maturationDuration{480.0};
    
    // Memory and state
    size_t maxVisionMemory{1024};
    
    // Debug and logging
    bool enableDebugLogging{false};
    bool validateInputs{true};
    
    // Defaults for backward compatibility
    static AgentBrainConfig getDefaultConfig() {
        AgentBrainConfig config;
        return config;
    }
};

// Neural Group Classification for better organization
enum class NeuralGroupType {
    MotorForward,
    MotorBackward,
    MotorTurnLeft,
    MotorTurnRight,
    MotorInteract,
    MotorWait,
    Vision,
    Touch,
    Internal,
    Proprioception,
    Unknown
};

// AgentBrain: Connects NLM brain to the world
// Handles sensory transduction and motor decoding with improved architecture
class AgentBrain {
public:
    // Create agent brain with brain and configuration
    explicit AgentBrain(std::shared_ptr<Brain> brain, const AgentBrainConfig& config = AgentBrainConfig::getDefaultConfig());
    
    ~AgentBrain();
    
    // Disable copying, enable moving
    AgentBrain(const AgentBrain&) = delete;
    AgentBrain& operator=(const AgentBrain&) = delete;
    AgentBrain(AgentBrain&&) noexcept;
    AgentBrain& operator=(AgentBrain&&) noexcept;
    
    // Initialize with world
    bool initialize(const SimpleWorld& world);
    
    // Get sensory input size expected by brain
    size_t getSensoryInputSize() const;
    
    // Get motor output size expected
    size_t getMotorOutputSize() const;
    
    // Process sensory percept and inject into brain
    bool processSensoryInput(const SensoryPercept& percept);
    
    // Decode brain motor activity into motor command
    MotorCommand decodeMotorCommand();
    
    // Apply neuromodulation based on reward
    bool applyRewardModulation(float reward, float predictedReward);
    
    // Update development system
    void updateDevelopment(double timestep);
    
    // Get current developmental stage
    DevelopmentalStage getDevelopmentalStage() const;
    
    // Get neuromodulation level
    float getNeuromodulationLevel() const;
    
    // Get curiosity level
    float getCuriosityLevel() const;
    
    // Get novelty level
    float getNoveltyLevel() const;
    
    // Get prediction error
    float getPredictionError() const;
    
    // Reset agent for new episode
    void reset();
    
    // Get brain pointer
    Brain* getBrain() { return brain_.get(); }
    const Brain* getBrain() const { return brain_.get(); }
    
    // Configuration methods
    void enableRewardModulation(bool enable) { rewardModulationEnabled_ = enable; }
    void enableStructuralPlasticity(bool enable) { structuralPlasticityEnabled_ = enable; }
    void enableDevelopment(bool enable) { developmentEnabled_ = enable; }
    void enableCuriosity(bool enable) { curiosityEnabled_ = enable; }
    
    bool isRewardModulationEnabled() const { return rewardModulationEnabled_; }
    bool isStructuralPlasticityEnabled() const { return structuralPlasticityEnabled_; }
    bool isDevelopmentEnabled() const { return developmentEnabled_; }
    bool isCuriosityEnabled() const { return curiosityEnabled_; }
    
    // Configuration updates
    void setConfig(const AgentBrainConfig& config) { config_ = config; }
    const AgentBrainConfig& getConfig() const { return config_; }
    
    // Validation and debugging
    bool validate() const;
    void logStatus() const;
    
    // Performance optimization helpers
    void optimizeNeuralGroups();
    void preAllocateMemory();
    
    // Advanced features
    bool batchProcessSensoryInputs(const std::vector<SensoryPercept>& percepts);
    std::vector<MotorCommand> batchDecodeMotorCommands(size_t count);
    void enableAdvancedLearning(bool enable) { advancedLearningEnabled_ = enable; }
    bool isAdvancedLearningEnabled() const { return advancedLearningEnabled_; }
    
    // Statistical analysis
    struct NeuralStats {
        size_t totalNeurons{0};
        size_t activeNeurons{0};
        size_t firingNeurons{0};
        float averageFiringRate{0.0f};
        float excitationInhibitionRatio{1.0f};
        float curiosityLevel{0.0f};
        float noveltyLevel{0.0f};
        double developmentalAge{0.0};
        float plasticityModifier{1.0f};
    };
    
    NeuralStats getStatistics() const;
    
    // Debug and analysis
    void analyzeNeuralActivity() const;
    void profilePerformance() const;
    
private:
    // Motor decoding: convert neural activity to motor command
    MotorCommand decodeFromMotorNeurons();
    
    // Motor command selection with curiosity/exploration
    MotorCommand selectWithCuriosity(MotorCommand defaultCmd);
    
    // Helper methods
    void validateSensoryInput(const SensoryPercept& percept) const;
    void validateMotorOutput(const MotorCommand& command) const;
    void validateRewardValues(float reward, float predictedReward) const;
    float clampValue(float value, float min, float max) const;
    NeuralGroupType classifyNeuron(Neuron* neuron) const;
    
    // Neural group management
    void initializeNeuralGroups();
    void updateNeuralGroupClassifications();
    void optimizeGroupAssignments();
    
    // Sensory processing
    void processVisionInput(const std::vector<float>& vision);
    void processTouchInput(const std::vector<float>& touch);
    void processInternalInput(const std::vector<float>& internal);
    void processProprioceptionInput(const std::vector<float>& proprio);
    
    // Activity calculation
    float calculateGroupActivity(const std::vector<Neuron*>& neurons) const;
    std::vector<Neuron*> getNeuronsByType(NeuralGroupType type) const;
    
    // State management
    void updateDevelopmentalState(double timestep);
    void updatePlasticityState();
    void updateNeuromodulationState();
    
    // Configuration
    std::shared_ptr<Brain> brain_;
    AgentBrainConfig config_;
    
    // Neural group management
    std::unordered_map<NeuralGroupType, std::vector<Neuron*>> neuralGroups_;
    
    // Motor neuron groups (for backward compatibility)
    std::vector<Neuron*> motorForward_;
    std::vector<Neuron*> motorBackward_;
    std::vector<Neuron*> motorTurnLeft_;
    std::vector<Neuron*> motorTurnRight_;
    std::vector<Neuron*> motorInteract_;
    std::vector<Neuron*> motorWait_;
    
    // Sensory neuron groups (for backward compatibility)
    std::vector<Neuron*> sensoryVision_;
    std::vector<Neuron*> sensoryTouch_;
    std::vector<Neuron*> sensoryInternal_;
    std::vector<Neuron*> sensoryProprioception_;
    
    // Neuromodulation state
    float dopamineLevel_;
    float noveltyLevel_;
    float curiosityLevel_;
    float predictionError_;
    float expectedReward_;
    
    // Development state
    double developmentalAge_;
    float plasticityModifier_;
    
    // Configuration flags
    bool rewardModulationEnabled_;
    bool structuralPlasticityEnabled_;
    bool developmentEnabled_;
    bool curiosityEnabled_;
    bool advancedLearningEnabled_;
    
    // Previous sensory state for novelty detection
    std::vector<float> previousVision_;
    float sensoryNoveltyDecay_;
    
    // Performance optimization
    mutable bool groupsInitialized_;
    mutable size_t lastActivityCheckStep_;
    
    // Configuration constants for easy tuning
    static constexpr float DEFAULT_ACTIVATION_THRESHOLD{0.5f};
    static constexpr float DEFAULT_NOVELTY_THRESHOLD{0.1f};
    static constexpr float DEFAULT_EXPLORATION_THRESHOLD{0.5f};
    static constexpr float MAX_EXPLORATION_CHANCE{0.3f};
    static constexpr float MIN_EXPLORATION_CHANCE{0.05f};
    
    // Vision bounds checking
    size_t maxVisionSamples_{0};
    size_t maxTouchSamples_{8};
    size_t maxInternalSamples_{4};
    size_t maxProprioceptionSamples_{6};
    
    // Random number generator for exploration
    std::mt19937 rng_;
    std::uniform_real_distribution<float> uniformDist_;
    std::uniform_int_distribution<int> intDist_;
    
    // Statistics for performance monitoring
    mutable size_t totalProcessCalls_;
    mutable size_t totalDecodeCalls_;
    mutable double totalProcessingTime_;
};

} // namespace nlm
