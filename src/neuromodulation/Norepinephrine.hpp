#pragma once

#include "../core/Types/Types.hpp"

namespace nlm {

// Acetylcholine: Attention and memory consolidation
class Acetylcholine {
public:
    Acetylcholine();
    ~Acetylcholine();
    
    // Initialize with brain reference
    void initialize(class Brain* brain);
    
    // Get ACh level
    float getLevel() const;
    void setLevel(float level);
    
    // Get ACh-modulated plasticity factor
    float getPlasticityFactor() const;
    
    // Update ACh state
    void update(TimestepDuration dt);
    
    // Signal attention
    void signalAttention(float attentionStrength);
    void signalMemoryEvent(float memoryStrength);
    
    // Get attention modulation
    float getAttentionModulation() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

// Norepinephrine: Arousal and vigilance
class Norepinephrine {
public:
    Norepinephrine();
    ~Norepinephrine();
    
    // Initialize with brain reference
    void initialize(class Brain* brain);
    
    // Get NE level
    float getLevel() const;
    void setLevel(float level);
    
    // Get NE-modulated plasticity factor
    float getPlasticityFactor() const;
    
    // Update NE state
    void update(TimestepDuration dt);
    
    // Signal arousal
    void signalArousal(float arousalStrength);
    void signalVigilance(float vigilanceStrength);
    
    // Get sensory gain modulation
    float getSensoryGain() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

// Serotonin: Mood, impulsivity, and social behavior
class Serotonin {
public:
    Serotonin();
    ~Serotonin();
    
    // Initialize with brain reference
    void initialize(class Brain* brain);
    
    // Get 5-HT level
    float getLevel() const;
    void setLevel(float level);
    
    // Get 5-HT-modulated plasticity factor
    float getPlasticityFactor() const;
    
    // Update 5-HT state
    void update(TimestepDuration dt);
    
    // Signal mood
    void signalMood(float moodStrength);
    void signalSocialBehavior(float socialStrength);
    
    // Get learning rate modulation
    float getLearningRateModulation() const;
    float getImpulsivityFactor() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm