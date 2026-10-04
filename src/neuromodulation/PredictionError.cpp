#include "PredictionError.hpp"
#include "../brain/Brain.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <deque>
#include <unordered_map>

namespace nlm {

struct PredictionError::Impl {
    // Temporal difference learning components
    struct {
        float currentQValue;       // Current estimated Q-value
        float targetQValue;        // Target Q-value (TD target)
        float temporalDifference;  // TD error (delta)
        float learningRate;        // TD learning rate (alpha)
        float discountFactor;      // Discount factor for future rewards (gamma)
        std::deque<float> eligibilityTrace; // Eligibility trace for credit assignment
        size_t traceMaxLength;     // Maximum length of eligibility trace
    } tdLearning;
    
    // Multi-step prediction error computation
    struct {
        std::vector<float> predictionHistory;  // History of predictions
        std::vector<float> errorHistory;      // History of prediction errors
        std::vector<size_t> timeSteps;       // Time steps for errors
        float predictionErrorVariance;        // Variance of prediction errors
        float predictionErrorMean;            // Mean of prediction errors
        size_t maxPredictionHistory;         // Maximum prediction history size
    } multiStep;
    
    // Online learning and prediction updates
    struct {
        float lastPrediction;       // Last predicted value
        float lastActualValue;      // Last actual value
        float predictionUpdateRate; // Rate of prediction updates
        float uncertaintyEstimate;  // Estimate of prediction uncertainty
        std::unordered_map<size_t, float> stateValues; // Value estimates for each state
        std::unordered_map<size_t, std::vector<float>> actionValues; // Value estimates for each (state, action) pair
        size_t currentState;       // Current state identifier
        size_t currentAction;      // Current action identifier
    } onlineLearning;
    
    // Credit assignment mechanisms
    struct {
        std::unordered_map<size_t, float> stateEligibilityTraces; // Eligibility traces for each state
        std::unordered_map<size_t, std::unordered_map<size_t, float>> actionEligibilityTraces; // Eligibility traces for each (state, action) pair
        float traceDecayRate;     // Decay rate for eligibility traces
        float traceIncrementRate; // Increment rate for eligibility traces
        float maxTraceValue;      // Maximum eligibility trace value
    } creditAssignment;
    
    // Implementation tracking
    bool initialized;            // Whether prediction error system has been initialized
    bool enabled;                 // Whether prediction error system is enabled
    float lastUpdateTime;         // Last update timestamp
    float predictionErrorHistory; // History of prediction error values
    size_t totalPredictionErrors;  // Total number of prediction errors computed
    
