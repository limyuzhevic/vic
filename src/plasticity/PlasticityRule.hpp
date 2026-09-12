#pragma once

#include "../core/Types/Types.hpp"
#include "../brain/Synapse.hpp"

namespace nlm {

/**
 * @brief Abstract base class for synaptic plasticity rules
 * 
 * Plasticity rules implement the learning mechanisms that modify synaptic weights
 * based on neural activity patterns. This abstract class defines the interface
 * for all plasticity implementations, including STDP, Hebbian, and reward-modulated rules.
 * 
 * Biological Significance:
 * - Synaptic weight changes are the foundation of memory and learning
 * - Different rules implement different computational principles
 * - Integration with neuromodulators (dopamine, acetylcholine) enables context-dependent learning
 * 
 * Learning Principles:
 * - STDP: Temporal correlation between pre- and post-synaptic spikes
 * - Hebbian: Co-activation of neurons strengthens connections
 * - Reward-modulated: Learning strength depends on prediction error
 * 
 * Integration with Brain Systems:
 * - Receives spike timing from neurons
 * - Modulated by neuromodulators (dopamine, novelty, curiosity)
 * - Stores eligibility traces for reward-modulated learning
 * - Connected to memory systems for consolidation
 * - Affected by developmental stage for critical period regulation
 * 
 * Implementation Requirements:
 * - Concrete implementations must define update() method
 * - Must provide applyWeightChange() for bounded weight updates
 * - Must implement getName() for identification and logging
 * 
 * @example
 * class STDP : public PlasticityRule {
 * public:
 *     void update(Synapse* synapse,
 *                  const std::vector<Timestamp>& preSpikes,
 *                  const std::vector<Timestamp>& postSpikes,
 *                  TimestepDuration dt) override {
 *         // Implement STDP weight changes based on spike timing
 *     }
 * };
 */
class PlasticityRule {
public:
    virtual ~PlasticityRule() = default;
    
    /**
     * @brief Update synaptic weights based on pre- and post-synaptic activity
     * 
     * Core learning mechanism that computes weight changes based on spike timing
     * and activity patterns. Called during brain step() when neurons spike.
     * 
     * Learning Algorithms:
     * - STDP: Exponential decay based on spike timing differences
     * - Hebbian: Simple co-activation detection
     * - Reward-modulated: Eligibility trace computation
     * 
     * Biological Implementation:
     * - Reflects synaptic strengthening from neural activity
     * - Implements Spike-Timing-Dependent Plasticity (STDP) biological mechanism
     * - Supports homosynaptic and heterosynaptic plasticity
     * 
     * Integration:
     * - Uses synapse's spike history for timing information
     * - Modulated by neuromodulators via eligibility traces
     * - Affects synaptic efficacy for future learning
     * 
     * @param synapse Target synapse for weight modification
     * @param preSpikes Vector of presynaptic spike timestamps (ms)
     * @param postSpikes Vector of postsynaptic spike timestamps (ms)
     * @param dt Timestep duration for learning rate scaling
     * 
     * @note This is the primary learning method - all concrete implementations
     *       must override this to provide specific learning rules
     */
    virtual void update(Synapse* synapse, 
                        const std::vector<Timestamp>& preSpikes,
                        const std::vector<Timestamp>& postSpikes,
                        TimestepDuration dt) = 0;
    
    /**
     * @brief Apply computed weight change to synapse
     * 
     * Applies accumulated weight changes with biological constraints:
     * - Weight bounds to prevent runaway excitation/inhibition
     * - Saturation at physiological limits
     * - Integration with synaptic efficacy
     * 
     * Weight Constraints:
     * - Typical range: -1.0 (strong inhibition) to +1.0 (strong excitation)
     * - Zero represents no connection
     * - Sign indicates excitatory (+) or inhibitory (-) effect
     * 
     * Learning Rate Integration:
     * - Base learning rate determines magnitude of weight changes
     * - Neuromodulators (dopamine) scale learning rate
     * - Development stage affects learning capacity
     * 
     * @param synapse Target synapse for modification
     * @param delta Weight change to apply (can be positive or negative)
     * 
     * @note Concrete implementations must handle weight bounds and
     *       implement specific weight update rules
     */
    virtual void applyWeightChange(Synapse* synapse, SynapticWeight delta) = 0;
    
    /**
     * @brief Get rule name for identification and logging
     * 
     * @return Human-readable name of plasticity rule (e.g., "STDP", "Hebbian")
     * 
     * @note Used for debugging, statistics, and configuration
     */
    virtual const char* getName() const = 0;
    
    /**
     * @brief Check if rule is currently enabled
     * 
     * @return true if plasticity rule is active, false otherwise
     * 
     * @note Can be disabled to temporarily pause learning
     */
    bool isEnabled() const;
    
