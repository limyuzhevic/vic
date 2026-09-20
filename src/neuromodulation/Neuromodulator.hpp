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
    
    // Get time constant for dynamics
    virtual float getTimeConstant() const { return 1.0f; }
    
    // Get baseline level for homeostasis
    virtual float getBaseline() const { return 0.0f; }
    
    // Get decay rate
    virtual float getDecayRate() const { return 0.1f; }
    
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
    
private:
    struct Impl;
    Impl* pImpl;
};

// Acetylcholine: Cholinergic burst-pause signaling, attention modulation, memory consolidation
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
    
    // Burst-pause dynamics for attention modulation
    void signalBurst();  // Phasic burst for attention
    void signalPause();  // Phasic pause for memory consolidation
    void updateAttention(float attentionLevel, float noveltyLevel);
    
    // Memory consolidation during pauses
    void consolidateMemory(float memoryStrength);
    
    // Cholinergic receptor dynamics
    float getMuscarinicEffect() const;
    float getNicotinicEffect() const;
    
    // Vesicular release and reuptake
    void triggerVesicularRelease();
    void setReuptakeRate(float rate);
    
    // Time constants for different processes
    float getRiseTime() const { return riseTime_; }
    float getFallTime() const { return fallTime_; }
    
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
    
    // Locus coeruleus firing modes
    void signalTonic();      // Tonic baseline firing (adenosine modulation)
    void signalPhasic();     // Phasic burst response (novelty/arousal)
    void updateArousal(float salience, float stressLevel);
    
    // Reuptake and clearance mechanisms
    void setReuptakeRate(float rate);
    void triggerVesicularRelease();
    
    // ADRB2 receptor dynamics for plasticity
    float getBetaAdrenergicEffect() const;
    float getAlphaAdrenergicEffect() const;
    
    // Arousal regulation parameters
    float getArousalLevel() const { return arousalLevel_; }
    float getStressLevel() const { return stressLevel_; }
    
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
    
    // Raphe nucleus firing dynamics
    void signalModulatory();      // Modulatory signaling (basal forebrain)
    void signalBurst();           // Burst firing (social behavior)
    void signalInhibition();      // Inhibitory signaling (anxiety suppression)
    
    // Receptor subtype dynamics
    float getFiveHT1AEffect() const;   // Anxiolytic, inhibitory
    float getFiveHT2AEffect() const;   // Excitatory, cognitive
    float getFiveHT3Effect() const;    // Fast, excitatory
    
    // Reuptake and clearance
    void setReuptakeRate(float rate);
    void triggerVesicularRelease();
    
    // Social behavior modulation
    void updateSocialContext(float socialCohesion, float stressLevel);
    
    // Impulse control mechanism
    void updateImpulseControl(float currentBehavior, float targetBehavior);
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
