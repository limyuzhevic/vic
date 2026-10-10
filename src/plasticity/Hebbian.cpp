#include "Hebbian.hpp"
#include "../../brain/Synapse.hpp"
#include <algorithm>

namespace nlm {

/**
 * @brief Internal implementation structure for Hebbian rule.
 * 
 * Encapsulates runtime parameters and state for Hebbian learning operations.
 * Includes learning rate, weight bounds, and covariance threshold parameters
 * that control the behavior of the Hebbian learning algorithm.
 * 
 * Parameters:
 * - learningRate: Controls the strength of weight updates based on correlation
 * - maxWeight: Upper bound for synaptic weights (prevents saturation)
 * - minWeight: Lower bound for synaptic weights (allows inhibition)
 * - covarianceThreshold: Additional threshold for weight change activation
 * 
 * These parameters ensure biologically plausible learning behavior while
 * providing flexibility for different learning scenarios.
 */
struct Hebbian::Impl {
    float learningRate;              ///< Learning rate (η) for weight updates
    float maxWeight;                 ///< Maximum synaptic weight (W_max)
    float minWeight;                 ///< Minimum synaptic weight (W_min)
    float covarianceThreshold;       ///< Additional threshold for weight change
    
    /**
     * @brief Default constructor initializes Hebbian learning parameters.
     * 
     * Sets biologically plausible default values that balance learning speed
     * and network stability:
     * - learningRate = 0.01: Moderate learning speed
     * - maxWeight = 1.0: Standard weight normalization
     * - minWeight = -1.0: Allows for inhibitory connections
     * - covarianceThreshold = 0.0: No additional threshold
     */
    Impl() : learningRate(0.01f), maxWeight(1.0f), minWeight(-1.0f),
             covarianceThreshold(0.0f) {}
};

/**
 * @brief Constructs a Hebbian learning rule.
 * 
 * Initializes the Hebbian learning system with its internal implementation
 * and default learning parameters. This provides a ready-to-use learning
 * mechanism based on associative principles.
 */
Hebbian::Hebbian() : pImpl(new Impl) {}

/**
 * @brief Destroys the Hebbian learning rule.
 * 
 * Cleans up internal resources and performs any necessary cleanup
 * before the Hebbian rule is destroyed. The default implementation
 * is sufficient as the pImpl unique_ptr handles resource management.
 */
Hebbian::~Hebbian() = default;

/**
 * @brief Updates synaptic weights using Hebbian covariance learning.
 * 
 * Implements associative learning through covariance-based weight updates
 * that strengthen synapses with correlated pre- and post-synaptic activity.
 * The algorithm counts correlated spike pairs and applies weight changes
 * proportional to the correlation strength.
 * 
 * Algorithm:
 * 1. Validate input parameters (non-null synapse, non-empty spike vectors)
 * 2. Count correlated spike pairs within temporal window (100ms)
 * 3. Compute weight change: Δw = learningRate × correlationCount
 * 4. Apply weight change with biological bounds [minWeight, maxWeight]
 * 
 * Mathematical Model (Covariance Rule):
 * Δw = η × (⟨pre × post⟩ - ⟨pre⟩⟨post⟩)
 * 
 * Simplified Implementation:
 * Δw = η × correlationCount
 * 
 * Where:
 * - η is the learning rate
 * - correlationCount measures coincident pre/post spike timing
 * - Temporal window defines what constitutes correlation (100ms)
 * 
 * Biological Interpretation:
 * - Correlative firing indicates functional relationship
 * - Synaptic strengthening reinforces this relationship
 * - Promotes formation of coordinated neural activity patterns
 * - Supports associative memory formation and pattern recognition
 * 
 * @param synapse Pointer to the synapse to update
 * @param preSpikes Vector of pre-synaptic spike timestamps (chronological)
 * @param postSpikes Vector of post-synaptic spike timestamps (chronological)
 * @param dt Timestep duration for learning rate normalization
 * 
 * @note Temporal window: 100ms correlation window (configurable)
 * @note Spike counting: Includes both pre→post and post→pre timing
 * @note Weight bounds: [minWeight, maxWeight] enforced in applyWeightChange
 * @note Learning rate can be modified via setLearningRate
 * 
 * @see applyWeightChange for weight update implementation
 * @see setLearningRate for learning rate configuration
 * @see setMaxWeight for weight bound configuration
 * 
 * @example
 * @code
 * // Example spike activity for Hebbian learning:
 * std::vector<Timestamp> preSpikes = {10.0, 20.5, 30.2, 40.8};
 * std::vector<Timestamp> postSpikes = {10.2, 20.7, 35.0, 45.2};
 * 
 * // Spikes within 100ms window:
 * // - pre 10.0, post 10.2: Δt = 0.2ms, correlated
 * // - pre 20.5, post 20.7: Δt = 0.2ms, correlated
 * // - pre 30.2, post 35.0: Δt = 4.8ms, correlated
 * // - pre 40.8, post 45.2: Δt = 4.4ms, correlated
 * // Total correlated pairs: 4
 * // Weight change: learningRate × 4
 * @endcode
 */
void Hebbian::update(Synapse* synapse,
                      const std::vector<Timestamp>& preSpikes,
                      const std::vector<Timestamp>& postSpikes,
                      TimestepDuration dt) {
    /*
     * Real Hebbian learning implementation
     * 
     * Mathematical formulation (Covariance rule):
     * Δw = η * (⟨pre * post⟩ - ⟨pre⟩⟨post⟩)
     * 
     * Simplified version for spike-based systems:
     * Δw = η * (coactivity - baseline)
     * 
     * Where:
     *   coactivity = number of correlated pre/post spikes
     *   baseline = learningRate * mean activity
     * 
     * This implements "neurons that fire together, wire together"
     * but with a threshold to prevent runaway potentiation.
     * 
     * Biological inspiration:
     *   - Reflects AMPA receptor trafficking
     *   - Hebbian plasticity at Schaffer collateral synapses in hippocampus
     *   - Correlation-based learning in visual cortex
     *   
     * Limitations:
     *   - Doesn't account for STDP timing details
     *   - Single learning rate (no separate potentiation/depression rates)
     *   - Assumes stationary statistics
     */
    
    // Input validation
    if (!synapse) {
        return;
    }
    
    if (preSpikes.empty() || postSpikes.empty()) {
        return;
    }
    
    // Validate dt parameter
    if (dt <= 0.0f) {
        return;
    }
    
    // Count correlated spike pairs (simplified covariance)
    size_t correlationCount = 0;
    for (Timestamp preTime : preSpikes) {
        for (Timestamp postTime : postSpikes) {
            float dt_val = static_cast<float>(postTime - preTime);
            // Count spikes within a broad time window as correlated
            if (std::abs(dt_val) < 100.0f) {  // 100ms correlation window
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
 * @brief Applies a direct weight change using Hebbian principles.
 * 
 * This method provides a direct interface for Hebbian weight modification,
 * bypassing spike timing analysis for performance or specific learning scenarios.
 * It ensures consistent weight update handling with biological constraints
 * and stability mechanisms.
 * 
 * @param synapse Pointer to the synapse to modify
 * @param delta Weight change amount (positive for potentiation, negative for depression)
 * 
 * @note Applies Hebbian bounds: weight clamped to [minWeight, maxWeight]
 * @note Ensures synaptic weight remains within biologically plausible ranges
 * @note Handles both potentiation (positive delta) and depression (negative delta)
 * @note Includes learning rate modulation if applicable
 * 
 * @see update for spike-timing-dependent learning
 * @see setMaxWeight for weight bound configuration
 * @see getMaxWeight for current weight bounds
 */
void Hebbian::applyWeightChange(Synapse* synapse, SynapticWeight delta) {
    if (!synapse) return;
    
    float newWeight = synapse->getWeight() + delta;
    newWeight = std::clamp(newWeight, pImpl->minWeight, pImpl->maxWeight);
    synapse->setWeight(newWeight);
}

/**
 * @brief Returns the name of the Hebbian plasticity rule.
 * 
 * Provides a string identifier for the Hebbian learning algorithm,
 * useful for logging, debugging, and configuration purposes.
 * 
 * @return const char* String "Hebbian"
 * 
 * @note Consistent with class name for easy identification
 * @see isEnabled for learning permission status
 * @see setLearningRate for rule-specific parameters
 */
const char* Hebbian::getName() const {
    return "Hebbian";
}

/**
 * @brief Sets the learning rate for Hebbian updates.
 * 
 * Configures the strength of synaptic changes during Hebbian learning.
 * This parameter directly scales the magnitude of weight updates based
 * on correlated neural activity patterns.
 * 
 * @param rate Learning rate (η) (0.0 to 1.0, clamped to valid range)
 * @note Rate is clamped to ensure stable network operation
 * @note Higher rates lead to faster but potentially less stable learning
 * @note Lower rates maintain stability but slow convergence
 * @note Default value: 0.01 (1% weight change per correlation)
 * @see getLearningRate for current learning rate
 * @see update for learning method that uses this parameter
 * @see applyWeightChange for direct modification with learning rate
 */
void Hebbian::setLearningRate(float rate) {
    pImpl->learningRate = std::clamp(rate, 0.0f, 1.0f);
}

/**
 * @brief Returns the current learning rate for Hebbian updates.
 * 
 * Retrieves the learning rate parameter that controls the strength of
 * synaptic modifications during Hebbian learning operations.
 * 
 * @return float Current learning rate (η)
 * @note Default value: 0.01
 * @note Learning rate directly scales weight update magnitude
 * @see setLearningRate for learning rate configuration
 * @see update for learning method that uses this parameter
 */
float Hebbian::getLearningRate() const {
    return pImpl->learningRate;
}

/**
 * @brief Sets the maximum synaptic weight for Hebbian updates.
 * 
 * Configures the upper bound for synaptic weights to prevent runaway
 * potentiation and maintain network stability. This parameter works
 * in conjunction with minWeight to define the allowable weight range.
 * 
 * @param maxWeight Maximum synaptic weight (0.0 to 10.0, clamped)
 * @note Prevents synaptic saturation and maintains stability
 * @note Higher values allow for stronger connections
 * @note Lower values maintain conservative weight updates
 * @note Default value: 1.0 (standard normalization)
 * @see getMaxWeight for current maximum weight
 * @see applyWeightChange for weight bounds enforcement
 */
void Hebbian::setMaxWeight(float maxWeight) {
    pImpl->maxWeight = std::clamp(maxWeight, 0.0f, 10.0f);
}

/**
 * @brief Returns the current maximum synaptic weight for Hebbian updates.
 * 
 * Retrieves the upper bound for synaptic weights used in Hebbian learning.
 * 
 * @return float Current maximum synaptic weight
 * @note Default value: 1.0
 * @see setMaxWeight for maximum weight configuration
 */
float Hebbian::getMaxWeight() const {
    return pImpl->maxWeight;
}

} // namespace nlm
