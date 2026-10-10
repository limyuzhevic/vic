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
    
    // Specific neuromodulator methods (implemented by subclasses)
    virtual void signalAttentionBoost() {}  // ACh specific
    virtual void signalArousalIncrease(float intensity) {}  // NE specific
    virtual void signalNoveltyDetection(float novelty) {}  // 5-HT specific
    virtual void signalSocialInteraction() {}  // 5-HT specific
    virtual void triggerBurstMode() {}  // NE specific
    virtual void signalStress(float stressLevel) {}  // NE specific
    
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
    void signalPleasurableExperience();
    
private:
    struct Impl;
    Impl* pImpl;
};

// Acetylcholine: Attention and memory consolidation
class Acetylcholine : public Neuromodulator {
public:
    Acetylcholine();
    ~Acetylcholine() override;
    
    const char* getName() const override { return "Acetylcholine (ACh)"; }
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Specific attention functions
    void signalAttentionBoost();
    void signalNoveltyDetection(float novelty);
    void enhanceMemoryConsolidation(const std::vector<NeuronId>& activeNeurons, float strength);
    
private:
    struct Impl;
    Impl* pImpl;
};

// Norepinephrine: Arousal and vigilance
class Norepinephrine : public Neuromodulator {
public:
    Norepinephrine();
    ~Norepinephrine() override;
    
    const char* getName() const override { return "Norepinephrine (NE)"; }
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Specific arousal functions
    void signalArousalIncrease(float intensity);
    void signalStress(float stressLevel);
    void triggerBurstMode();
    
private:
    struct Impl;
    Impl* pImpl;
};

// Serotonin: Mood, impulsivity, and social behavior
class Serotonin : public Neuromodulator {
public:
    Serotonin();
    ~Serotonin() override;
    
    const char* getName() const override { return "Serotonin (5-HT)"; }
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Specific social functions
    void signalSocialInteraction();
    void signalRewardExperience(float reward);
    void signalSocialLearning();
    
private:
    struct Impl;
    Impl* pImpl;
};

// Curiosity: Exploration motivation
class Curiosity : public Neuromodulator {
public:
    Curiosity();
    ~Curiosity() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Curiosity functions
    void signalNovelty(float novelty);
    void signalExplorationDrive();
    void reduceCuriosity();
    
private:
    struct Impl;
    Impl* pImpl;
};

// Novelty: Novelty detection
class Novelty : public Neuromodulator {
public:
    Novelty();
    ~Novelty() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Novelty functions
    void detectNovelty(const std::vector<float>& input);
    void signalSensorySurprise();
    
private:
    struct Impl;
    Impl* pImpl;
};

// Prediction Error: Learning from prediction mismatches
class PredictionError : public Neuromodulator {
public:
    PredictionError();
    ~PredictionError() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Prediction error functions
    void computePredictionError(float expected, float actual);
    void signalLearningOpportunity();
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
