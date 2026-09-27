// Include header and necessary libraries
#include "Neuromodulator.hpp"
#include <algorithm>

namespace nlm {

/**
 * @struct Dopamine::Impl
 * @brief Private implementation details for Dopamine neuromodulator
 * 
 * This struct contains the internal state and dynamics of the dopamine system.
 * It encapsulates the biological parameters and computational state.
 */
struct Dopamine::Impl {
    // Current dopamine concentration (0.0 to 1.0 normalized)
    float level;
    
    // Baseline firing rate when no reward is present
    float baseline;
    
    // Maximum dopamine level (peak for reward bursts)
    float peak;
    
    // Natural decay rate back to baseline (1.0/time_unit)
    float decayRate;
    
    // Release rate when dopamine is released (change per unit reward)
    float releaseRate;
    
    // Constructor initializes default parameters based on biological observations
    Impl() : level(0.0f), baseline(0.0f), peak(1.0f), decayRate(0.1f), releaseRate(1.0f) {}
};

/**
 * @brief Constructor for Dopamine neuromodulator
 * 
 * Initializes the internal implementation with default biological parameters.
 * The dopamine system starts at baseline with no immediate stimulation.
 */
Dopamine::Dopamine() : pImpl(new Impl) {}

/**
 * @brief Destructor
 * 
 * Cleans up the internal implementation.
 */
Dopamine::~Dopamine() = default;

/**
 * @brief Get the neuromodulator name
 * 
 * @return String identifier "DA" (Dopamine)
 */
const char* Dopamine::getName() const {
    return "DA";
}

/**
 * @brief Get current dopamine level
 * 
 * @return Current concentration (0.0 to 1.0 normalized)
 */
float Dopamine::getLevel() const {
    return pImpl->level;
}

/**
 * @brief Set dopamine level with bounds checking
 * 
 * Ensures the dopamine level stays within biologically plausible bounds
 * (0.0 to 1.0). This prevents invalid states that could destabilize the system.
 * 
 * @param level Desired concentration (will be clamped)
 */
void Dopamine::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, 1.0f);
}

/**
 * @brief Calculate dopamine-modulated plasticity factor
 * 
 * This represents how dopamine affects synaptic plasticity. The implementation
 * is currently simplified as a placeholder - real dopamine effects are more complex.
 * 
 * Biological rationale: Dopamine enhances plasticity when prediction errors occur,
 * helping the brain learn from surprising outcomes.
 * 
 * @return Plasticity factor between 0.5 and 1.5 (baseline at 1.0)
 */
float Dopamine::getPlasticityFactor() const {
    // TODO PHASE 2: Implement real dopamine-modulated plasticity factor
    // PLACEHOLDER: Higher dopamine increases plasticity
    return 0.5f + 0.5f * pImpl->level;
}

/**
 * @brief Update dopamine state over time
 * 
 * Implements the natural decay dynamics of dopamine levels back to baseline,
 * modeling the pharmacokinetics of dopamine clearance from synaptic cleft.
 * 
 * Biological basis: After a burst of dopamine activity, levels naturally decay
 * towards baseline over time.
 * 
 * @param dt Timestep duration in seconds
 */
void Dopamine::update(TimestepDuration dt) {
    // TODO PHASE 2: Implement real dopamine dynamics
    // PLACEHOLDER: Decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

/**
 * @brief Signal a reward event
 * 
 * This method is called when an actual reward is received. It simulates the
 * phasic burst of dopamine activity that occurs in response to unexpected rewards.
 * This represents a positive prediction error (reward > expected).
 * 
 * Biological basis: Ventral tegmental area (VTA) releases dopamine in response
 * to rewarding stimuli, signaling that the current state is beneficial.
 * 
 * @param reward Magnitude of reward (typically 0.0 to 1.0)
 */
void Dopamine::signalReward(float reward) {
    // TODO PHASE 2: Implement real reward signaling
    // PLACEHOLDER: Burst of dopamine on reward
    pImpl->level = std::min(pImpl->peak, pImpl->level + reward * pImpl->releaseRate);
}

/**
 * @brief Signal a reward prediction error
 * 
 * This method handles cases where the expected outcome differs from actual outcome.
 * Positive error = better than expected, negative = worse than expected.
 * 
 * Biological basis: Dopamine neurons respond not just to rewards, but to the
 * difference between expected and actual outcomes, driving learning.
 * 
 * @param error Prediction error (typically -1.0 to 1.0)
 */
void Dopamine::signalRewardPredictionError(float error) {
    // TODO PHASE 2: Implement reward prediction error signaling
    // PLACEHOLDER: Dopamine responds to prediction error
    pImpl->level = std::max(0.0f, pImpl->level + error * pImpl->releaseRate);
}

} // namespace nlm
