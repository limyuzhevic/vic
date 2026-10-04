#pragma once

#include "../core/Types/Types.hpp"
#include <string>
#include <vector>
#include <memory>

namespace nlm {

// Neuromodulator: Abstract base for neuromodulatory signals
// Enhanced with full system integration

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
    
    // Enhanced neuromodulator interfaces
    virtual void initialize(class Brain* brain) {}
    virtual void applyToNeuralExcitability() {}
    virtual void applyToPlasticity(STDP* stdp, Hebbian* hebbian) {}
    virtual void applyToMemory(class NeuralEpisodicMemory* episodicMemory) {}
    virtual void applyToAttention(class AttentionalSelection* attention) {}
    virtual void applyToDevelopment(class DevelopmentSystem* development) {}
    
protected:
    Neuromodulator() = default;
};

// Dopamine: Reward and reinforcement learning signal
// Full implementation
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
    
    // Enhanced neuromodulation effects
    void initialize(Brain* brain) override;
    void applyToNeuralExcitability() override;
    void applyToPlasticity(STDP* stdp, Hebbian* hebbian) override;
    void applyToMemory(class NeuralEpisodicMemory* episodicMemory) override;
    void applyToAttention(class AttentionalSelection* attention) override;
    void applyToDevelopment(class DevelopmentSystem* development) override;
    
    // Reward history
    void recordReward(float reward);
    float getAverageReward() const;
    size_t getRewardCount() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

// Acetylcholine: Attention and memory consolidation
// Full implementation
class Acetylcholine : public Neuromodulator {
public:
    Acetylcholine();
    ~Acetylcholine() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Enhanced neuromodulation effects
    void initialize(Brain* brain) override;
    void applyToNeuralExcitability() override;
    void applyToPlasticity(STDP* stdp, Hebbian* hebbian) override;
    void applyToMemory(class NeuralEpisodicMemory* episodicMemory) override;
    void applyToAttention(class AttentionalSelection* attention) override;
    void applyToDevelopment(class DevelopmentSystem* development) override;
    
    // Novelty detection and attention modulation
    void signalNovelty(float novelty);
    void enhanceAttentionDuringNovelty(bool isNovel);
    float getAttentionModulation() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

// Norepinephrine: Arousal and vigilance
// Full implementation
class Norepinephrine : public Neuromodulator {
public:
    Norepinephrine();
    ~Norepinephrine() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Enhanced neuromodulation effects
    void initialize(Brain* brain) override;
    void applyToNeuralExcitability() override;
    void applyToPlasticity(STDP* stdp, Hebbian* hebbian) override;
    void applyToMemory(class NeuralEpisodicMemory* episodicMemory) override;
    void applyToAttention(class AttentionalSelection* attention) override;
    void applyToDevelopment(class DevelopmentSystem* development) override;
    
    // Arousal and stress response
    void signalStress(float stress);
    void enhanceVigilance(bool isThreat);
    float getArousalLevel() const;
    float getVigilanceModulation() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

// Serotonin: Mood, impulsivity, and social behavior
// Full implementation
class Serotonin : public Neuromodulator {
public:
    Serotonin();
    ~Serotonin() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Enhanced neuromodulation effects
    void initialize(Brain* brain) override;
    void applyToNeuralExcitability() override;
    void applyToPlasticity(STDP* stdp, Hebbian* hebbian) override;
    void applyToMemory(class NeuralEpisodicMemory* episodicMemory) override;
    void applyToAttention(class AttentionalSelection* attention) override;
    void applyToDevelopment(class DevelopmentSystem* development) override;
    
    // Social and behavioral modulation
    void signalSocialFeedback(float socialFeedback);
    void modulateImpulsivity(float impulsivity);
    void regulateMood(float mood);
    float getSocialModulation() const;
    float getBehavioralControl() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm