#include "Neuromodulator.hpp"
#include <algorithm>

// Neuromodulator Implementation
// TODO PHASE 2: Implement real neuromodulation effects

Neuromodulator::Neuromodulator() = default;

float Neuromodulator::getPlasticityFactor() const {
    return 1.0f;
}

void Neuromodulator::update(TimestepDuration dt) {}

// Dopamine Implementation
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
    // TODO PHASE 2: Implement real dopamine-modulated plasticity factor
    // PLACEHOLDER: Higher dopamine increases plasticity
    return 0.5f + 0.5f * pImpl->level;
}

void Dopamine::update(TimestepDuration dt) {
    // TODO PHASE 2: Implement real dopamine dynamics
    // PLACEHOLDER: Decay towards baseline
    pImpl->level = std::max(pImpl->baseline, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

void Dopamine::signalReward(float reward) {
    // TODO PHASE 2: Implement real reward signaling
    // PLACEHOLDER: Burst of dopamine on reward
    pImpl->level = std::min(pImpl->peak, pImpl->level + reward * pImpl->releaseRate);
}

void Dopamine::signalRewardPredictionError(float error) {
    // TODO PHASE 2: Implement reward prediction error signaling
    // PLACEHOLDER: Dopamine responds to prediction error
    pImpl->level = std::max(0.0f, pImpl->level + error * pImpl->releaseRate);
}

// Curiosity Implementation
Curiosity::Curiosity()
    : pImpl(new Impl)
    , level_(0.0f)
    , explorationDrive_(0.0f)
{
}

Curiosity::~Curiosity() = default;

void Curiosity::initialize(Brain* brain) {
    pImpl->brain = brain;
    NLM_LOG_INFO("Curiosity initialized");
}

float Curiosity::getLevel() const {
    return level_;
}

void Curiosity::update(float novelty, float predictionError, TimestepDuration dt) {
    // Update curiosity based on novelty and prediction error
    float noveltyInfluence = novelty * 0.6f;
    float errorInfluence = std::abs(predictionError) * 0.4f;
    
    level_ = noveltyInfluence + errorInfluence;
    explorationDrive_ = level_;
    
    // Decay over time
    level_ *= (1.0f - dt * 0.001f);
}

float Curiosity::getExplorationDrive() const {
    return explorationDrive_;
}

void Curiosity::setNoveltyWeight(float weight) {
    // Store for use in update
    pImpl->noveltyWeight = weight;
}

void Curiosity::setPredictionErrorWeight(float weight) {
    // Store for use in update
    pImpl->predictionErrorWeight = weight;
}

void Curiosity::reset() {
    level_ = 0.0f;
    explorationDrive_ = 0.0f;
}

// Novelty Implementation
Novelty::Novelty()
    : pImpl(new Impl)
    , level_(0.0f)
{
}

Novelty::~Novelty() = default;

void Novelty::initialize(class Brain* brain) {
    pImpl->brain = brain;
    NLM_LOG_INFO("Novelty initialized");
}

float Novelty::getLevel() const {
    return level_;
}

void Novelty::setLevel(float level) {
    level_ = level;
}

void Novelty::detectNovelty(const class Observation& observation, 
                           const class Observation& previousObservation) {
    // Simple novelty detection: compute difference between observations
    float diff = 0.0f;
    const auto& curr = observation.getData();
    const auto& prev = previousObservation.getData();
    
    if (curr.size() == prev.size()) {
        for (size_t i = 0; i < curr.size(); ++i) {
            diff += std::abs(curr[i] - prev[i]);
        }
        diff /= curr.size();
    }
    
    // Set novelty level based on difference
    setLevel(diff);
}

void Novelty::detectNovelty(const std::vector<float>& currentPattern,
                           const std::vector<float>& previousPattern) {
    // Simple novelty detection from pattern difference
    float diff = 0.0f;
    size_t minSize = std::min(currentPattern.size(), previousPattern.size());
    
    for (size_t i = 0; i < minSize; ++i) {
        diff += std::abs(currentPattern[i] - previousPattern[i]);
    }
    
    if (minSize > 0) {
        diff /= minSize;
    }
    
    setLevel(diff);
}

void Novelty::update(TimestepDuration dt) {
    // Decay novelty over time
    level_ *= (1.0f - dt * 0.001f);
}

const std::vector<float>& Novelty::getHistory() const {
    return pImpl->history;
}

void Novelty::clearHistory() {
    pImpl->history.clear();
}

// PredictionError Implementation
PredictionError::PredictionError()
    : pImpl(new Impl)
    , error_(0.0f)
{
}

PredictionError::~PredictionError() = default;

void PredictionError::initialize(class Brain* brain) {
    pImpl->brain = brain;
    NLM_LOG_INFO("PredictionError initialized");
}

void PredictionError::update(float prediction, float actual, TimestepDuration dt) {
    // Update prediction error signal
    error_ = actual - prediction;
    
    // Decay over time
    error_ *= (1.0f - dt * 0.001f);
}

float PredictionError::getLevel() const {
    return error_;
}

void PredictionError::setLevel(float level) {
    error_ = level;
}

void PredictionError::reset() {
    error_ = 0.0f;
}

namespace nlm {
