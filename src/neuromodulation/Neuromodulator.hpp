#pragma once

#include "../core/Types/Types.hpp"
#include <string>
#include <vector>

namespace nlm {

// Neuromodulator: Abstract base for neuromodulatory signals
// PLACEHOLDER - Phase 2 will implement real neuromodulation effects

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
    
private:
    struct Impl;
    Impl* pImpl;
};

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
    
private:
    struct Impl;
    Impl* pImpl;
};

// Curiosity: Exploration motivation based on novelty and prediction error
class Curiosity {
public:
    Curiosity();
    ~Curiosity();
    
    // Initialize with brain reference
    void initialize(class Brain* brain);
    
    // Get curiosity level
    float getLevel() const;
    
    // Update curiosity based on novelty and prediction error
    void update(float novelty, float predictionError, TimestepDuration dt);
    
    // Get exploration drive (same as level)
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

// Novelty: Novelty detection from observations
class Novelty {
public:
    Novelty();
    ~Novelty();
    
    // Initialize with brain reference
    void initialize(class Brain* brain);
    
    // Get novelty level
    float getLevel() const;
    void setLevel(float level);
    
    // Detect novelty from observation
    void detectNovelty(const class Observation& observation, 
                       const class Observation& previousObservation);
    
    // Detect novelty from sensory input pattern
    void detectNovelty(const std::vector<float>& currentPattern,
                       const std::vector<float>& previousPattern);
    
    // Decay novelty over time
    void update(TimestepDuration dt);
    
    // Get novelty history
    const std::vector<float>& getHistory() const;
    void clearHistory();
    
private:
    struct Impl;
    Impl* pImpl;
};

// PredictionError: Prediction error signal for learning
class PredictionError {
public:
    PredictionError();
    ~PredictionError();
    
    // Initialize with brain reference
    void initialize(class Brain* brain);
    
    // Get error value
    float getError() const;
    
    // Compute prediction error
    void computeError(float predicted, float actual);
    
    // Update prediction
    void updatePrediction(float newPrediction);
    
    // Get history
    const std::vector<float>& getHistory() const;
    void clearHistory();
    
    // Get error magnitude for neuromodulation
    float getMagnitude() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

// Reward: Reward signaling system
class Reward {
public:
    Reward();
    ~Reward();
    
    // Initialize with brain reference
    void initialize(class Brain* brain);
    
    // Get reward level
    float getLevel() const;
    
    // Signal reward
    void signalReward(float reward);
    
    // Signal prediction error on reward
    void signalPredictionError(float error);
    
    // Update reward
    void update(TimestepDuration dt);
    
private:
    struct Impl;
    Impl* pImpl;
};

// Serotonin: Mood, learning, sleep, attention, avoidance learning
class Serotonin : public Neuromodulator {
public:
    Serotonin();
    ~Serotonin() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Initialize with brain reference
    void initialize(class Brain* brain);
    
    // Signal events
    void signalMood(float mood);
    void signalLearning(float learningSignal);
    void signalAvoidance(float avoidanceStrength);
    
    // Get effects
    float getAttentionEffect() const;
    float getAvoidanceLearningEffect() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

// Norepinephrine: Arousal, attention, working memory, cognitive control
class Norepinephrine : public Neuromodulator {
public:
    Norepinephrine();
    ~Norepinephrine() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Initialize with brain reference
    void initialize(class Brain* brain);
    
    // Signal events
    void signalArousal(float arousal);
    void signalAttention(float attention);
    void signalWorkingMemory(float memoryUpdate);
    
    // Get effects
    float getAttentionEffect() const;
    float getWorkingMemoryEffect() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

// Acetylcholine: Attention, memory consolidation, sleep, cortical activation
class Acetylcholine : public Neuromodulator {
public:
    Acetylcholine();
    ~Acetylcholine() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Initialize with brain reference
    void initialize(class Brain* brain);
    
    // Signal events
    void signalAttention(float attention);
    void signalMemory(float memorySignal);
    void signalSleep(float sleepState);
    
    // Get effects
    float getAttentionEffect() const;
    float getMemoryConsolidationEffect() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
