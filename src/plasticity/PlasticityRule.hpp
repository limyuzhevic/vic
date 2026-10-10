#pragma once

#include "../core/Types/Types.hpp"
#include "../brain/Synapse.hpp"

namespace nlm {

/**
 * @brief Abstract base class for synaptic plasticity rules.
 * 
 * Plasticity rules define how synaptic weights change in response to neural activity.
 * They implement learning algorithms that enable neural networks to adapt and learn
 * from experience. The class provides a common interface for different learning
 * paradigms while allowing specific implementations to define their unique
 * mathematical models and update rules.
 * 
 * The plasticity system supports various learning rules, each with different
 * biological plausibility and computational characteristics:
 * - Hebbian rules: "Neurons that fire together, wire together"
 * - Anti-Hebbian rules: "Neurons that fire together, weaken together"  
 * - STDP (Spike-Timing-Dependent Plasticity): Biologically plausible learning
 * - BCM (Bienenstock-Cooper-Munro) rule: Synaptic competition and stability
 * 
 * @note This is a placeholder implementation for Phase 2. The complete implementation
 *       will include real biological models, eligibility traces, and spike timing
 *       dependent dynamics.
 * 
 * @see HebbianRule for classic Hebbian learning implementation
 * @see AntiHebbianRule for anti-Hebbian learning implementation
 * @see BCMRule for synaptic competition rule implementation
 * @see StructuralPlasticity for structural changes (new synapses, pruning)
 * @see Synapse for synaptic weight management
 * 
 * @example
 * @code
 * // Create different plasticity rule instances
 * PlasticityRule* hebbian = new HebbianRule();
 * PlasticityRule* antiHebbian = new AntiHebbianRule();
 * PlasticityRule* bcm = new BCMRule();
 * 
 * // Configure rule parameters
 * hebbian->setLearningRate(0.01f);
 * 
 * // Apply learning based on neural activity
 * hebbian->update(synapse, preSpikes, postSpikes, dt);
 * 
 * // Or apply direct weight changes
 * hebbian->applyWeightChange(synapse, delta);
 * 
 * // Check if learning is enabled
 * if (hebbian->isEnabled()) {
 *     // Perform learning updates
 * }
 * 
 * // Clean up
 * delete hebbian;
 * delete antiHebbian;
 * delete bcm;
 * @endcode
 */
class PlasticityRule {
public:
    /**
     * @brief Virtual destructor for proper cleanup of derived class instances.
     * 
     * Ensures that derived class destructors are called when deleting
     * PlasticityRule pointers through this base class interface.
     */
    virtual ~PlasticityRule() = default;
    
    /**
     * @brief Updates synaptic weights based on pre- and post-synaptic activity.
     * 
     * This is the core learning method that implements the specific learning
     * rule's mathematical model. It analyzes the timing and patterns of neural
     * activity to determine appropriate weight changes for synaptic adaptation.
     * 
     * The method implements spike-timing-dependent plasticity (STDP) principles
     * where the relative timing of pre- and post-synaptic spikes determines
     * whether the synapse should be potentiated ( strengthened) or depressed
     * (weakened). The update rule varies by specific learning algorithm.
     * 
     * @param synapse Pointer to the synapse whose weight should be updated
     * @param preSpikes Vector of pre-synaptic spike timestamps (chronological order)
     * @param postSpikes Vector of post-synaptic spike timestamps (chronological order)
     * @param dt Timestep duration for learning rate normalization
     * 
     * @note Spike timing precision: Timestamps use system-specific time units
     * @note Spike vectors should be sorted chronologically for proper processing
     * @note dt should be the time interval between consecutive updates
     * @note Null synapse pointer should be handled by derived implementations
     * 
     * @see applyWeightChange for direct weight modification
     * @see getName for rule identification
     * @see isEnabled for learning permission checking
     * 
     * @example
     * @code
     * // For Hebbian learning:
     * // Coincident pre- and post-synaptic spikes strengthen the synapse
     * if (!preSpikes.empty() && !postSpikes.empty()) {
     *     // Count coincident spike pairs
     *     size_t coincidences = countCoincidentalSpikes(preSpikes, postSpikes);
     *     float weightChange = learningRate * coincidences;
     *     synapse->addToWeight(weightChange);
     * }
     * 
     * // For STDP:
     * // Spike timing determines potentiation vs depression
     * if (!preSpikes.empty() && !postSpikes.empty()) {
     *     float timeDiff = postSpikes.back() - preSpikes.front();
     *     if (timeDiff < 0) {  // Pre before post = LTP
     *         synapse->addToWeight(positiveLearningRate * timeDiff);
     *     } else {  // Post before pre = LTD
     *         synapse->addToWeight(negativeLearningRate * timeDiff);
     *     }
     * }
     * @endcode
     */
    virtual void update(Synapse* synapse, 
                        const std::vector<Timestamp>& preSpikes,
                        const std::vector<Timestamp>& postSpikes,
                        TimestepDuration dt) = 0;
    
    /**
     * @brief Applies a direct weight change to the specified synapse.
     * 
     * This method provides a direct interface for weight modification, useful
     * for implementing custom learning rules or bypassing spike timing analysis.
     * It ensures consistent weight update handling across all plasticity rules.
     * 
     * @param synapse Pointer to the synapse to modify
     * @param delta Weight change amount (positive for potentiation, negative for depression)
     * 
     * @note Weight change should be constrained by rule-specific limits
     * @note Clipping should be performed by the implementation based on biological constraints
     * @note Includes activity-dependent modulation if applicable to the rule
     * 
     * @see update for spike-timing-dependent learning
     * @see Synapse::addToWeight for weight modification implementation
     */
    virtual void applyWeightChange(Synapse* synapse, SynapticWeight delta) = 0;
    
    /**
     * @brief Returns the name/identifier of this plasticity rule.
     * 
     * Provides a string identifier for the learning algorithm, useful for:
     * - Logging and debugging
     * - Configuration and selection
     * - Serialization and persistence
     * - User interface display
     * 
     * @return const char* Human-readable name of the plasticity rule
     * 
     * @note Returns consistent name regardless of configuration state
     * @see isEnabled for learning permission status
     * @see setLearningRate for rule-specific parameters
     */
    virtual const char* getName() const = 0;
    
    /**
     * @brief Checks if this plasticity rule is currently enabled.
     * 
     * Determines whether the learning algorithm should be applied to synapses
     * during network operation. Rules can be disabled for various reasons:
     * - Experimental conditions
     * - Task-specific requirements
     * - Network stability considerations
     * - Computational constraints
     * 
     * @return true if the rule is enabled for learning, false if learning should be skipped
     * 
     * @note Default state is enabled (true)
     * @see setEnabled for changing learning permission
     * @see update for learning method that respects this setting
     */
    bool isEnabled() const;
    
    /**
     * @brief Enables or disables this plasticity rule.
     * 
     * Controls whether the learning algorithm should be applied to synapses
     * during network operation. This provides runtime control over learning
     * behavior without requiring object reconstruction.
     * 
     * @param enabled If true, the rule will participate in synaptic learning;
     *                if false, synapses will not be modified by this rule
     * 
     * @note Can be toggled during runtime without side effects
     * @note Affects all synapses processed by this rule
     * @note Useful for task-specific learning control
     */
    void setEnabled(bool enabled);
    
protected:
    /**
     * @brief Constructs a PlasticityRule instance with enabled state.
     * 
     * Initializes the learning rule with the specified enabled state, allowing
     * for flexible configuration of learning behavior.
     * 
     * @param enabled Initial enabled state for the plasticity rule
     */
    PlasticityRule(bool enabled = true);
    
    /**
     * @brief Constructs a default PlasticityRule instance.
     * 
     * Initializes the learning rule with default settings (enabled).
     * This constructor provides a convenient default for basic usage.
     */
    PlasticityRule();
    
private:
    /**
     * @brief Learning rule enabled state flag.
     * 
     * Controls whether synaptic weight updates are applied by this rule.
     * When disabled, all learning updates are silently skipped.
     * 
     * @note Default value is true (enabled)
     * @see isEnabled
     * @see setEnabled
     */
    bool enabled_;
};

/**
 * @brief Hebbian plasticity rule implementing classic associative learning.
 * 
 * This rule implements the fundamental Hebbian learning principle:
 * "Neurons that fire together, wire together." When pre- and post-synaptic
 * neurons are active simultaneously or in close temporal proximity, the
 * synaptic connection between them is strengthened.
 * 
 * The Hebbian rule is the most biologically inspired learning rule and
 * forms the foundation for many modern neural network architectures. It
 * promotes the formation of coordinated neural activity patterns and
 * supports associative memory formation.
 * 
 * Algorithm:
 * - Detect coincident pre- and post-synaptic spikes
 * - Count coincident spike pairs within a time window
 * - Apply weight change proportional to coincidence count
 * - Ensure weight stays within biological limits
 * 
 * @note This is a placeholder implementation for Phase 2. The complete
 *       implementation will include precise spike timing analysis and
 *       biological constraints.
 * 
 * @see PlasticityRule for base class interface
 * @see AntiHebbianRule for complementary learning rule
 * @see BCMRule for synaptic competition variant
 */
class HebbianRule : public PlasticityRule {
public:
    /**
     * @brief Constructs a Hebbian learning rule with default parameters.
     * 
     * Initializes the learning rule with standard Hebbian parameters
     * suitable for general neural network training. The learning rate
     * provides control over the strength of weight updates.
     */
    HebbianRule();
    
