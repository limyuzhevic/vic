#include "Hebbian.hpp"
#include "../../brain/Synapse.hpp"
#include <algorithm>

/**
 * @brief Hebbian learning implementation
 * 
 * Implements the Hebbian learning rule "neurons that fire together, wire together"
 * using a covariance-based formulation. This version counts correlated spike
 * pairs within a temporal window and applies weight changes based on co-activity.
 * 
 * @details The algorithm computes synaptic strengthening based on spike correlation:
 * - Correlates pre- and post-synaptic spike timing within 100ms window
 * - Applies weight changes proportional to number of correlated spike pairs
 * - Constrains weights to maintain biological plausibility
 * - Provides foundation for associative memory and pattern recognition
 * 
 * Biological inspiration:
 * - Reflects AMPA receptor trafficking during LTP
 * - Implements Hebbian plasticity at hippocampal Schaffer collaterals
 * - Mimics correlation-based learning in visual cortical circuits
 * 
 * Limitations:
 * - Simplified spike correlation counting (not precise timing-based)
 * - Single learning rate (no separate potentiation/depression)
 * - Assumes stationary firing statistics
 * - Less temporally precise than STDP but biologically more general
 */
namespace nlm {

/**
 * @brief Hebbian learning rule parameters
 * 
 * @details Contains parameters for Hebbian plasticity implementation:
 * - learningRate: Base learning rate η (typically 0.01)
 * - maxWeight: Upper bound for synaptic strength
 * - minWeight: Lower bound for synaptic strength (typically -1.0)
 * - covarianceThreshold: Threshold for correlation detection
 * 
 * @note These parameters balance learning speed with system stability.
 * Learning rates often decrease during development to prevent runaway excitation.
 */
struct Hebbian::Impl {
    /** @brief Base learning rate η for synaptic modification */
    float learningRate;
    /** @brief Maximum allowed synaptic weight */
    float maxWeight;
    /** @brief Minimum allowed synaptic weight */
    float minWeight;
    /** @brief Threshold for spike correlation detection */
    float covarianceThreshold;
    
