#pragma once

#include "AgentBody.hpp"
#include "SensoryPercept.hpp"
#include "../brain/Brain.hpp"
#include "../world/SimpleWorld.hpp"
#include "../core/Types/Types.hpp"
#include <memory>
#include <vector>
#include <map>
#include <string>
#include <cmath>
#include <algorithm>
#include <unordered_map>

namespace nlm {

// === NAMED CONSTANTS ===
namespace AgentBrainConstants {
    // Sensory scaling factors
    constexpr float VISION_SCALE_FACTOR = 5.0f;
    constexpr float TOUCH_SCALE_FACTOR = 8.0f;
    constexpr float INTERNAL_SCALE_FACTOR = 5.0f;
    constexpr float PROPRIOCEPTION_SCALE_FACTOR = 3.0f;
    constexpr float SENSORY_NOVELTY_DECAY = 0.99f;
    
    // Motor control thresholds
    constexpr float MOTOR_ACTIVITY_THRESHOLD = 0.5f;
    constexpr float CURIOSITY_EXPLORATION_THRESHOLD = 0.3f;
    constexpr float HIGH_CURIOSITY_EXPLORATION_CHANCE = 0.5f;
    
    // Neuromodulation
    constexpr float DOPAMINE_CLAMP_MIN = -1.0f;
    constexpr float DOPAMINE_CLAMP_MAX = 1.0f;
    constexpr float ELIGIBILITY_THRESHOLD = 0.001f;
    constexpr float ELASTICITY_TRACE_DECAY = 0.1f;
    constexpr float PLASITICITY_BASE_MODIFIER = 0.5f;
    constexpr float PLASITICITY_RANGE = 0.5f;
    
    // Development
    constexpr float INITIAL_PLASTICITY = 1.0f;
    constexpr float CRITICAL_PERIOD_PLASTICITY = 0.8f;
    constexpr float MATURATION_PLASTICITY = 0.5f;
    constexpr float ADULT_PLASTICITY = 0.2f;
    
    // Developmental stages (in simulation time)
    constexpr double INITIAL_STAGE_END = 60.0;
    constexpr double CRITICAL_PERIOD_END = 300.0;
    constexpr double MATURATION_END = 900.0;
    
    // Structural plasticity rates
    constexpr float SYNAPTROGENESIS_RATE_BASE = 0.0001f;
    constexpr float PRUNING_RATE_BASE = 0.00001f;
    
    // Reward modulation
    constexpr float REWARD_FORGETTING_FACTOR = 0.95f;
    constexpr float REWARD_LEARNING_RATE = 0.05f;
    
    // Novelty calculation
    constexpr float NOVELTY_SENSITIVITY = 2.0f;
    constexpr float PREDICTION_ERROR_WEIGHT = 0.5f;
    
    // Exploration
    constexpr float EXPLORATION_RANDOM_MAX = 0.3f;
    
    // Input sizes (for validation)
    constexpr size_t EXPECTED_VISION_SIZE = 256;
    constexpr size_t EXPECTED_TOUCH_SIZE = 8;
    constexpr size_t EXPECTED_INTERNAL_SIZE = 4;
    constexpr size_t EXPECTED_PROPRIOCEPTION_SIZE = 6;
    
    // Neuron distribution
    constexpr size_t MOTOR_GROUP_COUNT = 6;
    constexpr size_t SENSORY_GROUP_COUNT = 4;
}

// === MOTOR COMMANDS ===
// Already defined in AgentBody.hpp, but included here for clarity
// enum class MotorCommand { MoveForward = 0, MoveBackward = 1, TurnLeft = 2, 
//                          TurnRight = 3, LookLeft = 4, LookRight = 5, 
//                          Interact = 6, Wait = 7 };

// === SENSORY MODALITIES ===
enum class SensoryModality : uint8_t {
    Vision = 0,
    Touch = 1,
    Internal = 2,
    Proprioception = 3
};

// === NEURON GROUP ===
struct NeuronGroup {
    std::string name;
    SensoryModality modality;  // For sensory groups, MotorCommand for motor groups
    std::vector<Neuron*> neurons;
    
    NeuronGroup() : modality(SensoryModality::Vision) {}
    
    NeuronGroup(const std::string& name, SensoryModality modality)
        : name(name), modality(modality) {}
    
    size_t size() const { return neurons.size(); }
    bool empty() const { return neurons.empty(); }
    
    void addNeuron(Neuron* neuron) {
        if (!neuron) return;
        neurons.push_back(neuron);
    }
    
    float calculateActivity() const {
        if (empty()) return 0.0f;
        float sum = 0.0f;
        for (Neuron* neuron : neurons) {
            if (neuron) {
                sum += std::abs(neuron->getState().membranePotential - neuron->getState().restingPotential);
            }
        }
        return sum / neurons.size();
    }
    
    // Inject current into all neurons in this group
    void injectCurrent(float current) {
        for (Neuron* neuron : neurons) {
            if (neuron) {
                neuron->injectCurrent(current);
            }
        }
    }
};

// === SENSORY PROCESSOR ===
class SensoryProcessor {
public:
    SensoryProcessor() = default;
    ~SensoryProcessor() = default;
    
    // Process sensory input and distribute to appropriate neuron groups
    static void processVision(const std::vector<float>& vision, NeuronGroup& visionGroup);
    static void processTouch(const std::vector<float>& touch, NeuronGroup& touchGroup);
    static void processInternal(const std::vector<float>& internal, NeuronGroup& internalGroup);
    static void processProprioception(const std::vector<float>& proprioception, NeuronGroup& proprioceptionGroup);
    
    // Calculate novelty from previous and current vision
    static float calculateNovelty(const std::vector<float>& currentVision, 
                                 const std::vector<float>& previousVision);
    
    // Update curiosity level
    static float updateCuriosity(float novelty, float predictionError, 
                                 bool curiosityEnabled = true);
    
    // Normalize and scale sensory input
    static float normalizeAndScale(float value, float scaleFactor);
    
    // Get sensory modality name
    static const char* getModalityName(SensoryModality modality);
    
    // Validate sensory data dimensions
    static bool validateVisionSize(size_t size);
    static bool validateTouchSize(size_t size);
    static bool validateInternalSize(size_t size);
    static bool validateProprioceptionSize(size_t size);
};

// === MOTOR CONTROLLER ===
class MotorController {
public:
    MotorController() = default;
    ~MotorController() = default;
    
    // Decode motor command from neuron groups
    static MotorCommand decodeFromNeuronGroups(const std::map<MotorCommand, NeuronGroup>& motorGroups,
                                              float activityThreshold = AgentBrainConstants::MOTOR_ACTIVITY_THRESHOLD);
    
    // Calculate activity for a specific motor command group
    static float calculateMotorActivity(const NeuronGroup& motorGroup);
    
    // Select action with curiosity-based exploration
    static MotorCommand selectWithCuriosity(MotorCommand defaultCommand, float curiosityLevel,
                                           float explorationThreshold = AgentBrainConstants::HIGH_CURIOSITY_EXPLORATION_CHANCE,
                                           float exploreChanceMax = AgentBrainConstants::EXPLORATION_RANDOM_MAX,
                                           std::shared_ptr<Brain> brain = nullptr);
    
    // Get best motor command based on activity comparison
    static MotorCommand findBestCommand(const std::vector<std::pair<MotorCommand, float>>& commandActivities);
    
    // Generate random motor command for exploration
    static MotorCommand generateRandomCommand(std::shared_ptr<Brain> brain = nullptr);
    
    // Validate motor command
    static bool isValidMotorCommand(MotorCommand command);
    
    // Get motor command name for logging
    static const char* getCommandName(MotorCommand command);
};

// === AGENT BRAIN CLASS ===
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
    
    // Get references to neuron groups for testing/debugging
    const std::vector<Neuron*>& getMotorForward() const { return motorGroups_.at(MotorCommand::MoveForward).neurons; }
    const std::vector<Neuron*>& getMotorBackward() const { return motorGroups_.at(MotorCommand::MoveBackward).neurons; }
    const std::vector<Neuron*>& getMotorTurnLeft() const { return motorGroups_.at(MotorCommand::TurnLeft).neurons; }
    const std::vector<Neuron*>& getMotorTurnRight() const { return motorGroups_.at(MotorCommand::TurnRight).neurons; }
    const std::vector<Neuron*>& getMotorInteract() const { return motorGroups_.at(MotorCommand::Interact).neurons; }
    const std::vector<Neuron*>& getMotorWait() const { return motorGroups_.at(MotorCommand::Wait).neurons; }
    
    const std::vector<Neuron*>& getSensoryVision() const { return sensoryGroups_.at(SensoryModality::Vision).neurons; }
    const std::vector<Neuron*>& getSensoryTouch() const { return sensoryGroups_.at(SensoryModality::Touch).neurons; }
    const std::vector<Neuron*>& getSensoryInternal() const { return sensoryGroups_.at(SensoryModality::Internal).neurons; }
    const std::vector<Neuron*>& getSensoryProprioception() const { return sensoryGroups_.at(SensoryModality::Proprioception).neurons; }
    
private:
    // Helper methods for internal use
    void distributeNeuronsToGroups();
    bool validateNeuronGroups() const;
    void logNeuronGroupCounts() const;
    
    // Motor decoding helper
    MotorCommand decodeFromMotorNeurons();
    
    // Motor command selection with curiosity/exploration
    MotorCommand selectWithCuriosity(MotorCommand defaultCmd);
    
    // State variables
    std::shared_ptr<Brain> brain_;
    
    // Neuron groups organized by type
    std::map<MotorCommand, NeuronGroup> motorGroups_;
    std::map<SensoryModality, NeuronGroup> sensoryGroups_;
    
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
    
    // Constants for this instance
    const float sensoryNoveltyDecay_ = AgentBrainConstants::SENSORY_NOVELTY_DECAY;
};