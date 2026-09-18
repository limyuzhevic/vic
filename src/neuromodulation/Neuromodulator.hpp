#pragma once

#include "../core/Types/Types.hpp"
#include <string>
#include <vector>

namespace nlm {

// Neuromodulator: Abstract base for neuromodulatory signals
// Phase 3: Advanced neuromodulation implementation

class Neuromodulator {
public:
    virtual ~Neuromodulator() = default;
    
    // Get modulator name
    virtual const char* getName() const = 0;
    
    // Get current concentration/level
    virtual float getLevel() const = 0;
    virtual void setLevel(float level) = 0;
    
    // Apply neuromodulatory effect to plasticity
    // Combines with attention, arousal, and learning signals
    virtual float getPlasticityFactor() const = 0;
    
    // Update neuromodulator state based on inputs
    // Receives reward prediction errors, novelty, attention signals
    virtual void update(float rewardPredictionError, float novelty, 
                       float attention, float dt) = 0;
    
    // Reset to baseline state
    virtual void reset() = 0;
    
    // Get modulation statistics
    struct ModulationStats {
        float totalEvents;
        float averageEffect;
        float peakEffect;
        float baselineEffect;
        
        ModulationStats() : totalEvents(0.0f), averageEffect(0.0f), 
                           peakEffect(0.0f), baselineEffect(0.0f) {}
    };
    
    virtual ModulationStats getStats() const = 0;
    
protected:
    Neuromodulator() = default;
};

// Dopamine: Reward and reinforcement learning signal
// Phase 3: Biologically plausible dopamine dynamics
class Dopamine : public Neuromodulator {
public:
    Dopamine();
    ~Dopamine() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    
    // Enhanced plasticity factor with interaction terms
    float getPlasticityFactor() const override;
    
    // Update with comprehensive neuromodulatory inputs
    void update(float rewardPredictionError, float novelty, 
               float attention, float dt) override;
    
    // Reset to baseline state
    void reset() override;
    
    // Get modulation statistics
    ModulationStats getStats() const override;
    
    // Enhanced reward signaling with temporal dynamics
    void signalReward(float reward, float temporalDifference = 1.0f);
    
    // Reward prediction error with eligibility traces
    void signalRewardPredictionError(float error, float eligibility = 1.0f);
    
    // Get neuromodulatory state
    float getPredictionError() const;
    float getBaseline() const;
    float getPeak() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

// Acetylcholine: Attention and memory consolidation
// Phase 3: Gating-based attention modulation
class Acetylcholine : public Neuromodulator {
public:
    Acetylcholine();
    ~Acetylcholine() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    
    // Attention modulation: controls signal-to-noise ratio
    float getPlasticityFactor() const override;
    
    // Update attention state based on task demands and novelty
    void update(float rewardPredictionError, float novelty, 
               float attention, float dt) override;
    
    // Reset to baseline state
    void reset() override;
    
    // Get modulation statistics
    ModulationStats getStats() const override;
    
    // Boost attention for novel or rewarding stimuli
    void boostAttention(float intensity, float duration);
    
    // Decay towards baseline
    void decayAttention(float rate, float dt);
    
private:
    struct Impl;
    Impl* pImpl;
};

// Norepinephrine: Arousal and vigilance
// Phase 3: Dynamic arousal and locus coeruleus model
class Norepinephrine : public Neuromodulator {
public:
    Norepinephrine();
    ~Norepinephrine() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    
    // Arousal modulation: controls overall responsiveness
    float getPlasticityFactor() const override;
    
    // Update arousal based on stress, novelty, and reward prediction
    void update(float rewardPredictionError, float novelty, 
               float attention, float dt) override;
    
    // Reset to baseline state
    void reset() override;
    
    // Get modulation statistics
    ModulationStats getStats() const override;
    
    // Spike-tomography model: higher tonic for unexpected events
    void signalEvent(float saliency, bool unexpected = false);
    
private:
    struct Impl;
    Impl* pImpl;
};

// Serotonin: Mood, impulsivity, and behavioral flexibility
// Phase 3: Behavioral state regulation
class Serotonin : public Neuromodulator {
public:
    Serotonin();
    ~Serotonin() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    
    // Serotonin modulation: regulates behavioral flexibility
    float getPlasticityFactor() const override;
    
    // Update based on reward history and prediction errors
    void update(float rewardPredictionError, float novelty, 
               float attention, float dt) override;
    
    // Reset to baseline state
    void reset() override;
    
    // Get modulation statistics
    ModulationStats getStats() const override;
    
    // Adjust behavioral flexibility based on outcomes
    void adjustFlexibility(float outcome, float learningRate = 0.01f);
    
    // Get flexibility state
    float getFlexibility() const;
    
private:
    struct Impl;
    Impl* pImpl;
};
