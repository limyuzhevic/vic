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
    void enableSocialLearning(bool enable) { socialLearningEnabled_ = enable; }
    
    bool isRewardModulationEnabled() const { return rewardModulationEnabled_; }
    bool isStructuralPlasticityEnabled() const { return structuralPlasticityEnabled_; }
    bool isDevelopmentEnabled() const { return developmentEnabled_; }
    bool isCuriosityEnabled() const { return curiosityEnabled_; }
    bool isAttentionEnabled() const { return attentionEnabled_; }
    bool isSocialLearningEnabled() const { return socialLearningEnabled_; }
    
    // Advanced configuration methods
    void setExplorationWeight(float weight) { explorationWeight_ = weight; }
    void setExploitationWeight(float weight) { exploitationWeight_ = weight; }
    void setMemoryRetrievalStrength(float strength) { memoryRetrievalStrength_ = strength; }
    void setSocialLearningStrength(float strength) { socialLearningStrength_ = strength; }
    void setAttentionSpan(float span) { attentionSpan_ = span; }
    void setCuriosityDecayRate(float rate) { curiosityDecayRate_ = rate; }
    
    float getExplorationWeight() const { return explorationWeight_; }
    float getExploitationWeight() const { return exploitationWeight_; }
    float getMemoryRetrievalStrength() const { return memoryRetrievalStrength_; }
    float getSocialLearningStrength() const { return socialLearningStrength_; }
    float getAttentionSpan() const { return attentionSpan_; }
    float getCuriosityDecayRate() const { return curiosityDecayRate_; }
    
    // Agent behavior control
    void setGoal(const std::string& goal) { goal_ = goal; }
    void setStrategy(Strategy strategy) { strategy_ = strategy; }
    std::string getGoal() const { return goal_; }
    Strategy getStrategy() const { return strategy_; }
    
    // Action planning and decision making
    ActionPlan createActionPlan(float confidenceThreshold = 0.7f);
    void setActionPlan(ActionPlan plan);
    ActionPlan getCurrentActionPlan() const { return currentActionPlan_; }
    
    // Memory-based decision making
    void learnFromMemory(const EpisodicMemoryItem& memory);
    float evaluateActionBenefit(const MotorCommand& action) const;
    float getActionValue(const MotorCommand& action) const;
    void updateActionValue(const MotorCommand& action, float value, float learningRate = 0.1f);
    
    // Social learning
    void observeOtherAgent(const AgentBrain& otherAgent);
    void shareKnowledge(const AgentBrain& otherAgent);
    void teachSkill(const std::string& skillName, float proficiency);
    
    // Advanced curiosity and exploration
    void setCuriosityParameters(float base, float noveltyWeight, float predictionErrorWeight);
    void setExplorationStrategy(ExplorationStrategy strategy);
    void adjustExplorationBasedOnContext();
    
    // Performance tracking
    float getLearningProgress() const;
    float getPerformanceScore() const;
    float getConfidenceInDecisions() const;
    
    // Stress and fatigue management
    void setStressLevel(float stress);
    void updateStressBasedOnPerformance(float performance);
    void resetStress();
    
    // Agent personality traits
    void setPersonalityTraits(float curiosity, float exploration, float social, float conservative);
    void adjustPersonalityOverTime(double timestep);
    
    // Advanced reset functionality
    void reset(bool preserveMemory = false, bool resetPersonality = true);
    void saveState(const std::string& filename) const;
    void loadState(const std::string& filename);
    
    // Get brain pointer
    Brain* getBrain() const { return brain_.get(); }
    
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
