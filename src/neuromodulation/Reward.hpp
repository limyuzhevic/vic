#pragma once

#include "../core/Types/Types.hpp"
#include "Neuromodulator.hpp"
#include "NeuromodulationConstants.hpp"
#include <cmath>
#include <algorithm>
#include <random>

namespace nlm {

// Reward implementation - computes reward from environment observations
class Reward : public Neuromodulator {
public:
    Reward();
    ~Reward() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    void reset() override;
    bool isActive() const override;
    
    // Compute reward from environment observation
    float computeReward(const Observation& observation) const;
    
    // Compute reward from sensory pattern
    float computeReward(const std::vector<float>& sensoryPattern) const;
    
    // Accumulate reward
    void add(float delta);
    
    // Get reward statistics
    float getAverageReward() const;
    float getTotalReward() const;
    size_t getRewardCount() const;
    
    // Reward history and smoothing
    const std::vector<float>& getHistory() const;
    void clearHistory();
    
    // Reward prediction with temporal differences
    void updatePrediction(float value);
    float getPredictedValue() const;
    float computePredictionError(float actual) const;
    
    // Reward-based plasticity modulation
    float getLearningRateModulation() const;
    
    // Set reward computation mode
    enum class RewardMode {
        Collisions,      // Based on collision detection
        Goals,           // Based on goal achievement
        Proximity,       // Based on proximity to targets
        Custom          // User-defined reward function
    };
    
    void setRewardMode(RewardMode mode);
    
private:
    struct Impl;
    Impl* pImpl;
};

// Curiosity implementation - enhances exploration-exploitation balance
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
    bool isActive() const override;
    
    // Initialize with brain reference
    void initialize(class Brain* brain);
    
    // Update curiosity based on novelty and prediction error
    void update(float novelty, float predictionError, TimestepDuration dt);
    
    // Get exploration drive
    float getExplorationDrive() const;
    
    // Set curiosity parameters
    void setNoveltyWeight(float weight);
    void setPredictionErrorWeight(float weight);
    void setExplorationTemperature(float temp);
    void setEpsilon(float epsilon);
    
    // Action selection with curiosity-driven exploration
    float selectAction(const std::vector<float>& actionValues, 
                      const std::string& policy = "epsilon_greedy");
    
    // Exploration policy selection
    enum class ExplorationPolicy {
        EpsilonGreedy,  // Simple epsilon-greedy
        UCB,            // Upper Confidence Bound
        ThompsonSampling, // Thompson sampling
        Boltzmann,      // Boltzmann (softmax) exploration
        IntrinsicCuriosity  // Pure curiosity-driven
    };
    
    void setExplorationPolicy(ExplorationPolicy policy);
    
    // Get current exploration parameters
    ExplorationPolicy getExplorationPolicy() const;
    float getEpsilon() const;
    float getExplorationTemperature() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

// Novelty implementation - feature-based novelty detection
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
    bool isActive() const override;
    
    // Initialize with brain reference
    void initialize(class Brain* brain);
    
    // Detect novelty from observation
    void detectNovelty(const Observation& observation);
    
    // Detect novelty from sensory input pattern
    void detectNovelty(const std::vector<float>& currentPattern);
    
    // Feature-based novelty detection
    float computeFeatureNovelty(const std::vector<float>& current, 
                               const std::vector<float>& previous) const;
    
    // Statistical surprise detection
    float computeStatisticalNovelty(const std::vector<float>& pattern) const;
    
    // Get novelty history
    const std::vector<float>& getHistory() const;
    void clearHistory();
    
    // Novelty thresholds
    void setNoveltyThreshold(float threshold);
    void setStatisticalThreshold(float threshold);
    
    // Pattern memory management
    void setPatternMemorySize(size_t size);
    void forgetOldPatterns(float rate);
    
private:
    struct Impl;
    Impl* pImpl;
};

// PredictionError implementation - temporal difference learning
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
    bool isActive() const override;
    
    // Initialize with brain reference
    void initialize(class Brain* brain);
    
    // Compute prediction error
    void computeError(float predicted, float actual);
    
    // Update prediction
    void updatePrediction(float newPrediction);
    
    // Temporal difference learning
    float computeTD(float reward, float currentValue, float nextValue);
    
    // Value function learning
    void learnValueFunction(float stateValue, float targetValue);
    
    // Get prediction statistics
    float getPredictedValue() const;
    float getValuePredictionError() const;
    float getMagnitude() const;
    
    // Error propagation
    void propagateError(float factor);
    
    // Error history
    const std::vector<float>& getHistory() const;
    void clearHistory();
    
    // Set prediction parameters
    void setLearningRate(float rate);
    void setDiscountFactor(float gamma);
    void setErrorThreshold(float threshold);
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
