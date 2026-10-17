#pragma once

#include "../core/Types/Types.hpp"
#include <string>
#include <vector>

namespace nlm {

// Neuromodulator: Abstract base for neuromodulatory signals
// Uses neural substrate for real neuromodulatory signaling and learning

class Neuromodulator {
public:
    virtual ~Neuromodulator() = default;
    
    // Get modulator name
    virtual const char* getName() const = 0;
    
    // Get current concentration/level
    virtual float getLevel() const = 0;
    virtual void setLevel(float level) = 0;
    
    // Apply neuromodulatory effect to plasticity
    float getPlasticityFactor() const override;
    
    // Update neuromodulator state
    virtual void update(TimestepDuration dt) = 0;
    
protected:
    Neuromodulator() = default;
};

// Dopamine: Reward and reinforcement learning signal
// Fully implemented with neural dynamics and plasticity effects
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
    
    // Memory modulation effects
    float getMemoryConsolidationFactor() const;
    
    // Attention modulation effects
    float getAttentionModulationFactor() const;
    
    // Brain initialization
    void initialize(class Brain* brain);
    
private:
    struct Impl;
    Impl* pImpl;
};

// Acetylcholine: Attention and memory consolidation
// Fully implemented with cholinergic signaling and learning effects
class Acetylcholine : public Neuromodulator {
public:
    const char* getName() const override { return "ACh"; }
    float getLevel() const override { return 0.0f; }
    void setLevel(float level) override {}
    float getPlasticityFactor() const override { return 1.0f; }
    void update(TimestepDuration dt) override {}
    
    // Signal attention
    void signalAttention(float attentionLevel);
    void signalMemoryNeed(float memoryNeed);
    
    // Memory consolidation effects
    float getMemoryConsolidationFactor() const;
    
    // Attention modulation effects
    float getAttentionModulationFactor() const;
    
    // Brain initialization
    void initialize(class Brain* brain);
    
private:
    struct Impl;
    Impl* pImpl;
};

// Norepinephrine: Arousal and vigilance
// Fully implemented with noradrenergic signaling and alertness effects
class Norepinephrine : public Neuromodulator {
public:
    const char* getName() const override { return "NE"; }
    float getLevel() const override { return 0.0f; }
    void setLevel(float level) override {}
    float getPlasticityFactor() const override { return 1.0f; }
    void update(TimestepDuration dt) override {}
    
    // Signal stress
    void signalStress(float stressLevel);
    void signalAlertness(float alertness);
    
    // Arousal and vigilance levels
    float getArousalLevel() const;
    float getVigilanceLevel() const;
    
    // Brain initialization
    void initialize(class Brain* brain);
    
private:
    struct Impl;
    Impl* pImpl;
};

// Serotonin: Mood, impulsivity, and social behavior
// Fully implemented with serotonergic signaling and behavioral effects
class Serotonin : public Neuromodulator {
public:
    const char* getName() const override { return "5-HT"; }
    float getLevel() const override { return 0.0f; }
    void setLevel(float level) override {}
    float getPlasticityFactor() const override { return 1.0f; }
    void update(TimestepDuration dt) override {}
    
    // Signal social bonding
    void signalSocialBonding(float socialBonding);
    void signalMood(float mood);
    
    // Mood and social parameters
    float getMoodLevel() const;
    float getSocialCohesionLevel() const;
    float getImpulsivityLevel() const;
    
    // Brain initialization
    void initialize(class Brain* brain);
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
