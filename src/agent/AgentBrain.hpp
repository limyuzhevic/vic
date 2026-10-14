#pragma once

#include "AgentBody.hpp"
#include "SensoryPercept.hpp"
#include "../brain/Brain.hpp"
#include "../world/SimpleWorld.hpp"
#include <memory>
#include <vector>
#include <string>
#include <unordered_map>

namespace nlm {

// Configuration for neuron group distribution
struct NeuronGroupConfig {
    // Motor group configuration
    struct MotorGroup {
        std::string name;
        NeuronType type;
        int count;  // Number of neurons in this group
        float weight;  // Weight factor for activity
    };
    
    // Sensory group configuration  
    struct SensoryGroup {
        std::string name;
        NeuronType type;
        int count;  // Number of neurons in this group
        float sensitivity;  // Sensitivity factor for input processing
    };
    
    // Configuration containers
    std::vector<MotorGroup> motorGroups;
    std::vector<SensoryGroup> sensoryGroups;
    
    // Default constructor with predefined groups
    NeuronGroupConfig() {
        // Create default motor groups
        motorGroups.push_back({"Forward", NeuronType::Motor, 1, 1.0f});
        motorGroups.push_back({"Backward", NeuronType::Motor, 1, 1.0f});
        motorGroups.push_back({"TurnLeft", NeuronType::Motor, 1, 1.0f});
        motorGroups.push_back({"TurnRight", NeuronType::Motor, 1, 1.0f});
        motorGroups.push_back({"Interact", NeuronType::Motor, 1, 1.0f});
        motorGroups.push_back({"Wait", NeuronType::Motor, 1, 0.5f});
        
        // Create default sensory groups
        sensoryGroups.push_back({"Vision", NeuronType::Sensory, 1, 1.0f});
        sensoryGroups.push_back({"Touch", NeuronType::Sensory, 1, 1.0f});
        sensoryGroups.push_back({"Internal", NeuronType::Sensory, 1, 1.0f});
        sensoryGroups.push_back({"Proprioception", NeuronType::Sensory, 1, 1.0f});
    }
    
    // Add motor group
    void addMotorGroup(const std::string& name, NeuronType type, int count = 1, float weight = 1.0f) {
        motorGroups.push_back({name, type, count, weight});
    }
    
    // Add sensory group
    void addSensoryGroup(const std::string& name, NeuronType type, int count = 1, float sensitivity = 1.0f) {
        sensoryGroups.push_back({name, type, count, sensitivity});
    }
    
    // Get total motor groups
    size_t getMotorGroupCount() const { return motorGroups.size(); }
    
    // Get total sensory groups
    size_t getSensoryGroupCount() const { return sensoryGroups.size(); }
    
    // Get motor group name by index
    std::string getMotorGroupName(size_t index) const {
        if (index < motorGroups.size()) return motorGroups[index].name;
        return "Unknown";
    }
    
    // Get sensory group name by index
    std::string getSensoryGroupName(size_t index) const {
        if (index < sensoryGroups.size()) return sensoryGroups[index].name;
        return "Unknown";
    }
    
    // Check if advanced configuration is enabled (custom groups)
    bool hasCustomGroups() const {
        return motorGroups.size() > 6 || sensoryGroups.size() > 4;
    }
};

// AgentBrain: Connects NLM brain to the world
// Handles sensory transduction and motor decoding
class AgentBrain {
public:
    AgentBrain(std::shared_ptr<Brain> brain, const NeuronGroupConfig& groupConfig = NeuronGroupConfig());
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
    
    // Configuration methods
    void enableRewardModulation(bool enable) { rewardModulationEnabled_ = enable; }
    void enableStructuralPlasticity(bool enable) { structuralPlasticityEnabled_ = enable; }
    void enableDevelopment(bool enable) { developmentEnabled_ = enable; }
    void enableCuriosity(bool enable) { curiosityEnabled_ = enable; }
    
    // Advanced configuration methods
    void setNeuronGroupConfig(const NeuronGroupConfig& config) { groupConfig_ = config; }
    void enableCustomNeuronDistribution(bool enable) { customNeuronDistribution_ = enable; }
    
    bool isRewardModulationEnabled() const { return rewardModulationEnabled_; }
    bool isStructuralPlasticityEnabled() const { return structuralPlasticityEnabled_; }
    bool isDevelopmentEnabled() const { return developmentEnabled_; }
    bool isCuriosityEnabled() const { return curiosityEnabled_; }
    bool hasCustomNeuronDistribution() const { return customNeuronDistribution_; }
    
    // Get group statistics
    size_t getMotorGroupCount() const { return groupConfig_.getMotorGroupCount(); }
    size_t getSensoryGroupCount() const { return groupConfig_.getSensoryGroupCount(); }
    std::string getMotorGroupName(size_t index) const { return groupConfig_.getMotorGroupName(index); }
    std::string getSensoryGroupName(size_t index) const { return groupConfig_.getSensoryGroupName(index); }
    
private:
    // Motor decoding: convert neural activity to motor command
    MotorCommand decodeFromMotorNeurons();
    
    // Motor command selection with curiosity/exploration
    MotorCommand selectWithCuriosity(MotorCommand defaultCmd);
    
    std::shared_ptr<Brain> brain_;
    
    // Neuron group configuration
    NeuronGroupConfig groupConfig_;
    bool customNeuronDistribution_;
    
    // Motor neuron groups (dynamic based on configuration)
    std::vector<Neuron*> motorGroups_;
    std::vector<Neuron*> sensoryGroups_;
    
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
    
    // Legacy support for backward compatibility
    // These are kept for compatibility with existing code
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
};

} // namespace nlm
