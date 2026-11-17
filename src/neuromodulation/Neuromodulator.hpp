#pragma once

#include "../core/Types/Types.hpp"
#include <string>
#include <vector>

namespace nlm {

// Neuromodulator: Abstract base for neuromodulatory signals
// Implements biologically realistic neuromodulation with dynamic effects on neural plasticity

class Neuromodulator {
public:
    virtual ~Neuromodulator() = default;
    
    // Get modulator name
    virtual const char* getName() const = 0;
    
    // Get current concentration/level
    virtual float getLevel() const = 0;
    virtual void setLevel(float level) = 0;
    
    // Apply neuromodulatory effect to plasticity
    // Returns factor by which plasticity should be modulated (1.0 = normal, <1.0 = reduced, >1.0 = enhanced)
    virtual float getPlasticityFactor() const = 0;
    
    // Get baseline level for this neuromodulator
    virtual float getBaseline() const = 0;
    
    // Get peak level (maximum possible)
    virtual float getPeak() const = 0;
    
    // Update neuromodulator state
    virtual void update(TimestepDuration dt) = 0;
    
    // Signal specific events
    virtual void signalEvent(const std::string& eventType, float magnitude) = 0;
    
    // Reset to baseline
    virtual void reset() = 0;
    
protected:
    Neuromodulator() = default;
};

// Dopamine: Reward and reinforcement learning signal
// Implements phasic and tonic dopamine dynamics
class Dopamine : public Neuromodulator {
public:
    Dopamine();
    ~Dopamine() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    float getBaseline() const override;
    float getPeak() const override;
    void update(TimestepDuration dt) override;
    void signalEvent(const std::string& eventType, float magnitude) override;
    void reset() override;
    
    // Specialized signaling methods
    void signalReward(float reward);
    void signalPunishment(float punishment);
    void signalRewardPredictionError(float error);
    void signalNovelty(float novelty);
    
private:
    struct Impl;
    Impl* pImpl;
};

// Acetylcholine: Attention and memory consolidation
// Implements cholinergic modulation of attention and learning
class Acetylcholine : public Neuromodulator {
public:
    Acetylcholine();
    ~Acetylcholine() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    float getBaseline() const override;
    float getPeak() const override;
    void update(TimestepDuration dt) override;
    void signalEvent(const std::string& eventType, float magnitude) override;
    void reset() override;
    
    // Attention-related signaling
    void signalAttention(float attentionStrength);
    void signalWorkingMemoryLoad(float load);
    void signalGoalRelevance(float relevance);
    
private:
    struct Impl;
    Impl* pImpl;
};

// Norepinephrine: Arousal and vigilance
// Implements noradrenergic modulation of arousal, vigilance, and stress response
class Norepinephrine : public Neuromodulator {
public:
    Norepinephrine();
    ~Norepinephrine() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    float getBaseline() const override;
    float getPeak() const override;
    void update(TimestepDuration dt) override;
    void signalEvent(const std::string& eventType, float magnitude) override;
    void reset() override;
    
    // Stress and arousal signaling
    void signalStress(float stressLevel);
    void signalArousal(float arousalLevel);
    void signalTaskDifficulty(float difficulty);
    
private:
    struct Impl;
    Impl* pImpl;
};

// Serotonin: Mood, impulsivity, and social behavior
// Implements serotonergic modulation of mood, decision-making, and social behavior
class Serotonin : public Neuromodulator {
public:
    Serotonin();
    ~Serotonin() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    float getBaseline() const override;
    float getPeak() const override;
    void update(TimestepDuration dt) override;
    void signalEvent(const std::string& eventType, float magnitude) override;
    void reset() override;
    
    // Social and emotional signaling
    void signalSocialFeedback(float socialValue);
    void signalPunishmentIntensity(float intensity);
    void signalOvertoneBehavior(float tone);
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
