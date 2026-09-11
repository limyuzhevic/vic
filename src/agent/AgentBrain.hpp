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
    // Constants for sensory processing
    static constexpr float VISION_INPUT_SCALE_FACTOR = 5.0f;
    static constexpr float TOUCH_INPUT_SCALE_FACTOR = 8.0f;
    static constexpr float INTERNAL_INPUT_CENTER = 1.0f;
    static constexpr float INTERNAL_INPUT_SCALE = 2.0f;
    static constexpr float PROPRIOCEPTION_INPUT_CENTER = 1.0f;
    static constexpr float PROPRIOCEPTION_INPUT_SCALE = 2.0f;
    static constexpr float PROPRIOCEPTION_OUTPUT_SCALE = 3.0f;
    
    // Constants for neuromodulation and learning
    static constexpr float NOVELTY_DETECTION_DECAY_RATE = 0.99f;
    static constexpr float NOVELTY_NORMALIZATION_DIVISOR = 1.0f;
    static constexpr float CURIOUSITY_WEIGHT_NOVELTY = 2.0f;
    static constexpr float CURIOUSITY_WEIGHT_PREDICTION_ERROR = 0.5f;
    static constexpr float CURIOUSITY_THRESHOLD_FOR_EXPLORATION = 0.3f;
    static constexpr float MAX_CURIOUSITY_LEVEL = 1.0f;
    
    // Constants for exploration behavior
    static constexpr float EXPLORATION_CURIOUSITY_THRESHOLD = 0.5f;
    static constexpr float EXPLORATION_PROBABILITY_MAX = 0.3f;
    static constexpr int EXPLORATION_RANDOM_CHOICE_MIN = 0;
    static constexpr int EXPLORATION_RANDOM_CHOICE_MAX = 7;
    
    // Constants for reward modulation and plasticity
    static constexpr float ELIGIBILITY_TRACE_THRESHOLD = 0.001f;
    static constexpr float ELIGIBILITY_TRACE_DECAY_RATE = 0.1f;
    static constexpr float PLASTICITY_BASE_FACTOR = 0.5f;
    static constexpr float PLASTICITY_MAX_FACTOR = 2.0f;
    static constexpr float PLASTICITY_MIN_FACTOR = 0.1f;
    static constexpr float STDP_POTENTIATION_BASE_WEIGHT = 0.01f;
    static constexpr float STDP_DEPRESSION_BASE_WEIGHT = 0.012f;
    static constexpr float EXPECTED_REWARD_DECAY_FACTOR = 0.95f;
    static constexpr float EXPECTED_REWARD_LEARNING_RATE = 0.05f;
    
    // Constants for development and structural plasticity
    static constexpr float SYNAPTOGENESIS_BASE_RATE = 0.0001f;
    static constexpr float PRUNING_BASE_RATE = 0.00001f;
    
    // Constants for developmental stages
    static constexpr double DEVELOPMENTAL_STAGE_CRITICAL_PERIOD_START = 60.0;
    static constexpr double DEVELOPMENTAL_STAGE_MATURATION_END = 300.0;
    static constexpr double DEVELOPMENTAL_STAGE_ADULT_END = 900.0;
    
    // Constants for stage-specific plasticity modifiers
    static constexpr float STAGE_INITIAL_PLASTICITY = 1.0f;
    static constexpr float STAGE_CRITICAL_PERIOD_PLASTICITY = 0.8f;
    static constexpr float STAGE_MATURATION_PLASTICITY = 0.5f;
    static constexpr float STAGE_ADULT_PLASTICITY = 0.2f;
    
    // Constants for motor decoding
    static constexpr float MOTOR_ACTIVITY_THRESHOLD = 0.5f;
    static constexpr float MOTOR_ACTIVITY_RESTING_DEVIATION = 0.0f;
    
    // Motor command look commands
    static constexpr int MOTOR_COMMAND_LOOK_LEFT = 4;
    static constexpr int MOTOR_COMMAND_LOOK_RIGHT = 5;
    
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
};

} // namespace nlm
