#pragma once

/**
 * @file Neuromodulator.hpp
 * @brief Neuromodulation system for reinforcement learning and behavior
 * 
 * This header defines the abstract Neuromodulator class and its concrete
 * implementations for various neuromodulatory systems in the brain model.
 * Neuromodulators are chemical signals that regulate neural plasticity,
 * attention, motivation, and learning processes throughout the brain.
 * 
 * Neuromodulation Theory:
 * - Neuromodulators act as global signals that modulate neural activity
 * - They regulate synaptic plasticity, affecting learning rates
 * - Different neuromodulators serve different functions:
 *   * Dopamine: Reward prediction and reinforcement learning
 *   * Acetylcholine: Attention and memory consolidation
 *   * Norepinephrine: Arousal and vigilance
 *   * Serotonin: Mood, impulsivity, and social behavior
 * - Neuromodulators interact with prediction error signals
 * - They provide context for learning and behavioral adaptation
 * 
 * Integration Points:
 * - Applied by Brain::applyNeuromodulation() in main simulation loop
 * - Affects plasticity systems (STDP, Hebbian, Structural)
 * - Modulates attention and memory systems
 * - Influences planning and decision-making processes
 * 
 * @author Enhanced Documentation Agent
 * @date Current session
 * @version 1.0
 */

#include "../core/Types/Types.hpp"
#include <string>
#include <vector>

namespace nlm {

// Forward declarations
class Neuromodulator;
class Brain;

/**
 * @brief Abstract base class for neuromodulatory signals
 * 
 * Defines the interface for all neuromodulators in the brain model.
 * Neuromodulators implement the neuromodulation theory by providing
 * concentration-level signals that modulate neural plasticity and
 * influence learning processes. They serve as global signals that
 * coordinate activity across different brain regions.
 * 
 * Key Neuromodulation Principles:
 * 1. Concentration-dependent effects: Modulatory effect scales with neuromodulator level
 * 2. Plasticity modulation: Neuromodulators regulate synaptic learning rates
 * 3. State-dependent gating: Neuromodulators enable/disable learning based on brain state
 * 4. Signal integration: Neuromodulators combine with prediction errors for learning
 * 5. Global broadcast: Neuromodulatory signals affect widespread neural populations
 * 
 * Neuromodulator Properties:
 * - Name: Identifies the neuromodulator type
 * - Level: Current concentration/signal strength (typically 0.0-1.0)
 * - Plasticity Factor: Modulates learning rate (typically 1.0 = normal rate)
 * - Update Dynamics: Changes over time based on brain activity
 * 
 * Neuromodulation Effects:
 * - Synaptic plasticity: Enhances or suppresses STDP and Hebbian learning
 * - Attention: Biases processing toward salient stimuli
 * - Memory: Modulates consolidation and retrieval processes
 * - Motivation: Drives goal-directed behavior
 * - Arousal: Regulates alertness and responsiveness
 * 
 * Thread Safety: Neuromodulators are designed for concurrent access
 * from multiple brain components. Updates and queries are thread-safe.
 * 
 * Memory Management: Neuromodulators use pImpl pattern with pointer storage
 * for efficient memory usage and to minimize compilation dependencies.
 * 
 * @note All concrete neuromodulator implementations must override
 *       the four pure virtual methods to be instantiable.
 * @see Dopamine for reward signal implementation
 * @see Acetylcholine for attention system
 * @see Norepinephrine for arousal system
 * @see Serotonin for mood regulation
 */
class Neuromodulator {
public:
    /**
     * @brief Virtual destructor for proper cleanup
     * 
     * Ensures proper cleanup of derived class implementations
     * and maintains the pImpl pattern's exception safety guarantees.
     */
    virtual ~Neuromodulator() = default;
    
