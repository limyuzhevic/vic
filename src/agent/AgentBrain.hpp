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
    
    // Get state information for monitoring and debugging
    float getDopamineLevel() const { return dopamineLevel_; }
    float getCuriosityLevel() const { return curiosityLevel_; }
    float getNoveltyLevel() const { return noveltyLevel_; }
    float getPredictionError() const { return predictionError_; }
    float getExpectedReward() const { return expectedReward_; }
    double getDevelopmentalAge() const { return developmentalAge_; }
    float getPlasticityModifier() const { return plasticityModifier_; }
    
    // Advanced experimentation commands
    void setCuriosityEnabled(bool enable) { curiosityEnabled_ = enable; }
    void setDevelopmentStage(int stage) { developmentalAge_ = static_cast<double>(stage * 300.0); }
    void enableAllSystems(bool enable) {
        enableRewardModulation(enable);
        enableStructuralPlasticity(enable);
        enableDevelopment(enable);
        enableCuriosity(enable);
    }
    
    // Get system status and statistics
    size_t getWorkingMemorySize() const;
    size_t getEpisodicMemoryCount() const;
    float getAverageFiringRate() const;
    size_t getTotalSpikeCount() const;
    float getAttentionFocus() const;
    size_t getConceptCount() const;
    
    // Experiment control commands
    void pauseEverything(bool pause);
    void forceMemoryConsolidation();
    void triggerReplay(size_t episodeCount = 5);
    void resetNeuromodulation();
    void setGoal(const std::vector<float>& goal);
    std::vector<float> getCurrentGoal() const;
    
    // Debug and monitoring
    void logSystemStatus() const;
    void saveBrainState(const std::string& filepath) const;
    void loadBrainState(const std::string& filepath);
    void resetAll() {
        reset();
        if (brain_) brain_->reset();
    }
    
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
