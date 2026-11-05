#pragma once

#include "Neuromodulator.hpp"
#include "../environment/Observation.hpp"
#include <vector>

namespace nlm {

/**
 * @class Reward
 * @brief Reward signal for reinforcement learning and neuromodulation
 * 
 * The Reward class provides a comprehensive reward computation system for the NLM brain.
 * It computes rewards from sensory observations, maintains reward history, and provides
 * both instantaneous and accumulated reward signals for neuromodulation.
 * 
 * Reward computation integrates multiple factors:
 * - Resource finding (bright vision + proximity)
 * - Hazard avoidance (dark vision + proximity)
 * - Internal motivation (novelty and exploration)
 * - Movement efficiency (forward movement bias)
 * 
 * The reward system is designed to:
 * - Provide meaningful feedback for learning
 * - Balance exploration vs. exploitation
 * - Support diverse task environments
 * - Integrate with dopamine-based neuromodulation
 * 
 * @note This is Phase 2+: Real reward computation implementation
 *       (was a placeholder in earlier phases)
 */

class Reward {
public:
    /**
     * @brief Construct a new Reward object
     * 
     * Initializes reward accumulation and history tracking.
     */
    Reward();
    
    /**
     * @brief Destroy the Reward object
     * 
     * Cleans up reward state and resources.
     */
    ~Reward();
    
    /**
     * @brief Get the current reward value
     * 
     * @return float Current instantaneous reward value
     */
    float getValue() const;
    
    /**
     * @brief Set the current reward value
     * 
     * @param value New reward value to set
     */
    void setValue(float value);
    
    /**
     * @brief Add to the current reward value
     * 
     * @param delta Value to add to current reward
     */
    void add(float delta);
    
    /**
     * @brief Reset accumulated reward to zero
     * 
     * Clears both current and accumulated reward values.
     */
    void reset();
    
    /**
     * @brief Compute reward from sensory observation
     * 
     * Computes a comprehensive reward value based on the current sensory observation,
     * integrating multiple reward sources:
     * - Resource finding reward (bright vision + proximity)
     * - Hazard avoidance reward (dark vision + proximity)
     * - Internal motivation reward (novelty, exploration)
     * - Movement efficiency reward (forward movement)
     * 
     * The computed reward is normalized and clipped to a reasonable range [-2.0, 5.0].
     * 
     * @param observation Current sensory observation from the environment
     * @return float Computed reward value
     * 
     * @note This method implements real reward computation (Phase 2+),
     *       replacing the placeholder implementation from earlier phases.
     */
    float computeReward(const Observation& observation) const;
    
    /**
     * @brief Get the reward history
     * 
     * @return const std::vector<float>& Vector of past reward values
     */
    const std::vector<float>& getHistory() const;
    
    /**
     * @brief Clear the reward history
     */
    void clearHistory();
    
    /**
     * @brief Get the accumulated reward sum
     * 
     * @return float Total accumulated reward over time
     */
    float getAccumulatedReward() const;
    
    /**
     * @brief Check if reward computation is valid for an observation
     * 
     * @param observation Observation to check
     * @return true if observation is valid for reward computation
     */
    bool isValidObservation(const Observation& observation) const;
    
    /**
     * @brief Get reward components for analysis
     * 
     * @param observation Sensory observation
     * @return std::vector<float> Vector of reward components [resource, hazard, internal, proprio]
     */
    std::vector<float> getRewardComponents(const Observation& observation) const;
    
    /**
     * @brief Apply learned preference to reward computation
     * 
     * @param preference Learned preference value (-1.0 to 1.0)
     * @param observation Current sensory observation
     * @return float Reward value adjusted by learned preference
     */
    float computeRewardWithPreference(float preference, const Observation& observation) const;
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm

