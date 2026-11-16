// Reward System Improvements for Phase 6
// 
// This file demonstrates enhanced reward computation and neuromodulation
// for Phase 6 integration experiments.

#pragma once

#include "brain/Brain.hpp"
#include "agent/AgentBrain.hpp"
#include "world/SimpleWorld.hpp"
#include <memory>
#include <vector>
#include <string>

namespace nlm {

/**
 * Enhanced Reward Configuration
 */
struct EnhancedRewardConfig {
    // Reward source weighting
    float resourceRewardWeight = 1.0f;
    float hazardPenaltyWeight = -1.0f;
    float noveltyRewardWeight = 0.5f;
    float curiosityRewardWeight = 0.3f;
    
    // Reward prediction error parameters
    float rewardPredictionLearningRate = 0.01f;
    float rewardPredictionErrorDecay = 0.95f;
    
    // Neuromodulation parameters
    float dopamineLearningRate = 0.005f;
    float dopamineDecayRate = 0.98f;
    float curiosityExplorationBonus = 0.2f;
    
    // Reward history tracking
    size_t rewardHistorySize = 1000;
    float rewardSmoothingFactor = 0.1f;
    
    EnhancedRewardConfig() = default;
};

/**
 * Enhanced Reward System
 * 
 * Provides sophisticated reward computation with:
 * - Multiple reward sources (resources, hazards, novelty, curiosity)
 * - Reward prediction error for learning
 * - Enhanced neuromodulation integration
 * - Reward history and statistics tracking
 */
class EnhancedRewardSystem {
public:
    EnhancedRewardSystem(std::shared_ptr<Brain> brain, std::shared_ptr<AgentBrain> agent);
    ~EnhancedRewardSystem();
    
    /**
     * Compute enhanced reward based on action and outcome
     */
    float computeEnhancedReward(const ActionResult& actionResult,
                               const SensoryPercept& percept,
                               const MotorCommand& command);
    
    /**
     * Update reward prediction and learning
     */
    void updateRewardPrediction(float actualReward, float predictedReward);
    
    /**
     * Get current reward statistics
     */
    struct RewardStats {
        float meanReward;
        float rewardVariance;
        float maxReward;
        float minReward;
        float recentAverage;
        size_t totalSteps;
        float noveltyContribution;
        float curiosityContribution;
    };
    
    RewardStats getRewardStats() const { return stats_; }
    
    /**
     * Get reward prediction error
     */
    float getRewardPredictionError() const { return rewardPredictionError_; }
    
    /**
     * Get current dopamine level
     */
    float getDopamineLevel() const { return dopamineLevel_; }
    
    /**
     * Get current curiosity level
     */
    float getCuriosityLevel() const { return curiosityLevel_; }
    
    /**
     * Check if system is learning
     */
    bool isLearning() const { return learning_ > 0.0f; }
    
private:
    std::shared_ptr<Brain> brain_;
    std::shared_ptr<AgentBrain> agent_;
    EnhancedRewardConfig config_;
    
    // Reward history and statistics
    std::vector<float> rewardHistory_;
    float runningSum_;
    float runningSumOfSquares_;
    
    // Reward prediction system
    float predictedReward_;
    float rewardPredictionError_;
    float predictionErrorAccumulator_;
    
    // Neuromodulation levels
    float dopamineLevel_;
    float curiosityLevel_;
    float noveltyLevel_;
    
    // Learning state
    float learning_;
    float explorationRate_;
    
    // Reward statistics
    RewardStats stats_;
    
    /**
     * Compute base reward from action result
     */
    float computeBaseReward(const ActionResult& actionResult) const;
    
    /**
     * Compute novelty reward based on sensory percept
     */
    float computeNoveltyReward(const SensoryPercept& percept) const;
    
    /**
     * Compute curiosity reward based on exploration
     */
    float computeCuriosityReward(const MotorCommand& command,
                                const SensoryPercept& percept) const;
    
    /**
     * Update neuromodulation levels
     */
    void updateNeuromodulation(float reward, float predictionError);
    
