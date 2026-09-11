#pragma once

#include "../core/Types/Types.hpp"
#include "Neuromodulator.hpp"

namespace nlm {

// Neuromodulator: Abstract base for neuromodulatory signals

class Neuromodulator {
public:
    virtual ~Neuromodulator() = default;
    
    // Get modulator name
    virtual const char* getName() const = 0;
    
    // Get current concentration/level (normalized 0.0 to 1.0)
    virtual float getLevel() const = 0;
    virtual void setLevel(float level) = 0;
    
    // Apply neuromodulatory effect to plasticity
    // Returns factor by which to scale plasticity (1.0 = baseline)
    virtual float getPlasticityFactor() const = 0;
    
    // Update neuromodulator state based on time
    virtual void update(TimestepDuration dt) = 0;
    
    // Reset neuromodulator to baseline
    virtual void reset() {}
    
    // Apply neuromodulation effects to neural network
    // This is a helper function that applies this neuromodulator's effects
    // to a brain instance
    virtual void applyToBrain(class Brain* brain) const {}
    
protected:
    Neuromodulator() = default;
};

// Base implementation for neuromodulator helper functions
// These functions coordinate neuromodulation effects across the brain
namespace NeuromodulationHelpers {
    // Apply all neuromodulators to a brain in order
    // Order: ACh (attention), NE (arousal), DA (reward), 5-HT (mood)
    void ApplyAllNeuromodulators(Brain* brain, const Neuromodulator& ach, 
                                 const Neuromodulator& ne, 
                                 const Neuromodulator& da, 
                                 const Neuromodulator& serotonin);
    
    // Apply neuromodulation effects to neuron excitability
    void ApplyNeuromodulationToExcitability(Brain* brain, 
                                           const Neuromodulator& ach, 
                                           const Neuromodulator& ne,
                                           const Neuromodulator& da);
    
    // Apply neuromodulation effects to plasticity
    void ApplyNeuromodulationToPlasticity(Brain* brain, 
                                          const Neuromodulator& ach,
                                          const Neuromodulator& da);
    
    // Apply neuromodulation effects to memory systems
    void ApplyNeuromodulationToMemory(Brain* brain, 
                                      const Neuromodulator& ach,
                                      const Neuromodulator& da);
    
    // Apply neuromodulation effects to attention
    void ApplyNeuromodulationToAttention(Brain* brain, 
                                         const Neuromodulator& ach,
                                         const Neuromodulator& ne);
    
    // Apply neuromodulation effects to prediction system
    void ApplyNeuromodulationToPrediction(Brain* brain,
                                          const Neuromodulator& ne,
                                          const Neuromodulator& da);
}

// Dopamine: Reward and reinforcement learning signal
// Based on computational models of dopamine dynamics
class Dopamine : public Neuromodulator {
public:
    Dopamine();
    ~Dopamine() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    void reset() override;
    
    // Reward signaling
    void signalReward(float reward);
    void signalRewardPredictionError(float error);
    void computePredictionError(float predicted, float actual);
    
    // Get prediction error
    float getPredictionError() const;
    
    // Get expected value
    float getExpectedValue() const;
    void setExpectedValue(float value);
    
    // Get reward history
    const std::vector<float>& getRewardHistory() const { return rewardHistory_; }
    void clearRewardHistory() { rewardHistory_.clear(); }
    
    // Apply dopamine effects to neural excitability
    void applyToBrain(Brain* brain) const override;
    
private:
    struct Impl;
    Impl* pImpl;
    std::vector<float> rewardHistory_;
};

// Acetylcholine: Attention and memory consolidation
// Based on cholinergic models of attention and memory encoding
class Acetylcholine : public Neuromodulator {
public:
    Acetylcholine();
    ~Acetylcholine() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    void reset() override;
    
    // Attention modulation
    void modulateAttention(float attentionLevel);
    float getAttentionModulation() const;
    
    // Memory encoding enhancement
    void enhanceEncoding(float encodingStrength);
    void enhanceMemoryConsolidation(float consolidationFactor);
    
    // Apply cholinergic effects
    void applyToBrain(Brain* brain) const override;
    
    // Get memory enhancement factor
    float getMemoryEnhancement() const { return memoryEnhancement_; }
    
private:
    struct Impl;
    Impl* pImpl;
    float attentionLevel_;
    float memoryEnhancement_;
    std::vector<float> encodingHistory_;
};

// Norepinephrine: Arousal and vigilance
// Based on noradrenergic models of signal-to-noise enhancement
class Norepinephrine : public Neuromodulator {
public:
    Norepinephrine();
    ~Norepinephrine() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    void reset() override;
    
    // Arousal and vigilance modulation
    void increaseArousal(float arousalLevel);
    float getArousal() const { return arousal_; }
    
    // Signal-to-noise ratio enhancement
    float enhanceSignalToNoiseRatio(float baseSignal, float noise) const;
    void enhanceLearning(float learningRate) { learningRate_ = learningRate; }
    
    // Apply noradrenergic effects
    void applyToBrain(Brain* brain) const override;
    
private:
    struct Impl;
    Impl* pImpl;
    float arousal_;
    float learningRate_;
    std::vector<float> signalNoiseHistory_;
};

// Serotonin: Mood, impulsivity, and social behavior
// Based on serotonin models of mood regulation and impulsivity control
class Serotonin : public Neuromodulator {
public:
    Serotonin();
    ~Serotonin() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    void reset() override;
    
    // Mood regulation
    void regulateMood(float moodLevel);
    float getMoodLevel() const { return mood_; }
    
    // Impulsivity control
    void controlImpulsivity(float impulseControl);
    float getImpulseControl() const { return impulseControl_; }
    
    // Social behavior influence
    void influenceSocialBehavior(float socialInfluence);
    void enhanceSocialLearning(float socialLearningStrength);
    
    // Apply serotonergic effects
    void applyToBrain(Brain* brain) const override;
    
    // Get decision making bias
    float getDecisionMakingBias() const { return decisionBias_; }
    
private:
    struct Impl;
    Impl* pImpl;
    float mood_;
    float impulseControl_;
    float socialInfluence_;
    float socialLearningStrength_;
    float decisionBias_;
    std::vector<float> moodHistory_;
};

} // namespace nlm