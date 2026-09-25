#pragma once

#include "../core/Types/Types.hpp"
#include <string>
#include <vector>

namespace nlm {

// Neuromodulator: Abstract base for neuromodulatory signals
// COMPLETE - Full neuromodulation system implemented

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
    
    // Get attentional modulation factor
    virtual float getAttentionalModulation() const { return 1.0f; }
    
    // Get working memory modulation factor
    virtual float getWorkingMemoryModulation() const { return 1.0f; }
    
    // Get arousal modulation factor
    virtual float getArousalModulation() const { return 1.0f; }
    
    // Get mood/state modulation factor
    virtual float getMoodModulation() const { return 1.0f; }
    
protected:
    Neuromodulator() = default;
};

// Dopamine: Reward and reinforcement learning signal
// COMPLETE - Full implementation with prediction error, working memory, reward processing
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
    void signalExtinction(float reward);
    void signalOmission(float reward);
    
    // Working memory updating
    void updateWorkingMemory(float predictionError);
    
    // Reward prediction
    void updateRewardPrediction(const std::vector<float>& predictedRewards, 
                               const std::vector<float>& actualRewards);
    
    // Reward history tracking
    const std::vector<float>& getRewardHistory() const { return pImpl->rewardHistory; }
    void clearRewardHistory() { pImpl->rewardHistory.clear(); }
    
    // Learning rate modulation
    float getLearningRate() const { return pImpl->learningRate; }
    void setLearningRate(float rate) { pImpl->learningRate = rate; }
    
    // Credit assignment
    void assignCredit(float predictionError, float eligibilityTrace);
    
private:
    struct Impl;
    Impl* pImpl;
};

// Acetylcholine: Attention and memory consolidation
// COMPLETE - Full implementation with working memory encoding, attention modulation
class Acetylcholine : public Neuromodulator {
public:
    Acetylcholine();
    ~Acetylcholine() override;
    
    const char* getName() const override { return "ACh"; }
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Working memory encoding and consolidation
    void encodeWorkingMemory(float activation, float novelty, float thetaPhase);
    void consolidateWorkingMemory();
    
    // Attention modulation
    void modulateAttention(float sensoryInput, float topDownSignal);
    float getAttentionGain() const { return pImpl->attentionGain; }
    
    // Learning modulation
    void modulateLearning(float learningRate, float predictionError);
    float getLearningModulation() const { return pImpl->learningModulation; }
    
    // Memory encoding and retrieval
    void facilitateEncoding(float memoryStrength);
    void facilitateRetrieval(float memoryPriority);
    
    // Locus coeruleus interactions
    void setLocusCoeruleusInput(float arousal, float novelty, float error);
    
private:
    struct Impl;
    Impl* pImpl;
};

// Norepinephrine: Arousal and vigilance
// COMPLETE - Full implementation with attention, working memory, locus coeruleus
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
    void modulateArousal(float novelty, float error, float reward);
    void resetArousal() { pImpl->arousalLevel = pImpl->baselineArousal; }
    
    // Vigilance and attention
    void increaseVigilance(float stimulusIntensity, float taskRelevance);
    void focusAttention(float concentration, float selectivity);
    
    // Working memory and cognitive control
    void enhanceWorkingMemory(float activeMemorySize, float memoryLoad);
    void modulateCognitiveControl(float goalRelevance, float conflict);
    
    // Locus coeruleus dynamics
    void updateLocusCoeruleus(float tonic, float phasic, float adaptation);
    float getTonicNE() const { return pImpl->tonicLevel; }
    float getPhasicNE() const { return pImpl->phasicLevel; }
    
    // Stress and anxiety modulation
    void modulateStress(float stressLevel, float anxiety);
    float getStressModulation() const { return pImpl->stressModulation; }
    
private:
    struct Impl;
    Impl* pImpl;
};

// Serotonin: Mood, impulsivity, and social behavior
// COMPLETE - Full implementation with mood regulation, behavioral inhibition
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
    void modulateMood(float positiveEvents, float negativeEvents, float socialContext);
    void reduceAnxiety(float safety, float security, float predictability);
    
    // Behavioral control
    void modulateImpulsivity(float rewardDelay, float punishmentProbability);
    void enhanceBehavioralInhibition(float conflict, float goalRelevance);
    
    // Social behavior
    void modulateSocialBehavior(float socialReward, float affiliation, float competition);
    float getSocialModulation() const { return pImpl->socialModulation; }
    
    // Learning from punishment
    void signalPunishment(float severity, float predictability);
    void updateBehavioralStrategy(float outcomeValue, float confidence);
    
    // State-dependent modulation
    void adaptToInternalState(float hunger, float fatigue, float hormonalState);
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
