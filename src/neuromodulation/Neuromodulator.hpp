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
    
    // Additional dopamine effects
    float getBaseline() const { return pImpl->baseline; }
    void setBaseline(float baseline) { pImpl->baseline = baseline; }
    float getPeak() const { return pImpl->peak; }
    void setPeak(float peak) { pImpl->peak = peak; }
    
private:
    struct Impl;
    Impl* pImpl;
};

// Acetylcholine: Attention and memory consolidation
class Acetylcholine : public Neuromodulator {
public:
    Acetylcholine();
    ~Acetylcholine() override;
    
    const char* getName() const override { return "ACh"; }
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Attention enhancement
    void enhanceAttention(float stimulusSalience);
    void triggerMemoryConsolidation();
    
    // Get current attention and memory consolidation state
    float getAttentionGain() const { return pImpl->attentionGain; }
    float getMemoryConsolidation() const { return pImpl->memoryConsolidation; }
    
private:
    struct Impl;
    Impl* pImpl;
};

// Norepinephrine: Arousal and vigilance
class Norepinephrine : public Neuromodulator {
public:
    Norepinephrine();
    ~Norepinephrine() override;
    
    const char* getName() const override { return "NE"; }
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Arousal enhancement
    void signalStress(float stressLevel);
    void enhanceVigilance(float unexpectedStimulus);
    
    // Get current arousal and vigilance state
    float getArousal() const { return pImpl->arousal; }
    float getVigilance() const { return pImpl->vigilance; }
    
private:
    struct Impl;
    Impl* pImpl;
};

// Serotonin: Mood, impulsivity, and social behavior
class Serotonin : public Neuromodulator {
public:
    Serotonin();
    ~Serotonin() override;
    
    const char* getName() const override { return "5-HT"; }
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Mood enhancement
    void signalPositiveEvent(float reward);
    void signalSocialInteraction(float socialQuality);
    
    // Get current mood and behavior state
    float getMood() const { return pImpl->mood; }
    float getImpulseControl() const { return pImpl->impulseControl; }
    float getSocialBehavior() const { return pImpl->socialBehavior; }
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
