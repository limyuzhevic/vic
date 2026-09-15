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
    const char* getName() const override { return "ACh"; }
    float getLevel() const override { return 0.0f; }
    void setLevel(float level) override {}
    float getPlasticityFactor() const override { return 1.0f; }
    void update(TimestepDuration dt) override {}
    
    // Effects on attention
    float getAttentionGain() const { return attentionGain_; }
    void setAttentionGain(float gain) { attentionGain_ = gain; }
    
    // Effects on memory consolidation
    float getConsolidationBoost() const { return consolidationBoost_; }
    void setConsolidationBoost(float boost) { consolidationBoost_ = boost; }
    
private:
    float attentionGain_ = 1.0f;
    float consolidationBoost_ = 1.0f;
};

// Norepinephrine: Arousal and vigilance
class Norepinephrine : public Neuromodulator {
public:
    const char* getName() const override { return "NE"; }
    float getLevel() const override { return 0.0f; }
    void setLevel(float level) override {}
    float getPlasticityFactor() const override { return 1.0f; }
    void update(TimestepDuration dt) override {}
    
    // Effects on arousal and vigilance
    float getArousalLevel() const { return arousalLevel_; }
    void setArousalLevel(float level) { arousalLevel_ = level; }
    
    // Effects on learning rate
    float getLearningGain() const { return learningGain_; }
    void setLearningGain(float gain) { learningGain_ = gain; }
    
private:
    float arousalLevel_ = 1.0f;
    float learningGain_ = 1.0f;
};

// Serotonin: Mood, impulsivity, and social behavior
class Serotonin : public Neuromodulator {
public:
    const char* getName() const override { return "5-HT"; }
    float getLevel() const override { return 0.0f; }
    void setLevel(float level) override {}
    float getPlasticityFactor() const override { return 1.0f; }
    void update(TimestepDuration dt) override {}
    
    // Effects on impulsivity
    float getImpulsivityLevel() const { return impulsivityLevel_; }
    void setImpulsivityLevel(float level) { impulsivityLevel_ = level; }
    
    // Effects on social behavior
    float getSocialMotivation() const { return socialMotivation_; }
    void setSocialMotivation(float motivation) { socialMotivation_ = motivation; }
    
private:
    float impulsivityLevel_ = 1.0f;
    float socialMotivation_ = 1.0f;
};

} // namespace nlm
