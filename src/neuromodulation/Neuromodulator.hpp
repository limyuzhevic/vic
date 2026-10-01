#pragma once

#include "../core/Types/Types.hpp"
#include <string>
#include <vector>

namespace nlm {

// Forward declarations for all neuromodulators
class Dopamine;
class Acetylcholine;
class Norepinephrine;
class Serotonin;
class Curiosity;
class Novelty;
class PredictionError;

// Neuromodulator: Abstract base for neuromodulatory signals
// Placehoders are marked - Phase 2 will implement real neuromodulation effects

class Neuromodulator {
public:
    virtual ~Neuromodulator() = default;
    
    // Get modulator name
    virtual const char* getName() const = 0;
    
    // Get current concentration/level
    virtual float getLevel() const = 0;
    virtual void setLevel(float level) = 0;
    
    // Apply neuromodulatory effect to plasticity
    // TODO PHASE 2: Implement real modulation
    virtual float getPlasticityFactor() const = 0;
    
    // Update neuromodulator state
    virtual void update(TimestepDuration dt) = 0;
    
protected:
    Neuromodulator() = default;
};

// Dopamine: Reward and reinforcement learning signal
// PLACEHOLDER - Phase 2
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
    
    // Curiosity integration
    float getCuriosityLevel() const;
    void setCuriosityLevel(float level);
    
    // Novelty integration
    float getNoveltyLevel() const;
    void setNoveltyLevel(float level);
    
    // Prediction error integration
    float getPredictionErrorLevel() const;
    void setPredictionErrorLevel(float level);
    
private:
    struct Impl;
    Impl* pImpl;
};

// Acetylcholine: Attention and memory consolidation
// IMPLEMENTED - Real neuromodulation effects
class Acetylcholine : public Neuromodulator {
public:
    Acetylcholine();
    ~Acetylcholine() override;
    
    const char* getName() const override { return "ACh"; }
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Attention modulation
    void enhanceAttention(bool enable);
    float getAttentionStrength() const;
    
    // Memory consolidation
    void consolidateMemory(const std::vector<float>& memoryPattern);
    float getMemoryConsolidationLevel() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

// Norepinephrine: Arousal and vigilance
// IMPLEMENTED - Real neuromodulation effects
class Norepinephrine : public Neuromodulator {
public:
    Norepinephrine();
    ~Norepinephrine() override;
    
    const char* getName() const override { return "NE"; }
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Arousal control
    void increaseArousal(float intensity);
    void decreaseArousal(float intensity);
    float getArousalLevel() const;
    
    // Vigilance enhancement
    bool isVigilant() const;
    void setVigilanceLevel(float level);
    
private:
    struct Impl;
    Impl* pImpl;
};

// Serotonin: Mood, impulsivity, and social behavior
// IMPLEMENTED - Real neuromodulation effects
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
    void regulateMood(float moodLevel);
    float getMoodLevel() const;
    
    // Impulsivity control
    void reduceImpulsivity(float reduction);
    float getImpulsivityLevel() const;
    
    // Social behavior modulation
    void enhanceSocialBehavior(bool enable);
    float getSocialBehaviorStrength() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

// Curiosity: Exploration motivation based on novelty and prediction error
// INTEGRATED WITH NEUROMODULATOR HIERARCHY
class Curiosity : public Neuromodulator {
public:
    Curiosity();
    ~Curiosity() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Exploration drive
    float getExplorationDrive() const;
    
    // Update with novelty and prediction error
    void updateWithNoveltyAndError(float novelty, float predictionError, TimestepDuration dt);
    
    // Set curiosity parameters
    void setNoveltyWeight(float weight);
    void setPredictionErrorWeight(float weight);
    
    // Reset
    void reset();
    
private:
    struct Impl;
    Impl* pImpl;
};

// Novelty detection signal
// INTEGRATED WITH NEUROMODULATOR HIERARCHY
class Novelty : public Neuromodulator {
public:
    Novelty();
    ~Novelty() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Detect novelty from observation
    void detectNovelty(const std::vector<float>& currentPattern,
                       const std::vector<float>& previousPattern);
    
    // Get novelty history
    const std::vector<float>& getHistory() const;
    void clearHistory();
    
private:
    struct Impl;
    Impl* pImpl;
};

// Prediction error signal for curiosity and learning
// INTEGRATED WITH NEUROMODULATOR HIERARCHY
class PredictionError : public Neuromodulator {
public:
    PredictionError();
    ~PredictionError() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Compute prediction error
    void computeError(float predicted, float actual);
    
    // Get history
    const std::vector<float>& getHistory() const;
    void clearHistory();
    
    // Get error magnitude for neuromodulation
    float getMagnitude() const;
    
    // Is this a surprising event (error > threshold)?
    bool isSurprising() const;
    
    void setSurpriseThreshold(float t);
    float getSurpriseThreshold() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
