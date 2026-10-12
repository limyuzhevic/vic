#include "Neuromodulator.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <numeric>

namespace nlm {

// ============================================================================
// Dopamine Implementation
// ============================================================================

struct Dopamine::Impl {
    // Core state variables
    float level;                    // Current dopamine concentration (0.0 to 2.0)
    float baseline;                 // Baseline tonic dopamine level
    float phasic;                   // Phasic burst component
    float accumulatedReward;        // Total accumulated reward
    
    // Temporal dynamics
    float tonicDecay;               // Decay rate for tonic component
    float phasicDecay;              // Decay rate for phasic component
    float releaseRate;             // Rate of dopamine release on events
    float reuptakeRate;            // Rate of dopamine reuptake
    
    // Learning parameters
    float learningRate;            // TD learning rate
    float predictionErrorGain;     // Gain for prediction error signaling
    float rewardPredictionGain;    // Gain for reward prediction
    
    // State tracking
    float lastPrediction;          // Previous predicted value
    std::vector<float> rewardHistory;  // Recent rewards
    std::vector<float> errorHistory;   // Recent prediction errors
    
    // Integration with neural systems
    class Brain* brain;            // Reference to brain for plasticity modulation
    
    Impl() 
        : level(0.0f)
        , baseline(0.1f)
        , phasic(0.0f)
        , accumulatedReward(0.0f)
        , tonicDecay(0.05f)
        , phasicDecay(0.5f)
        , releaseRate(2.0f)
        , reuptakeRate(0.1f)
        , learningRate(0.01f)
        , predictionErrorGain(1.0f)
        , rewardPredictionGain(1.0f)
        , lastPrediction(0.0f)
        , brain(nullptr) {}
};

Dopamine::Dopamine() : pImpl(new Impl) {}

Dopamine::~Dopamine() = default;

const char* Dopamine::getName() const {
    return "Dopamine";
}

float Dopamine::getLevel() const {
    return pImpl->level;
}

void Dopamine::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, 2.0f);
}

float Dopamine::getPlasticityFactor() const {
    // Dopamine modulates plasticity: phasic DA enhances LTP, tonic DA regulates overall excitability
    float phasicModulation = pImpl->phasic * 1.5f;  // Phasic bursts enhance plasticity
    float tonicModulation = (pImpl->level - pImpl->baseline) / pImpl->baseline + 1.0f;  // Tonic affects baseline excitability
    
    // Combined effect: phasic DA has stronger impact on learning
    return std::clamp(1.0f + phasicModulation + (tonicModulation - 1.0f) * 0.3f, 0.5f, 3.0f);
}

void Dopamine::update(TimestepDuration dt) {
    // Update both tonic and phasic components
    float dtf = static_cast<float>(dt);
    
    // Tonic decay towards baseline
    pImpl->level = pImpl->baseline + (pImpl->level - pImpl->baseline) * std::exp(-pImpl->tonicDecay * dtf);
    
    // Phasic decay
    pImpl->phasic = pImpl->phasic * std::exp(-pImpl->phasicDecay * dtf);
}

void Dopamine::signalReward(float reward, float predictedValue) {
    // Compute prediction error
    float error = reward - predictedValue;
    
    // Store prediction
    pImpl->lastPrediction = predictedValue;
    
    // Update accumulated reward
    pImpl->accumulatedReward += reward;
    pImpl->rewardHistory.push_back(reward);
    
    // Phasic dopamine burst proportional to reward prediction error
    float phasicBurst = error * pImpl->predictionErrorGain;
    
    // Ensure phasic component doesn't create negative levels
    if (phasicBurst > 0.0f) {
        pImpl->phasic = std::min(phasicBurst, 2.0f);
        pImpl->level = std::min(2.0f, pImpl->level + phasicBurst * pImpl->releaseRate);
    }
    
    // Signal prediction error separately as well
    signalPredictionError(error);
}

