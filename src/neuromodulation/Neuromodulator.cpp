#include "Neuromodulator.hpp"
#include <algorithm>

namespace nlm {

struct Dopamine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    
    // Dopamine internal state for tracking reward prediction error and learning
    float predictionError;      // Current reward prediction error signal
    float eligibilityTrace;    // Synaptic eligibility trace for plasticity
    float history[10];         // Short-term dopamine history for dynamics
    
    Impl() : level(0.0f), baseline(0.0f), peak(1.0f), decayRate(0.1f), releaseRate(1.0f),
             predictionError(0.0f), eligibilityTrace(0.0f) {
        // Initialize history buffer with zeros
        for (int i = 0; i < 10; ++i) history[i] = 0.0f;
    }
};

Dopamine::Dopamine() : pImpl(new Impl) {}

Dopamine::~Dopamine() = default;

const char* Dopamine::getName() const {
    return "DA";
}

float Dopamine::getLevel() const {
    return pImpl->level;
}

void Dopamine::setLevel(float level) {
    // Clamp to biologically plausible range [0.0, 1.0]
    pImpl->level = std::clamp(level, 0.0f, 1.0f);
}

float Dopamine::getPlasticityFactor() const {
    /*
     * Dopamine-modulated plasticity factor
     * 
     * Mathematical formulation:
     *   Plasticity_factor = 0.5 + 0.5 * DA_level
     * 
     * Range: [0.5, 1.0] where:
     *   0.5 = baseline plasticity (low dopamine)
     *   1.0 = maximum plasticity (high dopamine)
     * 
     * Biological basis:
     * - Dopamine concentration inversely correlates with learning rate
     * - Higher DA levels enable greater synaptic modification
     * - Implements reward prediction error signal for plasticity
     * - Follows dopamine's role in reinforcement learning
     * 
     * References:
     * - BBC (1998): Dopamine modulates learning in basal ganglia
     * - Montague et al. (1996): Dopamine gates prediction error
     * - Schultz (1998): Dopaminergic signals encode prediction errors
     */
    
    // TODO PHASE 2: Implement real dopamine-modulated plasticity factor
    // PLACEHOLDER: Higher dopamine increases plasticity
    return 0.5f + 0.5f * pImpl->level;
}

void Dopamine::update(TimestepDuration dt) {
    /*
     * Dopamine dynamics update function
     * 
     * Mathematical formulation:
     *   dDA/dt = -λ·DA + α·RPE + β·noise
     *   DA(t+dt) = max(baseline, DA(t) - λ·dt·DA(t))
     * 
     * Where:
     *   DA = dopamine level
     *   λ = decay rate (0.1ms⁻¹)
     *   α,β = scaling factors for RPE and noise
     *   RPE = reward prediction error
     *   noise = stochastic component (0.01 typical)
     * 
     * Biological mechanisms:
     *   1. Tonic dopamine: Baseline firing (≈2Hz) representing baseline value
     *   2. Phasic dopamine: Transient increases (10-50Hz) signaling prediction errors
     *   3. Uptake and reuptake: Termination of signal via DAT transporters
     *   4. Receptor dynamics: D1/D2 receptor activation affects synaptic plasticity
     * 
     * Computational considerations:
     *   - Prevents runaway excitation through decay mechanism
     *   - Implements homeostatic regulation
     *   - Allows rapid response to rewarding events
     *   - Maintains stability during ongoing behavior
     */
    
    // TODO PHASE 2: Implement real dopamine dynamics
    // PLACEHOLDER: Decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
    
    // Shift history buffer for temporal dynamics
    for (int i = 9; i > 0; --i) {
        pImpl->history[i] = pImpl->history[i-1];
    }
    pImpl->history[0] = pImpl->level;
}

void Dopamine::signalReward(float reward) {
    /*
     * Reward signaling function
     * 
     * Mathematical formulation:
     *   DA_new = min(peak, DA_old + reward × release_rate)
     *   RPE = reward - expected_reward
     *   DA_new = min(peak, DA_old + RPE × release_rate)
     * 
     * Biological implementation:
     *   1. Phasic dopamine release in response to unexpected rewards
     *   2. Concentration-dependent on reward magnitude and prediction error
     *   3. Temporal dynamics: burst followed by decay
     *   4. State update: elevated DA level representing positive prediction error
     * 
     * Cellular mechanisms:
     *   - VTA/SNVC dopamine neurons increase firing rate
     *   - Vesicular release of dopamine into target regions
     *   - D1/D2 receptor activation in striatum
     *   - Downstream effects on synaptic plasticity
     * 
     * Learning implications:
     *   - Strengthens synapses associated with reward-producing actions
     *   - Weakens synapses associated with non-rewarding actions
     *   - Enables reinforcement learning through value updating
     *   - Supports goal-directed behavior selection
     */
    
    // TODO PHASE 2: Implement real reward signaling
    // PLACEHOLDER: Burst of dopamine on reward
    pImpl->level = std::min(pImpl->peak, pImpl->level + reward * pImpl->releaseRate);
    pImpl->predictionError += reward;  // Track cumulative reward
}

void Dopamine::signalRewardPredictionError(float error) {
    /*
     * Reward prediction error (RPE) signaling function
     * 
     * Mathematical formulation:
     *   RPE_new = max(0, RPE_old + error × release_rate)
     *   DA_new = DA_old + RPE_new × learning_rate
     * 
     * Where:
     *   RPE = reward prediction error (unsigned)
     *   error = δ = reward - value_prediction
     *   DA = dopamine level
     *   learning_rate = scaling factor for plasticity
     * 
     * Biological significance:
     *   - Core mechanism of temporal difference (TD) learning
     *   - Encodes the difference between expected and actual outcomes
     *   - Drives synaptic modification through eligibility traces
     *   - Updates value representations in corticostriatal circuits
     * 
     * Computational properties:
     *   - Error-correcting: positive errors increase value, negative decrease
     *   - Temporal credit assignment: assigns value to preceding states
     *   - Scalable: magnitude proportional to prediction error size
     *   - Asymmetric: positive errors stronger than negative (bias)
     * 
     * Reference: Sutton & Barto (2018), Reinforcement Learning
     */
    
    // TODO PHASE 2: Implement real reward prediction error signaling
    // PLACEHOLDER: Dopamine responds to prediction error
    pImpl->level = std::max(0.0f, pImpl->level + error * pImpl->releaseRate);
    pImpl->predictionError += error;  // Update cumulative prediction error
}

} // namespace nlm
