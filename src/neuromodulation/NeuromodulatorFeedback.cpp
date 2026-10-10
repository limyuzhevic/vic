#include "NeuromodulatorFeedback.hpp"
#include "../core/Logger/Logger.hpp"
#include "../core/Config/Config.hpp"
#include <algorithm>
#include <cmath>
#include <numeric>
#include <sstream>

namespace nlm {

struct NeuromodulatorFeedback::Impl {
    std::shared_ptr<Brain> brain;
    std::shared_ptr<NeuromodulationEngine> engine;
    FeedbackParams feedbackParams;
    FeedforwardParams feedforwardParams;
    FeedbackLoopType defaultFeedbackType;
    FeedbackLoopType defaultFeedforwardType;
    
    // Feedback loop state
    std::unordered_map<std::string, FeedbackSignalState> feedbackStates;
    
    // Feedforward loop state
    std::unordered_map<std::string, float> feedforwardStates;
    
    // Loop management
    std::unordered_map<std::string, std::unordered_map<std::string, float>> feedbackGains;
    std::unordered_map<std::string, std::unordered_map<std::string, float>> feedbackDelays;
    
    // Integration
    std::vector<const Neuromodulator*> integratedNeuromodulators;
    
    // Performance metrics
    bool isStable;
    float globalStability;
    float overallFeedbackPerformance;
    
    // History
    std::vector<float> stabilityHistory;
    std::vector<float> feedbackPerformanceHistory;
    std::vector<float> feedforwardPerformanceHistory;
    
    // Configuration
    float minGain;
    float maxGain;
    float minDelay;
    float maxDelay;
    float minStability;
    float maxStability;
    
