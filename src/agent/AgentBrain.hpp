// AgentBrain.hpp - improved with better memory management and constants
#pragma once

#include "AgentBody.hpp"
#include "SensoryPercept.hpp"
#include "../brain/Brain.hpp"
#include "../world/SimpleWorld.hpp"
#include <memory>
#include <vector>
#include <algorithm>
#include "../core/Config/ConfigConstants.hpp"

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
    void enableExploration(bool enable) { explorationEnabled_ = enable; }
    
    bool isRewardModulationEnabled() const { return rewardModulationEnabled_; }
    bool isStructuralPlasticityEnabled() const { return structuralPlasticityEnabled_; }
    bool isDevelopmentEnabled() const { return developmentEnabled_; }
    bool isCuriosityEnabled() const { return curiosityEnabled_; }
    bool isExplorationEnabled() const { return explorationEnabled_; }
    
private:
    // Motor decoding: convert neural activity to motor command
    MotorCommand decodeFromMotorNeurons();
    
    // Motor command selection with curiosity/exploration
    MotorCommand selectWithCuriosity(MotorCommand defaultCmd);
    
    // === IMPROVEMENTS ===
    // Use smart pointers for better memory management
    std::shared_ptr<Brain> brain_;
    
    // Use const references where appropriate
    static constexpr size_t MOTOR_GROUP_COUNT = 6;
    static constexpr size_t SENSORY_GROUP_COUNT = 4;
    static constexpr size_t MAX_EXPLORATION_ACTIONS = 7;
    
    // Motor neuron groups (using pointers to brain neurons)
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
    bool explorationEnabled_;
    
    // Previous sensory state for novelty detection
    std::vector<float> previousVision_;
    float sensoryNoveltyDecay_;
    
    // === NEW: Type-safe constants ===
    static constexpr float NEURAL_INJECTION_SCALE = 5.0f;
    static constexpr float COLLISION_SCALE = 8.0f;
    static constexpr float INTERNAL_SCALE = 5.0f;
    static constexpr float PROPRIOCEPTION_SCALE = 3.0f;
    static constexpr float NOVELTY_WEIGHT = 2.0f;
    static constexpr float PREDICTION_ERROR_WEIGHT = 0.5f;
    static constexpr float CURIOUSITY_THRESHOLD = 0.3f;
    static constexpr float EXPLORATION_THRESHOLD = 0.5f;
    static constexpr float EXPLORATION_CHANCE = 0.3f;
    static constexpr float MIN_ACTIVITY_FOR_ACTION = 0.5f;
    static constexpr float REWARD_PREDICTION_DECAY = 0.95f;
    static constexpr float REWARD_LEARNING_RATE = 0.05f;
    static constexpr float DOPAMINE_CLAMP_MIN = -1.0f;
    static constexpr float DOPAMINE_CLAMP_MAX = 1.0f;
    static constexpr float ELIGIBILITY_TRACE_THRESHOLD = 0.001f;
    static constexpr float ELIGIBILITY_TRACE_DECAY = 0.1f;
    static constexpr float PLASTICITY_FACTOR_MIN = 0.1f;
    static constexpr float PLASTICITY_FACTOR_MAX = 2.0f;
    static constexpr float STDP_LTP_BASE_WEIGHT = 0.01f;
    static constexpr float STDP_LTD_BASE_WEIGHT = 0.012f;
    static constexpr float DEVELOPMMENT_STAGE_INITIAL = 60.0f;
    static constexpr float DEVELOPMMENT_STAGE_CRITICAL = 300.0f;
    static constexpr float DEVELOPMMENT_STAGE_MATURATION = 900.0f;
    static constexpr float PLASTICITY_MODIFIER_INITIAL = 1.0f;
    static constexpr float PLASTICITY_MODIFIER_CRITICAL = 0.8f;
    static constexpr float PLASTICITY_MODIFIER_MATURATION = 0.5f;
    static constexpr float PLASTICITY_MODIFIER_ADULT = 0.2f;
    static constexpr float SYNAPTOGENESIS_BASE_RATE = 0.0001f;
    static constexpr float PRUNING_BASE_RATE = 0.00001f;
    
    // === ERROR HANDLING ===
    // Helper to validate brain state
    bool validateBrainState() const {
        return brain_ != nullptr && !brain_->getRegions().empty();
    }
    
    // Helper to safely access neuron groups
    template<typename T>
    T safeAccess(const std::vector<T>& vec, size_t index, T defaultValue) {
        return (index < vec.size()) ? vec[index] : defaultValue;
    }
    
    // === NEW: Validation methods ===
    void validateSensoryConsistency(const SensoryPercept& percept) const;
    void validateMotorCommandSelection() const;
};

} // namespace nlm