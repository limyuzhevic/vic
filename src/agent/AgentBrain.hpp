#pragma once

#include "AgentBody.hpp"
#include "SensoryPercept.hpp"
#include "../brain/Brain.hpp"
#include "../world/SimpleWorld.hpp"
#include "../brain/Neuron.hpp"
#include "../core/Types/Types.hpp"
#include <memory>
#include <vector>
#include <string>
#include <unordered_map>

namespace nlm {

// Forward declarations
class Brain;
class SensoryPercept;

// Configuration constants for AgentBrain
namespace AgentBrainConfig {
    // Neuron distribution constants
    constexpr size_t MOTOR_GROUP_COUNT = 6;
    constexpr size_t SENSORY_GROUP_COUNT = 4;
    
    // Vision configuration (from world SimpleWorld)
    constexpr size_t DEFAULT_VISION_WIDTH = 16;
    constexpr size_t DEFAULT_VISION_HEIGHT = 16;
    constexpr size_t VISION_SIZE = DEFAULT_VISION_WIDTH * DEFAULT_VISION_HEIGHT;  // 256
    
    // Sensory configuration
    constexpr size_t TOUCH_SENSOR_COUNT = 8;
    constexpr size_t INTERNAL_SENSOR_COUNT = 4;
    constexpr size_t PROPRIOCEPTION_COUNT = 6;
    
    // Motor configuration
    constexpr size_t MOTOR_ACTION_COUNT = 6;
    
    // Neuromodulation constants
    constexpr float DEFAULT_DOPAMINE_LEVEL = 0.0f;
    constexpr float DEFAULT_NOVELTY_LEVEL = 0.0f;
    constexpr float DEFAULT_CURIOUSITY_LEVEL = 0.0f;
    constexpr float DEFAULT_PREDICTION_ERROR = 0.0f;
    constexpr float DEFAULT_EXPECTED_REWARD = 0.0f;
    
    // Development constants
    constexpr double INITIAL_DEVELOPMENTAL_AGE = 0.0;
    constexpr float INITIAL_PLASTICITY_MODIFIER = 1.0f;
    constexpr float DEFAULT_NOVELTY_DECAY = 0.99f;
    
    // Configuration flags default values
    constexpr bool DEFAULT_REWARD_MODULATION_ENABLED = true;
    constexpr bool DEFAULT_STRUCTURAL_PLASTICITY_ENABLED = true;
    constexpr bool DEFAULT_DEVELOPMENT_ENABLED = true;
    constexpr bool DEFAULT_CURIOUSITY_ENABLED = true;
    
    // Neuromodulation thresholds
    constexpr float PREDICTION_ERROR_CLAMP_MIN = -1.0f;
    constexpr float PREDICTION_ERROR_CLAMP_MAX = 1.0f;
    constexpr float PLASTICITY_FACTOR_CLAMP_MIN = 0.1f;
    constexpr float PLASTICITY_FACTOR_CLAMP_MAX = 2.0f;
    
    // Development stages (in seconds)
    constexpr double INITIAL_STAGE_DURATION = 60.0;   // Initial
    constexpr double CRITICAL_PERIOD_DURATION = 240.0;  // Critical Period
    constexpr double MATURATION_DURATION = 540.0;   // Maturation
    
    // Motor decoding constants
    constexpr float MIN_MEAN_ACTIVITY_THRESHOLD = 0.5f;
    constexpr float EXPLORATION_BASE_CHANCE = 0.3f;
    
    // Novelty calculation
    constexpr float NOVELTY_SENSITIVITY = 2.0f;
    constexpr float PREDICTION_ERROR_SENSITIVITY = 0.5f;
    
    // Scale factors for sensory input
    constexpr float VISION_SCALE_FACTOR = 5.0f;
    constexpr float TOUCH_SCALE_FACTOR = 8.0f;
    constexpr float INTERNAL_SCALE_FACTOR = 5.0f;
    constexpr float PROPRIOCEPTION_SCALE_FACTOR = 3.0f;
}

// Enumeration for neuron group types
enum class NeuronGroupType {
    MotorForward,
    MotorBackward,
    MotorTurnLeft,
    MotorTurnRight,
    MotorInteract,
    MotorWait,
    SensoryVision,
    SensoryTouch,
    SensoryInternal,
    SensoryProprioception,
    COUNT
};

// NeuronGroup class encapsulates a group of neurons and provides utility functions
class NeuronGroup {
public:
    // Constructor
    NeuronGroup(const std::string& name, NeuronGroupType type, 
                size_t capacity = 0, bool active = true);
    
    // Destructor
    ~NeuronGroup();
    
    // Copy constructor and assignment operator (disabled)
    NeuronGroup(const NeuronGroup&) = delete;
    NeuronGroup& operator=(const NeuronGroup&) = delete;
    
    // Move constructor and assignment operator
    NeuronGroup(NeuronGroup&& other) noexcept;
    NeuronGroup& operator=(NeuronGroup&& other) noexcept;
    
    // Getters
    const std::string& getName() const { return name_; }
    NeuronGroupType getType() const { return type_; }
    bool isActive() const { return active_; }
    size_t size() const { return neurons_.size(); }
    bool empty() const { return neurons_.empty(); }
    
