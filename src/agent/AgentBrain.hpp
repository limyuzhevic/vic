// Enhanced AgentBrain interface connecting NLM brain to world
// Handles sensory transduction and motor decoding with cognitive integration
#pragma once

#include "AgentBody.hpp"
#include "SensoryPercept.hpp"
#include "../brain/Brain.hpp"
#include "../world/SimpleWorld.hpp"
#include <memory>
#include <vector>

namespace nlm {

class NeuralWorkingMemory;
class NeuralEpisodicMemory;
class PredictionSystem;
class NeuralPlanner;
class ConceptFormation;
class AttentionalSelection;
class SelfModel;
class SocialLearning;
class Dopamine;
class Novelty;
class Curiosity;
class PredictionError;

// AgentBrain: Connects NLM brain to the world
// Handles sensory transduction and motor decoding with integrated cognitive systems
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
    
    // Cognitive system access
    NeuralWorkingMemory* getWorkingMemory() { return workingMemory_.get(); }
    NeuralEpisodicMemory* getEpisodicMemory() { return episodicMemory_.get(); }
    PredictionSystem* getPredictionSystem() { return predictionSystem_.get(); }
    NeuralPlanner* getPlanner() { return planner_.get(); }
    ConceptFormation* getConceptFormation() { return conceptFormation_.get(); }
    AttentionalSelection* getAttention() { return attention_.get(); }
    SelfModel* getSelfModel() { return selfModel_.get(); }
    SocialLearning* getSocialLearning() { return socialLearning_.get(); }
    
    Dopamine* getDopamine() { return dopamine_.get(); }
    Curiosity* getCuriosity() { return curiosity_.get(); }
    Novelty* getNovelty() { return novelty_.get(); }
    PredictionError* getPredictionErrorSignal() { return predictionErrorSignal_.get(); }
    
    // Configuration for individual subsystems
    void enableRewardModulation(bool enable) { rewardModulationEnabled_ = enable; }
    void enableStructuralPlasticity(bool enable) { structuralPlasticityEnabled_ = enable; }
    void enableDevelopment(bool enable) { developmentEnabled_ = enable; }
    void enableCuriosity(bool enable) { curiosityEnabled_ = enable; }
    void enableConceptFormation(bool enable) { conceptFormationEnabled_ = enable; }
    void enablePlanning(bool enable) { planningEnabled_ = enable; }
    void enableSelfModel(bool enable) { selfModelEnabled_ = enable; }
    void enableSocialLearning(bool enable) { socialLearningEnabled_ = enable; }
    void enableMemoryConsolidation(bool enable) { memoryConsolidationEnabled_ = enable; }
    
    bool isRewardModulationEnabled() const { return rewardModulationEnabled_; }
    bool isStructuralPlasticityEnabled() const { return structuralPlasticityEnabled_; }
    bool isDevelopmentEnabled() const { return developmentEnabled_; }
    bool isCuriosityEnabled() const { return curiosityEnabled_; }
    bool isConceptFormationEnabled() const { return conceptFormationEnabled_; }
    bool isPlanningEnabled() const { return planningEnabled_; }
    bool isSelfModelEnabled() const { return selfModelEnabled_; }
    bool isSocialLearningEnabled() const { return socialLearningEnabled_; }
    bool isMemoryConsolidationEnabled() const { return memoryConsolidationEnabled_; }
    
private:
    // Motor decoding: convert neural activity to motor command
    MotorCommand decodeFromMotorNeurons();
    
    // Motor command selection with curiosity/exploration
    MotorCommand selectWithCuriosity(MotorCommand defaultCmd);
    
    // Helper methods for enhanced functionality
    float calculateSensoryVariability(const SensoryPercept& percept) const;
    float calculateMean(const std::vector<float>& values) const;
    void decayNoveltyLevel();
    
    std::shared_ptr<Brain> brain_;
    
    // Motor neuron groups
    std::vector<Neuron*> motorForward_;
    std::vector<Neuron*> motorBackward_;
    std::vector<Neuron*> motorTurnLeft_;
    std::vector<Neuron*> motorTurnRight_;
    std::vector<Neuron*> motorInteract_;
    std::vector<Neuron*> motorWait_;
    
    // Sensory neuron groups (legacy - use brain's sensory neurons)
    std::vector<Neuron*> sensoryVision_;
    std::vector<Neuron*> sensoryTouch_;
    std::vector<Neuron*> sensoryInternal_;
    std::vector<Neuron*> sensoryProprioception_;
    
    // Cognitive system integration
    std::unique_ptr<NeuralWorkingMemory> workingMemory_;
    std::unique_ptr<NeuralEpisodicMemory> episodicMemory_;
    std::unique_ptr<PredictionSystem> predictionSystem_;
    std::unique_ptr<NeuralPlanner> planner_;
    std::unique_ptr<ConceptFormation> conceptFormation_;
    std::unique_ptr<AttentionalSelection> attention_;
    std::unique_ptr<SelfModel> selfModel_;
    std::unique_ptr<SocialLearning> socialLearning_;
    
    // Neuromodulation systems
    std::unique_ptr<Dopamine> dopamine_;
    std::unique_ptr<Curiosity> curiosity_;
    std::unique_ptr<Novelty> novelty_;
    std::unique_ptr<PredictionError> predictionErrorSignal_;
    
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
    bool conceptFormationEnabled_;
    bool planningEnabled_;
    bool selfModelEnabled_;
    bool socialLearningEnabled_;
    bool memoryConsolidationEnabled_;
    
    // Previous sensory state for novelty detection
    std::vector<float> previousVision_;
    float sensoryNoveltyDecay_;
};

} // namespace nlm
