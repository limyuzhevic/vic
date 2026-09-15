#pragma once

#include "../core/Types/Types.hpp"
#include "Neuromodulator.hpp"
#include "NeuromodulationConstants.hpp"
#include <cmath>
#include <algorithm>
#include <numeric>
#include <random>

namespace nlm {

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
    
    // Get current pattern
    const std::vector<float>& getCurrentPattern() const;
    const std::vector<float>& getPreviousPattern() const;
    
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
