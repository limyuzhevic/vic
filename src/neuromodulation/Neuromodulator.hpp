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

// Acetylcholine: Attention and memory consolidation
// PLACEHOLDER - Phase 2
class Acetylcholine : public Neuromodulator {
public:
    const char* getName() const override { return "ACh"; }
    float getLevel() const override { return 0.0f; }
    void setLevel(float level) override {}
    float getPlasticityFactor() const override { return 1.0f; }
    void update(TimestepDuration dt) override {}
};

// Norepinephrine: Arousal and vigilance
// PLACEHOLDER - Phase 2
class Norepinephrine : public Neuromodulator {
public:
    const char* getName() const override { return "NE"; }
    float getLevel() const override { return 0.0f; }
    void setLevel(float level) override {}
    float getPlasticityFactor() const override { return 1.0f; }
    void update(TimestepDuration dt) override {}
};

// Serotonin: Mood, impulsivity, and social behavior
// PLACEHOLDER - Phase 2
class Serotonin : public Neuromodulator {
public:
    Serotonin();
    ~Serotonin() override;
    
    const char* getName() const override { return "5-HT"; }
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Mood state
    void setMood(float mood) { mood_ = mood; }
    float getMood() const { return mood_; }
    
    // Behavioral flexibility
    void setBehavioralFlexibility(float flexibility) { flexibility_ = flexibility; }
    float getBehavioralFlexibility() const { return flexibility_; }
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    float mood_;
    float flexibility_;
};

// Vasopressin: Social bonding and aggression
// PLACEHOLDER - Phase 2
class Vasopressin : public Neuromodulator {
public:
    Vasopressin();
    ~Vasopressin() override;
    
    const char* getName() const override { return "AVP"; }
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Social bonding strength
    void setBondingStrength(float strength) { bondingStrength_ = strength; }
    float getBondingStrength() const { return bondingStrength_; }
    
    // Aggression level
    void setAggressionLevel(float level) { aggressionLevel_ = level; }
    float getAggressionLevel() const { return aggressionLevel_; }
    
    // Pair bonding (mate/kin recognition)
    void formBond(const std::string& bondTarget);
    void breakBond(const std::string& bondTarget);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    float bondingStrength_;
    float aggressionLevel_;
};