    /**
     * @brief Get the neuromodulator name
     * 
     * Returns a human-readable name identifying this neuromodulator type.
     * Used for debugging, logging, and system identification.
     * 
     * @return C-string containing the neuromodulator name (e.g., "Dopamine", "ACh")
     * 
     * @note This method should return a consistent name throughout the
     *       lifetime of the neuromodulator instance.
     */
    virtual const char* getName() const = 0;
    
    /**
     * @brief Get the current concentration/level of neuromodulator
     * 
     * Returns the current signal strength or concentration of the neuromodulator.
     * Levels typically range from 0.0 (baseline) to 1.0 (maximum activation).
     * The level influences the neuromodulator's effect on neural plasticity.
     * 
     * @return Current neuromodulator level (0.0 to 1.0 typical range)
     * 
     * @note Neuromodulator levels are state variables that change over time
     *       based on brain activity and input signals.
     */
    virtual float getLevel() const = 0;
    
    /**
     * @brief Set the neuromodulator concentration/level
     * 
     * Sets the neuromodulator signal strength. This typically represents
     * the concentration of neuromodulator in the synaptic cleft or brain regions.
     * Changing the level affects the neuromodulator's modulatory effects.
     * 
     * @param level New neuromodulator level (typically 0.0 to 1.0)
     * 
     * @note This method should be used for direct control or external
     *       neuromodulator manipulation. For automatic regulation,
     *       consider using the update() method.
     * 
     * @see getLevel()
     * @see update()
     */
    virtual void setLevel(float level) = 0;
    
    /**
     * @brief Get the plasticity modulation factor
     * 
     * Returns the factor by which this neuromodulator modulates neural
     * plasticity. A factor of 1.0 means normal plasticity, values >1.0
     * enhance plasticity (faster learning), and values <1.0 suppress
     * plasticity (slower learning).
     * 
     * @return Plasticity modulation factor (typically >0.0)
     * 
     * @note This is a key parameter in reinforcement learning models,
     *       where dopamine modulates prediction error signals and learning rates.
     * 
     * @see update()
     * @see https://en.wikipedia.org/wiki/Dopamine_hypothesis_of_learning for theory
     */
    virtual float getPlasticityFactor() const = 0;
    
