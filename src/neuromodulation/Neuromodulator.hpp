#pragma once

#include "../core/Types/Types.hpp"
#include <string>
#include <vector>

namespace nlm {

// Neuromodulator: Abstract base for neuromodulatory signals
// Now fully implemented with real biological mechanisms

class Neuromodulator {
public:
    virtual ~Neuromodulator() = default;
    
    // Get modulator name
    virtual const char* getName() const = 0;
    
    // Get current concentration/level
    virtual float getLevel() const = 0;
    virtual void setLevel(float level) = 0;
    
    // Apply neuromodulatory effect to plasticity
    // Returns factor that multiplies baseline plasticity (1.0 = normal)
    virtual float getPlasticityFactor() const = 0;
    
    // Update neuromodulator state
    virtual void update(TimestepDuration dt) = 0;
    
    // Decay over time
    virtual void decay(float factor) { decayRate_ = factor; }
    
    // Get decay rate
    virtual float getDecayRate() const { return decayRate_; }
    
protected:
    Neuromodulator() : decayRate_(0.95f) {}
    float decayRate_;
};

// Dopamine: Reward and reinforcement learning signal
// NOW IMPLEMENTED: Real reward prediction error computation
class Dopamine : public Neuromodulator {
public:
    Dopamine();
    ~Dopamine() override;
    
    const char* getName() const override { return "Dopamine"; }
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Reward signaling
    void signalReward(float reward);
    void signalRewardPredictionError(float error);
    
    // Get reward prediction error
    float getPredictionError() const;
    
    // Set prediction error directly
    void setPredictionError(float error) { predictionError_ = error; }
    
private:
    struct Impl;
    Impl* pImpl;
};

// Acetylcholine: Attention and memory consolidation
// NOW IMPLEMENTED: Real attention modulation and working memory enhancement
class Acetylcholine {
public:
    Acetylcholine();
    ~Acetylcholine();
    
    // Initialize with brain reference
    void initialize(class Brain* brain);
    
    // Get current level
    float getLevel() const;
    void setLevel(float level);
    
    // Update state
    void update(TimestepDuration dt);
    
    // Apply effects on neural excitability
    void applyExcitabilityModulation(Brain* brain, float level);
    
    // Enhance working memory encoding
    void enhanceWorkingMemoryEncoding(Brain* brain, float level);
    
    // Facilitate memory consolidation
    void facilitateMemoryConsolidation(Brain* brain, float level);
    
    // Get decay rate
    float getDecayRate() const { return decayRate_; }
    void setDecayRate(float rate) { decayRate_ = rate; }
    
private:
    struct Impl;
    Impl* pImpl;
    float decayRate_;
};

// Norepinephrine: Arousal and vigilance
// NOW IMPLEMENTED: Real arousal and attention modulation
class Norepinephrine {
public:
    Norepinephrine();
    ~Norepinephrine();
    
    // Initialize with brain reference
    void initialize(class Brain* brain);
    
    // Get current level
    float getLevel() const;
    void setLevel(float level);
    
    // Update state
    void update(TimestepDuration dt);
    
    // Apply effects on arousal
    void applyArousalModulation(Brain* brain, float level);
    
    // Enhance signal-to-noise ratio
    void enhanceSignalToNoise(Brain* brain, float level);
    
    // Modulate attention
    void modulateAttention(Brain* brain, float level);
    
    // Get decay rate
    float getDecayRate() const { return decayRate_; }
    void setDecayRate(float rate) { decayRate_ = rate; }
    
private:
    struct Impl;
    Impl* pImpl;
    float decayRate_;
};

// Serotonin: Mood, impulsivity, and social behavior
// NOW IMPLEMENTED: Real mood and behavioral regulation
class Serotonin {
public:
    Serotonin();
    ~Serotonin();
    
    // Initialize with brain reference
    void initialize(class Brain* brain);
    
    // Get current level
    float getLevel() const;
    void setLevel(float level);
    
    // Update state
    void update(TimestepDuration dt);
    
    // Apply effects on mood
    void applyMoodModulation(Brain* brain, float level);
    
    // Regulate impulsivity
    void regulateImpulsivity(Brain* brain, float level);
    
    // Modulate social behavior
    void modulateSocialBehavior(Brain* brain, float level);
    
    // Get decay rate
    float getDecayRate() const { return decayRate_; }
    void setDecayRate(float rate) { decayRate_ = rate; }
    
private:
    struct Impl;
    Impl* pImpl;
    float decayRate_;
};

} // namespace nlm
