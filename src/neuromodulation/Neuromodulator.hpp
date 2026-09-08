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
// Real biological mechanism for attentional modulation
class Acetylcholine : public Neuromodulator {
public:
    Acetylcholine();
    ~Acetylcholine() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Attention modulation
    void enhanceAttention(float area);
    float getAttentionGain() const;
    
    // Working memory modulation
    void strengthenWorkingMemoryTrace(float traceId, float strength);
    float getWorkingMemoryStrength() const;
    
    // Signal-to-noise enhancement
    float getSignalToNoiseRatio() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

// Norepinephrine: Arousal and vigilance
// Real biological mechanism for arousal and stress response
class Norepinephrine : public Neuromodulator {
public:
    Norepinephrine();
    ~Norepinephrine() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Arousal modulation
    void enhanceArousal(float intensity);
    float getArousalLevel() const;
    
    // Vigilance modulation
    void increaseVigilance(float amount);
    float getVigilance() const;
    
    // Plasticity during novelty
    void boostPlasticityDuringNovelty(float novelty);
    float getPlasticityBoost() const;
    
    // Response inhibition
    void inhibitImpulsiveResponses();
    bool getResponseInhibition() const;
    
    // Response speed
    float getResponseSpeed() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

// Serotonin: Mood, impulsivity, and social behavior
// Real biological mechanism for mood regulation and impulse control
class Serotonin : public Neuromodulator {
public:
    Serotonin();
    ~Serotonin() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Mood modulation
    void improveMood(float amount);
    float getMood() const;
    
    // Impulsivity control
    void enhancePatience(float amount);
    void decreaseImpulsivity(float amount);
    float getPatience() const;
    float getImpulsivity() const;
    
    // Delayed gratification
    void enhanceDelayedGratisfaction(float amount);
    float getDelayedGratisfaction() const;
    
    // Response inhibition
    void inhibitReactiveResponses();
    float getInhibitionStrength() const;
    float getReactiveResponseLevel() const;
    
    // Reward processing
    float getRewardProcessing() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
