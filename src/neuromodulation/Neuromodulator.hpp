#pragma once

#include "../core/Types/Types.hpp"
#include <string>
#include <vector>

namespace nlm {

#pragma once

#include "../core/Types/Types.hpp"
#include <string>
#include <vector>

namespace nlm {

/**
 * @brief Abstract base class for neuromodulatory signals in the NLM system
 * 
 * Neuromodulators are chemical or electrical signals that modulate neural
 * activity and plasticity in the brain. In the NLM system, neuromodulators
 * play crucial roles in:
 * 
 * - **Dopamine**: Reward prediction, reinforcement learning, and motivation
 * - **Curiosity**: Exploration drive and intrinsic motivation
 * - **Novelty**: Detection of unexpected events and novelty detection
 * - **Prediction Error**: Learning from prediction errors (Bayesian learning)
 * - **Development**: Age-dependent plasticity and behavioral development
 * 
 * Neuromodulation is essential for:
 * 
 * 1. **Learning and Plasticity**: Neuromodulators strengthen or weaken synapses
 *    based on behavioral outcomes
 * 2. **Attention and Focus**: Neuromodulators help allocate neural resources
 * 3. **Motivation and Reward**: Neuromodulators drive goal-directed behavior
 * 4. **Development**: Neuromodulators change over the lifetime of the system
 * 5. **Exploration vs. Exploitation**: Neuromodulators balance new learning with
 *    exploitation of known behaviors
 * 
 * The abstract Neuromodulator class defines the interface for all
 * neuromodulatory systems, ensuring consistent behavior and integration
 * across the different modulation systems.
 */
class Neuromodulator {
public:
    /** Virtual destructor for proper polymorphic deletion */
    virtual ~Neuromodulator() = default;
    
    /**
     * @brief Get the name of the neuromodulator
     * 
     * @return Human-readable name of the neuromodulator (e.g., "DA", "Curiosity")
     */
    virtual const char* getName() const = 0;
    
    /**
     * @brief Get the current concentration or level of the neuromodulator
     * 
     * The level typically ranges from 0.0 (no modulation) to 1.0 (maximum modulation)
     * and can change dynamically based on behavioral and environmental factors.
     * 
     * @return Current modulation level
     */
    virtual float getLevel() const = 0;
    
    /**
     * @brief Set the current level of the neuromodulator
     * 
     * @param level New modulation level (typically clamped between 0.0 and 1.0)
     */
    virtual void setLevel(float level) = 0;
    
    /**
     * @brief Get the plasticity modulation factor
     * 
     * Neuromodulators affect synaptic plasticity by multiplying the
     * plasticity rule parameters. Higher neuromodulator levels typically
     * increase plasticity, but the exact relationship varies by neuromodulator.
     * 
     * @return Plasticity modulation factor (multiplier for learning rates)
     */
    virtual float getPlasticityFactor() const = 0;
    
    /**
     * @brief Update neuromodulator state over time
     * 
     * Neuromodulators evolve over time based on various inputs:
     * - Decay towards baseline levels
     * - Responses to reward/prediction errors
     * - Integration with other neuromodulators
     * - Development-related changes
     * 
     * @param dt Time step duration (typically in milliseconds)
     */
    virtual void update(TimestepDuration dt) = 0;
    
protected:
    /** Default constructor */
    Neuromodulator() = default;
};

/**
 * @brief Dopamine: Reward and reinforcement learning signal
 * 
 * Dopamine is the primary neuromodulator for reward prediction and reinforcement
 * learning in biological systems. In the NLM system, dopamine serves as:
 * 
 * 1. **Reward Prediction Error Signal**: Compares predicted vs. actual rewards
 * 2. **Motivation Signal**: Drives approach behavior towards rewarding stimuli
 * 3. **Plasticity Modulator**: Increases learning rates during reward prediction
 * 4. **Attention modulator**: Focuses neural resources on rewarding activities
 * 
 * Dopamine dynamics:
 * - Phasic bursts: Short, high-amplitude responses to unexpected rewards
 * - Tonic background: Persistent baseline level representing overall motivation
 * - Decay: Returns to baseline levels after events
 * - Integration: Summed effects of multiple reward sources
 * 
 * The Dopamine system interacts with:
 * - Reward prediction error signals from the prediction system
 * - Curiosity and novelty detection for exploration
 * - Memory systems for reward-based learning
 * - Motor systems for action selection
 */
class Dopamine : public Neuromodulator {
public:
    /** Default constructor */
    Dopamine();
    
    /** Virtual destructor */
    ~Dopamine() override;
    
    /**
     * @brief Get the neuromodulator name (should return "DA")
     * 
     * @return String identifier for this neuromodulator
     */
    const char* getName() const override;
    
    /**
     * @brief Get the current dopamine level
     * 
     * @return Current dopamine concentration (0.0 = no dopamine, 1.0 = maximum)
     */
    float getLevel() const override;
    
    /**
     * @brief Set dopamine level with bounds checking
     * 
     * @param level New dopamine level (automatically clamped to [0.0, 1.0])
     */
    void setLevel(float level) override;
    
    /**
     * @brief Get dopamine's effect on synaptic plasticity
     * 
     * Higher dopamine levels typically increase plasticity for
     * rewarded behaviors and decrease it for non-rewarded behaviors.
     * 
     * @return Plasticity modulation factor (typically 0.5 to 1.0)
     */
    float getPlasticityFactor() const override;
    
    /**
     * @brief Update dopamine state over time
     * 
     * Dopamine dynamics include:
     * - Decay towards baseline levels
     * - Responses to reward prediction errors
     * - Integration with other neuromodulators
     * - Development-related changes
     * 
     * @param dt Time step duration (typically in milliseconds)
     */
    void update(TimestepDuration dt) override;
    
    /**
     * @brief Signal a reward event to the dopamine system
     * 
     * This method models the phasic dopamine response to rewards,
     * increasing dopamine levels to signal reward prediction errors
     * and reinforce successful behaviors.
     * 
     * @param reward Magnitude of the reward (typically 0.0 to 1.0)
     */
    void signalReward(float reward);
    
    /**
     * @brief Signal a reward prediction error to the dopamine system
     * 
     * This models the predictive coding aspect of dopamine, where dopamine
     * responds to the difference between expected and actual rewards.
     * 
     * @param error Prediction error (positive for better-than-expected, 
     *               negative for worse-than-expected outcomes)
     */
    void signalRewardPredictionError(float error);
    
private:
    /**
     * @brief Internal implementation structure (Pimpl idiom)
     * 
     * Stores dopamine state variables and implementation details to
     * reduce compilation dependencies and improve encapsulation.
     */
    struct Impl;
    
    /** Pointer to internal implementation */
    Impl* pImpl;
};

} // namespace nlm
