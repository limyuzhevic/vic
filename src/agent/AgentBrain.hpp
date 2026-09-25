#pragma once

#include "AgentBody.hpp"
#include "SensoryPercept.hpp"
#include "../brain/Brain.hpp"
#include "../world/SimpleWorld.hpp"
#include "../cognition/NeuralPlanner.hpp"
#include "../cognition/ConceptFormation.hpp"
#include "../cognition/SelfModel.hpp"
#include "../cognition/SocialLearning.hpp"
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
    
    // === COGNITIVE SYSTEM INTEGRATION ===
    
    // Neural planner integration
    void enableNeuralPlanner(bool enable) { neuralPlannerEnabled_ = enable; }
    bool isNeuralPlannerEnabled() const { return neuralPlannerEnabled_; }
    void setPlanningDepth(size_t depth) { if (neuralPlanner_) { neuralPlanner_->setPlanningDepth(depth); } }
    
    // Concept formation integration
    void enableConceptFormation(bool enable) { conceptFormationEnabled_ = enable; }
    bool isConceptFormationEnabled() const { return conceptFormationEnabled_; }
    
    // Self-model integration
    void enableSelfModel(bool enable) { selfModelEnabled_ = enable; }
    bool isSelfModelEnabled() const { return selfModelEnabled_; }
    
    // Social learning integration
    void enableSocialLearning(bool enable) { socialLearningEnabled_ = enable; }
    bool isSocialLearningEnabled() const { return socialLearningEnabled_; }
    
    // === COGNITIVE SYSTEM ACCESSORS ===
    
    NeuralPlanner* getNeuralPlanner() { return neuralPlanner_.get(); }
    ConceptFormation* getConceptFormation() { return conceptFormation_.get(); }
    SelfModel* getSelfModel() { return selfModel_.get(); }
    SocialLearning* getSocialLearning() { return socialLearning_.get(); }
    
    // === ADVANCED FUNCTIONALITY ===
    
    // Novelty-based learning
    void processNovelty(const std::vector<float>& currentVision);
    
    // Experience encoding for concept formation
    void encodeExperience(const SensoryPercept& percept, float reward,
                          SimulationStep currentStep);
    
    // Prediction-based action selection
    MotorCommand decodeWithPrediction(const std::vector<float>& currentState);
    
    // Attention control
    void focusAttention(NeuronId neuron);
    void releaseAttention();
    
    // Cognitive learning
    void learnFromExperience(const SensoryPercept& percept, ActionResult actionResult,
                             SimulationStep currentStep);
    
    // Get cognitive statistics
    size_t getConceptCount() const;
    float getNoveltyLevel() const;
    float getPredictionAccuracy() const;
    
private:
    // Motor decoding: convert neural activity to motor command
    MotorCommand decodeFromMotorNeurons();
    
    // Motor command selection with curiosity/exploration
    MotorCommand selectWithCuriosity(MotorCommand defaultCmd);
    
    std::shared_ptr<Brain> brain_;
    
    // Cognitive systems
    std::unique_ptr<NeuralPlanner> neuralPlanner_;
    std::unique_ptr<ConceptFormation> conceptFormation_;
    std::unique_ptr<SelfModel> selfModel_;
    std::unique_ptr<SocialLearning> socialLearning_;
    
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
    
    // Cognitive configuration flags
    bool neuralPlannerEnabled_;
    bool conceptFormationEnabled_;
    bool selfModelEnabled_;
    bool socialLearningEnabled_;
    
    // Previous sensory state for novelty detection
    std::vector<float> previousVision_;
    float sensoryNoveltyDecay_;
    
    // Cognitive state tracking
    size_t experienceCount_;
    float predictionAccuracy_;
    std::vector<float> recentPredictions_;
};

} // namespace nlm