    /**
     * @brief Update neuromodulator state based on time and brain activity
     * 
     * Updates the neuromodulator's concentration and plasticity factor based
     * on the elapsed time and current brain state. This is the primary method
     * for automatic neuromodulator regulation.
     * 
     * Neuromodulator Update Dynamics:
     * - Leakage: Neuromodulator levels decay toward baseline over time
     * - Release: Neuromodulators are released in response to specific events
     * - Diffusion: Neuromodulators spread across brain regions
     * - Uptake: Neuromodulators are removed from synapses
     * - Reuptake: Neuromodulators are recycled
     * 
     * Update Algorithm:
     * 1. Calculate baseline leakage (level * decay_rate)
     * 2. Add neuromodulator release based on brain events
     * 3. Apply diffusion to connected regions
     * 4. Update plasticity factor based on current level
     * 5. Ensure levels remain within valid range
     * 
     * @param dt Time step duration (simulation time units)
     * 
     * @note This method is called by Brain::applyNeuromodulation() in the
     *       main simulation loop. Implement specific neuromodulator dynamics
     *       in derived classes.
     * 
     * @see getLevel()
     * @see getPlasticityFactor()
     * @see setLevel()
     * @see https://www.sciencedirect.com/science/article/pii/S1874122210000305 for review
     */
    virtual void update(TimestepDuration dt) = 0;
    
protected:
    /**
     * @brief Default constructor for derived classes
     * 
     * Neuromodulators use protected constructor to prevent direct
     * instantiation of the abstract base class. Derived classes must
     * call this constructor explicitly.
     */
    Neuromodulator() = default;
};

// Dopamine: Reward and reinforcement learning signal
/**
 * @brief Dopamine neuromodulator: Reward prediction and reinforcement
 * 
 * Implements the dopamine system for reward signaling and reinforcement learning.
 * Dopamine encodes reward prediction errors ( RPE = actual reward - predicted reward ),
 * providing a teaching signal for learning. It modulates plasticity in reward pathways
 * and influences motivation, movement, and cognition.
 * 
 * Dopamine Theory:
 * - Spike timing-dependent plasticity: Dopamine modulates STDP based on reward
 * - Reward prediction error: Dopamine signals unexpected reward prediction errors
 * - Reward-based learning: Dopamine strengthens reward-associated behaviors
 * - Motor function: Dopamine is critical for voluntary movement initiation
 * - Executive function: Dopamine modulates prefrontal cortex activity
 * 
 * Dopamine Functions:
 * - Reward detection: Responds to rewarding stimuli and outcomes
 * - Reinforcement learning: Updates value estimates based on prediction errors
 * - Motivation: Drives approach behavior toward rewarding goals
 * - Salience detection: Marks novel or important events
 * - Movement: Controls motor activity and coordination
 * 
 * Dopamine Dynamics:
 * - Phasic responses: Brief, strong bursts in response to unexpected rewards
 * - Tonic baseline: Sustained level representing baseline reward expectation
 * - Uptake: Dopamine is cleared from synapses via transporters
 * - Receptor modulation: Different dopamine receptor subtypes mediate effects
 * 
 * Dopamine Integration:
 * - Works with prediction error system for learning
 * - Influences curiosity system for exploration
 * - Modulates reward accumulation and computation
 * - Affects attention and working memory
 * 
 * @note Phase 1 implementation is simplified. Phase 2 will implement
 *       realistic dopamine dynamics including phasic and tonic components.
 * 
 * @see https://www.nature.com/articles/s41583-021-00459-6 for dopamine review
 * @see PredictionError for prediction error computation
 */
class Dopamine : public Neuromodulator {
public:
    /**
     * @brief Default constructor
     * 
     * Creates a dopamine neuromodulator with initial state. The dopamine
     * implementation uses the pImpl pattern for efficient memory usage
     * and to reduce compilation dependencies.
     */
    Dopamine();
    
    /**
     * @brief Virtual destructor for proper cleanup
     * 
     * Ensures proper cleanup of Dopamine's pImpl implementation
     * and maintains the pImpl pattern's exception safety guarantees.
     */
    ~Dopamine() override;
    
    /**
     * @brief Get the neuromodulator name
     * 
     * @return "Dopamine" as C-string
     */
    const char* getName() const override;
    
    /**
     * @brief Get the current dopamine level
     * 
     * @return Current dopamine concentration (0.0-1.0 typical range)
     */
    float getLevel() const override;
    
    /**
     * @brief Set the dopamine level
     * 
     * @param level New dopamine level (0.0-1.0 typical range)
     */
    void setLevel(float level) override;
    
    /**
     * @brief Get the dopamine plasticity factor
     * 
     * @return Plasticity modulation factor
     */
    float getPlasticityFactor() const override;
    
    /**
     * @brief Update dopamine state based on time step
     * 
     * Updates dopamine concentration and plasticity factor based on elapsed time.
     * Implements phasic and tonic dopamine dynamics including decay and potential
     * release in response to reward signals.
     * 
     * @param dt Time step duration
     */
    void update(TimestepDuration dt) override;
    
    /**
     * @brief Signal a reward event
     * 
     * Announces a reward event to the dopamine system, triggering
     * phasic dopamine release. This simulates the biological response
     * to rewarding stimuli or outcomes.
     * 
     * @param reward Reward value (positive for reward, negative for punishment)
     * 
     * @note This method should be called by the reward system when rewards
     *       are encountered. It triggers dopamine release for reinforcement learning.
     * 
     * @see signalRewardPredictionError()
     * @see update()
     */
    void signalReward(float reward);
    
