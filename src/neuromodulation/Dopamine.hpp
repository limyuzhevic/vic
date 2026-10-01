#pragma once

/**
 * Dopamine.hpp - Dopamine neuromodulator system
 * 
 * Implements dopamine signaling based on neuroscience research:
 * - Phasic responses to reward prediction errors
 * - Tonic baseline activity
 * - Modulates plasticity and neural excitability
 * - Implements reward learning
 * 
 * Role in NLM:
 * - Mediates reward prediction error signals
 * - Modulates synaptic plasticity via D1 receptor effects
 * - Controls exploration vs. exploitation balance
 * - Drives learning from surprising outcomes
 */

#include "../core/Types/Types.hpp"
#include <string>

namespace nlm {

class Dopamine {
public:
    Dopamine();
    ~Dopamine();
    
    /**
     * Get the neuromodulator name
     * 
     * @return "DA" (abbreviation for Dopamine)
     */
    const char* getName() const;
    
    /**
     * Get current dopamine level
     * 
     * @return Level between 0.0 (no dopamine) and 1.0 (maximal dopamine)
     */
    float getLevel() const;
    
    /**
     * Set dopamine level directly
     * 
     * @param level Desired level (will be clamped to [0.0, 1.0])
     */
    void setLevel(float level);
    
    /**
     * Get plasticity modulation factor based on current dopamine level
     * 
     * @return Factor that modulates plasticity rates (1.0 = baseline)
     */
    float getPlasticityFactor() const;
    
    /**
     * Update dopamine level based on time elapsed
     * 
     * Implements decay towards baseline and temporal dynamics
     * 
     * @param dt Time step duration (seconds or ms)
     */
    void update(TimestepDuration dt);
    
    /**
     * Signal that a reward was received
     * 
     * Creates a phasic dopamine burst proportional to reward size
     * 
     * @param reward Reward magnitude (positive only)
     */
    void signalReward(float reward);
    
    /**
     * Signal reward prediction error (RPE)
     * 
     * Implements the core dopamine RPE signal from reinforcement learning
     * 
     * @param error Prediction error (positive = better than expected, 
     *              negative = worse than expected)
     */
    void signalRewardPredictionError(float error);
    
    /**
     * Get accumulated reward (long-term reward history)
     * 
     * @return Total accumulated reward
     */
    float getAccumulatedReward() const;
    
    /**
     * Get the last prediction error signal
     * 
     * @return Last prediction error value
     */
    float getPredictionError() const;
    
    /**
     * Reset accumulated reward counter
     * 
     * Useful for episode-based reward tracking
     */
    void resetAccumulatedReward();
    
    /**
     * Get baseline dopamine level (tonic activity)
     * 
     * @return Baseline level
     */
    float getBaseline() const { return pImpl->baseline; }
    
    /**
     * Set baseline dopamine level
     * 
     * @param baseline New baseline (0.0 to 1.0)
     */
    void setBaseline(float baseline) { pImpl->baseline = baseline; }
    
    /**
     * Get peak dopamine level
     * 
     * @return Peak level
     */
    float getPeak() const { return pImpl->peak; }
    
    /**
     * Set peak dopamine level
     * 
     * @param peak New peak level (0.0 to 1.0)
     */
    void setPeak(float peak) { pImpl->peak = peak; }

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm