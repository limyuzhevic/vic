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
    
private:
    struct Impl;
    Impl* pImpl;
};

// Norepinephrine: Arousal and vigilance
// Real-time norepinephrine dynamics for alertness modulation,
// stress response, and adaptive attention allocation
class Norepinephrine : public Neuromodulator {
public:
    Norepinephrine();
    ~Norepinephrine();
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Arousal-related functions
    void signalArousal(float intensity);
    
    // Vigilance and alertness functions
    void updateVigilance(float level);
    void enhanceAlertness(float factor);
    
    // Integration with cognitive systems
    void integrateWithAttention(float attentionLevel, float taskComplexity);
    
    // Reset system
    void reset();
    
private:
    struct Impl;
    Impl* pImpl;
};

// Serotonin: Mood, impulsivity, and social behavior
// Real-time serotonin dynamics for mood regulation, 
// behavioral flexibility, and social interaction modulation
class Serotonin : public Neuromodulator {
public:
    Serotonin();
    ~Serotonin();
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Mood-related functions
    void signalReward(float reward, bool predictionError);
    void increaseMood(float delta);
    
    // Behavioral flexibility functions
    void introduceBehavioralVariability(float strength);
    
    // Social behavior functions
    void updateSocialBehavior(float interactionQuality);
    
    // Reset system
    void reset();
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