    Impl() : defaultFeedbackType(FeedbackLoopType::ADAPTIVE),
             defaultFeedforwardType(FeedbackLoopType::PREDICTIVE),
             isStable(true), globalStability(1.0f), overallFeedbackPerformance(1.0f),
             minGain(0.1f), maxGain(10.0f), minDelay(0.0f), maxDelay(1.0f),
             minStability(0.0f), maxStability(1.0f) {
        
        // Initialize default feedback states
        feedbackStates["Dopamine"] = FeedbackSignalState();
        feedbackStates["Acetylcholine"] = FeedbackSignalState();
        feedbackStates["Norepinephrine"] = FeedbackSignalState();
        feedbackStates["Serotonin"] = FeedbackSignalState();
        
        // Initialize feedforward states
        feedforwardStates["Dopamine"] = 0.0f;
        feedforwardStates["Acetylcholine"] = 0.0f;
        feedforwardStates["Norepinephrine"] = 0.0f;
        feedforwardStates["Serotonin"] = 0.0f;
        
        // Initialize default gains
        for (const auto& neuromodulator : {"Dopamine", "Acetylcholine", "Norepinephrine", "Serotonin"}) {
            for (const auto& target : {"Dopamine", "Acetylcholine", "Norepinephrine", "Serotonin"}) {
                if (neuromodulator != target) {
                    feedbackGains[neuromodulator][target] = 1.0f;
                    feedbackDelays[neuromodulator][target] = 0.01f;
                }
            }
        }
    }
};

NeuromodulatorFeedback::NeuromodulatorFeedback() : pImpl(new Impl) {
    NLM_LOG_INFO("NeuromodulatorFeedback created");
}

NeuromodulatorFeedback::~NeuromodulatorFeedback() {
    NLM_LOG_INFO("NeuromodulatorFeedback destroyed");
}

NeuromodulatorFeedback::NeuromodulatorFeedback(NeuromodulatorFeedback&& other) noexcept
    : pImpl(std::exchange(other.pImpl, nullptr)) {
    if (pImpl) {
        pImpl->brain = std::exchange(other.pImpl->brain, nullptr);
        pImpl->engine = std::exchange(other.pImpl->engine, nullptr);
    }
}

NeuromodulatorFeedback& NeuromodulatorFeedback::operator=(NeuromodulatorFeedback&& other) noexcept {
    if (this != &other) {
        shutdown();
        pImpl = std::exchange(other.pImpl, nullptr);
        if (pImpl) {
            pImpl->brain = std::exchange(other.pImpl->brain, nullptr);
            pImpl->engine = std::exchange(other.pImpl->engine, nullptr);
        }
    }
    return *this;
}

bool NeuromodulatorFeedback::initialize(std::shared_ptr<Brain> brain, std::shared_ptr<NeuromodulationEngine> engine) {
    try {
        pImpl->brain = brain;
        pImpl->engine = engine;
        
        if (!pImpl->brain || !pImpl->engine) {
            NLM_LOG_ERROR("NeuromodulatorFeedback initialization failed: invalid dependencies");
            return false;
        }
        
        NLM_LOG_INFO("NeuromodulatorFeedback initialization completed successfully");
        return true;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("NeuromodulatorFeedback initialization failed: " + std::string(e.what()));
        return false;
    }
}

void NeuromodulatorFeedback::update(TimestepDuration dt, SimulationStep step) {
    // Update feedback loops
    if (pImpl->enableFeedback_) {
        updateFeedbackLoops(dt, step);
    }
    
    // Update feedforward loops
    if (pImpl->enableFeedforward_) {
        updateFeedforwardLoops(dt, step);
    }
    
    // Analyze stability
    analyzeStability();
    
    // Update performance metrics
    updatePerformanceMetrics();
}

void NeuromodulatorFeedback::computeFeedback(const std::string& neuromodulatorName, float currentValue, float targetValue) {
    auto it = pImpl->feedbackStates.find(neuromodulatorName);
    if (it == pImpl->feedbackStates.end()) {
        return;
    }
    
    FeedbackSignalState& state = it->second;
    
    // Compute error signal
    state.errorSignal = targetValue - currentValue;
    
    // Apply feedback gain
    state.feedbackResponse = state.errorSignal * pImpl->feedbackParams.feedbackGain;
    
    // Update current value
    state.currentValue = currentValue + state.feedbackResponse * dt;
    
    // Apply bounds
    state.currentValue = std::clamp(state.currentValue, 0.0f, 1.0f);
    
    // Update filtered error
    state.filteredError = state.errorSignal * pImpl->feedbackParams.feedbackAdaptation;
    
    // Update delay line
    state.delayLine = state.currentValue * std::exp(-pImpl->feedbackParams.feedbackDelay * dt);
}

void NeuromodulatorFeedback::computeFeedbackLoop(FeedbackLoopType type, const FeedbackSignalState& state) {
    switch (type) {
        case FeedbackLoopType::NEGATIVE:
            computeNegativeFeedback(state);
            break;
        case FeedbackLoopType::POSITIVE:
            computePositiveFeedback(state);
            break;
        case FeedbackLoopType::PROPORTIONAL:
            computeProportionalFeedback(state);
            break;
        case FeedbackLoopType::INTEGRAL:
            computeIntegralFeedback(state);
            break;
        case FeedbackLoopType::DERIVATIVE:
            computeDerivativeFeedback(state);
            break;
        case FeedbackLoopType::PI:
            computePIFeedback(state);
            break;
        case FeedbackLoopType::PD:
            computePDFeedback(state);
            break;
        case FeedbackLoopType::PID:
            computePIDFeedback(state);
            break;
        case FeedbackLoopType::ADAPTIVE:
            computeAdaptiveFeedback(state);
            break;
        case FeedbackLoopType::PREDICTIVE:
            computePredictiveFeedback(state);
            break;
    }
}

void NeuromodulatorFeedback::computeNegativeFeedback(const FeedbackSignalState& state) {
    // Negative feedback reduces error
    float negativeFeedback = -state.errorSignal * pImpl->feedbackParams.feedbackGain;
    // Applied externally in computeFeedback
}

void NeuromodulatorFeedback::computePositiveFeedback(const FeedbackSignalState& state) {
    // Positive feedback amplifies error
    float positiveFeedback = state.errorSignal * pImpl->feedbackParams.feedbackGain * 0.5f;
    // Applied externally in computeFeedback
}

void NeuromodulatorFeedback::computeProportionalFeedback(const FeedbackSignalState& state) {
    // Proportional feedback: K_p * error
    float proportional = pImpl->feedbackParams.feedbackGain * state.errorSignal;
    // Applied externally in computeFeedback
}

void NeuromodulatorFeedback::computeIntegralFeedback(const FeedbackSignalState& state) {
    // Integral feedback: K_i * ∫error
    float integral = state.accumulatedValue * pImpl->feedbackParams.feedbackGain;
    state.accumulatedValue += state.errorSignal * dt;
    // Applied externally in computeFeedback
}

void NeuromodulatorFeedback::computeDerivativeFeedback(const FeedbackSignalState& state) {
    // Derivative feedback: K_d * d(error)/dt
    float derivative = 0.0f;  // Simplified derivative calculation
    // Applied externally in computeFeedback
}

void NeuromodulatorFeedback::computePIFeedback(const FeedbackSignalState& state) {
    // PI feedback: K_p * error + K_i * ∫error
    float proportional = pImpl->feedbackParams.feedbackGain * state.errorSignal;
    float integral = state.accumulatedValue * pImpl->feedbackParams.feedbackGain * 0.1f;
    // Applied externally in computeFeedback
}

void NeuromodulatorFeedback::computePDFeedback(const FeedbackSignalState& state) {
    // PD feedback: K_p * error + K_d * d(error)/dt
    float proportional = pImpl->feedbackParams.feedbackGain * state.errorSignal;
    float derivative = 0.0f;  // Simplified
    // Applied externally in computeFeedback
}

void NeuromodulatorFeedback::computePIDFeedback(const FeedbackSignalState& state) {
    // PID feedback: K_p * error + K_i * ∫error + K_d * d(error)/dt
    float proportional = pImpl->feedbackParams.feedbackGain * state.errorSignal;
    float integral = state.accumulatedValue * pImpl->feedbackParams.feedbackGain * 0.1f;
    float derivative = 0.0f;  // Simplified
    // Applied externally in computeFeedback
}

void NeuromodulatorFeedback::computeAdaptiveFeedback(const FeedbackSignalState& state) {
    // Adaptive feedback with learning
    float adaptiveGain = pImpl->feedbackParams.feedbackGain * (1.0f + state.adaptiveFactor);
    float adaptiveFeedback = state.errorSignal * adaptiveGain;
    // Applied externally in computeFeedback
}

void NeuromodulatorFeedback::computePredictiveFeedback(const FeedbackSignalState& state) {
    // Predictive feedback based on prediction
    float predictionError = state.errorSignal * 0.5f;  // Simplified prediction
    float predictiveFeedback = predictionError * pImpl->feedbackParams.feedbackGain * 1.5f;
    // Applied externally in computeFeedback
}

void NeuromodulatorFeedback::computeFeedforward(const std::string& neuromodulatorName, float desiredOutput, float currentState) {
    auto it = pImpl->feedbackStates.find(neuromodulatorName);
    if (it == pImpl->feedbackStates.end()) {
        return;
    }
    
    // Compute feedforward signal
    float feedforwardError = desiredOutput - currentState;
    float feedforwardSignal = feedforwardError * pImpl->feedforwardParams.feedforwardGain;
    
    // Apply feedforward response
    pImpl->feedforwardStates[neuromodulatorName] += feedforwardSignal * dt;
    
    // Apply bounds
    pImpl->feedforwardStates[neuromodulatorName] = 
        std::clamp(pImpl->feedforwardStates[neuromodulatorName], 0.0f, 1.0f);
}

void NeuromodulatorFeedback::computeFeedforwardLoop(FeedforwardParams& params, float input, float state) {
    // Compute feedforward loop response
    float feedforwardOutput = input * params.feedforwardGain;
    // Applied externally in computeFeedforward
}

void NeuromodulatorFeedback::updateFeedbackGain(const std::string& source, const std::string& target, float gain) {
    if (pImpl->feedbackGains.find(source) != pImpl->feedbackGains.end() &&
        pImpl->feedbackGains[source].find(target) != pImpl->feedbackGains[source].end()) {
        
        pImpl->feedbackGains[source][target] = std::clamp(gain, pImpl->minGain, pImpl->maxGain);
    }
}

void NeuromodulatorFeedback::setFeedbackDelay(const std::string& source, const std::string& target, float delay) {
    if (pImpl->feedbackDelays.find(source) != pImpl->feedbackDelays.end() &&
        pImpl->feedbackDelays[source].find(target) != pImpl->feedbackDelays[source].end()) {
        
        pImpl->feedbackDelays[source][target] = std::clamp(delay, pImpl->minDelay, pImpl->maxDelay);
    }
}

void NeuromodulatorFeedback::integrateWithNeuromodulators(const std::vector<const Neuromodulator*>& modulators) {
    pImpl->integratedNeuromodulators = modulators;
    
    // Update feedback and feedforward states based on neuromodulator levels
    for (const auto& modulator : modulators) {
        std::string name = modulator->getName();
        float level = modulator->getLevel();
        
        // Update feedback state
        if (pImpl->feedbackStates.find(name) != pImpl->feedbackStates.end()) {
            pImpl->feedbackStates[name].currentValue = level;
        }
        
        // Update feedforward state
        if (pImpl->feedforwardStates.find(name) != pImpl->feedforwardStates.end()) {
            pImpl->feedforwardStates[name] = level * 0.5f;
        }
    }
}

void NeuromodulatorFeedback::updateFeedbackSignals(const std::string& neuromodulatorName, const Neuromodulator& modulator) {
    auto it = pImpl->feedbackStates.find(neuromodulatorName);
    if (it == pImpl->feedbackStates.end()) {
        return;
    }
    
    FeedbackSignalState& state = it->second;
    
    // Get modulator level
    float modulatorLevel = modulator.getLevel();
    
    // Update feedback state based on modulator
    state.currentValue = modulatorLevel;
    state.feedbackResponse = modulatorLevel * pImpl->feedbackParams.feedbackGain;
}

void NeuromodulatorFeedback::updateFeedforwardSignals(const std::string& neuromodulatorName, const Neuromodulator& modulator) {
    float modulatorLevel = modulator.getLevel();
    pImpl->feedforwardStates[neuromodulatorName] = modulatorLevel * 0.5f;
}

void NeuromodulatorFeedback::updateFeedbackLoops(TimestepDuration dt, SimulationStep step) {
    // Update all feedback loops
    for (auto& [name, state] : pImpl->feedbackStates) {
        // Update accumulated error
        state.accumulatedValue += state.errorSignal * dt;
        
        // Apply feedback gain
        state.currentValue += state.feedbackResponse * dt;
        
        // Apply bounds
        state.currentValue = std::clamp(state.currentValue, 0.0f, 1.0f);
        
        // Update adaptive factor
        state.adaptiveFactor = std::min(1.0f, state.adaptiveFactor + 0.01f);
        
        // Update stability factor
        state.stabilityFactor = std::max(0.1f, state.stabilityFactor - 0.01f);
    }
}

void NeuromodulatorFeedback::updateFeedforwardLoops(TimestepDuration dt, SimulationStep step) {
    // Update all feedforward loops
    for (auto& [name, feedforwardValue] : pImpl->feedforwardStates) {
        // Apply feedforward adaptation
        feedforwardValue *= (1.0f + pImpl->feedforwardParams.feedforwardAdaptation * dt);
        
        // Apply bounds
        feedforwardValue = std::clamp(feedforwardValue, 0.0f, 1.0f);
    }
}

void NeuromodulatorFeedback::establishFeedbackLoop(const std::string& source, const std::string& target) {
    if (pImpl->feedbackStates.find(source) != pImpl->feedbackStates.end() &&
        pImpl->feedbackStates.find(target) != pImpl->feedbackStates.end()) {
        
        if (pImpl->feedbackGains.find(source) == pImpl->feedbackGains.end()) {
            pImpl->feedbackGains[source] = std::unordered_map<std::string, float>();
        }
        
        if (pImpl->feedbackDelays.find(source) == pImpl->feedbackDelays.end()) {
            pImpl->feedbackDelays[source] = std::unordered_map<std::string, float>();
        }
        
        pImpl->feedbackGains[source][target] = 1.0f;
        pImpl->feedbackDelays[source][target] = 0.01f;
    }
}

void NeuromodulatorFeedback::removeFeedbackLoop(const std::string& source, const std::string& target) {
    if (pImpl->feedbackGains.find(source) != pImpl->feedbackGains.end() &&
        pImpl->feedbackGains[source].find(target) != pImpl->feedbackGains[source].end()) {
        
        pImpl->feedbackGains[source].erase(target);
    }
    
    if (pImpl->feedbackDelays.find(source) != pImpl->feedbackDelays.end() &&
        pImpl->feedbackDelays[source].find(target) != pImpl->feedbackDelays[source].end()) {
        
        pImpl->feedbackDelays[source].erase(target);
    }
}

void NeuromodulatorFeedback::analyzeStability() {
    // Analyze stability of all feedback loops
    bool stable = true;
    float totalStability = 0.0f;
    
    for (const auto& [name, state] : pImpl->feedbackStates) {
        // Check if feedback response is reasonable
        if (std::abs(state.feedbackResponse) > 2.0f) {
            stable = false;
        }
        
        // Compute stability contribution
        float stability = 1.0f / (1.0f + std::abs(state.feedbackResponse) * 0.5f);
        totalStability += stability;
    }
    
    pImpl->isStable = stable;
    pImpl->globalStability = totalStability / pImpl->feedbackStates.size();
}

bool NeuromodulatorFeedback::checkStability(const std::string& neuromodulatorName) const {
    auto it = pImpl->feedbackStates.find(neuromodulatorName);
    if (it == pImpl->feedbackStates.end()) {
        return false;
    }
    
    const FeedbackSignalState& state = it->second;
    
    // Check if feedback response is within bounds
    return std::abs(state.feedbackResponse) < 2.0f &&
           state.currentValue >= 0.0f && state.currentValue <= 1.0f;
}

float NeuromodulatorFeedback::computeStabilityMargin(const std::string& neuromodulatorName) const {
    auto it = pImpl->feedbackStates.find(neuromodulatorName);
    if (it == pImpl->feedbackStates.end()) {
        return 0.0f;
    }
    
    const FeedbackSignalState& state = it->second;
    
    // Compute stability margin based on feedback response
    float margin = 1.0f / (1.0f + std::abs(state.feedbackResponse) * 0.5f);
    
    return std::clamp(margin, pImpl->minStability, pImpl->maxStability);
}

void NeuromodulatorFeedback::enforceStabilityConstraints() {
    // Enforce stability constraints on all feedback loops
    for (auto& [name, state] : pImpl->feedbackStates) {
        // Apply stability bounds
        state.feedbackResponse = std::clamp(state.feedbackResponse, -2.0f, 2.0f);
        state.currentValue = std::clamp(state.currentValue, 0.0f, 1.0f);
        
        // Update accumulated value
        state.accumulatedValue = std::clamp(state.accumulatedValue, -1.0f, 1.0f);
    }
}

float NeuromodulatorFeedback::computeFeedbackPerformance() const {
    float totalPerformance = 0.0f;
    
    for (const auto& [name, state] : pImpl->feedbackStates) {
        // Compute performance based on feedback quality
        float error = std::abs(state.errorSignal);
        float performance = 1.0f / (1.0f + error);
        totalPerformance += performance;
    }
    
    return totalPerformance / pImpl->feedbackStates.size();
}

float NeuromodulatorFeedback::computeFeedforwardPerformance() const {
    float totalPerformance = 0.0f;
    
    for (const auto& [name, feedforwardValue] : pImpl->feedforwardStates) {
        // Compute feedforward performance (simplified)
        float performance = feedforwardValue * (1.0f - feedforwardValue);
        totalPerformance += performance;
    }
    
    return totalPerformance / pImpl->feedforwardStates.size();
}

float NeuromodulatorFeedback::computeLoopStability() const {
    return pImpl->globalStability;
}

float NeuromodulatorFeedback::computeConvergenceRate() const {
    return 1.0f / (1.0f + pImpl->globalStability);
}

void NeuromodulatorFeedback::updatePerformanceMetrics() {
    // Update performance metrics history
    pImpl->stabilityHistory.push_back(pImpl->globalStability);
    pImpl->feedbackPerformanceHistory.push_back(computeFeedbackPerformance());
    pImpl->feedforwardPerformanceHistory.push_back(computeFeedforwardPerformance());
    
    // Keep history size limited
    if (pImpl->stabilityHistory.size() > 100) {
        pImpl->stabilityHistory.erase(pImpl->stabilityHistory.begin());
        pImpl->feedbackPerformanceHistory.erase(pImpl->feedbackPerformanceHistory.begin());
        pImpl->feedforwardPerformanceHistory.erase(pImpl->feedforwardPerformanceHistory.begin());
    }
    
    // Update overall performance
    pImpl->overallFeedbackPerformance = 
        (std::accumulate(pImpl->stabilityHistory.begin(), pImpl->stabilityHistory.end(), 0.0f) / pImpl->stabilityHistory.size()) *
        (std::accumulate(pImpl->feedbackPerformanceHistory.begin(), pImpl->feedbackPerformanceHistory.end(), 0.0f) / pImpl->feedbackPerformanceHistory.size()) *
        (std::accumulate(pImpl->feedforwardPerformanceHistory.begin(), pImpl->feedforwardPerformanceHistory.end(), 0.0f) / pImpl->feedforwardPerformanceHistory.size());
}

void NeuromodulatorFeedback::updateState(const std::string& neuromodulatorName, const FeedbackSignalState& state) {
    pImpl->feedbackStates[neuromodulatorName] = state;
}

FeedbackSignalState NeuromodulatorFeedback::getState(const std::string& neuromodulatorName) const {
    auto it = pImpl->feedbackStates.find(neuromodulatorName);
    if (it != pImpl->feedbackStates.end()) {
        return it->second;
    }
    return FeedbackSignalState();
}

void NeuromodulatorFeedback::exportFeedback(std::ostream& stream) const {
    // Export feedback states
    size_t stateCount = pImpl->feedbackStates.size();
    stream.write(reinterpret_cast<const char*>(&stateCount), sizeof(size_t));
    
    for (const auto& [name, state] : pImpl->feedbackStates) {
        stream.write(name.c_str(), name.size() + 1);  // Null-terminated string
        stream.write(reinterpret_cast<const char*>(&state), sizeof(FeedbackSignalState));
    }
}

void NeuromodulatorFeedback::importFeedback(std::istream& stream) {
    // Import feedback states
    size_t stateCount;
    stream.read(reinterpret_cast<char*>(&stateCount), sizeof(size_t));
    
    pImpl->feedbackStates.clear();
    
    for (size_t i = 0; i < stateCount; ++i) {
        std::string name;
        char buffer[256];
        stream.read(buffer, sizeof(buffer));
        name = buffer;
        
        FeedbackSignalState state;
        stream.read(reinterpret_cast<char*>(&state), sizeof(FeedbackSignalState));
        
        pImpl->feedbackStates[name] = state;
    }
}

float NeuromodulatorFeedback::getFeedbackValue(const std::string& neuromodulatorName) const {
    auto it = pImpl->feedbackStates.find(neuromodulatorName);
    if (it != pImpl->feedbackStates.end()) {
        return it->second.currentValue;
    }
    return 0.0f;
}

float NeuromodulatorFeedback::getFeedforwardValue(const std::string& neuromodulatorName) const {
    auto it = pImpl->feedforwardStates.find(neuromodulatorName);
    if (it != pImpl->feedforwardStates.end()) {
        return it->second;
    }
    return 0.0f;
}

float NeuromodulatorFeedback::getLoopGain(const std::string& source, const std::string& target) const {
    if (pImpl->feedbackGains.find(source) != pImpl->feedbackGains.end() &&
        pImpl->feedbackGains[source].find(target) != pImpl->feedbackGains[source].end()) {
        return pImpl->feedbackGains[source][target];
    }
    return 0.0f;
}

void NeuromodulatorFeedback::shutdown() {
    if (pImpl) {
        reset();
        pImpl->brain.reset();
        pImpl->engine.reset();
    }
}

} // namespace nlm