#include "PredictionError.hpp"
#include "../core/Logger/Logger.hpp"
#include "../core/Config/Config.hpp"
#include <cmath>
#include <algorithm>

namespace nlm {

struct PredictionError::Impl {
    class Brain* brain;
    float error;
    float predictedValue;
    float actualValue;
    float learningProgress;
    float confidence;
    float adaptiveThreshold;
    float eligibilityTrace;
    float temporalDifference;
    float neuromodulationWeight;
    std::vector<float> history;
    std::vector<float> eligibilityHistory;
    std::map<std::string, float> crossModulatorInfluence;
    
    // Interaction effects
    float dopamineInfluence;      // From Dopamine system
    float serotoninInfluence;     // From Serotonin system
    float acetylcholineInfluence; // From Acetylcholine system
    float endorphinsInfluence;    // From Endorphins system
    float curiosityInfluence;     // From Curiosity system
    float noveltyInfluence;       // From Novelty system
    
    Impl() : brain(nullptr), error(0.0f), predictedValue(0.0f), actualValue(0.0f),
             learningProgress(0.0f), confidence(1.0f), adaptiveThreshold(0.1f),
             eligibilityTrace(0.0f), temporalDifference(0.0f), neuromodulationWeight(1.0f),
             dopamineInfluence(0.0f), serotoninInfluence(0.0f), acetylcholineInfluence(0.0f),
             endorphinsInfluence(0.0f), curiosityInfluence(0.0f), noveltyInfluence(0.0f) {}
};

PredictionError::PredictionError() : pImpl(new Impl) {}

PredictionError::~PredictionError() = default;

void PredictionError::initialize(Brain* brain) {
    pImpl->brain = brain;
    NLM_LOG_INFO("Advanced PredictionError system initialized");
}

float PredictionError::getError() const {
    return pImpl->error;
}

void PredictionError::computeError(float predicted, float actual) {
    // Advanced error computation with neuromodulator integration
    
    // Base prediction error
    float baseError = actual - predicted;
    
    // Neuromodulator-weighted error
    float neuromodulatorEffect = 1.0f + pImpl->dopamineInfluence * 0.5f;
    neuromodulatorEffect *= (1.0f + pImpl->serotoninInfluence * 0.3f);
    neuromodulatorEffect *= (1.0f + pImpl->acetylcholineInfluence * 0.2f);
    neuromodulatorEffect *= (1.0f - pImpl->endorphinsInfluence * 0.1f); // Endorphins reduce error sensitivity
    neuromodulatorEffect *= (1.0f + pImpl->curiosityInfluence * 0.15f);
    neuromodulatorEffect *= (1.0f + pImpl->noveltyInfluence * 0.15f);
    
    pImpl->error = baseError * neuromodulatorEffect;
    
    // Confidence adjustment based on neuromodulators
    pImpl->confidence = std::lerp(pImpl->confidence, 1.0f - std::abs(pImpl->error), 0.1f);
    
    // Adaptive threshold based on learning progress
    pImpl->adaptiveThreshold = 0.1f + (1.0f - pImpl->learningProgress) * 0.2f;
    
    // Learning progress influences error significance
    if (pImpl->learningProgress > 0.7f) {
        // When learning well, reduce error significance
        pImpl->error *= (1.0f - (pImpl->learningProgress - 0.7f) * 0.3f);
    }
    
    // Update history
    pImpl->history.push_back(pImpl->error);
    
    // Calculate eligibility trace
    pImpl->eligibilityTrace = std::lerp(pImpl->eligibilityTrace, std::abs(pImpl->error), 0.2f);
    pImpl->eligibilityHistory.push_back(pImpl->eligibilityTrace);
    
    // Update temporal difference
    pImpl->temporalDifference = pImpl->error * pImpl->confidence;
    
    // Update neuromodulation weight based on error magnitude and confidence
    pImpl->neuromodulationWeight = std::lerp(pImpl->neuromodulationWeight, 
                                            std::min(1.0f, std::abs(pImpl->error) / pImpl->adaptiveThreshold), 
                                            0.1f);
}

void PredictionError::updatePrediction(float newPrediction) {
    pImpl->predictedValue = newPrediction;
}

float PredictionError::getMagnitude() const {
    // Advanced magnitude calculation with neuromodulator effects
    float baseMagnitude = std::abs(pImpl->error);
    
    // Neuromodulator-modulated magnitude
    float magnitudeModulation = 1.0f;
    magnitudeModulation += pImpl->dopamineInfluence * 0.4f;
    magnitudeModulation += pImpl->serotoninInfluence * 0.3f;
    magnitudeModulation += pImpl->curiosityInfluence * 0.2f;
    magnitudeModulation -= pImpl->endorphinsInfluence * 0.1f;
    magnitudeModulation += pImpl->noveltyInfluence * 0.15f;
    magnitudeModulation *= pImpl->confidence;
    
    float magnitude = baseMagnitude * magnitudeModulation;
    
    // Learning progress effect on magnitude perception
    if (pImpl->learningProgress > 0.8f) {
        magnitude *= (1.0f - (pImpl->learningProgress - 0.8f) * 0.5f);
    }
    
    return magnitude;
}

float PredictionError::getTemporalDifference() const {
    return pImpl->temporalDifference;
}

float PredictionError::getEligibilityTrace() const {
    return pImpl->eligibilityTrace;
}

float PredictionError::getNeuromodulationWeight() const {
    return pImpl->neuromodulationWeight;
}

float PredictionError::getConfidence() const {
    return pImpl->confidence;
}

float PredictionError::getLearningProgress() const {
    return pImpl->learningProgress;
}

void PredictionError::setLearningProgress(float progress) {
    pImpl->learningProgress = std::clamp(progress, 0.0f, 1.0f);
}

void PredictionError::setConfidence(float confidence) {
    pImpl->confidence = std::clamp(confidence, 0.0f, 1.0f);
}

void PredictionError::setAdaptiveThreshold(float threshold) {
    pImpl->adaptiveThreshold = std::clamp(threshold, 0.01f, 0.5f);
}

void PredictionError::addCrossModulatorInfluence(const std::string& modulatorName, float influence) {
    if (modulatorName == "dopamine") {
        pImpl->dopamineInfluence = std::clamp(influence, 0.0f, 1.0f);
    } else if (modulatorName == "serotonin") {
        pImpl->serotoninInfluence = std::clamp(influence, 0.0f, 1.0f);
    } else if (modulatorName == "acetylcholine") {
        pImpl->acetylcholineInfluence = std::clamp(influence, 0.0f, 1.0f);
    } else if (modulatorName == "endorphins") {
        pImpl->endorphinsInfluence = std::clamp(influence, 0.0f, 1.0f);
    } else if (modulatorName == "curiosity") {
        pImpl->curiosityInfluence = std::clamp(influence, 0.0f, 1.0f);
    } else if (modulatorName == "novelty") {
        pImpl->noveltyInfluence = std::clamp(influence, 0.0f, 1.0f);
    }
}

const std::vector<float>& PredictionError::getHistory() const {
    return pImpl->history;
}

const std::vector<float>& PredictionError::getEligibilityHistory() const {
    return pImpl->eligibilityHistory;
}

void PredictionError::clearHistory() {
    pImpl->history.clear();
}

void PredictionError::clearEligibilityHistory() {
    pImpl->eligibilityHistory.clear();
}

void PredictionError::reset() {
    pImpl->error = 0.0f;
    pImpl->predictedValue = 0.0f;
    pImpl->actualValue = 0.0f;
    pImpl->confidence = 1.0f;
    pImpl->learningProgress = 0.0f;
    pImpl->history.clear();
    pImpl->eligibilityHistory.clear();
    pImpl->eligibilityTrace = 0.0f;
    pImpl->temporalDifference = 0.0f;
    pImpl->neuromodulationWeight = 1.0f;
}

} // namespace nlm