    /**
     * @brief Enable or disable plasticity rule
     * 
     * @param enabled New enabled state
     * 
     * @note Used for implementing critical periods and developmental control
     */
    void setEnabled(bool enabled);
    
protected:
    /**
     * @brief Protected constructor for subclass implementation
     * 
     * @param enabled Initial enabled state
     */
    explicit PlasticityRule(bool enabled = true);
    
private:
    bool enabled_;
};

/**
 * @brief Hebbian plasticity rule: "neurons that fire together, wire together"
 * 
 * Implements the classic Hebbian learning rule where co-activation of neurons
 * strengthens their connection. This is a foundational learning mechanism that
 * enables the formation of associations between simultaneously active neurons.
 * 
 * Biological Basis:
 * - Reflects synaptic strengthening from concurrent activity
 * - Implements homosynaptic plasticity (same pre- and post-synaptic neuron)
 * - Supports memory formation through co-activation patterns
 * 
 * Learning Dynamics:
 * - Positive weight changes for simultaneous firing
 * - Simple additive rule: Δw = α * pre * post
 * - Complementary to STDP for temporal patterns
 * 
 * Integration with Brain Systems:
 * - Works with working memory to strengthen active patterns
 * - Cooperates with episodic memory for experience-based learning
 * - Modulated by dopamine for reward prediction error
 * - Affected by developmental stage for critical period regulation
 * 
 * Applications:
 * - Concept formation: Pattern discovery through co-activation
 * - Semantic memory: Building associative networks
 * - Procedural memory: Strengthening action sequences
 * - Social learning: Imitating observed behaviors
 * 
 * @example
 * // Configure Hebbian learning
 * auto hebbian = std::make_unique<HebbianRule>();
 * hebbian->setLearningRate(0.01f);  // Learning strength
 * 
 * // Use in brain plasticity
 * synapse->addPlasticityRule(std::move(hebbian));
 */
class HebbianRule : public PlasticityRule {
public:
    /**
     * @brief Constructor with default parameters
     * 
     * @param learningRate Initial learning rate (default: 0.01)
     */
    explicit HebbianRule(float learningRate = 0.01f);
    
    /**
     * @brief Destructor
     */
    ~HebbianRule() override;
    
    /**
     * @brief Update synapse using Hebbian learning rule
     * 
     * Implements the Hebbian learning algorithm:
     * Δw = learningRate * postSpike * preSpike
     * 
     * Where preSpike and postSpike are binary (1 if spiked, 0 otherwise).
     * 
     * Biological Implementation:
     * - Simple associative learning rule
     * - Reflects synaptic strengthening from co-activation
     * - Complementary to STDP for temporal sequences
     * 
     * Integration:
     * - Updates working memory traces
     * - Contributes to episodic memory formation
     * - Modulated by neuromodulators
     * 
     * @param synapse Target synapse
     * @param preSpikes Presynaptic spike history
     * @param postSpikes Postsynaptic spike history
     * @param dt Timestep duration
     */
    void update(Synapse* synapse,
                 const std::vector<Timestamp>& preSpikes,
                 const std::vector<Timestamp>& postSpikes,
                 TimestepDuration dt) override;
    
    /**
     * @brief Apply Hebbian weight change with biological constraints
     * 
     * @param synapse Target synapse
     * @param delta Weight change to apply
     */
    void applyWeightChange(Synapse* synapse, SynapticWeight delta) override;
    
    /**
     * @brief Get rule name
     * 
     * @return "Hebbian"
     */
    const char* getName() const override;
    
    /**
     * @brief Set learning rate parameter
     * 
     * @param rate New learning rate (0.0 to 1.0)
     * 
     * @note Learning rate controls strength of associative learning
     */
    void setLearningRate(float rate);
    
    /**
     * @brief Get current learning rate
     * 
     * @return Learning rate
     */
    float getLearningRate() const;
    
    /**
     * @brief Configure rule with common parameters
     * 
     * @param learningRate Learning rate for weight changes
     */
    void configure(float learningRate);
    
    /**
     * @brief Get developmental modulation factor
     * 
     * @return Modulation factor (0.0 to 1.0, decreases with age)
     */
    float getDevelopmentalModulation() const;
    
    /**
     * @brief Apply developmental modulation
     * 
     * @param stage Current developmental stage
     */
    void applyDevelopmentalModulation(DevelopmentalStage stage);
    
    /**
     * @brief Get neuromodulation effect
     * 
     * @return Neuromodulatory scaling factor
     */
    float getNeuromodulatoryEffect() const;
    
    /**
     * @brief Apply neuromodulatory scaling
     * 
     * @param level Neuromodulator level (dopamine, acetylcholine, etc.)
     */
    void applyNeuromodulation(float level);

private:
    struct Impl;
    Impl* pImpl;
};

/**
 * @brief Anti-Hebbian rule: decrease weight when neurons fire together
 * 
 * Implements anti-Hebbian learning where co-activation weakens connections,
 * implementing competition and normalization in neural networks.
 * 
 * Biological Basis:
 * - Implements competitive dynamics
 * - Prevents runaway excitation
 * - Implements lateral inhibition
 * 
 * Learning Dynamics:
 * - Negative weight changes for simultaneous firing
 * - Δw = -learningRate * preSpike * postSpike
 * - Complementary to Hebbian for balance
 * 
 * Applications:
 * - Attention: Selectivity through competition
 * - Sparse coding: Preventing over-activation
 * - Homeostatic regulation
 */
class AntiHebbianRule : public PlasticityRule {
public:
    /**
     * @brief Constructor with learning rate
     * 
     * @param learningRate Learning rate for weight changes
     */
    explicit AntiHebbianRule(float learningRate = 0.01f);
    
