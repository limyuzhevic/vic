#pragma once

#include "../core/Types/Types.hpp"
#include "../brain/Brain.hpp"
#include <string>
#include <vector>
#include <memory>

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
    
    // Effect on attention
    virtual void applyAttentionEffect(Brain* brain) = 0;
    
    // Effect on memory
    virtual void applyMemoryEffect(Brain* brain) = 0;
    
    // Effect on arousal/behavior
    virtual void applyArousalEffect(Brain* brain) = 0;
    
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
    
    // Apply attention effects (dopamine modulates attention based on prediction error)
    void applyAttentionEffect(Brain* brain) override;
    
    // Apply memory effects (dopamine enhances reward-related memory consolidation)
    void applyMemoryEffect(Brain* brain) override;
    
    // Apply arousal effects (dopamine increases arousal and exploration)
    void applyArousalEffect(Brain* brain) override;
    
    // Reward signaling
    void signalReward(float reward);
    void signalRewardPredictionError(float error);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
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
    
    // Apply attention effects (ACh enhances attention focus and top-down bias)
    void applyAttentionEffect(Brain* brain) override;
    
    // Apply memory effects (ACh enhances memory consolidation during encoding)
    void applyMemoryEffect(Brain* brain) override;
    
    // Apply arousal effects (ACh modulates wakefulness and vigilance)
    void applyArousalEffect(Brain* brain) override;
    
    // Focus attention on specific regions
    void focusAttention(RegionId region, float strength);
    
    // Enhance memory encoding
    void enhanceEncoding(const std::vector<float>& pattern, float reward);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
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
    
    // Apply attention effects (NE increases signal-to-noise ratio)
    void applyAttentionEffect(Brain* brain) override;
    
    // Apply memory effects (NE enhances consolidation of salient events)
    void applyMemoryEffect(Brain* brain) override;
    
    // Apply arousal effects (NE increases arousal, attention, and readiness)
    void applyArousalEffect(Brain* brain) override;
    
    // Increase vigilance and alertness
    void increaseVigilance(float amount);
    
    // Enhance encoding of novel/emerging stimuli
    void enhanceNoveltyEncoding(const std::vector<float>& pattern);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
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
    
    // Apply attention effects (serotonin modulates impulsivity and decision time)
    void applyAttentionEffect(Brain* brain) override;
    
    // Apply memory effects (serotonin influences social memory and context)
    void applyMemoryEffect(Brain* brain) override;
    
    // Apply arousal effects (serotonin regulates mood and behavioral inhibition)
    void applyArousalEffect(Brain* brain) override;
    
    // Modulate social behavior
    void modulateSocialBehavior(Brain* brain, float socialSalience);
    
    // Enhance associative memory for social patterns
    void enhanceSocialMemory(const std::vector<float>& pattern, float socialValue);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
