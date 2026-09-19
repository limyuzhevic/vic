#include "Hebbian.hpp"
#include "../../brain/Synapse.hpp"
#include <algorithm>

namespace nlm {

/**
 * @class Hebbian::Impl
 * @brief Private implementation details for Hebbian plasticity rule
 * 
 * Encapsulates all parameters for the Hebbian learning rule, which implements
 * the "neurons that fire together, wire together" principle. Uses the pImpl
 * pattern to hide implementation details from the interface and allow forward
 * declarations.
 */
struct Hebbian::Impl {
    float learningRate;               /**< η: Learning rate for Hebbian updates */
    float maxWeight;                  /**< w_max: Maximum synaptic weight to prevent runaway potentiation */
    float minWeight;                  /**< w_min: Minimum synaptic weight to prevent runaway depression */
    float covarianceThreshold;        /**< Threshold for covariance-based learning rule */
    size_t correlationCount;          /**< Running count of correlated spike pairs for current update */
    
    Impl() : learningRate(0.01f), maxWeight(1.0f), minWeight(-1.0f),
             covarianceThreshold(0.0f), correlationCount(0) {}
};

/**
 * @brief Constructs a new Hebbian plasticity rule with default parameters
 * 
 * Initializes the private implementation struct with default Hebbian parameters:
 * - learningRate: 0.01f (1% learning per update cycle)
 * - maxWeight: 1.0f (upper bound for synaptic strength)
 * - minWeight: -1.0f (lower bound for synaptic strength)
 * - covarianceThreshold: 0.0f (disabled by default, use fixed learning rate)
 */
Hebbian::Hebbian() : pImpl(new Impl) {}

/**
 * @brief Destroys the Hebbian object
 * 
 * Cleans up the private implementation by deleting the pImpl pointer.
 * Automatically called when Hebbian object goes out of scope.
 */
Hebbian::~Hebbian() = default;

/**
 * @brief Updates synaptic weights using Hebbian learning rule
 * 
 * Implements the Hebbian "fire together, wire together" principle for
 * spike-based neural networks. Uses a simplified covariance-based learning
 * rule that's computationally efficient for large-scale simulations.
 * 
 * Mathematical formulation (Covariance rule):
 * Δw = η * (⟨pre * post⟩ - ⟨pre⟩⟨post⟩)
 * 
 * Simplified version for spike-based systems:
 * Δw = η * (coactivity - baseline)
 * 
 * Where:
 *   coactivity = number of correlated pre/post spikes within time window
 *   baseline = learningRate * baseline activity level
 *   η = learningRate (learning speed parameter)
 * 
 * Biological inspiration and neural correlates:
 * - Reflects AMPA receptor insertion/trafficking at synapses
 * - Models Hebbian plasticity at Schaffer collateral synapses in hippocampus
 * - Represents correlation-based learning in visual cortex (Hubel-Wiesel)
 * - Similar to NMDA-dependent synaptic tagging and capture mechanisms
 * - Implements homeostatic-like bounds to maintain stability
 * 
 * Algorithmic approach:
 * 1. Counts correlated spike pairs within a 100ms window
 * 2. Applies learning rate scaling based on correlation count
 * 3. Clips resulting weights to prevent runaway potentiation/depression
 * 
 * Key properties:
 * - Potentiation only (no anti-Hebbian depression)
 * - Single learning rate for both directions
 * - Assumes stationary statistics (no temporal credit assignment)
 * 
 * Limitations:
 * - Doesn't account for precise spike timing (STDP improvements)
 * - Single learning rate (no separate LTP/LTD mechanisms)
 * - Assumes stationary statistics (no non-stationary learning)
 * - Limited to pairwise correlation (no higher-order interactions)
 * - No synaptic eligibility traces for delayed reward signals
 * 
 * @param synapse Pointer to the synapse being modified. Null is silently ignored.
 * @param preSpikes Vector of timestamps when pre-synaptic neuron spiked (ms)
 * @param postSpikes Vector of timestamps when post-synaptic neuron spiked (ms)
 * @param dt Timestep duration in milliseconds for this update cycle
 * 
 * @return void
 * 
 * @note Implementation uses two-pointer technique for O(n) complexity
 *       within the correlation window, making it efficient for large networks.
 *       Follows pImpl pattern for encapsulation and forward declarations.
 */
void Hebbian::update(Synapse* synapse,
                      const std::vector<Timestamp>& preSpikes,
                      const std::vector<Timestamp>& postSpikes,
                      TimestepDuration dt) {
    
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
    const float correlationWindow = 100.0f;  // 100ms correlation window
    
    while (preIdx < sortedPreSpikes.size() && postIdx < sortedPostSpikes.size()) {
        float dt = static_cast<float>(sortedPostSpikes[postIdx] - sortedPreSpikes[preIdx]);
        
        if (std::abs(dt) < correlationWindow) {
            // Spikes within correlation window: count as correlated activity
            // This approximates the covariance term ⟨pre * post⟩
            ++correlationCount;
            // Move the pointer that will lead to more correlations
            // Increment post pointer if post spike is before pre spike (dt < 0)
            // Increment pre pointer if pre spike is before post spike (dt >= 0)
            if (dt < 0) {
                // Post spike is before pre spike, move post pointer forward
                ++postIdx;
            } else {
                // Pre spike is before post spike, move pre pointer forward
                ++preIdx;
            }
        } else if (dt > correlationWindow) {
            // Post spike is too far ahead (more than window), move pre pointer
            // This maintains the sliding window property
            ++preIdx;
        } else {
            // dt < -correlationWindow, pre spike is too far ahead, move post pointer
            // Post spike is behind pre spike by more than window
            ++postIdx;
        }
    }
    
    // Compute weight change based on correlation count
    // More sophisticated implementations could use actual spike counts and firing rates
    // Δw = η * correlationCount (where η = learningRate)
    float delta = pImpl->learningRate * static_cast<float>(correlationCount);
    
    // Apply with bounds to prevent runaway potentiation/depression
    if (std::abs(delta) > 1e-6f) {
        applyWeightChange(synapse, delta);
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
void Hebbian::applyWeightChange(Synapse* synapse, SynapticWeight delta) {
    if (!synapse) return;
    
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
 * @return const char* String literal "Hebbian" identifying this rule
 */
const char* Hebbian::getName() const {
    return "Hebbian";
}

/**
 * @brief Sets the learning rate for Hebbian plasticity
 * 
 * The learning rate (η) determines the magnitude of weight changes per update
 * cycle. It scales the correlation count to produce actual weight changes.
 * 
 * @param rate The learning rate value to set. Clamped to [0.0f, 1.0f] range
 *             to ensure stable learning dynamics.
 * 
 * @return void
 * 
 * @note Typical biological values range from 0.001 to 0.01 per timestep.
 *       Larger values can cause unstable learning with runaway weights.
 */
void Hebbian::setLearningRate(float rate) {
    pImpl->learningRate = std::clamp(rate, 0.0f, 1.0f);
}

/**
 * @brief Returns the current learning rate for Hebbian plasticity
 * 
 * @return float Current learning rate value.
 * 
 * @note This is a read-only accessor for the learning rate parameter.
 */
float Hebbian::getLearningRate() const {
    return pImpl->learningRate;
}

/**
 * @brief Sets the maximum allowable synaptic weight
 * 
 * Establishes an upper bound for synaptic strength to prevent runaway
 * potentiation and maintain network stability. The maximum weight should
 * be set based on the biological or artificial constraints of the synapse.
 * 
 * @param maxWeight The maximum weight value to set. Clamped to [0.0f, 10.0f]
 *                   to ensure stable network behavior.
 * 
 * @return void
 * 
 * @note Typical values depend on the synapse model: biological synapses often
 *       have weights in the [-1, 1] or [0, 1] range, while artificial
 *       synapses may have wider ranges.
 */
void Hebbian::setMaxWeight(float maxWeight) {
    pImpl->maxWeight = std::clamp(maxWeight, 0.0f, 10.0f);
}

/**
 * @brief Returns the current maximum synaptic weight
 * 
 * @return float Current maximum weight value.
 * 
 * @note This is a read-only accessor for the maximum weight parameter.
 */
float Hebbian::getMaxWeight() const {
    return pImpl->maxWeight;
}

} // namespace nlm