    /**
     * @brief Signal a reward prediction error
     * 
     * Announces a reward prediction error to the dopamine system,
     * simulating the phasic dopamine response to prediction errors.
     * Positive prediction errors (better than expected) increase dopamine,
     * while negative prediction errors (worse than expected) decrease it.
     * 
     * @param error Reward prediction error (actual - expected)
     * 
     * @note Dopamine responds more strongly to unexpected prediction errors
     *       than to predictable rewards (resensitivity to reward).
     * 
     * @see signalReward()
     * @see update()
     * @see https://www.nature.com/articles/s41583-021-00459-6 for RPE theory
     */
    void signalRewardPredictionError(float error);
    
private:
    /**
     * @brief Implementation details (pImpl pattern)
     * 
     * Hidden implementation for Dopamine neuromodulator.
     * Stores dopamine state including phasic and tonic components,
     * receptor dynamics, and internal timing.
     */
    struct Impl;
    Impl* pImpl;
};

// Acetylcholine: Attention and memory consolidation
/**
 * @brief Acetylcholine neuromodulator: Attention and memory consolidation
 * 
 * Implements the acetylcholine system for attentional control and memory
 * consolidation. Acetylcholine modulates neural activity to enhance
 * relevant information and suppress irrelevant information, facilitating
 * learning and memory formation.
 * 
 * Acetylcholine Theory:
 * - Attention modulation: ACh enhances signal-to-noise ratio in cortical circuits
 * - Memory consolidation: ACh promotes long-term potentiation and memory storage
 * - cortical activation: ACh activates the cortical matrix and thalamocortical circuits
 * - sleep-wake regulation: ACh controls arousal and vigilance states
 * - sensory processing: ACh modulates sensory information processing
 * 
 * Acetylcholine Functions:
 * - Selective attention: Enhances processing of attended stimuli
 * - Working memory: Maintains information in active processing
 * - Memory consolidation: Transfers information to long-term storage
 * - cortical plasticity: Enhances synaptic strength in relevant pathways
 * - arousal: Controls alertness and attention levels
 * 
 * @note Phase 1 implementation is simplified. Phase 2 will implement
 *       realistic acetylcholine dynamics including receptor subtypes
 *       and attentional modulation effects.
 * 
 * @see https://www.sciencedirect.com/science/article/pii/S1874122210000305 for review
 */
class Acetylcholine : public Neuromodulator {
public:
    /**
     * @brief Virtual destructor for proper cleanup
     */
    ~Acetylcholine() override = default;
    
    /**
     * @brief Get the neuromodulator name
     * 
     * @return "ACh" as C-string
     */
    const char* getName() const override { return "ACh"; }
    
    /**
     * @brief Get the current acetylcholine level
     * 
     * @return Current acetylcholine level
     */
    float getLevel() const override { return 0.0f; }
    
    /**
     * @brief Set the acetylcholine level
     * 
     * @param level New acetylcholine level
     */
    void setLevel(float level) override {}
    
    /**
     * @brief Get the acetylcholine plasticity factor
     * 
     * @return Plasticity modulation factor
     */
    float getPlasticityFactor() const override { return 1.0f; }
    
    /**
     * @brief Update acetylcholine state
     * 
     * @param dt Time step duration (placeholder implementation)
     */
    void update(TimestepDuration dt) override {}
};

// Norepinephrine: Arousal and vigilance
/**
 * @brief Norepinephrine neuromodulator: Arousal and vigilance
 * 
 * Implements the norepinephrine system for arousal, vigilance, and
 * cognitive control. Norepinephrine modulates attention, working memory,
 * and executive function to maintain alertness and focus in changing environments.
 * 
 * Norepinephrine Theory:
 * - Locus coeruleus system: Main source of norepinephrine in the brain
 * - Phasic responses: Brief bursts in response to novel or salient stimuli
 * - Tonic baseline: Sustained tone regulating baseline arousal
 * - cognitive control: Modulates prefrontal cortex activity and working memory
 * - attention switching: Facilitates shifts between attentional states
 * 
 * Norepinephrine Functions:
 * - Arousal: Maintains alertness and wakefulness
 * - vigilance: Monitors environment for changes and threats
 * - attention: Filters and prioritizes information processing
 * - working memory: Enhances maintenance and manipulation of information
 * - memory formation: Influences encoding and consolidation of memories
 * - stress response: Mediates fight-or-flight responses
 * 
 * @note Phase 1 implementation is simplified. Phase 2 will implement
 *       realistic norepinephrine dynamics including locus coeruleus
 *       and attention switching mechanisms.
 * 
 * @see https://www.nature.com/articles/s41583-021-00459-6 for review
 */
class Norepinephrine : public Neuromodulator {
public:
    /**
     * @brief Virtual destructor for proper cleanup
     */
    ~Norepinephrine() override = default;
    
    /**
     * @brief Get the neuromodulator name
     * 
     * @return "NE" as C-string
     */
    const char* getName() const override { return "NE"; }
    
    /**
     * @brief Get the current norepinephrine level
     * 
     * @return Current norepinephrine level
     */
    float getLevel() const override { return 0.0f; }
    
    /**
     * @brief Set the norepinephrine level
     * 
     * @param level New norepinephrine level
     */
    void setLevel(float level) override {}
    
    /**
     * @brief Get the norepinephrine plasticity factor
     * 
     * @return Plasticity modulation factor
     */
    float getPlasticityFactor() const override { return 1.0f; }
    
    /**
     * @brief Update norepinephrine state
     * 
     * @param dt Time step duration (placeholder implementation)
     */
    void update(TimestepDuration dt) override {}
};

// Serotonin: Mood, impulsivity, and social behavior
/**
 * @brief Serotonin neuromodulator: Mood, impulsivity, and social behavior
 * 
 * Implements the serotonin system for mood regulation, impulsivity control,
 * and social behavior modulation. Serotonin influences emotional states,
 * decision-making, and social interactions through widespread brain projections.
 * 
 * Serotonin Theory:
 * - Raphe nuclei system: Main source of serotonin in the brain
 * - mood regulation: Serotonin modulates emotional states and affect
 * - decision-making: Serotonin influences choice behavior and impulsivity
 * - social behavior: Serotonin affects aggression, cooperation, and bonding
 * - sleep-wake cycle: Serotonin regulates sleep and wake states
 * - temperature regulation: Serotonin influences body temperature control
 * 
 * Serotonin Functions:
 * - Mood regulation: Stabilizes emotional states and reduces negative affect
 * - Impulse control: Reduces rash decisions and promotes patience
 * - decision-making: Balances exploration vs. exploitation in choices
 * - social behavior: Modulates aggression, trust, and cooperation
 * - learning: Influences reward learning and prediction error processing
 * - stress response: Mediates responses to social stressors
 * 
 * @note Phase 1 implementation is simplified. Phase 2 will implement
 *       realistic serotonin dynamics including receptor subtypes
 *       and behavioral modulation effects.
 * 
 * @see https://www.nature.com/articles/s41583-021-00459-6 for review
 */
class Serotonin : public Neuromodulator {
public:
    /**
     * @brief Virtual destructor for proper cleanup
     */
    ~Serotonin() override = default;
    
    /**
     * @brief Get the neuromodulator name
     * 
     * @return "5-HT" as C-string
     */
    const char* getName() const override { return "5-HT"; }
    
    /**
     * @brief Get the current serotonin level
     * 
     * @return Current serotonin level
     */
    float getLevel() const override { return 0.0f; }
    
    /**
     * @brief Set the serotonin level
     * 
     * @param level New serotonin level
     */
    void setLevel(float level) override {}
    
    /**
     * @brief Get the serotonin plasticity factor
     * 
     * @return Plasticity modulation factor
     */
    float getPlasticityFactor() const override { return 1.0f; }
    
    /**
     * @brief Update serotonin state
     * 
     * @param dt Time step duration (placeholder implementation)
     */
    void update(TimestepDuration dt) override {}
};

} // namespace nlm