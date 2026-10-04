#pragma once

#include "../core/Types/Types.hpp"
#include <string>
#include <vector>

namespace nlm {

// Neuromodulator: Abstract base for neuromodulatory signals
// Phase 6 implementation provides biologically realistic neuromodulation
//
// Key mechanisms:
// - Neuromodulators affect neural excitability
// - They influence plasticity rates and memory consolidation
// - They regulate attention and arousal
// - They implement reward prediction error signaling

class Neuromodulator {
public:
    virtual ~Neuromodulator() = default;
    
    // Get modulator name
    virtual const char* getName() const = 0;
    
    // Get current concentration/level
    virtual float getLevel() const = 0;
    virtual void setLevel(float level) = 0;
    
    // Apply neuromodulatory effect to plasticity
    // Returns scaling factor for plasticity rates
    virtual float getPlasticityFactor() const = 0;
    
    // Update neuromodulator state
    virtual void update(TimestepDuration dt) = 0;
    
protected:
    Neuromodulator() = default;
};

// Acetylcholine: Attention and memory consolidation
// Phase 6 implementation with attention modulation
class Acetylcholine : public Neuromodulator {
public:
    Acetylcholine();
    ~Acetylcholine() override;
    
    const char* getName() const override { return "ACh"; }
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Modulate attention and memory
    void modulateAttention(float strength);
    void modulateMemoryConsolidation(float strength);
    
private:
    struct Impl;
    Impl* pImpl;
};

// Norepinephrine: Arousal and vigilance
// Phase 6 implementation with arousal modulation
class Norepinephrine : public Neuromodulator {
public:
    Norepinephrine();
    ~Norepinephrine() override;
    
    const char* getName() const override { return "NE"; }
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Modulate arousal and alertness
    void modulateArousal(float strength);
    void modulateVigilance(float strength);
    
private:
    struct Impl;
    Impl* pImpl;
};

// Serotonin: Mood, impulsivity, and social behavior
// Phase 6 implementation with mood and social modulation
class Serotonin : public Neuromodulator {
public:
    Serotonin();
    ~Serotonin() override;
    
    const char* getName() const override { return "5-HT"; }
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Modulate mood and impulsivity
    void modulateMood(float strength);
    void modulateImpulsivity(float strength);
    
private:
    struct Impl;
    Impl* pImpl;
};

// Dopamine: Reward and reinforcement learning signal
// Phase 6 implementation with realistic dopamine dynamics
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
    
    // Get current reward prediction error
    float getRewardPredictionError() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

// Novelty: Novelty detection and curiosity
// Phase 6 implementation with novelty detection
class Novelty : public Neuromodulator {
public:
    Novelty();
    ~Novelty() override;
    
    const char* getName() const override { return "Novelty"; }
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Detect novelty in sensory input
    void detectNovelty(const std::vector<float>& sensoryInput);
    
    // Get novelty level
    float getNoveltyLevel() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

// Prediction error: Prediction error signaling
// Phase 6 implementation with prediction error computation
class PredictionError : public Neuromodulator {
public:
    PredictionError();
    ~PredictionError() override;
    
    const char* getName() const override { return "PredictionError"; }
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Compute prediction error
    void computePredictionError(const std::vector<float>& predicted,
                               const std::vector<float>& actual);
    
    // Get prediction error value
    float getPredictionErrorValue() const;
    
private:
    struct Impl;
    Impl* pImpl;
};
