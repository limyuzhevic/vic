#pragma once

#include "../core/Types/Types.hpp"
#include <string>
#include <vector>
#include <memory>

namespace nlm {

// Neuromodulator: Abstract base for neuromodulatory signals
// Enhanced with comprehensive neuromodulation effects for Phase 6

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
    
    // Neuromodulatory effects on brain systems
    virtual void affectAttention() = 0;
    virtual void affectMemory() = 0;
    virtual void affectArousal() = 0;
    virtual void affectLearning() = 0;
    
protected:
    Neuromodulator() = default;
};

// Dopamine: Reward and reinforcement learning signal
// Enhanced with comprehensive reward processing
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
    
    // Advanced reward processing
    void processRewardHistory(float reward, float prediction);
    void adaptRewardSensitivity(float learningRate);
    void modulateApproachWithdrawal(float balance);
    
    // Neural effects
    void enhanceReconstructionLearning(float intensity);
    void modulateAddictivePotential(float potential);
    void influenceDecisionMaking(float bias);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Acetylcholine: Attention and memory consolidation
// Enhanced Phase 6 implementation with real effects
class Acetylcholine : public Neuromodulator {
public:
    Acetylcholine();
    ~Acetylcholine() override;
    
    const char* getName() const override { return "ACh"; }
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Memory effects
    void enhanceMemoryEncoding(float novelty);
    void facilitateMemoryConsolidation();
    void reduceMemoryReverberation();
    
    // Attention effects
    void increaseSelectiveAttention(float relevance);
    void modulateAttentionGating(float threshold);
    void enhanceTopDownControl(float bias);
    
    // Cognitive effects
    void boostWorkingMemoryCapacity(float capacity);
    void enhancePatternCompletion(float completeness);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Norepinephrine: Arousal and vigilance
// Enhanced Phase 6 implementation with real effects
class Norepinephrine : public Neuromodulator {
public:
    Norepinephrine();
    ~Norepinephrine() override;
    
    const char* getName() const override { return "NE"; }
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Arousal effects
    void modulateArousalLevel(float arousal);
    void enhanceSensoryGain(float gain);
    void increaseAlertness(float level);
    void reduceSleepPropensity(float reduction);
    
    // Cognitive effects
    void enhanceWorkingMemoryUpdating();
    void increaseCognitiveFlexibility(float flexibility);
    void boostExecutiveFunction(float efficiency);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Serotonin: Mood, impulsivity, and social behavior
// Enhanced Phase 6 implementation with real effects
class Serotonin : public Neuromodulator {
public:
    Serotonin();
    ~Serotonin() override;
    
    const char* getName() const override { return "5-HT"; }
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Behavioral effects
    void modulateMood(float mood);
    void reduceImpulsivity(float reduction);
    void enhanceSocialBehavior(float sociality);
    void regulateSleepWakeCycle(float regulation);
    
    // Cognitive effects
    void enhanceCognitiveSet(float set);
    void modulateRiskTaking(float risk);
    void facilitateEmotionalLearning(float learning);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm