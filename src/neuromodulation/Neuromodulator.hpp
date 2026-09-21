#pragma once

#include "../core/Types/Types.hpp"
#include <string>
#include <vector>
#include <random>
#include <cmath>

namespace nlm {

// Neuromodulator: Abstract base for neuromodulatory signals
// Core neuromodulators for biologically inspired learning

class Neuromodulator {
public:
    virtual ~Neuromodulator() = default;
    
    // Get modulator name
    virtual const char* getName() const = 0;
    
    // Get current concentration/level (-1.0 to 1.0)
    virtual float getLevel() const = 0;
    virtual void setLevel(float level) = 0;
    
    // Apply neuromodulatory effect to plasticity
    // Returns a factor that modulates learning rates (0.1 to 2.0 typically)
    virtual float getPlasticityFactor() const = 0;
    
    // Update neuromodulator state based on learning history
    virtual void update(TimestepDuration dt) = 0;
    
    // Reset modulator to baseline
    virtual void reset() {}
    
    // Get target level for modulation
    virtual float getTargetLevel() const { return 0.0f; }
    
    // Set target level (for gradual modulation)
    virtual void setTargetLevel(float target) {}
    
    // Get modulation strength (0.0 to 1.0)
    virtual float getModulationStrength() const { return 1.0f; }
    
    // Is this modulator currently active?
    virtual bool isActive() const { return std::abs(getLevel()) > 0.001f; }
    
    // Get adaptation time constant (in seconds)
    virtual float getTimeConstant() const { return 1.0f; }
    
    // Decay rate (fractional decay per timestep)
    virtual float getDecayRate() const { return 0.01f; }
    
protected:
    Neuromodulator() {}
};

// Dopamine: Reward and reinforcement learning signal
// Biological implementation with realistic dynamics
class Dopamine : public Neuromodulator {
public:
    Dopamine();
    ~Dopamine() override;
    
    const char* getName() const override { return "Dopamine"; }
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    void reset() override;
    
    // Reward signaling
    void signalReward(float reward);
    void signalRewardPredictionError(float error);
    
    // Reward history
    const std::vector<float>& getHistory() const { return history_; }
    void clearHistory() { history_.clear(); }
    
    // Get tonic and phasic dopamine levels
    float getTonicLevel() const { return tonicLevel_; }
    float getPhasicLevel() const { return phasicLevel_; }
    
    // Set baseline tonic level
    void setTonicLevel(float tonic) { tonicLevel_ = tonic; }
    
    // Set modulation strength
    void setModulationStrength(float strength) { modulationStrength_ = strength; }
    
private:
    struct Impl;
    Impl* pImpl;
    
    // Reward history for prediction error calculation
    std::vector<float> history_;
    
    // Biological components
    float tonicLevel_;
    float phasicLevel_;
    float predictionError_;
    float expectedReward_;
    float modulationStrength_;
    
    // State tracking
    float currentTime_;
    float targetLevel_;
    float decayRate_;
    float adaptationTimeConstant_;
    float phi_;
    
    // Random generator for exploration
    mutable std::mt19937 rng_;
    mutable std::uniform_real_distribution<float> uniformDist_;
    mutable std::normal_distribution<float> normalDist_;
};

// Acetylcholine: Attention and memory consolidation
// Real implementation with attentional modulation
class Acetylcholine : public Neuromodulator {
public:
    Acetylcholine();
    ~Acetylcholine() override;
    
    const char* getName() const override { return "Acetylcholine (ACh)"; }
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    void reset() override;
    
    // Attention modulation
    void signalNovelty(float novelty);
    void signalAttention(float attentionSignal);
    void signalMemoryDemand(float demand);
    
    // Get attentional state
    float getAttentionLevel() const { return attentionLevel_; }
    float getNoveltyLevel() const { return noveltyLevel_; }
    float getMemoryDemand() const { return memoryDemand_; }
    
    // Learning modulation
    void setLearningModulation(float modulation) { learningModulation_ = modulation; }
    void setMemoryModulation(float modulation) { memoryModulation_ = modulation; }
    
private:
    struct Impl;
    Impl* pImpl;
    
    // Attentional state variables
    float attentionLevel_;
    float noveltyLevel_;
    float memoryDemand_;
    float learningModulation_;
    float memoryModulation_;
    
    // State tracking
    float currentTime_;
    float targetLevel_;
    float cholinergicReceptorDensity_;
    float adaptiveCapacity_;
    
    // Neural correlates
    float frontalLobeActivity_;
    float prefrontalActivation_;
    float workingMemoryLoad_;
    
    // Biological parameters
    float synthesisRate_;
    float reuptakeRate_;
    float receptorAffinity_;
    float tonicBaseline_;
};

// Norepinephrine: Arousal and vigilance
// Real implementation with locus coeruleus dynamics
class Norepinephrine : public Neuromodulator {
public:
    Norepinephrine();
    ~Norepinephrine() override;
    
    const char* getName() const override { return "Norepinephrine (NE)"; }
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    void reset() override;
    
    // Arousal modulation
    void signalStress(float stress);
    void signalExcitement(float excitement);
    void signalAlertness(float alertness);
    
    // Cognitive modulation
    float getCognitiveModulation() const { return cognitiveModulation_; }
    float getSensoryGating() const { return sensoryGating_; }
    float getWorkingMemoryUpdate() const { return workingMemoryUpdate_; }
    
    // Set neuromodulatory parameters
    void setBaselineArousal(float baseline) { baselineArousal_ = baseline; }
    void setResponseGain(float gain) { responseGain_ = gain; }
    
private:
    struct Impl;
    Impl* pImpl;
    
    // Arousal state variables
    float arousalLevel_;
    float stressLevel_;
    float cognitiveModulation_;
    float sensoryGating_;
    float workingMemoryUpdate_;
    
    // Brain region activities
    float locusCoeruleusActivity_;
    float pontineActivity_;
    float thalamicActivity_;
    
    // Neurochemical dynamics
    float baselineArousal_;
    float responseGain_;
    float saturationPoint_;
    float tonicRelease_;
    float phasicRelease_;
    
    // State tracking
    float currentTime_;
    float targetLevel_;
    float adaptationRate_;
    float recoveryRate_;
    
    // Biological parameters
    float synthesisRate_;
    float degradationRate_;
    float receptorDensity_;
    float reuptakeEfficiency_;
};

// Serotonin: Mood, impulsivity, and social behavior
// Real implementation with raphe nucleus dynamics
class Serotonin : public Neuromodulator {
public:
    Serotonin();
    ~Serotonin() override;
    
    const char* getName() const override { return "Serotonin (5-HT)"; }
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    void reset() override;
    
    // Mood modulation
    void signalRewardOutcome(float outcome);
    void signalSocialBonding(float bonding);
    void signalPunishment(float punishment);
    
    // Behavioral modulation
    float getMoodLevel() const { return moodLevel_; }
    float getImpulsivityLevel() const { return impulsivityLevel_; }
    float getAnhedoniaLevel() const { return anhedoniaLevel_; }
    
    // Learning rate modulation
    float getLearningRateModulation() const { return learningRateMod_; }
    float getExtinctionModulation() const { return extinctionMod_; }
    
    // Set social parameters
    void setSocialCohesion(float cohesion) { socialCohesion_ = cohesion; }
    void setDominanceLevel(float level) { dominanceLevel_ = level; }
    
private:
    struct Impl;
    Impl* pImpl;
    
    // Mood state variables
    float moodLevel_;
    float impulsivityLevel_;
    float anhedoniaLevel_;
    float learningRateMod_;
    float extinctionMod_;
    
    // Raphe nucleus activity
    float dorsalRapheActivity_;
    float medianRapheActivity_;
    float ponsCoeruleusActivity_;
    
    // Neurochemical dynamics
    float tonicRelease_;
    float phasicRelease_;
    float synthesisRate_;
    float reuptakeRate_;
    float receptorSensitivity_;
    
    // Behavioral correlates
    float socialCohesion_;
    float dominanceLevel_;
    float anxietyLevel_;
    float rewardSensitivity_;
    
    // State tracking
    float currentTime_;
    float targetLevel_;
    float adaptationTimeConstant_;
    float feedbackDelay_;
    float homeostaticRegulation_;
    
    // Biological parameters
    float synthesisCapacity_;
    float enzymaticActivity_;
    float receptorDensity_;
    float metabolicClearance_;
};

} // namespace nlm
