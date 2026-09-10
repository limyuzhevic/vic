#pragma once

#include "../core/Types/Types.hpp"
#include <string>
#include <vector>

namespace nlm {

// Neuromodulator: Abstract base for neuromodulatory signals
// Enhanced with brain reference for integration

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
    
    // Connect to brain for integration
    virtual void initialize(class Brain* brain) {}
    
    // Process sensory input
    virtual void processInput(const std::vector<float>& input) {}
    
    // Get neuromodulator effect on specific brain region
    virtual float getEffectOnRegion(class NeuralRegion* region) const { return 1.0f; }
    
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
    void initialize(class Brain* brain) override;
    
    // Reward signaling
    void signalReward(float reward);
    void signalRewardPredictionError(float error);
    
    // Memory consolidation effects
    void enhanceMemoryConsolidation(float strength);
    
    // Learning modulation
    void modulateSTDP(float predictedError);
    
private:
    struct Impl;
    Impl* pImpl;
};

// Acetylcholine: Attention and memory consolidation
class Acetylcholine : public Neuromodulator {
public:
    Acetylcholine();
    ~Acetylcholine() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    void initialize(class Brain* brain) override;
    
    // Attention modulation
    void modulateAttention(float attentionStrength);
    
    // Memory encoding
    void enhanceMemoryEncoding(float noveltyLevel);
    
    // Working memory maintenance
    void maintainWorkingMemory(float activation);
    
private:
    struct Impl;
    Impl* pImpl;
};

// Norepinephrine: Arousal and vigilance
class Norepinephrine : public Neuromodulator {
public:
    Norepinephrine();
    ~Norepinephrine() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    void initialize(class Brain* brain) override;
    
    // Arousal modulation
    void modulateArousal(float novelty, float predictionError);
    
    // Alertness and focus
    void modulateAlertness(float sustainedAttention);
    
    // Stress response
    void handleStress(float threatLevel);
    
private:
    struct Impl;
    Impl* pImpl;
};

// Serotonin: Mood, impulsivity, and social behavior
class Serotonin : public Neuromodulator {
public:
    Serotonin();
    ~Serotonin() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    void initialize(class Brain* brain) override;
    
    // Mood regulation
    void modulateMood(float environmentalRiches);
    
    // Behavioral inhibition
    void modulateImpulsivity(float urgency);
    
    // Social behavior
    void modulateSocialBehavior(float socialContext);
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
