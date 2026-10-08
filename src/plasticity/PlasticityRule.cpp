#include "PlasticityRule.hpp"

namespace nlm {

bool PlasticityRule::isEnabled() const {
    return enabled_;
}

void PlasticityRule::setEnabled(bool enabled) {
    enabled_ = enabled;
}

struct Hebbian::Impl {
    float learningRate;
    float maxWeight;
    float minWeight;
    float covarianceThreshold;
    float activityBaseline;
    float adaptationRate;
    float runningPreActivity;
    float runningPostActivity;
    float runningCovariance;
    float activityWindow;
    
    Impl() : learningRate(0.01f), maxWeight(1.0f), minWeight(-1.0f),
             covarianceThreshold(0.0f), activityBaseline(0.1f),
             adaptationRate(0.01f), runningPreActivity(0.0f),
             runningPostActivity(0.0f), runningCovariance(0.0f),
             activityWindow(100.0f) {}
};

Hebbian::Hebbian() : pImpl(new Impl) {}

Hebbian::~Hebbian() = default;

void Hebbian::update(Synapse* synapse,
                     const std::vector<Timestamp>& preSpikes,
                     const std::vector<Timestamp>& postSpikes,
                     TimestepDuration dt) {
    if (!synapse || preSpikes.empty() || postSpikes.empty()) {
        return;
    }
    
    float preActivity = calculateActivity(preSpikes);
    float postActivity = calculateActivity(postSpikes);
    float covariance = calculateCovariance(preSpikes, postSpikes);
    
    updateRunningStatistics(preActivity, postActivity, covariance, dt);
    
    float delta = calculateWeightChange(covariance, preActivity, postActivity);
    
    if (std::abs(delta) > 1e-6f) {
        applyWeightChange(synapse, delta);
    }
}

void Hebbian::applyWeightChange(Synapse* synapse, SynapticWeight delta) {
    if (!synapse) return;
    
    float newWeight = synapse->getWeight() + delta;
    newWeight = std::clamp(newWeight, pImpl->minWeight, pImpl->maxWeight);
    synapse->setWeight(newWeight);
}

const char* Hebbian::getName() const {
    return "Hebbian";
}

void Hebbian::setLearningRate(float rate) {
    pImpl->learningRate = std::clamp(rate, 0.0f, 1.0f);
}

float Hebbian::getLearningRate() const {
    return pImpl->learningRate;
}

void Hebbian::setMaxWeight(float maxWeight) {
    pImpl->maxWeight = std::clamp(maxWeight, 0.0f, 10.0f);
}

float Hebbian::getMaxWeight() const {
    return pImpl->maxWeight;
}

void Hebbian::setMinWeight(float minWeight) {
    pImpl->minWeight = std::clamp(minWeight, -10.0f, 0.0f);
}

float Hebbian::getMinWeight() const {
    return pImpl->minWeight;
}

void Hebbian::setCovarianceThreshold(float threshold) {
    pImpl->covarianceThreshold = threshold;
}

float Hebbian::getCovarianceThreshold() const {
    return pImpl->covarianceThreshold;
}

void Hebbian::setActivityBaseline(float baseline) {
    pImpl->activityBaseline = std::clamp(baseline, 0.0f, 1.0f);
}

float Hebbian::getActivityBaseline() const {
    return pImpl->activityBaseline;
}

void Hebbian::setAdaptationRate(float rate) {
    pImpl->adaptationRate = std::clamp(rate, 0.001f, 0.1f);
}

float Hebbian::getAdaptationRate() const {
    return pImpl->adaptationRate;
}

void Hebbian::setActivityWindow(float window) {
    pImpl->activityWindow = std::max(window, 1.0f);
}

float Hebbian::getActivityWindow() const {
    return pImpl->activityWindow;
}

float Hebbian::calculateActivity(const std::vector<Timestamp>& spikes) {
    if (spikes.empty()) return 0.0f;
    
    float windowDuration = pImpl->activityWindow;
    size_t spikeCount = spikes.size();
    
    return (static_cast<float>(spikeCount) / windowDuration) * 1000.0f;
}

float Hebbian::calculateCovariance(const std::vector<Timestamp>& preSpikes,
                                   const std::vector<Timestamp>& postSpikes) {
    if (preSpikes.empty() || postSpikes.empty()) return 0.0f;
    
    size_t preCount = preSpikes.size();
    size_t postCount = postSpikes.size();
    
    size_t overlappingSpikes = 0;
    for (Timestamp preTime : preSpikes) {
        for (Timestamp postTime : postSpikes) {
            if (std::abs(static_cast<float>(postTime) - static_cast<float>(preTime)) < 20.0f) {
                overlappingSpikes++;
                break;
            }
        }
    }
    
    float normalizedOverlap = (overlappingSpikes / static_cast<float>(preCount * postCount));
    float covariance = normalizedOverlap * static_cast<float>(preCount) * static_cast<float>(postCount) * pImpl->activityBaseline;
    
    return covariance;
}

void Hebbian::updateRunningStatistics(float preActivity, float postActivity, float covariance, TimestepDuration dt) {
    float decayFactor = std::exp(-static_cast<float>(dt) * 0.001f / pImpl->activityWindow);
    
    pImpl->runningPreActivity = pImpl->runningPreActivity * decayFactor + preActivity * (1.0f - decayFactor);
    pImpl->runningPostActivity = pImpl->runningPostActivity * decayFactor + postActivity * (1.0f - decayFactor);
    pImpl->runningCovariance = pImpl->runningCovariance * decayFactor + covariance * (1.0f - decayFactor);
}

float Hebbian::calculateWeightChange(float covariance, float preActivity, float postActivity) {
    float delta = pImpl->learningRate * covariance;
    
    float baseline = pImpl->learningRate * pImpl->activityBaseline * preActivity * postActivity;
    delta -= baseline;
    
    if (std::abs(delta) < pImpl->covarianceThreshold) {
        delta = 0.0f;
    }
    
    float activityNormalization = std::sqrt(preActivity * postActivity);
    if (activityNormalization > 0.0f) {
        delta /= activityNormalization;
    }
    
    return delta;
}

struct AntiHebbianRule::Impl {
    float learningRate;
    float maxWeight;
    float minWeight;
    float threshold;
    
    Impl() : learningRate(0.01f), maxWeight(1.0f), minWeight(-1.0f), threshold(0.0f) {}
};

AntiHebbianRule::AntiHebbianRule() : pImpl(new Impl) {}

AntiHebbianRule::~AntiHebbianRule() = default;

void AntiHebbianRule::update(Synapse* synapse,
                             const std::vector<Timestamp>& preSpikes,
                             const std::vector<Timestamp>& postSpikes,
                             TimestepDuration dt) {
    if (!synapse || preSpikes.empty() || postSpikes.empty()) {
        return;
    }
    
    size_t postSpikeCount = postSpikes.size();
    size_t preSpikeCount = preSpikes.size();
    
    float weightChange = -pImpl->learningRate * 
                         static_cast<float>(postSpikeCount) * 
                         static_cast<float>(preSpikeCount);
    
    applyWeightChange(synapse, weightChange);
    
    float currentWeight = synapse->getWeight();
    if (currentWeight > pImpl->maxWeight) {
        synapse->setWeight(pImpl->maxWeight);
    } else if (currentWeight < pImpl->minWeight) {
        synapse->setWeight(pImpl->minWeight);
    }
}

void AntiHebbianRule::applyWeightChange(Synapse* synapse, SynapticWeight delta) {
    if (!synapse) return;
    
    float newWeight = synapse->getWeight() + delta;
    newWeight = std::clamp(newWeight, pImpl->minWeight, pImpl->maxWeight);
    synapse->setWeight(newWeight);
}

const char* AntiHebbianRule::getName() const {
    return "AntiHebbian";
}

void AntiHebbianRule::setLearningRate(float rate) {
    pImpl->learningRate = std::clamp(rate, 0.0f, 1.0f);
}

float AntiHebbianRule::getLearningRate() const {
    return pImpl->learningRate;
}

void AntiHebbianRule::setThreshold(float threshold) {
    pImpl->threshold = threshold;
}

float AntiHebbianRule::getThreshold() const {
    return pImpl->threshold;
}

struct BCMRule::Impl {
    float learningRate;
    float maxWeight;
    float minWeight;
    float threshold;
    float activityWindow;
    float currentActivity;
    