    /**
     * @brief Default destructor
     */
    ~AntiHebbianRule() override = default;
    
    /**
     * @brief Update synapse using anti-Hebbian rule
     * 
     * Decreases weight when both pre- and post-synaptic neurons fire.
     * 
     * @param synapse Target synapse
     * @param preSpikes Presynaptic spike history
     * @param postSpikes Postsynaptic spike history
     * @param dt Timestep duration
     */
    void update(Synapse* synapse,
                 const std::vector<Timestamp>& preSpikes,
                 const std::vector<Timestamp>& postSpikes,
                 TimestepDuration dt) override;
    
    /**
     * @brief Apply anti-Hebbian weight change
     * 
     * @param synapse Target synapse
     * @param delta Weight change (negative for anti-Hebbian)
     */
    void applyWeightChange(Synapse* synapse, SynapticWeight delta) override;
    
    /**
     * @brief Get rule name
     * 
     * @return "AntiHebbian"
     */
    const char* getName() const override;
    
    /**
     * @brief Set learning rate
     * 
     * @param rate Learning rate
     */
    void setLearningRate(float rate);
    
    /**
     * @brief Get learning rate
     */
    float getLearningRate() const;

private:
    float learningRate_;
    float minWeight_;
    float maxWeight_;
};

/**
 * @brief Bienenstock-Cooper-Munro (BCM) learning rule
 * 
 * Implementa homeostatic synaptic learning with sliding threshold mechanism.
 * BCM theory proposes that synaptic modification threshold θ(y) adapts based on
 * average post-synaptic activity, implementing stability-plasticity balance.
 * 
 * Biological Significance:
 * - Homeostatic regulation prevents runaway excitation
 * - Sliding threshold maintains stability
 * - Implements smart stabilization of synaptic weights
 * 
 * Learning Dynamics:
 * - Weight change sign depends on post-synaptic activity vs. threshold
 * - Θ(y) adapts to maintain average activity at target level
 * - Implements local learning rule with global stability
 * 
 * Equations:
 * - If y > Θ(y): LTP (potentiation)
 * - If y < Θ(y): LTD (depression)
 * - Θ(y+Δy) = Θ(y) + η*(y - y₀)  // Threshold adaptation
 * 
 * Applications:
 * - Memory consolidation
 * - Critical period regulation
 * - Stability-plasticity balance
 */
class BCMRule : public PlasticityRule {
public:
    /**
     * @brief Constructor with BCM parameters
     * 
     * @param learningRate Learning rate
     * @param thresholdBaseline Target activity level
     * @param adaptationRate Threshold adaptation speed
     */
    BCMRule(float learningRate = 0.01f, float thresholdBaseline = 0.5f,
            float adaptationRate = 0.001f);
    
    /**
     * @brief Default destructor
     */
    ~BCMRule() override = default;
    
    /**
     * @brief Update synapse using BCM learning rule
     * 
     * Implements BCM synaptic modification rule with sliding threshold.
     * 
     * @param synapse Target synapse
     * @param preSpikes Presynaptic spike history
     * @param postSpikes Postsynaptic spike history
     * @param dt Timestep duration
     */
    void update(Synapse* synapse,
                 const std::vector<Timestamp>& preSpikes,
                 const std::vector<Timestamp>& postSpikes,
                 TimestepDuration dt) override;
    
    /**
     * @brief Apply BCM weight change
     * 
     * @param synapse Target synapse
     * @param delta Weight change
     */
    void applyWeightChange(Synapse* synapse, SynapticWeight delta) override;
    
    /**
     * @brief Get rule name
     * 
     * @return "BCM"
     */
    const char* getName() const override;
    
    /**
     * @brief Set learning rate
     */
    void setLearningRate(float rate);
    
    /**
     * @brief Get learning rate
     */
    float getLearningRate() const;
    
    /**
     * @brief Set threshold baseline
     */
    void setThresholdBaseline(float baseline);
    
    /**
     * @brief Get threshold baseline
     */
    float getThresholdBaseline() const;
    
    /**
     * @brief Set adaptation rate
     */
    void setAdaptationRate(float rate);
    
    /**
     * @brief Get adaptation rate
     */
    float getAdaptationRate() const;
    
    /**
     * @brief Get current threshold value
     */
    float getThreshold() const;
    
    /**
     * @brief Set current threshold value
     */
    void setThreshold(float threshold);

private:
    float learningRate_;
    float thresholdBaseline_;
    float adaptationRate_;
    float threshold_;
    float averagePostActivity_;
    float minWeight_;
    float maxWeight_;
};

} // namespace nlm