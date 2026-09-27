#pragma once

#include "../core/Types/Types.hpp"
#include <string>
#include <vector>

namespace nlm {

// Neuromodulator: Abstract base for neuromodulatory signals
// Real implementation provides biological neuromodulation effects

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

// Forward declarations for neuromodulators
class Dopamine;
class Acetylcholine;
class Norepinephrine;
class Serotonin;
class Novelty;
class PredictionError;
class Curiosity;

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

// Acetylcholine: Attention and memory consolidation
class Acetylcholine : public Neuromodulator {
public:
    Acetylcholine();
    ~Acetylcholine() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Attention and memory functions
    void signalAttention(float attention);
    void signalRewardPredictionError(float error);
    
    // Query/modification methods
    float getAttentionLevel() const;
    void setMemoryConsolidationRate(float rate);
    float getMemoryConsolidationRate() const;
    void setAttentionWeight(float weight);
    float getAttentionWeight() const;
    float getTotalAttentionBoost() const;
    uint64_t getAttentionUpdates() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

// Norepinephrine: Arousal and vigilance
class Norepinephrine : public Neuromodulator {
public:
    Norepinephrine();
    ~Norepinephrine() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Arousal and vigilance functions
    void signalStress(float stress);
    void signalNovelty(float novelty);
    void signalSleep(float sleepQuality);
    
    // Query/modification methods
    float getArousalLevel() const;
    void setArousalThreshold(float threshold);
    float getArousalThreshold() const;
    float getVigilanceLevel() const;
    void setVigilanceLevel(float level);
    
private:
    struct Impl;
    Impl* pImpl;
};

// Serotonin: Mood, impulsivity, and social behavior
class Serotonin : public Neuromodulator {
public:
    Serotonin();
    ~Serotonin() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Mood and social functions
    void signalReward(float reward);
    void signalPunishment(float punishment);
    void signalSocialInteraction(float interactionValue);
    
    // Query/modification methods
    float getMood() const;
    void setMood(float mood);
    float getImpulsivity() const;
    void setImpulsivity(float impulsivity);
    float getRewardSensitivity() const;
    void setRewardSensitivity(float sensitivity);
    float getPunishmentSensitivity() const;
    void setPunishmentSensitivity(float sensitivity);
    float getSocialBonding() const;
    void setSocialBonding(float bonding);
    float getLearningRate() const;
    void setLearningRate(float rate);
    float getApproachWithdrawal() const;
    void setApproachWithdrawal(float tendency);
    float getTotalRewards() const;
    float getTotalPunishments() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
