#pragma once

#include "../core/Types/Types.hpp"

namespace nlm {

/**
 * @brief Dopamine neuromodulator implementation
 * 
 * This header file declares the Dopamine class, which implements the
 * reward and reinforcement learning signal in the NLM system. The Dopamine
 * neuromodulator is essential for:
 * 
 * 1. **Reward Processing**: Detecting and responding to reward events
 * 2. **Motivation**: Generating approach behavior towards rewarding stimuli
 * 3. **Learning**: Modulating synaptic plasticity based on reward prediction errors
 * 4. **Attention**: Focusing neural resources on rewarding activities
 * 
 * The Dopamine class follows the Neuromodulator abstract base class interface,
 * ensuring consistency across all neuromodulatory systems in the NLM.
 * 
 * Dopamine dynamics in the NLM:
 * 
 * **Phasic Responses**: Short, high-amplitude bursts triggered by:
 * - Unexpected rewards (positive prediction errors)
 * - Novel events and novelty detection
 * - Intrinsic motivation and curiosity
 * 
 * **Tonic Levels**: Persistent background activity representing:
 * - Overall motivation and drive
 * - Current behavioral relevance of stimuli
 * - Integration with other neuromodulators
 * 
 * **Key Functions**:
 * 
 * 1. **signalReward()**: Responds to actual reward events, typically causing
 *    phasic dopamine bursts that reinforce the preceding behavior
 * 
 * 2. **signalRewardPredictionError()**: Responds to prediction errors, where
 *    dopamine levels encode the difference between expected and actual rewards
 * 
 * 3. **getPlasticityFactor()**: Modulates the strength of synaptic plasticity
 *    rules based on current dopamine levels
 * 
 * 4. **update()**: Implements dopamine dynamics including decay towards
 *    baseline levels and integration with other neural signals
 * 
 * The Dopamine system integrates with other neural systems:
 * 
 * - **Memory Systems**: Reward-based memory consolidation
 * - **Motor Systems**: Reward-driven action selection
 * - **Cognition Systems**: Reward-guided decision making
 * - **Development System**: Age-dependent dopamine changes
 * 
 * Typical dopamine applications in the NLM:
 * - Reinforcement learning and reward-based behavior
 * - Exploration vs. exploitation trade-offs
 * - Motivation and goal-directed behavior
 * - Behavioral adaptation and learning
 */
class Dopamine {
public:
    /**
     * @brief Default constructor
     * 
     * Initializes the Dopamine neuromodulator with default parameters:
     * - Level: 0.0 (no initial modulation)
     * - Baseline: 0.0 (no tonic activity)
     * - Peak: 1.0 (maximum possible response)
     * - Decay rate: 0.1 (moderate decay)
     * - Release rate: 1.0 (strong response to rewards)
     */
    Dopamine();
    
    /**
     * @brief Virtual destructor for proper polymorphic deletion
     */
    virtual ~Dopamine();
    
    /**
     * @brief Get the neuromodulator name
     * 
     * Returns the standard abbreviation for dopamine ("DA") used consistently
     * throughout the NLM system for this neuromodulator.
     * 
     * @return String identifier "DA"
     */
    const char* getName() const;
    
    /**
     * @brief Get the current dopamine level
     * 
     * The dopamine level represents the current modulation state of the neural
     * system. This value ranges from 0.0 (no modulation) to 1.0 (maximum modulation)
     * and can change dynamically based on behavioral and environmental inputs.
     * 
     * @return Current dopamine level (0.0 to 1.0)
     */
    float getLevel() const;
    
    /**
     * @brief Set the dopamine level with automatic bounds checking
     * 
     * Ensures that dopamine levels remain within biologically plausible bounds
     * (typically 0.0 to 1.0). This prevents pathological states where dopamine
     * levels become too high or too low.
     * 
     * @param level New dopamine level (automatically clamped to valid range)
     */
    void setLevel(float level);
    
    /**
     * @brief Get the plasticity modulation factor
     * 
     * This method returns how much the current dopamine level affects synaptic
     * plasticity. Higher dopamine levels typically increase plasticity for
     * rewarded behaviors and can also decrease plasticity for non-rewarded behaviors
     * (possibly through opponent processes).
     * 
     * The plasticity factor is a multiplier that gets applied to synaptic
     * plasticity rules (like STDP, Hebbian, etc.).
     * 
     * @return Plasticity modulation factor (typically 0.5 to 1.0)
     * @note Implementation uses: 0.5f + 0.5f * level (placeholder)
     */
    float getPlasticityFactor() const;
    
    /**
     * @brief Update dopamine state over time
     * 
     * Implements the temporal dynamics of dopamine including:
     * 
     * **Decay**: Natural tendency to return to baseline levels
     * - Models the reuptake and clearance of dopamine
     * - Prevents unbounded accumulation of dopamine
     * 
     * **Baseline**: Homeostatic set point
     * - Represents tonic dopamine activity
     * - Varies with motivation and arousal
     * 
     * **Release**: Responses to events and rewards
     * - Burst responses to unexpected rewards
     * - Phasic responses to novel events
     * - Integration with other neuromodulators
     * 
     * @param dt Time step duration (typically in milliseconds)
     */
    void update(TimestepDuration dt);
    
    /**
     * @brief Signal a reward event to the dopamine system
     * 
     * This method models the immediate phasic response to rewards, typically
     * causing a burst of dopamine activity. In biological systems, this response
     * is often called "reward prediction error signaling" when compared to
     * expected rewards, but this method responds to actual rewards.
     * 
     * The dopamine response follows these dynamics:
     * - Strength proportional to reward magnitude
     * - Limited by peak response (prevents saturation)
     * - Combined with current baseline level
     * 
     * @param reward Magnitude of the reward event (0.0 = no reward, 1.0 = maximum)
     * @note Implementation uses: min(peak, current + reward * releaseRate)
     */
    void signalReward(float reward);
    
    /**
     * @brief Signal a reward prediction error to the dopamine system
     * 
     * This method models the predictive coding aspect of dopamine, where dopamine
     * responds to the difference between expected and actual rewards. This is
     * the classic "reward prediction error" signal in reinforcement learning
     * theory.
     * 
     * The response characteristics:
     * - Positive error (better than expected): Increases dopamine
     * - Negative error (worse than expected): Decreases dopamine
     * - Response magnitude proportional to error magnitude
     * - Minimum dopamine level prevented (no negative values)
     * 
     * @param error Prediction error (positive for better-than-expected, 
     *               negative for worse-than-expected outcomes)
     * @note Implementation uses: max(0.0f, current + error * releaseRate)
     */
    void signalRewardPredictionError(float error);
    
private:
    /**
     * @brief Internal implementation structure (Pimpl idiom)
     * 
     * This structure holds all the implementation details of the Dopamine class,
     * including state variables and private methods. This separation reduces
     * compilation dependencies and improves encapsulation.
     * 
     * State variables include:
     * - level: Current dopamine concentration
     * - baseline: Homeostatic set point
     * - peak: Maximum possible response
     * - decayRate: Natural decay rate
     * - releaseRate: Response magnitude to events
     */
    struct Impl;
    
    /** Pointer to internal implementation */
    Impl* pImpl;
};

} // namespace nlm