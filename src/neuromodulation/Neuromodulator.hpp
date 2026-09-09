#pragma once

#include "../core/Types/Types.hpp"
#include <string>
#include <vector>

namespace nlm {

// Neuromodulator: Abstract base for neuromodulatory signals
// Provides common interface for all neuromodulators (Dopamine, Novelty, PredictionError, Curiosity)
class Neuromodulator {
public:
    Neuromodulator();
    virtual ~Neuromodulator() = default;
    
    // Get modulator name
    virtual const char* getName() const = 0;
    
    // Get current concentration/level
    virtual float getLevel() const = 0;
    virtual void setLevel(float level) = 0;
    
    // Get plasticity factor (modulates synaptic plasticity)
    virtual float getPlasticityFactor() const = 0;
    
    // Update neuromodulator state based on timestep
    virtual void update(TimestepDuration dt) = 0;
    
    // Get neuromodulator type
    virtual NeuromodulatorType getType() const = 0;
    
    // Reset neuromodulator state
    virtual void reset() = 0;
    
    // Check if neuromodulator is active
    virtual bool isActive() const { return level_ > 0.0f; }
    
protected:
    float level_;
    float baseline_;
    float peak_;
    float decayRate_;
    float releaseRate_;
};

// Dopamine: Reward and reinforcement learning signal
// Implements reward prediction error signaling
class Dopamine : public Neuromodulator {
public:
    Dopamine();
    ~Dopamine() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    NeuromodulatorType getType() const override;
    void reset() override;
    
    // Reward signaling
    void signalReward(float reward);
    void signalRewardPredictionError(float error);
    
    // Dopamine effects on learning
    void enhanceRewardLearning(float strength);
    void modulateSelectionStrength(float strength);
    
    // Get additional dopamine-specific properties
    float getBaseline() const { return baseline_; }
    float getPeak() const { return peak_; }
    
private:
    struct Impl;
    Impl* pImpl;
};

// Novelty: Detect unexpected sensory input
// Implements change detection in sensory patterns
class Novelty : public Neuromodulator {
public:
    Novelty();
    ~Novelty() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    NeuromodulatorType getType() const override;
    void reset() override;
    
    // Novelty detection
    void detectNovelty(const std::vector<float>& currentPattern, 
                       const std::vector<float>& previousPattern);
    void updatePattern(const std::vector<float>& pattern);
    void setNoveltyThreshold(float threshold);
    
    // Get novelty-specific properties
    const std::vector<float>& getHistory() const { return history_; }
    float getNoveltyThreshold() const { return noveltyThreshold_; }
    
private:
    struct Impl;
    Impl* pImpl;
    std::vector<float> history_;
    std::vector<float> lastPattern_;
    float noveltyThreshold_;
};

// PredictionError: Computes difference between predicted and actual outcomes
// Implements temporal difference learning signals
class PredictionError : public Neuromodulator {
public:
    PredictionError();
    ~PredictionError() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    NeuromodulatorType getType() const override;
    void reset() override;
    
    // Prediction error computation
    void computeError(float predicted, float actual);
    void updatePrediction(float newPrediction);
    void setLearningRate(float rate);
    
    // Get prediction error-specific properties
    float getError() const { return error_; }
    float getPredictedValue() const { return predictedValue_; }
    float getActualValue() const { return actualValue_; }
    const std::vector<float>& getHistory() const { return history_; }
    
private:
    struct Impl;
    Impl* pImpl;
    float error_;
    float predictedValue_;
    float actualValue_;
    std::vector<float> history_;
};

// Curiosity: Drives exploration based on novelty and prediction error
// Integrates novelty and prediction error signals for exploration motivation
class Curiosity : public Neuromodulator {
public:
    Curiosity();
    ~Curiosity() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    NeuromodulatorType getType() const override;
    void reset() override;
    
    // Curiosity computation
    void update(float novelty, float predictionError, TimestepDuration dt);
    void setNoveltyWeight(float weight);
    void setPredictionErrorWeight(float weight);
    
    // Get curiosity-specific properties
    float getNoveltyWeight() const { return noveltyWeight_; }
    float getPredictionErrorWeight() const { return predictionErrorWeight_; }
    
private:
    struct Impl;
    Impl* pImpl;
    float noveltyWeight_;
    float predictionErrorWeight_;
};

} // namespace nlm
