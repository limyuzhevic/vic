#pragma once

#include "../core/Types/Types.hpp"
#include <string>
#include <vector>

namespace nlm {

// Neuromodulator: Abstract base for neuromodulatory signals
// Real neuromodulation affects plasticity, attention, arousal, and learning

class Neuromodulator {
public:
    virtual ~Neuromodulator() = default;
    
    // Get modulator name
    virtual const char* getName() const = 0;
    
    // Get current concentration/level
    virtual float getLevel() const = 0;
    virtual void setLevel(float level) = 0;
    
    // Apply neuromodulatory effect to plasticity
    virtual float getPlasticityFactor() const = 0;
    
    // Update neuromodulator state
    virtual void update(TimestepDuration dt) = 0;
    
protected:
    Neuromodulator() = default;
};

// Dopamine: Reward and reinforcement learning signal
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

// Serotonin: Aversion and behavioral inhibition
class Serotonin : public Neuromodulator {
public:
    Serotonin();
    ~Serotonin() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Aversion signaling
    void signalReward(float reward);
    void signalRewardPredictionError(float error);
    void signalAversion(float stimulus);
    
private:
    struct Impl;
    Impl* pImpl;
};

// Norepinephrine: Arousal and attention modulation
class Norepinephrine : public Neuromodulator {
public:
    Norepinephrine();
    ~Norepinephrine() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getArousalModifier() const;
    float getAttentionBias() const;
    void update(TimestepDuration dt) override;
    
    // Arousal signaling
    void signalNovelty(float novelty);
    void signalStress(float stress);
    void signalReward(float reward);
    
private:
    struct Impl;
    Impl* pImpl;
};

// Acetylcholine: Attention and memory modulation
class Acetylcholine : public Neuromodulator {
public:
    Acetylcholine();
    ~Acetylcholine() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getMemoryModulator() const;
    float getAttentionGain() const;
    float getCorticalActivator() const;
    void update(TimestepDuration dt) override;
    
    // Attention and memory signaling
    void signalNovelty(float novelty);
    void signalAttention(float focus);
    void signalMemory(float memoryLoad);
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm

} // namespace nlm
