#pragma once

#include "../core/Types/Types.hpp"
#include "../brain/Brain.hpp"
#include "Neuromodulator.hpp"
#include <vector>
#include <memory>
#include <string>
#include <unordered_map>
#include <algorithm>
#include <cmath>

namespace nlm {

// Forward declarations
class NeuromodulationEngine;

// Feedback loop parameters
struct FeedbackParams {
    float feedbackGain;                   // Feedback gain
    float feedbackDelay;                  // Feedback delay
    float feedbackThreshold;              // Feedback threshold
    float feedbackAdaptation;             // Feedback adaptation rate
    float feedbackNoise;                  // Feedback noise level
    float feedbackSaturation;             // Feedback saturation level
    float feedbackStability;              // Feedback stability margin
    float feedbackPhase;                   // Feedback phase
    float feedbackBandwidth;               // Feedback bandwidth
    
    FeedbackParams()
        : feedbackGain(1.0f), feedbackDelay(0.01f), feedbackThreshold(0.5f),
          feedbackAdaptation(0.1f), feedbackNoise(0.0f), feedbackSaturation(1.0f),
          feedbackStability(1.0f), feedbackPhase(0.0f), feedbackBandwidth(1.0f) {}
};

// Feedforward loop parameters
struct FeedforwardParams {
    float feedforwardGain;                 // Feedforward gain
    float feedforwardDelay;                // Feedforward delay
    float feedforwardThreshold;            // Feedforward threshold
    float feedforwardAdaptation;           // Feedforward adaptation rate
    float feedforwardNoise;                // Feedforward noise level
    float feedforwardSaturation;           // Feedforward saturation level
    float feedforwardStability;            // Feedforward stability margin
    float feedforwardPhase;                 // Feedforward phase
    float feedforwardBandwidth;            // Feedforward bandwidth
    
    FeedforwardParams()
        : feedforwardGain(1.0f), feedforwardDelay(0.0f), feedforwardThreshold(0.5f),
          feedforwardAdaptation(0.1f), feedforwardNoise(0.0f), feedforwardSaturation(1.0f),
          feedforwardStability(1.0f), feedforwardPhase(0.0f), feedforwardBandwidth(1.0f) {}
};

// Feedback signal state
struct FeedbackSignalState {
    float currentValue;                    // Current feedback value
    float targetValue;                     // Target feedback value
    float accumulatedValue;                // Accumulated feedback value
    float errorSignal;                     // Error signal
    float filteredError;                   // Filtered error
    float delayLine;                       // Delay line value
    float feedbackResponse;                // Feedback response
    float feedforwardResponse;             // Feedforward response
    float adaptiveFactor;                  // Adaptive factor
    float stabilityFactor;                  // Stability factor
    
    FeedbackSignalState()
        : currentValue(0.0f), targetValue(0.0f), accumulatedValue(0.0f),
          errorSignal(0.0f), filteredError(0.0f), delayLine(0.0f),
          feedbackResponse(0.0f), feedforwardResponse(0.0f),
          adaptiveFactor(1.0f), stabilityFactor(1.0f) {}
};

// Feedback loop types
enum class FeedbackLoopType {
    NEGATIVE,
    POSITIVE,
    PROPORTIONAL,
    INTEGRAL,
    DERIVATIVE,
    PI,
    PD,
    PID,
    ADAPTIVE,
    PREDICTIVE
};

// Neuromodulator feedback: Feedback and feedforward loops
class NeuromodulatorFeedback {
public:
    NeuromodulatorFeedback();
    ~NeuromodulatorFeedback();
    
    // Disable copying, enable moving
    NeuromodulatorFeedback(const NeuromodulatorFeedback&) = delete;
    NeuromodulatorFeedback& operator=(const NeuromodulatorFeedback&) = delete;
    NeuromodulatorFeedback(NeuromodulatorFeedback&&) noexcept;
    NeuromodulatorFeedback& operator=(NeuromodulatorFeedback&&) noexcept;
    
    // Initialization
    bool initialize(std::shared_ptr<Brain> brain, std::shared_ptr<NeuromodulationEngine> engine);
    
    // Main update step
    void update(TimestepDuration dt, SimulationStep step);
    
