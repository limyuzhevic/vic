#pragma once

#include "AgentBody.hpp"
#include "SensoryPercept.hpp"
#include "../brain/Brain.hpp"
#include "../cognition/NeuralPlanner.hpp"  // For SelfModel and SocialLearning
#include "../cognition/ConceptFormation.hpp"  // For SpatialRepresentation and TemporalRelation
#include "../prediction/NeuralPrediction.hpp"  // For NeuralPrediction
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
    
    // Brain cognitive system accessors (Phase 6 integration)
    
    // Planning and decision-making systems
    NeuralPlanner* getPlanner() const {
        return brain_ ? brain_->getPlanner() : nullptr;
    }
    
    ConceptFormation* getConceptFormation() const {
        return brain_ ? brain_->getConceptFormation() : nullptr;
    }
    
    AttentionalSelection* getAttention() const {
        return brain_ ? brain_->getAttention() : nullptr;
    }
    
    // Advanced cognitive systems (Phase 6)
    
    // Self model of agent's own actions and capabilities
    SelfModel* getSelfModel() const {
        return brain_ ? brain_->getSelfModel() : nullptr;
    }
    
    // Social learning from observing other agents
    SocialLearning* getSocialLearning() const {
        return brain_ ? brain_->getSocialLearning() : nullptr;
    }
    
    // Spatial representation of environment
    SpatialRepresentation* getSpatialRepresentation() const {
        return brain_ ? brain_->getSpatialRepresentation() : nullptr;
    }
    
    // Temporal relations and sequence processing
    TemporalRelation* getTemporalRelation() const {
        return brain_ ? brain_->getTemporalRelation() : nullptr;
    }
    
    // Advanced prediction system
    NeuralPrediction* getNeuralPrediction() const {
        return brain_ ? brain_->getNeuralPrediction() : nullptr;
    }
    
    // Basic prediction system
    PredictionSystem* getPredictionSystem() const {
        return brain_ ? brain_->getPredictionSystem() : nullptr;
    }
    
    // Memory system access (Phase 6 integration)
    
    // Working memory - transient active information
    NeuralWorkingMemory* getWorkingMemory() const {
        return brain_ ? brain_->getWorkingMemory() : nullptr;
    }
    
    // Episodic memory - experience storage
    NeuralEpisodicMemory* getEpisodicMemory() const {
        return brain_ ? brain_->getEpisodicMemory() : nullptr;
    }
    
    // Associative memory - pattern associations
    NeuralAssociativeMemory* getAssociativeMemory() const {
        return brain_ ? brain_->getAssociativeMemory() : nullptr;
    }
    
    // Neuromodulation system access
    
    // Dopamine - reward and reinforcement
    Dopamine* getDopamine() const {
        return brain_ ? brain_->getDopamine() : nullptr;
    }
    
    // Curiosity - exploration motivation
    Curiosity* getCuriosity() const {
        return brain_ ? brain_->getCuriosity() : nullptr;
    }
    
    // Novelty - novelty detection
    Novelty* getNovelty() const {
        return brain_ ? brain_->getNovelty() : nullptr;
    }
    
    // Prediction error signal
    PredictionError* getPredictionErrorSignal() const {
        return brain_ ? brain_->getPredictionErrorSignal() : nullptr;
    }
    
    // Development system
    DevelopmentSystem* getDevelopmentSystem() const {
        return brain_ ? brain_->getDevelopmentSystem() : nullptr;
    }
    
    // Get current configuration
    std::shared_ptr<const Config> getConfig() const {
        return brain_ ? brain_->getConfig() : nullptr;
    }
    
    // Get random generator
    RandomGenerator* getRandomGenerator() const {
        return brain_ ? brain_->getRandomGenerator() : nullptr;
    }
    
    // Logging
    void logStatus() const {
        if (brain_) {
            brain_->logStatus();
        }
    }
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