    Impl() : learningRate(0.01f), maxWeight(1.0f), minWeight(-1.0f),
             threshold(0.0f), activityWindow(10.0f), currentActivity(0.0f) {}
};

BCMRule::BCMRule() : pImpl(new Impl) {}

BCMRule::~BCMRule() = default;

void BCMRule::update(Synapse* synapse,
                     const std::vector<Timestamp>& preSpikes,
                     const std::vector<Timestamp>& postSpikes,
                     TimestepDuration dt) {
    if (!synapse) {
        return;
    }
    
    float postActivity = calculatePostActivity(postSpikes, dt);
    updateThreshold(postActivity, dt);
    
    float weightChange = calculateBCMWeightChange(synapse, preSpikes, postActivity, dt);
    
    applyWeightChange(synapse, weightChange);
}

void BCMRule::applyWeightChange(Synapse* synapse, SynapticWeight delta) {
    if (!synapse) return;
    
    float newWeight = synapse->getWeight() + delta;
    newWeight = std::clamp(newWeight, pImpl->minWeight, pImpl->maxWeight);
    synapse->setWeight(newWeight);
}

const char* BCMRule::getName() const {
    return "BCM";
}

void BCMRule::setLearningRate(float rate) {
    pImpl->learningRate = std::clamp(rate, 0.0f, 1.0f);
}

float BCMRule::getLearningRate() const {
    return pImpl->learningRate;
}

void BCMRule::setThreshold(float threshold) {
    pImpl->threshold = threshold;
}

float BCMRule::getThreshold() const {
    return pImpl->threshold;
}

void BCMRule::setActivityWindow(float window) {
    pImpl->activityWindow = std::max(window, 1.0f);
}

float BCMRule::getActivityWindow() const {
    return pImpl->activityWindow;
}

float BCMRule::calculatePostActivity(const std::vector<Timestamp>& spikes, TimestepDuration dt) {
    if (spikes.empty()) {
        return 0.0f;
    }
    
    float windowDuration = pImpl->activityWindow;
    size_t recentSpikes = 0;
    
    for (Timestamp spikeTime : spikes) {
        if (spikeTime >= 0.0) {
            recentSpikes++;
        }
    }
    
    float firingRate = (recentSpikes / windowDuration) * 1000.0f;
    pImpl->currentActivity = firingRate;
    
    return firingRate;
}

void BCMRule::updateThreshold(float postActivity, TimestepDuration dt) {
    float epsilon = 0.1f;
    float postSquared = postActivity * postActivity;
    
    float dTheta = epsilon * (postSquared - pImpl->threshold) * (1.0f - pImpl->threshold);
    
    pImpl->threshold += dTheta * static_cast<float>(dt) * 0.001f;
    
    pImpl->threshold = std::clamp(pImpl->threshold, 0.0f, 2.0f);
}

float BCMRule::calculateBCMWeightChange(Synapse* synapse,
                                       const std::vector<Timestamp>& preSpikes,
                                       float postActivity,
                                       TimestepDuration dt) {
    float preActivity = calculatePreActivity(preSpikes, dt);
    
    float weightChange = pImpl->learningRate * 
                         (postActivity - pImpl->threshold) * 
                         preActivity;
    
    return weightChange;
}

float BCMRule::calculatePreActivity(const std::vector<Timestamp>& spikes, TimestepDuration dt) {
    if (spikes.empty()) {
        return 0.0f;
    }
    
    float windowDuration = pImpl->activityWindow;
    size_t recentSpikes = 0;
    
    for (Timestamp spikeTime : spikes) {
        if (spikeTime >= 0.0) {
            recentSpikes++;
        }
    }
    
    return (recentSpikes / windowDuration) * 1000.0f;
}

struct RewardModulatedSTDP::Impl {
    float baseLtpWeight;
    float baseLtdWeight;
    float timeConstant;
    float minWeight;
    float maxWeight;
    float rewardFactor;
    float eligibilityTrace;
    float traceDecay;
    
    Impl() : baseLtpWeight(0.01f), baseLtdWeight(0.012f), timeConstant(20.0f),
             minWeight(-1.0f), maxWeight(1.0f), rewardFactor(1.0f),
             eligibilityTrace(0.0f), traceDecay(0.95f) {}
};

RewardModulatedSTDP::RewardModulatedSTDP() : pImpl(new Impl) {}

RewardModulatedSTDP::~RewardModulatedSTDP() = default;

void RewardModulatedSTDP::update(Synapse* synapse,
                                 const std::vector<Timestamp>& preSpikes,
                                 const std::vector<Timestamp>& postSpikes,
                                 TimestepDuration dt) {
    if (!synapse || preSpikes.empty() || postSpikes.empty()) {
        return;
    }
    
    float weightChange = calculateR-STDPWeightChange(synapse, preSpikes, postSpikes, dt);
    
    updateEligibilityTrace(synapse, weightChange, dt);
    
    applyWeightChange(synapse, weightChange);
}

void RewardModulatedSTDP::applyWeightChange(Synapse* synapse, SynapticWeight delta) {
    if (!synapse) return;
    
    float actualDelta = delta * pImpl->rewardFactor;
    
    float newWeight = synapse->getWeight() + actualDelta;
    newWeight = std::clamp(newWeight, pImpl->minWeight, pImpl->maxWeight);
    synapse->setWeight(newWeight);
}

const char* RewardModulatedSTDP::getName() const {
    return "R-STDP";
}

void RewardModulatedSTDP::setLTPWeight(float weight) {
    pImpl->baseLtpWeight = std::clamp(weight, 0.0f, 1.0f);
}

float RewardModulatedSTDP::getLTPWeight() const {
    return pImpl->baseLtpWeight;
}

void RewardModulatedSTDP::setLTDWeight(float weight) {
    pImpl->baseLtdWeight = std::clamp(weight, 0.0f, 1.0f);
}

float RewardModulatedSTDP::getLTDWeight() const {
    return pImpl->baseLtdWeight;
}

void RewardModulatedSTDP::setTimeConstant(float tau) {
    pImpl->timeConstant = std::clamp(tau, 1.0f, 100.0f);
}

float RewardModulatedSTDP::getTimeConstant() const {
    return pImpl->timeConstant;
}

void RewardModulatedSTDP::setRewardFactor(float factor) {
    pImpl->rewardFactor = std::max(0.0f, factor);
}

float RewardModulatedSTDP::getRewardFactor() const {
    return pImpl->rewardFactor;
}

float RewardModulatedSTDP::calculateR-STDPWeightChange(Synapse* synapse,
                                                       const std::vector<Timestamp>& preSpikes,
                                                       const std::vector<Timestamp>& postSpikes,
                                                       TimestepDuration dt) {
    float totalDelta = 0.0f;
    float tau = pImpl->timeConstant;
    
    for (Timestamp preTime : preSpikes) {
        for (Timestamp postTime : postSpikes) {
            float dtVal = static_cast<float>(postTime - preTime);
            
            if (dtVal > 0) {
                float delta = pImpl->baseLtpWeight * std::exp(-dtVal / tau);
                totalDelta += delta;
            } else if (dtVal < 0) {
                float delta = -pImpl->baseLtdWeight * std::exp(dtVal / tau);
                totalDelta += delta;
            }
        }
    }
    
    return totalDelta;
}

void RewardModulatedSTDP::updateEligibilityTrace(Synapse* synapse, float weightChange, TimestepDuration dt) {
    if (!synapse) return;
    
    float currentTrace = synapse->getEligibilityTrace();
    currentTrace = currentTrace * pImpl->traceDecay + weightChange * pImpl->rewardFactor;
    synapse->setEligibilityTrace(currentTrace);
}

struct TripletSTDP::Impl {
    float ltpWeight;
    float ltdWeight;
    float timeConstant;
    float minWeight;
    float maxWeight;
    float preTraceDecay;
    float postTraceDecay;
    float preTraceValue;
    float postTraceValue;
    
