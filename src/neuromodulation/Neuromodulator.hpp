#pragma once

#include "../core/Types/Types.hpp"
#include <string>
#include <vector>

namespace nlm {

// Neuromodulator: Abstract base for neuromodulatory signals
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
    
private:
    struct Impl;
    Impl* pImpl;
};

// Acetylcholine: Attention and memory consolidation
// REAL IMPLEMENTATION - Acetylcholine regulates attention and memory consolidation
class Acetylcholine : public Neuromodulator {
public:
    Acetylcholine();
    ~Acetylcholine() override;
    
    const char* getName() const override { return "ACh"; }
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Acetylcholine signaling
    void signalAttentionFocus(float focusStrength);
    void signalMemoryConsolidation(float memoryPriority);
    void signalLearningReadiness(float learningReadiness);
    void signalCorticalActivation(float activationLevel);
    
    float getAttentionStrength() const;
    float getMemoryConsolidationRate() const;
    float getLearningReadiness() const;
    float getCorticalActivation() const;
    
    void reset();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Norepinephrine: Arousal and vigilance
// REAL IMPLEMENTATION - Norepinephrine regulates arousal, attention, and vigilance
class Norepinephrine : public Neuromodulator {
public:
    Norepinephrine();
    ~Norepinephrine() override;
    
    const char* getName() const override { return "NE"; }
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Norepinephrine signaling
    void signalArousalChange(float arousalDelta);
    void signalAttentionNeed(float attentionDemand);
    void signalStressResponse(float stressLevel);
    void signalMemorySalience(float memorySalience);
    
    float getArousal() const;
    float getVigilance() const;
    float getAttentionBias() const;
    float getAlertness() const;
    
    void reset();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Serotonin: Mood, impulsivity, and social behavior
// REAL IMPLEMENTATION - Serotonin regulates mood, social behavior, and impulsivity
class Serotonin : public Neuromodulator {
public:
    Serotonin();
    ~Serotonin() override;
    
    const char* getName() const override { return "5-HT"; }
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Serotonin signaling
    void signalMoodChange(float moodDelta);
    void signalSocialExperience(float socialReward);
    void signalRewardPrediction(float predictedReward, float actualReward);
    
    float getMood() const;
    float getImpulsivity() const;
    float getSocialLearningAccumulation() const;
    
    void reset();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
