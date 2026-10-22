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
    
    // Get baseline and peak levels
    float getBaseline() const { return pImpl->baseline; }
    float getPeak() const { return pImpl->peak; }
    
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
    
    // Attention modulation
    float getAttentionGain() const;
    void setAttentionGain(float gain);
    
    // Working memory enhancement
    float getMemoryConsolidationFactor() const;
    void setMemoryConsolidationFactor(float factor);
    
    // Learning rate modulation
    float getLearningRateModulation() const;
    void setLearningRateModulation(float mod);
    
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
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Signal detection enhancement
    float getSignalDetectionStrength() const;
    void setSignalDetectionStrength(float strength);
    
    // Arousal and vigilance
    float getArousalLevel() const;
    void setArousalLevel(float level);
    
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
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Mood regulation
    float getMoodLevel() const;
    void setMoodLevel(float level);
    
    // Impulsivity modulation
    float getImpulsivityLevel() const;
    void setImpulsivityLevel(float level);
    
    // Behavioral flexibility
    float getBehavioralFlexibility() const;
    void setBehavioralFlexibility(float level);
    
    // Reset
    void reset();
    
private:
    struct Impl;
    Impl* pImpl;
};

// Novelty: Detection of new stimuli
class Novelty : public Neuromodulator {
public:
    Novelty();
    ~Novelty() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Get novelty level
    float getNoveltyLevel() const { return pImpl->level; }
    
    // Reset
    void reset();
    
private:
    struct Impl;
    Impl* pImpl;
};

// Reward prediction error: Learning from prediction errors
class PredictionError : public Neuromodulator {
public:
    PredictionError();
    ~PredictionError() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Get prediction error level
    float getPredictionErrorLevel() const { return pImpl->level; }
    
    // Reset
    void reset();
    
private:
    struct Impl;
    Impl* pImpl;
};

// Reward: Immediate reward signal
class Reward : public Neuromodulator {
public:
    Reward();
    ~Reward() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Compute reward from observation
    float computeReward(const class Observation& observation) const;
    
    // Reset
    void reset();
    
private:
    struct Impl;
    Impl* pImpl;
};

// Curiosity: Exploration drive based on novelty and prediction error
class Curiosity : public Neuromodulator {
public:
    Curiosity();
    ~Curiosity() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Initialize with brain reference
    void initialize(class Brain* brain);
    
    // Get curiosity level
    float getExplorationDrive() const;
    
    // Set curiosity parameters
    void setNoveltyWeight(float weight);
    void setPredictionErrorWeight(float weight);
    
    // Reset
    void reset();
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm

} // namespace nlm
