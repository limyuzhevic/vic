#pragma once

#include "Neuromodulator.hpp"
#include <vector>

namespace nlm {

/**
 * @class Reward
 * @brief Reward signal for reinforcement learning with comprehensive reward shaping
 * 
 * The Reward class provides a sophisticated reward system for NLM (Neural Learning Machine)
 * that includes multiple reward objectives, exploration bonuses, risk awareness,
 * and long-term planning capabilities. This class is designed to support both
 * simple reinforcement learning scenarios and complex multi-objective learning.
 * 
 * Key features:
 * - Multi-objective reward computation (survival, efficiency, exploration, novelty)
 * - Risk-aware reward shaping for safe learning
 * - Comprehensive reward history tracking and statistics
 * - Exploration bonuses for curiosity-driven learning
 * - Long-term planning with discount factors
 * - State checkpointing for replay and recovery
 * - External reward integration for curriculum learning
 * - Reward analysis with variance and entropy calculations
 * 
 * The reward system is designed to be:
 * - Biologically plausible (inspired by neuromodulation)
 * - Computationally efficient (bounded storage and processing)
 * - Flexible (supports various reward schemes)
 * - Robust (handles sparse and dense reward environments)
 */

class Reward {
public:
    /**
     * @brief Default constructor
     */
    Reward();
    
    /**
     * @brief Destructor
     */
    ~Reward();
    
    //=== Basic Reward Management ===
    
    /**
     * @brief Get the current reward value (most recent reward)
     * @return Current reward value
     */
    float getValue() const;
    
    /**
     * @brief Set the current reward value
     * @param value The new current reward value
     */
    void setValue(float value);
    
    /**
     * @brief Get the accumulated reward (total over time)
     * @return Total accumulated reward since last reset
     */
    float getAccumulatedReward() const;
    
    /**
     * @brief Add a reward delta to the current reward
     * @param delta The reward delta to add
     */
    void add(float delta);
    
    /**
     * @brief Reset all reward values to zero
     */
    void reset();
    
    //=== Core Reward Computation ===
    
    /**
     * @brief Compute reward based on current observation using basic reward shaping
     * 
     * This method implements the basic reward computation that considers:
     * - Energy preservation (encourage efficiency)
     * - Health maintenance (survival objective)
     * - Activity costs (movement, turning)
     * - Neuromodulation (dopamine influence)
     * 
     * @param observation Current sensory observation from the environment
     * @return Computed reward value
     */
    float computeReward(const class Observation& observation) const;
    
    /**
     * @brief Compute reward using multiple objectives for better learning
     * 
     * This method computes reward by combining multiple objectives:
     * - Survival: health and energy preservation
     * - Efficiency: successful interactions vs. movement costs
     * - Exploration: curiosity-driven rewards
     * - Novelty: reward for encountering new situations
     * 
     * @param observation Current sensory observation
     * @return Multi-objective reward value
     */
    float computeMultiObjectiveReward(const class Observation& observation);
    
    /**
     * @brief Compute risk-aware reward that considers uncertainty and safety
     * 
     * This method adjusts rewards based on estimated risk levels to promote:
     * - Safe exploration (avoid dangerous actions)
     * - Conservative learning in uncertain environments
     * - Balance between exploration and exploitation
     * 
     * @param observation Current sensory observation
     * @return Risk-adjusted reward value
     */
    float computeRiskAwareReward(const class Observation& observation);
    
    //=== Reward History and Statistics ===
    
    /**
     * @brief Get average reward from history
     * @return Average reward value
     */
    float getAverageReward() const;
    
    /**
     * @brief Get maximum reward from history
     * @return Maximum reward value
     */
    float getMaxReward() const;
    
    /**
     * @brief Get minimum reward from history
     * @return Minimum reward value
     */
    float getMinReward() const;
    
    /**
     * @brief Get standard deviation of rewards from history
     * @return Standard deviation
     */
    float getStdDeviation() const;
    
    /**
     * @brief Get reward at specific index in history
     * @param index Index in history (0 = oldest, history.size()-1 = most recent)
     * @return Reward value at specified index
     */
    float getRewardAt(size_t index) const;
    
    /**
     * @brief Get number of rewards in history
     * @return Size of reward history
     */
    size_t getHistorySize() const;
    
    /**
     * @brief Normalize reward history to specified range
     * @param minTarget Minimum target value after normalization
     * @param maxTarget Maximum target value after normalization
     */
    void normalizeHistory(float minTarget, float maxTarget);
    
    /**
     * @brief Get cumulative reward from start of current episode
     * @return Total reward accumulated in current episode
     */
    float getEpisodeReturn() const;
    
    /**
     * @brief Get discounted return over a lookback period
     * @param gamma Discount factor (0.0 = no future consideration, 1.0 = full future)
     * @param lookbackSteps Number of steps to look back
     * @return Discounted return value
     */
    float getDiscountedReturn(float gamma, size_t lookbackSteps) const;
    
    /**
     * @brief Get complete reward history
     * @return Reference to internal reward history vector
     */
    const std::vector<float>& getHistory() const;
    
    /**
     * @brief Clear all reward history
     */
    void clearHistory();
    
    //=== Exploration and Learning ===
    
    /**
     * @brief Update exploration bonus based on novelty detection
     * @param novelty Novelty score (0.0 = familiar, 1.0 = completely novel)
     */
    void updateExploration(float novelty);
    
    /**
     * @brief Check if agent should explore based on current state
     * @param threshold Exploration threshold (higher = more exploration)
     * @return True if agent should explore, false otherwise
     */
    bool shouldExplore(float threshold) const;
    
    /**
     * @brief Estimate risk level based on current observation
     * @param observation Current sensory observation
     * @return Risk score (0.0 = safe, 1.0 = high risk)
     */
    float estimateRisk(const class Observation& observation) const;
    
    //=== Long-term Planning ===
    
    /**
     * @brief Compute long-term reward with discount factor
     * @param discountFactor Discount factor for future rewards (typically < 1.0)
     * @param lookbackSteps Number of steps to consider for long-term planning
     * @return Long-term discounted reward
     */
    float computeLongTermReward(float discountFactor, size_t lookbackSteps) const;
    
    /**
     * @brief Set the starting value for current episode
     * @param startValue Reward value at start of episode
     */
    void setEpisodeStart(float startValue);
    
    /**
     * @brief Get the starting value of current episode
     * @return Reward value at start of episode
     */
    float getEpisodeStart() const;
    
    //=== State Management ===
    
    /**
     * @brief Save current state for checkpointing
     * @param state Vector to store state data
     */
    void saveState(std::vector<float>& state) const;
    
    /**
     * @brief Load state from checkpoint
     * @param state Vector containing state data
     */
    void loadState(const std::vector<float>& state);
    
    //=== External Influence ===
    
    /**
     * @brief Update reward from external source (e.g., expert demonstrations)
     * @param externalReward External reward value
     */
    void updateFromExternalSource(float externalReward);
    
    //=== Analysis and Statistics ===
    
    /**
     * @brief Get variance of rewards in history
     * @return Variance of reward distribution
     */
    float getRewardVariance() const;
    
    /**
     * @brief Get entropy of reward distribution
     * @return Entropy (measure of unpredictability)
     */
    float getRewardEntropy() const;
    
    /**
     * @brief Get current exploration bonus
     * @return Current exploration bonus value
     */
    float getExplorationBonus() const;
    
    /**
     * @brief Set exploration bonus
     * @param bonus New exploration bonus value
     */
    void setExplorationBonus(float bonus);
    
private:
    /**
     * @brief Internal implementation structure (pImpl pattern)
     */
    struct Impl;
    
    /**
     * @brief Pointer to internal implementation
     */
    Impl* pImpl;
};

} // namespace nlm
