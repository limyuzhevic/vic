#pragma once

#include "../core/Types/Types.hpp"
#include <string>
#include <vector>

namespace nlm {

// Neuromodulator: Abstract base for neuromodulatory signals
// Provides common interface for all neuromodulatory systems

class Neuromodulator {
public:
    virtual ~Neuromodulator() = default;
    
    // Get modulator name
    virtual const char* getName() const = 0;
    
    // Get current concentration/level (-1.0 to 1.0)
    virtual float getLevel() const = 0;
    virtual void setLevel(float level) = 0;
    
    // Apply neuromodulatory effect to plasticity (1.0 = normal plasticity)
    virtual float getPlasticityFactor() const = 0;
    
    // Update neuromodulator state for next timestep
    virtual void update(TimestepDuration dt) = 0;
    
    // Reset neuromodulator to initial state
    virtual void reset() = 0;
    
    // Check if neuromodulator is enabled
    virtual bool isEnabled() const { return true; }
    
protected:
    Neuromodulator() = default;
};

// Dopamine: Reward and reinforcement learning signal
// Implements reward prediction error signaling for reinforcement learning
class Dopamine : public Neuromodulator {
public:
    Dopamine();
    ~Dopamine() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    void reset() override;
    
    // Reward signaling
    void signalReward(float reward);
    void signalRewardPredictionError(float error);
    
    // Get historical reward data
    const std::vector<float>& getRewardHistory() const { return pImpl->rewardHistory; }
    void clearRewardHistory() { pImpl->rewardHistory.clear(); }
    
private:
    struct Impl;
    Impl* pImpl;
};

// Acetylcholine: Attention and memory consolidation
// Modulates attention, working memory, and memory consolidation
class Acetylcholine : public Neuromodulator {
public:
    Acetylcholine();
    ~Acetylcholine() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    void reset() override;
    
    // Attention modulation
    void enhanceAttention(float strength = 1.0f);
    void reduceAttention(float strength = 1.0f);
    
private:
    struct Impl;
    Impl* pImpl;
};

// Norepinephrine: Arousal and vigilance
// Controls alertness, arousal, and behavioral flexibility
class Norepinephrine : public Neuromodulator {
public:
    Norepinephrine();
    ~Norepinephrine() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    void reset() override;
    
    // Arousal control
    void increaseArousal(float intensity = 1.0f);
    void decreaseArousal(float intensity = 1.0f);
    
private:
    struct Impl;
    Impl* pImpl;
};

// Serotonin: Mood, impulsivity, and social behavior
// Regulates mood, social behavior, and behavioral inhibition
class Serotonin : public Neuromodulator {
public:
    Serotonin();
    ~Serotonin() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    void reset() override;
    
    // Mood regulation
    void improveMood(float amount = 0.1f);
    void worsenMood(float amount = 0.1f);
    
    // Social behavior modulation
    void enhanceSocialBehavior(float strength = 1.0f);
    void reduceSocialBehavior(float strength = 1.0f);
    
private:
    struct Impl;
    Impl* pImpl;
};

// Curiosity system for exploration motivation
// Drives exploration behavior based on novelty and prediction errors
class Curiosity : public Neuromodulator {
public:
    Curiosity();
    ~Curiosity() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    void reset() override;
    
    // Curiosity dynamics
    void updateWithNovelty(float novelty, float predictionError, TimestepDuration dt);
    bool shouldExplore() const;
    float getExplorationDrive() const { return getLevel(); }
    
    // Configuration
    void setNoveltyWeight(float weight);
    void setPredictionErrorWeight(float weight);
    void setExplorationThreshold(float threshold);
    
private:
    struct Impl;
    Impl* pImpl;
};

// Novelty detection for surprise and new information
// Detects and tracks novel sensory inputs and experiences
class Novelty : public Neuromodulator {
public:
    Novelty();
    ~Novelty() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    void reset() override;
    
    // Novelty processing
    void setNovelty(float novelty);
    float getSurprise() const { return getLevel(); }
    
    // Configuration
    void setDetectionThreshold(float threshold);
    void setSensitivity(float sensitivity);
    
private:
    struct Impl;
    Impl* pImpl;
};

// Prediction error signal for learning
// Computes and tracks prediction errors for adaptive learning
class PredictionError : public Neuromodulator {
public:
    PredictionError();
    ~PredictionError() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    void reset() override;
    
    // Prediction error computation
    void computeError(float predicted, float actual);
    void updatePrediction(float newPrediction);
    float getMagnitude() const { return std::abs(pImpl->error); }
    
    // History tracking
    const std::vector<float>& getErrorHistory() const { return pImpl->history; }
    void clearErrorHistory() { pImpl->history.clear(); }
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
