#pragma once

#include "../core/Types/Types.hpp"
#include <string>
#include <vector>

namespace nlm {

/**
 * @class Neuromodulator
 * @brief Abstract base class for neuromodulatory signals in the NLM brain
 * 
 * Neuromodulators are chemical messengers that influence neural processing across
the brain. In NLM, they implement reward prediction error, curiosity, novelty,
and other forms of motivational signaling that shape learning and behavior.
 * 
 * Neuromodulation in NLM serves several key functions:
 * - Modulate neural excitability and firing thresholds
 * - Influence synaptic plasticity rules (STDP, Hebbian)
 * - Control exploration vs. exploitation balance
 * - Integrate learning signals with motor output
 * - Implement developmental modulation of learning rates
 * 
 * @note This is Phase 2+: Real neuromodulation implementation
 *       (was a placeholder in earlier phases)
 */

class Neuromodulator {
public:
    /**
     * @brief Virtual destructor
     */
    virtual ~Neuromodulator() = default;
    
    /**
     * @brief Get the name of the neuromodulator
     * 
     * @return const char* Human-readable name of the neuromodulator
     */
    virtual const char* getName() const = 0;
    
    /**
     * @brief Get the current concentration/level of the neuromodulator
     * 
     * @return float Current neuromodulator concentration (typically 0.0-1.0)
     */
    virtual float getLevel() const = 0;
    
    /**
     * @brief Set the neuromodulator concentration/level
     * 
     * @param level New neuromodulator concentration
     */
    virtual void setLevel(float level) = 0;
    
    /**
     * @brief Get the plasticity modulation factor
     * 
     * Returns a factor that multiplies standard plasticity learning rates.
     * A value of 1.0 indicates no modulation, >1.0 increases plasticity,
     * <1.0 decreases plasticity.
     * 
     * @return float Plasticity modulation factor
     */
    virtual float getPlasticityFactor() const = 0;
    
    /**
     * @brief Update neuromodulator state for the current timestep
     * 
     * This method should implement the time-dependent dynamics of the
     * neuromodulator, including decay, adaptation, and integration with
     * environmental inputs.
     * 
     * @param dt Time duration of the timestep
     */
    virtual void update(TimestepDuration dt) = 0;
    
protected:
    /**
     * @brief Default constructor
     */
    Neuromodulator() = default;
};

/**
 * @class Dopamine
 * @brief Reward prediction error signal for reinforcement learning
 * 
 * Dopamine implements the reward prediction error (RPE) signal that is
central to reinforcement learning in biological and artificial brains.
In NLM, dopamine:
 * - Encodes the difference between predicted and actual rewards
 * - Modulates synaptic plasticity based on reward prediction errors
 * - Influences neural excitability and firing patterns
 * - Drives learning towards maximizing expected rewards
 * - Integrates with curiosity and novelty detection systems
 * 
 * The dopamine signal follows the temporal difference learning framework,
 * providing both immediate reward signals and long-term prediction updates.
 */

class Dopamine : public Neuromodulator {
public:
    /**
     * @brief Construct a new Dopamine object
     */
    Dopamine();
    
    /**
     * @brief Destroy the Dopamine object
     */
    ~Dopamine() override;
    
    /**
     * @brief Get the name of this neuromodulator
     * 
     * @return const char* Always returns "Dopamine"
     */
    const char* getName() const override;
    
    /**
     * @brief Get the current dopamine level
     * 
     * @return float Current dopamine concentration
     */
    float getLevel() const override;
    
    /**
     * @brief Set dopamine level
     * 
     * @param level New dopamine concentration
     */
    void setLevel(float level) override;
    
    /**
     * @brief Get the dopamine plasticity modulation factor
     * 
     * Returns a factor that scales plasticity learning rates based on
     * dopamine level. Positive dopamine increases plasticity, negative
     * dopamine decreases it.
     * 
     * @return float Plasticity modulation factor based on dopamine level
     */
    float getPlasticityFactor() const override;
    
    /**
     * @brief Update dopamine state based on reward prediction error
     * 
     * Updates dopamine level based on prediction error and environmental
     * context. Includes decay mechanisms to prevent runaway excitation.
     * 
     * @param dt Time duration of the timestep
     */
    void update(TimestepDuration dt) override;
    
    /**
     * @brief Signal a reward event
     * 
     * Processes an actual reward received from the environment, updating
     * internal prediction and generating appropriate neuromodulatory response.
     * 
     * @param reward Magnitude of the received reward
     */
    void signalReward(float reward);
    
    /**
     * @brief Signal a reward prediction error
     * 
     * Processes a prediction error between expected and actual rewards,
     * updating learning weights and neuromodulatory state.
     * 
     * @param error Prediction error (actual - predicted reward)
     */
    void signalRewardPredictionError(float error);
    
    /**
     * @brief Get the current prediction error
     * 
     * @return float Current reward prediction error
     */
    float getPredictionError() const;
    
