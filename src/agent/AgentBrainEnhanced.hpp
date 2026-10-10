// AgentBrain.hpp improvements - Enhanced AgentBrain implementation
#pragma once

#include "AgentBody.hpp"
#include "SensoryPercept.hpp"
#include "../brain/Brain.hpp"
#include "../world/SimpleWorld.hpp"
#include <memory>
#include <vector>
#include <algorithm>
#include <cmath>

namespace nlm {

// Helper class for sensory processing configuration
struct SensoryConfig {
    float visionScale = 5.0f;       // Scale factor for vision input
    float touchScale = 8.0f;        // Scale factor for touch input
    float internalScale = 5.0f;     // Scale factor for internal signals
    float proprioceptionScale = 3.0f; // Scale factor for proprioception
    
    // Configuration methods
    SensoryConfig& setVisionScale(float scale) { visionScale = scale; return *this; }
    SensoryConfig& setTouchScale(float scale) { touchScale = scale; return *this; }
    SensoryConfig& setInternalScale(float scale) { internalScale = scale; return *this; }
    SensoryConfig& setProprioceptionScale(float scale) { proprioceptionScale = scale; return *this; }
};

// Helper class for motor processing configuration
struct MotorConfig {
    float curiosityThreshold = 0.3f;    // Curiosity level needed for exploration
    float curiosityExplorationChance = 0.3f;  // Max exploration probability
    float curiosityLevelThreshold = 0.5f;      // Min curiosity for exploration
    float activityThreshold = 0.5f;     // Minimum activity to trigger action
    
    MotorConfig& setCuriosityThreshold(float thresh) { curiosityThreshold = thresh; return *this; }
    MotorConfig& setCuriosityExplorationChance(float chance) { curiosityExplorationChance = chance; return *this; }
    MotorConfig& setCuriosityLevelThreshold(float thresh) { curiosityLevelThreshold = thresh; return *this; }
    MotorConfig& setActivityThreshold(float thresh) { activityThreshold = thresh; return *this; }
};

// Helper class for neuromodulation configuration
struct NeuromodulationConfig {
    float expectedRewardDecay = 0.95f;  // Exponential decay factor for expected reward
    float rewardLearningRate = 0.05f;   // Learning rate for reward prediction
    float eligibilityTraceDecay = 0.1f; // Decay rate for eligibility traces
    float plasticityBase = 0.5f;        // Base plasticity level
    float plasticityRange = 1.0f;       // Range of plasticity modulation
    
    NeuromodulationConfig& setExpectedRewardDecay(float decay) { expectedRewardDecay = decay; return *this; }
    NeuromodulationConfig& setRewardLearningRate(float rate) { rewardLearningRate = rate; return *this; }
    NeuromodulationConfig& setEligibilityTraceDecay(float decay) { eligibilityTraceDecay = decay; return *this; }
    NeuromodulationConfig& setPlasticityBase(float base) { plasticityBase = base; return *this; }
    NeuromodulationConfig& setPlasticityRange(float range) { plasticityRange = range; return *this; }
};

// Enhanced AgentBrain with better modularity and configuration
class AgentBrainEnhanced {
public:
    AgentBrainEnhanced(std::shared_ptr<Brain> brain,
                      const SensoryConfig& sensoryConfig = SensoryConfig(),
                      const MotorConfig& motorConfig = MotorConfig(),
                      const NeuromodulationConfig& neuromodConfig = NeuromodulationConfig());
    
    ~AgentBrainEnhanced();
    
    // Initialize with world
    void initialize(const SimpleWorld& world);
    
    // Get sensory input size expected by brain
    size_t getSensoryInputSize() const;
    
    // Get motor output size expected
    size_t getMotorOutputSize() const;
    
    // Process sensory percept and inject into brain
    void processSensoryInput(const SensoryPercept& percept);
    
    // Decode brain motor activity into motor command
    MotorCommand decodeMotorCommand();
    
    // Apply neuromodulation based on reward
    void applyRewardModulation(float reward, float predictedReward);
    
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
    
    // Configuration
    void enableRewardModulation(bool enable) { rewardModulationEnabled_ = enable; }
    void enableStructuralPlasticity(bool enable) { structuralPlasticityEnabled_ = enable; }
    void enableDevelopment(bool enable) { developmentEnabled_ = enable; }
    void enableCuriosity(bool enable) { curiosityEnabled_ = enable; }
    
    bool isRewardModulationEnabled() const { return rewardModulationEnabled_; }
    bool isStructuralPlasticityEnabled() const { return structuralPlasticityEnabled_; }
    bool isDevelopmentEnabled() const { return developmentEnabled_; }
    bool isCuriosityEnabled() const { return curiosityEnabled_; }
    
    // Configuration getters
    const SensoryConfig& getSensoryConfig() const { return sensoryConfig_; }
    const MotorConfig& getMotorConfig() const { return motorConfig_; }
    const NeuromodulationConfig& getNeuromodulationConfig() const { return neuromodConfig_; }
    
    // Setters for configuration
    void setSensoryConfig(const SensoryConfig& config) { sensoryConfig_ = config; }
    void setMotorConfig(const MotorConfig& config) { motorConfig_ = config; }
    void setNeuromodulationConfig(const NeuromodulationConfig& config) { neuromodConfig_ = config; }
    
private:
    // Motor decoding: convert neural activity to motor command
    MotorCommand decodeFromMotorNeurons();
    
    // Motor command selection with curiosity/exploration
    MotorCommand selectWithCuriosity(MotorCommand defaultCmd);
    
    // Helper function for calculating neuron activity
    static float calculateNeuronActivity(const std::vector<Neuron*>& neurons);
    
    // Random motor command selection
    MotorCommand getRandomMotorCommand() const;
    
    // Neuromodulation helper methods
    float calculatePlasticityFactor(float dopamineLevel) const;
    void applyRewardToSynapses(float reward);
    
    // Development helper methods
    void updateDevelopmentalStage(double timestep);
    void updateStructuralPlasticity(float plasticityModifier);
    
    // State management
    void clearPreviousVision();
    
    std::shared_ptr<Brain> brain_;
    
    // Motor neuron groups
    std::vector<Neuron*> motorForward_;
    std::vector<Neuron*> motorBackward_;
    std::vector<Neuron*> motorTurnLeft_;
    std::vector<Neuron*> motorTurnRight_;
    std::vector<Neuron*> motorInteract_;
    std::vector<Neuron*> motorWait_;
    
    // Sensory neuron groups
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
    float sensoryNoveltyDecay_;
    
    // Previous sensory state for novelty detection
    std::vector<float> previousVision_;
    
    // Configuration objects
    SensoryConfig sensoryConfig_;
    MotorConfig motorConfig_;
    NeuromodulationConfig neuromodConfig_;
};

} // namespace nlm
