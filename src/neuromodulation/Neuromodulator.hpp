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
    
void signalRewardPredictionError(float error);
    
    // Enhance attention (for Acetylcholine)
    void enhanceAttention(float strength);
    
    // Promote memory consolidation (for Acetylcholine)
    void promoteMemoryConsolidation(float strength);
    
    // Enhance arousal (for Norepinephrine)
    void enhanceArousal(float strength);
    
    // Increase vigilance (for Norepinephrine)
    void increaseVigilance(float strength);
    
    // Enhance mood (for Serotonin)
    void enhanceMood(float strength);
    
    // Reduce impulsivity (for Serotonin)
    void reduceImpulsivity(float strength);
    
    // Enhance social behavior (for Serotonin)
    void enhanceSocialBehavior(float strength);
    
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
    
    // Enhance attention
    void enhanceAttention(float strength) {
        // TODO PHASE 2: Implement real attention enhancement
        // PLACEHOLDER: Increase attention focus
        setLevel(std::min(1.0f, getLevel() + strength * 1.0f));
    }
    
    // Promote memory consolidation
    void promoteMemoryConsolidation(float strength) {
        // TODO PHASE 2: Implement real memory consolidation enhancement
        // PLACEHOLDER: Enhance memory consolidation
        setLevel(std::min(1.0f, getLevel() + strength * 1.0f));
    }
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
    
    // Enhance arousal
    void enhanceArousal(float strength) {
        // TODO PHASE 2: Implement real arousal enhancement
        // PLACEHOLDER: Increase arousal and vigilance
        setLevel(std::min(1.0f, getLevel() + strength * 1.0f));
    }
    
    // Increase vigilance
    void increaseVigilance(float strength) {
        // TODO PHASE 2: Implement real vigilance enhancement
        // PLACEHOLDER: Increase alertness and focus
        setLevel(std::min(1.0f, getLevel() + strength * 1.0f));
    }
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
    
    // Enhance mood
    void enhanceMood(float strength) {
        // TODO PHASE 2: Implement real mood enhancement
        // PLACEHOLDER: Increase mood and positive affect
        setLevel(std::min(1.0f, getLevel() + strength * 1.0f));
    }
    
    // Reduce impulsivity
    void reduceImpulsivity(float strength) {
        // TODO PHASE 2: Implement real impulsivity reduction
        // PLACEHOLDER: Reduce impulsive behavior
        setLevel(std::min(1.0f, getLevel() + strength * 1.0f));
    }
    
    // Enhance social behavior
    void enhanceSocialBehavior(float strength) {
        // TODO PHASE 2: Implement real social behavior enhancement
        // PLACEHOLDER: Enhance social engagement
        setLevel(std::min(1.0f, getLevel() + strength * 1.0f));
    }
};

} // namespace nlm
