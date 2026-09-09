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
    
    // Initialize neuromodulator with brain reference
    virtual void initialize(class Brain* brain) {}
    
protected:
    Neuromodulator() = default;
};

// Real implementation - implements reward prediction, reinforcement learning, working memory modulation
class Dopamine : public Neuromodulator {
public:
    Dopamine();
    ~Dopamine() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Initialize with brain reference
    void initialize(class Brain* brain) override;
    
    // Reward signaling
    void signalReward(float reward);
    void signalRewardPredictionError(float error);
    
    // Real dopamine dynamics
    void setBaseline(float baseline);
    float getBaseline() const;
    void setReleaseRate(float rate);
    float getReleaseRate() const;
    void setDecayRate(float rate);
    float getDecayRate() const;
    
    // History tracking
    const std::vector<float>& getHistory() const;
    void clearHistory();
    
private:
    struct Impl;
    Impl* pImpl;
};

// Acetylcholine: Attention and memory consolidation
// Real implementation - modulates attention, memory consolidation, cortical plasticity
class Acetylcholine : public Neuromodulator {
public:
    Acetylcholine();
    ~Acetylcholine() override;
    
    // Initialize with brain reference
    void initialize(class Brain* brain) override;
    
    // Get ACh level
    float getLevel() const override;
    void setLevel(float level) override;
    
    // Update ACh dynamics
    void update(TimestepDuration dt) override;
    
    // Get ACh effects on plasticity
    float getPlasticityFactor() const override;
    
private:
    struct Impl;
    Impl* pImpl;
};

// Norepinephrine: Arousal and vigilance
// Real implementation - modulates arousal, vigilance, signal-to-noise ratio in neural processing
class Norepinephrine : public Neuromodulator {
public:
    Norepinephrine();
    ~Norepinephrine() override;
    
    // Initialize with brain reference
    void initialize(class Brain* brain) override;
    
    // Get NE level
    float getLevel() const override;
    void setLevel(float level) override;
    
    // Update NE dynamics
    void update(TimestepDuration dt) override;
    
    // Get NE effects on plasticity
    float getPlasticityFactor() const override;
    
private:
    struct Impl;
    Impl* pImpl;
};

// Serotonin: Mood, impulsivity, and social behavior
// Real implementation - modulates mood regulation, impulsivity control, social behavior
class Serotonin : public Neuromodulator {
public:
    Serotonin();
    ~Serotonin() override;
    
    // Initialize with brain reference
    void initialize(class Brain* brain) override;
    
    // Get 5-HT level
    float getLevel() const override;
    void setLevel(float level) override;
    
    // Update 5-HT dynamics
    void update(TimestepDuration dt) override;
    
    // Get 5-HT effects on plasticity
    float getPlasticityFactor() const override;
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
