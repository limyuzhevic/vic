#pragma once

#include "AgentBody.hpp"
#include "SensoryPercept.hpp"
#include "../brain/Brain.hpp"
#include "../world/SimpleWorld.hpp"
#include <memory>
#include <vector>

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
    void enableAdvancedBehavior(bool enable) { advancedBehaviorEnabled_ = enable; }
    
    bool isRewardModulationEnabled() const { return rewardModulationEnabled_; }
    bool isStructuralPlasticityEnabled() const { return structuralPlasticityEnabled_; }
    bool isDevelopmentEnabled() const { return developmentEnabled_; }
    bool isCuriosityEnabled() const { return curiosityEnabled_; }
    bool isAdvancedBehaviorEnabled() const { return advancedBehaviorEnabled_; }
    
    // Advanced behavior state
    bool isExploring() const { return isExploring_; }
    bool isHarvesting() const { return isHarvesting_; }
    bool isRepairing() const { return isRepairing_; }
    float getExplorationRange() const { return explorationRange_; }
    int getResourcesCollected() const { return resourcesCollected_; }
    float getRepairNeeds() const { return repairNeeds_; }
    
    // Behavior control
    void setExplorationRange(float range) { explorationRange_ = range; }
    void incrementResourcesCollected() { resourcesCollected_++; }
    void setRepairNeeds(float needs) { repairNeeds_ = std::clamp(needs, 0.0f, 1.0f); }
    
private:
    // Motor decoding: convert neural activity to motor command
    MotorCommand decodeFromMotorNeurons();
    
    // Motor command selection with curiosity/exploration
    MotorCommand selectWithCuriosity(MotorCommand defaultCmd);
    
    // Advanced behavior selection
    MotorCommand selectGoalDirectedBehavior(MotorCommand defaultCmd);
    MotorCommand prioritizeExploration(MotorCommand defaultCmd);
    MotorCommand prioritizeResourceGathering(MotorCommand defaultCmd);
    MotorCommand prioritizeRepair(MotorCommand defaultCmd);
    MotorCommand selectRepairAction();
    bool isLocationKnown(NeuralWorkingMemory* workingMem, float x, float y) const;
    
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
    bool advancedBehaviorEnabled_;
    
    // Previous sensory state for novelty detection
    std::vector<float> previousVision_;
    float sensoryNoveltyDecay_;
    
    // Advanced behavior state
    bool isExploring_;
    bool isHarvesting_;
    bool isRepairing_;
    bool hasSetPath_;
    float explorationRange_;
    int resourcesCollected_;
    float repairNeeds_;
    std::vector<std::pair<float, float>> path_; // Navigation path
    
    // Position for location tracking (would come from world in real implementation)
    float x_;
    float y_;
};

} // namespace nlm
