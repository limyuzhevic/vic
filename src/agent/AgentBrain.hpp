#pragma once

#include "AgentBody.hpp"
#include "SensoryPercept.hpp"
#include "../brain/Brain.hpp"
#include "../world/SimpleWorld.hpp"
#include <memory>
#include <vector>

namespace nlm {

// Forward declarations for cognitive systems
class NeuralPlanner;
class ConceptFormation;
class AttentionalSelection;
class SelfModel;
class SocialLearning;

// AgentBrain: Connects NLM brain to the world
// Handles sensory transduction and motor decoding with cognitive system integration
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
    // Also processes through cognitive systems for integrated behavior
    void processSensoryInput(const SensoryPercept& percept);
    
    // Decode brain motor activity into motor command
    // Integrates planner decisions and attention for action selection
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
    
    // Get cognitive system pointers for integration
    NeuralPlanner* getPlanner() { return planner_.get(); }
    ConceptFormation* getConceptFormation() { return conceptFormation_.get(); }
    AttentionalSelection* getAttention() { return attention_.get(); }
    SelfModel* getSelfModel() { return selfModel_.get(); }
    SocialLearning* getSocialLearning() { return socialLearning_.get(); }
    
    // Configure cognitive system integration
    void enablePlanner(bool enable) { plannerEnabled_ = enable; }
    void enableConceptFormation(bool enable) { conceptFormationEnabled_ = enable; }
    void enableAttention(bool enable) { attentionEnabled_ = enable; }
    void enableSelfModel(bool enable) { selfModelEnabled_ = enable; }
    void enableSocialLearning(bool enable) { socialLearningEnabled_ = enable; }
    
    bool isPlannerEnabled() const { return plannerEnabled_; }
    bool isConceptFormationEnabled() const { return conceptFormationEnabled_; }
    bool isAttentionEnabled() const { return attentionEnabled_; }
    bool isSelfModelEnabled() const { return selfModelEnabled_; }
    bool isSocialLearningEnabled() const { return socialLearningEnabled_; }
    
    // Integration methods
    void integrateCognitiveSystems();
    bool areCognitiveSystemsIntegrated() const;
    
private:
    // Motor decoding: convert neural activity to motor command
    MotorCommand decodeFromMotorNeurons();
    
    // Motor command selection with curiosity/exploration
    MotorCommand selectWithCuriosity(MotorCommand defaultCmd);
    
    std::shared_ptr<Brain> brain_;
    
    // Cognitive system integration
    std::unique_ptr<NeuralPlanner> planner_;
    std::unique_ptr<ConceptFormation> conceptFormation_;
    std::unique_ptr<AttentionalSelection> attention_;
    std::unique_ptr<SelfModel> selfModel_;
    std::unique_ptr<SocialLearning> socialLearning_;
    
    // Configuration flags
    bool rewardModulationEnabled_;
    bool structuralPlasticityEnabled_;
    bool developmentEnabled_;
    bool curiosityEnabled_;
    bool plannerEnabled_;
    bool conceptFormationEnabled_;
    bool attentionEnabled_;
    bool selfModelEnabled_;
    bool socialLearningEnabled_;
    
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
    
    // Previous sensory state for novelty detection
    std::vector<float> previousVision_;
    float sensoryNoveltyDecay_;
};

} // namespace nlm