    /**
     * @brief Reset dopamine state to initial conditions
     */
    void reset();
    
private:
    struct Impl;
    Impl* pImpl;
};

/**
 * @class Acetylcholine
 * @brief Attention and memory consolidation modulator
 * 
 * Acetylcholine (ACh) modulates attention, working memory capacity,
 * and memory consolidation processes. In NLM, it:
 * - Enhances neural excitability during attention-demanding tasks
 * - Increases working memory capacity for complex processing
 * - Promotes memory consolidation during rest or sleep
 * - Facilitates learning of novel stimuli
 * - Modulates the balance between exploration and exploitation
 */

class Acetylcholine : public Neuromodulator {
public:
    /**
     * @brief Get the name of this neuromodulator
     * 
     * @return const char* Always returns "Acetylcholine" (ACh)
     */
    const char* getName() const override { return "ACh"; }
    
    /**
     * @brief Get the current acetylcholine level
     * 
     * @return float Current acetylcholine concentration
     */
    float getLevel() const override;
    
    /**
     * @brief Set acetylcholine level
     * 
     * @param level New acetylcholine concentration
     */
    void setLevel(float level) override;
    
    /**
     * @brief Get the acetylcholine plasticity modulation factor
     * 
     * @return float Plasticity modulation factor
     */
    float getPlasticityFactor() const override;
    
    /**
     * @brief Update acetylcholine state
     * 
     * @param dt Time duration of the timestep
     */
    void update(TimestepDuration dt) override;
    
    /**
     * @brief Signal attention shift to a new stimulus
     * 
     * @param novelty Amount of novelty in the stimulus (0.0-1.0)
     */
    void signalAttentionShift(float novelty);
    
    /**
     * @brief Signal memory encoding event
     * 
     * @param memory_strength Strength of memory encoding (0.0-1.0)
     */
    void signalMemoryEncoding(float memory_strength);
};

/**
 * @class Norepinephrine
 * @brief Arousal and vigilance modulator
 * 
 * Norepinephrine (NE) regulates arousal, vigilance, and stress responses.
 * In NLM, it:
 * - Increases neural gain during high-arousal states
 * - Enhances signal-to-noise ratio in neural processing
 * - Modulates reactive vs. proactive control
 * - Influences sustained attention and focus
 * - Responds to novel and unexpected stimuli
 */

class Norepinephrine : public Neuromodulator {
public:
    /**
     * @brief Get the name of this neuromodulator
     * 
     * @return const char* Always returns "Norepinephrine" (NE)
     */
    const char* getName() const override { return "NE"; }
    
    /**
     * @brief Get the current norepinephrine level
     * 
     * @return float Current norepinephrine concentration
     */
    float getLevel() const override;
    
    /**
     * @brief Set norepinephrine level
     * 
     * @param level New norepinephrine concentration
     */
    void setLevel(float level) override;
    
    /**
     * @brief Get the norepinephrine plasticity modulation factor
     * 
     * @return float Plasticity modulation factor
     */
    float getPlasticityFactor() const override;
    
    /**
     * @brief Update norepinephrine state
     * 
     * @param dt Time duration of the timestep
     */
    void update(TimestepDuration dt) override;
    
    /**
     * @brief Signal increased arousal
     * 
     * @param arousal_level Level of arousal (0.0-1.0)
     */
    void signalArousal(float arousal_level);
    
    /**
     * @brief Signal vigilance state change
     * 
     * @param vigilance_level Vigilance level (0.0-1.0)
     */
    void signalVigilanceChange(float vigilance_level);
};

/**
 * @class Serotonin
 * @brief Mood, impulsivity, and social behavior modulator
 * 
 * Serotonin (5-HT) regulates mood, impulsivity, and social behavior.
 * In NLM, it:
 * - Modulates reward sensitivity and pleasure
 * - Controls impulsivity and decision-making
 * - Influences social behavior and bonding
 * - Regulates sleep and wake cycles
 * - Affects appetite and energy balance
 */

class Serotonin : public Neuromodulator {
public:
    /**
     * @brief Get the name of this neuromodulator
     * 
     * @return const char* Always returns "Serotonin" (5-HT)
     */
    const char* getName() const override { return "5-HT"; }
    
    /**
     * @brief Get the current serotonin level
     * 
     * @return float Current serotonin concentration
     */
    float getLevel() const override;
    
    /**
     * @brief Set serotonin level
     * 
     * @param level New serotonin concentration
     */
    void setLevel(float level) override;
    
    /**
     * @brief Get the serotonin plasticity modulation factor
     * 
     * @return float Plasticity modulation factor
     */
    float getPlasticityFactor() const override;
    
    /**
     * @brief Update serotonin state
     * 
     * @param dt Time duration of the timestep
     */
    void update(TimestepDuration dt) override;
    
    /**
     * @brief Signal mood change
     * 
     * @param mood_level New mood level (-1.0 to 1.0)
     */
    void signalMoodChange(float mood_level);
    
    /**
     * @brief Signal impulsivity change
     * 
     * @param impulsivity_level New impulsivity level (0.0-1.0)
     */
    void signalImpulsivityChange(float impulsivity_level);
    
    /**
     * @brief Signal social behavior change
     * 
     * @param social_drive New social drive level (0.0-1.0)
     */
    void signalSocialDriveChange(float social_drive);
};

} // namespace nlm

