#pragma once

#include "../core/Types/Types.hpp"
#include <string>
#include <vector>

namespace nlm {

/**
 * @file Neuromodulator.hpp
 * 
 * Neuromodulation system for NLM
 * 
 * This file provides the neuromodulation architecture for the NLM system,
 * implementing multiple neuromodulatory signals that regulate neural
 * plasticity, learning, and behavior. Neuromodulation is essential for:
 * - Regulating synaptic strength and plasticity
 * - Controlling learning rates and timing
 * - Modulating attention and arousal
 * - Implementing reward and prediction error signaling
 * - Enabling behavioral flexibility and adaptation
 * 
 * The neuromodulation system in NLM consists of:
 * - Neuromodulator: Abstract base class for all neuromodulators
 * - Dopamine: Reward and reinforcement learning signal
 * - Acetylcholine: Attention and memory consolidation
 * - Norepinephrine: Arousal and vigilance
 * - Serotonin: Mood, impulsivity, and social behavior
 * - Novelty: Novelty detection and response
 * - Curiosity: Exploration motivation
 * - PredictionError: Prediction error signal
 * 
 * Key features:
 * - Multiple neuromodulatory signals with distinct functions
 * - Integration with neural plasticity and learning
 * - Real-time modulation of neural activity
 * - Behavioral reinforcement and adaptation
 * - Cognitive and emotional regulation
 * - Learning rate and timing control
 * 
 * @note Critical for learning, motivation, and behavior
 * @note Used extensively throughout NLM neural systems
 * @note Essential for adaptive behavior and plasticity
 * 
 * @see Config.hpp - Configuration options for neuromodulation
 * @see Brain.hpp - Integration with brain architecture
 * @see DevelopmentSystem.hpp - Neuromodulation development
 * 
 * @author NLM Development Team
 * @date 2026
 * @version 1.0
 */

/**
 * @class Neuromodulator
 * 
 * Abstract base class for neuromodulatory signals.
 * 
 * This class defines the interface for all neuromodulatory signals in NLM.
 * Neuromodulators are chemical signals that modulate neural activity,
 * synaptic plasticity, and learning processes. They are essential for:
 * - Regulating neural excitability and responsiveness
 * - Controlling learning rates and plasticity
 * - Implementing reward and prediction error signaling
 * - Modulating attention and arousal
 * - Enabling behavioral flexibility and adaptation
 * 
 * Neuromodulators in NLM:
 * - **Dopamine**: Reward and reinforcement learning
 * - **Acetylcholine**: Attention and memory consolidation
 * - **Norepinephrine**: Arousal and vigilance
 * - **Serotonin**: Mood, impulsivity, and social behavior
 * - **Novelty**: Novelty detection and response
 * - **Curiosity**: Exploration motivation
 * - **PredictionError**: Prediction error signal
 * 
 * Key characteristics of neuromodulators in NLM:
 * - **Modulation**: Modulate neural activity and plasticity
 * - **Learning**: Essential for reinforcement learning
 * - **Adaptation**: Enable behavioral adaptation
 * - **Regulation**: Control arousal and attention
 * - **Integration**: Coordinate multiple neural systems
 * 
 * @note Neuromodulators are essential for NLM learning and behavior
 * @note Used extensively throughout all neural systems
 * @note Critical for adaptive and flexible behavior
 * 
 * @see Dopamine - Dopamine neuromodulator
 * @see Acetylcholine - Acetylcholine neuromodulator
 * @see Norepinephrine - Norepinephrine neuromodulator
 * @see Serotonin - Serotonin neuromodulator
 * @see Novelty - Novelty detection neuromodulator
 * @see Curiosity - Curiosity neuromodulator
 * 
 * @author NLM Development Team
 */
class Neuromodulator {
public:
    /**
     * @brief Virtual destructor
     * 
     * Ensures proper cleanup of neuromodulator resources.
     */
    virtual ~Neuromodulator() = default;
    
    /**
     * @brief Get neuromodulator name
     * 
     * Returns the name of this neuromodulator. Different neuromodulators
     * have distinct names reflecting their function and role in the
     * neural system.
     * 
     * @return C-style string with neuromodulator name
     * 
     * @post Return value is constant for neuromodulator lifetime
     * @complexity O(1)
     * 
     * @note Used for identification and debugging
     * @see getLevel() - For current neuromodulator level
     */
    virtual const char* getName() const = 0;
    
    /**
     * @brief Get current neuromodulator level
     * 
     * Returns the current concentration or level of the neuromodulator
     * signal. This reflects the current state of neuromodulatory activity
     * and is used by neural systems to modulate their behavior.
     * 
     * @return Current neuromodulator concentration/level
     * 
     * @post Return value fluctuates during neural computation
     * @complexity O(1)
     * 
     * @note Level changes dynamically during neural activity
     * @see setLevel() - For setting neuromodulator level
     */
    virtual float getLevel() const = 0;
    
    /**
     * @brief Set neuromodulator level
     * 
     * Sets the neuromodulator level, which affects neural processing
     * and plasticity. This is used for:
     * - Controlling neuromodulator activity
     * - Implementing neuromodulatory signals
     * - Regulating neural plasticity
     * - Modulating learning rates
     * 
     * @param level New neuromodulator level
     * 
     * @post Neuromodulator level updated
     * @post Neural processing modulated
     * @post Plasticity affected
     */
    virtual void setLevel(float level) = 0;
    
    /**
     * @brief Get neuromodulatory plasticity factor
     * 
     * Returns the current plasticity factor, which determines how strongly
     * this neuromodulator affects synaptic plasticity. This factor is used
     * by neural systems to scale their plasticity responses based on
     * neuromodulatory state.
     * 
     * @return Current plasticity factor (typically 0.0 to 2.0)
     * 
     * @post Return value fluctuates during neural computation
     * @complexity O(1)
     * 
     * @note Higher values increase plasticity sensitivity
     * @note Lower values decrease plasticity sensitivity
     * @see getLevel() - For neuromodulator level
     */
    virtual float getPlasticityFactor() const = 0;
    
    /**
     * @brief Update neuromodulator state
     * 
     * Updates the neuromodulator state based on current neural activity
     * and environmental conditions. This is essential for:
     * - Implementing dynamic neuromodulatory signals
     * - Responding to neural activity patterns
     * - Adjusting neuromodulatory levels
     * - Supporting real-time neuromodulation
     */
    virtual void update(TimestepDuration dt) = 0;
    
protected:
    /**
     * @brief Default constructor
     * 
     * Creates a new neuromodulator instance with default parameters.
     * The neuromodulator is initialized with default settings for
     * neuromodulatory activity.
     * 
     * @post Neuromodulator created with default settings
     * @post Ready for neuromodulatory activity
     */
    Neuromodulator() = default;
};

/**
 * @class Dopamine
 * 
 * Dopamine: Reward and reinforcement learning signal.
 * 
 * Dopamine is a key neuromodulator in NLM that implements reward and
 * reinforcement learning. It is essential for:
 * - Reward prediction and signaling
 * - Reinforcement learning and behavior shaping
 * - Incentive motivation and salience
 * - Learning rate modulation
 * - Prediction error signaling
 * 
 * Dopamine mechanisms in NLM:
 * - **Reward signaling**: Signals reward prediction errors
 * - **Reinforcement learning**: Guides behavior based on rewards
 * - **Incentive motivation**: Motivates goal-directed behavior
 * - **Learning rate**: Modulates learning speed and accuracy
 * - **Prediction error**: Computes prediction errors for learning
 * 
 * Key characteristics of dopamine in NLM:
 * - **Rewarding**: Signals when rewards are received
 * - **Motivating**: Drives goal-directed behavior
 * - **Learning**: Enables reinforcement learning
 * - **Timing**: Precisely times reward delivery
 * - **Adaptive**: Adjusts to environmental contingencies
 * 
 * @note Critical for reward-based learning and behavior
 * @note Used extensively in learning and decision-making
 * @note Essential for reinforcement learning
 * @see Neuromodulator - Base neuromodulator class
 * @see Novelty - For novelty detection
 * @see Curiosity - For exploration motivation
 * 
 * @author NLM Development Team
 */
class Dopamine : public Neuromodulator {
public:
    /**
     * @brief Default constructor
     * 
     * Creates a new dopamine neuromodulator instance with default parameters.
     * The dopamine neuromodulator is initialized with baseline levels
     * and ready for reward signaling and reinforcement learning.
     */
    Dopamine();
    
