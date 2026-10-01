#pragma once

#include "Neuromodulator.hpp"
#include "../brain/Brain.hpp"
#include "../environment/Observation.hpp"
#include "../prediction/NeuralPrediction.hpp"
#include "../core/Logger/Logger.hpp"
#include <vector>
#include <algorithm>

namespace nlm {

// Reward signal for reinforcement learning
// Computes reward from environmental states and integrates with neuromodulators

class Reward {
public:
    Reward();
    ~Reward();
    
    // Initialize with brain reference for neuromodulator integration
    void initialize(Brain* brain);
    
    // Get current reward value
    float getValue() const;
    void setValue(float value);
    
    // Accumulate reward
    void add(float delta);
    
    // Reset accumulated reward
    void reset();
    
    // Compute reward from environment state
    // Real reward computation based on prediction error and neuromodulator states
    float computeReward(const Observation& observation);
    
    // Get reward history
    const std::vector<float>& getHistory() const;
    void clearHistory();
    
    // Get reward prediction error
    float getRewardPredictionError() const { return pImpl->predictionError; }
    
    // Integrate with dopamine system
    void integrateWithDopamine(float error, float learningRate = 0.1f);
    
    // Get neuromodulator influence on reward
    float getNeuromodulatorInfluence() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
