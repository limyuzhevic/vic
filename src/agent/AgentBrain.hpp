#pragma once

#include "AgentBody.hpp"
#include "SensoryPercept.hpp"
#include "../brain/Brain.hpp"
#include "../world/SimpleWorld.hpp"
#include <memory>
#include <vector>

namespace nlm {

// Configuration structure for AgentBrain parameters
struct AgentBrainConfig {
    // Neuron distribution
    static constexpr size_t MOTOR_GROUPS = 6;
    static constexpr size_t SENSORY_GROUPS = 4;
    
    // Sensory processing scale factors
    static constexpr float VISION_SCALE = 5.0f;
    static constexpr float TOUCH_SCALE = 8.0f;
    static constexpr float INTERNAL_SCALE = 10.0f;
    static constexpr float PROPRIOCEPTION_SCALE = 6.0f;
    
    // Sensory input sizes
    static constexpr size_t VISION_SIZE = 256;  // 16x16
    static constexpr size_t TOUCH_SIZE = 8;
    static constexpr size_t INTERNAL_SIZE = 4;
    static constexpr size_t PROPRIOCEPTION_SIZE = 6;
    
    // Motor output size
    static constexpr size_t MOTOR_OUTPUT_SIZE = 6;
    
    // Neuromodulation parameters
    static constexpr float NOVELTY_DECAY_RATE = 0.99f;
    static constexpr float CURIOUS_FACTOR_NOVELTY = 2.0f;
    static constexpr float CURIOUS_FACTOR_PREDICTION = 0.5f;
    static constexpr float CURIOUS_THRESHOLD = 0.3f;
    static constexpr float EXPLORATION_THRESHOLD = 0.5f;
    
    // Reward modulation parameters
    static constexpr float DOSE_ALPHA = 1.0f;  // Simple exponential moving average
    static constexpr float DOSE_BETA = 0.05f;
    static constexpr float DOSE_MIN = -1.0f;
    static constexpr float DOSE_MAX = 1.0f;
    static constexpr float PLASTICITY_BASE = 0.5f;
    static constexpr float PLASTICITY_RANGE = 0.5f;
    
    // STDP parameters
    static constexpr float STDP_LTP_BASE = 0.01f;
    static constexpr float STDP_LTD_BASE = 0.012f;
    
    // Development parameters
    static constexpr double DEVELOPMENT_INITIAL = 60.0;  // seconds
    static constexpr double DEVELOPMENT_CRITICAL = 300.0;  // seconds
    static constexpr double DEVELOPMENT_MATURATION = 900.0;  // seconds
    static constexpr float PLASTICITY_INITIAL = 1.0f;
    static constexpr float PLASTICITY_CRITICAL = 0.8f;
    static constexpr float PLASTICITY_MATURATION = 0.5f;
    static constexpr float PLASTICITY_ADULT = 0.2f;
    
    // Structural plasticity rates
    static constexpr float SYNAPTONGENESIS_BASE = 0.0001f;
    static constexpr float PRUNING_BASE = 0.00001f;
    
    // Activity threshold
    static constexpr float MOTOR_ACTIVITY_THRESHOLD = 0.5f;
    
    // Exploration parameters
    static constexpr float EXPLORATION_PROBABILITY = 0.3f;
    static constexpr size_t MOTOR_COMMAND_COUNT = 8;
};

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
    
private:
    // Configuration parameters
    static constexpr size_t MOTOR_GROUPS = 6;
    static constexpr size_t SENSORY_GROUPS = 4;
    
    // Sensory processing scale factors
    static constexpr float VISION_SCALE = 5.0f;
    static constexpr float TOUCH_SCALE = 8.0f;
    static constexpr float INTERNAL_SCALE = 10.0f;
    static constexpr float PROPRIOCEPTION_SCALE = 6.0f;
    
    // Sensory input sizes
    static constexpr size_t VISION_SIZE = 256;  // 16x16
    static constexpr size_t TOUCH_SIZE = 8;
    static constexpr size_t INTERNAL_SIZE = 4;
    static constexpr size_t PROPRIOCEPTION_SIZE = 6;
    
    // Motor output size
    static constexpr size_t MOTOR_OUTPUT_SIZE = 6;
    
    // Neuromodulation parameters
    static constexpr float NOVELTY_DECAY_RATE = 0.99f;
    static constexpr float CURIOUS_FACTOR_NOVELTY = 2.0f;
    static constexpr float CURIOUS_FACTOR_PREDICTION = 0.5f;
    static constexpr float CURIOUS_THRESHOLD = 0.3f;
    static constexpr float EXPLORATION_THRESHOLD = 0.5f;
    
    // Reward modulation parameters
    static constexpr float DOSE_ALPHA = 1.0f;  // Simple exponential moving average
    static constexpr float DOSE_BETA = 0.05f;
    static constexpr float DOSE_MIN = -1.0f;
    static constexpr float DOSE_MAX = 1.0f;
    static constexpr float PLASTICITY_BASE = 0.5f;
    static constexpr float PLASTICITY_RANGE = 0.5f;
    
    // STDP parameters
    static constexpr float STDP_LTP_BASE = 0.01f;
    static constexpr float STDP_LTD_BASE = 0.012f;
    
    // Development parameters
    static constexpr double DEVELOPMENT_INITIAL = 60.0;  // seconds
    static constexpr double DEVELOPMENT_CRITICAL = 300.0;  // seconds
    static constexpr double DEVELOPMENT_MATURATION = 900.0;  // seconds
    static constexpr float PLASTICITY_INITIAL = 1.0f;
    static constexpr float PLASTICITY_CRITICAL = 0.8f;
    static constexpr float PLASTICITY_MATURATION = 0.5f;
    static constexpr float PLASTICITY_ADULT = 0.2f;
    
    // Structural plasticity rates
    static constexpr float SYNAPTONGENESIS_BASE = 0.0001f;
    static constexpr float PRUNING_BASE = 0.00001f;
    
    // Activity threshold
    static constexpr float MOTOR_ACTIVITY_THRESHOLD = 0.5f;
    
    // Exploration parameters
    static constexpr float EXPLORATION_PROBABILITY = 0.3f;
    static constexpr size_t MOTOR_COMMAND_COUNT = 8;
    
    // Initialize neuron distribution
    void initializeNeuronDistribution();
    
    // Distribute neurons to motor and sensory groups
    void distributeMotorNeurons();
    void distributeSensoryNeurons();
    void distributeMotorPopulation(const std::vector<Neuron*>& neurons);
    void distributeSensoryPopulation(const std::vector<Neuron*>& neurons);
    
    // Sensory processing methods
    void processVisionInput(const SensoryPercept& percept);
    void processTouchInput(const SensoryPercept& percept);
    void processInternalInput(const SensoryPercept& percept);
    void processProprioceptionInput(const SensoryPercept& percept);
    void computeNovelty(const SensoryPercept& percept);
    void updateCuriosity();
    
    // Motor processing methods
    MotorCommand selectBestCommand(float forwardAct, float backwardAct, float leftAct,
                                 float rightAct, float interactAct, float waitAct);
    float calculateMotorActivity(const std::vector<Neuron*>& neurons);
    MotorCommand getRandomMotorCommand();
    
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
