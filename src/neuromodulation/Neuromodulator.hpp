#pragma once

#include "../core/Types/Types.hpp"
#include <string>
#include <vector>
#include "../plasticity/STDP.hpp"
#include "../plasticity/Hebbian.hpp"

namespace nlm {

// Neuromodulator: Abstract base for neuromodulatory signals
// Phase 2: Real neuromodulation effects with proper implementation

class Neuromodulator {
public:
    virtual ~Neuromodulator() = default;
    
    // Get modulator name
    virtual const char* getName() const = 0;
    
    // Get current concentration/level
    virtual float getLevel() const = 0;
    virtual void setLevel(float level) = 0;
    
    // Apply neuromodulatory effect to plasticity
    // Returns a factor that multiplies baseline plasticity
    virtual float getPlasticityFactor() const = 0;
    
    // Update neuromodulator state based on neural activity
    virtual void update(TimestepDuration dt) = 0;
    
    // Apply neuromodulatory effects to plasticity rules
    // Modifies the learning rates and eligibility traces of plasticity rules
    virtual void applyToPlasticity(STDP* stdp, Hebbian* hebbian) = 0;
    
protected:
    Neuromodulator() = default;
};

// Dopamine: Reward and reinforcement learning signal
// Phase 2: Real reward processing
class Dopamine : public Neuromodulator {
public:
    Dopamine();
    ~Dopamine() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    void applyToPlasticity(STDP* stdp, Hebbian* hebbian) override;
    
    // Reward signaling
    void signalReward(float reward);
    void signalRewardPredictionError(float error);
    
    // Get dopamine level
    float getValue() const { return getLevel(); }
    
private:
    struct Impl;
    Impl* pImpl;
};

// Acetylcholine: Attention and memory consolidation
// Phase 2: Real attention and memory modulation
class Acetylcholine : public Neuromodulator {
public:
    Acetylcholine();
    ~Acetylcholine() override;
    
    const char* getName() const override { return "ACh"; }
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    void applyToPlasticity(STDP* stdp, Hebbian* hebbian) override;
    
    // Attention modulation
    void modulateAttention(float attentionBias);
    void enhanceMemoryConsolidation();
    
private:
    struct Impl;
    Impl* pImpl;
};

// Norepinephrine: Arousal and vigilance
// Phase 2: Real arousal and alertness modulation
class Norepinephrine : public Neuromodulator {
public:
    Norepinephrine();
    ~Norepinephrine() override;
    
    const char* getName() const override { return "NE"; }
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    void applyToPlasticity(STDP* stdp, Hebbian* hebbian) override;
    
    // Arousal modulation
    void increaseAlertness(float level);
    void modulateSensoryGain(float gainFactor);
    
private:
    struct Impl;
    Impl* pImpl;
};

// Serotonin: Mood, impulsivity, and social behavior
// Phase 2: Real mood and behavioral modulation
class Serotonin : public Neuromodulator {
public:
    Serotonin();
    ~Serotonin() override;
    
    const char* getName() const override { return "5-HT"; }
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    void applyToPlasticity(STDP* stdp, Hebbian* hebbian) override;
    
    // Mood and impulsivity modulation
    void modulateMood(float moodLevel);
    void decreaseImpulsivity(float level);
    void enhanceSocialBehavior(float socialFactor);
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