    /**
     * @brief Destructor
     * 
     * Cleans up dopamine neuromodulator resources and performs any necessary
     * cleanup operations. The dopamine neuromodulator is left in a valid but
     * non-operational state after destruction.
     */
    ~Dopamine() override;
    
    // Disable copying - PImpl idiom
    Dopamine(const Dopamine&) = delete;
    Dopamine& operator=(const Dopamine&) = delete;
    Dopamine(Dopamine&&) noexcept;
    Dopamine& operator=(Dopamine&&) noexcept;
    
    /**
     * @brief Get neuromodulator name
     * 
     * Returns the name of this neuromodulator ("Dopamine").
     * 
     * @return C-style string with neuromodulator name ("Dopamine")
     * 
     * @post Return value is constant for neuromodulator lifetime
     * @complexity O(1)
     */
    const char* getName() const override;
    
    /**
     * @brief Get current dopamine level
     * 
     * Returns the current dopamine level, which reflects reward prediction
     * errors and reward prediction signals. This is used by neural systems
     * to modulate their behavior based on reward information.
     * 
     * @return Current dopamine level
     * 
     * @post Return value fluctuates during neural computation
     * @complexity O(1)
     */
    float getLevel() const override;
    
    /**
     * @brief Set dopamine level
     * 
     * Sets the dopamine level, which affects neural processing and plasticity.
     * Dopamine level is increased by:
     * - Reward prediction errors (positive)
     * - Better-than-expected outcomes (positive)
     * - Novel or surprising events (positive)
     * - Increased when learning occurs
     * 
     * @param level New dopamine level
     * 
     * @post Dopamine level updated
     * @post Neural processing modulated
     * @post Plasticity affected
     */
    void setLevel(float level) override;
    
    /**
     * @brief Get dopamine plasticity factor
     * 
     * Returns the current dopamine plasticity factor, which determines how
     * strongly dopamine affects synaptic plasticity. This factor is higher
     * during reward prediction errors and learning.
     * 
     * @return Dopamine plasticity factor
     * 
     * @post Return value fluctuates with neural activity
     * @complexity O(1)
     * 
     * @note Higher during reward prediction errors
     * @note Increases learning speed
     * @see getLevel() - For dopamine level
     */
    float getPlasticityFactor() const override;
    
    /**
     * @brief Update dopamine state
     * 
     * Updates the dopamine state based on reward prediction errors and
     * environmental feedback. This implements:
     * - Reward prediction error computation
     * - Dopamine release modulation
     * - Learning rate adjustment
     * - Behavioral reinforcement
     */
    void update(TimestepDuration dt) override;
    
    /**
     * @brief Signal a reward
     * 
     * Signals a reward to the dopamine system, which increases dopamine levels
     * and modulates learning. This is used for:
     * - Reward-based learning
     * - Behavior reinforcement
     * - Learning rate adjustment
     * - Value learning
     */
    void signalReward(float reward);
    
    /**
     * @brief Signal a reward prediction error
     * 
     * Signals a reward prediction error to the dopamine system, which is
     * essential for reinforcement learning. This computes the difference
     * between expected and actual rewards.
     * 
     * @param error Reward prediction error
     * 
     * @post Reward prediction error recorded
     * @post Dopamine system updated
     */
    void signalRewardPredictionError(float error);
    
private:
    struct Impl;
    Impl* pImpl;
};

/**
 * @class Acetylcholine
 * 
 * Acetylcholine: Attention and memory consolidation.
 * 
 * Acetylcholine is a key neuromodulator in NLM that implements attention
 * and memory consolidation. It is essential for:
 * - Attention modulation and selection
 * - Memory encoding and consolidation
 * - Learning rate control
 * - Neuronal excitability modulation
 * - Cognitive function regulation
 * 
 * Acetylcholine mechanisms in NLM:
 * - **Attention**: Modulates attention and focus
 * - **Memory**: Consolidates memories and supports encoding
 * - **Learning**: Controls learning rates and plasticity
 * - **Excitability**: Modulates neuronal excitability
 * - **Cognition**: Regulates cognitive functions
 * 
 * Key characteristics of acetylcholine in NLM:
 * - **Attention**: Essential for attention and focus
 * - **Memory**: Critical for memory consolidation
 * - **Learning**: Controls learning rates
 * - **Excitability**: Modulates neural excitability
 * - **Cognition**: Regulates cognitive processing
 * 
 * @note Critical for attention and memory
 * @note Used extensively in cognitive function
 * @note Essential for learning and memory
 * @see Neuromodulator - Base neuromodulator class
 * @see Dopamine - For reward signaling
 * @see Novelty - For novelty detection
 * 
 * @author NLM Development Team
 */
class Acetylcholine : public Neuromodulator {
public:
    /**
     * @brief Get neuromodulator name
     * 
     * Returns the name of this neuromodulator ("ACh").
     * 
     * @return C-style string with neuromodulator name ("ACh")
     * 
     * @post Return value is constant for neuromodulator lifetime
     * @complexity O(1)
     */
    const char* getName() const override { return "ACh"; }
    