    Impl() : ltpWeight(0.01f), ltdWeight(0.012f), timeConstant(20.0f),
             minWeight(-1.0f), maxWeight(1.0f), preTraceDecay(0.95f),
             postTraceDecay(0.95f), preTraceValue(0.0f), postTraceValue(0.0f) {}
};

TripletSTDP::TripletSTDP() : pImpl(new Impl) {}

TripletSTDP::~TripletSTDP() = default;

void TripletSTDP::update(Synapse* synapse,
                         const std::vector<Timestamp>& preSpikes,
                         const std::vector<Timestamp>& postSpikes,
                         TimestepDuration dt) {
    if (!synapse || preSpikes.empty() || postSpikes.empty()) {
        return;
    }
    
    updateActivityTraces(preSpikes, postSpikes, dt);
    
    float weightChange = calculateTripletWeightChange(synapse, preSpikes, postSpikes, dt);
    
    applyWeightChange(synapse, weightChange);
}

void TripletSTDP::applyWeightChange(Synapse* synapse, SynapticWeight delta) {
    if (!synapse) return;
    
    float newWeight = synapse->getWeight() + delta;
    newWeight = std::clamp(newWeight, pImpl->minWeight, pImpl->maxWeight);
    synapse->setWeight(newWeight);
}

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

void TripletSTDP::setPreTraceDecay(float decay) {
    pImpl->preTraceDecay = std::clamp(decay, 0.1f, 0.999f);
}

float TripletSTDP::getPreTraceDecay() const {
    return pImpl->preTraceDecay;
}

void TripletSTDP::setPostTraceDecay(float decay) {
    pImpl->postTraceDecay = std::clamp(decay, 0.1f, 0.999f);
}

float TripletSTDP::getPostTraceDecay() const {
    return pImpl->postTraceDecay;
}

float TripletSTDP::calculateTripletWeightChange(Synapse* synapse,
                                                const std::vector<Timestamp>& preSpikes,
                                                const std::vector<Timestamp>& postSpikes,
                                                TimestepDuration dt) {
    float totalDelta = 0.0f;
    float tau = pImpl->timeConstant;
    
    for (Timestamp preTime : preSpikes) {
        for (Timestamp postTime : postSpikes) {
            float dtVal = static_cast<float>(postTime - preTime);
            
            if (dtVal > 0) {
                float weightChange = pImpl->ltpWeight * std::exp(-dtVal / tau);
                weightChange *= (1.0f + pImpl->postTraceValue);
                totalDelta += weightChange;
            } else if (dtVal < 0) {
                float weightChange = -pImpl->ltdWeight * std::exp(dtVal / tau);
                weightChange *= (1.0f + pImpl->preTraceValue);
                totalDelta += weightChange;
            }
        }
    }
    
    return totalDelta;
}

void TripletSTDP::updateActivityTraces(const std::vector<Timestamp>& preSpikes,
                                       const std::vector<Timestamp>& postSpikes,
                                       TimestepDuration dt) {
    pImpl->preTraceValue *= pImpl->preTraceDecay;
    pImpl->postTraceValue *= pImpl->postTraceDecay;
    
    float dtMs = static_cast<float>(dt) * 1000.0f;
    
    for (Timestamp preTime : preSpikes) {
        pImpl->preTraceValue += 1.0f;
    }
    
    for (Timestamp postTime : postSpikes) {
        pImpl->postTraceValue += 1.0f;
    }
    
    pImpl->preTraceValue = std::min(pImpl->preTraceValue, 10.0f);
    pImpl->postTraceValue = std::min(pImpl->postTraceValue, 10.0f);
}

} // namespace nlm
