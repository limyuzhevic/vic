#pragma once

#include "../core/Types/Types.hpp"
#include <string>
#include <vector>

namespace nlm {

// Neuromodulation system: Abstract base for neuromodulatory signals
// PLACEHOLDER - Phase 2 will implement real neuromodulation effects

class Neuromodulator {
public:
    virtual ~Neuromodulator() = default;
    
    /**
     * @brief Get the name of this neuromodulator
     * 
     * @return Human-readable name (e.g., "DA", "ACh", "NE", "5-HT")
     */
    virtual const char* getName() const = 0;
    
    /**
     * @brief Get the current concentration/level of the neuromodulator
     * 
     * @return Current level (typically 0.0 to 1.0)
     */
    virtual float getLevel() const = 0;
    
    /**
     * @brief Set the concentration/level of the neuromodulator
     * 
     * @param level New level (will be clamped to valid range)
     */
    virtual void setLevel(float level) = 0;
    
    /**
     * @brief Apply neuromodulatory effect to plasticity
     * 
     * @return Plasticity modulation factor (multiplies learning rate)
     */
    virtual float getPlasticityFactor() const = 0;
    
    /**
     * @brief Update neuromodulator state based on time step
     * 
     * @param dt Time step duration
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
    
    /**
     * @brief Signal a reward event (dopamine burst)
     * 
     * @param reward Reward value (positive for rewards)
     */
    void signalReward(float reward);
    
    /**
     * @brief Signal a reward prediction error
     * 
     * @param error Prediction error (positive = better than expected)
     */
    void signalRewardPredictionError(float error);
    
private:
    struct Impl;
    Impl* pImpl;
};

// Acetylcholine: Attention and memory consolidation
// PLACEHOLDER - Phase 2
class Acetylcholine : public Neuromodulator {
public:
    /**
     * @brief Get the neuromodulator name
     * 
     * @return Always "ACh"
     */
    const char* getName() const override { return "ACh"; }
    
    /**
     * @brief Get current level (always 0.0)
     * 
     * @return 0.0 (placeholder implementation)
     */
    float getLevel() const override { return 0.0f; }
    
    /**
     * @brief Set level (does nothing in placeholder)
     * 
     * @param level Ignored
     */
    void setLevel(float level) override {}
    
    /**
     * @brief Get plasticity factor (always 1.0)
     * 
     * @return 1.0 (no modulation in placeholder)
     */
    float getPlasticityFactor() const override { return 1.0f; }
    
    /**
     * @brief Update (does nothing in placeholder)
     * 
     * @param dt Ignored
     */
    void update(TimestepDuration dt) override {}
};

// Norepinephrine: Arousal and vigilance
// PLACEHOLDER - Phase 2
class Norepinephrine : public Neuromodulator {
public:
    /**
     * @brief Get the neuromodulator name
     * 
     * @return Always "NE"
     */
    const char* getName() const override { return "NE"; }
    
    /**
     * @brief Get current level (always 0.0)
     * 
     * @return 0.0 (placeholder implementation)
     */
    float getLevel() const override { return 0.0f; }
    
    /**
     * @brief Set level (does nothing in placeholder)
     * 
     * @param level Ignored
     */
    void setLevel(float level) override {}
    
    /**
     * @brief Get plasticity factor (always 1.0)
     * 
     * @return 1.0 (no modulation in placeholder)
     */
    float getPlasticityFactor() const override { return 1.0f; }
    
    /**
     * @brief Update (does nothing in placeholder)
     * 
     * @param dt Ignored
     */
    void update(TimestepDuration dt) override {}
};

// Serotonin: Mood, impulsivity, and social behavior
// PLACEHOLDER - Phase 2
class Serotonin : public Neuromodulator {
public:
    /**
     * @brief Get the neuromodulator name
     * 
     * @return Always "5-HT"
     */
    const char* getName() const override { return "5-HT"; }
    
    /**
     * @brief Get current level (always 0.0)
     * 
     * @return 0.0 (placeholder implementation)
     */
    float getLevel() const override { return 0.0f; }
    
    /**
     * @brief Set level (does nothing in placeholder)
     * 
     * @param level Ignored
     */
    void setLevel(float level) override {}
    
    /**
     * @brief Get plasticity factor (always 1.0)
     * 
     * @return 1.0 (no modulation in placeholder)
     */
    float getPlasticityFactor() const override { return 1.0f; }
    
    /**
     * @brief Update (does nothing in placeholder)
     * 
     * @param dt Ignored
     */
    void update(TimestepDuration dt) override {}
};

} // namespace nlm
