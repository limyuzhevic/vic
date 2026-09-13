#pragma once

#include "../cognition/ConceptFormation.hpp"
#include "../cognition/SocialLearning.hpp"

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
// PLACEHOLDER - Phase 2
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
    
    // Enhanced dopamine functions for full integration
    void enhanceWorkingMemory(class NeuralWorkingMemory* workingMemory, float factor = 1.0f);
    void enhanceEpisodicMemory(class NeuralEpisodicMemory* episodicMemory, float consolidation = 1.0f);
    void modulateAttention(class AttentionalSelection* attention, float bias = 1.0f);
    void modulateConceptFormation(ConceptFormation* conceptFormation, float threshold = 1.0f);
    
    // Get enhanced modulation factors
    float getMemoryConsolidationFactor() const;
    float getAttentionalModulation() const;
    float getConceptModulation() const;
    
    // Reward prediction interaction
    void interactWithRewardPrediction(float predictionError, float expectedValue);
    
    // Integration with other neuromodulators
    void integrateWithCuriosity(float curiosityLevel);
    void integrateWithNovelty(float noveltyLevel);
    
private:
    struct Impl;
    Impl* pImpl;
};

class Novelty : public Neuromodulator {
public:
    Novelty();
    ~Novelty() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Novelty detection for exploration
    void detectNovelty(float activation, float noveltyThreshold);
    
    // Integration with other systems
    void integrateWithCuriosity(float curiosityLevel);
    
private:
    struct Impl;
    Impl* pImpl;
};

// Acetylcholine: Attention and memory consolidation
// PLACEHOLDER - Phase 2
class Acetylcholine : public Neuromodulator {
public:
    Acetylcholine();
    ~Acetylcholine() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Attention modulation
    void modulateAttention(class AttentionalSelection* attention, float bias = 1.0f);
    
    // Memory consolidation
    void enhanceMemoryConsolidation(class NeuralEpisodicMemory* episodicMemory, float strength = 1.0f);
    void enhanceWorkingMemory(class NeuralWorkingMemory* workingMemory, float factor = 1.0f);
    
    // Signal acetylcholine events
    void signalAttentionEvent();
    void signalMemoryEvent();
    
    // Integration with other systems
    void integrateWithDopamine(float dopamineLevel);
    void integrateWithPredictionError(float predictionError);
    
private:
    struct Impl;
    Impl* pImpl;
};

// Norepinephrine: Arousal and vigilance
// PLACEHOLDER - Phase 2
class Norepinephrine : public Neuromodulator {
public:
    Norepinephrine();
    ~Norepinephrine() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Arousal and vigilance effects
    void modulateArousal(float arousalLevel);
    void enhanceVigilance(class AttentionalSelection* attention, float vigilanceBoost = 1.0f);
    
    // Integration with other systems
    void integrateWithDopamine(float dopamineLevel);
    void integrateWithAcetylcholine(float achLevel);
    
private:
    struct Impl;
    Impl* pImpl;
};

// Serotonin: Mood, impulsivity, and social behavior
// PLACEHOLDER - Phase 2
class Serotonin : public Neuromodulator {
public:
    Serotonin();
    ~Serotonin() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Mood and social behavior effects
    void modulateMood(float moodLevel);
    void modulateImpulsivity(float impulsivityLevel);
    void enhanceSocialBehavior(class SocialLearning* socialLearning, float socialBoost = 1.0f);
    
    // Integration with other systems
    void integrateWithDopamine(float dopamineLevel);
    void integrateWithAcetylcholine(float achLevel);
    void integrateWithNorepinephrine(float neLevel);
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm