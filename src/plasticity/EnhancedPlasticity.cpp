// Enhanced Plasticity Rules Implementation
// 
// This file provides implementations for the advanced plasticity rules
// including Triplet STDP, BCM, Oja, and Reward-Modulated STDP.

#include "plasticity/EnhancedPlasticity.hpp"
#include "brain/Synapse.hpp"
#include "core/Logger/Logger.hpp"
#include <cmath>
#include <algorithm>
#include <random>

namespace nlm {

// === Triplet STDP Implementation ===

class TripletSTDP::Impl {
public:
    float ltpWeight;
    float ltdWeight;
    float timeConstant;
    float tripletLTP;
    float tripletLTD;
    float tripletTimeConstant;
    float ltpBaseline;
    float ltdBaseline;
    
    Impl() : ltpWeight(0.01f), ltdWeight(0.012f), timeConstant(20.0f),
             tripletLTP(0.005f), tripletLTD(0.006f), tripletTimeConstant(50.0f),
             ltpBaseline(0.001f), ltdBaseline(0.001f) {}
};

TripletSTDP::TripletSTDP() : pImpl(new Impl) {}

TripletSTDP::~TripletSTDP() = default;

const char* TripletSTDP::getName() const {
    return "TripletSTDP";
}

void TripletSTDP::setLTPWeight(float weight) {
    pImpl->ltpWeight = std::clamp(weight, 0.0f, 1.0f);
}

float TripletSTDP::getLTPWeight() const {
    return pImpl->ltpWeight;
}

void TripletSTDP::setLTDWeight(float weight) {
    pImpl->ltdWeight = std::clamp(weight, 0.0f, 1.0f);
}

float TripletSTDP::getLTDWeight() const {
    return pImpl->ltdWeight;
}

void TripletSTDP::setTimeConstant(float tau) {
    pImpl->timeConstant = std::clamp(tau, 1.0f, 100.0f);
}

float TripletSTDP::getTimeConstant() const {
    return pImpl->timeConstant;
}

void TripletSTDP::setTripletLTP(float weight) {
    pImpl->tripletLTP = std::clamp(weight, 0.0f, 0.1f);
}

float TripletSTDP::getTripletLTP() const {
    return pImpl->tripletLTP;
}

void TripletSTDP::setTripletLTD(float weight) {
    pImpl->tripletLTD = std::clamp(weight, 0.0f, 0.1f);
}

float TripletSTDP::getTripletLTD() const {
    return pImpl->tripletLTD;
}

void TripletSTDP::configure(float ltp, float ltd, float tau, float tripletLtp, float tripletLtd) {
    setLTPWeight(ltp);
    setLTDWeight(ltd);
    setTimeConstant(tau);
    setTripletLTP(tripletLtp);
    setTripletLTD(tripletLtd);
}

void TripletSTDP::update(Synapse* synapse,
                        const std::vector<Timestamp>& preSpikes,
                        const std::vector<Timestamp>& postSpikes,
                        TimestepDuration dt) {
    if (!synapse || preSpikes.empty() || postSpikes.empty()) {
        return;
    }
    
    // Calculate pairwise STDP contributions
    float pairwiseDelta = calculatePairwiseSTDP(preSpikes, postSpikes);
    
    // Calculate triplet contributions
    float tripletDelta = calculateTripletSTDP(synapse, preSpikes, postSpikes, dt);
    
    // Combine contributions
    float totalDelta = pairwiseDelta + tripletDelta;
    
    // Apply with eligibility trace
    if (std::abs(totalDelta) > 1e-6f) {
        totalDelta = computeSafeDelta(totalDelta);
        recordLearningEvent(totalDelta, static_cast<float>(preSpikes.back()));
        
        // Update eligibility trace
        updateEligibilityTrace(synapse, totalDelta);
        
        // Apply weight change
        applyWeightChange(synapse, totalDelta);
    }
}

void TripletSTDP::applyWeightChange(Synapse* synapse, SynapticWeight delta) {
    if (!synapse) return;
    
    float newWeight = synapse->getWeight() + delta;
    newWeight = clampWeight(newWeight);
    synapse->setWeight(newWeight);
}

float TripletSTDP::calculatePairwiseSTDP(const std::vector<Timestamp>& preSpikes,
                                        const std::vector<Timestamp>& postSpikes) {
    float totalDelta = 0.0f;
    float tau = pImpl->timeConstant;
    
    for (Timestamp preTime : preSpikes) {
        for (Timestamp postTime : postSpikes) {
            float dt = static_cast<float>(postTime - preTime);
            
            if (dt > 0) {
                // Pre before post: LTP
                float delta = pImpl->ltpWeight * std::exp(-dt / tau) - pImpl->ltpBaseline;
                totalDelta += delta;
            } else if (dt < 0) {
                // Post before pre: LTD
                float delta = -pImpl->ltdWeight * std::exp(dt / tau) - pImpl->ltdBaseline;
                totalDelta += delta;
            }
        }
    }
    
    return totalDelta;
}

float TripletSTDP::calculateTripletSTDP(Synapse* synapse,
                                      const std::vector<Timestamp>& preSpikes,
                                      const std::vector<Timestamp>& postSpikes,
                                      TimestepDuration dt) {
    if (preSpikes.size() < 2 || postSpikes.size() < 2) {
        return 0.0f;
    }
    
    // Calculate recent spike patterns for triplet interactions
    size_t recentPreCount = std::min<size_t>(5, preSpikes.size());
    size_t recentPostCount = std::min<size_t>(5, postSpikes.size());
    
    float prePrePostDelta = 0.0f;
    float postPostPreDelta = 0.0f;
    
    // Analyze pre-pre-post patterns (potentiation)
    for (size_t i = 0; i < recentPreCount - 1; ++i) {
        for (size_t j = 0; j < recentPostCount; ++j) {
            float timeDiff1 = static_cast<float>(preSpikes[i] - preSpikes[i + 1]);
            float timeDiff2 = static_cast<float>(postSpikes[j] - preSpikes[i + 1]);
            
            if (timeDiff2 > 0 && timeDiff1 > 0) {
                // Pre-pre-post triplet: strong potentiation
                prePrePostDelta += pImpl->tripletLTP * std::exp(-timeDiff1 / pImpl->tripletTimeConstant_) *
                                 std::exp(-timeDiff2 / pImpl->timeConstant_);
            }
        }
    }
    
    // Analyze post-post-pre patterns (depression)
    for (size_t i = 0; i < recentPostCount - 1; ++i) {
        for (size_t j = 0; j < recentPreCount; ++j) {
            float timeDiff1 = static_cast<float>(postSpikes[i] - postSpikes[i + 1]);
            float timeDiff2 = static_cast<float>(preSpikes[j] - postSpikes[i + 1]);
            
            if (timeDiff2 < 0 && timeDiff1 > 0) {
                // Post-post-pre triplet: strong depression
                postPostPreDelta += pImpl->tripletLTD * std::exp(-timeDiff1 / pImpl->tripletTimeConstant_) *
                                  std::exp(timeDiff2 / pImpl->timeConstant_);
            }
        }
    }
    
    return prePrePostDelta + postPostPreDelta;
}

// === BCM (Bienenstock-Cooper-Munro) Implementation ===

class BCMRule::Impl {
public:
    float postMeanActivity;
    float smrWidth;
    float thresholdLearningRate;
    float stabilizationFactor;
    
