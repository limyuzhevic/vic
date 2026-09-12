#pragma once

#include "../core/Types/Types.hpp"
#include <string>
#include <vector>

namespace nlm {

// Neuromodulator: Abstract base for neuromodulatory signals
/**
 * @class Neuromodulator
 * @brief Abstract base class for neuromodulators (dopamine, curiosity, novelty, etc.)
 * 
 * Neuromodulators are chemical signals that modulate neural activity and plasticity
 * throughout the brain. They provide teaching signals for learning and help regulate
 * attention, arousal, reward prediction, and exploration behaviors.
 * 
 * Neuromodulators affect:
 * - Neural excitability and firing rates
 * - Synaptic plasticity (learning)
 * - Memory consolidation
 * - Attention and focus
 * - Motivation and exploration
 * 
 * @note Phase 2: Implementation ongoing. Current placeholder implementations
 *       need proper biological mechanisms and integration with brain systems.
 * 
 * @author NLM Development Team
 * @date Phase 2 Implementation
 */
class Neuromodulator {
public:
    /**
     * @brief Virtual destructor
     */
    virtual ~Neuromodulator() = default;
    
    /**
     * @brief Get neuromodulator name
     * 
     * @return Human-readable name of neuromodulator
     */
    virtual const char* getName() const = 0;
    
    /**
     * @brief Get current concentration/level of neuromodulator
     * 
     * @return Current level (typically normalized 0-1)
     */
    virtual float getLevel() const = 0;
    
    /**
     * @brief Set neuromodulator level
     * 
     * @param level New level (typically normalized 0-1)
     */
    virtual void setLevel(float level) = 0;
    
    /**
     * @brief Get plasticity factor influenced by neuromodulator
     * 
     * @return Factor to multiply synaptic plasticity rates by
     *         (1.0 = normal, >1.0 = enhanced, <1.0 = suppressed)
     */
    virtual float getPlasticityFactor() const = 0;
    
    /**
     * @brief Update neuromodulator state based on neural activity
     * 
     * @param dt Timestep duration
     */
    virtual void update(TimestepDuration dt) = 0;
    
protected:
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
