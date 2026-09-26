#pragma once

#include "../core/Types/Types.hpp"
#include "../brain/Brain.hpp"
#include <vector>
#include <memory>
#include <unordered_map>

namespace nlm {

// Prediction system: predicts future sensory states and consequences
// PHASE 2: Implements real predictive coding using neural substrate

class PredictionSystem {
public:
    PredictionSystem();
    ~PredictionSystem();
    
    // Initialize with brain reference for neural substrate access
    void initialize(Brain* brain);
    
    // Make prediction for next timestep using neural activity patterns
    // Uses sensory neuron activity patterns and working memory targets
    std::unique_ptr<SensoryInput> predictNextState(const SensoryInput& currentState);
    
    // Update predictions based on actual observation using prediction error
    // Integrates with neuromodulation system for learning
    void updatePredictions(const SensoryInput& predicted, const SensoryInput& actual);
    
    // Get prediction error from neuromodulation system
    float getPredictionError() const;
    
    // Get prediction confidence based on neural activity stability
    float getConfidence() const;
    
    // Get prediction history from neuromodulation system
    const std::vector<float>& getErrorHistory() const;
    void clearHistory();
    
    // Train prediction model using episodic memory replay
    // Called during rest periods when replay is active
    void train(const SensoryInput& observation);
    
    // Train from episodic memory replay
    // Used during offline consolidation periods
    void trainFromReplay(const std::vector<const EpisodicMemoryItem*>& episodes);
    
    // Get current prediction targets from working memory
    std::vector<float> getWorkingMemoryTargets() const;
    
    // Get sensory prediction targets from sensory neurons
    std::vector<float> getSensoryPredictionTargets(const SensoryInput& input) const;
    
    // Get concept prototypes as prediction targets
    std::vector<float> getConceptPrototypeTargets(size_t conceptId) const;

    // Get all concept prototypes for prediction
    std::vector<std::pair<size_t, std::vector<float>>> getAllConceptPrototypes() const;
    
    // Get prediction error as concept update signal
    float getConceptUpdateSignal() const;
    
    // Reset prediction system (clear learned associations)
    void reset();
    
    // Update neural dynamics parameters
    void updateParameters(float learningRate, float predictionHorizon,
                         float noveltyThreshold, float conceptInfluenceWeight);

private:
    struct Impl;
    std::uniquePtr<Impl> pImpl;
};

} // namespace nlm
