#pragma once

#include "../core/Types/Types.hpp"
#include <string>
#include <vector>

namespace nlm {

// Neuromodulator: Abstract base for neuromodulatory signals
// Full implementation for Phase 6

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
    
    // Integration with other systems
    void integrateAcetylcholine(float achLevel);
    void integrateNorepinephrine(float neLevel);
    void integrateSerotonin(float serotoninLevel);
    
    // State management
    void reset();
    
private:
    struct Impl;
    Impl* pImpl;
};

// Acetylcholine: Attention and memory consolidation
class Acetylcholine : public Neuromodulator {
public:
    Acetylcholine();
    ~Acetylcholine() override;
    
    const char* getName() const override { return "ACh"; }
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Integration with other systems
    void integrateDopamine(float daLevel);
    void integrateNorepinephrine(float neLevel);
    void integrateSerotonin(float serotoninLevel);
    
    // Memory consolidation
    void enableMemoryConsolidation(bool enable);
    void setConsolidationStrength(float strength);
    
    // State management
    void reset();
    
private:
    struct Impl;
    Impl* pImpl;
};

// Norepinephrine: Arousal and vigilance
class Norepinephrine : public Neuromodulator {
public:
    Norepinephrine();
    ~Norepinephrine() override;
    
    const char* getName() const override { return "NE"; }
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Integration with other systems
    void integrateDopamine(float daLevel);
    void integrateAcetylcholine(float achLevel);
    void integrateSerotonin(float serotoninLevel);
    
    // Arousal modulation
    void setArousalLevel(float arousal);
    float getArousalLevel() const;
    
    // Vigilance
    void updateVigilance(float dt);
    float getVigilance() const;
    
    // State management
    void reset();
    
private:
    struct Impl;
    Impl* pImpl;
};

// Serotonin: Mood, impulsivity, and social behavior
class Serotonin : public Neuromodulator {
public:
    Serotonin();
    ~Serotonin() override;
    
    const char* getName() const override { return "5-HT"; }
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Integration with other systems
    void integrateDopamine(float daLevel);
    void integrateAcetylcholine(float achLevel);
    void integrateNorepinephrine(float neLevel);
    
    // Mood regulation
    void setMoodLevel(float mood);
    float getMoodLevel() const;
    
    // Impulsivity control
    void updateImpulsivity(float dt);
    void reduceImpulsivity(float amount);
    float getImpulsivity() const;
    
    // Social behavior
    void updateSocialMotivation(float dt);
    float getSocialMotivation() const;
    
    // State management
    void reset();
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
