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
 * @class Neuromodulator
 * @brief Abstract base class for neuromodulatory signals in the brain
 * 
 * Neuromodulators are diffuse chemical signals that modulate neural activity
 * and plasticity throughout the brain. This abstract base class defines the
 * interface for all neuromodulator systems.
 * 
 * Neuromodulators influence:
 * - Neural excitability (affects firing rates)
 * - Synaptic plasticity (modifies learning rates)
 * - Attention and vigilance
 * - Memory consolidation
 * - Motivational states
 * 
 * The implementation follows biological models of neuromodulation systems
 * while providing a flexible computational framework.
 */
class Neuromodulator {
public:
    virtual ~Neuromodulator() = default;
    
    /**
     * @brief Get the name/identifier of this neuromodulator
     * @return String identifier (e.g., "DA", "ACh", "NE", "5-HT")
     */
    virtual const char* getName() const = 0;
    
    /**
     * @brief Get the current concentration or level of the neuromodulator
     * @return Current level (typically 0.0 to 1.0 normalized)
     */
    virtual float getLevel() const = 0;
    
    /**
     * @brief Set the current level of the neuromodulator
     * @param level Desired level (should be clamped within valid range)
     */
    virtual void setLevel(float level) = 0;
    
    /**
     * @brief Calculate the plasticity factor based on current neuromodulator level
     * 
     * Neuromodulators affect synaptic plasticity. Higher levels typically
     * increase learning rates, though specific effects vary by neuromodulator.
     * 
     * @return Plasticity factor (typically 0.0 to 2.0, where 1.0 is baseline)
     */
    virtual float getPlasticityFactor() const = 0;
    
    /**
     * @brief Update the neuromodulator state over time
     * 
     * This method implements the temporal dynamics of the neuromodulator,
     * including decay towards baseline, response to stimuli, etc.
     * 
     * @param dt Timestep duration in seconds
     */
    virtual void update(TimestepDuration dt) = 0;
    
protected:
    Neuromodulator() = default;
};

/**
 * @class Dopamine
 * @brief Dopamine neuromodulator system - reward and reinforcement learning
 * 
 * Dopamine is primarily associated with reward prediction, reinforcement learning,
 * motivation, and motor control. It signals the difference between expected and
 * actual outcomes (reward prediction error).
 * 
 * Biological basis:
 * - Ventral tegmental area (VTA) to nucleus accumbens and prefrontal cortex
 * - Phasic bursts for unexpected rewards
 * - Tonic firing rate reflects motivation level
 * - Long-term potentiation/depression of synapses
 * 
 * Computational model:
 * - Level represents prediction error signal
 * - Plasticity factor modulates learning rate
 * - Integration with reward system
 */
class Dopamine : public Neuromodulator {
public:
    Dopamine();
    ~Dopamine() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    /**
     * @brief Signal a reward event
     * 
     * Called when a reward is actually received. Causes a phasic burst
     * of dopamine activity representing positive prediction error.
     * 
     * @param reward Magnitude of reward (typically 0.0 to 1.0)
     */
    void signalReward(float reward);
    
    /**
     * @brief Signal a reward prediction error
     * 
     * Called when the predicted outcome differs from actual outcome.
     * Positive error = better than expected, negative = worse than expected.
     * 
     * @param error Prediction error (typically -1.0 to 1.0)
     */
    void signalRewardPredictionError(float error);
    
private:
    struct Impl;
    Impl* pImpl;
};

/**
 * @class Acetylcholine
 * @brief Acetylcholine neuromodulator - attention and memory
 * 
 * Acetylcholine is crucial for attention, arousal, working memory,
 * and memory consolidation. It modulates the strength of sensory
 * and cortical processing based on behavioral relevance.
 * 
 * Biological basis:
 * - Basal forebrain to cortical and hippocampal targets
 * - Increases cortical excitability
 * - Enhances signal-to-noise ratio
 * - Facilitates memory encoding
 */
class Acetylcholine : public Neuromodulator {
public:
    const char* getName() const override { return "ACh"; }
    float getLevel() const override { return 0.0f; }
    void setLevel(float level) override {}
    float getPlasticityFactor() const override { return 1.0f; }
    void update(TimestepDuration dt) override {}
};

/**
 * @class Norepinephrine
 * @brief Norepinephrine neuromodulator - arousal and vigilance
 * 
 * Norepinephrine is associated with alertness, attention, arousal,
 * fight-or-flight response, and memory formation. It prepares the
 * brain for action and novel stimuli.
 * 
 * Biological basis:
 * - Locus coeruleus to widespread brain targets
 * - Response to novel and salient stimuli
 * - Increases arousal and vigilance
 * - Modulates stress response
 */
class Norepinephrine : public Neuromodulator {
public:
    const char* getName() const override { return "NE"; }
    float getLevel() const override { return 0.0f; }
    void setLevel(float level) override {}
    float getPlasticityFactor() const override { return 1.0f; }
    void update(TimestepDuration dt) override {}
};

/**
 * @class Serotonin
 * @brief Serotonin neuromodulator - mood and impulsivity
 * 
 * Serotonin regulates mood, sleep, appetite, impulsivity, aggression,
 * and various cognitive functions. It has complex effects on behavior
 * and emotional processing.
 * 
 * Biological basis:
 * - Raphe nuclei to widespread targets
 * - Response to environmental rewards and punishments
 * - Modulates mood and emotional state
 * - Affects decision-making and impulsivity
 */
class Serotonin : public Neuromodulator {
public:
    const char* getName() const override { return "5-HT"; }
    float getLevel() const override { return 0.0f; }
    void setLevel(float level) override {}
    float getPlasticityFactor() const override { return 1.0f; }
    void update(TimestepDuration dt) override {}
};

} // namespace nlm
