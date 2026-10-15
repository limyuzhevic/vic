#pragma once

#include "../core/Types/Types.hpp"
#include <string>
#include <vector>

namespace nlm {

// Neuromodulator: Abstract base for neuromodulatory signals
// Implemented with real neuromodulation effects in Neuromodulator.cpp
//
// Key mechanisms:
// - Dopamine affects reward prediction error and learning
// - Acetylcholine modulates attention and memory consolidation
// - Norepinephrine regulates arousal and vigilance
// - Serotonin influences mood and impulsivity
// - All neuromodulators can affect neural excitability and plasticity

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
// Implemented in Dopamine.cpp
//
// Mechanisms:
// - Signals reward prediction error
// - Modulates synaptic plasticity
// - Affects neural excitability
// - Drives learning and motivation

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
    std::unique_ptr<Impl> pImpl;
};

// Acetylcholine: Attention and memory consolidation
// Implemented in Acetylcholine.cpp
//
// Mechanisms:
// - Modulates attention and arousal
// - Enhances memory encoding and consolidation
// - Affects neural gain control
// - Influences cortical plasticity

class Acetylcholine : public Neuromodulator {
public:
    Acetylcholine();
    ~Acetylcholine() override;
    
    const char* getName() const override { return "ACh"; }
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Modulation effects
    void modulateAttention(float strength);
    void enhanceMemoryConsolidation(float duration);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Norepinephrine: Arousal and vigilance
// Implemented in Norepinephrine.cpp
//
// Mechanisms:
// - Regulates arousal and alertness
// - Enhances signal detection
// - Modulates working memory
// - Affects stress response

class Norepinephrine : public Neuromodulator {
public:
    Norepinephrine();
    ~Norepinephrine() override;
    
    const char* getName() const override { return "NE"; }
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Modulation effects
    void increaseArousal(float level);
    void enhanceVigilance(float duration);
    void reduceStress(float duration);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Serotonin: Mood, impulsivity, and social behavior
// Implemented in Serotonin.cpp
//
// Mechanisms:
// - Regulates mood and emotional state
// - Controls impulsivity
// - Influences social behavior
// - Modulates sleep and appetite

class Serotonin : public Neuromodulator {
public:
    Serotonin();
    ~Serotonin() override;
    
    const char* getName() const override { return "5-HT"; }
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Modulation effects
    void elevateMood(float level);
    void reduceImpulsivity(float duration);
    void promoteSocialBehavior(float strength);
    void regulateSleep(float duration);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
