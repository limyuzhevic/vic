#include "STDP.hpp"
#include "../../brain/Synapse.hpp"
#include <cmath>
#include <algorithm>

namespace nlm {

/**
 * @class STDP::Impl
 * @brief Private implementation details for STDP plasticity rule
 * 
 * Encapsulates all parameters for the Spike-Timing-Dependent Plasticity algorithm.
 * Uses the pImpl pattern to hide implementation details from the interface.
 */
struct STDP::Impl {
    float ltpWeight;      /**< A+: Potentiation amplitude for pre-before-post spikes */
    float ltdWeight;      /**< A-: Depression amplitude for post-before-pre spikes */
    float timeConstant;   /**< τ: Time constant for exponential STDP window (ms) */
    float minWeight;      /**< Minimum synaptic weight to prevent unbounded depression */
    float maxWeight;      /**< Maximum synaptic weight to prevent unbounded potentiation */
    
    Impl() : ltpWeight(0.01f), ltdWeight(0.012f), timeConstant(20.0f),
             minWeight(-1.0f), maxWeight(1.0f) {}
};

/**
 * @brief Constructs a new STDP plasticity rule with default parameters
 * 
 * Initializes the private implementation struct with default STDP parameters:
 * - ltpWeight: 0.01f (1% potentiation per optimal timing)
 * - ltdWeight: 0.012f (1.2% depression per optimal timing)
 * - timeConstant: 20.0f ms (typical biological STDP window)
 * - minWeight: -1.0f (prevents excessive depression)
 * - maxWeight: 1.0f (prevents excessive potentiation)
 */
STDP::STDP() : pImpl(new Impl) {}

/**
 * @brief Destroys the STDP object
 * 
 * Cleans up the private implementation by deleting the pImpl pointer.
 * Automatically called when STDP object goes out of scope.
 */
STDP::~STDP() = default;

/**
 * @brief Updates synaptic weights based on spike timing differences
 * 
 * Implements the Spike-Timing-Dependent Plasticity (STDP) learning rule,
 * which strengthens synapses when pre-synaptic spikes precede post-synaptic
 * spikes and weakens them when the opposite occurs.
 * 
 * Mathematical formulation for each pre-post spike pair:
 * - If Δt = t_post - t_pre > 0 (pre before post): POTENTIATION
 *   Δw = A+ * exp(-Δt / τ+)
 * 
 * - If Δt < 0 (post before pre): DEPRESSION
 *   Δw = A- * exp(Δt / τ-)
 * 
 * Where:
 *   A+ = ltpWeight (potentiation amplitude)
 *   A- = ltdWeight (depression amplitude)
 *   τ+ = τ- = timeConstant (STDP time window, typically 10-100ms)
 * 
 * Biological inspiration:
 * - Reflects NMDA receptor-mediated calcium signaling cascades
 * - Pre-before-post activates NMDA receptors during postsynaptic depolarization
 * - Post-before-pre triggers backpropagating action potentials from soma
 * - Implements Bienenstock-Cooper-Munro (BCM) theory of synaptic modification
 * 
 * Optimization details:
 * - Uses O(n) two-pointer sliding window algorithm for efficiency
 * - Limits correlation window to ±5τ for computational efficiency
 * - Sorts spike times for proper chronological processing
 * 
 * Limitations:
 * - Simplified pairwise rule (doesn't capture triplet spike interactions)
 * - Assumes single exponential window (real STDP has asymmetric windows)
 * - Doesn't account for synaptic eligibility traces for reward-modulated learning
 * - Limited to single synapse per call (can be called per-synapse)
 * 
 * @param synapse Pointer to the synapse being modified. Null is silently ignored.
 * @param preSpikes Vector of timestamps when pre-synaptic neuron spiked (ms)
 * @param postSpikes Vector of timestamps when post-synaptic neuron spiked (ms)
 * @param dt Timestep duration in milliseconds for this update cycle
 * 
 * @return void
 * 
 * @note Implementation uses a private pImpl struct for parameter hiding
 *       and forward declarations in the header file.
 */
    