    // Feedback computation
    void computeFeedback(const std::string& neuromodulatorName, float currentValue, float targetValue);
    void computeFeedbackLoop(FeedbackLoopType type, const FeedbackSignalState& state);
    void computeNegativeFeedback(const FeedbackSignalState& state);
    void computePositiveFeedback(const FeedbackSignalState& state);
    void computeProportionalFeedback(const FeedbackSignalState& state);
    void computeIntegralFeedback(const FeedbackSignalState& state);
    void computeDerivativeFeedback(const FeedbackSignalState& state);
    void computePIFeedback(const FeedbackSignalState& state);
    void computePDFeedback(const FeedbackSignalState& state);
    void computePIDFeedback(const FeedbackSignalState& state);
    void computeAdaptiveFeedback(const FeedbackSignalState& state);
    void computePredictiveFeedback(const FeedbackSignalState& state);
    
    // Feedforward computation
    void computeFeedforward(const std::string& neuromodulatorName, float desiredOutput, float currentState);
    void computeFeedforwardLoop(FeedforwardParams& params, float input, float state);
    void updateFeedforwardGain(const std::string& neuromodulatorName, float reward, float predictionError);
    
    // Integration with neuromodulators
    void integrateWithNeuromodulators(const std::vector<const Neuromodulator*>& modulators);
    void updateFeedbackSignals(const std::string& neuromodulatorName, const Neuromodulator& modulator);
    void updateFeedforwardSignals(const std::string& neuromodulatorName, const Neuromodulator& modulator);
    
    // State management
    void updateState(const std::string& neuromodulatorName, const FeedbackSignalState& state);
    FeedbackSignalState getState(const std::string& neuromodulatorName) const;
    
    // Configuration
    void setFeedbackParams(const FeedbackParams& params);
    void setFeedforwardParams(const FeedforwardParams& params);
    void setFeedbackLoopType(FeedbackLoopType type) { pImpl->defaultFeedbackType = type; }
    void setFeedforwardLoopType(FeedbackLoopType type) { pImpl->defaultFeedforwardType = type; }
    void enableFeedback(bool enable) { pImpl->enableFeedback_ = enable; }
    void enableFeedforward(bool enable) { pImpl->enableFeedforward_ = enable; }
    
    // Feedback loop management
    void establishFeedbackLoop(const std::string& source, const std::string& target);
    void removeFeedbackLoop(const std::string& source, const std::string& target);
    void updateFeedbackGain(const std::string& source, const std::string& target, float gain);
    void setFeedbackDelay(const std::string& source, const std::string& target, float delay);
    
    // Stability analysis
    void analyzeStability();
    bool checkStability(const std::string& neuromodulatorName) const;
    float computeStabilityMargin(const std::string& neuromodulatorName) const;
    void enforceStabilityConstraints();
    
    // Performance metrics
    float computeFeedbackPerformance() const;
    float computeFeedforwardPerformance() const;
    float computeLoopStability() const;
    float computeConvergenceRate() const;
    
    // Export/import
    void exportFeedback(std::ostream& stream) const;
    void importFeedback(std::istream& stream);
    
    // Testing interface
    float getFeedbackValue(const std::string& neuromodulatorName) const;
    float getFeedforwardValue(const std::string& neuromodulatorName) const;
    float getLoopGain(const std::string& source, const std::string& target) const;
    bool isStable() const { return pImpl->isStable; }
    
private:
    struct Impl;
    Impl* pImpl;
    
    std::shared_ptr<Brain> brain_;
    std::shared_ptr<NeuromodulationEngine> engine_;
    
    FeedbackParams feedbackParams_;
    FeedforwardParams feedforwardParams_;
    FeedbackLoopType defaultFeedbackType;
    FeedbackLoopType defaultFeedforwardType;
    
    bool enableFeedback_;
    bool enableFeedforward_;
    
    // Loop management
    std::unordered_map<std::string, std::unordered_map<std::string, float>> feedbackGains;
    std::unordered_map<std::string, std::unordered_map<std::string, float>> feedbackDelays;
    
    // State
    bool isStable;
    float globalStability;
    float overallFeedbackPerformance;
};

} // namespace nlm