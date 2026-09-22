#pragma once

#include "../core/Types/Types.hpp"
#include "../brain/Brain.hpp"
#include <memory>
#include <vector>
#include <algorithm>

namespace nlm {

// PredictionSystem: Coordinates prediction mechanisms across the brain
// Integrates working memory, episodic memory, and neural predictions
class PredictionSystem {
public:
    PredictionSystem();
    ~PredictionSystem();

    // Initialize with brain reference
    void initialize(Brain* brain);

    // Make prediction for next state
    std::unique_ptr<SensoryInput> predictNextState(const SensoryInput& currentState);

    // Update predictions based on actual observation
    void updatePredictions(const SensoryInput& predicted, const SensoryInput& actual);

    // Get prediction error
    float getPredictionError() const;

    // Get prediction confidence
    float getConfidence() const;

    // Get prediction history
    const std::vector<float>& getErrorHistory() const;
    void clearHistory();

    // Train prediction model
    void train(const SensoryInput& observation);

    // Update prediction system (called each simulation step)
    void update(TimestepDuration dt);

    // Integrate with working memory
    void integrateWithWorkingMemory(const std::vector<float>& sensoryState);

    // Integrate with episodic memory
    void integrateWithEpisodicMemory(const EpisodicMemoryItem& episode);

    // Get neural predictions for planning
    std::vector<std::pair<ActionType, float>> getActionPredictions(const std::vector<float>& state);

    // Update prediction error based on novelty
    void updatePredictionError(float noveltyLevel);

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;

    Brain* brain_;
};

} // namespace nlm