    // Activity history for threshold calculation
    std::vector<float> preActivityHistory;
    std::vector<float> postActivityHistory;
    size_t historyIndex;
    
    Impl() : postMeanActivity(0.5f), smrWidth(2.0f), 
             thresholdLearningRate(0.01f), stabilizationFactor(0.95f),
             historyIndex(0) {
        preActivityHistory.resize(100, 0.0f);
        postActivityHistory.resize(100, 0.0f);
    }
};

BCMRule::BCMRule() : pImpl(new Impl) {}

BCMRule::~BCMRule() = default;

const char* BCMRule::getName() const {
    return "BCM";
}

void BCMRule::setPostMeanActivity(float activity) {
    pImpl->postMeanActivity = std::clamp(activity, 0.0f, 2.0f);
}

float BCMRule::getPostMeanActivity() const {
    return pImpl->postMeanActivity;
}

void BCMRule::setSMRWidth(float width) {
    pImpl->smrWidth = std::max(0.5f, width);
}

float BCMRule::getSMRWidth() const {
    return pImpl->smrWidth;
}

void BCMRule::setThresholdLearningRate(float rate) {
    pImpl->thresholdLearningRate = std::clamp(rate, 0.0f, 0.1f);
}

float BCMRule::getThresholdLearningRate() const {
    return pImpl->thresholdLearningRate;
}

void BCMRule::configure(float postMean, float smrWidth, float thresholdRate) {
    setPostMeanActivity(postMean);
    setSMRWidth(smrWidth);
    setThresholdLearningRate(thresholdRate);
}

void BCMRule::update(Synapse* synapse,
                    const std::vector<Timestamp>& preSpikes,
                    const std::vector<Timestamp>& postSpikes,
                    TimestepDuration dt) {
    if (!synapse || preSpikes.empty() || postSpikes.empty()) {
        return;
    }
    
    // Calculate current activities
    float preActivity = calculateActivityFromSpikes(preSpikes, dt);
    float postActivity = calculateActivityFromSpikes(postSpikes, dt);
    
    // Update activity histories
    updateActivityHistory(preActivity, postActivity);
    
    // Calculate BCM threshold (sliding threshold)
    float threshold = calculateBCMThreshold();
    
    // Compute BCM learning rule
    float delta = computeBCMLearning(preActivity, postActivity, threshold);
    
    // Apply weight change
    if (std::abs(delta) > 1e-6f) {
        delta = computeSafeDelta(delta);
        recordLearningEvent(delta, static_cast<float>(preSpikes.back()));
        applyWeightChange(synapse, delta);
    }
}

void BCMRule::applyWeightChange(Synapse* synapse, SynapticWeight delta) {
    if (!synapse) return;
    
    float newWeight = synapse->getWeight() + delta;
    newWeight = clampWeight(newWeight);
    synapse->setWeight(newWeight);
}

float BCMRule::calculateActivityFromSpikes(const std::vector<Timestamp>& spikes, TimestepDuration dt) {
    if (spikes.empty()) return 0.0f;
    
    float timeSpan = static_cast<float>(spikes.back() - spikes.front());
    if (timeSpan <= 0.0f) return 1.0f;
    
    float activity = static_cast<float>(spikes.size()) / (timeSpan * 0.001f + 1.0f);
    return std::min(2.0f, std::max(0.0f, activity));
}

void BCMRule::updateActivityHistory(float preActivity, float postActivity) {
    pImpl->preActivityHistory[pImpl->historyIndex_] = preActivity;
    pImpl->postActivityHistory[pImpl->historyIndex_] = postActivity;
    
    pImpl->historyIndex_ = (pImpl->historyIndex_ + 1) % pImpl->preActivityHistory.size();
}

float BCMRule::calculateBCMThreshold() {
    // Sliding threshold based on post-synaptic activity history
    float meanPostActivity = getMeanPostActivity();
    
    // Threshold follows mean activity with some inertia
    float threshold = meanPostActivity * pImpl->smrWidth;
    
    // Apply stabilization
    threshold = threshold * pImpl->stabilizationFactor + 
                pImpl->postMeanActivity * (1.0f - pImpl->stabilizationFactor);
    
    // Constrain to reasonable range
    return std::clamp(threshold, 0.1f, 3.0f);
}

float BCMRule::computeBCMLearning(float preActivity, float postActivity, float threshold) {
    // BCM learning rule: Δw = η * (x - θ(y)) * (y - θ(y))
    float error = preActivity - threshold;
    float modulation = postActivity - threshold;
    
    float delta = getParameters().learningRate * error * modulation;
    
    // Normalize by SMR width
    delta /= pImpl->smrWidth;
    
    // Apply non-linear saturation
    delta = std::tanh(delta / 10.0f) * getParameters().learningRate;
    
    return delta;
}

float BCMRule::getCurrentThreshold() const {
    return calculateBCMThreshold();
}

float BCMRule::getMeanPreActivity() const {
    float sum = 0.0f;
    for (float activity : pImpl->preActivityHistory) {
        sum += activity;
    }
    return sum / pImpl->preActivityHistory.size();
}

float BCMRule::getMeanPostActivity() const {
    float sum = 0.0f;
    for (float activity : pImpl->postActivityHistory) {
        sum += activity;
    }
    return sum / pImpl->postActivityHistory.size();
}

// === Oja Learning Rule Implementation ===

class OjaRule::Impl {
public:
    float normalizationFactor;
    float maxWeightMagnitude;
    float activitySmoothing;
    