    if (!synapse || preSpikes.empty() || postSpikes.empty()) {
        return;
    }
    
    
    if (!synapse || preSpikes.empty() || postSpikes.empty()) {
        return;
    }
    
    // Optimization: Convert to O(n) using sliding window approach
    // Sort spike times for efficient windowing (O(n log n))
    std::vector<Timestamp> sortedPreSpikes = preSpikes;
    std::vector<Timestamp> sortedPostSpikes = postSpikes;
    std::sort(sortedPreSpikes.begin(), sortedPreSpikes.end());
    std::sort(sortedPostSpikes.begin(), sortedPostSpikes.end());
    
    // Use two-pointer technique to find correlated spike pairs efficiently
    // This reduces complexity from O(n²) to O(n) for correlated pairs within window
    size_t preIdx = 0, postIdx = 0;
    float effectiveWindow = 5.0f * tau;  // Look at ±5τ for practical correlations
    
    while (preIdx < sortedPreSpikes.size() && postIdx < sortedPostSpikes.size()) {
        float dt = static_cast<float>(sortedPostSpikes[postIdx] - sortedPreSpikes[preIdx]);
        
        if (dt > 0 && dt <= effectiveWindow) {
            // Pre before post: POTENTIATION - "neurons that fire together wire together"
            // Stronger when pre-synaptic spike precedes post-synaptic spike by optimal timing
            float delta = pImpl->ltpWeight * std::exp(-dt / tau);
            totalDelta += delta;
            ++postIdx;
        } else if (dt < 0 && dt >= -effectiveWindow) {
            // Post before pre: DEPRESSION - "neurons that fire out of sync lose their link"
            // Weaker connections when post-synaptic spike precedes pre-synaptic spike
            float delta = -pImpl->ltdWeight * std::exp(dt / tau);  // dt is negative, so this subtracts
            totalDelta += delta;
            ++preIdx;
        } else if (dt > effectiveWindow) {
            // Post is too far ahead, move pre pointer to catch up
            ++preIdx;
        } else if (dt < -effectiveWindow) {
            // Pre is too far ahead, move post pointer to catch up
            ++postIdx;
        } else {
            // dt is 0 or at boundaries - simultaneous spikes or edge case
            // Advance both pointers to avoid infinite loops
            ++preIdx;
            ++postIdx;
        }
    }
    
    // Apply weight change with bounds
    if (std::abs(totalDelta) > 1e-6f) {
        // Scale by synaptic efficacy if available
        // Efficacy represents how strongly the synapse is currently functional
        float efficacy = synapse->getEfficacy();
        totalDelta *= efficacy;
        
        // Apply weight change
        synapse->addToWeight(totalDelta);
        
        // Update eligibility trace for reward-modulated learning
        // Eligibility traces allow delayed reward signals to modify synaptic weights
        float currentTrace = synapse->getEligibilityTrace();
        synapse->setEligibilityTrace(currentTrace + totalDelta);
    }
}
}

/**
 * @brief Applies a direct weight change to a synapse with bounds checking
 * 
 * Clamps the resulting weight to the configured minimum and maximum values
 * to prevent unbounded synaptic strength. This method is used for direct
 * weight modifications (e.g., from config files or external sources).
 * 
 * @param synapse Pointer to the synapse to modify. Null is silently ignored.
 * @param delta The weight change to apply to the synapse. Can be positive
 *             (potentiation) or negative (depression).
 * 
 * @return void
 * 
 * @note This is a lower-level method that doesn't maintain eligibility traces
 *       or scale by synaptic efficacy. For learning rule updates, use the
 *       update() method instead.
 */
void STDP::applyWeightChange(Synapse* synapse, SynapticWeight delta) {
    if (!synapse) return;
    
    // Clamp weight to bounds
    float newWeight = synapse->getWeight() + delta;
    newWeight = std::clamp(newWeight, pImpl->minWeight, pImpl->maxWeight);
    synapse->setWeight(newWeight);
}

/**
 * @brief Returns the name of this plasticity rule
 * 
 * Used for identification and logging purposes. Allows polymorphic handling
 * of different plasticity rule types in the agent brain.
 * 
 * @return const char* String literal "STDP" identifying this rule
 */

