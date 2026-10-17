#pragma once

#include "../core/Types/Types.hpp"
#include <string>
#include <vector>

namespace nlm {

// Acetylcholine: Attention and memory consolidation
class Acetylcholine {
public:
    Acetylcholine();
    ~Acetylcholine();
    
    // Initialize with brain reference
    void initialize(class Brain* brain);
    
    // Get current level
    float getLevel() const;
    void setLevel(float level);
    
    // Get plasticity factor for attention-related plasticity
    float getAttentionPlasticityFactor() const;
    float getMemoryPlasticityFactor() const;
    
    // Update state
    void update(TimestepDuration dt);
    
    // Signal attentional focus
    void signalAttention(float intensity);
    
    // Signal memory encoding
    void signalMemoryEncoding();
    
    // Signal memory consolidation
    void signalMemoryConsolidation();
    
    // Reset
    void reset();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Norepinephrine: Arousal and vigilance
class Norepinephrine {
public:
    Norepinephrine();
    ~Norepinephrine();
    
    // Initialize with brain reference
    void initialize(class Brain* brain);
    
    // Get current level
    float getLevel() const;
    void setLevel(float level);
    
    // Get arousal-related effects
    float getArousalLevel() const;
    float getSignalDetection() const;
    
    // Update state
    void update(TimestepDuration dt);
    
    // Signal arousal (waking up, increased alertness)
    void signalArousal(float intensity);
    
    // Signal signal detection (sensing novel stimuli)
    void signalNovelStimulus(float intensity);
    
    // Reset
    void reset();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Serotonin: Mood, impulsivity, and social behavior
class Serotonin {
public:
    Serotonin();
    ~Serotonin();
    
    // Initialize with brain reference
    void initialize(class Brain* brain);
    
    // Get current level
    float getLevel() const;
    void setLevel(float level);
    
    // Get social-related effects
    float getSocialCohesion() const;
    float getBehavioralFlexibility() const;
    float getImpulseControl() const;
    
    // Update state
    void update(TimestepDuration dt);
    
    // Signal social reward
    void signalSocialReward(float intensity);
    
    // Signal reward prediction error for social learning
    void signalSocialPredictionError(float error);
    
    // Signal mood change
    void signalMoodChange(float delta);
    
    // Reset
    void reset();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
