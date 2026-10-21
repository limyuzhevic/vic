#pragma once

#include "AgentBody.hpp"
#include "SensoryPercept.hpp"
#include "../brain/Brain.hpp"
#include "../world/SimpleWorld.hpp"
#include <memory>
#include <vector>
#include <map>

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
    
    bool isRewardModulationEnabled() const { return rewardModulationEnabled_; }
    bool isStructuralPlasticityEnabled() const { return structuralPlasticityEnabled_; }
    bool isDevelopmentEnabled() const { return developmentEnabled_; }
    bool isCuriosityEnabled() const { return curiosityEnabled_; }
    
    // Advanced methods for power users
    // Batch processing for efficiency
    void processBatchSensoryInputs(const std::vector<SensoryPercept>& percepts);
    std::vector<MotorCommand> decodeBatchMotorCommands(size_t count);
    
    // Advanced neuromodulation
    void applyCustomModulation(const std::map<std::string, float>& customSignals);
    std::map<std::string, float> getModulationState() const;
    
    // Development and maturation control
    void setDevelopmentSpeed(float speed);  // 1.0 = normal, <1.0 = slowed, >1.0 = accelerated
    void pauseDevelopment(bool pause);
    double getDevelopmentalAge() const;
    
    // Novelty and exploration control
    void setCuriosityThreshold(float threshold);
    void setExplorationRate(float rate);
    float getExplorationRate() const;
    
    // Learning and memory control
    void enableAllLearning(bool enable);
    void resetAllMemories();
    size_t getMemoryStats() const;  // Get statistics from all memory systems
    
    // Performance monitoring
    double getProcessingTimePerStep() const;
    size_t getPerformanceStats() const;  // Get performance metrics
    void resetPerformanceStats();
    
    // Advanced world interaction
    void setCustomWorldInterface(const std::shared_ptr<void>& interface);
    bool isWorldInterfaceActive() const;
    
    // Safety and limits
    void setActionLimits(const std::map<ActionType, float>& limits);
    void setEnergyLimits(float maxEnergy, float minEnergy);
    bool checkSafetyConstraints() const;
    
    // Serialization and persistence
    std::string exportState() const;
    bool importState(const std::string& state);
    
    // Calibration and tuning
    void calibrateMotorMapping(const std::map<ActionType, std::vector<float>>& mapping);
    void calibrateSensoryScaling(const std::map<SensoryType, float>& scaling);
    void autoTuneParameters();  // Automated parameter tuning
    
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
    
    // Advanced state for power user features
    float curiosityThreshold_;
    float explorationRate_;
    double developmentSpeed_;
    size_t processingStepCount_;
    double processingTimePerStep_;
    size_t performanceStepCount_;
    
    float maxEnergy_;
    float minEnergy_;
    float energy_;
    
    std::map<ActionType, float> actionLimits_;
    std::map<SensoryType, float> sensoryScaling_;
    std::map<ActionType, std::vector<float>> motorMapping_;
    std::shared_ptr<void> customWorldInterface_;
    bool worldInterfaceActive_;
};

} // namespace nlm