    // Activity tracking
    std::vector<float> postActivityHistory;
    size_t historyIndex;
    
    Impl() : normalizationFactor(1.0f), maxWeightMagnitude(1.0f),
             activitySmoothing(0.1f), historyIndex(0) {
        postActivityHistory.resize(50, 0.0f);
    }
};

OjaRule::OjaRule() : pImpl(new Impl) {}

OjaRule::~OjaRule() = default;

const char* OjaRule::getName() const {
    return "Oja";
}

void OjaRule::setMaxWeightMagnitude(float max) {
    pImpl->maxWeightMagnitude = std::max(0.5f, max);
}

float OjaRule::getMaxWeightMagnitude() const {
    return pImpl->maxWeightMagnitude;
}

void OjaRule::setActivitySmoothing(float smoothing) {
    pImpl->activitySmoothing = std::clamp(smoothing, 0.01f, 0.5f);
}

float OjaRule::getActivitySmoothing() const {
    return pImpl->activitySmoothing;
}

void OjaRule::configure(float maxWeight, float smoothing) {
    setMaxWeightMagnitude(maxWeight);
    setActivitySmoothing(smoothing);
}

void OjaRule::update(Synapse* synapse,
                    const std::vector<Timestamp>& preSpikes,
                    const std::vector<Timestamp>& postSpikes,
                    TimestepDuration dt) {
    if (!synapse || preSpikes.empty() || postSpikes.empty()) {
        return;
    }
    
    // Calculate current activities
    float preActivity = calculatePreActivity(preSpikes, dt);
    float postActivity = calculatePostActivity(postSpikes, dt);
    
    // Update activity history
    updateActivityHistory(postActivity);
    
    // Calculate Oja learning rule
    float delta = computeOjaLearning(preActivity, postActivity, synapse);
    
    // Apply weight change
    if (std::abs(delta) > 1e-6f) {
        delta = computeSafeDelta(delta);
        recordLearningEvent(delta, static_cast<float>(preSpikes.back()));
        applyWeightChange(synapse, delta);
    }
}

void OjaRule::applyWeightChange(Synapse* synapse, SynapticWeight delta) {
    if (!synapse) return;
    
    float newWeight = synapse->getWeight() + delta;
    
    // Apply Oja normalization
    newWeight = normalizeOjaWeight(synapse, newWeight);
    
    // Apply standard clamping
    newWeight = clampWeight(newWeight);
    synapse->setWeight(newWeight);
}

float OjaRule::calculatePreActivity(const std::vector<Timestamp>& spikes, TimestepDuration dt) {
    if (spikes.empty()) return 0.0f;
    
    float timeSpan = static_cast<float>(spikes.back() - spikes.front());
    if (timeSpan <= 0.0f) return 1.0f;
    
    float activity = static_cast<float>(spikes.size()) / (timeSpan * 0.001f + 1.0f);
    return std::min(2.0f, std::max(0.0f, activity));
}

float OjaRule::calculatePostActivity(const std::vector<Timestamp>& spikes, TimestepDuration dt) {
    if (spikes.empty()) return 0.0f;
    
    float timeSpan = static_cast<float>(spikes.back() - spikes.front());
    if (timeSpan <= 0.0f) return 1.0f;
    
    float activity = static_cast<float>(spikes.size()) / (timeSpan * 0.001f + 1.0f);
    return std::min(2.0f, std::max(0.0f, activity));
}

void OjaRule::updateActivityHistory(float postActivity) {
    pImpl->postActivityHistory[pImpl->historyIndex_] = postActivity;
    pImpl->historyIndex_ = (pImpl->historyIndex_ + 1) % pImpl->postActivityHistory.size();
    
    // Update normalization factor based on average activity
    float meanActivity = getMeanPostActivity();
    pImpl->normalizationFactor = pImpl->activitySmoothing * meanActivity +
                                (1.0f - pImpl->activitySmoothing) * pImpl->normalizationFactor;
    pImpl->normalizationFactor = std::clamp(pImpl->normalizationFactor, 0.1f, 10.0f);
}

float OjaRule::computeOjaLearning(float preActivity, float postActivity, Synapse* synapse) {
    // Oja learning rule: Δw = η * y * (x - y * w)
    float currentWeight = synapse->getWeight();
    
    float delta = getParameters().learningRate * postActivity * (preActivity - postActivity * currentWeight);
    
    // Apply normalization factor
    delta /= pImpl->normalizationFactor;
    
    // Apply non-linear saturation
    delta = std::tanh(delta / 5.0f) * getParameters().learningRate;
    
    return delta;
}

float OjaRule::normalizeOjaWeight(Synapse* synapse, float newWeight) {
    float currentWeight = synapse->getWeight();
    
    // Oja normalization: w_new = w / sqrt(w^2 + η * y^2 * dt)
    float denominator = std::sqrt(currentWeight * currentWeight +
                                  getParameters().learningRate * 
                                  pImpl->postActivityHistory[0] * 
                                  pImpl->postActivityHistory[0] *
                                  static_cast<float>(synapse->getLastSpikeTime() - 1.0f));
    
    if (denominator > 0.0f) {
        newWeight = currentWeight / denominator;
    }
    
    // Constrain to maximum magnitude
    float magnitude = std::sqrt(newWeight * newWeight);
    if (magnitude > pImpl->maxWeightMagnitude) {
        newWeight = newWeight / magnitude * pImpl->maxWeightMagnitude;
    }
    
    return newWeight;
}

float OjaRule::getMeanPostActivity() const {
    float sum = 0.0f;
    for (float activity : pImpl->postActivityHistory) {
        sum += activity;
    }
    return sum / pImpl->postActivityHistory.size();
}

// === Reward-Modulated STDP Implementation ===

class RewardModulatedSTDP::Impl {
public:
    float rewardWeight;
    float predictionWeight;
    float eligibilityDecay;
    float maxEligibilityTrace;
    float baselinePlasticity;
    