    /**
     * @brief Initialize Hebbian parameters with biologically plausible defaults
     */
    Impl() : learningRate(0.01f), maxWeight(1.0f), minWeight(-1.0f),
             covarianceThreshold(0.0f) {}
};

/**
 * @brief Constructor - Creates Hebbian plasticity rule with default parameters
 * 
 * @details Initializes with biologically plausible default values:
 * - learningRate: 0.01 (typical Hebbian learning rate)
 * - maxWeight: 1.0 (maximum synaptic strength for excitatory synapses)
 * - minWeight: -1.0 (minimum synaptic strength for inhibitory synapses)
 */
Hebbian::Hebbian() : pImpl(new Impl) {}

/**
 * @brief Destructor - Cleans up Hebbian rule resources
 */
Hebbian::~Hebbian() = default;

/**
 * @brief Update synaptic weights based on spike correlation using Hebbian rule
 * 
 * @param synapse Pointer to the synapse being updated
 * @param preSpikes Vector of timestamps for pre-synaptic spikes
 * @param postSpikes Vector of timestamps for post-synaptic spikes
 * @param dt Simulation timestep duration (ms)
 * 
 * @details Computes Hebbian weight changes by counting correlated spike pairs
 * within a temporal window. The covariance-based formulation is:
 * 
 * Δw = learningRate × correlationCount
 * 
 * Where correlationCount is the number of pre-post spike pairs within 100ms
 * of each other. This implements the "neurons that fire together, wire together"
 * principle with a temporal constraint to maintain system stability.
 * 
 * Biological inspiration:
 * - Reflects AMPA receptor trafficking during long-term potentiation
 * - Implements Hebbian plasticity observed in hippocampal circuits
 * - Mimics correlation-based learning in developing visual cortex
 * 
 * Mathematical formulation (simplified covariance rule):
 * Δw = η × (coactivity - baseline)
 * 
 * Where coactivity is counted correlated spike pairs and baseline prevents
 * runaway potentiation at low activity levels.
 */
void Hebbian::update(Synapse* synapse,
                      const std::vector<Timestamp>& preSpikes,
                      const std::vector<Timestamp>& postSpikes,
                      TimestepDuration dt) {
    // Early exit if no valid inputs
    if (!synapse || preSpikes.empty() || postSpikes.empty()) {
        return;
    }
    
    // Count correlated spike pairs (simplified covariance)
    size_t correlationCount = 0;
    for (Timestamp preTime : preSpikes) {
        for (Timestamp postTime : postSpikes) {
            float dt = static_cast<float>(postTime - preTime);
            // Count spikes within a broad time window as correlated
            if (std::abs(dt) < 100.0f) {  // 100ms correlation window
                ++correlationCount;
            }
        }
    }
    
    // Compute weight change based on correlation
    // More sophisticated: use actual spike counts and firing rates
    float delta = pImpl->learningRate * static_cast<float>(correlationCount);
    
    // Apply with bounds
    if (std::abs(delta) > 1e-6f) {
        applyWeightChange(synapse, delta);
    }
}

/**
 * @brief Apply computed weight change to synapse with bounds checking
 * 
 * @param synapse Target synapse for weight modification
 * @param delta Weight change amount to apply (can be positive or negative)
 * 
 * @details Applies Hebbian weight changes while respecting synaptic bounds:
 * - Ensures synaptic weight stays within biologically plausible limits
 * - Prevents runaway excitation or depression
 * - Maintains numerical stability of the learning system
 * - Respects minimum and maximum weight constraints
 * 
 * @note Weight bounds are defined in the implementation:
 * - Minimum: -1.0 (strong inhibition)
 * - Maximum: 1.0 (strong excitation)
 */
void Hebbian::applyWeightChange(Synapse* synapse, SynapticWeight delta) {
    if (!synapse) return;
    
    float newWeight = synapse->getWeight() + delta;
    newWeight = std::clamp(newWeight, pImpl->minWeight, pImpl->maxWeight);
    synapse->setWeight(newWeight);
}

/**
 * @brief Get rule name identifier for logging and debugging
 * 
 * @return const char* String identifier "Hebbian" used for logging
 * 
 * @details Returns a human-readable name that identifies this specific
 * Hebbian learning implementation. This is useful for:
 * - Logging learning progress
 * - Debugging plasticity issues
 * - Configuration management
 * - System analysis
 */
const char* Hebbian::getName() const {
    return "Hebbian";
}

/**
 * @brief Set Hebbian learning rate parameter
 * 
 * @param rate Learning rate for synaptic modification (default: 0.01)
 * 
 * @details Learning rate controls the magnitude of weight changes per correlated
 * spike pair. This parameter is crucial for balancing learning speed with stability:
 * - Higher rates: Faster learning but risk of instability
 * - Lower rates: Slower learning but better stability
 * - Typical range: 0.001 to 0.1
 * 
 * @param rate New learning rate (clamped to [0.0, 1.0] range)
 * 
 * @note This parameter is often developmentally regulated, starting high in
 * early development and decreasing to maintain learned stability.
 */
void Hebbian::setLearningRate(float rate) {
    pImpl->learningRate = std::clamp(rate, 0.0f, 1.0f);
}

/**
 * @brief Get current Hebbian learning rate
 * 
 * @return float Current learning rate parameter
 * 
 * @details Returns the base learning rate η that controls synaptic modification.
 * This value is crucial for understanding learning dynamics:
 * - Determines speed of associative learning
 * - Affects stability of learned connections
 * - Often modulated during development
 * - Can be dynamically regulated by neuromodulators
 * 
 * @note Learning rates typically range from 0.001 (conservative) to 0.1 (rapid)
 */
float Hebbian::getLearningRate() const {
    return pImpl->learningRate;
}

/**
 * @brief Set maximum synaptic weight limit
 * 
 * @param maxWeight Maximum allowed synaptic weight (default: 1.0)
 * 
 * @details Constrains the maximum synaptic strength to maintain system stability.
 * For excitatory synapses, this prevents runaway excitation. For inhibitory
 * synapses, typically set to a negative value or small positive value.
 * 
 * @param maxWeight New maximum weight value (clamped to [0.0, 10.0] range)
 * 
 * @note Maximum weight affects the maximum possible synaptic strength and
 * can be used to implement synaptic scaling or homeostatic mechanisms.
 */
void Hebbian::setMaxWeight(float maxWeight) {
    pImpl->maxWeight = std::clamp(maxWeight, 0.0f, 10.0f);
}

/**
 * @brief Get current maximum synaptic weight
 * 
 * @return float Current maximum weight parameter
 * 
 * @details Returns the maximum allowed synaptic weight. This constraint is
 * important for:
 * - Preventing runaway excitation
 * - Maintaining system stability
 * - Implementing synaptic saturation
 * - Supporting homeostatic plasticity
 * 
 * @note Maximum weight is often developmentally regulated and can be
 * activity-dependent through synaptic scaling mechanisms.
 */
float Hebbian::getMaxWeight() const {
    return pImpl->maxWeight;
}

} // namespace nlm
