#pragma once

#include "../core/Types/Types.hpp"
#include <string>
#include <vector>

namespace nlm {

/**
 * @class Neuromodulator
 * @brief Abstract base class for all neuromodulatory signals in the agent
 * 
 * Neuromodulators are chemical signals that modulate neural activity and plasticity,
 * guiding learning and behavior. This abstract base class defines the interface
 * for all neuromodulatory systems including dopamine, acetylcholine, norepinephrine,
 * and serotonin. Each neuromodulator affects different aspects of neural processing
 * and learning.
 * 
 * Neuromodulators work by:
 * - Regulating synaptic strength through plasticity modulation
 * - Modifying neuronal responsiveness and excitability
 * - Guiding attention and learning prioritization
 * - Balancing exploration vs. exploitation behavior
 * 
 * @note This class is part of Phase 1 implementation. Phase 2 will implement
 *       biologically-realistic neuromodulation effects.
 * 
 * @see Dopamine
 * @see Acetylcholine
 * @see Norepinephrine
 * @see Serotonin
 * @see Brain
 */
class Neuromodulator {
public:
    /**
     * @brief Virtual destructor for proper cleanup of derived classes
     * 
     * Ensures that derived class destructors are called when deleting
     * through base class pointers.
     */
    virtual ~Neuromodulator() = default;
    
    /**
     * @brief Get the human-readable name of this neuromodulator
     * 
     * Returns a descriptive name that identifies the neuromodulatory system,
     * such as "Dopamine", "Acetylcholine", or "Norepinephrine".
     * 
     * @return const char* String identifier of the neuromodulator
     * @post None
     * @note This identifier can be used for logging and debugging purposes
     * @see getLevel()
     * @see getPlasticityFactor()
     */
    virtual const char* getName() const = 0;
    
    /**
     * @brief Get the current concentration/level of the neuromodulator
     * 
     * Returns the current concentration or activation level of the neuromodulator
     * in the agent's brain. Higher levels typically indicate stronger neuromodulatory
     * effects on neural circuits and learning processes.
     * 
     * @return float Current neuromodulator concentration (typically normalized 0.0-1.0)
     * @post None
     * @note Level values are determined by biological processes, learning history,
     *       and environmental inputs. Different neuromodulators have different
     *       concentration ranges and physiological meanings.
     * @see setLevel(float)
     * @see getPlasticityFactor()
     * @see getName()
     */
    virtual float getLevel() const = 0;
    
    /**
     * @brief Set the concentration/level of the neuromodulator
     * 
     * Manually sets the neuromodulator concentration, typically used for
     * testing, simulation, or controlled experiments. In natural operation,
     * levels are determined by biological processes and environmental inputs.
     * 
     * @param level New neuromodulator concentration (typically 0.0-1.0 range)
     * @post Neuromulator level is updated to the specified value
     * @note This method overrides natural level determination. Use with caution
     *       as it may bypass important biological regulatory mechanisms.
     * @see getLevel()
     * @see applyToPlasticity()
     */
    virtual void setLevel(float level) = 0;
    
    /**
     * @brief Get the current plasticity modulation factor
     * 
     * Returns the factor by which this neuromodulator enhances or suppresses
     * synaptic plasticity (learning capability). Values > 1.0 enhance learning,
     * values < 1.0 suppress learning, and values = 1.0 have no effect.
     * 
     * @return float Plasticity modulation factor (0.0-2.0+ typical range)
     * @post None
     * @note Plasticity factors are neuromodulator-specific and context-dependent.
     *       Different neuromodulators have different effects on different types of
     *       plasticity (Hebbian vs. anti-Hebbian, structural vs. synaptic).
     * @see getLevel()
     * @see getName()
     */
    virtual float getPlasticityFactor() const = 0;
    
    /**
     * @brief Update the neuromodulator state over time
     * 
     * Advances the neuromodulator's internal state based on biological processes,
     * learning history, and environmental inputs. This includes production,
     * decay, feedback regulation, and interaction with other neuromodulators.
     * 
     * @param dt Time step duration for the update (in seconds)
     * @post Neuromodulator level and internal state are updated based on dt
     * @note Updates typically include:
     *       - Decay of current levels toward baseline
     *       - Production based on neural activity patterns
     *       - Feedback regulation from other systems
     *       - Interaction with other neuromodulators
     * @see getLevel()
     * @see update(TimestepDuration)
     */
    virtual void update(TimestepDuration dt) = 0;
    
protected:
    /**
     * @brief Protected default constructor
     * 
     * Default constructor is protected to prevent direct instantiation of
     * the abstract base class. Derived classes must be instantiated.
     */
    Neuromodulator() = default;
};

// Dopamine: Reward and reinforcement learning signal
// PLACEHOLDER - Phase 2
class Dopamine : public Neuromodulator {
public:
    Dopamine();
    ~Dopamine() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Reward signaling
    void signalReward(float reward);
    void signalRewardPredictionError(float error);
    
private:
    struct Impl;
    Impl* pImpl;
};

// Acetylcholine: Attention and memory consolidation
// PLACEHOLDER - Phase 2
class Acetylcholine : public Neuromodulator {
public:
    const char* getName() const override { return "ACh"; }
    float getLevel() const override { return 0.0f; }
    void setLevel(float level) override {}
    float getPlasticityFactor() const override { return 1.0f; }
    void update(TimestepDuration dt) override {}
};

// Norepinephrine: Arousal and vigilance
// PLACEHOLDER - Phase 2
class Norepinephrine : public Neuromodulator {
public:
    const char* getName() const override { return "NE"; }
    float getLevel() const override { return 0.0f; }
    void setLevel(float level) override {}
    float getPlasticityFactor() const override { return 1.0f; }
    void update(TimestepDuration dt) override {}
};

// Serotonin: Mood, impulsivity, and social behavior
// PLACEHOLDER - Phase 2
class Serotonin : public Neuromodulator {
public:
    const char* getName() const override { return "5-HT"; }
    float getLevel() const override { return 0.0f; }
    void setLevel(float level) override {}
    float getPlasticityFactor() const override { return 1.0f; }
    void update(TimestepDuration dt) override {}
};

} // namespace nlm
