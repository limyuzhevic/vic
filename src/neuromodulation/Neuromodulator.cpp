#include "Neuromodulator.hpp"
#include <algorithm>

namespace nlm {

struct Dopamine::Impl {
    float level;
    float baseline;
    float peak;
    float decayRate;
    float releaseRate;
    
    Impl() : level(0.0f), baseline(0.0f), peak(1.0f), decayRate(0.1f), releaseRate(1.0f) {}
};

Dopamine::Dopamine() : pImpl(new Impl) {}

Dopamine::~Dopamine() = default;

const char* Dopamine::getName() const {
    return "DA";
}

float Dopamine::getLevel() const {
    return pImpl->level;
}

void Dopamine::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, 1.0f);
}

float Dopamine::getPlasticityFactor() const {
    return 0.5f + 0.5f * pImpl->level;
}

void Dopamine::update(TimestepDuration dt) {
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

NeuromodulatorType Dopamine::getType() const {
    return NeuromodulatorType::Dopamine;
}

void Dopamine::reset() {
    pImpl->level = 0.0f;
}

void Dopamine::signalReward(float reward) {
    pImpl->level = std::min(pImpl->peak, pImpl->level + reward * pImpl->releaseRate);
}

void Dopamine::signalRewardPredictionError(float error) {
    pImpl->level = std::max(0.0f, pImpl->level + error * pImpl->releaseRate);
}

void Dopamine::enhanceRewardLearning(float strength) {
    pImpl->level = std::min(pImpl->peak, pImpl->level + strength * 0.1f);
}

void Dopamine::modulateSelectionStrength(float strength) {
    // Modulate selection strength based on dopamine level
    // Higher dopamine increases selection strength
    if (pImpl->level > 0.5f) {
        // Apply selection modulation
        // Implementation depends on specific selection mechanism
    }
}

struct Novelty::Impl {
    class Brain* brain;
    float level;
    float decayRate;
    std::vector<float> history;
    std::vector<float> lastPattern;
    float noveltyThreshold;
    
    Impl() : brain(nullptr), level(0.0f), decayRate(0.1f), noveltyThreshold(0.3f) {}
};

Novelty::Novelty() : pImpl(new Impl), history_(), lastPattern_(), noveltyThreshold_(0.3f) {}

Novelty::~Novelty() = default;

const char* Novelty::getName() const {
    return "Novelty";
}

float Novelty::getLevel() const {
    return pImpl->level;
}

void Novelty::setLevel(float level) {
    pImpl->level = level;
}

float Novelty::getPlasticityFactor() const {
    // Novelty increases plasticity for learning new information
    return 0.3f + 0.7f * pImpl->level;
}

void Novelty::update(TimestepDuration dt) {
    pImpl->level = std::max(0.0f, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

NeuromodulatorType Novelty::getType() const {
    return NeuromodulatorType::Novelty;
}

void Novelty::reset() {
    pImpl->level = 0.0f;
    pImpl->history.clear();
    pImpl->lastPattern.clear();
}

void Novelty::detectNovelty(const std::vector<float>& currentPattern,
                            const std::vector<float>& previousPattern) {
    if (currentPattern.empty() || previousPattern.empty()) {
        pImpl->level = 0.0f;
        return;
    }
    
    // Compute difference between patterns
    float totalDiff = 0.0f;
    size_t compareLen = std::min(currentPattern.size(), previousPattern.size());
    
    for (size_t i = 0; i < compareLen; ++i) {
        float diff = std::abs(currentPattern[i] - previousPattern[i]);
        totalDiff += diff;
    }
    
    // Normalize by pattern size
    float avgDiff = totalDiff / compareLen;
    
    // Update novelty level based on difference
    pImpl->level = std::min(1.0f, avgDiff / pImpl->noveltyThreshold);
    pImpl->history.push_back(pImpl->level);
    
    // Store current pattern for next comparison
    pImpl->lastPattern = currentPattern;
}

void Novelty::updatePattern(const std::vector<float>& pattern) {
    pImpl->lastPattern = pattern;
}

void Novelty::setNoveltyThreshold(float threshold) {
    pImpl->noveltyThreshold = threshold;
}

struct PredictionError::Impl {
    class Brain* brain;
    float error;
    float predictedValue;
    float actualValue;
    std::vector<float> history;
    
    Impl() : brain(nullptr), error(0.0f), predictedValue(0.0f), actualValue(0.0f) {}
};

PredictionError::PredictionError() : pImpl(new Impl), error_(0.0f), predictedValue_(0.0f), actualValue_(0.0f) {}

PredictionError::~PredictionError() = default;

const char* PredictionError::getName() const {
    return "PredictionError";
}

float PredictionError::getLevel() const {
    return pImpl->error;
}

void PredictionError::setLevel(float level) {
    pImpl->error = level;
}

float PredictionError::getPlasticityFactor() const {
    // Prediction error modulates plasticity for learning
    return 0.5f + 0.5f * std::abs(pImpl->error);
}

void PredictionError::update(TimestepDuration dt) {
    // Prediction error naturally decays over time
    pImpl->error = std::max(0.0f, pImpl->error - 0.1f * static_cast<float>(dt));
}

NeuromodulatorType PredictionError::getType() const {
    return NeuromodulatorType::PredictionError;
}

void PredictionError::reset() {
    pImpl->error = 0.0f;
    pImpl->history.clear();
}

void PredictionError::computeError(float predicted, float actual) {
    pImpl->predictedValue = predicted;
    pImpl->actualValue = actual;
    pImpl->error = actual - predicted;
    pImpl->history.push_back(pImpl->error);
}

void PredictionError::updatePrediction(float newPrediction) {
    pImpl->predictedValue = newPrediction;
}

void PredictionError::setLearningRate(float rate) {
    // Learning rate affects how prediction error updates
    // Implementation would use this in learning algorithms
}

struct Curiosity::Impl {
    class Brain* brain;
    float level;
    float noveltyWeight;
    float predictionErrorWeight;
    float decayRate;
    
    Impl() 
        : brain(nullptr)
        , level(0.0f)
        , noveltyWeight(0.5f)
        , predictionErrorWeight(0.5f)
        , decayRate(0.05f) {}
};

Curiosity::Curiosity() : pImpl(new Impl) {}

Curiosity::~Curiosity() = default;

const char* Curiosity::getName() const {
    return "Curiosity";
}

float Curiosity::getLevel() const {
    return pImpl->level;
}

void Curiosity::setLevel(float level) {
    pImpl->level = level;
}

float Curiosity::getPlasticityFactor() const {
    // Curiosity increases exploratory plasticity
    return 0.4f + 0.6f * pImpl->level;
}

void Curiosity::update(TimestepDuration dt) {
    pImpl->level = std::max(0.0f, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

NeuromodulatorType Curiosity::getType() const {
    return NeuromodulatorType::Curiosity;
}

void Curiosity::reset() {
    pImpl->level = 0.0f;
}

void Curiosity::update(float novelty, float predictionError, TimestepDuration dt) {
    // Curiosity increases with novelty and prediction error
    float target = pImpl->noveltyWeight * novelty + 
                   pImpl->predictionErrorWeight * predictionError;
    
    // Smooth update
    pImpl->level += (target - pImpl->level) * 0.1f;
    
    // Decay
    pImpl->level = std::max(0.0f, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

void Curiosity::setNoveltyWeight(float weight) {
    pImpl->noveltyWeight = weight;
}

void Curiosity::setPredictionErrorWeight(float weight) {
    pImpl->predictionErrorWeight = weight;
}

} // namespace nlm
