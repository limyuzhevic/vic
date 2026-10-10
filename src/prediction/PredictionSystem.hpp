#pragma once

#include "../core/Types/Types.hpp"
#include <vector>
#include <memory>

namespace nlm {

// Prediction system: implements predictive coding and forward models
// Phase 2: Real predictive dynamics using NLM's neural substrate

class PredictionSystem {
public:
    /**
     * @brief Constructor
     */
    PredictionSystem();

    /**
     * @brief Destructor
     */
    ~PredictionSystem();

    /**
     * @brief Make prediction for next timestep based on current state
     * @param currentState Current sensory input
     * @return Predicted sensory input for next timestep
     */
    std::unique_ptr<SensoryInput> predictNextState(const SensoryInput& currentState);

    /**
     * @brief Update predictions based on actual observation
     * @param predicted Predicted sensory input
     * @param actual Actual sensory input received
     */
    void updatePredictions(const SensoryInput& predicted, const SensoryInput& actual);

    /**
     * @brief Get the current prediction error
     * @return Prediction error (lower is better)
     */
    float getPredictionError() const;

    /**
     * @brief Get the prediction confidence
     * @return Confidence level (0.0 to 1.0)
     */
    float getConfidence() const;

    /**
     * @brief Get the history of prediction errors
     * @return Vector of prediction errors over time
     */
    const std::vector<float>& getErrorHistory() const;

    /**
     * @brief Clear prediction error history
     */
    void clearHistory();

    /**
     * @brief Train the prediction model with new observations
     * @param observation Training sensory input
     */
    void train(const SensoryInput& observation);

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
