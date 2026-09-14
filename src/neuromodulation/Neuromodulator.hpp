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
// PLACEHOLDER - Phase 2
class Acetylcholine : public Neuromodulator {
public:
    Acetylcholine();
    ~Acetylcholine() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Initialize with brain reference
    void initialize(Brain* brain);
    
    // Attention signaling
    void signalAttention(float attentionLevel);
    
    // Working memory modulation
    float getWorkingMemoryGain() const;
    void setWorkingMemoryGain(float gain);
    
    // Attention gain control
    float getAttentionGain() const;
    void setAttentionGain(float gain);
    
private:
    struct Impl;
    Impl* pImpl;
};

// Norepinephrine: Arousal and vigilance
// PLACEHOLDER - Phase 2
class Norepinephrine : public Neuromodulator {
public:
    Norepinephrine();
    ~Norepinephrine() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Initialize with brain reference
    void initialize(Brain* brain);
    
    // Arousal signaling
    void signalArousal(float arousalLevel);
    
    // Arousal gain control
    float getArousalGain() const;
    void setArousalGain(float gain);
    
    // Signal detection gain
    float getSignalDetectionGain() const;
    void setSignalDetectionGain(float gain);
    
private:
    struct Impl;
    Impl* pImpl;
};

// Serotonin: Mood, impulsivity, and social behavior
// PLACEHOLDER - Phase 2
class Serotonin : public Neuromodulator {
public:
    Serotonin();
    ~Serotonin() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Initialize with brain reference
    void initialize(Brain* brain);
    
    // Mood signaling
    void signalMood(float moodLevel, float valence, float impulsivity);
    
    // Emotional state tracking
    float getEmotionalValence() const;
    float getImpulsivity() const;
    void setEmotionalValence(float valence);
    void setImpulsivity(float impulsivity);
    
private:
    struct Impl;
    Impl* pImpl;
};

// Oxytocin: Social bonding, trust, and social learning
// PLACEHOLDER - Phase 2
class Oxytocin : public Neuromodulator {
public:
    Oxytocin();
    ~Oxytocin() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Initialize with brain reference
    void initialize(Brain* brain);
    
    // Social bonding signaling
    void signalSocialBonding(float socialCohesion);
    
    // Trust signaling  
    void signalTrust(float trustLevel);
    
    // Social history and trust tracking
    float getSocialBondHistory(size_t index) const;
    float getTrustScore() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
