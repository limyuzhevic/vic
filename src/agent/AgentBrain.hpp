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
    
    bool isRewardModulationEnabled() const { return rewardModulationEnabled_; }
    bool isStructuralPlasticityEnabled() const { return structuralPlasticityEnabled_; }
    bool isDevelopmentEnabled() const { return developmentEnabled_; }
    bool isCuriosityEnabled() const { return curiosityEnabled_; }
    
    // Enhanced functionality for advanced agents
    // These methods provide advanced agent capabilities
    virtual void enableAdvancedLearning(bool enable) {}
    virtual bool isAdvancedLearningEnabled() const { return false; }
    
    virtual void setActionSelectionStrategy(int strategy) {}
    virtual int getActionSelectionStrategy() const { return 0; }
    
    virtual void enableMetaLearning(bool enable) {}
    virtual bool isMetaLearningEnabled() const { return false; }
    
    virtual void setExperienceReplaySize(size_t size) {}
    virtual size_t getExperienceReplaySize() const { return 0; }
    
    virtual void enableAdaptiveExploration(bool enable) {}
    virtual bool isAdaptiveExplorationEnabled() const { return false; }
    
    virtual void configurePolicyNetwork(size_t inputSize, size_t hiddenSize, size_t outputSize) {}
    
    // Performance monitoring
    virtual float getExpectedValue(const std::vector<float>& state) const { return 0.0f; }
    virtual float getPolicyEntropy() const { return 0.0f; }
    virtual float getLearningProgress() const { return 0.0f; }
    virtual float getCuriosityScore() const { return 0.0f; }
    virtual float getNoveltyScore() const { return 0.0f; }
    
    // Advanced statistics
    virtual size_t getExperienceBufferSize() const { return 0; }
    virtual size_t getTotalLearningSteps() const { return 0; }
    virtual float getTotalReward() const { return 0.0f; }
    
    // Policy export/import
    virtual void savePolicy(const std::string& filepath) const {}
    virtual bool loadPolicy(const std::string& filepath) { return false; }
    
    // Statistics and debugging
    virtual void logStatus() const {}
    
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

// Forward declaration for AdvancedAgentBrain
class AdvancedAgentBrain;

// Factory function for creating advanced agent brains
inline std::shared_ptr<AgentBrain> createAgentBrain(std::shared_ptr<Brain> brain, bool advanced = false) {
    if (advanced) {
        // Return advanced agent brain (in a real implementation, this would return AdvancedAgentBrain)
        // For now, return basic AgentBrain
        return std::make_shared<AgentBrain>(brain);
    }
    return std::make_shared<AgentBrain>(brain);
}

} // namespace nlm