    /**
     * @brief Get current acetylcholine level
     * 
     * Returns the current acetylcholine level, which reflects attention
     * and memory consolidation. Acetylcholine level is increased during:
     * - Attention-demanding tasks
     * - Learning and memory encoding
     * - Novel stimulus exposure
     * - Cognitive challenge
     * 
     * @return Current acetylcholine level
     * 
     * @post Return value fluctuates during neural computation
     * @complexity O(1)
     */
    float getLevel() const override { return 0.0f; }
    
    /**
     * @brief Set acetylcholine level
     * 
     * Sets the acetylcholine level, which affects neural processing and
     * memory consolidation. This is a placeholder implementation that
     * currently returns immediately without side effects.
     * 
     * @param level New acetylcholine level
     * 
     * @post Acetylcholine level updated
     */
    void setLevel(float level) override {}
    
    /**
     * @brief Get acetylcholine plasticity factor
     * 
     * Returns the acetylcholine plasticity factor, which determines how
     * strongly acetylcholine affects synaptic plasticity. This factor is
     * higher during attention-demanding tasks and learning.
     * 
     * @return Acetylcholine plasticity factor
     * 
     * @post Return value is constant for neuromodulator lifetime
     * @complexity O(1)
     */
    float getPlasticityFactor() const override { return 1.0f; }
    
    /**
     * @brief Update acetylcholine state
     * 
     * Updates the acetylcholine state based on attention and learning
     * requirements. This is a placeholder implementation that currently
     * returns immediately without side effects.
     */
    void update(TimestepDuration dt) override {}
};

/**
 * @class Norepinephrine
 * 
 * Norepinephrine: Arousal and vigilance.
 * 
 * Norepinephrine is a key neuromodulator in NLM that implements arousal
 * and vigilance. It is essential for:
 * - Arousal and alertness
 * - Vigilance and attention
 * - Stress response
 * - Alertness modulation
 * - Wake state regulation
 * 
 * Norepinephrine mechanisms in NLM:
 * - **Arousal**: Increases overall neural excitability
 * - **Vigilance**: Maintains attention and focus
 * - **Stress**: Responds to stress and challenges
 * - **Alertness**: Modulates alertness levels
 * - **Wake**: Regulates wake state and sleep
 * 
 * Key characteristics of norepinephrine in NLM:
 * - **Arousal**: Essential for arousal and alertness
 * - **Vigilance**: Critical for sustained attention
 * - **Stress**: Responds to stress and challenges
 * - **Alertness**: Modulates alertness levels
 * - **Wake**: Regulates wake state and sleep
 * 
 * @note Critical for arousal and vigilance
 * @note Used extensively in attention and alertness
 * @note Essential for stress response
 * @see Neuromodulator - Base neuromodulator class
 * @see Dopamine - For reward signaling
 * @see Acetylcholine - For attention
 * 
 * @author NLM Development Team
 */
class Norepinephrine : public Neuromodulator {
public:
    /**
     * @brief Get neuromodulator name
     * 
     * Returns the name of this neuromodulator ("NE").
     * 
     * @return C-style string with neuromodulator name ("NE")
     * 
     * @post Return value is constant for neuromodulator lifetime
     * @complexity O(1)
     */
    const char* getName() const override { return "NE"; }
    
