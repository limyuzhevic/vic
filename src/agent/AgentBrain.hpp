#pragma once

#include "AgentBody.hpp"
#include "SensoryPercept.hpp"
#include "../brain/Brain.hpp"
#include "../world/SimpleWorld.hpp"
#include "../brain/Neuron.hpp"
#include <memory>
#include <vector>
#include <map>
#include <string>
#include <type_traits>

namespace nlm {

// Template configuration classes for flexible neural architecture
template<typename T>
class NeuronConfig {
public:
    std::string name;
    NeuronType type;
    size_t count;
    float scaleFactor;
    std::map<std::string, float> parameters;
    
    NeuronConfig(std::string n, NeuronType t, size_t c, float sf = 1.0f)
        : name(std::move(n)), type(t), count(c), scaleFactor(sf) {}
    
    // Factory method for creating common configs
    static NeuronConfig visionConfig(size_t count, float scale = 5.0f) {
        return NeuronConfig("vision", NeuronType::Sensory, count, scale);
    }
    
    static NeuronConfig touchConfig(size_t count, float scale = 8.0f) {
        return NeuronConfig("touch", NeuronType::Sensory, count, scale);
    }
    
    static NeuronConfig internalConfig(size_t count, float scale = 5.0f) {
        return NeuronConfig("internal", NeuronType::Sensory, count, scale);
    }
    
    static NeuronConfig proprioceptionConfig(size_t count, float scale = 3.0f) {
        return NeuronConfig("proprioception", NeuronType::Sensory, count, scale);
    }
};

// Enhanced ActionConfig with more flexibility
class ActionConfig {
public:
    MotorCommand command;
    std::string name;
    float defaultScale;
    
    ActionConfig(MotorCommand cmd, std::string n, float scale = 1.0f)
        : command(cmd), name(std::move(n)), defaultScale(scale) {}
    
    // Factory methods for common actions
    static ActionConfig moveForward() { return ActionConfig(MotorCommand::MoveForward, "move_forward", 1.0f); }
    static ActionConfig moveBackward() { return ActionConfig(MotorCommand::MoveBackward, "move_backward", 1.0f); }
    static ActionConfig turnLeft() { return ActionConfig(MotorCommand::TurnLeft, "turn_left", 1.0f); }
    static ActionConfig turnRight() { return ActionConfig(MotorCommand::TurnRight, "turn_right", 1.0f); }
    static ActionConfig interact() { return ActionConfig(MotorCommand::Interact, "interact", 1.0f); }
    static ActionConfig wait() { return ActionConfig(MotorCommand::Wait, "wait", 1.0f); }
};

// Distribution strategy enum for neural group management
enum class DistributionStrategy {
    Fixed,           // Fixed-size groups
    Proportional,    // Proportional to total neurons
    Custom          // User-defined distribution
};

// Enhanced neural group template for better type safety
template<typename T>
class NeuralGroup {
public:
    std::vector<T*> neurons;
    std::string name;
    DistributionStrategy strategy;
    float scaleFactor;
    std::map<std::string, float> parameters;
    
    NeuralGroup(const std::string& n, DistributionStrategy s = DistributionStrategy::Fixed, float scale = 1.0f)
        : name(n), strategy(s), scaleFactor(scale) {}
    
    // Add neuron with validation
    void addNeuron(T* neuron) {
        if (neuron) {
            neurons.push_back(neuron);
        }
    }
    
    size_t size() const { return neurons.size(); }
    bool empty() const { return neurons.empty(); }
    
    T* operator[](size_t index) {
        return neurons[index];
    }
    
    const T* operator[](size_t index) const {
        return neurons[index];
    }
};

// Type aliases for better readability
typedef NeuralGroup<Neuron> VisionGroup;
typedef NeuralGroup<Neuron> TouchGroup;
typedef NeuralGroup<Neuron> InternalGroup;
typedef NeuralGroup<Neuron> ProprioceptionGroup;

// Enhanced AgentBrain with improved architecture and configuration options
class AgentBrain {
public:
    AgentBrain(std::shared_ptr<Brain> brain);
    ~AgentBrain();
    
    // Initialize with world and flexible configuration options
    void initialize(const SimpleWorld& world,
                    const std::vector<NeuronConfig<Neuron*>>& visionConfig = 
                        std::vector<NeuronConfig<Neuron*>>{NeuronConfig<Neuron*>::visionConfig(64)},
                    const std::vector<NeuronConfig<Neuron*>>& touchConfig = 
                        std::vector<NeuronConfig<Neuron*>>{NeuronConfig<Neuron*>::touchConfig(8)},
                    const std::vector<NeuronConfig<Neuron*>>& internalConfig = 
                        std::vector<NeuronConfig<Neuron*>>{NeuronConfig<Neuron*>::internalConfig(4)},
                    const std::vector<NeuronConfig<Neuron*>>& proprioceptionConfig = 
                        std::vector<NeuronConfig<Neuron*>>{NeuronConfig<Neuron*>::proprioceptionConfig(6)});
    
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
    
    // Update development system with enhanced parameters
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
    
    // Configuration methods for enhanced customization
    void enableRewardModulation(bool enable) { rewardModulationEnabled_ = enable; }
    void enableStructuralPlasticity(bool enable) { structuralPlasticityEnabled_ = enable; }
    void enableDevelopment(bool enable) { developmentEnabled_ = enable; }
    void enableCuriosity(bool enable) { curiosityEnabled_ = enable; }
    
    bool isRewardModulationEnabled() const { return rewardModulationEnabled_; }
    bool isStructuralPlasticityEnabled() const { return structuralPlasticityEnabled_; }
    bool isDevelopmentEnabled() const { return developmentEnabled_; }
    bool isCuriosityEnabled() const { return curiosityEnabled_; }
    
    // Configuration parameter setters
    void setCuriosityParameters(const std::map<std::string, float>& params) { curiosityParameters_ = params; }
    void setDevelopmentalParameters(const std::map<std::string, float>& params) { developmentalParameters_ = params; }
    void setStructuralParameters(const std::map<std::string, float>& params) { structuralParameters_ = params; }
    
    // Get configuration parameters
    const std::map<std::string, float>& getCuriosityParameters() const { return curiosityParameters_; }
    const std::map<std::string, float>& getDevelopmentalParameters() const { return developmentalParameters_; }
    const std::map<std::string, float>& getStructuralParameters() const { return structuralParameters_; }
    
    // Enhanced reset with parameter validation
    void reset(double initialAge = 0.0, float initialPlasticity = 1.0f);

private:
    // Brain reference
    std::shared_ptr<Brain> brain_;
    
    // Enhanced neural group organization with type safety
    std::vector<VisionGroup> visionGroups_;
    std::vector<TouchGroup> touchGroups_;
    std::vector<InternalGroup> internalGroups_;
    std::vector<ProprioceptionGroup> proprioceptionGroups_;
    
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
    
    // Configuration parameters
    std::map<std::string, float> curiosityParameters_;
    std::map<std::string, float> developmentalParameters_;
    std::map<std::string, float> structuralParameters_;
    
    // Legacy motor decoding methods (for backward compatibility)
    MotorCommand decodeFromMotorNeurons();
    MotorCommand selectWithCuriosity(MotorCommand defaultCmd);
    
    // Helper methods
    void initializeDefaultGroups();
    size_t calculateConfiguredNeuronCount(const std::vector<NeuronConfig<Neuron*>>& configs) const;
};
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
