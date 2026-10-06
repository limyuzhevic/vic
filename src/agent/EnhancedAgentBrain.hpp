#pragma once

#include "AgentBody.hpp"
#include "SensoryPercept.hpp"
#include "../brain/Brain.hpp"
#include "../cognition/NeuralPlanner.hpp"
#include "../cognition/ConceptFormation.hpp"
#include "../memory/NeuralWorkingMemory.hpp"
#include "../cognition/NeuralWorkingMemory.hpp"
#include "../world/SimpleWorld.hpp"
#include <memory>
#include <vector>
#include <unordered_map>
#include <algorithm>

namespace nlm {

// AgentBrain: Enhanced with higher cognition systems integration
// Integrates NeuralPlanner for advanced action planning
// Connects ConceptFormation for pattern discovery from sensory input
// Integrates AttentionalSelection for focused attention
// Connects prediction system for forward planning
class AgentBrain {
public:
    AgentBrain(std::shared_ptr<Brain> brain);
    ~AgentBrain();
    
    // Initialize with world and cognitive systems
    void initialize(const SimpleWorld& world);
    
    // Get sensory input size expected by brain
    size_t getSensoryInputSize() const;
    
    // Get motor output size expected
    size_t getMotorOutputSize() const;
    
    // Process sensory percept and inject into brain
    void processSensoryInput(const SensoryPercept& percept);
    
    // Decode brain motor activity into motor command with cognitive enhancement
    MotorCommand decodeMotorCommand();
    
    // Cognitive-based action selection (enhanced)
    MotorCommand decodeCognitiveAction();
    
    // Concept-based behavior integration
    MotorCommand decodeConceptBasedAction(const std::vector<float>& sensoryState);
    
    // Attention-guided action selection
    MotorCommand decodeAttentionGuidedAction();
    
    // Prediction-based planning and decision making
    MotorCommand decodePredictionBasedAction(const std::vector<float>& currentState);
    
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
    
    // Get cognitive confidence level
    float getCognitiveConfidence() const;
    
    // Get concept stability
    float getConceptStability() const;
    
    // Get attention focus level
    float getAttentionFocusLevel() const;
    
    // Reset agent for new episode
    void reset();
    
    // Get brain pointer
    Brain* getBrain() { return brain_.get(); }
    
    // Cognitive system access
    NeuralPlanner* getPlanner() { return planner_.get(); }
    ConceptFormation* getConceptFormation() { return conceptFormation_.get(); }
    NeuralWorkingMemory* getWorkingMemory() { return workingMemory_.get(); }
    AttentionalSelection* getAttention() { return attention_.get(); }
    
    // Configuration
    void enableRewardModulation(bool enable) { rewardModulationEnabled_ = enable; }
    void enableStructuralPlasticity(bool enable) { structuralPlasticityEnabled_ = enable; }
    void enableDevelopment(bool enable) { developmentEnabled_ = enable; }
    void enableCuriosity(bool enable) { curiosityEnabled_ = enable; }
    void enableCognitivePlanning(bool enable) { cognitivePlanningEnabled_ = enable; }
    void enableConceptBasedBehavior(bool enable) { conceptBasedBehaviorEnabled_ = enable; }
    void enableAttentionGuidance(bool enable) { attentionGuidanceEnabled_ = enable; }
    void enablePredictionPlanning(bool enable) { predictionPlanningEnabled_ = enable; }
    
    bool isRewardModulationEnabled() const { return rewardModulationEnabled_; }
    bool isStructuralPlasticityEnabled() const { return structuralPlasticityEnabled_; }
    bool isDevelopmentEnabled() const { return developmentEnabled_; }
    bool isCuriosityEnabled() const { return curiosityEnabled_; }
    bool isCognitivePlanningEnabled() const { return cognitivePlanningEnabled_; }
    bool isConceptBasedBehaviorEnabled() const { return conceptBasedBehaviorEnabled_; }
    bool isAttentionGuidanceEnabled() const { return attentionGuidanceEnabled_; }
    bool isPredictionPlanningEnabled() const { return predictionPlanningEnabled_; }
    
private:
    // Motor decoding: convert neural activity to motor command
    MotorCommand decodeFromMotorNeurons();
    
    // Motor command selection with curiosity/exploration
    MotorCommand selectWithCuriosity(MotorCommand defaultCmd);
    
    // Neural state extraction for cognitive systems
    std::vector<float> extractNeuralState() const;
    
    // Cognitive system initialization
    void initializeCognitiveSystems();
    
    // Cognitive evaluation with motor selection
    MotorCommand evaluateWithCognitiveSystems(const std::vector<float>& neuralState);
    
    // Cognitive-based curiosity and exploration
    void updateCognitiveCuriosity();
    
    // Integration points for cognitive systems
    void integrateWithPredictionSystem();
    void integrateWithMemorySystem();
    void integrateWithAttentionSystem();
    
    std::shared_ptr<Brain> brain_;
    
    // Cognitive systems
    std::unique_ptr<NeuralPlanner> planner_;
    std::unique_ptr<ConceptFormation> conceptFormation_;
    std::unique_ptr<NeuralWorkingMemory> workingMemory_;
    std::unique_ptr<AttentionalSelection> attention_;
    
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
    float cognitiveConfidence_;
    float conceptStability_;
    float attentionFocusLevel_;
    
    // Development state
    double developmentalAge_;
    float plasticityModifier_;
    
    // Cognitive evaluation state
    std::vector<float> previousNeuralState_;
    float planningConfidence_;
    float attentionWeight_;
    float conceptUpdateThreshold_;
    
    // Configuration flags
    bool rewardModulationEnabled_;
    bool structuralPlasticityEnabled_;
    bool developmentEnabled_;
    bool curiosityEnabled_;
    bool cognitivePlanningEnabled_;
    bool conceptBasedBehaviorEnabled_;
    bool attentionGuidanceEnabled_;
    bool predictionPlanningEnabled_;
    
    // Previous sensory state for novelty detection
    std::vector<float> previousVision_;
    float sensoryNoveltyDecay_;
    
    // Concept tracking for behavior
    std::unordered_map<size_t, float> conceptBehaviorWeights_;
    std::vector<size_t> activeConcepts_;
    
    // Prediction system integration
    float predictionAccuracy_;
    std::vector<float> predictedState_;
    
    // Attention system integration
    std::vector<RegionId> attendedRegions_;
    float topDownBiasStrength_;
    
    // Integration flags
    bool predictionSystemInitialized_;
    bool memorySystemInitialized_;
    bool attentionSystemInitialized_;
};

} // namespace nlm