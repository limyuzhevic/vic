#pragma once

#include "../core/Types/Types.hpp"
#include <string>
#include <vector>
#include <memory>

namespace nlm {

// Neuromodulator: Abstract base for neuromodulatory signals
// Provides common functionality and interface for all neuromodulators

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
    
    // Get neuromodulator statistics
    virtual float getMaxLevel() const { return 0.0f; }
    virtual float getMinLevel() const { return 0.0f; }
    virtual float getAverageLevel() const { return 0.0f; }
    
    // Reset to baseline
    virtual void reset() {}
    
    // Set parameters
    virtual void setBaseline(float baseline) {}
    virtual void setDecayRate(float rate) {}
    virtual void setReleaseRate(float rate) {}
    
protected:
    Neuromodulator() = default;
};

// Dopamine: Reward and reinforcement learning signal
// Implements biologically plausible dopamine dynamics for reward learning

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
    
    // Dopamine dynamics control
    void setBaseline(float baseline) override;
    void setDecayRate(float rate) override;
    void setReleaseRate(float rate) override;
    
    // Statistics
    float getMaxLevel() const override;
    float getMinLevel() const override;
    float getAverageLevel() const override;
    
    // History tracking
    const std::vector<float>& getHistory() const;
    void clearHistory();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Acetylcholine: Attention and memory consolidation
// Implements cholinergic signaling for attention and working memory

class Acetylcholine : public Neuromodulator {
public:
    Acetylcholine();
    ~Acetylcholine() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Cholinergic effects
    void signalAttention(float attentionStrength);
    void signalMemoryPriority(float priority);
    
    // Attention dynamics
    void setBaseline(float baseline) override;
    void setDecayRate(float rate) override;
    void setReleaseRate(float rate) override;
    
    // Statistics
    float getMaxLevel() const override;
    float getMinLevel() const override;
    float getAverageLevel() const override;
    
    // History tracking
    const std::vector<float>& getHistory() const;
    void clearHistory();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Norepinephrine: Arousal and vigilance
// Implements noradrenergic signaling for arousal and stress responses

class Norepinephrine : public Neuromodulator {
public:
    Norepinephrine();
    ~Norepinephrine() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Arousal signaling
    void signalArousal(float arousalLevel);
    void signalStress(float stressLevel);
    void signalLearning(float learningRate);
    
    // Arousal dynamics
    void setBaseline(float baseline) override;
    void setDecayRate(float rate) override;
    void setReleaseRate(float rate) override;
    
    // Statistics
    float getMaxLevel() const override;
    float getMinLevel() const override;
    float getAverageLevel() const override;
    
    // History tracking
    const std::vector<float>& getHistory() const;
    void clearHistory();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Serotonin: Mood, impulsivity, and social behavior
// Implements serotonergic signaling for mood regulation and social behavior

class Serotonin : public Neuromodulator {
public:
    Serotonin();
    ~Serotonin() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Serotonergic effects
    void signalMood(float moodLevel);
    void signalSocialBehavior(float socialBehavior);
    void signalImpulsivity(float impulsivityLevel);
    
    // Serotonin dynamics
    void setBaseline(float baseline) override;
    void setDecayRate(float rate) override;
    void setReleaseRate(float rate) override;
    
    // Statistics
    float getMaxLevel() const override;
    float getMinLevel() const override;
    float getAverageLevel() const override;
    
    // History tracking
    const std::vector<float>& getHistory() const;
    void clearHistory();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Curiosity: Exploration and novelty-driven learning
// Placeholder - Phase 2
class Curiosity : public Neuromodulator {
public:
    Curiosity();
    ~Curiosity() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Curiosity dynamics
    void update(float novelty, float predictionError, TimestepDuration dt);
    
    // Exploration drive
    float getExplorationDrive() const;
    void setNoveltyWeight(float weight);
    void setPredictionErrorWeight(float weight);
    
    // Statistics
    float getMaxLevel() const override;
    float getMinLevel() const override;
    float getAverageLevel() const override;
    
    // History tracking
    const std::vector<float>& getHistory() const;
    void clearHistory();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Novelty: Novelty detection and pattern recognition
// Placeholder - Phase 2
class Novelty : public Neuromodulator {
public:
    Novelty();
    ~Novelty() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Novelty detection
    void detectNovelty(const Observation& observation, const Observation& previousObservation);
    void detectNovelty(const std::vector<float>& currentPattern, const std::vector<float>& previousPattern);
    
    // Novelty statistics
    float getNoveltyLevel() const;
    float getNoveltyThreshold() const;
    void setNoveltyThreshold(float threshold);
    
    // Statistics
    float getMaxLevel() const override;
    float getMinLevel() const override;
    float getAverageLevel() const override;
    
    // History tracking
    const std::vector<float>& getHistory() const;
    void clearHistory();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
