#pragma once

#include "../core/Types/Types.hpp"
#include <string>
#include <vector>

namespace nlm {

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
    
    // Enhance attention and memory consolidation
    void signalAttention(float novelty, float predictionError);
    void enhanceMemoryConsolidation();
    
    // Get modulation effects
    float getAttentionModulator() const;
    float getMemoryConsolidationRate() const;
    
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
    
    // Modulate arousal and exploration
    void signalStimulation(float sensoryInput, float stressLevel);
    void increaseVigilance(float increment);
    
    // Get modulation effects
    float getArousalLevel() const;
    float getVigilanceFactor() const;
    float getExplorationDrive() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

// Serotonin: Mood, social behavior, and regulation
class Serotonin : public Neuromodulator {
public:
    Serotonin();
    ~Serotonin() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Regulate mood and social behavior
    void signalRewardOutcome(float reward, float predictionError);
    void enhanceSocialLearning(float socialCohesion);
    
    // Get modulation effects
    float getMoodFactor() const;
    float getImpulsivityLevel() const;
    float getSocialBehaviorModulator() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm