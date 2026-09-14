#pragma once

#include "NeuromodulatorBase.hpp"
#include "../core/Types/Types.hpp"

namespace nlm {

// Neuromodulator: Abstract base for neuromodulatory signals
// PLACEHOLDER - Phase 2 will implement real neuromodulation effects

class NeuromodulatorBase; // Forward declaration

// Dopamine: Reward and reinforcement learning signal
// PLACEHOLDER - Phase 2
class Dopamine : public NeuromodulatorBase {
public:
    Dopamine();
    ~Dopamine() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    void initialize(class Brain* brain) override;
    NeuromodulatorType getType() const override;
    void loadConfig(const std::string& key, float value) override;
    
    // Reward signaling
    void signalReward(float reward);
    void signalRewardPredictionError(float error);
    
private:
    struct Impl;
    Impl* pImpl;
};

// Curiosity drive: exploration motivation based on novelty and prediction error
// PLACEHOLDER - Phase 2
class Curiosity : public NeuromodulatorBase {
public:
    Curiosity();
    ~Curiosity() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    void initialize(class Brain* brain) override;
    NeuromodulatorType getType() const override;
    void loadConfig(const std::string& key, float value) override;
    
    // Curiosity-specific methods
    float getExplorationDrive() const;
    void setNoveltyWeight(float weight);
    void setPredictionErrorWeight(float weight);
    void reset();
    
private:
    struct Impl;
    Impl* pImpl;
};

// Novelty detection signal
// PLACEHOLDER - Phase 2
class Novelty : public NeuromodulatorBase {
public:
    Novelty();
    ~Novelty() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    void initialize(class Brain* brain) override;
    NeuromodulatorType getType() const override;
    void loadConfig(const std::string& key, float value) override;
    
    // Novelty-specific methods
    void detectNovelty(const class Observation& observation, 
                       const class Observation& previousObservation);
    void detectNovelty(const std::vector<float>& currentPattern,
                       const std::vector<float>& previousPattern);
    const std::vector<float>& getHistory() const;
    void clearHistory();
    
private:
    struct Impl;
    Impl* pImpl;
};

// Prediction error signal for curiosity and learning
// PLACEHOLDER - Phase 2
class PredictionError : public NeuromodulatorBase {
public:
    PredictionError();
    ~PredictionError() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    void initialize(class Brain* brain) override;
    NeuromodulatorType getType() const override;
    void loadConfig(const std::string& key, float value) override;
    
    // Prediction error-specific methods
    float getError() const;
    void computeError(float predicted, float actual);
    void updatePrediction(float newPrediction);
    float getMagnitude() const;
    const std::vector<float>& getHistory() const;
    void clearHistory();
    
private:
    struct Impl;
    Impl* pImpl;
};

// Factory function for creating neuromodulators
Neuromodulator* createNeuromodulator(NeuromodulatorType type);

} // namespace nlm
