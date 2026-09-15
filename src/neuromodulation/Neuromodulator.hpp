#pragma once

#include "../core/Types/Types.hpp"
#include <string>
#include <vector>
#include "../memory/NeuralWorkingMemory.hpp"
#include "../memory/NeuralEpisodicMemory.hpp"
#include "../cognition/AttentionalSelection.hpp"
#include "../brain/Brain.hpp"
#include "../prediction/PredictionSystem.hpp"

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
    
    // Set reward prediction error
    void setRewardPredictionError(float error);
    
    // Get baseline level
    void setBaseline(float baseline);
    float getBaseline() const;
    
    // Get peak level
    void setPeak(float peak);
    float getPeak() const;
    
    // Get decay rate
    void setDecayRate(float rate);
    float getDecayRate() const;
    
    // Get release rate
    void setReleaseRate(float rate);
    float getReleaseRate() const;
    
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
    
    // Attention signaling
    void signalAttention(float attentionSignal);
    
    // Memory consolidation
    void signalMemoryConsolidation();
    
    // Enhance working memory
    void enhanceWorkingMemory(NeuralWorkingMemory* wm);
    
    // Modulate attention
    void modulateAttention(AttentionalSelection* attention);
    
    // Set baseline level
    void setBaseline(float baseline);
    float getBaseline() const;
    
    // Set decay rate
    void setDecayRate(float rate);
    float getDecayRate() const;
    
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
    
    // Arousal signaling
    void signalArousal(float arousalSignal);
    
    // Vigilance
    void signalVigilance();
    
    // Modulate neural excitability
    void modulateExcitability(Brain* brain);
    
    // Enhance signal detection
    void enhanceSignalDetection(class SensoryInput* input);
    
    // Set baseline level
    void setBaseline(float baseline);
    float getBaseline() const;
    
    // Set decay rate
    void setDecayRate(float rate);
    float getDecayRate() const;
    
    // Set arousal response strength
    void setArousalStrength(float strength);
    float getArousalStrength() const;
    
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
    
    // Mood signaling
    void signalMood(float moodSignal);
    
    // Impulsivity control
    void signalImpulsivityControl();
    
    // Social behavior modulation
    void modulateSocialBehavior();
    
    // Modulate plasticity rules
    void modulatePlasticity(class STDP* stdp, class Hebbian* hebbian);
    
    // Reduce impulsivity
    void reduceImpulsivity(class NeuralPlanner* planner);
    
    // Set baseline level
    void setBaseline(float baseline);
    float getBaseline() const;
    
    // Set decay rate
    void setDecayRate(float rate);
    float getDecayRate() const;
    
    // Set social behavior strength
    void setSocialStrength(float strength);
    float getSocialStrength() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm