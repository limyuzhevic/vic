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
    
    // Accessors
    void setLearningRate(float rate) { learningRate_ = rate; }
    float getLearningRate() const { return learningRate_; }
    
    void setRewardScale(float scale) { rewardScale_ = scale; }
    float getRewardScale() const { return rewardScale_; }
    
    void reset() { pImpl_->level = 0.0f; }
    
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
    void update(float attention, float memory, TimestepDuration dt) override;
    
    // Accessors
    void setAttentionWeight(float weight);
    void setMemoryConsolidationWeight(float weight);
    float getAttentionWeight() const;
    float getMemoryConsolidationWeight() const;
    
    void reset();
    
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
    void update(float arousal, float vigilance, float stress, TimestepDuration dt) override;
    
    // Accessors
    void setArousalWeight(float weight);
    void setVigilanceWeight(float weight);
    void setStressWeight(float weight);
    float getArousalWeight() const;
    float getVigilanceWeight() const;
    float getStressWeight() const;
    
    void reset();
    
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
    void update(float reward, float punishment, float social, float mood, TimestepDuration dt) override;
    
    // Accessors
    void setRewardWeight(float weight);
    void setPunishmentWeight(float weight);
    void setSocialWeight(float weight);
    void setMoodWeight(float weight);
    float getRewardWeight() const;
    float getPunishmentWeight() const;
    float getSocialWeight() const;
    float getMoodWeight() const;
    
    void reset();
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
