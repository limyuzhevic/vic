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
    
// Cognitive planning integration
    // Integrate with NeuralPlanner for action planning
    bool planAction(const class NeuralPlanner* planner, const std::vector<float>& currentState);
    bool executePlannedAction(MotorCommand plannedAction, const std::vector<float>& currentState);
    
    // Self-model integration for body awareness
    // Integrate with NeuralSelfModel for body schema and prediction
    bool updateSelfModel(const class NeuralSelfModel* selfModel, const std::vector<float>& beforeState, const std::vector<float>& afterState);
    std::vector<float> predictConsequence(ActionType action, const std::vector<float>& currentState);
    
    // Social learning integration for observation and imitation
    // Integrate with NeuralSocialLearning for social knowledge
    bool observeSocialAction(ActionType action, const std::vector<float>& observerState, const std::vector<float>& resultingState);
    bool canImitate(ActionType action) const;
    ActionType getImitationAction(const std::vector<float>& currentState) const;
    
    // Spatial representation integration for navigation
    // Integrate with NeuralSpatialRepresentation for spatial knowledge
    bool updateSpatialRepresentation(const class NeuralSpatialRepresentation* spatialRep, float x, float y, const std::vector<float>& sensoryCues);
    std::pair<float, float> getPredictedPosition() const;
    float getPositionActivation(float x, float y) const;
    
    // Concept formation integration for pattern discovery
    // Integrate with ConceptFormation for knowledge formation
    bool updateConceptFormation(const class ConceptFormation* conceptFormation, const std::vector<float>& currentPattern);
    bool formConceptFromExperience(const std::vector<float>& experiencePattern);
    
    // Memory system integration for learning and consolidation
    // Integrate with memory systems for experience storage
    bool storeExperienceInMemory(const class NeuralWorkingMemory* workingMem, const std::vector<float>& currentState);
    bool storeEpisodeInEpisodicMemory(const class NeuralEpisodicMemory* episodicMem, const std::vector<float>& experience);
    bool consolidateMemory(const class NeuralEpisodicMemory* episodicMem, float consolidationThreshold = 0.5f);

private:
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
    
    // Cognitive system integration state
    std::shared_ptr<NeuralSelfModel> selfModel_;
    std::shared_ptr<NeuralSocialLearning> socialLearning_;
    std::shared_ptr<NeuralSpatialRepresentation> spatialRepresentation_;
    std::shared_ptr<ConceptFormation> conceptFormation_;
    std::shared_ptr<NeuralWorkingMemory> workingMem_;
    std::shared_ptr<NeuralEpisodicMemory> episodicMem_;
    
    // Tracking variables
    ActionType plannedAction_;
    float currentX_, currentY_;
    double currentTime_;
    
    // Action and social experience recording
    struct ActionEffect {
        std::vector<float> beforeState;
        std::vector<float> afterState;
        float confidence;
        size_t observationCount;
    };
    std::vector<std::vector<ActionEffect>> recordedActions_;  // Indexed by ActionType
    
    struct SocialObservation {
        ActionType action;
        std::vector<float> observerState;
        std::vector<float> resultingState;
        float reward;
    };
    std::vector<SocialObservation> recordedSocialActions_;
    
    // Concept tracking
    std::vector<std::vector<float>> currentConcepts_;
    std::vector<std::vector<float>> formedConcepts_;
};

} // namespace nlm
