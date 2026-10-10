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
    
    bool isRewardModulationEnabled() const { return rewardModulationEnabled_; }
    bool isStructuralPlasticityEnabled() const { return structuralPlasticityEnabled_; }
    bool isDevelopmentEnabled() const { return developmentEnabled_; }
    bool isCuriosityEnabled() const { return curiosityEnabled_; }

private:
    // ===== MOTOR STRATEGY ENUM =====
    enum class MotorCommandStrategy {
        ActivityBased,      // Pure neural activity
        CuriosityBased,     // Exploration-driven
        RewardBased,        // Reinforcement learning
        Hybrid,             // Combination
        DevelopmentBased    // Development-stage specific
    };
    
    // ===== ADVANCED STATE VARIABLES =====
    
    // System health and status
    mutable bool systemHealthy_;
    mutable float systemEnergy_;
    mutable size_t lastHealthCheck_;
    
    // Advanced neuromodulation state
    float explorationBonus_;
    float confidenceLevel_;
    float noveltyAttenuation_;
    float rewardHistory_[10];  // Last 10 rewards
    size_t rewardHistoryIndex_;
    
    // Motor command history for learning
    struct MotorHistoryEntry {
        MotorCommand command;
        float activity;
        float reward;
        double timestamp;
    };
    std::vector<MotorHistoryEntry> motorHistory_;
    size_t maxHistorySize_;
    
    // Advanced developmental state
    float customPlasticityMultiplier_;
    float developmentProgress_;
    std::vector<float> developmentalWindows_;
    
    // Configuration for expert users
    size_t motorGroupCounts_[6];  // [forward, backward, left, right, interact, wait]
    size_t sensoryGroupCounts_[4]; // [vision, touch, internal, proprioception]
    float curiosityThreshold_;  // Minimum curiosity for exploration
    
    // ===== ADVANCED PRIVATE METHODS =====
    
    // Motor decoding with different strategies
    MotorCommand decodeActivityBased();
    MotorCommand decodeCuriosityBased(MotorCommand defaultCmd);
    MotorCommand decodeRewardBased();
    MotorCommand decodeDevelopmentBased();
    
    // Advanced curiosity and exploration
    MotorCommand selectWithAdvancedCuriosity(MotorCommand defaultCmd);
    float calculateCuriosityComposite() const;
    float calculateExplorationProbability() const;
    
    // Reward processing
    float calculateRewardPrediction(float reward) const;
    void updateRewardHistory(float reward);
    float getAverageRecentReward(size_t count = 5) const;
    
    // Developmental systems
    void updateDevelopmentalWindows();
    float calculateStageSpecificPlasticity() const;
    
    // History-based learning
    void updateMotorHistory(MotorCommand command, float activity, float reward, double timestamp);
    MotorCommand selectBasedOnHistory(size_t lookbackSteps = 10);
    
    // System health and status
    void updateSystemHealth() const;
    void updateSystemEnergy() const;
    
    // Safety and robustness
    bool validateMotorCommand(MotorCommand cmd) const;
    bool validateSensoryConfig() const;
    void ensureSystemConsistency() const;
};
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
