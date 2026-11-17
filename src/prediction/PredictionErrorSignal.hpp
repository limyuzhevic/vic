#pragma once

#include "../core/Types/Types.hpp"
#include <vector>
#include <deque>
#include <memory>
#include <cmath>

namespace nlm {

// PredictionErrorSignal: Computes and broadcasts prediction error
// Acts as neuromodulatory signal to enhance learning
//
// Key principles:
// - Separates error computation from prediction generation
// - Can be used independently of full prediction system
// - Provides error signals for neuromodulation
class PredictionErrorSignal {
public:
    PredictionErrorSignal();
    ~PredictionErrorSignal();

    // Compute error between predicted and actual
    // Returns magnitude of error for neuromodulation
    float computeError(const std::vector<float>& predicted,
                       const std::vector<float>& actual);

    // Get error components (what aspect was unexpected)
    struct ErrorComponents {
        float intensityError;    // How bright/loud things were
        float spatialError;      // Where things were
        float temporalError;     // When things happened
        float totalError;
    };
    ErrorComponents getErrorComponents() const { return errorComponents_; }

    // Get neuromodulation signal strength (0-1)
    float getModulationSignal() const;

    // Update history
    void recordError(float error, SimulationStep step);
    const std::deque<float>& getErrorHistory() const { return errorHistory_; }

    // Is this a surprising event (error > threshold)?
    bool isSurprising() const { return lastError_ > surpriseThreshold_; }

    void setSurpriseThreshold(float t) { surpriseThreshold_ = t; }
    float getSurpriseThreshold() const { return surpriseThreshold_; }

    // Configuration
    void setErrorDecayRate(float rate) { errorDecayRate_ = rate; }
    float getErrorDecayRate() const { return errorDecayRate_; }

    // Reset error signal
    void clear();

    // Apply error to neuromodulation system
    // This would integrate with dopamine, acetylcholine, etc.
    void applyToNeuromodulation(float& dopamineLevel,
                                float& acetylcholineLevel,
                                float& norepinephrineLevel) const;

private:
    // Compute individual error components
    ErrorComponents computeErrorComponents(const std::vector<float>& predicted,
                                           const std::vector<float>& actual) const;

    struct Impl;
    std::unique_ptr<Impl> pImpl;

    float lastError_;
    float surpriseThreshold_;
    float errorDecayRate_;
    ErrorComponents errorComponents_;
    std::deque<float> errorHistory_;
};

} // namespace nlm
