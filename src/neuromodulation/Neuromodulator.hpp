#pragma once

#include "../core/Types/Types.hpp"
#include <string>
#include <vector>

namespace nlm {

// Neuromodulator: Abstract base for neuromodulatory signals
// Provides interface for neuromodulator implementations

class Neuromodulator {
public:
    virtual ~Neuromodulator() = default;
    
    // Get modulator name
    virtual const char* getName() const = 0;
    
    // Get current concentration/level
    virtual float getLevel() const = 0;
    virtual void setLevel(float level) = 0;
    
    // Apply neuromodulatory effect to plasticity
    // Virtual plasticity factor based on neuromodulator level
    virtual float getPlasticityFactor() const = 0;
    
    // Update neuromodulator state based on time
    virtual void update(TimestepDuration dt) = 0;
    
    // Get exploration value for this neuromodulator
    virtual float getExplorationValue() const { return 0.0f; }
    
protected:
    Neuromodulator() = default;
};

// Dopamine: Reward and reinforcement learning signal
// Real dopamine dynamics with temporal filtering, phasic/tonic signaling, and prediction error
class Dopamine : public Neuromodulator {
public:
    Dopamine();
    ~Dopamine() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    
    // Mode control
    void setMode(float mode);  // 0.0f = tonic only, 1.0f = phasic only
    void setPhasicGain(float gain);
    void setTonicGain(float gain);
    void setTimeConstant(float tc);
    void setLearningRate(float lr);
    
    // Get current neuromodulator levels
    float getPhasicComponent() const;
    float getTonicComponent() const;
    float getPredictionError() const;
    float getRewardPredictionError() const;
    
    // Plasticity factor computation
    float getPlasticityFactor() const override;
    float getPlasticityModulation() const;
    
    // Reward signaling
    void signalReward(float reward);
    void signalRewardPredictionError(float error);
    void signalNovelty(float novelty);
    
    // Update neuromodulator state
    void update(TimestepDuration dt) override;
    
    // Reward history tracking
    const std::vector<float>& getRewardHistory() const;
    void clearHistory();
    
    // Exploration value
    float getExplorationValue() const;
    
private:
    struct Impl;
    Impl* pImpl;
    
    // Helper method to update components
    void updateComponents();
};

class Acetylcholine : public Neuromodulator {
public:
    Acetylcholine();
    ~Acetylcholine() override;
    
    const char* getName() const override { return "ACh"; }
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Memory consolidation
    void signalAttention(float attentionSignal);
    void consolidateMemory(const std::vector<float>& memoryTrace);
    
    // Get attention/memory values
    float getAttentionValue() const;
    float getMemoryConsolidationValue() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

// Norepinephrine: Arousal and vigilance
// Real NE dynamics with stress and arousal responses
class Norepinephrine : public Neuromodulator {
public:
    Norepinephrine();
    ~Norepinephrine() override;
    
    const char* getName() const override { return "NE"; }
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Arousal and stress modulation
    void signalArousal(float arousalLevel);
    void signalVigilance(float vigilanceLevel);
    void signalStress(float stressLevel);
    
    // Get NE values
    float getArousal() const;
    float getVigilance() const;
    float getStressLevel() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

// Serotonin: Mood, impulsivity, and social behavior
// Real 5-HT dynamics with mood and social modulation
class Serotonin : public Neuromodulator {
public:
    Serotonin();
    ~Serotonin() override;
    
    const char* getName() const override { return "5-HT"; }
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Mood and social modulation
    void signalMood(float moodValue);
    void signalSocialBehavior(float socialValue);
    void signalImpulse(float impulseValue);
    
    // Get 5-HT values
    float getMood() const;
    float getSocialBehavior() const;
    float getImpulseControl() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
