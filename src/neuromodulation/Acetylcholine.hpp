#pragma once

#include "../core/Types/Types.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>

namespace nlm {

// Acetylcholine: Attention and memory consolidation
// Real implementation - ACh enhances attention and modulates memory consolidation
// Key mechanisms:
// - Responds to novel stimuli and attentional focus
// - Enhances plasticity for attended information
// - Modulates working memory encoding
// - Increases during focused attention

class Acetylcholine {
public:
    Acetylcholine();
    ~Acetylcholine();
    
    const char* getName() const;
    float getLevel() const;
    void setLevel(float level);
    float getPlasticityFactor() const;
    void update(TimestepDuration dt);
    
    // Attention signaling
    void signalAttention(float attentionSignal);
    void signalNovelty(float novelty);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Norepinephrine: Arousal and vigilance
// Real implementation - NE modulates arousal, alertness, and working memory
// Key mechanisms:
// - Responds to salient events (threat, reward, novelty)
// - Modulates arousal levels and vigilance
// - Enhances working memory encoding for salient information
// - Reduces during relaxation/sleep

class Norepinephrine {
public:
    Norepinephrine();
    ~Norepinephrine();
    
    const char* getName() const;
    float getLevel() const;
    void setLevel(float level);
    float getPlasticityFactor() const;
    void update(TimestepDuration dt);
    
    // Arousal signaling
    void signalArousal(float arousalLevel);
    void signalStress(float stressLevel);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Serotonin: Mood, impulsivity, and social behavior
// Real implementation - 5-HT modulates mood, punishment sensitivity, and social behavior
// Key mechanisms:
// - Responds to reward prediction errors (negative errors)
// - Modulates impulsivity and risk-taking
// - Influences social behavior and acceptance
// - regulates mood and anxiety

class Serotonin {
public:
    Serotonin();
    ~Serotonin();
    
    const char* getName() const;
    float getLevel() const;
    void setLevel(float level);
    float getPlasticityFactor() const;
    void update(TimestepDuration dt);
    
    // Mood and social signaling
    void signalReward(float reward);
    void signalPunishment(float punishment);
    void signalSocialFeedback(float socialFeedback);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm