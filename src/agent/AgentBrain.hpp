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
    
    // Advanced debugging and exploration commands for experienced users
    
    // Manual neuromodulation control
    void setDopamineLevel(float level) {
        dopamineLevel_ = std::clamp(level, -1.0f, 1.0f);
        if (brain_) {
            auto* dopamine = brain_->getDopamine();
            if (dopamine) {
                dopamine->setLevel(dopamineLevel_);
            }
        }
    }
    
    void setCuriosityLevel(float level) {
        curiosityLevel_ = std::clamp(level, 0.0f, 1.0f);
    }
    
    void setNoveltyLevel(float level) {
        noveltyLevel_ = std::clamp(level, 0.0f, 1.0f);
    }
    
    // Memory system access and manipulation
    NeuralWorkingMemory* getWorkingMemory() {
        return brain_ ? brain_->getWorkingMemory() : nullptr;
    }
    
    NeuralEpisodicMemory* getEpisodicMemory() {
        return brain_ ? brain_->getEpisodicMemory() : nullptr;
    }
    
    NeuralAssociativeMemory* getAssociativeMemory() {
        return brain_ ? brain_->getAssociativeMemory() : nullptr;
    }
    
    // Cognitive system access and manipulation
    NeuralPlanner* getPlanner() {
        return brain_ ? brain_->getPlanner() : nullptr;
    }
    
    ConceptFormation* getConceptFormation() {
        return brain_ ? brain_->getConceptFormation() : nullptr;
    }
    
    AttentionalSelection* getAttention() {
        return brain_ ? brain_->getAttention() : nullptr;
    }
    
    // Performance and monitoring
    float getTotalSpikeCount() const {
        return brain_ ? brain_->getTotalSpikeCount() : 0;
    }
    
    float getAverageFiringRate() const {
        return brain_ ? brain_->getAverageFiringRate() : 0.0f;
    }
    
    float getExcitationInhibitionRatio() const {
        return brain_ ? brain_->getExcitationInhibitionRatio() : 1.0f;
    }
    
    // Exploration and control commands
    
    // Trigger exploration behavior
    void triggerExploration() {
        if (brain_) {
            auto* curiosity = brain_->getCuriosity();
            if (curiosity) {
                // Set curiosity to high level to trigger exploration
                curiosityLevel_ = 0.8f;
            }
        }
    }
    
    // Set development stage
    void setDevelopmentStage(DevelopmentalStage stage) {
        if (brain_) {
            brain_->setDevelopmentalStage(stage);
        }
    }
    
    // Get developmental stage
    DevelopmentalStage getDevelopmentStage() const {
        return brain_ ? brain_->getDevelopmentalStage() : DevelopmentalStage::Initial;
    }
    
    // Memory consolidation command
    void triggerMemoryConsolidation() {
        if (brain_) {
            auto* episodicMemory = brain_->getEpisodicMemory();
            if (episodicMemory) {
                // Trigger consolidation of episodic memories
                episodicMemory->consolidate(0.5f);
            }
        }
    }
    
    // Replay command
    void triggerMemoryReplay(size_t episodeCount = 3) {
        if (brain_) {
            auto* episodicMemory = brain_->getEpisodicMemory();
            if (episodicMemory) {
                // Get episodes for replay
                auto episodes = episodicMemory->getEpisodesForReplay(episodeCount);
                // Replay the episodes
                for (const auto* episode : episodes) {
                    episodicMemory->replayEpisode(episode);
                }
            }
        }
    }
    
    // Reset brain and all systems
    void fullReset() {
        // Reset agent brain state
        reset();
        
        // Reset cognitive systems if accessible
        if (brain_) {
            auto* workingMemory = brain_->getWorkingMemory();
            if (workingMemory) {
                workingMemory->clear();
            }
            
            auto* episodicMemory = brain_->getEpisodicMemory();
            if (episodicMemory) {
                episodicMemory->clear();
            }
            
            auto* conceptFormation = brain_->getConceptFormation();
            if (conceptFormation) {
                conceptFormation->clear();
            }
            
            auto* planner = brain_->getPlanner();
            if (planner) {
                planner->clearCache();
            }
            
            // Reset development stage
            brain_->setDevelopmentalStage(DevelopmentalStage::Initial);
        }
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
