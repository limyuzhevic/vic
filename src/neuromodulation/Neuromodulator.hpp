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
    
    // Get reward prediction error
    float getPredictionError() const;
    
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
    
    // Attention modulation
    float getAttentionModulation() const { return attentionMod_; }
    void setAttentionModulation(float mod) { attentionMod_ = mod; }
    
    // Memory consolidation boost
    float getMemoryConsolidationBoost() const { return memoryConsolidationBoost_; }
    void setMemoryConsolidationBoost(float boost) { memoryConsolidationBoost_ = boost; }
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    float attentionMod_;
    float memoryConsolidationBoost_;
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
    
    // Arousal effects
    float getArousal() const { return arousal_; }
    void setArousal(float arousal) { arousal_ = arousal; }
    
    // Planning effects
    float getPlanningModulation() const { return planningMod_; }
    void setPlanningModulation(float mod) { planningMod_ = mod; }
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    float arousal_;
    float planningMod_;
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
    
    // Mood effects
    float getMood() const { return mood_; }
    void setMood(float mood) { mood_ = mood; }
    
    // Exploration/exploitation balance
    float getExplorationBias() const { return explorationBias_; }
    void setExplorationBias(float bias) { explorationBias_ = bias; }
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    float mood_;
    float explorationBias_;
};

} // namespace nlm
