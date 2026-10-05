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
    
protected:
    Neuromodulator() = default;
};

// Dopamine: Reward and reinforcement learning signal
// COMPLETE - Full implementation with reward prediction error
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
    
    // Working memory integration
    void signalWorkingMemoryAccess(float activation);
    void modulateAttention(float attentionSignal);
    
private:
    struct Impl;
    Impl* pImpl;
};

// Acetylcholine: Attention and memory consolidation
// COMPLETE - Full implementation
class Acetylcholine : public Neuromodulator {
public:
    Acetylcholine();
    ~Acetylcholine() override;
    
    const char* getName() const override { return "ACh"; }
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Memory encoding enhancement
    void signalSensoryInput(float inputStrength, float novelty);
    void signalWorkingMemoryAccess(class Brain* brain, NeuronId neuron, float activation);
    void updateEncoding(float inputPatternStrength);
    
    // Integration with attention
    void updateAttention(float topDownBias, float bottomUpSalience, float goalRelevance);
    
    // Getters for derived effects
    float getMemoryEnhancement() const;
    float getEncodingBoost() const;
    
    // History tracking
    const std::vector<float>& getHistory() const;
    void addToHistory(float value);
    void reset();
    
private:
    struct Impl;
    Impl* pImpl;
};

// Norepinephrine: Arousal and vigilance
// COMPLETE - Full implementation
class Norepinephrine : public Neuromodulator {
public:
    Norepinephrine();
    ~Norepinephrine() override;
    
    const char* getName() const override { return "NE"; }
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Arousal modulation
    void signalArousal(float stimulus);
    void signalAttention(float attentionSignal);
    
    // Integration with prediction systems
    void updateArousal(float baseline, float novelty, float predictionError);
    void updateAttention(float topDownBias, float bottomUpSalience, float goalRelevance);
    
    // Getters for derived effects
    float getArousal() const;
    float getAttention() const;
    
    // History tracking
    const std::vector<float>& getHistory() const;
    void addToHistory(float value);
    void reset();
    
private:
    struct Impl;
    Impl* pImpl;
};

// Serotonin: Mood, impulsivity, and social behavior
// COMPLETE - Full implementation
class Serotonin : public Neuromodulator {
public:
    Serotonin();
    ~Serotonin() override;
    
    const char* getName() const override { return "5-HT"; }
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Mood regulation
    void signalReward(float reward);
    void signalPredictionError(float error);
    void updateMood(float predictedReward, float actualReward);
    
    // Behavioral inhibition
    float getBehavioralInhibition() const;
    
    // Integration with working memory
    void signalWorkingMemoryUpdate(float activation, float stability);
    
    // History tracking
    const std::vector<float>& getHistory() const;
    void addToHistory(float value);
    void reset();
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm