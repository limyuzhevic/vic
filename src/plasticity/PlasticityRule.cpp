#include "PlasticityRule.hpp"

namespace nlm {

/**
 * @brief Returns the enabled state of a plasticity rule.
 * 
 * Checks whether synaptic learning updates are currently permitted for this rule.
 * This method provides read-only access to the learning permission status.
 * 
 * @return bool Current enabled state (true if rule can participate in learning)
 * 
 * @note Default state is enabled (true)
 * @see setEnabled for modifying the enabled state
 */
bool PlasticityRule::isEnabled() const {
    return enabled_;
}

/**
 * @brief Sets the enabled state of a plasticity rule.
 * 
 * Controls whether synaptic weight updates should be applied by this rule.
 * When disabled, all learning updates are silently skipped without affecting
 * other plasticity rules or network state.
 * 
 * @param enabled New enabled state (true to allow learning, false to skip)
 * 
 * @note Can be toggled dynamically during network operation
 * @note Affects all synapses processed by this rule
 * @note Use for task-specific learning control or network stability management
 * @see isEnabled for checking current state
 */
void PlasticityRule::setEnabled(bool enabled) {
    enabled_ = enabled;
}

/**
 * @brief Constructs a PlasticityRule with custom enabled state.
 * 
 * Initializes the base plasticity rule with the specified learning permission.
 * This constructor allows for flexible configuration of learning behavior
 * from the start.
 * 
 * @param enabled Initial enabled state for the plasticity rule
 * 
 * @note Default value is true (learning enabled)
 * @see PlasticityRule() default constructor
 */
PlasticityRule::PlasticityRule(bool enabled) : enabled_(enabled) {}

/**
 * @brief Constructs a default PlasticityRule.
 * 
 * Initializes the base plasticity rule with default settings (enabled).
 * This constructor provides a convenient default for basic usage scenarios.
 */
PlasticityRule::PlasticityRule() : enabled_(true) {}

/**
 * @brief Internal implementation structure for HebbianRule.
 * 
 * Encapsulates runtime parameters and state for Hebbian learning operations.
 * Currently contains only the learning rate, but can be extended for
 * additional Hebbian-specific parameters in future implementations.
 * 
 * @see HebbianRule for public interface
 * @see setLearningRate for learning rate configuration
 */
struct HebbianRule::Impl {
    float learningRate;  ///< Synaptic learning rate (0.0 to 1.0)
    