    /**
     * Update reward statistics
     */
    void updateStatistics(float reward);
};

/**
 * Enhanced Motor Control System
 * 
 * Provides sophisticated action selection with:
 * - Multiple action selection strategies
 * - Curiosity-driven exploration
 * - Reward prediction integration
 * - Self-model awareness
 * - Planning overrides
 */
class EnhancedMotorControl {
public:
    EnhancedMotorControl(std::shared_ptr<Brain> brain, std::shared_ptr<AgentBrain> agent);
    ~EnhancedMotorControl();
    
    /**
     * Select action using enhanced strategy
     */
    MotorCommand selectAction(const SensoryPercept& percept,
                             float curiosityLevel,
                             float noveltyLevel);
    
    /**
     * Get action selection statistics
     */
    struct ActionStats {
        float moveForwardScore;
        float moveBackwardScore;
        float turnLeftScore;
        float turnRightScore;
        float interactScore;
        float waitScore;
        float explorationScore;
        float predictionScore;
    };
    
    ActionStats getActionStats() const { return actionStats_; }
    
    /**
     * Get current action selection strategy
     */
    std::string getStrategyName() const { return strategyName_; }
    
    /**
     * Set action selection strategy
     */
    void setStrategy(const std::string& strategy) { strategyName_ = strategy; }
    
    /**
     * Get self-model confidence for actions
     */
    float getActionConfidence(MotorCommand command) const;
    
private:
    std::shared_ptr<Brain> brain_;
    std::shared_ptr<AgentBrain> agent_;
    
    // Action selection strategies
    enum Strategy {
        SIMPLE_MAXIMUM,
        EXPLORATION_DRIVEN,
        REWARD_PREDICTION,
        MULTI_STRATEGY
    };
    
    Strategy currentStrategy_;
    std::string strategyName_;
    
    // Action statistics
    ActionStats actionStats_;
    
    // Action history for learning
    std::vector<std::pair<MotorCommand, float>> actionHistory_;
    
    /**
     * Select action using maximum firing rate
     */
    MotorCommand selectMaxFiring(const SensoryPercept& percept);
    
    /**
     * Select action using exploration strategy
     */
    MotorCommand selectExploration(const SensoryPercept& percept,
                                   float curiosityLevel,
                                   float noveltyLevel);
    
    /**
     * Select action using reward prediction strategy
     */
    MotorCommand selectRewardPrediction(const SensoryPercept& percept,
                                       float curiosityLevel);
    
    /**
     * Compute action scores using multiple strategies
     */
    ActionStats computeActionScores(const SensoryPercept& percept,
                                   float curiosityLevel,
                                   float noveltyLevel);
    
    /**
     * Get action score based on motor neuron group activity
     */
    float getActionScore(MotorCommand command) const;
    
    /**
     * Add exploration bonus to action selection
     */
    MotorCommand addExplorationBonus(const MotorCommand& baseAction,
                                     const SensoryPercept& percept) const;
};

/**
 * Complete Phase 6 Enhanced Simulation
 * 
 * Demonstrates the enhanced reward system and motor control
 * in a complete Phase 6 integration experiment.
 */
class Phase6EnhancedExperiment {
public:
    Phase6EnhancedExperiment();
    ~Phase6EnhancedExperiment();
    
    /**
     * Run the enhanced Phase 6 experiment
     */
    bool run(const EnhancedRewardConfig& rewardConfig);
    
    /**
     * Get experiment results
     */
    struct EnhancedResults {
        float totalReward;
        float avgFiringRate;
        float rewardPredictionError;
        float dopamineLevel;
        float curiosityLevel;
        float noveltyLevel;
        float learningProgress;
        size_t successfulActions;
        float actionEfficiency;
        float explorationRate;
    };
    
    EnhancedResults getResults() const { return results_; }
    
    /**
     * Print enhanced experiment summary
     */
    void printSummary() const;
    
private:
    EnhancedResults results_;
    std::unique_ptr<Brain> brain_;
    std::unique_ptr<AgentBrain> agent_;
    std::unique_ptr<SimpleWorld> world_;
    std::unique_ptr<EnhancedRewardSystem> rewardSystem_;
    std::unique_ptr<EnhancedMotorControl> motorControl_;
    
    /**
     * Initialize enhanced experiment
     */
    bool initialize();
    
    /**
     * Run enhanced simulation loop
     */
    void runEnhancedSimulation();
    
    /**
     * Collect enhanced results
     */
    void collectResults();
};

} // namespace nlm
