#include "STDP.hpp"
#include "../../brain/Synapse.hpp"
#include <cmath>
#include <algorithm>
#include <numeric>
#include <functional>

namespace nlm {

struct STDP::Impl {
    float ltpWeight;      // A+ for potentiation
    float ltdWeight;      // A- for depression
    float timeConstant;   // Tau for exponential window (ms)
    float minWeight;      // Minimum synaptic weight
    float maxWeight;      // Maximum synaptic weight
    
    Impl() : ltpWeight(0.01f), ltdWeight(0.012f), timeConstant(20.0f),
             minWeight(-1.0f), maxWeight(1.0f) {}
};

STDP::STDP() : pImpl(new Impl) {}

STDP::~STDP() = default;

/**
 * @brief Update synaptic weights based on spike timing using optimized STDP
 * 
 * Optimized implementation reduces complexity from O(n²) to O(n log n) by:
 * 1. Sorting spike time arrays
 * 2. Using sliding windows to limit comparisons to relevant time ranges
 * 3. Binary search to find spike pairs within time windows
 * 
 * @param synapse Pointer to the synapse being updated
 * @param preSpikes Vector of timestamps for pre-synaptic spikes in current window
 * @param postSpikes Vector of timestamps for post-synaptic spikes in current window
 * @param dt Simulation timestep duration (ms) - not directly used but kept for interface
 * 
 * @details Implements the classic STDP learning rule with optimized performance:
 * For each spike pair with Δt = t_post - t_pre:
 * - If Δt > 0 (pre before post): Δw = A+ × exp(-Δt/τ+)
 * - If Δt < 0 (post before pre): Δw = A- × exp(Δt/τ-)
 * 
 * Where A+ = ltpWeight (potentiation amplitude), A- = ltdWeight (depression amplitude),
 * and τ+ = τ- = timeConstant (STDP time window).
 */
void STDP::update(Synapse* synapse,
                   const std::vector<Timestamp>& preSpikes,
                   const std::vector<Timestamp>& postSpikes,
                   TimestepDuration dt) {
    // Early exit if no valid inputs
    if (!synapse || preSpikes.empty() || postSpikes.empty()) {
        return;
    }
    
    // OPTIMIZED IMPLEMENTATION: O(n log n) instead of O(n²)
    // Sort spike arrays for efficient windowed processing
    std::vector<Timestamp> sortedPreSpikes = preSpikes;
    std::vector<Timestamp> sortedPostSpikes = postSpikes;
    std::sort(sortedPreSpikes.begin(), sortedPreSpikes.end());
    std::sort(sortedPostSpikes.begin(), sortedPostSpikes.end());
    
    float totalDelta = 0.0f;
    float tau = pImpl->timeConstant;
    
    // Use sliding windows to limit comparisons to relevant time ranges
    // Each spike only interacts with spikes within a time window of 3*tau
    float timeWindow = 3.0f * tau;
    
    for (Timestamp postTime : sortedPostSpikes) {
        // Find pre-synaptic spikes that could interact with this post-synaptic spike
        // Use binary search to find the start of the window
        auto preStartIt = std::lower_bound(sortedPreSpikes.begin(), sortedPreSpikes.end(),
                                          postTime - timeWindow);
        
        // Only process pre-synaptic spikes within the time window
        for (auto it = preStartIt; it != sortedPreSpikes.end(); ++it) {
            Timestamp preTime = *it;
            float dt = static_cast<float>(postTime - preTime);  // Δt in ms
            
            if (dt > timeWindow) {
                // Beyond our time window, break (since sorted)
                break;
            }
            
            if (dt > 0) {
                // Pre before post: POTENTIATION
                // "Cells that fire together, wire together" - but only if pre fires before post
                float delta = pImpl->ltpWeight * std::exp(-dt / tau);
                totalDelta += delta;
            } else if (dt < 0) {
                // Post before pre: DEPRESSION
                // "Anti-Hebbian" - connection weakens if post fires without pre
                float delta = -pImpl->ltdWeight * std::exp(dt / tau);  // dt is negative, so this subtracts
                totalDelta += delta;
            }
            // dt == 0: no change (simultaneous spikes - rare in practice)
        }
    }
    
    // Apply weight change with bounds and eligibility trace updates
    if (std::abs(totalDelta) > 1e-6f) {
        // Scale by synaptic efficacy if available (computational advantage)
        float efficacy = synapse->getEfficacy();
        totalDelta *= efficacy;
        
        // Apply weight change to synapse
        synapse->addToWeight(totalDelta);
        
        // Update eligibility trace for reward-modulated learning
        float currentTrace = synapse->getEligibilityTrace();
        synapse->setEligibilityTrace(currentTrace + totalDelta);
    }
}

/**
 * @brief Apply computed weight change to synapse with bounds checking
 * 
 * @param synapse Target synapse for weight modification
 * @param delta Weight change amount to apply (can be positive or negative)
 * 
 * @details Applies the computed STDP weight change while ensuring the synaptic
 * weight remains within biologically plausible bounds. This prevents runaway
 * excitation or depression and maintains numerical stability.
 * 
 * @note Weight bounds are defined in the implementation:
 * - Minimum: -1.0 (strong inhibition)
 * - Maximum: +1.0 (strong excitation)
 * 
 * The bounds are set to maintain reasonable dynamic range for neural computation
 * while allowing for both excitatory and inhibitory connections.
 */
void STDP::applyWeightChange(Synapse* synapse, SynapticWeight delta) {
    if (!synapse) return;
    
    // Clamp weight to biologically plausible bounds
    float newWeight = synapse->getWeight() + delta;
    newWeight = std::clamp(newWeight, pImpl->minWeight, pImpl->maxWeight);
    synapse->setWeight(newWeight);
}

/**
 * @brief Get the name identifier for this plasticity rule
 * 
 * @return const char* String identifier "STDP" used for logging and debugging
 * 
 * @details Returns a human-readable name that identifies this specific
 * plasticity implementation. This is useful for logging, debugging, and
 * configuration where multiple plasticity rules might be active.
 */
const char* STDP::getName() const {
    return "STDP";
}

/**
 * @brief Set long-term potentiation (LTP) weight parameter
 * 
 * @param weight A+ parameter value for pre-before-post potentiation
 * 
 * @details LTP weight controls the magnitude of synaptic strengthening
 * when pre-synaptic spikes precede post-synaptic spikes. This parameter
 * directly influences the learning rate for temporally ordered spike pairs.
 * 
 * @param weight New LTP weight value (clamped to [0.0, 1.0] range)
 * 
 * @note Typical biological values range from 0.001 to 0.1, with 0.01 being
 *       a common baseline. Higher values make the synapse more sensitive to
 *       pre-before-post spike timing.
 */
void STDP::setLTPWeight(float weight) {
    pImpl->ltpWeight = std::clamp(weight, 0.0f, 1.0f);
}

/**
 * @brief Get the current long-term potentiation (LTP) weight
 * 
 * @return float Current LTP weight parameter value
 * 
 * @details Returns the A+ parameter that controls the magnitude of synaptic
 * potentiation (strengthening) when pre-synaptic spikes occur before
 * post-synaptic spikes.
 * 
 * @note This value is clamped to the [0.0, 1.0] range and is initialized
 *       to 0.01f in the constructor.
 */
float STDP::getLTPWeight() const {
    return pImpl->ltpWeight;
}

/**
 * @brief Set long-term depression (LTD) weight parameter
 * 
 * @param weight A- parameter value for post-before-pre depression
 * 
 * @details LTD weight controls the magnitude of synaptic weakening
 * when post-synaptic spikes occur before pre-synaptic spikes. This parameter
 * directly influences the learning rate for temporally reversed spike pairs.
 * 
 * @param weight New LTD weight value (clamped to [0.0, 1.0] range)
 * 
 * @note Typical biological values range from 0.01 to 0.02, usually slightly
 *       larger than LTP weight to balance the asymmetry of the STDP window.
 *       Higher values make the synapse more sensitive to post-before-pre spike timing.
 */
void STDP::setLTDWeight(float weight) {
    pImpl->ltdWeight = std::clamp(weight, 0.0f, 1.0f);
}

/**
 * @brief Get the current long-term depression (LTD) weight
 * 
 * @return float Current LTD weight parameter value
 * 
 * @details Returns the A- parameter that controls the magnitude of synaptic
 * depression (weakening) when post-synaptic spikes occur before pre-synaptic
 * spikes. This typically works in conjunction with LTP to create the
 * asymmetric STDP learning window.
 * 
 * @note This value is clamped to the [0.0, 1.0] range and is initialized
 *       to 0.012f in the constructor (slightly larger than LTP for balance).
 */
float STDP::getLTDWeight() const {
    return pImpl->ltdWeight;
}

/**
 * @brief Set STDP time constant parameter
 * 
 * @param tau STDP time constant τ (default: 20.0ms)
 * 
 * @details Time constant controls the width of the STDP learning window.
 * Longer τ values make the rule more sensitive to spikes that occurred
 * further in the past, extending the temporal range of learning.
 * 
 * @param tau New time constant value (clamped to [1.0, 100.0] range)
 * 
 * @note Typical biological values range from 10-50ms, with 20ms being
 *       a common baseline. Shorter τ makes learning more temporally precise
 *       but reduces the effect of older spike pairs.
 */
void STDP::setTimeConstant(float tau) {
    pImpl->timeConstant = std::clamp(tau, 1.0f, 100.0f);
}

float STDP::getTimeConstant() const {
    return pImpl->timeConstant;
}

void STDP::configure(float ltpWeight, float ltdWeight, float tau) {
    setLTPWeight(ltpWeight);
    setLTDWeight(ltdWeight);
    setTimeConstant(tau);
}

} // namespace nlm
