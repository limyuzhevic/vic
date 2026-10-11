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
    // TODO PHASE 2: Implement real modulation
    virtual float getPlasticityFactor() const = 0;
    
    // Update neuromodulator state
    virtual void update(TimestepDuration dt) = 0;
    
    // Initialize with brain reference
    virtual void initialize(class Brain* brain) {}
    
    // Reset neuromodulator state
    virtual void reset() {}
    
protected:
    Neuromodulator() = default;
};

// Dopamine: Reward and reinforcement learning signal
class Dopamine : public Neuromodulator {
    struct Impl;
    Impl* pImpl;
public:
    Dopamine();
    ~Dopamine() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    void initialize(class Brain* brain) override;
    void reset() override;
    
    // Reward signaling
    void signalReward(float reward);
    void signalRewardPredictionError(float error);
};

// Acetylcholine: Attention and memory consolidation
class Acetylcholine : public Neuromodulator {
    struct Impl;
    Impl* pImpl;
public:
    Acetylcholine();
    ~Acetylcholine() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    void initialize(class Brain* brain) override;
    void reset() override;
    
    // Enhance attention and memory encoding
    void enhanceAttention(float enhancement);
    void enhanceMemoryEncoding(float strength);
    void setMemoryReverie(bool reverie);
};

// Norepinephrine: Arousal and vigilance
class Norepinephrine : public Neuromodulator {
    struct Impl;
    Impl* pImpl;
public:
    Norepinephrine();
    ~Norepinephrine() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    void initialize(class Brain* brain) override;
    void reset() override;
    
    // Increase arousal and alertness
    void increaseArousal(float level);
    void enhanceSensoryProcessing(float enhancement);
    void setFocusLevel(float level);
};

// Serotonin: Mood, impulsivity, and social behavior
class Serotonin : public Neuromodulator {
    struct Impl;
    Impl* pImpl;
public:
    Serotonin();
    ~Serotonin() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    void initialize(class Brain* brain) override;
    void reset() override;
    
    // Modulate mood and social behavior
    void modulateMood(float level);
    void reduceImpulsivity(float reduction);
    void enhanceSocialBehavior(float strength);
    void setSocialBonding(float bonding);
};

// Novelty: Novelty detection and memory updating
class Novelty : public Neuromodulator {
    struct Impl;
    Impl* pImpl;
public:
    Novelty();
    ~Novelty() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    void initialize(class Brain* brain) override;
    void reset() override;
};

// Curiosity: Exploration motivation
class Curiosity : public Neuromodulator {
    struct Impl;
    Impl* pImpl;
public:
    Curiosity();
    ~Curiosity() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    void initialize(class Brain* brain) override;
    void reset() override;
    
    // Get exploration drive
    float getExplorationDrive() const;
    
    // Configure curiosity weights
    void setNoveltyWeight(float weight);
    void setPredictionErrorWeight(float weight);
};

// Prediction Error: Prediction error signaling
class PredictionError : public Neuromodulator {
    struct Impl;
    Impl* pImpl;
public:
    PredictionError();
    ~PredictionError() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    void initialize(class Brain* brain) override;
    void reset() override;
    
    // Compute prediction error
    void computeError(float predicted, float actual);
    
    // Update prediction
    void updatePrediction(float newPrediction);
    
    // Get error magnitude
    float getMagnitude() const;
    
    // Get history
    const std::vector<float>& getHistory() const;
    void clearHistory();
};

// Reward signal for reinforcement learning
class Reward : public Neuromodulator {
    struct Impl;
    Impl* pImpl;
public:
    Reward();
    ~Reward() override;
    
    // Initialize with brain reference
    void initialize(class Brain* brain) override;
    
    // Get current reward value
    float getValue() const;
    void setValue(float value);
    
    // Accumulate reward
    void add(float delta);
    
    // Reset accumulated reward
    void reset() override;
    
    // Compute reward from environment state
    // TODO PHASE 2: Implement real reward computation
    float computeReward(const class Observation& observation) const;
    
    // Reward history
    const std::vector<float>& getHistory() const;
    void clearHistory();
};

} // namespace nlm