void Dopamine::signalPredictionError(float error) {
    // Dopamine encodes prediction errors: positive for better-than-expected outcomes, negative for worse
    pImpl->errorHistory.push_back(error);
    
    if (std::abs(error) > 0.01f) {  // Only signal significant errors
        float errorSignal = error * pImpl->rewardPredictionGain;
        
        if (error > 0.0f) {  // Positive prediction error (reward)
            pImpl->phasic = std::max(pImpl->phasic, errorSignal);
            pImpl->level = std::min(2.0f, pImpl->level + errorSignal * pImpl->releaseRate);
        } else {  // Negative prediction error (punishment)
            // Negative errors suppress dopamine
            float suppression = std::min(0.0f, errorSignal);
            pImpl->phasic = std::min(pImpl->phasic, 0.5f * errorSignal);
            pImpl->level = std::max(pImpl->baseline, pImpl->level + suppression * pImpl->releaseRate * 0.5f);
        }
    }
}

void Dopamine::addReward(float delta) {
    pImpl->accumulatedReward += delta;
    pImpl->level = std::min(2.0f, pImpl->level + delta * pImpl->releaseRate * 0.5f);
    pImpl->rewardHistory.push_back(delta);
}

float Dopamine::getAccumulatedReward() const {
    return pImpl->accumulatedReward;
}

const std::vector<float>& Dopamine::getRewardHistory() const {
    return pImpl->rewardHistory;
}

void Dopamine::setBaseline(float baseline) {
    pImpl->baseline = baseline;
}

void Dopamine::setLearningRate(float rate) {
    pImpl->learningRate = rate;
}

void Dopamine::setPhasicModulation(float gain) {
    pImpl->predictionErrorGain = gain;
}

void Dopamine::setTimeConstant(float tcs, float tcd) {
    pImpl->tonicDecay = tcs;
    pImpl->phasicDecay = tcd;
}

void Dopamine::setReleaseRate(float rate) {
    pImpl->releaseRate = rate;
}

void Dopamine::reset() {
    pImpl->level = pImpl->baseline;
    pImpl->phasic = 0.0f;
    pImpl->accumulatedReward = 0.0f;
    pImpl->lastPrediction = 0.0f;
    pImpl->rewardHistory.clear();
    pImpl->errorHistory.clear();
}

// ============================================================================
// Curiosity Implementation
// ============================================================================

struct Curiosity::Impl {
    class Brain* brain;
    float level;
    float noveltyWeight;
    float predictionErrorWeight;
    float decayRate;
    float explorationCost;
    float learningRate;
    float statisticalPrecision;
    
    std::vector<float> stateHistory;
    std::vector<float> noveltyHistory;
    std::vector<float> predictionErrorHistory;
    std::vector<float> statisticalModel;
    
    Impl() 
        : brain(nullptr)
        , level(0.0f)
        , noveltyWeight(0.5f)
        , predictionErrorWeight(0.5f)
        , decayRate(0.05f)
        , explorationCost(0.1f)
        , learningRate(0.01f)
        , statisticalPrecision(1.0f)
    {}
};

Curiosity::Curiosity() : pImpl(new Impl) {}

Curiosity::~Curiosity() = default;

void Curiosity::initialize(Brain* brain) {
    pImpl->brain = brain;
    pImpl->statisticalModel.resize(100, 0.0f);  // Initialize statistical model
    NLM_LOG_INFO("Curiosity system initialized");
}

float Curiosity::getLevel() const {
    return pImpl->level;
}

void Curiosity::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, 1.0f);
}

float Curiosity::getPlasticityFactor() const {
    // Curiosity enhances exploratory plasticity and learning
    float noveltyModulation = pImpl->noveltyWeight * 0.5f * getLevel();
    float predictionErrorModulation = pImpl->predictionErrorWeight * 0.3f * getLevel();
    
    // Curiosity increases overall plasticity for exploration
    return std::clamp(1.0f + noveltyModulation + predictionErrorModulation, 1.0f, 3.0f);
}

void Curiosity::update(TimestepDuration dt) {
    float dtf = static_cast<float>(dt);
    
    // Smooth curiosity dynamics
    float target = pImpl->noveltyWeight * pImpl->noveltyHistory.back() + 
                   pImpl->predictionErrorWeight * pImpl->predictionErrorHistory.back();
    
    pImpl->level += (target - pImpl->level) * 0.1f;
    
    // Decay
    pImpl->level = std::max(0.0f, pImpl->level - pImpl->decayRate * dtf);
}

void Curiosity::computeCuriosity(float novelty, float predictionError) {
    pImpl->noveltyHistory.push_back(novelty);
    pImpl->predictionErrorHistory.push_back(predictionError);
    
    // Compute curiosity as weighted combination
    float target = pImpl->noveltyWeight * novelty + pImpl->predictionErrorWeight * predictionError;
    pImpl->level = std::min(1.0f, target);
}

void Curiosity::onNoveltyDetected(const std::vector<float>& pattern) {
    float novelty = computeStatisticalNovelty(pattern);
    computeCuriosity(novelty, pImpl->predictionErrorHistory.empty() ? 0.0f : pImpl->predictionErrorHistory.back());
}

void Curiosity::onPredictionErrorComputed(float error) {
    computeCuriosity(pImpl->noveltyHistory.empty() ? 0.0f : pImpl->noveltyHistory.back(), error);
}

float Curiosity::getExplorationProbability() const {
    // Exploration probability increases with curiosity level, weighted by cost
    return getLevel() * (1.0f - pImpl->explorationCost);
}

void Curiosity::setExplorationBias(float bias) {
    pImpl->explorationCost = std::clamp(bias, 0.0f, 1.0f);
}

void Curiosity::updateStatisticalModel(const std::vector<float>& state) {
    pImpl->stateHistory.push_back(state.size() > 0 ? state[0] : 0.0f);
    
    if (pImpl->stateHistory.size() > static_cast<size_t>(100)) {
        pImpl->stateHistory.erase(pImpl->stateHistory.begin());
    }
    
    // Update statistical model using simple moving average
    for (size_t i = 0; i < std::min(pImpl->stateHistory.size(), pImpl->statisticalModel.size()); ++i) {
        pImpl->statisticalModel[i] = pImpl->statisticalModel[i] * (1.0f - pImpl->learningRate) + 
                                    pImpl->stateHistory[i] * pImpl->learningRate;
    }
}

void Curiosity::resetStatisticalModel() {
    pImpl->statisticalModel.assign(pImpl->statisticalModel.size(), 0.0f);
    pImpl->stateHistory.clear();
}

void Curiosity::setLearningRate(float rate) {
    pImpl->learningRate = rate;
}

void Curiosity::setExplorationCost(float cost) {
    pImpl->explorationCost = cost;
}

const std::vector<float>& Curiosity::getStateHistory() const {
    return pImpl->stateHistory;
}

float Curiosity::getModelPredictionError() const {
    if (pImpl->stateHistory.empty() || pImpl->statisticalModel.empty()) {
        return 0.0f;
    }
    
    float prediction = pImpl->statisticalModel[0];
    return std::abs(prediction - pImpl->stateHistory.back());
}

// ============================================================================
// PredictionError Implementation
// ============================================================================

struct PredictionError::Impl {
    class Brain* brain;
    float error;
    float predictedValue;
    float actualValue;
    std::vector<float> history;
    float learningRate;
    float discountFactor;
    float temporalHorizon;
    float valuePredictionAccuracy;
    
    Impl() : brain(nullptr), error(0.0f), predictedValue(0.0f), actualValue(0.0f),
             learningRate(0.01f), discountFactor(0.95f), temporalHorizon(10.0f),
             valuePredictionAccuracy(1.0f) {}
};

PredictionError::PredictionError() : pImpl(new Impl) {}

PredictionError::~PredictionError() = default;

void PredictionError::initialize(Brain* brain) {
    pImpl->brain = brain;
    NLM_LOG_INFO("PredictionError system initialized");
}

float PredictionError::getLevel() const {
    return std::abs(pImpl->error);
}

void PredictionError::setLevel(float level) {
    pImpl->error = level;
}

float PredictionError::getPlasticityFactor() const {
    // Prediction error modulates reward prediction learning and plasticity
    return std::clamp(1.0f + std::abs(pImpl->error) * 2.0f, 1.0f, 5.0f);
}

void PredictionError::update(TimestepDuration dt) {
    // Prediction error decays over time
    float dtf = static_cast<float>(dt);
    pImpl->error = pImpl->error * std::exp(-0.1f * dtf);
}

