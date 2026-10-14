#pragma once

#include "AgentBody.hpp"
#include "SensoryPercept.hpp"
#include "../brain/Brain.hpp"
#include "../world/SimpleWorld.hpp"
#include <memory>
#include <vector>
#include <map>

// Include the actual subsystem headers
#include "SensoryNeuronGroup.hpp"
#include "MotorCommandGroup.hpp"
#include "SensoryProcessor.hpp"
#include "MotorDecoder.hpp"
#include "NeuromodulationSystem.hpp"
#include "DevelopmentSystem.hpp"
#include "CuriositySystem.hpp"
#include "AgentBrainConfig.hpp"

namespace nlm {

// AgentBrain: Connects NLM brain to the world
// Handles sensory transduction and motor decoding through modular subsystems
class AgentBrain {
public:
    AgentBrain(std::shared_ptr<Brain> brain);
    ~AgentBrain();
    
    // Delete copy operations
    AgentBrain(const AgentBrain&) = delete;
    AgentBrain& operator=(const AgentBrain&) = delete;
    
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
    
    // Subsystem access for testing and advanced usage
    const SensoryProcessor* getSensoryProcessor() const { return sensoryProcessor_.get(); }
    const MotorDecoder* getMotorDecoder() const { return motorDecoder_.get(); }
    const NeuromodulationSystem* getNeuromodulationSystem() const { return neuromodulationSystem_.get(); }
    const DevelopmentSystem* getDevelopmentSystem() const { return developmentSystem_.get(); }
    const CuriositySystem* getCuriositySystem() const { return curiositySystem_.get(); }
    
private:
    // Private helper methods
    void initializeSubsystems();
    void resetSubsystems();
    void createSensoryNeuronGroups(const std::vector<std::unique_ptr<NeuralRegion>>& regions);
    
    std::shared_ptr<Brain> brain_;
    
    // Neuromodulation state variables
    float dopamineLevel_;
    float noveltyLevel_;
    float curiosityLevel_;
    float predictionError_;
    float expectedReward_;
    
    // Development state variables
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
    
    // Subsystem instances
    std::unique_ptr<SensoryProcessor> sensoryProcessor_;
    std::unique_ptr<MotorDecoder> motorDecoder_;
    std::unique_ptr<NeuromodulationSystem> neuromodulationSystem_;
    std::unique_ptr<DevelopmentSystem> developmentSystem_;
    std::unique_ptr<CuriositySystem> curiositySystem_;
    
    // Sensory neuron groups
    SensoryNeuronGroup sensoryVisionGroup_;
    SensoryNeuronGroup sensoryTouchGroup_;
    SensoryNeuronGroup sensoryInternalGroup_;
    SensoryNeuronGroup sensoryProprioceptionGroup_;
    
    // Motor neuron groups (for direct access if needed)
    std::vector<Neuron*> motorForward_;
    std::vector<Neuron*> motorBackward_;
    std::vector<Neuron*> motorTurnLeft_;
    std::vector<Neuron*> motorTurnRight_;
    std::vector<Neuron*> motorInteract_;
    std::vector<Neuron*> motorWait_;
    
    // Legacy neuron group arrays (for backward compatibility)
    std::vector<Neuron*> sensoryVision_;
    std::vector<Neuron*> sensoryTouch_;
    std::vector<Neuron*> sensoryInternal_;
    std::vector<Neuron*> sensoryProprioception_;
};

} // namespace nlm
