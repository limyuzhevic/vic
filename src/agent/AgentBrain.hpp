#pragma once

#include "AgentBody.hpp"
#include "SensoryPercept.hpp"
#include "../brain/Brain.hpp"
#include "../world/SimpleWorld.hpp"
#include <memory>
#include <vector>
#include <unordered_map>
#include <cmath>

namespace nlm {

// Advanced AgentBrain configuration options
struct AgentBrainConfig {
    // Sensory processing parameters
    float visionScale = 5.0f;
    float touchScale = 8.0f;
    float internalScale = 5.0f;
    float proprioceptionScale = 3.0f;
    
    // Novelty detection parameters
    float noveltySensitivity = 1.0f;
    float noveltyDecayRate = 0.99f;
    
    // Curiosity parameters
    float curiosityBaseLevel = 0.1f;
    float curiosityNoveltyWeight = 2.0f;
    float curiosityPredictionErrorWeight = 0.5f;
    float curiosityMaxLevel = 1.0f;
    
    // Exploration parameters
    float explorationThreshold = 0.5f;
    float explorationRandomFactor = 0.3f;
    float explorationCoolDown = 10.0;
    
    // Neuromodulation parameters
    float rewardPredictionErrorAlpha = 0.95f;
    float dopaminePositiveGain = 1.0f;
    float dopamineNegativeGain = 0.8f;
    float plasticityModulationRange = 2.0f;
    
    // Development parameters
    double developmentalStageDuration[4] = {60.0, 240.0, 600.0, 1000.0};  // Initial, Critical, Maturation, Adult
    float developmentalPlasticityModifiers[4] = {1.0f, 0.8f, 0.5f, 0.2f};
    
    // Memory integration parameters
    float workingMemoryWeight = 0.3f;
    float episodicMemoryWeight = 0.5f;
    float predictionMemoryWeight = 0.2f;
    
    // Social learning parameters
    float socialObservationWindow = 10.0;
    float socialImitationProbability = 0.3f;
    float socialModelConfidenceThreshold = 0.7f;
};

// AgentBrain: Connects NLM brain to the world
// Handles sensory transduction and motor decoding
class AgentBrain {
public:
    AgentBrain(std::shared_ptr<Brain> brain, const AgentBrainConfig& config = AgentBrainConfig());
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
    
    // Get exploration level
    float getExplorationLevel() const;
    
    // Get developmental age
    double getDevelopmentalAge() const;
    
    // Get plasticity modifier
    float getPlasticityModifier() const;
    
    // Reset agent for new episode
    void reset();
    
    // Get brain pointer
    Brain* getBrain() { return brain_.get(); }
    
    // Configuration
    void setConfig(const AgentBrainConfig& config) { config_ = config; }
    const AgentBrainConfig& getConfig() const { return config_; }
    
    // Learning history tracking
    struct LearningEpisode {
        double timestamp;
        float reward;
        float curiosity;
        float novelty;
        MotorCommand action;
        float predictionError;
        float dopamineLevel;
    };
    
    std::vector<LearningEpisode> getLearningHistory() const { return learningHistory_; }
    void clearLearningHistory() { learningHistory_.clear(); }
    
    // Pattern recognition
    struct Pattern {
        std::vector<float> features;
        float strength;
        double lastSeen;
    };
    
    void recordPattern(const std::vector<float>& features);
    std::vector<Pattern> getPatterns() const { return patterns_; }
    void clearPatterns() { patterns_.clear(); }
    
    // Social learning
    struct SocialModel {
        std::string id;
        std::vector<float> behaviorPattern;
        float reliability;
        double lastObserved;
    };
    
    void observeSocialBehavior(const std::string& modelId, const std::vector<float>& behavior);
    MotorCommand imitateSocialBehavior();
    
private:
    // Motor decoding: convert neural activity to motor command
    MotorCommand decodeFromMotorNeurons();
    
    // Motor command selection with curiosity/exploration
    MotorCommand selectWithCuriosity(MotorCommand defaultCmd);
    
    // Advanced curiosity calculation
    void updateCuriosityLevel();
    
    // Novelty detection with temporal discounting
    void computeNovelty(const std::vector<float>& vision);
    
    // Reward prediction error computation
    void computePredictionError(float reward, float predictedReward);
    
    // Dopamine-like modulation
    void updateDopamineLevel();
    
    // Pattern extraction from sensory data
    std::vector<float> extractFeatures(const std::vector<float>& vision) const;
    
    // Pattern matching
    float matchPattern(const std::vector<float>& features) const;
    
    // Social behavior learning
    void updateSocialModels(const std::vector<float>& behavior);
    
    // Development system
    void updateDevelopmentalSystem(double timestep);
    
    // Advanced memory integration
    void integrateWorkingMemory();
    void integrateEpisodicMemory();
    void integratePredictionMemory();
    
    // Configuration
    AgentBrainConfig config_;
    
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
    
    // Advanced state variables
    float explorationLevel_;
    double lastExplorationTime_;
    std::vector<float> socialBehaviorPattern_;
    std::vector<SocialModel> socialModels_;
    
    // Learning and memory
    std::vector<LearningEpisode> learningHistory_;
    std::vector<Pattern> patterns_;
    
    // Temporal integration
    double currentTime_;
    double timeStep_;
};