    // Neuron access
    Neuron* operator[](size_t index) { return neurons_[index]; }
    const Neuron* operator[](size_t index) const { return neurons_[index]; }
    Neuron* at(size_t index);
    const Neuron* at(size_t index) const;
    
    // Neuron management
    void addNeuron(Neuron* neuron);
    void removeNeuron(Neuron* neuron);
    void clear();
    void setActive(bool active) { active_ = active; }
    
    // Neuron manipulation
    void injectCurrentToAll(float current) const;
    float calculateAverageActivity() const;
    float calculateTotalActivity() const;
    
    // Validation
    bool isValid() const;
    
private:
    std::string name_;
    NeuronGroupType type_;
    std::vector<Neuron*> neurons_;
    bool active_;
    
    // Helper function to calculate neuron activity
    float calculateNeuronActivity(Neuron* neuron) const;
};

// AgentBrain: Connects NLM brain to the world
// Handles sensory transduction and motor decoding
class AgentBrain {
public:
    // Constructor
    AgentBrain(std::shared_ptr<Brain> brain);
    
    // Destructor
    ~AgentBrain();
    
    // Copy constructor and assignment operator (disabled)
    AgentBrain(const AgentBrain&) = delete;
    AgentBrain& operator=(const AgentBrain&) = delete;
    
    // Move constructor and assignment operator
    AgentBrain(AgentBrain&& other) noexcept;
    AgentBrain& operator=(AgentBrain&& other) noexcept;
    
    // Main public interface
    
    // Initialize with world
    // @param world Reference to the SimpleWorld instance
    void initialize(const SimpleWorld& world);
    
    // Get sensory input size expected by brain
    // @return Size of sensory input vector expected by brain
    size_t getSensoryInputSize() const;
    
    // Get motor output size expected
    // @return Number of possible motor commands
    size_t getMotorOutputSize() const;
    
    // Process sensory percept and inject into brain
    // @param percept The SensoryPercept containing sensory data
    void processSensoryInput(const SensoryPercept& percept);
    
    // Decode brain motor activity into motor command
    // @return The selected motor command
    MotorCommand decodeMotorCommand();
    
    // Apply neuromodulation based on reward
    // @param reward The actual reward received
    // @param predictedReward The predicted reward
    void applyRewardModulation(float reward, float predictedReward);
    
    // Update development system
    // @param timestep Time step duration in seconds
    void updateDevelopment(double timestep);
    
    // Get current developmental stage
    // @return Current developmental stage
    DevelopmentalStage getDevelopmentalStage() const;
    
    // Get neuromodulation level
    // @return Current dopamine level
    float getNeuromodulationLevel() const;
    
    // Get curiosity level
    // @return Current curiosity level
    float getCuriosityLevel() const;
    
    // Get novelty level
    // @return Current novelty level
    float getNoveltyLevel() const;
    
    // Get prediction error
    // @return Current prediction error
    float getPredictionError() const;
    
    // Reset agent for new episode
    void reset();
    
    // Get brain pointer
    // @return Pointer to the brain
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
    
    // Neuron group access (for testing and debugging)
    const NeuronGroup& getNeuronGroup(NeuronGroupType type) const;
    NeuronGroup& getNeuronGroup(NeuronGroupType type);
    
    // Get all active neuron groups
    std::vector<NeuronGroupType> getActiveNeuronGroups() const;
    
private:
    // Private helper methods for better organization
    
    // Neuron group management
    void initializeNeuronGroups();
    NeuronGroup& getOrCreateNeuronGroup(NeuronGroupType type);
    const NeuronGroup& getNeuronGroupInternal(NeuronGroupType type) const;
    
    // Brain integration
    void injectSensoryNeurons();
    void extractMotorNeurons();
    
    // Validation helpers
    void validateBrain() const;
    bool validateNeuronGroupIndex(size_t index) const;
    bool validateSensoryIndex(size_t groupIndex, size_t signalIndex) const;
    
    // Neuromodulation calculations
    void updateDopamineLevel(float predictionError);
    void updateNoveltyLevel(const std::vector<float>& vision);
    void updateCuriosityLevel();
    
    // Development calculations
    void updateDevelopmentalStage(double timestep);
    void updatePlasticityModifier();
    
    // Motor decoding helpers
    MotorCommand decodeFromMotorNeurons();
    MotorCommand selectWithCuriosity(MotorCommand defaultCmd);
    
    // Configuration access
    static float getPlasticityFactorFromDopamine(float dopamineLevel);
    static float calculateExplorationChance(float curiosityLevel);
    
    // State management
    void resetNeuromodulationState();
    void resetDevelopmentState();
    void resetSensoryState(const SimpleWorld& world);
    
    // Validation and error handling
    void validateParameters() const;
    void validateNeuromodulationParameters() const;
    
    // Member variables (organized into logical groups)
    
    // Core brain interface
    std::shared_ptr<Brain> brain_;
    
    // Neuron groups - using NeuronGroup class for better encapsulation
    std::unordered_map<NeuronGroupType, NeuronGroup> neuronGroups_;
    
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

// Utility functions
inline float clamp(float value, float min, float max) {
    return std::max(min, std::min(value, max));
}

inline float normalize(float value, float min, float max) {
    if (max == min) return 0.0f;
    return (value - min) / (max - min);
}