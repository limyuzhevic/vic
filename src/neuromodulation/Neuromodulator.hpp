#pragma once

#include "../core/Types/Types.hpp"
#include <string>
#include <vector>

namespace nlm {

// Neuromodulator: Abstract base for neuromodulatory signals
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
    
    // Memory consolidation effects
    void enhanceMemoryConsolidation(Brain* brain, TimestepDuration dt);
    
private:
    struct Impl;
    Impl* pImpl;
};

// Acetylcholine: Attention and memory consolidation system
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
    void enhanceAttention(Brain* brain, TimestepDuration dt);
    void regulateAttention(Brain* brain, TimestepDuration dt);
    
    // Memory effects
    void enhanceMemoryEncoding(Brain* brain, TimestepDuration dt);
    void enhanceMemoryRetrieval(Brain* brain, TimestepDuration dt);
    
private:
    struct Impl;
    Impl* pImpl;
};

// Norepinephrine: Arousal and vigilance system
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
    void modulateArousal(Brain* brain, TimestepDuration dt);
    void enhanceSensoryProcessing(Brain* brain, TimestepDuration dt);
    
    // Learning effects
    void enhanceNoveltyEncoding(Brain* brain, TimestepDuration dt);
    
private:
    struct Impl;
    Impl* pImpl;
};

// Serotonin: Mood, impulsivity, and social behavior system
class Serotonin : public Neuromodulator {
public:
    Serotonin();
    ~Serotonin() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Behavioral regulation
    void regulateImpulsivity(Brain* brain, TimestepDuration dt);
    void modulateMood(Brain* brain, TimestepDuration dt);
    
    // Learning effects
    void enhancePatternCompletion(Brain* brain, TimestepDuration dt);
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
