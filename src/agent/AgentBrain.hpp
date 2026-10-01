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
    
    // ========== MEMORY SYSTEM ACCESSORS ==========
    
    // Get working memory for active information storage
    NeuralWorkingMemory* getWorkingMemory() { return brain_ ? brain_->getWorkingMemory() : nullptr; }
    
    // Get episodic memory for experience storage
    NeuralEpisodicMemory* getEpisodicMemory() { return brain_ ? brain_->getEpisodicMemory() : nullptr; }
    
    // Get associative memory for pattern associations
    NeuralAssociativeMemory* getAssociativeMemory() { return brain_ ? brain_->getAssociativeMemory() : nullptr; }
    
    // Get prediction system for sensory prediction and error computation
    PredictionSystem* getPredictionSystem() { return brain_ ? brain_->getPredictionSystem() : nullptr; }
    
    // ========== MEMORY PROCESSING METHODS ==========
    
    // Process sensory input and store in working memory
    void processSensoryInputWithMemory(const SensoryPercept& percept);
    
    // Store experience in episodic memory
    void storeExperience(float reward, const std::vector<float>& sensoryState, 
                         const std::vector<float>& resultingSensoryState, ActionType action);
    
    // Retrieve similar experiences from episodic memory
    std::vector<const EpisodicMemoryItem*> retrieveSimilarExperiences(const std::vector<float>& sensoryState,
                                                                     size_t maxResults = 3);
    
    // Get memory-based motor command
    MotorCommand decodeMotorCommandWithMemory();
    
    // Apply memory-based reward modulation
    void applyRewardModulationWithMemory(float reward, float predictedReward);
    
    // Update memory systems with current agent state
    void updateMemorySystems(double timestep);
    
    // Check if memory systems are integrated
    bool isMemoryIntegrated() const {
        return getWorkingMemory() != nullptr || 
               getEpisodicMemory() != nullptr || 
               getPredictionSystem() != nullptr;
    }
    
    // Configuration
    void enableRewardModulation(bool enable) { rewardModulationEnabled_ = enable; }
    void enableStructuralPlasticity(bool enable) { structuralPlasticityEnabled_ = enable; }
    void enableDevelopment(bool enable) { developmentEnabled_ = enable; }
    void enableCuriosity(bool enable) { curiosityEnabled_ = enable; }
    
    bool isRewardModulationEnabled() const { return rewardModulationEnabled_; }
    bool isStructuralPlasticityEnabled() const { return structuralPlasticityEnabled_; }
    bool isDevelopmentEnabled() const { return developmentEnabled_; }
    bool isCuriosityEnabled() const { return curiosityEnabled_; }
    
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
