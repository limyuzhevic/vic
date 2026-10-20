#pragma once

#include "../core/Types/Types.hpp"
#include "../brain/Brain.hpp"
#include <string>
#include <vector>

namespace nlm {

// Neuromodulator: Abstract base for neuromodulatory signals
// Now implemented with real neural mechanisms

class Neuromodulator {
public:
    virtual ~Neuromodulator() = default;
    
    // Get modulator name
    virtual const char* getName() const = 0;
    
    // Get current concentration/level
    virtual float getLevel() const = 0;
    virtual void setLevel(float level) = 0;
    
    // Apply neuromodulatory effect to plasticity
    // Real implementation affects neural excitability and plasticity rates
    virtual float getPlasticityFactor() const = 0;
    
    // Update neuromodulator state
    virtual void update(TimestepDuration dt) = 0;
    
protected:
    Neuromodulator() = default;
};

// Dopamine: Reward and reinforcement learning signal
// Real implementation with neural mechanisms
class Dopamine : public Neuromodulator {
public:
    Dopamine();
    ~Dopamine() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Reward signaling with neural effects
    void signalReward(float reward);
    void signalRewardPredictionError(float error);
    
    // Apply effects to brain networks
    void applyEffects(Brain* brain);

private:
    struct Impl;
    Impl* pImpl;
};

// Acetylcholine: Attention and memory consolidation
// Real implementation with cholinergic mechanisms
class Acetylcholine : public Neuromodulator {
public:
    Acetylcholine();
    ~Acetylcholine() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Apply attentional effects
    void applyToAttention(Brain* brain);
    void enhanceMemoryConsolidation(Brain* brain);

private:
    struct Impl;
    Impl* pImpl;
};

// Norepinephrine: Arousal and vigilance
// Real implementation with noradrenergic mechanisms
class Norepinephrine : public Neuromodulator {
public:
    Norepinephrine();
    ~Norepinephrine() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Apply arousal effects
    void increase alertness(Brain* brain);
    void enhance signalDetection(Brain* brain);

private:
    struct Impl;
    Impl* pImpl;
};

// Serotonin: Mood, impulsivity, and social behavior
// Real implementation with serotonergic mechanisms
class Serotonin : public Neuromodulator {
public:
    Serotonin();
    ~Serotonin() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Apply mood and behavioral effects
    void modulateMood(Brain* brain);
    void regulate impulsivity(Brain* brain);

private:
    struct Impl;
    Impl* pImpl;
};

// Novelty: Novelty detection and exploration drive
// Real implementation with detecting unexpected sensory input
class Novelty : public Neuromodulator {
public:
    Novelty();
    ~Novelty() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Detect novelty in sensory input
    void analyzeInputChange(const std::vector<float>& newInput);
    float computeNovelty(const std::vector<float>& input1, const std::vector<float>& input2);

private:
    struct Impl;
    Impl* pImpl;
};

// Curiosity: Exploration motivation combining novelty and prediction error
// Real implementation with integrated exploration mechanisms
class Curiosity : public Neuromodulator {
public:
    Curiosity();
    ~Curiosity() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Drive exploration behavior
    void increaseExploration(Brain* brain);
    void reduceExploration(Brain* brain);
    float computeCuriosityDrive(float novelty, float predictionError);

private:
    struct Impl;
    Impl* pImpl;
};

// PredictionError: Prediction error signal for learning
// Real implementation with predictive coding mechanisms
class PredictionError : public Neuromodulator {
public:
    PredictionError();
    ~PredictionError() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Compute prediction errors
    float computeError(const std::vector<float>& predicted,
                      const std::vector<float>& actual);
    float getPredictionError() const { return lastError_; }
    void recordError(float error, SimulationStep step);

private:
    struct Impl;
    Impl* pImpl;
    
    float lastError_;
    float surpriseThreshold_;
};

} // namespace nlm
