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
    
    // Get neural planner
    NeuralPlanner* getNeuralPlanner() { return brain_ ? brain_->getPlanner() : nullptr; }
    
    // Get concept formation system
    ConceptFormation* getConceptFormation() { return brain_ ? brain_->getConceptFormation() : nullptr; }
    
    // Get prediction system
    PredictionSystem* getPredictionSystem() { return brain_ ? brain_->getPredictionSystem() : nullptr; }
    
    // Get attention system
    AttentionalSelection* getAttentionSystem() { return brain_ ? brain_->getAttention() : nullptr; }
    
    // Cognitive-based action selection with concept guidance
    MotorCommand decodeWithConcepts(MotorCommand defaultCmd);
    
    // Prediction-based action planning
    MotorCommand decodeWithPredictions(MotorCommand defaultCmd);
    
    // Attention-enhanced action selection
    MotorCommand decodeWithAttention(MotorCommand defaultCmd);
    
    // Developmental concept adaptation
    void adaptConceptsForDevelopment(double timestep);
    
    // Enhanced curiosity with cognitive weighting
    void updateCuriosityWithCognition();
    
    // Get current concept-based action preferences
    const std::vector<float>& getConceptActionPreferences() const { return conceptActionPreferences_; }
    
    // Get prediction-based action values
    const std::vector<float>& getPredictedActionValues() const { return predictedActionValues_; }
    
    // Get attention weights for different action categories
    const std::vector<float>& getAttentionActionWeights() const { return attentionActionWeights_; }

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
    
    // Cognitive integration state
    std::vector<float> conceptActionPreferences_;
    std::vector<float> predictedActionValues_;
    std::vector<float> attentionActionWeights_;
    size_t lastBestConceptId_;
    size_t bestActionConceptId_;
    float cognitiveExplorationBonus_;
    float conceptAdaptationRate_;
};

} // namespace nlm
