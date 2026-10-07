#pragma once

#include "../core/Types/Types.hpp"
#include "../core/Logger/Logger.hpp"

namespace nlm {

// Serotonin: Mood regulation, impulsivity, and social behavior
class Serotonin {
public:
    Serotonin();
    ~Serotonin();
    
    // Initialize with brain reference
    void initialize(class Brain* brain);
    
    // Get serotonin level
    float getLevel() const;
    void setLevel(float level);
    
    // Update serotonin based on mood state and social interactions
    void update(float moodState, float socialInteraction, TimestepDuration dt);
    
    // Get effect on impulsivity (higher serotonin = lower impulsivity)
    float getImpulsivityEffect() const;
    
    // Get effect on social bonding/behavior
    float getSocialBondingEffect() const;
    
    // Set parameters
    void setMoodWeight(float weight);
    void setSocialWeight(float weight);
    
    // Reset
    void reset();
    
private:
    struct Impl;
    Impl* pImpl;
};

// Norepinephrine: Arousal, vigilance, and attention modulation
class Norepinephrine {
public:
    Norepinephrine();
    ~Norepinephrine();
    
    // Initialize with brain reference
    void initialize(class Brain* brain);
    
    // Get norepinephrine level
    float getLevel() const;
    void setLevel(float level);
    
    // Update norepinephrine based on arousal and attention demands
    void update(float arousalLevel, float attentionDemands, TimestepDuration dt);
    
    // Get effect on signal detection (higher NE = better signal detection)
    float getSignalDetectionEffect() const;
    
    // Get effect on working memory (inverted U-curve relationship)
    float getWorkingMemoryEffect() const;
    
    // Set parameters
    void setArousalWeight(float weight);
    void setAttentionWeight(float weight);
    
    // Reset
    void reset();
    
private:
    struct Impl;
    Impl* pImpl;
};

// Acetylcholine: Attention, memory encoding, and cortical activation
class Acetylcholine {
public:
    Acetylcholine();
    ~Acetylcholine();
    
    // Initialize with brain reference
    void initialize(class Brain* brain);
    
    // Get acetylcholine level
    float getLevel() const;
    void setLevel(float level);
    
    // Update acetylcholine based on attention demands and memory encoding needs
    void update(float attentionDemands, float memoryEncodingNeeds, TimestepDuration dt);
    
    // Get effect on cortical activation
    float getCorticalActivationEffect() const;
    
    // Get effect on memory encoding
    float getMemoryEncodingEffect() const;
    
    // Get effect on attention filtering
    float getAttentionFilteringEffect() const;
    
    // Set parameters
    void setAttentionWeight(float weight);
    void setMemoryWeight(float weight);
    
    // Reset
    void reset();
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
