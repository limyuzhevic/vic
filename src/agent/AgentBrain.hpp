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
    void enableAttention(bool enable) { attentionEnabled_ = enable; }
    void enableConceptFormation(bool enable) { conceptFormationEnabled_ = enable; }
    
    bool isRewardModulationEnabled() const { return rewardModulationEnabled_; }
    bool isStructuralPlasticityEnabled() const { return structuralPlasticityEnabled_; }
    bool isDevelopmentEnabled() const { return developmentEnabled_; }
    bool isCuriosityEnabled() const { return curiosityEnabled_; }
    bool isAttentionEnabled() const { return attentionEnabled_; }
    bool isConceptFormationEnabled() const { return conceptFormationEnabled_; }
    
    // Advanced Configuration
    void setPlanningDepth(size_t depth) { if (brain_) brain_->getPlanner()->setPlanningDepth(depth); }
    void setMemoryCapacity(size_t capacity) { if (brain_ && brain_->getWorkingMemory()) brain_->getWorkingMemory()->setCapacity(capacity); }
    void setDevelopmentStage(DevelopmentalStage stage) { if (brain_) brain_->setDevelopmentalStage(stage); }
    
    // Monitoring and Analysis
    size_t getActiveMemoryTraces() const { return brain_ && brain_->getWorkingMemory() ? 
        brain_->getWorkingMemory()->getActiveTraces() : 0; }
    size_t getMemoryEpisodes() const { return brain_ && brain_->getEpisodicMemory() ? 
        brain_->getEpisodicMemory()->getEpisodeCount() : 0; }
    size_t getConceptCount() const { return brain_ && brain_->getConceptFormation() ? 
        brain_->getConceptFormation()->getConceptCount() : 0; }
    float getAverageFiringRate() const { return brain_ ? brain_->getAverageFiringRate() : 0.0f; }
    size_t getTotalNeuronCount() const { return brain_ ? brain_->getTotalNeuronCount() : 0; }
    
    // Export and Import
    bool saveBrainState(const std::string& filepath) const { return brain_ ? brain_->save(filepath) : false; }
    bool loadBrainState(const std::string& filepath) { return brain_ ? brain_->load(filepath) : false; }
    
    // Debugging
    void injectTestStimulus(float current, NeuronType type) { if (brain_) brain_->injectCurrentToNeurons(type, current); }
    void resetAllSystems() { reset(); }
    
    // Advanced Analysis
    std::vector<float> getMemoryActivity() const { return brain_ && brain_->getWorkingMemory() ? 
        brain_->getWorkingMemory()->retrieve() : std::vector<float>(); }
    std::vector<NeuronId> getMemoryNeurons() const { return brain_ && brain_->getWorkingMemory() ? 
        brain_->getWorkingMemory()->getMemoryNeurons() : std::vector<NeuronId>(); }
    
    // Configuration
    void setAttentionEnabled(bool enable) { attentionEnabled_ = enable; }
    void setConceptFormationEnabled(bool enable) { conceptFormationEnabled_ = enable; }
    
    bool isAttentionEnabled() const { return attentionEnabled_; }
    bool isConceptFormationEnabled() const { return conceptFormationEnabled_; }
    
    // Simulation Control
    void setSimulationSpeed(float speed) { simulationSpeed_ = speed; }
    void pauseSimulation() { simulationPaused_ = true; }
    void resumeSimulation() { simulationPaused_ = false; }
    bool isSimulationPaused() const { return simulationPaused_; }
    float getSimulationSpeed() const { return simulationSpeed_; }
    
    // Debug and Analysis
    void enableDebugMode(bool enable) { debugMode_ = enable; }
    bool isDebugModeEnabled() const { return debugMode_; }
    
    // Learning Analysis
    float getLearningProgress() const { return brain_ ? 
        (brain_->getTotalSpikeCount() / static_cast<float>(std::max(1, brain_->getTotalNeuronCount()))) : 0.0f; }
    float getMemoryRetention() const { return brain_ && brain_->getEpisodicMemory() ? 
        brain_->getEpisodicMemory()->getEpisodeCount() / 10.0f : 0.0f; } // Normalized
    
    // System Status
    bool isMemorySystemHealthy() const { return brain_ && brain_->getWorkingMemory() && 
        brain_->getWorkingMemory()->getMemoryActivity() > 0.1f; }
    bool isCognitiveSystemHealthy() const { return brain_ && brain_->getPlanner() && 
        brain_->getConceptFormation(); }
    bool isPredictionSystemHealthy() const { return brain_ && brain_->getPredictionSystem(); }
    
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
