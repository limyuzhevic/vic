#pragma once

#include "../core/Types/Types.hpp"
#include <string>
#include <vector>

namespace nlm {

// Neuromodulator: Abstract base for neuromodulatory signals
// PLACEHOLDER - Phase 2 will implement real neuromodulation effects

class Neuromodulator {
public:
    virtual ~Neuromodulator() = default;
    
    /**
     * @brief Get modulator name
     * @return Human-readable name of the neuromodulator
     */
    virtual const char* getName() const = 0;
    
    /**
     * @brief Get current concentration/level
     * @return Current level normalized to [0, 1] range
     */
    virtual float getLevel() const = 0;
    virtual void setLevel(float level) = 0;
    
    /**
     * @brief Get plasticity modulation factor
     * @return Factor that multiplies plasticity learning rates
     * @note Higher values increase learning speed, lower values decrease it
     */
    virtual float getPlasticityFactor() const = 0;
    
    /**
     * @brief Update neuromodulator state
     * @param dt Timestep duration in milliseconds
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
     * @brief Signal reward reception
     * @param reward Reward value (typically positive)
     */
    void signalReward(float reward);
    
    /**
     * @brief Signal reward prediction error
     * @param error Prediction error (positive = reward higher than expected)
     */
    void signalRewardPredictionError(float error);
    
private:
    struct Impl;
    Impl* pImpl;
};

// Acetylcholine: Attention and memory consolidation neuromodulator
// PLACEHOLDER - Phase 2
class Acetylcholine : public Neuromodulator {
public:
    const char* getName() const override { return "ACh"; }
    float getLevel() const override { return 0.0f; }
    void setLevel(float level) override {}
    float getPlasticityFactor() const override { return 1.0f; }
    void update(TimestepDuration dt) override {}
};

// Norepinephrine: Arousal and vigilance neuromodulator
// PLACEHOLDER - Phase 2
class Norepinephrine : public Neuromodulator {
public:
    const char* getName() const override { return "NE"; }
    float getLevel() const override { return 0.0f; }
    void setLevel(float level) override {}
    float getPlasticityFactor() const override { return 1.0f; }
    void update(TimestepDuration dt) override {}
};

// Serotonin: Mood, impulsivity, and social behavior neuromodulator
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
