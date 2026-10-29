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
    
    // Advanced agent behaviors and control systems
    
    // Set learning rate for plasticity
    void setLearningRate(float rate) { learningRate_ = rate; }
    
    // Get current learning rate
    float getLearningRate() const { return learningRate_; }
    
    // Set exploration-exploitation balance
    void setExplorationRate(float rate) { explorationRate_ = rate; }
    
    // Get exploration rate
    float getExplorationRate() const { return explorationRate_; }
    
    // Set goal position for goal-oriented behavior
    void setGoalPosition(float x, float y) {
        goalPosition_.x = x;
        goalPosition_.y = y;
        hasGoal_ = true;
    }
    
    // Get goal position
    AgentBody getGoalPosition() const { return goalPosition_; }
    
    // Check if goal is set
    bool hasGoal() const { return hasGoal_; }
    
    // Clear goal
    void clearGoal() { hasGoal_ = false; }
    
    // Enable/disable threat detection
    void enableThreatDetection(bool enable) { threatDetectionEnabled_ = enable; }
    
    // Get threat detection status
    bool isThreatDetectionEnabled() const { return threatDetectionEnabled_; }
    
    // Enable/disable social learning
    void enableSocialLearning(bool enable) { socialLearningEnabled_ = enable; }
    
    // Get social learning status
    bool isSocialLearningEnabled() const { return socialLearningEnabled_; }
    
    // Get behavior profile (JSON-like string representation)
    std::string getBehaviorProfile() const;
    
    // Set agent personality traits
    void setPersonality(float curiosity, float boldness, float exploration, float learning);
    
    // Get personality traits
    struct Personality {
        float curiosity;
        float boldness;
        float exploration;
        float learning;
    };
    Personality getPersonality() const { return personality_; }
    
    // Adaptive learning: adjust parameters based on performance
    void adaptiveLearning(const std::vector<float>& rewards, float successRate);
    
    // Get recent performance metrics
    struct PerformanceMetrics {
        float averageReward;
        float successRate;
        float explorationEfficiency;
        float learningSpeed;
    };
    PerformanceMetrics getPerformanceMetrics() const { return performanceMetrics_; }
    
    // Enable/disable multi-task learning
    void enableMultiTaskLearning(bool enable) { multiTaskLearning_ = enable; }
    
    // Get multi-task learning status
    bool isMultiTaskLearningEnabled() const { return multiTaskLearning_; }
    
    // Set task priorities (0.0-1.0, higher = more important)
    void setTaskPriority(const std::string& task, float priority);
    
    // Get task priority
    float getTaskPriority(const std::string& task) const;
    
    // Enable/disable transfer learning
    void enableTransferLearning(bool enable) { transferLearningEnabled_ = enable; }
    
    // Get transfer learning status
    bool isTransferLearningEnabled() const { return transferLearningEnabled_; }
    
    // Set knowledge base for transfer learning
    void setKnowledgeBase(const std::map<std::string, std::vector<float>>& knowledge);
    
    // Apply learned behaviors to new tasks
    std::vector<float> applyTransferLearning(const std::string& task);
    
    // Enable/disable hierarchical reinforcement learning
    void enableHierarchicalRL(bool enable) { hierarchicalRL_ = enable; }
    
    // Get hierarchical RL status
    bool isHierarchicalRLEnabled() const { return hierarchicalRL_; }
    
    // Set skill hierarchy levels
    void setSkillLevel(const std::string& skill, int level);
    
    // Get skill level
    int getSkillLevel(const std::string& skill) const;
    
    // Enable/disable meta-cognition (thinking about thinking)
    void enableMetaCognition(bool enable) { metaCognitionEnabled_ = enable; }
    
    // Get meta-cognition status
    bool isMetaCognitionEnabled() const { return metaCognitionEnabled_; }
    
    // Get confidence in current decisions
    float getDecisionConfidence() const { return decisionConfidence_; }
    
    // Get uncertainty in current state
    float getUncertainty() const { return uncertainty_; }
    
    // Reset all learned behaviors and personality
    void resetAll();
    
    // Save current state (for checkpointing)
    bool saveState(const std::string& filepath);
    
    // Load saved state
    bool loadState(const std::string& filepath);
    
    // Get current agent state summary
    std::string getStateSummary() const;
    
    // Create agent from configuration
    static std::shared_ptr<AgentBrain> createFromConfig(const std::shared_ptr<Config>& config);
    
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
    
    // Agent personality traits
    float curiosity_;
    float boldness_;
    float exploration_;
    float learning_;
    
    // Performance tracking
    float lastReward_;
    float successRate_;
    float explorationEfficiency_;
    float learningSpeed_;
    
    // Advanced learning systems
    std::map<std::string, std::vector<float>> knowledgeBase_;
    std::map<std::string, float> taskPriorities_;
    std::map<std::string, int> skillLevels_;
    
    // State management
    AgentBody goalPosition_;
    bool hasGoal_;
    bool threatDetectionEnabled_;
    bool socialLearningEnabled_;
    bool multiTaskLearning_;
    bool transferLearningEnabled_;
    bool hierarchicalRL_;
    bool metaCognitionEnabled_;
    
    // Cognitive state
    float decisionConfidence_;
    float uncertainty_;
    
    // Additional learning parameters
    float learningRate_;
    float explorationRate_;
    
    PerformanceMetrics performanceMetrics_;
