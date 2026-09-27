#pragma once

#include "AgentConfig.hpp"
#include "../agent/MotorDecoder.hpp"
#include "../agent/SensoryProcessor.hpp"
#include "../agent/NeuromodulationManager.hpp"
#include "../agent/DevelopmentManager.hpp"
#include "../agent/CuriosityManager.hpp"
#include "../brain/Brain.hpp"
#include "../world/SimpleWorld.hpp"
#include <memory>
#include <vector>

namespace nlm {

// Refactored AgentBrain: Orchestrates multiple specialized components
// Each component handles a specific aspect of agent behavior
class AgentBrain {
public:
    AgentBrain(std::shared_ptr<Brain> brain, const AgentConfig& config = AgentConfig());
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
    
    // Update component configurations
    void updateComponentConfigs() {
        motorDecoder_.setConfig(config_);
        sensoryProcessor_.setConfig(config_);
        neuromodulationManager_.setConfig(config_);
        developmentManager_.setConfig(config_);
        curiosityManager_.setConfig(config_);
    }
    
    // Set global agent configuration
    void setConfig(const AgentConfig& config) {
        config_ = config;
        updateComponentConfigs();
    }
    
    const AgentConfig& getConfig() const { return config_; }
    
private:
    std::shared_ptr<Brain> brain_;
    
    // Component managers
    MotorDecoder motorDecoder_;
    SensoryProcessor sensoryProcessor_;
    NeuromodulationManager neuromodulationManager_;
    DevelopmentManager developmentManager_;
    CuriosityManager curiosityManager_;
    
    // Agent configuration
    AgentConfig config_;
    
    // Configuration flags
    bool rewardModulationEnabled_;
    bool structuralPlasticityEnabled_;
    bool developmentEnabled_;
    bool curiosityEnabled_;
    
    // Neuromodulation state
    float dopamineLevel_;
    float noveltyLevel_;
    float curiosityLevel_;
    float predictionError_;
    float expectedReward_;
    
    // Development state
    double developmentalAge_;
    float plasticityModifier_;
    
    // Previous sensory state for novelty detection
    std::vector<float> previousVision_;
    
    // Neuron groups (organized by type)
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
    
    // Helper methods
    void initializeNeuronGroups();
    void organizeNeuronsByType(const Brain* brain);
    void organizeMotorNeurons(const Brain* brain);
    void organizeSensoryNeurons(const Brain* brain);
};

} // namespace nlm
