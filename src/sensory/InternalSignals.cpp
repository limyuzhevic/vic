#include "InternalSignals.hpp"
#include <algorithm>
#include <cmath>
#include <queue>
#include <unordered_map>
#include <array>

namespace nlm {

struct InternalSignalsProcessor::Impl {
    // Homeostatic state tracking
    std::vector<float> glucoseLevel; // Blood glucose concentration
    std::vector<float> oxygenLevel; // Oxygen saturation
    std::vector<float> temperature; // Core body temperature
    std::vector<float> hydration; // Body hydration level
    
    // Reward prediction error integration
    std::vector<float> rewardPredictionError; // RPE from dopaminergic system
    std::vector<float> expectedValue; // Expected future reward
    std::vector<float> predictionErrorHistory; // Historical prediction errors
    
    // Emotional valence mapping
    std::vector<float> valenceMap; // Emotional valence (-1 to 1)
    std::vector<float> arousalMap; // Emotional arousal (0 to 1)
    std::vector<float> dominanceMap; // Sense of control (0 to 1)
    
    // Interoceptive signals
    std::vector<float> interoceptiveSignals; // Body sensations
    std::vector<float> homeostaticDrives; // Drive states (hunger, thirst, etc.)
    
    // Motor connection signals
    std::vector<float> motorPlans; // Planned movements
    std::vector<float> actionValues; // Expected value of actions
    std::vector<float> actionSelections; // Selected actions
    
    // Processing buffers
    std::vector<float> processedSignals;
    std::vector<float> homeostaticSignals;
    
    Impl() {
        // Clear all vectors
        glucoseLevel.clear();
        oxygenLevel.clear();
        temperature.clear();
        hydration.clear();
        rewardPredictionError.clear();
        expectedValue.clear();
        predictionErrorHistory.clear();
        valenceMap.clear();
        arousalMap.clear();
        dominanceMap.clear();
        interoceptiveSignals.clear();
        homeostaticDrives.clear();
        motorPlans.clear();
        actionValues.clear();
        actionSelections.clear();
        processedSignals.clear();
        homeostaticSignals.clear();
    }
};

InternalSignalsProcessor::InternalSignalsProcessor() : pImpl(new Impl) {}

InternalSignalsProcessor::~InternalSignalsProcessor() = default;

void InternalSignalsProcessor::process(const InternalSignals& signals) {
    const std::vector<float>& data = signals.getData();
    size_t numSignals = data.size();
    
    // Initialize processing buffers
    pImpl->processedSignals.clear();
    pImpl->homeostaticSignals.clear();
    
    // Parse input signals (assuming structured format)
    parseInternalSignals(data, numSignals);
    
    // Implement homeostatic signal processing
    processHomeostaticSignals();
    
    // Integrate reward prediction error with interoception
    integrateRPEWithInteroception();
    
    // Map emotional valence
    mapEmotionalValence();
    
    // Connect to motor planning and action selection
    connectToMotorPlanning();
    
    // Generate final outputs
    generateOutputs();
}

void InternalSignalsProcessor::parseInternalSignals(const std::vector<float>& data, size_t numSignals) {
    // Clear all signal buffers
    pImpl->glucoseLevel.clear();
    pImpl->oxygenLevel.clear();
    pImpl->temperature.clear();
    pImpl->hydration.clear();
    pImpl->rewardPredictionError.clear();
    pImpl->expectedValue.clear();
    pImpl->predictionErrorHistory.clear();
    pImpl->interoceptiveSignals.clear();
    pImpl->homeostaticDrives.clear();
    
    // Parse signals from input vector
    // Signal layout: [glucose, oxygen, temperature, hydration, 
    //                  rpe, expected_value, pred_error_history (4 values),
    //                  interoceptive (4 values), drives (4 values),
    //                  motor_plans (4 values), action_values (4 values)]
    
    size_t idx = 0;
    
    // Homeostatic signals (4)
    if (idx + 4 <= numSignals) {
        for (size_t i = 0; i < 4; ++i) {
            pImpl->homeostaticDrives.push_back(data[idx + i]);
        }
        idx += 4;
    }
    
    // Parse into specific categories
    if (idx + 24 <= numSignals) {
        // Glucose level (continuous, 0-100 mg/dL)
        pImpl->glucoseLevel.push_back(data[idx]);
        idx++;
        
        // Oxygen level (continuous, 0-100%)
        pImpl->oxygenLevel.push_back(data[idx]);
        idx++;
        
        // Temperature (continuous, 35-42°C)
        pImpl->temperature.push_back(data[idx]);
        idx++;
        
        // Hydration level (continuous, 0-100%)
        pImpl->hydration.push_back(data[idx]);
        idx++;
        
        // Reward prediction error (from dopamine system)
        pImpl->rewardPredictionError.push_back(data[idx]);
        idx++;
        
        // Expected value of future outcomes
        pImpl->expectedValue.push_back(data[idx]);
        idx++;
        
        // Prediction error history (4 values for temporal integration)
        for (size_t i = 0; i < 4 && idx < numSignals; ++i) {
            pImpl->predictionErrorHistory.push_back(data[idx]);
            idx++;
        }
        
        // Interoceptive signals (4 body sensations)
        for (size_t i = 0; i < 4 && idx < numSignals; ++i) {
            pImpl->interoceptiveSignals.push_back(data[idx]);
            idx++;
        }
        
        // Drive states (4, normalized 0-1)
        for (size_t i = 0; i < 4 && idx < numSignals; ++i) {
            pImpl->homeostaticDrives.push_back(data[idx]);
            idx++;
        }
        
        // Motor plans (4, movement intentions)
        for (size_t i = 0; i < 4 && idx < numSignals; ++i) {
            pImpl->motorPlans.push_back(data[idx]);
            idx++;
        }
        
        // Action values (4, expected outcomes)
        for (size_t i = 0; i < 4 && idx < numSignals; ++i) {
            pImpl->actionValues.push_back(data[idx]);
            idx++;
        }
        
        // Action selections (4, softmax probabilities)
        for (size_t i = 0; i < 4 && idx < numSignals; ++i) {
            pImpl->actionSelections.push_back(data[idx]);
            idx++;
        }
    } else {
        // Default values if input is too short
        pImpl->glucoseLevel.push_back(100.0f); // Normal glucose
        pImpl->oxygenLevel.push_back(98.0f); // Normal oxygen
        pImpl->temperature.push_back(37.0f); // Normal temperature
        pImpl->hydration.push_back(75.0f); // Normal hydration
        pImpl->rewardPredictionError.push_back(0.0f); // No prediction error
        pImpl->expectedValue.push_back(0.5f); // Moderate expectation
        pImpl->interoceptiveSignals = {0.5f, 0.5f, 0.5f, 0.5f}; // Default interoception
        pImpl->homeostaticDrives = {0.5f, 0.5f, 0.5f, 0.5f}; // Default drives
        pImpl->motorPlans = {0.0f, 0.0f, 0.0f, 0.0f}; // No movement
        pImpl->actionValues = {0.5f, 0.5f, 0.5f, 0.5f}; // Equal action values
        pImpl->actionSelections = {0.25f, 0.25f, 0.25f, 0.25f}; // Equal probabilities
    }
}

void InternalSignalsProcessor::processHomeostaticSignals() {
    // Implement homeostatic regulation
    // Generate homeostatic signals based on internal state
    
    pImpl->homeostaticSignals.clear();
    pImpl->homeostaticSignals.reserve(8);
    
    // Clamp values to physiological ranges
    float glucose = pImpl->glucoseLevel.empty() ? 100.0f : std::max(70.0f, std::min(200.0f, pImpl->glucoseLevel[0]));
    float oxygen = pImpl->oxygenLevel.empty() ? 98.0f : std::max(90.0f, std::min(100.0f, pImpl->oxygenLevel[0]));
    float temp = pImpl->temperature.empty() ? 37.0f : std::max(36.0f, std::min(40.0f, pImpl->temperature[0]));
    float hydration = pImpl->hydration.empty() ? 75.0f : std::max(50.0f, std::min(100.0f, pImpl->hydration[0]));
    
    // Allostatic load: deviation from setpoint
    float glucoseAllostasis = std::abs(glucose - 100.0f) / 50.0f; // Scale to 0-1
    float oxygenAllostasis = std::abs(oxygen - 98.0f) / 10.0f;
    float tempAllostasis = std::abs(temp - 37.0f) / 5.0f;
    float hydrationAllostasis = std::abs(hydration - 75.0f) / 50.0f;
    
    // Metabolic state indicator
    float metabolicState = (glucose * 0.3f + oxygen * 0.2f + temp * 0.2f + hydration * 0.3f) / 100.0f;
    
    // Add homeostatic signals to output
    pImpl->homeostaticSignals.push_back(glucoseAllostasis); // Allostatic load
    pImpl->homeostaticSignals.push_back(metabolicState); // Metabolic state
    pImpl->homeostaticSignals.push_back((glucoseAllostasis + oxygenAllostasis) * 0.5f); // Combined stress
    pImpl->homeostaticSignals.push_back(hydration * 0.01f); // Hydration signal
    pImpl->homeostaticSignals.push_back(temp * 0.01f); // Temperature signal
    pImpl->homeostaticSignals.push_back(oxygen * 0.01f); // Oxygen signal
    pImpl->homeostaticSignals.push_back(glucose * 0.01f); // Glucose signal
    pImpl->homeostaticSignals.push_back(1.0f - metabolicState); // Energy availability
}

void InternalSignalsProcessor::integrateRPEWithInteroception() {
    // Integrate reward prediction error with interoceptive signals
    pImpl->rewardPredictionError.clear();
    pImpl->expectedValue.clear();
    
    // Calculate integrated RPE
    float baseRPE = pImpl->rewardPredictionError.empty() ? 0.0f : pImpl->rewardPredictionError[0];
    
    // Interoceptive modulation: body state affects reward processing
    float interoceptionSum = std::accumulate(pImpl->interoceptiveSignals.begin(),
                                           pImpl->interoceptiveSignals.end(), 0.0f);
    float interoceptiveModulation = interoceptionSum / pImpl->interoceptiveSignals.size();
    
    // Calculate expected value with interoceptive influence
    float baseEV = pImpl->expectedValue.empty() ? 0.5f : pImpl->expectedValue[0];
    float integratedEV = baseEV * (1.0f + interoceptiveModulation * 0.2f);
    
    // Update prediction error history
    pImpl->predictionErrorHistory.clear();
    
    // Add new error to history
    float newError = baseRPE * (1.0f + interoceptiveModulation * 0.1f);
    pImpl->predictionErrorHistory.push_back(newError);
    
    // Temporal integration of prediction errors
    float integratedRPE = newError;
    for (size_t i = 0; i < 3 && i < pImpl->predictionErrorHistory.size() - 1; ++i) {
        integratedRPE += pImpl->predictionErrorHistory[i] * 0.1f * (i + 1);
    }
    
    pImpl->rewardPredictionError.push_back(integratedRPE);
    pImpl->expectedValue.push_back(integratedEV);
}

void InternalSignalsProcessor::mapEmotionalValence() {
    // Map homeostatic and interoceptive signals to emotional valence
    pImpl->valenceMap.clear();
    pImpl->arousalMap.clear();
    pImpl->dominanceMap.clear();
    
    // Calculate valence (positive/negative emotional quality)
    float valence = 0.0f;
    
    // Glucose affects satiety valence (high = negative, low = positive)
    if (!pImpl->glucoseLevel.empty()) {
        float glucoseValence = (pImpl->glucoseLevel[0] - 100.0f) / 100.0f; // -1 to 1
        valence += glucoseValence * 0.3f;
    }
    
    // Oxygen affects valence (low = negative)
    if (!pImpl->oxygenLevel.empty()) {
        float oxygenValence = (pImpl->oxygenLevel[0] - 98.0f) / 10.0f; // -0.2 to 0.2
        valence += oxygenValence * 0.2f;
    }
    
    // Temperature affects valence (too hot/cold = negative)
    if (!pImpl->temperature.empty()) {
        float tempDeviation = std::abs(pImpl->temperature[0] - 37.0f);
        float tempValence = -(tempDeviation / 5.0f); // Negative for deviation
        valence += tempValence * 0.2f;
    }
    
    // Interoceptive signals contribute to valence
    float interoceptionValence = 0.0f;
    for (float signal : pImpl->interoceptiveSignals) {
        interoceptionValence += (signal - 0.5f) * 2.0f; // Scale to -1 to 1
    }
    valence += interoceptionValence * 0.3f;
    
    // Clamp valence to valid range
    valence = std::max(-1.0f, std::min(1.0f, valence));
    
    // Calculate arousal (intensity of emotion)
    float arousal = 0.0f;
    arousal += std::abs(pImpl->rewardPredictionError.empty() ? 0.0f : pImpl->rewardPredictionError[0]) * 0.4f;
    arousal += std::abs(valence) * 0.3f;
    
    // Interoceptive variance contributes to arousal
    if (pImpl->interoceptiveSignals.size() > 1) {
        float mean = std::accumulate(pImpl->interoceptiveSignals.begin(),
                                   pImpl->interoceptiveSignals.end(), 0.0f) /
                    pImpl->interoceptiveSignals.size();
        float variance = 0.0f;
        for (float signal : pImpl->interoceptiveSignals) {
            variance += (signal - mean) * (signal - mean);
        }
        variance /= pImpl->interoceptiveSignals.size();
        arousal += variance * 0.3f;
    }
    
    // Normalize arousal to [0, 1]
    arousal = std::min(1.0f, arousal);
    
    // Calculate dominance (sense of control)
    float dominance = 0.0f;
    if (!pImpl->actionSelections.empty()) {
        // Higher confidence in action selection = higher dominance
        float maxSelection = *std::max_element(pImpl->actionSelections.begin(),
                                              pImpl->actionSelections.end());
        dominance = maxSelection;
    }
    
    // Store in maps
    pImpl->valenceMap.push_back(valence);
    pImpl->arousalMap.push_back(arousal);
    pImpl->dominanceMap.push_back(dominance);
}

void InternalSignalsProcessor::connectToMotorPlanning() {
    // Connect to motor planning and action selection
    pImpl->motorPlans.clear();
    pImpl->actionValues.clear();
    pImpl->actionSelections.clear();
    
    // If no original plans, generate based on needs
    if (pImpl->motorPlans.empty()) {
        // Generate motor plans based on homeostatic needs
        std::vector<float> generatedPlans;
        
        // Hungry -> move to food area
        if (!pImpl->homeostaticDrives.empty() && pImpl->homeostaticDrives[0] > 0.7f) {
            generatedPlans.push_back(1.0f); // Move forward
            generatedPlans.push_back(0.0f); // No turning
        } else {
            generatedPlans.push_back(0.0f);
            generatedPlans.push_back(0.0f);
        }
        
        // Thirsty -> seek water
        if (!pImpl->homeostaticDrives.empty() && pImpl->homeostaticDrives[1] > 0.7f) {
            generatedPlans.push_back(0.0f);
            generatedPlans.push_back(0.0f);
            generatedPlans.push_back(1.0f); // Drink action
        } else {
            generatedPlans.push_back(0.0f);
            generatedPlans.push_back(0.0f);
            generatedPlans.push_back(0.0f);
        }
        
        // Temperature regulation -> adjust movement
        if (!pImpl->temperature.empty() && (pImpl->temperature[0] < 35.0f || pImpl->temperature[0] > 39.0f)) {
            generatedPlans.push_back(0.0f);
            generatedPlans.push_back(0.0f);
            generatedPlans.push_back(0.0f);
            generatedPlans.push_back(1.0f); // Shelter/thermal regulation
        }
        
        pImpl->motorPlans = generatedPlans;
    }
    
    // Update action values based on emotion and prediction error
    if (!pImpl->valenceMap.empty() && !pImpl->rewardPredictionError.empty()) {
        float valence = pImpl->valenceMap[0];
        float rpe = pImpl->rewardPredictionError[0];
        
        // Positive valence and positive RPE increase action value
        float actionValue = (valence + 1.0f) * 0.5f * (rpe + 1.0f) * 0.5f;
        
        // Generate action values for multiple actions
        for (size_t i = 0; i < 4; ++i) {
            float baseValue = actionValue * (0.5f + static_cast<float>(i) * 0.2f);
            pImpl->actionValues.push_back(baseValue);
        }
    }
    
    // Calculate action selection probabilities (softmax over action values)
    if (!pImpl->actionValues.empty()) {
        float maxValue = *std::max_element(pImpl->actionValues.begin(), pImpl->actionValues.end());
        std::vector<float> expValues;
        
        for (float value : pImpl->actionValues) {
            expValues.push_back(std::exp(value - maxValue)); // For numerical stability
        }
        
        float sumExp = std::accumulate(expValues.begin(), expValues.end(), 0.0f);
        
        for (float expVal : expValues) {
            pImpl->actionSelections.push_back(expVal / sumExp);
        }
    }
}

void InternalSignalsProcessor::generateOutputs() {
    // Generate final processed signals for output
    pImpl->processedSignals.clear();
    
    // Combine all processed signals
    size_t totalSignals = 0;
    
    // Homeostatic signals
    totalSignals += pImpl->homeostaticSignals.size();
    pImpl->processedSignals.insert(pImpl->processedSignals.end(),
                                 pImpl->homeostaticSignals.begin(),
                                 pImpl->homeostaticSignals.end());
    
    // RPE and expected value
    totalSignals += pImpl->rewardPredictionError.size();
    pImpl->processedSignals.insert(pImpl->processedSignals.end(),
                                 pImpl->rewardPredictionError.begin(),
                                 pImpl->rewardPredictionError.end());
    
    totalSignals += pImpl->expectedValue.size();
    pImpl->processedSignals.insert(pImpl->processedSignals.end(),
                                 pImpl->expectedValue.begin(),
                                 pImpl->expectedValue.end());
    
    // Emotional maps
    totalSignals += pImpl->valenceMap.size();
    pImpl->processedSignals.insert(pImpl->processedSignals.end(),
                                 pImpl->valenceMap.begin(),
                                 pImpl->valenceMap.end());
    
    totalSignals += pImpl->arousalMap.size();
    pImpl->processedSignals.insert(pImpl->processedSignals.end(),
                                 pImpl->arousalMap.begin(),
                                 pImpl->arousalMap.end());
    
    totalSignals += pImpl->dominanceMap.size();
    pImpl->processedSignals.insert(pImpl->processedSignals.end(),
                                 pImpl->dominanceMap.begin(),
                                 pImpl->dominanceMap.end());
    
    // Interoceptive signals
    totalSignals += pImpl->interoceptiveSignals.size();
    pImpl->processedSignals.insert(pImpl->processedSignals.end(),
                                 pImpl->interoceptiveSignals.begin(),
                                 pImpl->interoceptiveSignals.end());
    
    // Motor and action signals
    totalSignals += pImpl->motorPlans.size();
    pImpl->processedSignals.insert(pImpl->processedSignals.end(),
                                 pImpl->motorPlans.begin(),
                                 pImpl->motorPlans.end());
    
    totalSignals += pImpl->actionValues.size();
    pImpl->processedSignals.insert(pImpl->processedSignals.end(),
                                 pImpl->actionValues.begin(),
                                 pImpl->actionValues.end());
    
    totalSignals += pImpl->actionSelections.size();
    pImpl->processedSignals.insert(pImpl->processedSignals.end(),
                                 pImpl->actionSelections.begin(),
                                 pImpl->actionSelections.end());
    
    // Ensure we have at least some output
    if (pImpl->processedSignals.empty()) {
        pImpl->processedSignals = {0.5f, 0.5f, 0.5f, 0.5f, 0.5f, 0.5f, 0.5f, 0.5f,
                                  0.5f, 0.5f, 0.5f, 0.5f, 0.5f, 0.5f, 0.5f, 0.5f,
                                  0.5f, 0.5f, 0.5f, 0.5f, 0.5f, 0.5f, 0.5f, 0.5f,
                                  0.5f, 0.5f, 0.5f, 0.5f, 0.5f, 0.5f, 0.5f, 0.5f};
    }
}

const std::vector<float>& InternalSignalsProcessor::getProcessedSignals() const {
    return pImpl->processedSignals;
}

const std::vector<float>& InternalSignalsProcessor::getHomeostaticSignals() const {
    return pImpl->homeostaticSignals;
}

}
