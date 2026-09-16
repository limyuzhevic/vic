#pragma once

#include "../core/Types/Types.hpp"
#include "../core/Logger/Logger.hpp"
#include "Neuromodulator.hpp"
#include <algorithm>
#include <cmath>
#include <numeric>

namespace nlm {

// Norepinephrine (NE) - Arousal and Vigilance System
class Norepinephrine : public Neuromodulator {
public:
    Norepinephrine();
    ~Norepinephrine() override;
    
    void initialize(Brain* brain);
    
    const char* getName() const override { return "NE"; }
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // NE-specific functions
    void enhanceArousal(float noveltyLevel = 0.0f);
    void increaseFiringThreshold(float thresholdBoost = 0.0f);
    void enhanceSignalDetection(float signalStrength = 0.0f);
    void improveMemoryEncoding(float encodingBoost = 0.0f);
    void enhanceAttention(float focusGain = 0.0f);
    
    // Signal detection and filtering
    float getSignalToNoiseRatio() const;
    float get alertness() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

// Serotonin (5-HT) - Mood, Impulsivity, and Social Behavior System
class Serotonin : public Neuromodulator {
public:
    Serotonin();
    ~Serotonin() override;
    
    void initialize(Brain* brain);
    
    const char* getName() const override { return "5-HT"; }
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // 5-HT specific functions
    void modulateRiskRewardEvaluation(float rewardValue = 0.0f, float riskLevel = 0.0f);
    void influenceImpulseControl(float decisionBias = 0.0f);
    void affectSocialLearning(float socialCohesion = 0.0f);
    void regulateDevelopmentalTiming(float developmentalStage = 0.0f);
    void enhanceExploration(float explorationDrive = 0.0f);
    
    // Behavioral modulation
    float getBehavioralFlexibility() const;
    float getCooperationLevel() const;
    float getRiskAversion() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

// Acetylcholine (ACh) - Memory and Attention System - Full implementation
class Acetylcholine : public Neuromodulator {
public:
    Acetylcholine();
    ~Acetylcholine() override;
    
    void initialize(Brain* brain);
    
    const char* getName() const override { return "ACh"; }
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // ACh-specific functions
    void enhanceWorkingMemory() const;
    void enhanceAttention() const;
    void enhancePlasticity() const;
    void signalAttentionEvent(float noveltyLevel);
    void signalLearningEvent();
    
    // Accessors
    const std::vector<float>& getMemoryTrace() const;
    void clearMemoryTrace();
    float getConsolidationFactor() const;
    float getAttentionGain() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
