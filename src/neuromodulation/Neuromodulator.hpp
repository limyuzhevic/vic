#pragma once

#include "../core/Types/Types.hpp"
#include <string>
#include <vector>

namespace nlm {

// Neuromodulator: Abstract base for neuromodulatory signals
// Real neuromodulatory effects on neural plasticity and behavior

class Neuromodulator {
public:
    virtual ~Neuromodulator() = default;
    
    // Get modulator name
    virtual const char* getName() const = 0;
    
    // Get current concentration/level
    virtual float getLevel() const = 0;
    virtual void setLevel(float level) = 0;
    
    // Apply neuromodulatory effect to plasticity
    // Returns modulation factor: 0.0-1.0 (lower values reduce plasticity)
    virtual float getPlasticityFactor() const = 0;
    
    // Update neuromodulator state based on time
    virtual void update(TimestepDuration dt) = 0;
    
    // Get baseline level
    virtual float getBaselineLevel() const { return 0.0f; }
    
    // Get peak level
    virtual float getPeakLevel() const { return 1.0f; }
    
    // Check if neuromodulator is active
    virtual bool isActive() const { return getLevel() > getBaselineLevel() + 0.01f; }
    
    // Get response time constant
    virtual float getTimeConstant() const { return 1.0f; }
    
protected:
    Neuromodulator() = default;
};

// Dopamine: Reward and reinforcement learning signal
// Real dopamine dynamics based on reward prediction errors
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
    
    // Get reward prediction error (PHE) state
    float getPredictionError() const;
    
    // Set baseline level based on context
    void setContextBaseline(float baseline);
    
    // Get current baseline
    float getCurrentBaseline() const { return currentBaseline_; }
    
    // Set learning rate for reward updates
    void setLearningRate(float rate) { learningRate_ = rate; }
    
private:
    struct Impl;
    Impl* pImpl;
    float currentBaseline_;
    float learningRate_;
};

// Acetylcholine: Attention and memory consolidation
// Real cholinergic modulation of attention and working memory
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
    void updateAttention(float attentionStrength, TimestepDuration dt);
    
    // Get attention level
    float getAttentionLevel() const;
    
    // Get memory modulation factor
    float getMemoryModulationFactor() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

// Norepinephrine: Arousal and vigilance
// Real noradrenergic modulation of arousal and stress responses
class Norepinephrine : public Neuromodulator {
public:
    Norepinephrine();
    ~Norepinephrine() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Arousal modulation
    void signalArousalEvent();
    void reduceArousal(TimestepDuration dt);
    
    // Get arousal level
    float getArousalLevel() const;
    
    // Get stress level
    float getStressLevel() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

// Serotonin: Mood, impulsivity, and social behavior
// Real serotonergic modulation of mood and social behavior
class Serotonin : public Neuromodulator {
public:
    Serotonin();
    ~Serotonin() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Mood modulation
    void updateMood(float newMood);
    void signalSocialInteraction(float socialValue);
    
    // Get mood level
    float getMoodLevel() const;
    
    // Get impulsivity level (inversely related to serotonin)
    float getImpulsivityLevel() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

// Neuromodulation system coordinator
class NeuromodulationSystem {
public:
    NeuromodulationSystem();
    ~NeuromodulationSystem();
    
    // Add a neuromodulator
    void addNeuromodulator(std::unique_ptr<Neuromodulator> modulator);
    
    // Get neuromodulator by name
    Neuromodulator* getNeuromodulator(const std::string& name);
    
    // Update all neuromodulators
    void update(TimestepDuration dt);
    
    // Apply combined neuromodulatory effects
    float getCombinedPlasticityFactor() const;
    
    // Get overall neuromodulation level
    float getOverallModulationLevel() const;
    
    // Reset all neuromodulators
    void reset();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
