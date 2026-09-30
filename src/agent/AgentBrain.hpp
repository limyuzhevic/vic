#pragma once

#include "AgentBody.hpp"
#include "SensoryPercept.hpp"
#include "../brain/Brain.hpp"
#include "../world/SimpleWorld.hpp"
#include <memory>
#include <vector>

namespace nlm {

// AgentBrain Configuration Constants
namespace AgentBrainConfig {
    // Curiosity and exploration thresholds
    constexpr float CURIOUS_THRESHOLD = 0.3f;           // Curiosity level needed for exploration
    constexpr float EXPLORATION_THRESHOLD = 0.5f;        // Curiosity level needed for random actions
    constexpr float EXPLORATION_CHANCE_MAX = 0.3f;       // Maximum exploration probability
    
    // Motor decoding thresholds
    constexpr float MOTOR_ACTIVATION_THRESHOLD = 0.5f;   // Minimum activity for action selection
    constexpr float NOISE_THRESHOLD = 0.01f;             // Eligibility trace threshold
    
    // Neuromodulation parameters
    constexpr float REWARD_LEARNING_RATE = 0.05f;        // Expected reward learning rate
    constexpr float REWARD_DECAY_RATE = 0.95f;           // Expected reward decay
    constexpr float DOPAMINE_CLAMP_MIN = -1.0f;          // Minimum dopamine level
    constexpr float DOPAMINE_CLAMP_MAX = 1.0f;           // Maximum dopamine level
    constexpr float PLASTICITY_BASE_FACTOR = 0.5f;        // Base plasticity factor
    constexpr float PLASTICITY_MODIFIER_MIN = 0.1f;       // Minimum plasticity
    constexpr float PLASTICITY_MODIFIER_MAX = 2.0f;       // Maximum plasticity
    constexpr float STDP_LTP_WEIGHT_BASE = 0.01f;         // Base STDP LTP weight
    constexpr float STDP_LTD_WEIGHT_BASE = 0.012f;        // Base STDP LTD weight
    
    // Development parameters
    constexpr double DEVELOPMENT_STAGE_1_MIN = 60.0;     // Initial stage duration (~1 minute)
    constexpr double DEVELOPMENT_STAGE_2_MIN = 300.0;    // Critical period start (~5 minutes)
    constexpr double DEVELOPMENT_STAGE_3_MIN = 900.0;    // Maturation start (~15 minutes)
    constexpr float PLASTICITY_INITIAL = 1.0f;            // Initial plasticity
    constexpr float PLASTICITY_CRITICAL = 0.8f;          // Critical period plasticity
    constexpr float PLASTICITY_MATURING = 0.5f;          // Maturation plasticity
    constexpr float PLASTICITY_ADULT = 0.2f;              // Adult plasticity
    constexpr float SYNAPTOGENESIS_RATE_BASE = 0.0001f;   // Base synaptogenesis rate
    constexpr float PRUNING_RATE_BASE = 0.00001f;        // Base pruning rate
    
    // Sensory processing parameters
    constexpr float VISION_SCALE_FACTOR = 5.0f;           // Vision signal scaling
    constexpr float TOUCH_SCALE_FACTOR = 8.0f;            // Touch signal scaling
    constexpr float INTERNAL_SCALE_FACTOR = 5.0f;         // Internal signal scaling
    constexpr float PROPRIOCEPTION_SCALE_FACTOR = 3.0f;   // Proprioception scaling
    constexpr float NOVELTY_DECAY_RATE = 0.99f;           // Novelty decay
    
    // Curiosity calculation parameters
    constexpr float NOVELTY_CURRENCY = 2.0f;              // Novelty weight in curiosity
    constexpr float PREDICTION_ERROR_CURRENCY = 0.5f;     // Prediction error weight in curiosity
    
    // Motor group distribution
    constexpr size_t MOTOR_GROUP_COUNT = 6;              // Forward, backward, left, right, interact, wait
    constexpr size_t SENSORY_GROUP_COUNT = 4;             // Vision, touch, internal, proprioception
    
    // Default sensory sizes
    constexpr size_t VISION_WIDTH = 16;                  // Vision grid width
    constexpr size_t VISION_HEIGHT = 16;                 // Vision grid height
    constexpr size_t TOUCH_SENSORS = 8;                   // Touch sensor count
    constexpr size_t INTERNAL_SENSORS = 4;                // Internal sensor count
    constexpr size_t PROPRIOCEPTION_SENSORS = 6;          // Proprioception sensor count
    constexpr size_t AUDIO_SENSORS = 0;                   // Audio sensor count (currently unused)
}

// AgentBrain: Connects NLM brain to the world
// Handles sensory transduction and motor decoding
class AgentBrain {
public:
    // Constructor with configuration
    AgentBrain(std::shared_ptr<Brain> brain,
               float curiosityThreshold = AgentBrainConfig::CURIOUS_THRESHOLD,
               float explorationChanceMax = AgentBrainConfig::EXPLORATION_CHANCE_MAX,
               float motorActivationThreshold = AgentBrainConfig::MOTOR_ACTIVATION_THRESHOLD,
               float neuromodulationLearningRate = AgentBrainConfig::REWARD_LEARNING_RATE,
               float plasticityBaseFactor = AgentBrainConfig::PLASTICITY_BASE_FACTOR,
               float noveltyDecayRate = AgentBrainConfig::NOVELTY_DECAY_RATE);
    ~AgentBrain();
    
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
    
    // Get current configuration
    float getCuriosityThreshold() const { return curiosityThreshold_; }
    float getExplorationChanceMax() const { return explorationChanceMax_; }
    float getMotorActivationThreshold() const { return motorActivationThreshold_; }
    float getNeuromodulationLearningRate() const { return neuromodulationLearningRate_; }
    float getPlasticityBaseFactor() const { return plasticityBaseFactor_; }
    float getNoveltyDecayRate() const { return noveltyDecayRate_; }
    
    // Set configuration values
    void setCuriosityThreshold(float threshold) { curiosityThreshold_ = threshold; }
    void setExplorationChanceMax(float chance) { explorationChanceMax_ = chance; }
    void setMotorActivationThreshold(float threshold) { motorActivationThreshold_ = threshold; }
    void setNeuromodulationLearningRate(float rate) { neuromodulationLearningRate_ = rate; }
    void setPlasticityBaseFactor(float factor) { plasticityBaseFactor_ = factor; }
    void setNoveltyDecayRate(float rate) { noveltyDecayRate_ = rate; }
    
    // Reset to default configuration
    void resetToDefaults();
    
private:
    // Motor decoding: convert neural activity to motor command
    MotorCommand decodeFromMotorNeurons();
    
    // Motor command selection with curiosity/exploration
    MotorCommand selectWithCuriosity(MotorCommand defaultCmd);
    
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
    
    // Configuration parameters
    float curiosityThreshold_;
    float explorationChanceMax_;
    float motorActivationThreshold_;
    float neuromodulationLearningRate_;
    float plasticityBaseFactor_;
    float noveltyDecayRate_;
    
    // Previous sensory state for novelty detection
    std::vector<float> previousVision_;
    float sensoryNoveltyDecay_;
};

} // namespace nlm
