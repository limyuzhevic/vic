#pragma once

#include "../core/Types/Types.hpp"
#include <string>

namespace nlm {

// Dopamine: Reward and reinforcement learning signal
// Implements real dopamine signaling for reward prediction error and reward delivery
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
    void signalCuriosity(float curiosity);
    
    // Get neuromodulation effects
    float getAttentionModulation() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Acetylcholine: Attention and memory consolidation
// Implements ACh signaling for attention allocation and memory consolidation
class Acetylcholine : public Neuromodulator {
public:
    Acetylcholine();
    ~Acetylcholine() override;
    
    const char* getName() const override { return "ACh"; }
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Signal attention and memory effects
    void signalAttention(float attention);
    void signalNovelty(float novelty);
    void signalSensoryInput(float inputStrength);
    
    // Get neuromodulation effects
    float getAttentionStrength() const;
    float getMemoryConsolidation() const;
    float getWorkingMemoryBoost() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Norepinephrine: Arousal and vigilance
// Implements NE signaling for arousal, stress response, and enhanced processing
class Norepinephrine : public Neuromodulator {
public:
    Norepinephrine();
    ~Norepinephrine() override;
    
    const char* getName() const override { return "NE"; }
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Signal arousal and stress effects
    void signalArousal(float arousal);
    void signalStress(float stress);
    void signalWarning(float warning);
    
    // Get neuromodulation effects
    float getArousalLevel() const;
    float getStressLevel() const;
    float getProcessingSpeed() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Serotonin: Mood, impulsivity, and social behavior
// Implements 5-HT signaling for mood regulation, impulsivity control, and social behavior
class Serotonin : public Neuromodulator {
public:
    Serotonin();
    ~Serotonin() override;
    
    const char* getName() const override { return "5-HT"; }
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Signal mood and social effects
    void signalMood(float mood);
    void signalReward(float reward);
    void signalPunishment(float punishment);
    void signalSocialBonding(float bonding);
    
    // Get neuromodulation effects
    float getMoodLevel() const;
    float getImpulsivityLevel() const;
    float getSocialCohesion() const;
    float getRewardSensitivity() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
