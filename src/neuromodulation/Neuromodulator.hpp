#pragma once

#include "../core/Types/Types.hpp"
#include <string>
#include <vector>
#include <cmath>
#include <algorithm>

namespace nlm {

// Neuromodulator: Abstract base for neuromodulatory signals
class Neuromodulator {
public:
    virtual ~Neuromodulator() = default;
    
    // Get modulator name
    virtual const char* getName() const = 0;
    
    // Get current concentration/level (0.0 to 1.0 normalized)
    virtual float getLevel() const = 0;
    virtual void setLevel(float level) = 0;
    
    // Apply neuromodulatory effect to plasticity (1.0 = baseline)
    virtual float getPlasticityFactor() const = 0;
    
    // Update neuromodulator state
    virtual void update(TimestepDuration dt) = 0;
    
    // Signal external event (e.g., reward, punishment)
    virtual void signalEvent(float value) {}
    
protected:
    Neuromodulator() = default;
    
    // Clamp level to valid range
    float clampLevel(float level) const {
        return std::clamp(level, 0.0f, 1.0f);
    }
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
    
    // Reward prediction error signaling (biologically realistic)
    void signalReward(float reward);
    void signalRewardPredictionError(float error);
    
    // Configure dopamine dynamics
    void configure(float baselineLevel, float peakLevel, float decayRate, float releaseRate,
                   float learningRateGain, float burstPhasingFactor);
    
    // Get dopamine burst/pause state
    bool isInBurst() const;
    bool isInPause() const;
    float getBurstPhase() const;
    
    // Get current reward prediction error
    float getRPE() const;
    
    // Reset dopamine to baseline
    void reset();
    
    // Getter for internal fields (for NeuromodulatorManager)
    float getBaseline() const;
    float getPeak() const;
    float getDecayRate() const;
    float getReleaseRate() const;
    float getLearningRateGain() const;
    float getBurstPhasingFactor() const;
    
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
    
    // Configure ACh dynamics for attention and filtering
    void configure(float baselineLevel, float filteringStrength, float attentionGain,
                   float memoryGain, float filteringTimeConstant);
    
    // Signal external stimulus for attention modulation
    void signalStimulus(float intensity);
    void signalNovelty(float noveltyLevel);
    
    // Get current attention state
    bool isAttentive() const;
    float getFilteringOutput() const;
    
    // Reset ACh to baseline
    void reset();
    
    // Getter for internal fields (for NeuromodulatorManager)
    float getBaseline() const;
    float getFilteringStrength() const;
    float getAttentionGain() const;
    float getMemoryGain() const;
    float getFilteringTimeConstant() const;
    
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
    
    // Configure NE dynamics for arousal and stress response
    void configure(float baselineLevel, float arousalGain, float vigilanceGain,
                   float stressSensitivity, float recoveryRate);
    
    // Signal stress or alertness events
    void signalStress(float stressLevel);
    void signalAlertness(float alertnessLevel);
    
    // Get current arousal and vigilance state
    float getArousalLevel() const;
    float getVigilanceLevel() const;
    bool isAlert() const;
    
    // Reset NE to baseline
    void reset();
    
    // Getter for internal fields (for NeuromodulatorManager)
    float getBaseline() const;
    float getArousalGain() const;
    float getVigilanceGain() const;
    float getStressSensitivity() const;
    float getRecoveryRate() const;
    
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
    
    // Configure 5-HT dynamics for mood regulation
    void configure(float baselineLevel, float moodGain, float impulsivityGain,
                   float socialSensitivity, float recoveryTimeConstant);
    
    // Signal mood-related or impulsivity-related events
    void signalMoodEvent(float moodValue);
    void signalImpulsivity(float impulsivityLevel);
    void signalSocialEvent(float socialCohesion);
    
    // Get current mood and impulsivity state
    float getMoodLevel() const;
    float getImpulsivityLevel() const;
    bool isSociallyCooperative() const;
    
    // Reset 5-HT to baseline
    void reset();
    
    // Getter for internal fields (for NeuromodulatorManager)
    float getBaseline() const;
    float getMoodGain() const;
    float getImpulsivityGain() const;
    float getSocialSensitivity() const;
    float getRecoveryTimeConstant() const;
    
    float getSocialCohesion() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

// NeuromodulatorManager: Coordinate all neuromodulators and their interactions
class NeuromodulatorManager {
public:
    NeuromodulatorManager();
    ~NeuromodulatorManager();
    
    // Initialize with brain reference for integration
    void initialize(class Brain* brain);
    
    // Update all neuromodulators
    void updateAll(TimestepDuration dt);
    
    // Process integrated signals and cross-talk
    void processIntegration(TimestepDuration dt);
    
    // Apply all neuromodulators to the brain
    void applyAll(class Brain* brain);
    
    // Get combined neuromodulatory effect
    float getCombinedEffect() const;
    
    // Signal external events through all relevant neuromodulators
    void signalEvents(float reward, float predictionError, float novelty, 
                      float stress, float mood, float socialCohesion);
    
    // Get individual neuromodulator references
    Dopamine* getDopamine() const;
    Acetylcholine* getAcetylcholine() const;
    Norepinephrine* getNorepinephrine() const;
    Serotonin* getSerotonin() const;
    
    // Reset all neuromodulators to baseline
    void reset();
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm