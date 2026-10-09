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
    
    // Get modulator name
    virtual const char* getName() const = 0;
    
    // Get current concentration/level
    virtual float getLevel() const = 0;
    virtual void setLevel(float level) = 0;
    
    // Apply neuromodulatory effect to plasticity
    // TODO PHASE 2: Implement real modulation
    virtual float getPlasticityFactor() const = 0;
    
    // Update neuromodulator state
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
    Acetylcholine();
    ~Acetylcholine() override;
    
    const char* getName() const override { return "ACh"; }
    
    // Get current concentration/level
    float getLevel() const override;
    void setLevel(float level) override;
    
    // Apply neuromodulatory effect to plasticity
    // TODO PHASE 2: Implement real modulation
    float getPlasticityFactor() const override;
    
    // Update neuromodulator state
    void update(TimestepDuration dt) override;
    
    // Memory consolidation effect
    void consolidateMemory(float consolidationFactor);
    
    // Attention enhancement
    void enhanceAttention(float attentionBoost);
    
    // Reset neuromodulator state
    void reset();
    
private:
    struct Impl;
    Impl* pImpl;
};

// Norepinephrine: Arousal and vigilance
// PLACEHOLDER - Phase 2
class Norepinephrine : public Neuromodulator {
public:
    Norepinephrine();
    ~Norepinephrine() override;
    
    const char* getName() const override { return "NE"; }
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Arousal effect
    void enhanceArousal(float arousalBoost);
    
    // Alertness modulation
    void modulateAlertness(float alertness);
    
    // Reset neuromodulator state
    void reset();
    
private:
    struct Impl;
    Impl* pImpl;
};

// Serotonin: Mood, impulsivity, and social behavior
// PLACEHOLDER - Phase 2
class Serotonin : public Neuromodulator {
public:
    Serotonin();
    ~Serotonin() override;
    
    const char* getName() const override { return "5-HT"; }
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Social behavior modulation
    void promoteSocialBehavior(float socialFactor);
    
    // Impulse control
    void enhanceImpulseControl(float controlFactor);
    
    // Reset neuromodulator state
    void reset();
    
private:
    struct Impl;
    Impl* pImpl;
};

// Generic Neuromodulator reset function
inline void resetAllNeuromodulators(nlm::Dopamine* dopamine,
                                   nlm::Acetylcholine* acetylcholine,
                                   nlm::Norepinephrine* norepinephrine,
                                   nlm::Serotonin* serotonin,
                                   nlm::Curiosity* curiosity,
                                   nlm::Novelty* novelty,
                                   nlm::PredictionError* predictionError) {
    if (dopamine) dopamine->reset();
    if (acetylcholine) acetylcholine->reset();
    if (norepinephrine) norepinephrine->reset();
    if (serotonin) serotonin->reset();
    if (curiosity) curiosity->reset();
    if (novelty) novelty->reset();
    if (predictionError) predictionError->reset();
}