    /**
     * @brief Destroys the Hebbian learning rule.
     * 
     * Cleans up internal resources and performs any necessary cleanup
     * before the Hebbian rule is destroyed.
     */
    ~HebbianRule() override;
    
    /**
     * @brief Updates synaptic weights based on Hebbian learning principles.
     * 
     * Implements associative learning by strengthening synapses when
     * pre- and post-synaptic neurons fire together. The rule analyzes
     * spike timing to determine appropriate weight changes.
     * 
     * Mathematical Model:
     * Δw = η × Σᵢⱼ δ(tᵢ - tⱼ) for tᵢ ≈ tⱼ
     * where:
     * - Δw is the weight change
     * - η is the learning rate
     * - tᵢ and tⱼ are pre- and post-synaptic spike times
     * - δ is a coincidence detection function
     * 
     * Implementation Details:
     * 1. Count coincident spike pairs within time window (10ms)
     * 2. Apply weight change: Δw = learningRate × coincidences
     * 3. Ensure weight stays within [0.0, 1.0] range
     * 4. Update statistics and learning history
     * 
     * @param synapse Pointer to the synapse to update
     * @param preSpikes Vector of pre-synaptic spike timestamps
     * @param postSpikes Vector of post-synaptic spike timestamps
     * @param dt Timestep duration for learning rate normalization
     * 
     * @note Spike coincidence window: 10ms (configurable)
     * @note Weight bounds: [0.0, 1.0] (standard normalization)
     * @note Learning rate can be modified via setLearningRate
     * 
     * @see applyWeightChange for direct modification
     * @see setLearningRate for parameter adjustment
     * @see getLearningRate for current learning rate
     */
    void update(Synapse* synapse,
                 const std::vector<Timestamp>& preSpikes,
                 const std::vector<Timestamp>& postSpikes,
                 TimestepDuration dt) override;
    
    /**
     * @brief Applies a direct weight change using Hebbian principles.
     * 
     * Provides a direct interface for Hebbian weight modification,
     * bypassing spike timing analysis for performance or specific
     * learning scenarios.
     * 
     * @param synapse Pointer to the synapse to modify
     * @param delta Weight change amount (positive for potentiation)
     * 
     * @note Applies Hebbian constraint: delta cannot cause weight to exceed 1.0
     * @note Ensures synaptic weight remains within biological limits [0.0, 1.0]
     * @note Includes learning rate modulation if applicable
     * 
     * @see update for spike-timing-dependent learning
     * @see Synapse::addToWeight for implementation details
     */
    void applyWeightChange(Synapse* synapse, SynapticWeight delta) override;
    
    /**
     * @brief Returns the name of the Hebbian plasticity rule.
     * 
     * @return const char* String "Hebbian"
     */
    const char* getName() const override;
    
    /**
     * @brief Sets the learning rate for Hebbian updates.
     * 
     * Configures the strength of synaptic changes during Hebbian learning.
     * Higher learning rates lead to faster but potentially less stable learning.
     * 
     * @param rate Learning rate (0.0 to 1.0, clamped to valid range)
     * @note Rate is clamped to ensure stable network operation
     * @see getLearningRate for current learning rate
     * @see update for learning method that uses this parameter
     */
    void setLearningRate(float rate);
    
    /**
     * @brief Returns the current learning rate for Hebbian updates.
     * 
     * @return float Current learning rate (0.0 to 1.0)
     * @note Default value is 0.01 (1% weight change per coincidence)
     * @see setLearningRate for rate adjustment
     * @see update for learning method that uses this parameter
     */
    float getLearningRate() const;
    
private:
    /**
     * @brief Internal implementation structure for HebbianRule.
     * 
     * Encapsulates runtime parameters and state for Hebbian learning.
     * Includes the learning rate and any additional parameters needed
     * for specific Hebbian implementations.
     */
    struct Impl;
    Impl* pImpl;
};

/**
 * @brief Anti-Hebbian plasticity rule implementing opposite associative learning.
 * 
 * This rule implements the principle of opposite associative learning:
 * "Neurons that fire together, weaken together." When pre- and post-synaptic
 * neurons are active simultaneously, the synaptic connection between them
 * is weakened instead of strengthened.
 * 
 * The anti-Hebbian rule provides complementary learning capabilities to
 * standard Hebbian learning, enabling network stability and preventing
 * runaway excitation. It can be useful for:
 * - Inhibitory learning and normalization
 * - Preventing synaptic saturation
 * - Implementing competition among synapses
 * - Stability maintenance in recurrent networks
 * 
 * Algorithm:
 * - Detect coincident pre- and post-synaptic spikes
 * - Apply negative weight change proportional to coincidence count
 * - Ensure weight stays within biological limits (may not go below 0.0)
 * 
 * @note This is a placeholder implementation for Phase 2
 * @note The implementation is simplified and may not reflect biological accuracy
 * 
 * @see PlasticityRule for base class interface
 * @see HebbianRule for complementary Hebbian learning rule
 * @see BCMRule for synaptic competition variant
 */
class AntiHebbianRule : public PlasticityRule {
public:
    /**
     * @brief Constructs an Anti-Hebbian learning rule.
     * 
     * Initializes the learning rule with default settings for anti-associative learning.
     */
    AntiHebbianRule();
    
    /**
     * @brief Updates synaptic weights using anti-Hebbian principles.
     * 
     * This method is currently a placeholder implementation. In a complete
     * implementation, it would weaken synapses when pre- and post-synaptic
     * neurons fire together.
     * 
     * @param synapse Pointer to the synapse to update
     * @param preSpikes Vector of pre-synaptic spike timestamps
     * @param postSpikes Vector of post-synaptic spike timestamps
     * @param dt Timestep duration for learning rate normalization
     */
    void update(Synapse* synapse,
                 const std::vector<Timestamp>& preSpikes,
                 const std::vector<Timestamp>& postSpikes,
                 TimestepDuration dt) override;
    
    /**
     * @brief Applies a direct weight change using anti-Hebbian principles.
     * 
     * This method is currently a placeholder implementation. In a complete
     * implementation, it would apply negative weight changes.
     * 
     * @param synapse Pointer to the synapse to modify
     * @param delta Weight change amount (typically negative)
     */
    void applyWeightChange(Synapse* synapse, SynapticWeight delta) override;
    
    /**
     * @brief Returns the name of the anti-Hebbian plasticity rule.
     * 
     * @return const char* String "AntiHebbian"
     */
    const char* getName() const override { return "AntiHebbian"; }
};

/**
 * @brief Bienenstock-Cooper-Munro (BCM) plasticity rule implementing synaptic competition.
 * 
 * The BCM rule introduces synaptic competition and stability through a dynamic
 * threshold mechanism. It was designed to solve the stability problem in Hebbian
 * learning by implementing homeostatic regulation of synaptic weights.
 * 
 * Key Features:
 * - Synaptic competition: Strong synapses inhibit weak ones
 * - Dynamic threshold: Self-organizing threshold adapts to average activity
 * - Stability: Prevents runaway excitation and maintains network balance
 * - Learning rules: Combines Hebbian potentiation with homeostatic depression
 * 
 * Mathematical Model:
 * Δw = { (w - θ) × x × y        if w > θ (LTP)
 *       { -(w - θ) × x × y    if w < θ (LTD)
 * where:
 * - Δw is the weight change
 * - w is the current synaptic weight
 * - θ is the dynamic threshold
 * - x is the post-synaptic activity
 * - y is the pre-synaptic activity
 * 
 * Threshold Dynamics:
 * θ = φ × (average post-synaptic activity over time)
 * where φ is a stability parameter
 * 
 * @note This is a placeholder implementation for Phase 2
 * @note The implementation is simplified and may not reflect biological accuracy
 * @note BCM provides excellent stability for neural network training
 * 
 * @see PlasticityRule for base class interface
 * @see HebbianRule for simpler Hebbian implementation
 * @see AntiHebbianRule for opposite learning rule
 */
class BCMRule : public PlasticityRule {
public:
    /**
     * @brief Constructs a BCM plasticity rule.
     * 
     * Initializes the BCM learning rule with default parameters for
     * synaptic competition and stability.
     */
    BCMRule();
    
    /**
     * @brief Updates synaptic weights using BCM learning principles.
     * 
     * This method is currently a placeholder implementation. In a complete
     * implementation, it would implement synaptic competition with dynamic
     * threshold regulation.
     * 
     * @param synapse Pointer to the synapse to update
     * @param preSpikes Vector of pre-synaptic spike timestamps
     * @param postSpikes Vector of post-synaptic spike timestamps
     * @param dt Timestep duration for learning rate normalization
     */
    void update(Synapse* synapse,
                 const std::vector<Timestamp>& preSpikes,
                 const std::vector<Timestamp>& postSpikes,
                 TimestepDuration dt) override;
    
    /**
     * @brief Applies a direct weight change using BCM principles.
     * 
     * This method is currently a placeholder implementation. In a complete
     * implementation, it would apply weight changes with synaptic competition.
     * 
     * @param synapse Pointer to the synapse to modify
     * @param delta Weight change amount
     */
    void applyWeightChange(Synapse* synapse, SynapticWeight delta) override;
    
    /**
     * @brief Returns the name of the BCM plasticity rule.
     * 
     * @return const char* String "BCM"
     */
    const char* getName() const override { return "BCM"; }
};

} // namespace nlm