    // Reward history tracking
    std::vector<float> rewardHistory;
    size_t historyIndex;
    
    Impl() : rewardWeight(0.5f), predictionWeight(0.3f), eligibilityDecay(0.95f),
             maxEligibilityTrace(10.0f), baselinePlasticity(1.0f), historyIndex(0) {
        rewardHistory.resize(100, 0.0f);
    }
};

RewardModulatedSTDP::RewardModulatedSTDP() : pImpl(new Impl) {}

RewardModulatedSTDP::~RewardModulatedSTDP() = default;

const char* RewardModulatedSTDP::getName() const {
    return "RewardModulatedSTDP";
}

void RewardModulatedSTDP::setRewardWeight(float weight) {
    pImpl->rewardWeight = std::clamp(weight, 0.0f, 1.0f);
}

float RewardModulatedSTDP::getRewardWeight() const {
    return pImpl->rewardWeight;
}

void RewardModulatedSTDP::setPredictionWeight(float weight) {
    pImpl->predictionWeight = std::clamp(weight, 0.0f, 1.0f);
}

float RewardModulatedSTDP::getPredictionWeight() const {
    return pImpl->predictionWeight;
}

void RewardModulatedSTDP::setEligibilityDecay(float decay) {
    pImpl->eligibilityDecay = std::clamp(decay, 0.5f, 1.0f);
}

float RewardModulatedSTDP::getEligibilityDecay() const {
    return pImpl->eligibilityDecay;
}

void RewardModulatedSTDP::setBaselinePlasticity(float baseline) {
    pImpl->baselinePlasticity = std::clamp(baseline, 0.5f, 2.0f);
}

float RewardModulatedSTDP::getBaselinePlasticity() const {
    return pImpl->baselinePlasticity;
}

void RewardModulatedSTDP::applyRewardModulation(float reward, float predictionError) {
    // Update reward history
    pImpl->rewardHistory[pImpl->historyIndex_] = reward;
    pImpl->historyIndex_ = (pImpl->historyIndex_ + 1) % pImpl->rewardHistory.size();
    
    // Calculate reward-modulated plasticity factor
    float averageReward = calculateAverageReward();
    float rewardModulation = 1.0f + (averageReward - pImpl->baselinePlasticity) * pImpl->rewardWeight;
    
    // Calculate prediction error modulation
    float predictionModulation = 1.0f + predictionError * pImpl->predictionWeight;
    
    // Combine modulations
    float plasticityFactor = rewardModulation * predictionModulation;
    plasticityFactor = std::clamp(plasticityFactor, 0.5f, 2.0f);
    
    // Apply to STDP weights
    float currentLTP = getLTPWeight();
    float currentLTD = getLTDWeight();
    
    setLTPWeight(currentLTP * plasticityFactor);
    setLTDWeight(currentLTD * plasticityFactor);
    
    NLM_LOG_INFO("R-STDP modulation: reward=" + std::to_string(reward) +
                " pred_err=" + std::to_string(predictionError) +
                " factor=" + std::to_string(plasticityFactor));
}

void RewardModulatedSTDP::update(Synapse* synapse,
                               const std::vector<Timestamp>& preSpikes,
                               const std::vector<Timestamp>& postSpikes,
                               TimestepDuration dt) {
    // Update eligibility trace with decay
    if (synapse) {
        float currentTrace = synapse->getEligibilityTrace();
        currentTrace *= pImpl->eligibilityDecay;
        synapse->setEligibilityTrace(currentTrace);
    }
    
    // Apply reward-modulated STDP
    EnhancedSTDP::update(synapse, preSpikes, postSpikes, dt);
}

float RewardModulatedSTDP::calculateAverageReward() const {
    float sum = 0.0f;
    for (float reward : pImpl->rewardHistory) {
        sum += reward;
    }
    return sum / pImpl->rewardHistory.size();
}

// === Factory Function Implementations ===

std::unique_ptr<PlasticityRule> createEnhancedSTDP(
    float learningRate, float minWeight, float maxWeight, float timeConstant) {
    
    auto rule = std::make_unique<EnhancedSTDP>();
    rule->configure(learningRate, minWeight, maxWeight, timeConstant);
    return rule;
}

std::unique_ptr<PlasticityRule> createEnhancedHebbian(
    float learningRate, float correlationWindow) {
    
    auto rule = std::make_unique<EnhancedHebbian>();
    PlasticityParameters p;
    p.learningRate = learningRate;
    p.timeConstant = correlationWindow / 5.0f;
    rule->setParameters(p);
    return rule;
}

std::unique_ptr<PlasticityRule> createTripletSTDP(
    float ltpWeight, float ltdWeight, float timeConstant,
    float tripletLTP, float tripletLTD) {
    
    auto rule = std::make_unique<TripletSTDP>();
    rule->configure(ltpWeight, ltdWeight, timeConstant, tripletLTP, tripletLTD);
    return rule;
}

std::unique_ptr<PlasticityRule> createBCMRule(
    float postMeanActivity, float smrWidth, float thresholdLearningRate) {
    
    auto rule = std::make_unique<BCMRule>();
    rule->configure(postMeanActivity, smrWidth, thresholdLearningRate);
    return rule;
}

std::unique_ptr<PlasticityRule> createOjaRule(
    float maxWeightMagnitude, float activitySmoothing) {
    
    auto rule = std::make_unique<OjaRule>();
    rule->configure(maxWeightMagnitude, activitySmoothing);
    return rule;
}

std::unique_ptr<PlasticityRule> createRewardModulatedSTDP(
    float ltpWeight, float ltdWeight, float timeConstant, float rewardWeight) {
    
    auto rule = std::make_unique<RewardModulatedSTDP>();
    rule->configure(ltpWeight, ltdWeight, timeConstant);
    static_cast<RewardModulatedSTDP*>(rule.get())->setRewardWeight(rewardWeight);
    return rule;
}

} // namespace nlm