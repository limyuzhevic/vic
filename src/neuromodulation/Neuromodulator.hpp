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
class Acetylcholine : public Neuromodulator {
public:
    Acetylcholine();
    ~Acetylcholine() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // ACh-specific functions for attention and working memory
    float getAttentionModulator() const;
    void signalAttention(bool focusEvent);
    float getMemoryConsolidationFactor() const;
    void promoteMemoryConsolidation(int neuralTraceId);
    
    // ACh effects on cortical networks
    void enhanceSensoryProcessing() const;
    void suppressBackgroundActivity() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

// Norepinephrine: Arousal and vigilance
class Norepinephrine : public Neuromodulator {
public:
    Norepinephrine();
    ~Norepinephrine() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // NE-specific functions for arousal and vigilance
    float getArousalLevel() const;
    void signalArousal(float intensity, bool novelEvent);
    float getVigilanceModulator() const;
    void maintainVigilance(bool isAlertState);
    float getLocusCoeruleusGain() const;
    
    // NE effects on global brain state
    void increaseAlertness() const;
    void enhanceSignalToNoiseRatio() const;
    void prepareForAction() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

// Serotonin: Mood, motivation, and social behavior
class Serotonin : public Neuromodulator {
public:
    Serotonin();
    ~Serotonin() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // 5-HT-specific functions for mood and motivation
    float getMoodLevel() const;
    void signalMoodChange(float moodDelta, bool positiveEvent);
    float getMotivationModulator() const;
    void modulateMotivation(bool isRewardingContext);
    float getSocialBehaviorModulator() const;
    void influenceSocialBehavior(float socialContext);
    
    // 5-HT effects on behavioral state
    void improveMood() const;
    void modulateRewardProcessing() const;
    void regulateImpulsivity() const;
    void enhanceSocialLearning() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