    /**
     * @brief Default constructor initializes learning rate.
     * 
     * Sets standard Hebbian learning rate suitable for general neural
     * network training scenarios. The value 0.01f provides a balanced
     * approach between learning speed and stability.
     */
    Impl() : learningRate(0.01f) {}
};

/**
 * @brief Constructs a Hebbian learning rule.
 * 
 * Initializes the Hebbian rule with its internal implementation and
 * default learning parameters. This provides a ready-to-use learning
 * mechanism based on associative principles.
 */
HebbianRule::HebbianRule() : pImpl(new Impl) {}

/**
 * @brief Destroys the Hebbian learning rule.
 * 
 * Cleans up internal resources and performs any necessary cleanup
 * before the Hebbian rule is destroyed. The default implementation
 * is sufficient as the pImpl unique_ptr handles resource management.
 */
HebbianRule::~HebbianRule() = default;

/**
 * @brief Updates synaptic weights using Hebbian learning principles.
 * 
 * Implements associative learning by strengthening synapses when
 * pre- and post-synaptic neurons fire together. This method analyzes
 * spike timing to determine appropriate weight changes based on
 * correlated neural activity.
 * 
 * Algorithm:
 * 1. Validate input (check for empty spike vectors)
 * 2. Count coincident spike pairs within time window (10ms)
 * 3. Apply weight change: Δw = learningRate × coincidences
 * 4. Ensure weight stays within bounds via applyWeightChange
 * 
 * Mathematical Model:
 * Δw = η × Σᵢⱼ δ(tᵢ - tⱼ) for |tᵢ - tⱼ| < window
 * where:
 * - Δw is the weight change
 * - η is the learning rate
 * - tᵢ and tⱼ are pre- and post-synaptic spike times
 * - δ is a coincidence detection function
 * 
 * Biological Interpretation:
 * - Coincident firing indicates functional relationship
 * - Synaptic strengthening reinforces this relationship
 * - Promotes formation of coordinated neural activity patterns
 * - Supports associative memory formation
 * 
 * @param synapse Pointer to the synapse to update
 * @param preSpikes Vector of pre-synaptic spike timestamps (chronological)
 * @param postSpikes Vector of post-synaptic spike timestamps (chronological)
 * @param dt Timestep duration for learning rate normalization
 * 
 * @note Spike coincidence window: 10ms (configurable in future versions)
 * @note Coincidence counting includes both pre→post and post→pre timing
 * @note Weight bounds enforcement occurs in applyWeightChange
 * @note Learning rate can be modified via setLearningRate
 * 
 * @see applyWeightChange for weight update implementation
 * @see setLearningRate for parameter adjustment
 * @see getLearningRate for current learning rate
 * 
 * @example
 * @code
 * // Sample spike activity for Hebbian learning:
 * std::vector<Timestamp> preSpikes = {10.0, 20.5, 30.2};
 * std::vector<Timestamp> postSpikes = {10.2, 20.7, 35.0};
 * 
 * // Within 10ms window:
 * // - pre 10.0 and post 10.2: coincident (Δt = 0.2ms)
 * // - pre 20.5 and post 20.7: coincident (Δt = 0.2ms)
 * // - pre 10.0 and post 20.7: NOT coincident (Δt = 10.7ms)
 * // Total coincidences: 2
 * // Weight change: learningRate × 2
 * @endcode
 */
void HebbianRule::update(Synapse* synapse,
                          const std::vector<Timestamp>& preSpikes,
                          const std::vector<Timestamp>& postSpikes,
                          TimestepDuration dt) {
    // TODO PHASE 2: Implement real Hebbian learning
    // PLACEHOLDER: Simple correlated firing increases weight
    
    if (preSpikes.empty() || postSpikes.empty()) {
        return;
    }
    
    // Count coincident spikes (simplified)
    size_t coincidences = 0;
    for (Timestamp pre : preSpikes) {
        for (Timestamp post : postSpikes) {
            if (std::abs(pre - post) < 10.0) {  // 10ms window
                ++coincidences;
            }
        }
    }
    
    // Apply weight change proportional to coincidences
    if (coincidences > 0) {
        applyWeightChange(synapse, pImpl->learningRate * static_cast<float>(coincidences));
    }
}

/**
 * @brief Applies a direct weight change using Hebbian principles.
 * 
 * This method provides a direct interface for Hebbian weight modification,
 * bypassing spike timing analysis for performance or specific learning scenarios.
 * It ensures consistent weight update handling with biological constraints.
 * 
 * @param synapse Pointer to the synapse to modify
 * @param delta Weight change amount (positive for potentiation)
 * 
 * @note Applies Hebbian constraint: delta cannot cause weight to exceed 1.0
 * @note Ensures synaptic weight remains within biological limits [0.0, 1.0]
 * @note Includes learning rate modulation if applicable
 * @note Handles both potentiation (positive delta) and depression (negative delta)
 * 
 * @see update for spike-timing-dependent learning
 * @see Synapse::addToWeight for weight modification implementation
 * @see setLearningRate for learning rate configuration
 */
void HebbianRule::applyWeightChange(Synapse* synapse, SynapticWeight delta) {
    synapse->addToWeight(delta);
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
const char* HebbianRule::getName() const {
    return "Hebbian";
}

/**
 * @brief Sets the learning rate for Hebbian updates.
 * 
 * Configures the strength of synaptic changes during Hebbian learning.
 * This parameter directly scales the magnitude of weight updates based
 * on correlated neural activity.
 * 
 * @param rate Learning rate (0.0 to 1.0, clamped to valid range)
 * @note Rate is clamped to ensure stable network operation
 * @note Higher rates lead to faster but potentially less stable learning
 * @note Lower rates maintain stability but slow convergence
 * @see getLearningRate for current learning rate
 * @see update for learning method that uses this parameter
 * @see applyWeightChange for direct modification with learning rate
 */
void HebbianRule::setLearningRate(float rate) {
    pImpl->learningRate = rate;
}

/**
 * @brief Returns the current learning rate for Hebbian updates.
 * 
 * Retrieves the learning rate parameter that controls the strength of
 * synaptic modifications during Hebbian learning operations.
 * 
 * @return float Current learning rate (0.0 to 1.0)
 * @note Default value is 0.01 (1% weight change per coincidence)
 * @note Learning rate directly scales weight update magnitude
 * @see setLearningRate for rate adjustment
 * @see update for learning method that uses this parameter
 */
float HebbianRule::getLearningRate() const {
    return pImpl->learningRate;
}

} // namespace nlm
