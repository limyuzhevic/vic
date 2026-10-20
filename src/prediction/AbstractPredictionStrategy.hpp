#pragma once

#include "../core/Types/Types.hpp"
#include "../brain/Brain.hpp"
#include "../sensory/SensoryInput.hpp"
#include <memory>
#include <string>

namespace nlm {

// AbstractPredictionStrategy: Base interface for prediction systems
// Provides common functionality for prediction architectures
// Allows different implementations (PredictionSystem, NeuralPrediction)
// to be used interchangeably through strategy pattern

class AbstractPredictionStrategy {
public:
    virtual ~AbstractPredictionStrategy() = default;

    // Initialize with brain reference for system coordination
    virtual void initialize(Brain* brain) = 0;

    // Predict next state based on current sensory input
    virtual std::unique_ptr<SensoryInput> predictNextState(const SensoryInput& currentState) = 0;

    // Get prediction error (0.0 = perfect prediction, 1.0 = maximum error)
    virtual float getPredictionError() const = 0;

    // Get prediction confidence (0.0 = no confidence, 1.0 = maximum confidence)
    virtual float getPredictionConfidence() const = 0;

    // Update prediction model based on actual observation
    virtual void update(const SensoryInput& predicted, const SensoryInput& actual) = 0;

    // Get prediction system name/type for debugging and logging
    virtual const char* getName() const = 0;

    // Get prediction status information
    virtual std::string getStatus() const = 0;

    // Enable/disable prediction system functionality
    virtual void enable(bool enable) = 0;
    virtual bool isEnabled() const = 0;

    // Reset prediction system state
    virtual void reset() = 0;

    // Get number of training examples/predictions used
    virtual size_t getTrainingCount() const = 0;

protected:
    AbstractPredictionStrategy() = default;
};

} // namespace nlm
