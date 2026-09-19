// IPredictionSystem.hpp
// Abstract interface for prediction systems
// Predicts future sensory states and consequences using neural dynamics
// Supports multiple prediction mechanisms and error computation

#pragma once

#include "../core/Types/Types.hpp"
#include <memory>
#include <vector>
#include <functional>
#include <string>
#include <map>

namespace nlm {

// Forward declarations
class Brain;
class SensoryInput;
class SensoryOutput;

class IPredictionSystem {
public:
    virtual ~IPredictionSystem() = default;

    // Initialize prediction system with brain reference
    virtual bool initialize(Brain* brain) = 0;

    // Make prediction for next timestep
    // Returns predicted sensory output based on current state
    virtual std::unique_ptr<SensoryOutput> predictNextState(
        const SensoryInput& currentState) = 0;

    // Update predictions based on actual observation
    virtual void updatePredictions(const SensoryInput& predicted,
                                   const SensoryInput& actual) = 0;

    // Get prediction error (difference between predicted and actual)
    virtual float getPredictionError() const = 0;

    // Get prediction confidence (how certain the prediction is)
    virtual float getConfidence() const = 0;

    // Get prediction error history
    virtual const std::vector<float>& getErrorHistory() const = 0;
    virtual void clearHistory() = 0;

    // Train prediction model with new observation
    virtual void train(const SensoryInput& observation) = 0;

    // Get current prediction state
    virtual std::vector<float> getCurrentPrediction() const = 0;

    // Reset prediction system
    virtual void reset() = 0;

    // Update prediction dynamics (time integration)
    virtual void update(TimestepDuration dt) = 0;

    // Get prediction system statistics
    virtual std::string getPredictionStats() const = 0;

    // Interface information
    virtual std::string getInterfaceName() const { return "IPredictionSystem"; }
    virtual std::string getVersion() const { return "1.0.0"; }

    // Error handling
    virtual bool hasError() const = 0;
    virtual std::string getErrorMessage() const = 0;
    virtual void clearError() = 0;

    // Callback pattern for prediction events
    using PredictionMadeCallback = std::function<void(const SensoryOutput& prediction)>;
    virtual void setPredictionMadeCallback(const PredictionMadeCallback& callback) = 0;

    using ErrorComputedCallback = std::function<void(float error, float confidence)>;
    virtual void setErrorComputedCallback(const ErrorComputedCallback& callback) = 0;

    // Debug information
    virtual std::string getDebugInfo() const = 0;

    // Performance metrics
    virtual float getPredictionAccuracy() const = 0;
    virtual float getPredictionSpeed() const = 0;  // Predictions per second
    virtual uint64_t getTotalPredictionsMade() const = 0;

    // Configuration parameters
    virtual float getLearningRate() const = 0;
    virtual void setLearningRate(float rate) = 0;
    virtual float getTemporalHorizon() const = 0;  // How far ahead to predict
    virtual void setTemporalHorizon(float horizon) = 0;

protected:
    IPredictionSystem() = default;
};

} // namespace nlm
