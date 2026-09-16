#pragma once

#include "AgentBody.hpp"
#include "SensoryPercept.hpp"
#include "../brain/Brain.hpp"
#include "../world/SimpleWorld.hpp"
#include <memory>
#include <vector>

namespace nlm {

// AgentBrain: Connects NLM brain to the world
// Handles sensory transduction and motor decoding
class AgentBrain {
public:
    AgentBrain(std::shared_ptr<Brain> brain);
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
    
    // Configuration struct for external configuration
    struct Config {
        bool rewardModulationEnabled = true;
        bool structuralPlasticityEnabled = true;
        bool developmentEnabled = true;
        bool curiosityEnabled = true;
        float sensoryNoveltyDecay = 0.99f;
        
        // Sensor neuron distribution (per 100 neurons)
        size_t visionNeuronCount = 25;    // 25% of neurons for vision
        size_t touchNeuronCount = 8;      // 8 neurons for touch
        size_t internalNeuronCount = 4;   // 4 neurons for internal
        size_t proprioceptionNeuronCount = 6; // 6 neurons for proprioception
    };
    
    // Legacy AgentBrainConfig type for backward compatibility
    using AgentBrainConfig = Config;

public:
    // Motor decoder constants
    static constexpr float MOTOR_ACTIVITY_THRESHOLD = 0.5f;
    static constexpr float MOTOR_CURIOUSITY_THRESHOLD = 0.3f;
    static constexpr float MOTOR_EXPLORATION_CHANCE_FACTOR = 0.3f;
    static constexpr float MOTOR_EXPLORATION_CURiosity_THRESHOLD = 0.5f;
    static constexpr size_t MOTOR_RANDOM_ACTION_RANGE = 7;
    
    // Sensory input scaling constants
    static constexpr float VISION_CURRENT_SCALE = 5.0f;
    static constexpr float TOUCH_CURRENT_SCALE = 8.0f;
    static constexpr float INTERNAL_CURRENT_SCALE = 5.0f;
    static constexpr float INTERNAL_CURRENT_CENTER = 1.0f;
    static constexpr float PROPRIOCEPTION_CURRENT_SCALE = 3.0f;
    
    // Neuromodulation constants
    static constexpr float NEUROMODULATION_CLAMP_MIN = -1.0f;
    static constexpr float NEUROMODULATION_CLAMP_MAX = 1.0f;
    static constexpr float NEUROMODULATION_WEIGHT_CHANGE_CLIP = 0.001f;
    static constexpr float NEUROMODULATION_PLATYSITY_FACTOR_MIN = 0.1f;
    static constexpr float NEUROMODULATION_PLATYSITY_FACTOR_MAX = 2.0f;
    
    // Development constants
    static constexpr double DEVELOPMENT_STAGE_1_DURATION = 60.0;
    static constexpr double DEVELOPMENT_STAGE_2_DURATION = 300.0; // 60 + 240
    static constexpr double DEVELOPMENT_STAGE_3_DURATION = 900.0; // 300 + 600
    static constexpr float INITIAL_PLASTICITY = 1.0f;
    static constexpr float CRITICAL_PERIOD_PLASTICITY = 0.8f;
    static constexpr float MATURATION_PLASTICITY = 0.5f;
    static constexpr float ADULT_PLASTICITY = 0.2f;
    
    // Memory system constants
    static constexpr float NOVELTY_DECAY_RATE = 0.99f;
    static constexpr float NOVELTY_MULTIPLIER = 2.0f;
    static constexpr float PREDICTION_ERROR_MULTIPLIER = 0.5f;
    static constexpr float MAX_NOVELTY_LEVEL = 1.0f;
    
    // Reward modulation constants
    static constexpr float EXPECTED_REWARD_SMOOTHING = 0.95f;
    static constexpr float EXPECTED_REWARD_LEARNING_RATE = 0.05f;
    static constexpr float DOPAMINE_PREDICTION_ERROR_SCALE = 1.0f;
    static constexpr float PLASTICITY_MODULATION_BASE = 0.5f;
    static constexpr float PLASTICITY_MODULATION_AMPLITUDE = 0.5f;
    
    // Structural plasticity constants
    static constexpr float SYNAPTOGENESIS_BASE_RATE = 0.0001f;
    static constexpr float PRUNING_BASE_RATE = 0.00001f;
    static constexpr float PLASTICITY_MODIFIER_RANGE = 2.0f;

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
    
    // Previous sensory state for novelty detection
    std::vector<float> previousVision_;
    float sensoryNoveltyDecay_;
    
    // Configuration for sensor neuron distribution
    size_t visionNeuronCount_;
    size_t touchNeuronCount_;
    size_t internalNeuronCount_;
    size_t proprioceptionNeuronCount_;
};

} // namespace nlm