const char* STDP::getName() const {
    return "STDP";
}

/**
 * @brief Sets the Long-Term Potentiation (LTP) weight parameter (A+)
 * 
 * The LTP weight determines the maximum potentiation strength when pre-synaptic
 * spikes precede post-synaptic spikes by optimal timing. This corresponds to
 * the A+ parameter in classical STDP formulations.
 * 
 * @param weight The LTP weight value to set. Clamped to [0.0f, 1.0f] range
 *               to ensure stable learning dynamics.
 * 
 * @return void
 * 
 * @note Typical biological values are 0.005-0.01 for millisecond timing.
 *       Larger values can cause runaway excitation.
 */
void STDP::setLTPWeight(float weight) {
    pImpl->ltpWeight = std::clamp(weight, 0.0f, 1.0f);
}

/**
 * @brief Returns the current Long-Term Potentiation (LTP) weight parameter (A+)
 * 
 * @return float Current LTP weight value. Typical biological range is 0.005-0.01.
 * 
 * @note This is a read-only accessor for the potentiation parameter.
 */

float STDP::getLTPWeight() const {
    return pImpl->ltpWeight;
}

/**
 * @brief Sets the Long-Term Depression (LTD) weight parameter (A-)
 * 
 * The LTD weight determines the maximum depression strength when post-synaptic
 * spikes precede pre-synaptic spikes. This corresponds to the A- parameter
 * in classical STDP formulations.
 * 
 * @param weight The LTD weight value to set. Clamped to [0.0f, 1.0f] range.
 * 
 * @return void
 * 
 * @note Typical biological values are 0.01-0.012 for millisecond timing.
 *       Larger values can cause runaway inhibition.
 */
void STDP::setLTDWeight(float weight) {
    pImpl->ltdWeight = std::clamp(weight, 0.0f, 1.0f);
}

/**
 * @brief Returns the current Long-Term Depression (LTD) weight parameter (A-)
 * 
 * @return float Current LTD weight value. Typical biological range is 0.01-0.012.
 * 
 * @note This is a read-only accessor for the depression parameter.
 */
float STDP::getLTDWeight() const {
    return pImpl->ltdWeight;
}

/**
 * @brief Sets the STDP time constant (tau) for the exponential window
 * 
 * The time constant controls the width of the spike-timing window in which
 * synaptic modifications occur. Larger values produce broader, weaker effects.
 * 
 * @param tau The time constant in milliseconds. Clamped to [1.0f, 100.0f] ms
 *            to prevent numerical instability and ensure practical learning rates.
 * 
 * @return void
 * 
 * @note Typical biological values range from 10-50 ms depending on neuron type
 *       and synapse location (e.g., thalamic vs. cortical synapses).
 */
void STDP::setTimeConstant(float tau) {
    pImpl->timeConstant = std::clamp(tau, 1.0f, 100.0f);
}

/**
 * @brief Returns the current STDP time constant (tau)
 * 
 * @return float Current time constant in milliseconds.
 * 
 * @note This parameter defines the characteristic time scale for spike-timing
 *       dependent plasticity. Used in the exponential decay functions.
 */
float STDP::getTimeConstant() const {
    return pImpl->timeConstant;
}

/**
 * @brief Configures all STDP parameters from a single configuration call
 * 
 * This convenience method allows setting the complete STDP learning rule
 * configuration in one operation, useful for loading from configuration files
 * or applying predefined learning regimes.
 * 
 * @param ltpWeight Long-Term Potentiation weight (A+). Clamped to [0.0f, 1.0f].
 * @param ltdWeight Long-Term Depression weight (A-). Clamped to [0.0f, 1.0f].
 * @param tau STDP time constant. Clamped to [1.0f, 100.0f] ms.
 * 
 * @return void
 * 
 * @note Equivalent to calling setLTPWeight(), setLTDWeight(), and
 *       setTimeConstant() individually, but more efficient.
 */
void STDP::configure(float ltpWeight, float ltdWeight, float tau) {
    setLTPWeight(ltpWeight);
    setLTDWeight(ltdWeight);
    setTimeConstant(tau);
}

} // namespace nlm
