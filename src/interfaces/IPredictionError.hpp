// IPredictionError.hpp
// Abstract interface for prediction error neuromodulator
// Implements error signals from prediction mismatch
// Prediction error drives learning and plasticity

#pragma once

#include "INeurosystem.hpp"
#include "../core/Types/Types.hpp"
#include <vector>
#include <memory>
#include <functional>
#include <string>
#include <map>

namespace nlm {

class IPredictionError : public INeurosystem {
public:
    virtual ~IPredictionError() = default;

    // Initialize prediction error system with brain reference
    virtual bool initialize(Brain* brain) override = 0;

    // Get neuromodulator name
    virtual const char* getName() const override { return "PredictionError"; }

    // Get current prediction error level
    virtual float getLevel() const override = 0;
    virtual void setLevel(float level) override = 0;

    // Update prediction error state (called each timestep)
    virtual void update(TimestepDuration dt) override = 0;

    // Apply prediction error effect to plasticity
    virtual float getPlasticityFactor() const override = 0;

    // Compute prediction error from predicted vs actual values
    virtual float computeError(float predicted, float actual) = 0;

    // Update prediction based on new observation
    virtual void updatePrediction(float newPrediction) = 0;

    // Get prediction error magnitude
    virtual float getErrorMagnitude() const = 0;

    // Get error history
    virtual const std::vector<float>& getErrorHistory() const = 0;
    virtual const std::vector<float>& getPredictionHistory() const = 0;
    virtual void clearHistory() = 0;

    // Get error statistics
    virtual float getAverageError() const = 0;
    virtual float getErrorVariance() const = 0;
    virtual float getErrorConfidence() const = 0;

    // Set/Get error threshold for significance
    virtual float getErrorThreshold() const = 0;
    virtual void setErrorThreshold(float threshold) = 0;

    // Get/Get prediction accuracy
    virtual float getPredictionAccuracy() const = 0;
    virtual void setPredictionAccuracy(float accuracy) = 0;

    // Reset prediction error system to initial state
    virtual void reset() override = 0;

    // Interface information override
    virtual NeuromodulatorType getType() const override { return NeuromodulatorType::PredictionError; }

    // Error handling override
    virtual bool hasError() const override = 0;
    virtual std::string getErrorMessage() const override = 0;
    virtual void clearError() override = 0;

    // Callback pattern for prediction error events
    using ErrorComputedCallback = std::function<void(float error, float magnitude)>;
    virtual void setErrorComputedCallback(const ErrorComputedCallback& callback) = 0;

    using PredictionUpdatedCallback = std::function<void(float newPrediction, float error)>;
    virtual void setPredictionUpdatedCallback(const PredictionUpdatedCallback& callback) = 0;

    // Debug information override
    virtual std::string getDebugInfo() const override = 0;

    // Performance metrics
    virtual uint64_t getTotalErrorsComputed() const = 0;
    virtual float getRootMeanSquareError() const = 0;
    virtual float getMeanAbsoluteError() const = 0;

protected:
    IPredictionError() = default;
};

} // namespace nlm
