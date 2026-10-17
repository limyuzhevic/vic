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

class Dopamine;
class Acetylcholine;
class Norepinephrine;
class Serotonin;

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
    void initialize(class Brain* brain);
    
    // Attention and memory consolidation
    void signalAttention(float attentionStrength);
    void enhanceMemory(const std::vector<float>& pattern, float strength);
    
    float getConsolidationEffect() const;
    void setConsolidationWeight(float weight);
    
    const std::vector<float>& getMemoryHistory() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

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
    void initialize(class Brain* brain);
    
    // Arousal and vigilance
    void signalArousal(float arousalStrength);
    void enhanceVigilance(float vigilanceLevel);
    
    float getArousalLevel() const;
    float getVigilanceBoost() const;
    
    void setArousalThreshold(float threshold);
    void setVigilanceWeight(float weight);
    
private:
    struct Impl;
    Impl* pImpl;
};

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
    void initialize(class Brain* brain);
    
    // Mood and social behavior
    void signalMood(float moodStrength);
    void enhanceSocialBehavior(float socialLevel);
    
    float getMoodLevel() const;
    float getSocialModulation() const;
    
    void setMoodWeight(float weight);
    void setSocialModulation(float modulation);
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