void PredictionError::computeTDError(float currentValue, float targetValue, float discount) {
    // Temporal difference learning
    float temporalDiscount = discount > 0.0f ? discount : pImpl->discountFactor;
    float temporalTarget = currentValue + temporalDiscount * (targetValue - currentValue);
    
    float error = targetValue - temporalTarget;
    
    pImpl->actualValue = targetValue;
    pImpl->predictedValue = temporalTarget;
    pImpl->error = error;
    
    // Update value prediction accuracy
    if (std::abs(targetValue) > 0.01f) {
        pImpl->valuePredictionAccuracy = 1.0f - std::abs(error) / std::abs(targetValue);
    }
    
    pImpl->history.push_back(error);
    
    if (pImpl->history.size() > 1000) {
        pImpl->history.erase(pImpl->history.begin());
    }
}

void PredictionError::updatePrediction(float newPrediction) {
    pImpl->predictedValue = newPrediction;
}

void PredictionError::onStateTransition(const std::vector<float>& state, float value) {
    computeTDError(pImpl->predictedValue, value, pImpl->discountFactor);
}

float PredictionError::getErrorMagnitude() const {
    return std::abs(pImpl->error);
}

float PredictionError::getSignedError() const {
    return pImpl->error;
}

float PredictionError::getPredictionAccuracy() const {
    return pImpl->valuePredictionAccuracy;
}

const std::vector<float>& PredictionError::getErrorHistory() const {
    return pImpl->history;
}

void PredictionError::setLearningRate(float rate) {
    pImpl->learningRate = rate;
}

void PredictionError::setDiscountFactor(float discount) {
    pImpl->discountFactor = discount;
}

void PredictionError::setTemporalHorizon(float horizon) {
    pImpl->temporalHorizon = horizon;
}

void PredictionError::reset() {
    pImpl->error = 0.0f;
    pImpl->predictedValue = 0.0f;
    pImpl->actualValue = 0.0f;
    pImpl->history.clear();
    pImpl->valuePredictionAccuracy = 1.0f;
}

void PredictionError::clearHistory() {
    pImpl->history.clear();
}

// ============================================================================
// Novelty Implementation
// ============================================================================

struct Novelty::Impl {
    class Brain* brain;
    float level;
    float decayRate;
    float noveltyThreshold;
    float sensitivity;
    float statisticalVariance;
    float adaptationRate;
    std::vector<float> history;
    std::vector<float> patternHistory;
    std::vector<float> statisticalMean;
    std::vector<float> statisticalStdDev;
    
    Impl() : brain(nullptr), level(0.0f), decayRate(0.1f), noveltyThreshold(0.3f),
             sensitivity(1.0f), statisticalVariance(1.0f), adaptationRate(0.01f) {}
};

Novelty::Novelty() : pImpl(new Impl) {}

Novelty::~Novelty() = default;

void Novelty::initialize(Brain* brain) {
    pImpl->brain = brain;
    pImpl->statisticalMean.resize(50, 0.0f);
    pImpl->statisticalStdDev.resize(50, 1.0f);
    NLM_LOG_INFO("Novelty detection system initialized");
}

float Novelty::getLevel() const {
    return pImpl->level;
}

void Novelty::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, 1.0f);
}

float Novelty::getPlasticityFactor() const {
    // Novelty modulates learning and plasticity for new information
    return std::clamp(1.0f + pImpl->level * 1.5f, 1.0f, 4.0f);
}

void Novelty::update(TimestepDuration dt) {
    // Decay novelty over time
    float dtf = static_cast<float>(dt);
    pImpl->level = std::max(0.0f, pImpl->level - pImpl->decayRate * dtf);
}

void Novelty::detectNovelty(const std::vector<float>& observation) {
    updateStatistics(observation);
    float novelty = computeStatisticalNovelty(observation);
    
    pImpl->level = novelty;
    pImpl->history.push_back(novelty);
    pImpl->patternHistory.push_back(observation);
    
    if (pImpl->patternHistory.size() > 1000) {
        pImpl->patternHistory.erase(pImpl->patternHistory.begin());
    }
}

void Novelty::detectNovelty(const Observation& obs) {
    std::vector<float> features;
    // Simplified feature extraction from observation
    features.push_back(obs.getSensoryInput().getValue());
    
    detectNovelty(features);
}

void Novelty::updateStatistics(const std::vector<float>& pattern) {
    size_t patternSize = std::min(pattern.size(), pImpl->statisticalMean.size());
    
    if (patternSize == 0) return;
    
    // Update mean using running average
    float oldMean = pImpl->statisticalMean[0];
    float newValue = pattern[0];
    pImpl->statisticalMean[0] = oldMean * (1.0f - pImpl->adaptationRate) + newValue * pImpl->adaptationRate;
    
    // Update variance
    pImpl->statisticalVariance = (pImpl->statisticalVariance * (1.0f - pImpl->adaptationRate) +
                                   std::abs(newValue - pImpl->statisticalMean[0]) * pImpl->adaptationRate);
}

float Novelty::computeStatisticalNovelty(const std::vector<float>& pattern) const {
    if (pImpl->statisticalMean.empty() || pImpl->statisticalMean[0] == 0.0f) {
        return 0.0f;
    }
    
    float mean = pImpl->statisticalMean[0];
    float stdDev = std::max(0.01f, pImpl->statisticalVariance);
    
    float deviation = std::abs(pattern[0] - mean);
    
    // Novelty is standardized deviation from mean
    float novelty = deviation / stdDev;
    
    // Apply sensitivity factor and threshold
    novelty = std::min(1.0f, novelty * pImpl->sensitivity / pImpl->noveltyThreshold);
    
    return novelty;
}

void Novelty::adaptThreshold(float noveltyRate) {
    // Adapt novelty threshold based on recent novelty rate
    pImpl->noveltyThreshold = std::clamp(pImpl->noveltyThreshold * (1.0f + noveltyRate * 0.1f - 0.05f), 0.1f, 2.0f);
}

void Novelty::setSensitivity(float sensitivity) {
    pImpl->sensitivity = sensitivity;
}

const std::vector<float>& Novelty::getNoveltyHistory() const {
    return pImpl->history;
}

const std::vector<std::vector<float>>& Novelty::getPatternHistory() const {
    return pImpl->patternHistory;
}

float Novelty::getNoveltyRate() const {
    if (pImpl->history.empty()) return 0.0f;
    
    float total = std::accumulate(pImpl->history.begin(), pImpl->history.end(), 0.0f);
    return total / pImpl->history.size();
}

float Novelty::getRecentNoveltyRate(float timeWindow) const {
    if (pImpl->history.empty()) return 0.0f;
    
    size_t startIdx = std::max(0UL, pImpl->history.size() - static_cast<size_t>(timeWindow * 10.0f));
    size_t count = pImpl->history.size() - startIdx;
    
    if (count == 0) return 0.0f;
    
    float total = std::accumulate(pImpl->history.begin() + startIdx, pImpl->history.end(), 0.0f);
    return total / count;
}

void Novelty::reset() {
    pImpl->level = 0.0f;
    pImpl->history.clear();
    pImpl->patternHistory.clear();
}

void Novelty::clearHistory() {
    pImpl->history.clear();
}

std::vector<float> Novelty::extractFeatures(const std::vector<float>& pattern) const {
    std::vector<float> features;
    
    // Simple feature extraction: statistics of the pattern
    if (!pattern.empty()) {
        features.push_back(pattern[0]);  // First element
        
        float mean = std::accumulate(pattern.begin(), pattern.end(), 0.0f) / pattern.size();
        features.push_back(mean);  // Mean
        
        float variance = 0.0f;
        for (float val : pattern) {
            variance += (val - mean) * (val - mean);
        }
        variance /= pattern.size();
        features.push_back(std::sqrt(variance));  // Standard deviation
    }
    
    return features;
}

float Novelty::computePatternDistance(const std::vector<float>& a, const std::vector<float>& b) const {
    if (a.empty() || b.empty()) return 0.0f;
    
    size_t compareLen = std::min(a.size(), b.size());
    float totalDiff = 0.0f;
    
    for (size_t i = 0; i < compareLen; ++i) {
        totalDiff += std::abs(a[i] - b[i]);
    }
    
    return totalDiff / compareLen;
}

} // namespace nlm
