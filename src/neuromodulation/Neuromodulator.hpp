#pragma once

#include "../core/Types/Types.hpp"
#include <vector>
#include <cmath>

namespace nlm {

// Neuromodulator: Abstract base for neuromodulatory signals with real dynamics
class Neuromodulator {
public:
    virtual ~Neuromodulator() = default;
    
    // Get modulator name
    virtual const char* getName() const = 0;
    
    // Get current concentration/level
    virtual float getLevel() const = 0;
    virtual void setLevel(float level) = 0;
    
    // Apply neuromodulatory effect to plasticity
    // Returns plasticity modulation factor (0.0 to 2.0, 1.0 = baseline)
    virtual float getPlasticityFactor() const = 0;
    
    // Update neuromodulator state with temporal dynamics
    virtual void update(TimestepDuration dt) = 0;
    
    // Get neuromodulator state for introspection
    virtual float getNormalizedValue() const { return getLevel() / 1.0f; }
    
    // Check if neuromodulator is in a learning state
    virtual bool isLearning() const { return std::abs(getLevel()) > 0.1f; }
    
protected:
    Neuromodulator() = default;
};

// Dopamine: Real dopamine dynamics with reward prediction error and reinforcement learning
class Dopamine : public Neuromodulator {
public:
    Dopamine();
    ~Dopamine() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Reward prediction error signaling (TD learning)
    void signalReward(float reward, float predictedValue);
    void signalPredictionError(float error);
    
    // Reward accumulation and history
    void addReward(float delta);
    float getAccumulatedReward() const;
    const std::vector<float>& getRewardHistory() const;
    
    // State-dependent modulation
    void setBaseline(float baseline);
    void setLearningRate(float rate);
    void setPhasicModulation(float gain);
    
    // Temporal dynamics parameters
    void setTimeConstant(float tcs, float tcd);  // tonic, phasic decay
    void setReleaseRate(float rate);
    
    // Reset state
    void reset();
    
private:
    struct Impl;
    Impl* pImpl;
};

// Curiosity: Real curiosity-driven exploration dynamics
class Curiosity : public Neuromodulator {
public:
    Curiosity();
    ~Curiosity() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Curiosity computation from novelty and prediction error
    void computeCuriosity(float novelty, float predictionError);
    
    // Integration with neural circuits
    void initialize(class Brain* brain);
    void onNoveltyDetected(const std::vector<float>& pattern);
    void onPredictionErrorComputed(float error);
    
    // Behavioral modulation
    float getExplorationProbability() const;
    void setExplorationBias(float bias);
    
    // Statistical learning for novelty detection
    void updateStatisticalModel(const std::vector<float>& state);
    void resetStatisticalModel();
    
    // Learning parameters
    void setLearningRate(float rate);
    void setExplorationCost(float cost);
    
    // State tracking
    const std::vector<float>& getStateHistory() const;
    float getModelPredictionError() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

// PredictionError: Real prediction error signaling with temporal difference learning
class PredictionError : public Neuromodulator {
public:
    PredictionError();
    ~PredictionError() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Temporal difference learning
    void computeTDError(float currentValue, float targetValue, float discount = 0.95f);
    void updateValuePrediction(float newPrediction);
    
    // Integration with learning systems
    void initialize(class Brain* brain);
    void onStateTransition(const std::vector<float>& state, float value);
    
    // Error statistics for neuromodulation
    float getErrorMagnitude() const;
    float getSignedError() const;
    float getPredictionAccuracy() const;
    const std::vector<float>& getErrorHistory() const;
    
    // Learning parameters
    void setLearningRate(float rate);
    void setDiscountFactor(float discount);
    void setTemporalHorizon(float horizon);
    
    // Reset and state management
    void reset();
    void clearHistory();
    
private:
    struct Impl;
    Impl* pImpl;
};

// Novelty: Real novelty detection with statistical learning and adaptation
class Novelty : public Neuromodulator {
public:
    Novelty();
    ~Novelty() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Novelty detection from observations
    void detectNovelty(const std::vector<float>& observation);
    void detectNovelty(const class Observation& obs);
    
    // Statistical novelty detection
    void updateStatistics(const std::vector<float>& pattern);
    float computeStatisticalNovelty(const std::vector<float>& pattern) const;
    
    // Integration with perceptual systems
    void initialize(class Brain* brain);
    void onSensoryInput(const class SensoryInput& input);
    
    // Adaptive threshold maintenance
    void adaptThreshold(float noveltyRate);
    void setSensitivity(float sensitivity);
    
    // Novelty history and statistics
    const std::vector<float>& getNoveltyHistory() const;
    const std::vector<std::vector<float>>& getPatternHistory() const;
    float getNoveltyRate() const;
    float getRecentNoveltyRate(float timeWindow = 100.0f) const;
    
    // State management
    void reset();
    void clearHistory();
    
    // Feature extraction
    std::vector<float> extractFeatures(const std::vector<float>& pattern) const;
    float computePatternDistance(const std::vector<float>& a, const std::vector<float>& b) const;
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm