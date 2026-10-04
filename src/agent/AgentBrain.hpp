#pragma once

#include "AgentBody.hpp"
#include "SensoryPercept.hpp"
#include "../brain/Brain.hpp"
#include "../world/SimpleWorld.hpp"
#include <memory>
#include <vector>
#include <string>

namespace nlm {

// AgentBrain: Connects NLM brain to the world
// Handles sensory transduction and motor decoding

class AgentBrain;

// Enhanced neural group structure to replace parallel vectors
struct NeuralGroups {
    std::vector<Neuron*> motorForward_;
    std::vector<Neuron*> motorBackward_;
    std::vector<Neuron*> motorTurnLeft_;
    std::vector<Neuron*> motorTurnRight_;
    std::vector<Neuron*> motorInteract_;
    std::vector<Neuron*> motorWait_;
    std::vector<Neuron*> sensoryVision_;
    std::vector<Neuron*> sensoryTouch_;
    std::vector<Neuron*> sensoryInternal_;
    std::vector<Neuron*> sensoryProprioception_;
    
    // Helper methods for cleaner code
    void clearAll() {
        motorForward_.clear(); motorBackward_.clear(); motorTurnLeft_.clear();
        motorTurnRight_.clear(); motorInteract_.clear(); motorWait_.clear();
        sensoryVision_.clear(); sensoryTouch_.clear(); sensoryInternal_.clear();
        sensoryProprioception_.clear();
    }
    
    size_t totalMotorNeurons() const {
        return motorForward_.size() + motorBackward_.size() + motorTurnLeft_.size() + 
               motorTurnRight_.size() + motorInteract_.size() + motorWait_.size();
    }
    
    size_t totalSensoryNeurons() const {
        return sensoryVision_.size() + sensoryTouch_.size() + sensoryInternal_.size() +
               sensoryProprioception_.size();
    }
};

// Configuration constants for neural simulation
namespace Constants {
    // Motor decoding thresholds and scaling
    constexpr float MOTOR_DECODE_THRESHOLD = 0.5f;           // Minimum activity to trigger action
    constexpr float MOTOR_ACTIVITY_WEIGHT = 1.0f;            // Weight for activity calculation
    constexpr float NEURON_ACTIVITY_SCALE = 5.0f;            // Scale factor for vision input
    constexpr float TOUCH_ACTIVITY_SCALE = 8.0f;            // Scale factor for touch input
    constexpr float INTERNAL_ACTIVITY_SCALE = 5.0f;         // Scale factor for internal signals
    constexpr float PROPRIOCEPTION_ACTIVITY_SCALE = 3.0f;   // Scale factor for proprioception
    
    constexpr float DEFAULT_DOPAMINE_LEVEL = 0.0f;          // Base dopamine level
    constexpr float MAX_DOPAMINE_LEVEL = 1.0f;              // Maximum dopamine level
    constexpr float MIN_DOPAMINE_LEVEL = -1.0f;             // Minimum dopamine level
    constexpr float EXPECTED_REWARD_DECAY = 0.95f;          // EMA decay factor
    
    // Curiosity and novelty parameters
    constexpr float CURIOSITY_NOVELTY_MULTIPLIER = 2.0f;     // Curiosity = novelty * multiplier
    constexpr float CURIOSITY_PREDICTION_ERROR_WEIGHT = 0.5f; // Curiosity = novelty * + predictionError * weight
    constexpr float MAX_CURIOSITY_LEVEL = 1.0f;             // Maximum curiosity level
    constexpr float CURIOSITY_EXPLORATION_THRESHOLD = 0.5f; // Curiosity level above which exploration happens
    constexpr float MAX_EXPLORATION_CHANCE = 0.3f;          // Maximum random exploration chance
    
    // Novelty detection
    constexpr float NOVELTY_DECAY_FACTOR = 0.99f;           // How quickly novelty decays
    constexpr float NOVELTY_NORMALIZATION_DIVISOR = 1.0f;   // For normalization (will be divided by actual size)
    
    // Development system
    constexpr double DEVELOPMENT_INITIAL_STAGE = 60.0;     // Initial development stage (seconds)
    constexpr double DEVELOPMENT_CRITICAL_PERIOD_START = 60.0;    // Start of critical period
    constexpr double DEVELOPMENT_CRITICAL_PERIOD_END = 300.0;      // End of critical period
    constexpr double DEVELOPMENT_MATURATION_START = 300.0;         // Start of maturation phase
    constexpr double DEVELOPMENT_MATURATION_END = 900.0;  // End of maturation phase
    constexpr float PLASTICITY_INITIAL = 1.0f;               // Initial plasticity level
    constexpr float PLASTICITY_CRITICAL_PERIOD = 0.8f;       // Plasticity during critical period
    constexpr float PLASTICITY_MATURATION = 0.5f;            // Plasticity during maturation
    constexpr float PLASTICITY_ADULT = 0.2f;                 // Adult plasticity level
    
    // Structural plasticity
    constexpr float SYNAPTAGENESIS_RATE_INITIAL = 0.0001f;   // Initial synaptogenesis rate
    constexpr float SYNAPTAGENESIS_RATE_REDUCED = 0.00005f;  // Reduced synaptogenesis rate
    constexpr float PRUNING_RATE_INITIAL = 0.00001f;         // Initial pruning rate
    constexpr float PRUNING_RATE_MAX = 0.00002f;             // Maximum pruning rate
    
    // Reward and prediction
    constexpr float PREDICTION_ERROR_WEIGHT = 1.0f;          // Weight for prediction error
    constexpr float PLASTICITY_FACTOR_BASE = 0.5f;           // Base plasticity factor
    constexpr float PLASTICITY_FACTOR_RANGE = 0.5f;          // Range for plasticity factor adjustment
    constexpr float STDP_LTP_BASE = 0.01f;                   // Base LTP weight
    constexpr float STDP_LTD_BASE = 0.012f;                  // Base LTD weight
    
    // Simulation constants
    constexpr double DEFAULT_SIMULATION_TIMESTEP = 0.001;   // Default simulation timestep

}

// Motor command helper functions
namespace MotorCommands {
    MotorCommand CalculateBestCommand(float forwardAct, float backwardAct,
                                     float leftAct, float rightAct,
                                     float interactAct, float waitAct);
    
    MotorCommand GenerateExplorationCommand(std::shared_ptr<Brain> brain, 
                                           float curiosityLevel);
}

// AgentBrain: Connects NLM brain to the world
// Handles sensory transduction and motor decoding
class AgentBrain {
public:
    AgentBrain(std::shared_ptr<Brain> brain);
    ~AgentBrain();
    
    // Disable copying, enable moving
    AgentBrain(const AgentBrain&) = delete;
    AgentBrain& operator=(const AgentBrain&) = delete;
    AgentBrain(AgentBrain&&) noexcept = default;
    AgentBrain& operator=(AgentBrain&&) noexcept = default;
    
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
    const Brain* getBrain() const { return brain_.get(); }
    
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
    std::shared_ptr<Brain> brain_;
    
    // Enhanced neural group structure
    NeuralGroups neuralGroups_;
    
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
    
    // Update neural groups structure
    void updateNeuralGroups();
};

} // namespace nlm
