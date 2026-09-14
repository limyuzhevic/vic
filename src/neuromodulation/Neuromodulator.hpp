#pragma once

#include "../core/Types/Types.hpp"
#include "../memory/NeuralWorkingMemory.hpp"
#include "../cognition/AttentionalSelection.hpp"
#include "../brain/Brain.hpp"
#include <memory>
#include <optional>

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
    
    // State access
    float getPredictionError() const;
    float getTemporalDifference() const;
    float getExpectedValue() const;
    float getBurstIntensity() const;
    float getAdaptivePlasticity() const;
    
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
    void signalAttention(float attentionStrength);
    void enableMemoryConsolidation(bool enable);
    
    // Working memory enhancement
    void enhanceWorkingMemory(NeuralWorkingMemory* workingMemory);
    
    // State access
    float getCholinergicTone() const;
    float getMemoryBoost() const;
    float getAttentionGain() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

// Norepinephrine: Arousal and vigilance
// PLACEHOLDER - Phase 2
class Norepinephrine : public Neuromodulator {
public:
    const char* getName() const override { return "NE"; }
    float getLevel() const override { return 0.0f; }
    void setLevel(float level) override {}
    float getPlasticityFactor() const override { return 1.0f; }
    void update(TimestepDuration dt) override {}
    
    // Arousal and vigilance control
    void increaseArousal(float amount);
    void decreaseArousal(float amount);
    void triggerVigilance(boost::optional<float> duration = boost::none);
    
    // State access
    float getArousalLevel() const;
    float getVigilanceLevel() const;
    float getLocusCoeruleusActivity() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

// Serotonin: Mood, impulsivity, and social behavior
// PLACEHOLDER - Phase 2
class Serotonin : public Neuromodulator {
public:
    const char* getName() const override { return "5-HT"; }
    float getLevel() const override { return 0.0f; }
    void setLevel(float level) override {}
    float getPlasticityFactor() const override { return 1.0f; }
    void update(TimestepDuration dt) override {}
    
    // Mood regulation
    void increaseMood(float amount);
    void decreaseMood(float amount);
    void modulateImpulsivity(float amount);
    void triggerSocialBehavior(bool socialInteraction);
    
    // State access
    float getMoodLevel() const;
    float getImpulseControl() const;
    float getSocialMotivation() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

// Neuromodulator interaction and cross-talk
class NeuromodulatorManager {
public:
    NeuromodulatorManager();
    ~NeuromodulatorManager();
    
    void initialize(Brain* brain);
    void update(TimestepDuration dt);
    
    // Get neuromodulators
    Dopamine* getDopamine();
    Acetylcholine* getAcetylcholine();
    Norepinephrine* getNorepinephrine();
    Serotonin* getSerotonin();
    
    // Cross-talk mechanisms
    void propagateRewardPrediction(Dopamine* dopamine);
    void enhanceAttentionThroughACh(Acetylcholine* ach, Norepinephrine* ne);
    void regulateMoodSerotonin(Acetylcholine* ach, Serotonin* serotonin);
    void maintainArousalBalance(Norepinephrine* ne, Dopamine* dopamine);
    
    // Integrated neuromodulation
    void applyNeuromodulationToBrain();
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
