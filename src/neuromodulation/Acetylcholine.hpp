#pragma once

#include "../core/Types/Types.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

// Acetylcholine: Attention and memory consolidation
class Acetylcholine : public Neuromodulator {
public:
    Acetylcholine();
    ~Acetylcholine() override;
    
    const char* getName() const override { return "ACh"; }
    float getLevel() const override { return pImpl->level; }
    void setLevel(float level) override { pImpl->level = std::clamp(level, 0.0f, 1.0f); }
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Attention modulation
    float getAttentionGain() const { return pImpl->attentionGain; }
    void setAttentionGain(float gain) { pImpl->attentionGain = gain; }
    
    // Working memory enhancement
    float getMemoryConsolidationFactor() const { return pImpl->memoryConsolidation; }
    void setMemoryConsolidationFactor(float factor) { pImpl->memoryConsolidation = factor; }
    
    // Learning rate modulation
    float getLearningRateModulation() const { return pImpl->learningRateMod; }
    void setLearningRateModulation(float mod) { pImpl->learningRateMod = mod; }
    
    // Reset
    void reset();
    
private:
    struct Impl;
    Impl* pImpl;
};

// Norepinephrine: Arousal and vigilance
class Norepinephrine : public Neuromodulator {
public:
    Norepinephrine();
    ~Norepinephrine() override;
    
    const char* getName() const override { return "NE"; }
    float getLevel() const override { return pImpl->level; }
    void setLevel(float level) override { pImpl->level = std::clamp(level, 0.0f, 1.0f); }
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Signal detection enhancement
    float getSignalDetectionStrength() const { return pImpl->signalDetection; }
    void setSignalDetectionStrength(float strength) { pImpl->signalDetection = strength; }
    
    // Arousal and vigilance
    float getArousalLevel() const { return pImpl->arousal; }
    void setArousalLevel(float level) { pImpl->arousal = level; }
    
    // Reset
    void reset();
    
private:
    struct Impl;
    Impl* pImpl;
};

// Serotonin: Mood, impulsivity, and behavioral flexibility
class Serotonin : public Neuromodulator {
public:
    Serotonin();
    ~Serotonin() override;
    
    const char* getName() const override { return "5-HT"; }
    float getLevel() const override { return pImpl->level; }
    void setLevel(float level) override { pImpl->level = std::clamp(level, 0.0f, 1.0f); }
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Mood regulation
    float getMoodLevel() const { return pImpl->mood; }
    void setMoodLevel(float level) { pImpl->mood = level; }
    
    // Impulsivity modulation
    float getImpulsivityLevel() const { return pImpl->impulsivity; }
    void setImpulsivityLevel(float level) { pImpl->impulsivity = level; }
    
    // Behavioral flexibility
    float getBehavioralFlexibility() const { return pImpl->flexibility; }
    void setBehavioralFlexibility(float level) { pImpl->flexibility = level; }
    
    // Reset
    void reset();
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
