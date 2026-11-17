#include "PredictionErrorSignal.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

struct PredictionErrorSignal::Impl {
    float errorSmoothing;        // How much to smooth error over time
    float neuromodulationGain;    // How strongly error affects neuromodulation
    float learningRate;           // Learning rate for error signals
    
    Impl() : errorSmoothing(0.9f), neuromodulationGain(1.0f), learningRate(0.01f) {}
};

PredictionErrorSignal::PredictionErrorSignal() : pImpl(new Impl),
    lastError_(0.0f), surpriseThreshold_(0.5f), errorDecayRate_(0.95f) {}

PredictionErrorSignal::~PredictionErrorSignal() = default;

float PredictionErrorSignal::computeError(const std::vector<float>& predicted,
                                          const std::vector<float>& actual) {
    if (predicted.empty() || actual.empty() || predicted.size() != actual.size()) {
        return 0.0f;
    }
    
    float sumSquaredError = 0.0f;
    for (size_t i = 0; i < predicted.size(); ++i) {
        float diff = predicted[i] - actual[i];
        sumSquaredError += diff * diff;
    }
    
    float mse = sumSquaredError / static_cast<float>(predicted.size());
    lastError_ = std::sqrt(mse);  // RMS error
    
    // Apply smoothing
    lastError_ = pImpl->errorSmoothing * lastError_ + (1.0f - pImpl->errorSmoothing) * lastError_;
    
    NLM_LOG_TRACE("Prediction error computed: RMS=" + std::to_string(lastError_));
    
    return lastError_;
}

PredictionErrorSignal::ErrorComponents PredictionErrorSignal::computeErrorComponents(
    const std::vector<float>& predicted,
    const std::vector<float>& actual) const {
    ErrorComponents components = {0.0f, 0.0f, 0.0f, 0.0f};
    
    if (predicted.empty() || actual.empty() || predicted.size() != actual.size()) {
        return components;
    }
    
    // Compute individual components
    for (size_t i = 0; i < predicted.size(); ++i) {
        float diff = predicted[i] - actual[i];
        
        // Intensity error: magnitude of change
        components.intensityError += std::abs(diff);
        
        // Spatial error: larger differences (assuming index represents spatial coordinate)
        if (i < 3) components.spatialError += diff * diff;  // First 3 dims as spatial
        
        // Temporal error: assume later indices represent temporal aspects
        if (i >= 3) components.temporalError += diff * diff;
    }
    
    // Normalize components
    components.intensityError /= static_cast<float>(predicted.size());
    components.spatialError = std::sqrt(components.spatialError);
    components.temporalError = std::sqrt(components.temporalError);
    components.totalError = lastError_;
    
    return components;
}

float PredictionErrorSignal::getModulationSignal() const {
    // Convert error to neuromodulation signal (0-1 scale)
    float signal = std::min(1.0f, lastError_ / surpriseThreshold_);
    
    // Apply decay to prevent signal from persisting too long
    signal *= errorDecayRate_;
    
    NLM_LOG_TRACE("Prediction modulation signal: " + std::to_string(signal));
    
    return signal;
}

void PredictionErrorSignal::recordError(float error, SimulationStep step) {
    errorHistory_.push_back(error);
    
    // Keep only recent history
    if (errorHistory_.size() > 1000) {
        errorHistory_.pop_front();
    }
    
    // Update error components based on recent error history
    errorComponents_.totalError = error;
    errorComponents_.intensityError = error * 0.8f;  // Assume intensity is dominant
    errorComponents_.spatialError = error * 0.1f;
    errorComponents_.temporalError = error * 0.1f;
    
    NLM_LOG_INFO("Recorded prediction error: " + std::to_string(error) +
                 " at step " + std::to_string(step));
}

void PredictionErrorSignal::clear() {
    errorHistory_.clear();
    lastError_ = 0.0f;
    errorComponents_ = ErrorComponents{0.0f, 0.0f, 0.0f, 0.0f};
    NLM_LOG_INFO("PredictionErrorSignal cleared");
}

void PredictionErrorSignal::applyToNeuromodulation(float& dopamineLevel,
                                                    float& acetylcholineLevel,
                                                    float& norepinephrineLevel) const {
    float modulation = getModulationSignal();
    
    if (modulation > 0.1f) {
        // Apply error signals to neuromodulators
        // Use a fixed multiplier since the impl doesn't have neuromodulator levels
        float neuromodulatorGain = 0.5f;
        
        dopamineLevel += modulation * neuromodulatorGain;
        acetylcholineLevel += modulation * neuromodulatorGain;
        norepinephrineLevel += modulation * neuromodulatorGain;
        
        // Cap levels to prevent runaway excitation
        dopamineLevel = std::min(1.0f, dopamineLevel);
        acetylcholineLevel = std::min(1.0f, acetylcholineLevel);
        norepinephrineLevel = std::min(1.0f, norepinephrineLevel);
        
        NLM_LOG_INFO("Applied prediction error to neuromodulation: " +
                     "dopamine+=" + std::to_string(modulation) +
                     " acetylcholine+=" + std::to_string(modulation) +
                     " norepinephrine+=" + std::to_string(modulation));
    }
}

} // namespace nlm
