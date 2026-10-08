#pragma once

#include "AgentBody.hpp"
#include "SensoryPercept.hpp"
#include "../brain/Brain.hpp"
#include "../world/SimpleWorld.hpp"
#include <memory>
#include <vector>
#include <string>
#include <functional>
#include <optional>
#include <variant>

namespace nlm {

// Forward declarations
class Brain;
class SimpleWorld;
class SensoryPercept;
class SensoryInput;
class Action;

// Custom exceptions for AgentBrain
class AgentBrainError : public std::runtime_error {
public:
    explicit AgentBrainError(const std::string& message) : std::runtime_error(message) {}
};

class InitializationError : public AgentBrainError {
public:
    explicit InitializationError(const std::string& message) : AgentBrainError(message) {}
};

class ProcessingError : public AgentBrainError {
public:
    explicit ProcessingError(const std::string& message) : AgentBrainError(message) {}
};

// AgentBrain: Connects NLM brain to the world
// Handles sensory transduction and motor decoding
class AgentBrain {
public:
    AgentBrain(std::shared_ptr<Brain> brain);
    ~AgentBrain();
    
    // Disable copying, enable moving
    AgentBrain(const AgentBrain&) = delete;
    AgentBrain& operator=(const AgentBrain&) = delete;
    AgentBrain(AgentBrain&&) noexcept;
    AgentBrain& operator=(AgentBrain&&) noexcept;
    
    // Initialize with world
    bool initialize(const SimpleWorld& world);
    
    // Context manager support
    AgentBrain& operator()();  // Begin operation chain
    bool commit();  // Complete operations
    void rollback();  // Undo operations
    
    // Get sensory input size expected by brain
    size_t getSensoryInputSize() const;
    
    // Get motor output size expected
    size_t getMotorOutputSize() const;
    
    // Process sensory percept and inject into brain
    bool processSensoryInput(const SensoryPercept& percept);
    
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
    Brain* getBrain() const { return brain_.get(); }
    
    // Get agent body state
    const AgentBody& getAgentBody() const;
    
    // Configuration with validation
    void enableRewardModulation(bool enable);
    void enableStructuralPlasticity(bool enable);
    void enableDevelopment(bool enable);
    void enableCuriosity(bool enable);
    
    bool isRewardModulationEnabled() const;
    bool isStructuralPlasticityEnabled() const;
    bool isDevelopmentEnabled() const;
    bool isCuriosityEnabled() const;
    
    // Advanced configuration methods
    void setPlasticityModifier(float modifier);
    void setSensoryNoveltyDecay(float decay);
    void setMotorCommandSelectionStrategy(const std::string& strategy);
    
    // Get current configuration values
    float getPlasticityModifier() const;
    float getSensoryNoveltyDecay() const;
    std::string getMotorCommandSelectionStrategy() const;
    
    // Status and validation
    bool isInitialized() const;
    bool validateState() const;
    std::string getValidationErrors() const;
    
    // Statistical methods
    float getAverageFiringRate() const;
    float getStandardDeviation() const;
    size_t getActiveNeuronCount() const;
    
    // Advanced processing methods
    // Batch process multiple sensory inputs
    size_t processSensoryBatch(const std::vector<SensoryPercept>& percepts);
    
    // Adaptive learning methods
    void adjustLearningRate(float factor);
    void adjustExplorationFactor(float factor);
    
    // Debugging and monitoring
    std::vector<float> getNeuronActivityPattern() const;
    std::vector<float> getMotorNeuronsActivity() const;
    std::vector<float> getSensoryNeuronsActivity() const;
    
    // Configuration import/export
    bool saveConfiguration(const std::string& filepath) const;
    bool loadConfiguration(const std::string& filepath);
    
    // Event callbacks
    void registerStateChangeCallback(std::function<void()> callback);
    void registerLearningEventCallback(std::function<void(float)> callback);
    void registerRewardCallback(std::function<void(float)> callback);
    
    // Comparison operators for Python
    bool operator==(const AgentBrain& other) const;
    bool operator!=(const AgentBrain& other) const;
    
    // String representation
    std::string toString() const;
    std::string getConfigSummary() const;
    
    // Context manager implementation
    class Context {
    public:
        Context(AgentBrain& agent) : agent_(agent) {}
        ~Context() { agent_.commit(); }
    private:
        AgentBrain& agent_;
    };
    
    Context withContext() { return Context(*this); }

private:
    // Motor decoding: convert neural activity to motor command
    MotorCommand decodeFromMotorNeurons();
    
    // Motor command selection with curiosity/exploration
    MotorCommand selectWithCuriosity(MotorCommand defaultCmd);
    
    // Internal helper methods
    void validateConfiguration() const;
    void validateInput(const SensoryPercept& percept) const;
    void updateNeuromodulationState();
    
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
    
    // State tracking
    bool initialized_;
    std::vector<std::function<void()>> stateChangeCallbacks_;
    std::vector<std::function<void(float)>> learningEventCallbacks_;
    std::vector<std::function<void(float)>> rewardCallbacks_;
    
    // Operation mode for context manager
    enum class OperationMode { Normal, Transaction };
    OperationMode operationMode_;
    bool transactionCommitted_;
};

} // namespace nlm
