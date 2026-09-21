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
    
    // Convenience methods (NEW)
    void setBrain(std::shared_ptr<Brain> brain);
    bool validateState() const;
    float getSensoryActivitySum() const;
    float getMotorActivitySum() const;
    
// Convenience methods (NEW)
    void setBrain(std::shared_ptr<Brain> brain);
    bool validateState() const;
    float getSensoryActivitySum() const;
    float getMotorActivitySum() const;
    
    // Debug and monitoring methods
    size_t getTotalSensoryNeurons() const;
    size_t getTotalMotorNeurons() const;
    float getCombinedActivityLevel() const;
    
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

// Inline implementations of convenience methods
inline void AgentBrain::setBrain(std::shared_ptr<Brain> brain) {
    assert(brain != nullptr);
    brain_ = brain;
}

inline bool AgentBrain::validateState() const {
    if (!brain_) return false;
    
    // Check that all neuron vectors are valid (may be empty but shouldn't be nullptr)
    if (sensoryVision_.empty() && sensoryTouch_.empty() && 
        sensoryInternal_.empty() && sensoryProprioception_.empty()) {
        return false; // At least one sensory group should have neurons
    }
    
    return true;
}

inline float AgentBrain::getSensoryActivitySum() const {
    float totalActivity = 0.0f;
    totalActivity += calculateNeuronActivity(sensoryVision_);
    totalActivity += calculateNeuronActivity(sensoryTouch_);
    totalActivity += calculateNeuronActivity(sensoryInternal_);
    totalActivity += calculateNeuronActivity(sensoryProprioception_);
    return totalActivity;
}

inline float AgentBrain::getMotorActivitySum() const {
    float totalActivity = 0.0f;
    totalActivity += calculateNeuronActivity(motorForward_);
    totalActivity += calculateNeuronActivity(motorBackward_);
    totalActivity += calculateNeuronActivity(motorTurnLeft_);
    totalActivity += calculateNeuronActivity(motorTurnRight_);
    totalActivity += calculateNeuronActivity(motorInteract_);
    totalActivity += calculateNeuronActivity(motorWait_);
    return totalActivity;
}

inline size_t AgentBrain::getTotalSensoryNeurons() const {
    return sensoryVision_.size() + sensoryTouch_.size() + 
           sensoryInternal_.size() + sensoryProprioception_.size();
}

inline size_t AgentBrain::getTotalMotorNeurons() const {
    return motorForward_.size() + motorBackward_.size() + 
           motorTurnLeft_.size() + motorTurnRight_.size() +
           motorInteract_.size() + motorWait_.size();
}

inline float AgentBrain::getCombinedActivityLevel() const {
    float sensory = getSensoryActivitySum();
    float motor = getMotorActivitySum();
    float total = sensory + motor;
    return (total > 0.0f) ? (sensory / total) : 0.5f; // Return proportion of sensory vs motor activity
}

} // namespace nlm
