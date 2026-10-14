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
// PLACEHOLDER - Phase 2 will implement real neuromodulation effects
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
// PLACEHOLDER - Phase 2
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
    
    // Signal specific cholinergic effects
    // Enhance attentional filtering and selection
    void signalAttentionGain(float gain);
    
    // Enhance memory encoding and consolidation
    void signalMemoryEncoding(float strength);
    
    // Modulate learning rate for plasticity rules
    void signalLearningRate(float rate);
    
    // Increase cortical excitability
    void signalCorticalExcitability(float excitability);
    
    // Get ACh-specific effects
    float getAttentionBias() const;
    float getMemoryEncodingGain() const;
    float getLearningRate() const;
    float getCorticalExcitability() const;
    
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
    void update(TimestepDuration dt) override;
    
    // Initialize with brain reference
    void initialize(class Brain* brain);
    
    // Signal specific noradrenergic effects
    // Increase arousal and vigilance
    void signalArousal(float level);
    void signalAlertness(float level);
    
    // Initiate stress response
    void signalStress(float intensity);
    
    // Memory consolidation
    void signalMemoryConsolidation(float factor);
    
    // Adaptive processing
    void signalAdaptation(float rate);
    
    // Get NE-specific effects
    float getArousal() const;
    float getAlertness() const;
    float getStressLevel() const;
    float getMemoryConsolidation() const;
    float getAdaptation() const;
    
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
    void update(TimestepDuration dt) override;
    
    // Initialize with brain reference
    void initialize(class Brain* brain);
    
    // Signal specific serotonergic effects
    // Regulate mood state
    void signalMood(float level);
    
    // Regulate sleep/wake cycle
    void signalSleepWake(float cycle);
    
    // Enhance reward processing and learning
    void signalRewardProcessing(float level);
    
    // Modulate anxiety and stress response
    void signalAnxiety(float level);
    
    // Control impulsivity and decision-making
    void signalImpulsivity(float level);
    
    // Modulate social behavior and bonding
    void signalSocialBehavior(float level);
    
    // Get 5-HT-specific effects
    float getMood() const;
    float getSleepWake() const;
    float getRewardProcessing() const;
    float getAnxiety() const;
    float getImpulsivity() const;
    float getSocialBehavior() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm