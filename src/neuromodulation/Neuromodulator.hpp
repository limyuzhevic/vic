#pragma once

#include "../core/Types/Types.hpp"
#include <string>
#include <vector>

namespace nlm {

// Neuromodulator: Abstract base for neuromodulatory signals
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
    
    // Memory modulation
    void modulateMemoryConsolidation(bool enhance);
    
    // Attention modulation
    void modulateAttention(bool enhance);
    
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
    
    // Memory modulation
    void enhanceMemoryEncoding(bool enhance);
    void enhanceMemoryReplay(bool enhance);
    
    // Attention modulation
    void enhanceSelectiveAttention(bool enhance);
    void enhanceGating(bool enhance);
    
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
    
    // Arousal effects
    void increaseAlertness(bool increase);
    void enhanceSignalToNoise(bool enhance);
    
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
    
    // Behavioral modulation
    void modulateImpulsivity(bool reduce);
    void enhanceSocialLearning(bool enhance);
    
    // Mood effects
    void enhanceMemoryRetrieval(bool enhance);
    void reduceNegativeArousal(bool reduce);
    
private:
    struct Impl;
    Impl* pImpl;
};

// Curiosity: Novelty-driven exploration
class Curiosity : public Neuromodulator {
public:
    Curiosity();
    ~Curiosity() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Exploration effects
    void enhanceExploration(bool enhance);
    void strengthenActionSelection(bool enhance);
    
    // Learning effects
    void enhanceIntrinsicallyMotivatedLearning(bool enhance);
    void modulateApproachAvoidance(bool modulate);
    
private:
    struct Impl;
    Impl* pImpl;
};

// Novelty: Novelty detection and adaptation
class Novelty : public Neuromodulator {
public:
    Novelty();
    ~Novelty() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Adaptation effects
    void triggerRapidAdaptation(bool trigger);
    void enhancePatternSeparation(bool enhance);
    
    // Learning effects
    void enhanceReceptiveFieldExpansion(bool enhance);
    void promoteExploreExploitBalance(bool balance);
    
private:
    struct Impl;
    Impl* pImpl;
};

// Prediction Error: Prediction accuracy monitoring
class PredictionError : public Neuromodulator {
public:
    PredictionError();
    ~PredictionError() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Learning effects
    void adjustLearningRate(bool increase);
    void updatePredictiveModels(bool update);
    
    // Memory effects
    void modifyMemoryPrioritization(bool enhance);
    void updatePredictionErrorSignals(bool update);
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
