#pragma once

#include "AgentBody.hpp"
#include "SensoryPercept.hpp"
#include "../brain/Brain.hpp"
#include "../world/SimpleWorld.hpp"
#include <memory>
#include <vector>
#include <string>

namespace nlm {

// Additional enumeration types
enum class MotorCommandSelectionStrategy {
    Conservative,     // Select most active command, no exploration
    Balanced,         // Standard behavior with moderate exploration
    Aggressive,       // High exploration rate
    Adaptive,         // Dynamic exploration based on environment
    Statistical       // Select based on confidence intervals
};

enum class BehaviorMode {
    Reactive,         // React immediately to sensory input
    Reflective,        // Consider past experiences before acting
    Proactive,         // Act based on internal goals
    Adaptive,          // Learn and adapt behavior dynamically
    Autonomous         // Self-directed exploration
};

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
    
    // Advanced motor command selection with different strategies
    MotorCommand decodeMotorCommand(MotorCommandSelectionStrategy strategy);
    MotorCommand decodeMotorCommandWithExploration(MotorCommand defaultCmd, float explorationRate);
    MotorCommand decodeMotorCommandWithThreshold(MotorCommand defaultCmd, float activityThreshold);
    
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
    
    // Advanced configuration methods
    void setVisionInputScale(float scale) { visionInputScale_ = scale; }
    void setTouchInputScale(float scale) { touchInputScale_ = scale; }
    void setInternalInputScale(float scale) { internalInputScale_ = scale; }
    void setProprioceptionInputScale(float scale) { proprioceptionInputScale_ = scale; }
    
    float getVisionInputScale() const { return visionInputScale_; }
    float getTouchInputScale() const { return touchInputScale_; }
    float getInternalInputScale() const { return internalInputScale_; }
    float getProprioceptionInputScale() const { return proprioceptionInputScale_; }
    
    void setNoveltyDecayRate(float rate) { sensoryNoveltyDecay_ = rate; }
    float getNoveltyDecayRate() const { return sensoryNoveltyDecay_; }
    
    void setCuriosityThreshold(float threshold) { curiosityThreshold_ = threshold; }
    float getCuriosityThreshold() const { return curiosityThreshold_; }
    
    void setExplorationFactor(float factor) { explorationFactor_ = factor; }
    float getExplorationFactor() const { return explorationFactor_; }
    
    void setMotorCommandThreshold(float threshold) { motorCommandThreshold_ = threshold; }
    float getMotorCommandThreshold() const { return motorCommandThreshold_; }
    
    // State information methods
    float getExpectedReward() const { return expectedReward_; }
    double getDevelopmentalAge() const { return developmentalAge_; }
    float getPlasticityModifier() const { return plasticityModifier_; }
    
    // Statistics and debugging
    size_t getVisionSensoryNeuronCount() const { return sensoryVision_.size(); }
    size_t getTouchSensoryNeuronCount() const { return sensoryTouch_.size(); }
    size_t getInternalSensoryNeuronCount() const { return sensoryInternal_.size(); }
    size_t getProprioceptionSensoryNeuronCount() const { return sensoryProprioception_.size(); }
    
    size_t getMotorForwardCount() const { return motorForward_.size(); }
    size_t getMotorBackwardCount() const { return motorBackward_.size(); }
    size_t getMotorTurnLeftCount() const { return motorTurnLeft_.size(); }
    size_t getMotorTurnRightCount() const { return motorTurnRight_.size(); }
    size_t getMotorInteractCount() const { return motorInteract_.size(); }
    size_t getMotorWaitCount() const { return motorWait_.size(); }
    
    // Behavior control methods
    void setBehaviorMode(BehaviorMode mode) { behaviorMode_ = mode; }
    BehaviorMode getBehaviorMode() const { return behaviorMode_; }
    
    // State saving/loading for advanced use cases
    void saveState(const std::string& filepath) const;
    void loadState(const std::string& filepath);
    
    // Validation and safety methods
    bool validateState() const;
    void emergencyReset();
    
private:
    // Motor decoding: convert neural activity to motor command
    MotorCommand decodeFromMotorNeurons();
    
    // Motor command selection with curiosity/exploration
    MotorCommand selectWithCuriosity(MotorCommand defaultCmd);
    
    // Advanced selection methods
    MotorCommand selectWithAdaptiveExploration(MotorCommand defaultCmd, float dynamicThreshold);
    MotorCommand selectWithStatisticalAnalysis(MotorCommand defaultCmd, float confidenceThreshold);
    
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
    
    // Advanced configuration
    float visionInputScale_;
    float touchInputScale_;
    float internalInputScale_;
    float proprioceptionInputScale_;
    float noveltyDecayRate_;
    float curiosityThreshold_;
    float explorationFactor_;
    float motorCommandThreshold_;
    BehaviorMode behaviorMode_;
    
    // Previous sensory state for novelty detection
    std::vector<float> previousVision_;
    float sensoryNoveltyDecay_;
};

} // namespace nlm