    /**
     * @brief Get current norepinephrine level
     * 
     * Returns the current norepinephrine level, which reflects arousal
     * and vigilance. Norepinephrine level is increased during:
     * - Stress and challenges
     * - High-arousal situations
     * - Alertness requirements
     * - Wake state regulation
     */
    float getLevel() const override { return 0.0f; }
    
    /**
     * @brief Set norepinephrine level
     * 
     * Sets the norepinephrine level, which affects neural arousal and
     * alertness. This is a placeholder implementation that currently
     * returns immediately without side effects.
     */
    void setLevel(float level) override {}
    
    /**
     * @brief Get norepinephrine plasticity factor
     * 
     * Returns the norepinephrine plasticity factor, which determines how
     * strongly norepinephrine affects synaptic plasticity. This factor
     * is higher during stress and arousal.
     * 
     * @return Norepinephrine plasticity factor
     * 
     * @post Return value is constant for neuromodulator lifetime
     * @complexity O(1)
     */
    float getPlasticityFactor() const override { return 1.0f; }
    
    /**
     * @brief Update norepinephrine state
     * 
     * Updates the norepinephrine state based on arousal and vigilance
     * requirements. This is a placeholder implementation that currently
     * returns immediately without side effects.
     */
    void update(TimestepDuration dt) override {}
};

/**
 * @class Serotonin
 * 
 * Serotonin: Mood, impulsivity, and social behavior.
 * 
 * Serotonin is a key neuromodulator in NLM that implements mood regulation,
 * impulsivity control, and social behavior. It is essential for:
 * - Mood regulation
 * - Impulsivity control
 * - Social behavior
 * - Decision-making
 * - Emotional regulation
 * 
 * Serotonin mechanisms in NLM:
 * - **Mood**: Regulates mood and emotional state
 * - **Impulsivity**: Controls impulse control and decision-making
 * - **Social**: Modulates social behavior
 * - **Decision**: Influences decision-making processes
 * - **Emotion**: Regulates emotional responses
 * 
 * Key characteristics of serotonin in NLM:
 * - **Mood**: Essential for mood regulation
 * - **Impulsivity**: Critical for impulse control
 * - **Social**: Important for social behavior
 * - **Decision**: Influences decision-making
 * - **Emotion**: Regulates emotional responses
 * 
 * @note Critical for mood and emotional regulation
 * @note Used extensively in decision-making
 * @note Essential for social behavior
 * @see Neuromodulator - Base neuromodulator class
 * @see Dopamine - For reward signaling
 * @see Acetylcholine - For attention
 * 
 * @author NLM Development Team
 */
class Serotonin : public Neuromodulator {
public:
    /**
     * @brief Get neuromodulator name
     * 
     * Returns the name of this neuromodulator ("5-HT").
     * 
     * @return C-style string with neuromodulator name ("5-HT")
     * 
     * @post Return value is constant for neuromodulator lifetime
     * @complexity O(1)
     */
    const char* getName() const override { return "5-HT"; }
    
    /**
     * @brief Get current serotonin level
     * 
     * Returns the current serotonin level, which reflects mood regulation
     * and emotional state. Serotonin level is increased during:
     * - Positive mood states
     * - Social interaction
     * - Emotional regulation
     * - Decision-making
     */
    float getLevel() const override { return 0.0f; }
    
    /**
     * @brief Set serotonin level
     * 
     * Sets the serotonin level, which affects mood regulation and
     * emotional state. This is a placeholder implementation that currently
     * returns immediately without side effects.
     */
    void setLevel(float level) override {}
    
    /**
     * @brief Get serotonin plasticity factor
     * 
     * Returns the serotonin plasticity factor, which determines how
     * strongly serotonin affects synaptic plasticity. This factor is
     * higher during mood regulation and emotional processing.
     * 
     * @return Serotonin plasticity factor
     * 
     * @post Return value is constant for neuromodulator lifetime
     * @complexity O(1)
     */
    float getPlasticityFactor() const override { return 1.0f; }
    
    /**
     * @brief Update serotonin state
     * 
     * Updates the serotonin state based on mood regulation and emotional
     * processing. This is a placeholder implementation that currently
     * returns immediately without side effects.
     */
    void update(TimestepDuration dt) override {}
};

} // namespace nlm