    Impl() {
        // Initialize TD learning
        tdLearning.currentQValue = 0.0f;
        tdLearning.targetQValue = 0.0f;
        tdLearning.temporalDifference = 0.0f;
        tdLearning.learningRate = 0.1f;
        tdLearning.discountFactor = 0.95f;
        tdLearning.traceMaxLength = 100;
        tdLearning.eligibilityTrace.reserve(tdLearning.traceMaxLength);
        
        // Initialize multi-step prediction
        multiStep.predictionErrorVariance = 0.0f;
        multiStep.predictionErrorMean = 0.0f;
        multiStep.maxPredictionHistory = 1000;
        
        // Initialize online learning
        onlineLearning.lastPrediction = 0.0f;
        onlineLearning.lastActualValue = 0.0f;
        onlineLearning.predictionUpdateRate = 0.01f;
        onlineLearning.uncertaintyEstimate = 0.1f;
        onlineLearning.currentState = 0;
        onlineLearning.currentAction = 0;
        
        // Initialize credit assignment
        creditAssignment.traceDecayRate = 0.9f;
        creditAssignment.traceIncrementRate = 0.1f;
        creditAssignment.maxTraceValue = 1.0f;
        
        // Implementation tracking
        initialized = false;
        enabled = true;
        lastUpdateTime = 0.0f;
        predictionErrorHistory = 0.0f;
        totalPredictionErrors = 0;
    }
};

PredictionError::PredictionError() : pImpl(new Impl) {}

PredictionError::~PredictionError() = default;

void PredictionError::initialize(Brain* brain) {
    if (!brain) {
        NLM_LOG_ERROR("Cannot initialize PredictionError system with null brain");
        return;
    }
    
    pImpl->brain = brain;
    pImpl->initialized = true;
    
    NLM_LOG_INFO("PredictionError system initialized with TD learning and eligibility traces");
}

float PredictionError::getError() const {
    if (!pImpl->initialized) return 0.0f;
    
    // Return the most recent prediction error
    if (!pImpl->multiStep.errorHistory.empty()) {
        return pImpl->multiStep.errorHistory.back();
    }
    
    return pImpl->tdLearning.temporalDifference;
}

void PredictionError::computeError(float predicted, float actual) {
    if (!pImpl->initialized) return;
    
    float error = actual - predicted;
    
    // Store in history
    pImpl->multiStep.errorHistory.push_back(error);
    pImpl->multiStep.predictionHistory.push_back(predicted);
    pImpl->multiStep.timeSteps.push_back(pImpl->totalPredictionErrors++);
    
    // Maintain history size limits
    if (pImpl->multiStep.errorHistory.size() > pImpl->multiStep.maxPredictionHistory) {
        pImpl->multiStep.errorHistory.erase(pImpl->multiStep.errorHistory.begin());
        pImpl->multiStep.predictionHistory.erase(pImpl->multiStep.predictionHistory.begin());
        pImpl->multiStep.timeSteps.erase(pImpl->multiStep.timeSteps.begin());
    }
    
    // Update statistics
    updateErrorStatistics();
    
    // Update TD learning components
    updateTDLearning(error, predicted, actual);
    
    // Update online learning
    updateOnlineLearning(error, predicted, actual);
    
    // Update credit assignment
    updateCreditAssignment(predicted, actual);
    
    // Store last values
    pImpl->onlineLearning.lastPrediction = predicted;
    pImpl->onlineLearning.lastActualValue = actual;
    
    pImpl->predictionErrorHistory = error;
    
    NLM_LOG_DEBUG("PredictionError computed: predicted=" + std::to_string(predicted) + 
                 ", actual=" + std::to_string(actual) + 
                 ", error=" + std::to_string(error) + 
                 ", TD=" + std::to_string(pImpl->tdLearning.temporalDifference));
}

void PredictionError::updatePrediction(float newPrediction) {
    if (!pImpl->initialized) return;
    
    pImpl->onlineLearning.lastPrediction = newPrediction;
    pImpl->multiStep.predictionHistory.push_back(newPrediction);
    
    // Maintain history size
    if (pImpl->multiStep.predictionHistory.size() > pImpl->multiStep.maxPredictionHistory) {
        pImpl->multiStep.predictionHistory.erase(pImpl->multiStep.predictionHistory.begin());
    }
}

const std::vector<float>& PredictionError::getHistory() const {
    return pImpl->multiStep.errorHistory;
}

void PredictionError::clearHistory() {
    pImpl->multiStep.errorHistory.clear();
    pImpl->multiStep.predictionHistory.clear();
    pImpl->multiStep.timeSteps.clear();
    pImpl->predictionErrorHistory = 0.0f;
    
    NLM_LOG_INFO("PredictionError system history cleared");
}

float PredictionError::getMagnitude() const {
    return std::abs(getError());
}

void PredictionError::updateTDLearning(float error, float predicted, float actual) {
    // Update TD learning
    pImpl->tdLearning.temporalDifference = error;
    
    // Update current Q-value using TD update rule
    pImpl->tdLearning.currentQValue += pImpl->tdLearning.learningRate * error;
    
    // Update eligibility trace
    updateEligibilityTrace(error);
    
    // Compute target Q-value (TD target)
    // In a complete implementation, this would consider future rewards
    pImpl->tdLearning.targetQValue = actual + 
        pImpl->tdLearning.discountFactor * 
        (pImpl->tdLearning.currentQValue); // Simplified TD target
}

void PredictionError::updateOnlineLearning(float error, float predicted, float actual) {
    if (!pImpl->brain) return;
    
    auto* rng = pImpl->brain->getRandomGenerator();
    if (!rng) return;
    
    // Update state value estimate
    size_t currentState = pImpl->onlineLearning.currentState;
    
    // TD update for state value
    float currentStateValue = getStateValue(currentState);
    float tdUpdate = pImpl->tdLearning.learningRate * error;
    setStateValue(currentState, currentStateValue + tdUpdate);
    
    // Update action value if we have an action
    size_t currentAction = pImpl->onlineLearning.currentAction;
    auto& actionValues = getActionValues(currentState);
    
    if (!actionValues.empty()) {
        // Compute average action value
        float sum = std::accumulate(actionValues.begin(), actionValues.end(), 0.0f);
        float avgValue = sum / actionValues.size();
        
        // Update action value based on prediction error
        float actionUpdate = pImpl->tdLearning.learningRate * error * 
                           (actual - avgValue); // Advantage calculation
        actionValues[currentAction] += actionUpdate;
    }
    
    // Update prediction uncertainty estimate
    float variance = 0.0f;
    if (pImpl->multiStep.errorHistory.size() > 1) {
        float mean = pImpl->multiStep.predictionErrorMean;
        for (float e : pImpl->multiStep.errorHistory) {
            variance += (e - mean) * (e - mean);
        }
        variance /= pImpl->multiStep.errorHistory.size();
    }
    
    pImpl->onlineLearning.uncertaintyEstimate = std::sqrt(std::max(0.0f, variance));
}

void PredictionError::updateCreditAssignment(float predicted, float actual) {
    if (!pImpl->brain) return;
    
    float error = actual - predicted;
    
    // Update state eligibility traces
    size_t currentState = pImpl->onlineLearning.currentState;
    
    // Decay existing traces
    for (auto& pair : pImpl->creditAssignment.stateEligibilityTraces) {
        pair.second *= pImpl->creditAssignment.traceDecayRate;
    }
    
    // Update current state trace
    pImpl->creditAssignment.stateEligibilityTraces[currentState] = 
        std::min(pImpl->creditAssignment.maxTraceValue,
                pImpl->creditAssignment.stateEligibilityTraces[currentState] + 
                pImpl->creditAssignment.traceIncrementRate);
    
    // Update action eligibility traces
    size_t currentAction = pImpl->onlineLearning.currentAction;
    
    for (auto& statePair : pImpl->creditAssignment.actionEligibilityTraces) {
        for (auto& actionPair : statePair.second) {
            actionPair.second *= pImpl->creditAssignment.traceDecayRate;
        }
    }
    
    // Update current action trace
    pImpl->creditAssignment.actionEligibilityTraces[currentState][currentAction] = 
        std::min(pImpl->creditAssignment.maxTraceValue,
                pImpl->creditAssignment.actionEligibilityTraces[currentState][currentAction] + 
                pImpl->creditAssignment.traceIncrementRate);
}

void PredictionError::updateEligibilityTrace(float error) {
    // Add current error to eligibility trace
    pImpl->tdLearning.eligibilityTrace.push_back(std::abs(error) * pImpl->tdLearning.learningRate);
    
    // Maintain trace length
    if (pImpl->tdLearning.eligibilityTrace.size() > pImpl->tdLearning.traceMaxLength) {
        pImpl->tdLearning.eligibilityTrace.pop_front();
    }
    
    // Decay trace over time
    for (size_t i = 0; i < pImpl->tdLearning.eligibilityTrace.size(); ++i) {
        pImpl->tdLearning.eligibilityTrace[i] *= pImpl->creditAssignment.traceDecayRate;
    }
}

void PredictionError::updateErrorStatistics() {
    if (pImpl->multiStep.errorHistory.empty()) return;
    
    float sum = std::accumulate(pImpl->multiStep.errorHistory.begin(), 
                               pImpl->multiStep.errorHistory.end(), 0.0f);
    float mean = sum / pImpl->multiStep.errorHistory.size();
    
    pImpl->multiStep.predictionErrorMean = mean;
    
    // Compute variance
    float variance = 0.0f;
    for (float error : pImpl->multiStep.errorHistory) {
        variance += (error - mean) * (error - mean);
    }
    variance /= pImpl->multiStep.errorHistory.size();
    
    pImpl->multiStep.predictionErrorVariance = variance;
}

float PredictionError::getStateValue(size_t state) const {
    auto it = pImpl->onlineLearning.stateValues.find(state);
    if (it != pImpl->onlineLearning.stateValues.end()) {
        return it->second;
    }
    return 0.0f;
}

void PredictionError::setStateValue(size_t state, float value) {
    pImpl->onlineLearning.stateValues[state] = value;
}

std::unordered_map<size_t, float>& PredictionError::getActionValues(size_t state) {
    auto it = pImpl->onlineLearning.actionValues.find(state);
    if (it != pImpl->onlineLearning.actionValues.end()) {
        return it->second;
    }
    return pImpl->onlineLearning.actionValues[state]; // Create empty map if not exists
}

void PredictionError::setCurrentState(size_t state) {
    pImpl->onlineLearning.currentState = state;
}

void PredictionError::setCurrentAction(size_t action) {
    pImpl->onlineLearning.currentAction = action;
}

float PredictionError::computeMultiStepTD(float currentStateValue, float nextStateValue, 
                                          float reward, size_t steps) {
    // Compute multi-step TD error
    float target = reward;
    
    // Look ahead multiple steps
    for (size_t i = 0; i < steps; ++i) {
        // In a complete implementation, this would consider the value of the next state
        target += pImpl->tdLearning.discountFactor * 
                 (pImpl->onlineLearning.stateValues[pImpl->onlineLearning.currentState] - currentStateValue);
    }
    
    // Compute TD error
    float tdError = target - currentStateValue;
    
    return tdError;
}

float PredictionError::getExpectedReturn(size_t state, size_t action) const {
    auto actionIt = pImpl->onlineLearning.actionValues.find(state);
    if (actionIt != pImpl->onlineLearning.actionValues.end()) {
        auto actionMap = actionIt->second;
        auto actionValIt = actionMap.find(action);
        if (actionValIt != actionMap.end()) {
            return actionValIt->second;
        }
    }
    return 0.0f;
}

void PredictionError::updateStateActionValue(size_t state, size_t action, float value) {
    pImpl->onlineLearning.actionValues[state][action] = value;
}

} // namespace nlm
