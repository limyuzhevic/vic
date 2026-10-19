#include "homeostatic_plasticity.hpp"
#include "../../brain/Synapse.hpp"
#include "../../brain/Brain.hpp"
#include <cmath>
#include <algorithm>
#include <numeric>

namespace nlm
{

// HomeostaticPlasticity implementation
struct HomeostaticPlasticity::Impl {
    float targetFiringRate;
    float plasticityExponent;
    float learningRate;
    bool synapticScalingEnabled;
    bool intrinsicPlasticityEnabled;
    float activityThreshold;
    
    float currentFiringRate;
    float averageFiringRate;
    float firingRateVariance;
    float timeAccumulator;
    
    Impl() : targetFiringRate(0.1f), plasticityExponent(2.0f), learningRate(0.01f),
             synapticScalingEnabled(true), intrinsicPlasticityEnabled(true),
             activityThreshold(0.1f), currentFiringRate(0.0f),
             averageFiringRate(0.0f), firingRateVariance(0.0f),
             timeAccumulator(0.0f) {}
};

HomeostaticPlasticity::HomeostaticPlasticity() : pImpl(std::make_unique<Impl>()) {}

HomeostaticPlasticity::~HomeostaticPlasticity() = default;

void HomeostaticPlasticity::update(Synapse* synapse,
                                    const std::vector<Timestamp>& preSpikes,
                                    const std::vector<Timestamp>& postSpikes,
                                    TimestepDuration dt) {
    if (!synapse) {
        return;
    }
    
    // Calculate current firing rate from post-synaptic activity
    float postRate = postSpikes.empty() ? 0.0f : 
                     static_cast<float>(postSpikes.size()) / std::max(dt, 1.0);
    pImpl->currentFiringRate = postRate;
    
    // Update activity statistics
    updateActivityStatistics(preSpikes, dt);
    
    // Apply homeostatic regulation
    float homeostaticDelta = 0.0f;
    
    if (pImpl->synapticScalingEnabled) {
        // Synaptic scaling: scale all synaptic weights based on average firing rate
        float scalingFactor = calculateSynapticScaling(pImpl->currentFiringRate, pImpl->targetFiringRate);
        homeostaticDelta += pImpl->learningRate * (scalingFactor - 1.0f);
    }
    
    if (pImpl->intrinsicPlasticityEnabled) {
        // Intrinsic plasticity: adjust intrinsic excitability
        float plasticityChange = calculateIntrinsicPlasticity(pImpl->currentFiringRate, pImpl->targetFiringRate);
        homeostaticDelta += plasticityChange;
    }
    
    // Apply activity-dependent plasticity
    float activityDelta = calculateActivityDependentPlasticity(pImpl->currentFiringRate);
    homeostaticDelta += activityDelta;
    
    applyWeightChange(synapse, homeostaticDelta);
}

void HomeostaticPlasticity::applyWeightChange(Synapse* synapse, SynapticWeight delta) {
    if (!synapse) return;
    
    float newWeight = synapse->getWeight() + delta;
    newWeight = std::clamp(newWeight, -1.0f, 1.0f);
    synapse->setWeight(newWeight);
}

const char* HomeostaticPlasticity::getName() const {
    return "HomeostaticPlasticity";
}

void HomeostaticPlasticity::setTargetFiringRate(float target) {
    pImpl->targetFiringRate = std::max(0.0f, target);
}

void HomeostaticPlasticity::setPlasticityExponent(float exponent) {
    pImpl->plasticityExponent = std::max(0.5f, exponent);
}

void HomeostaticPlasticity::setLearningRate(float rate) {
    pImpl->learningRate = std::clamp(rate, 0.0f, 1.0f);
}

void HomeostaticPlasticity::setSynapticScaling(bool enable) {
    pImpl->synapticScalingEnabled = enable;
}

void HomeostaticPlasticity::setIntrinsicPlasticity(bool enable) {
    pImpl->intrinsicPlasticityEnabled = enable;
}

void HomeostaticPlasticity::setActivityThreshold(float threshold) {
    pImpl->activityThreshold = std::max(0.0f, threshold);
}

float HomeostaticPlasticity::getTargetFiringRate() const {
    return pImpl->targetFiringRate;
}

float HomeostaticPlasticity::getPlasticityExponent() const {
    return pImpl->plasticityExponent;
}

float HomeostaticPlasticity::getLearningRate() const {
    return pImpl->learningRate;
}

bool HomeostaticPlasticity::isSynapticScalingEnabled() const {
    return pImpl->synapticScalingEnabled;
}

bool HomeostaticPlasticity::isIntrinsicPlasticityEnabled() const {
    return pImpl->intrinsicPlasticityEnabled;
}

float HomeostaticPlasticity::getActivityThreshold() const {
    return pImpl->activityThreshold;
}

void HomeostaticPlasticity::updateActivityStatistics(const std::vector<Timestamp>& spikes, float dt) {
    float rate = spikes.empty() ? 0.0f : static_cast<float>(spikes.size()) / std::max(dt, 1.0f);
    
    if (pImpl->averageFiringRate == 0.0f) {
        pImpl->averageFiringRate = rate;
        pImpl->firingRateVariance = 0.0f;
    } else {
        // Exponential moving average
        pImpl->averageFiringRate = 0.9f * pImpl->averageFiringRate + 0.1f * rate;
        // Update variance (simplified)
        pImpl->firingRateVariance = std::abs(rate - pImpl->averageFiringRate);
    }
}

float HomeostaticPlasticity::calculateSynapticScaling(float currentRate, float targetRate) const {
    // Synaptic scaling: multiplicative scaling of all synaptic weights
    // to maintain target firing rate
    if (currentRate <= 0.0f) return 1.0f;
    
    float scalingFactor = std::pow(targetRate / currentRate, 1.0f / pImpl->plasticityExponent);
    return scalingFactor;
}

float HomeostaticPlasticity::calculateIntrinsicPlasticity(float currentRate, float targetRate) const {
    // Intrinsic plasticity: adjust intrinsic excitability
    float plasticityChange = 0.0f;
    
    float rateError = currentRate - targetRate;
    if (std::abs(rateError) > pImpl->activityThreshold) {
        plasticityChange = pImpl->learningRate * rateError * 0.1f;
    }
    
    return plasticityChange;
}

float HomeostaticPlasticity::calculateActivityDependentPlasticity(float activity) const {
    // Activity-dependent plasticity for stability
    float activityDelta = 0.0f;
    
    if (activity > pImpl->activityThreshold) {
        // High activity: depress synapses to reduce overall activity
        activityDelta = -pImpl->learningRate * 0.5f;
    } else if (activity < pImpl->activityThreshold * 0.5f) {
        // Low activity: potentiate synapses to increase activity
        activityDelta = pImpl->learningRate * 0.3f;
    }
    
    return activityDelta;
}

float HomeostaticPlasticity::getAverageFiringRate() const {
    return pImpl->averageFiringRate;
}

float HomeostaticPlasticity::getFiringRateVariance() const {
    return pImpl->firingRateVariance;
}

float HomeostaticPlasticity::getActivityFluctuation() const {
    return pImpl->firingRateVariance / std::max(pImpl->averageFiringRate, 0.001f);
}

// SynapticScaling implementation
struct SynapticScaling::Impl {
    float targetFiringRate;
    float scalingExponent;
    float scalingRate;
    float minimumWeight;
    float maximumWeight;
    float networkTotalActivity;
    
    Impl() : targetFiringRate(0.1f), scalingExponent(1.0f), scalingRate(0.01f),
             minimumWeight(-0.5f), maximumWeight(0.5f), networkTotalActivity(0.0f) {}
};

SynapticScaling::SynapticScaling() : pImpl(std::make_unique<Impl>()) {}

SynapticScaling::~SynapticScaling() = default;

void SynapticScaling::update(Synapse* synapse,
                             const std::vector<Timestamp>& preSpikes,
                             const std::vector<Timestamp>& postSpikes,
                             TimestepDuration dt) {
    if (!synapse) return;
    
    // Calculate scaling factor based on network activity
    float scalingFactor = calculateScalingFactor(pImpl->currentRate, pImpl->targetFiringRate);
    
    // Apply scaling to synapse weight
    float delta = pImpl->scalingRate * (scalingFactor - 1.0f);
    applyWeightChange(synapse, delta);
}

void SynapticScaling::applyWeightChange(Synapse* synapse, SynapticWeight delta) {
    if (!synapse) return;
    
    float newWeight = synapse->getWeight() + delta;
    newWeight = std::clamp(newWeight, pImpl->minimumWeight, pImpl->maximumWeight);
    synapse->setWeight(newWeight);
}

const char* SynapticScaling::getName() const {
    return "SynapticScaling";
}

void SynapticScaling::setTargetFiringRate(float target) {
    pImpl->targetFiringRate = std::max(0.0f, target);
}

void SynapticScaling::setScalingExponent(float exponent) {
    pImpl->scalingExponent = std::max(0.5f, exponent);
}

void SynapticScaling::setScalingRate(float rate) {
    pImpl->scalingRate = std::clamp(rate, 0.0f, 1.0f);
}

void SynapticScaling::setMinimumWeight(float minWeight) {
    pImpl->minimumWeight = std::clamp(minWeight, -1.0f, 0.0f);
}

void SynapticScaling::setMaximumWeight(float maxWeight) {
    pImpl->maximumWeight = std::clamp(maxWeight, 0.0f, 1.0f);
    if (pImpl->minimumWeight >= pImpl->maximumWeight) {
        pImpl->maximumWeight = pImpl->minimumWeight + 0.1f;
    }
}

float SynapticScaling::getTargetFiringRate() const {
    return pImpl->targetFiringRate;
}

float SynapticScaling::getScalingExponent() const {
    return pImpl->scalingExponent;
}

float SynapticScaling::getScalingRate() const {
    return pImpl->scalingRate;
}

float SynapticScaling::getMinimumWeight() const {
    return pImpl->minimumWeight;
}

float SynapticScaling::getMaximumWeight() const {
    return pImpl->maximumWeight;
}

float SynapticScaling::calculateScalingFactor(float currentRate, float targetRate) const {
    if (currentRate <= 0.0f) return 1.0f;
    
    float scalingFactor = std::pow(targetRate / currentRate, 1.0f / pImpl->scalingExponent);
    return std::clamp(scalingFactor, 0.5f, 2.0f);
}

float SynapticScaling::getNetworkTotalActivity() const {
    return pImpl->networkTotalActivity;
}

void SynapticScaling::updateNetworkStatistics(Brain* brain) {
    // Update network statistics for scaling calculations
    if (!brain) return;
    
    // This would update network-wide statistics
    // For now, placeholder implementation
    pImpl->networkTotalActivity = 0.0f;
}

// IntrinsicPlasticity implementation
struct IntrinsicPlasticity::Impl {
    float targetFiringRate;
    float plasticityRate;
    float activityDependentGain;
    float homeostaticDrift;
    float intrinsicExcitability;
    float timeAccumulator;
    
    Impl() : targetFiringRate(0.1f), plasticityRate(0.01f), activityDependentGain(0.1f),
             homeostaticDrift(0.001f), intrinsicExcitability(1.0f), timeAccumulator(0.0f) {}
};

IntrinsicPlasticity::IntrinsicPlasticity() : pImpl(std::make_unique<Impl>()) {}

IntrinsicPlasticity::~IntrinsicPlasticity() = default;

void IntrinsicPlasticity::update(Synapse* synapse,
                                 const std::vector<Timestamp>& preSpikes,
                                 const std::vector<Timestamp>& postSpikes,
                                 TimestepDuration dt) {
    if (!synapse) return;
    
    // Calculate current firing rate
    float currentRate = postSpikes.empty() ? 0.0f : 
                        static_cast<float>(postSpikes.size()) / std::max(dt, 1.0);
    
    // Calculate intrinsic plasticity effect
    float plasticityChange = calculateIntrinsicPlasticity(currentRate, pImpl->targetFiringRate);
    
    // Apply to intrinsic excitability
    pImpl->intrinsicExcitability += plasticityChange * dt;
    pImpl->intrinsicExcitability = std::clamp(pImpl->intrinsicExcitability, 0.1f, 10.0f);
    
    // Apply to synapse
    float delta = plasticityChange * pImpl->intrinsicExcitability;
    applyWeightChange(synapse, delta);
}

void IntrinsicPlasticity::applyWeightChange(Synapse* synapse, SynapticWeight delta) {
    if (!synapse) return;
    
    float newWeight = synapse->getWeight() + delta;
    newWeight = std::clamp(newWeight, -1.0f, 1.0f);
    synapse->setWeight(newWeight);
}

const char* IntrinsicPlasticity::getName() const {
    return "IntrinsicPlasticity";
}

void IntrinsicPlasticity::setTargetFiringRate(float target) {
    pImpl->targetFiringRate = std::max(0.0f, target);
}

void IntrinsicPlasticity::setPlasticityRate(float rate) {
    pImpl->plasticityRate = std::clamp(rate, 0.0f, 1.0f);
}

void IntrinsicPlasticity::setActivityDependentGain(float gain) {
    pImpl->activityDependentGain = std::clamp(gain, 0.0f, 1.0f);
}

void IntrinsicPlasticity::setHomeostaticDrift(float drift) {
    pImpl->homeostaticDrift = std::max(0.0f, drift);
}

float IntrinsicPlasticity::getTargetFiringRate() const {
    return pImpl->targetFiringRate;
}

float IntrinsicPlasticity::getPlasticityRate() const {
    return pImpl->plasticityRate;
}

float IntrinsicPlasticity::getActivityDependentGain() const {
    return pImpl->activityDependentGain;
}

float IntrinsicPlasticity::getHomeostaticDrift() const {
    return pImpl->homeostaticDrift;
}

float IntrinsicPlasticity::calculateIntrinsicPlasticity(float currentRate, float targetRate) const {
    float plasticityChange = 0.0f;
    
    // Activity-dependent component
    float activityError = currentRate - targetRate;
    plasticityChange += activityError * pImpl->activityDependentGain;
    
    // Homeostatic drift
    plasticityChange += pImpl->homeostaticDrift * (currentRate - targetRate);
    
    return plasticityChange;
}

float IntrinsicPlasticity::getIntrinsicExcitability() const {
    return pImpl->intrinsicExcitability;
}

void IntrinsicPlasticity::updateIntrinsicExcitability(float dt) {
    pImpl->timeAccumulator += dt;
}

// BCMPlasticity implementation
struct BCMPlasticity::Impl {
    float theta;                    // Sliding threshold (θ)
    float sigma;                    // Threshold sensitivity (σ)
    float learningRate;             // Learning rate
    float maxWeight;                // Maximum weight
    float minWeight;                // Minimum weight
    float thetaAdaptationRate;      // Rate of threshold adaptation
    float slidingThreshold;         // Current BCM threshold (θ)
    
    Impl() : theta(0.5f), sigma(0.1f), learningRate(0.01f),
             maxWeight(1.0f), minWeight(-1.0f), thetaAdaptationRate(0.001f),
             slidingThreshold(0.5f) {}
};

BCMPlasticity::BCMPlasticity() : pImpl(std::make_unique<Impl>()) {}

BCMPlasticity::~BCMPlasticity() = default;

void BCMPlasticity::update(Synapse* synapse,
                           const std::vector<Timestamp>& preSpikes,
                           const std::vector<Timestamp>& postSpikes,
                           TimestepDuration dt) {
    if (!synapse || postSpikes.empty()) {
        return;
    }
    
    // Calculate post-synaptic firing rate
    float postRate = static_cast<float>(postSpikes.size()) / std::max(dt, 1.0);
    
    // Update sliding threshold
    updateSlidingThreshold(postRate);
    
    // Calculate BCM change
    float delta = calculateBCMChange(postRate, pImpl->slidingThreshold);
    
    applyWeightChange(synapse, delta);
}

void BCMPlasticity::applyWeightChange(Synapse* synapse, SynapticWeight delta) {
    if (!synapse) return;
    
    float newWeight = synapse->getWeight() + delta;
    newWeight = std::clamp(newWeight, pImpl->minWeight, pImpl->maxWeight);
    synapse->setWeight(newWeight);
}

const char* BCMPlasticity::getName() const {
    return "BCMPlasticity";
}

void BCMPlasticity::setTheta(float theta) {
    pImpl->theta = std::clamp(theta, 0.0f, 1.0f);
    pImpl->slidingThreshold = pImpl->theta;
}

void BCMPlasticity::setSigma(float sigma) {
    pImpl->sigma = std::clamp(sigma, 0.0f, 1.0f);
}

void BCMPlasticity::setLearningRate(float rate) {
    pImpl->learningRate = std::clamp(rate, 0.0f, 1.0f);
}

void BCMPlasticity::setMaximumWeight(float maxWeight) {
    pImpl->maxWeight = std::clamp(maxWeight, 0.0f, 2.0f);
}

void BCMPlasticity::setMinimumWeight(float minWeight) {
    pImpl->minWeight = std::clamp(minWeight, -2.0f, 0.0f);
    if (pImpl->maxWeight <= pImpl->minWeight) {
        pImpl->maxWeight = pImpl->minWeight + 0.1f;
    }
}

void BCMPlasticity::setThetaAdaptationRate(float rate) {
    pImpl->thetaAdaptationRate = std::clamp(rate, 0.0f, 0.01f);
}

float BCMPlasticity::getTheta() const {
    return pImpl->theta;
}

float BCMPlasticity::getSigma() const {
    return pImpl->sigma;
}

float BCMPlasticity::getLearningRate() const {
    return pImpl->learningRate;
}

float BCMPlasticity::getMaximumWeight() const {
    return pImpl->maxWeight;
}

float BCMPlasticity::getMinimumWeight() const {
    return pImpl->minWeight;
}

float BCMPlasticity::getThetaAdaptationRate() const {
    return pImpl->thetaAdaptationRate;
}

void BCMPlasticity::updateSlidingThreshold(float postActivity) {
    // BCM sliding threshold adaptation
    // θ = θ × (1 - σ) + σ × post²
    float newTheta = pImpl->slidingThreshold * (1.0f - pImpl->sigma) + 
                     pImpl->sigma * postActivity * postActivity;
    
    // Constrain threshold
    newTheta = std::clamp(newTheta, 0.0f, 1.0f);
    pImpl->slidingThreshold = newTheta;
}

float BCMPlasticity::getSlidingThreshold() const {
    return pImpl->slidingThreshold;
}

float BCMPlasticity::calculateBCMChange(float postActivity, float theta) const {
    // BCM learning rule: Δw = η × (post × (post - θ))
    float postMinusTheta = postActivity - theta;
    float delta = pImpl->learningRate * postActivity * postMinusTheta;
    
    // Limit the change magnitude
    delta = std::clamp(delta, -pImpl->learningRate, pImpl->learningRate);
    
    return delta;
}

// MetaplasticHomeostasis implementation
struct MetaplasticHomeostasis::Impl {
    float homeostaticAdaptationRate;
    float stabilityTarget;
    float flexibilityTarget;
    size_t learningHistoryCapacity;
    float metaplasticThreshold;
    
    float averageLearningMagnitude;
    float learningVolatility;
    float stabilityIndex;
    float timeAccumulator;
    
    Impl() : homeostaticAdaptationRate(0.01f), stabilityTarget(0.5f), flexibilityTarget(0.5f),
             learningHistoryCapacity(1000), metaplasticThreshold(0.05f),
             averageLearningMagnitude(0.0f), learningVolatility(0.0f),
             stabilityIndex(1.0f), timeAccumulator(0.0f) {}
};

MetaplasticHomeostasis::MetaplasticHomeostasis() : pImpl(std::make_unique<Impl>()) {}

MetaplasticHomeostasis::~MetaplasticHomeostasis() = default;

void MetaplasticHomeostasis::update(Synapse* synapse,
                                    const std::vector<Timestamp>& preSpikes,
                                    const std::vector<Timestamp>& postSpikes,
                                    TimestepDuration dt) {
    // Metaplastic homeostasis: plasticity of homeostatic mechanisms
    // Adapt homeostatic parameters based on learning history
    
    if (!synapse) return;
    
    // Calculate current stability based on learning volatility
    float currentVolatility = calculateLearningVolatility();
    
    // Adapt homeostatic parameters
    float homeostaticAdaptation = calculateHomeostaticAdaptation(currentVolatility);
    
    // Update metaplastic state
    updateMetaplasticState(dt);
    
    // Apply to synapse
    float delta = homeostaticAdaptation * 0.01f;
    applyWeightChange(synapse, delta);
}

void MetaplasticHomeostasis::applyWeightChange(Synapse* synapse, SynapticWeight delta) {
    if (!synapse) return;
    
    float newWeight = synapse->getWeight() + delta;
    newWeight = std::clamp(newWeight, -1.0f, 1.0f);
    synapse->setWeight(newWeight);
}

const char* MetaplasticHomeostasis::getName() const {
    return "MetaplasticHomeostasis";
}

void MetaplasticHomeostasis::setHomeostaticAdaptationRate(float rate) {
    pImpl->homeostaticAdaptationRate = std::clamp(rate, 0.0f, 0.1f);
}

void MetaplasticHomeostasis::setStabilityTarget(float target) {
    pImpl->stabilityTarget = std::clamp(target, 0.0f, 1.0f);
}

void MetaplasticHomeostasis::setFlexibilityTarget(float target) {
    pImpl->flexibilityTarget = std::clamp(target, 0.0f, 1.0f);
}

void MetaplasticHomeostasis::setLearningHistoryCapacity(size_t capacity) {
    pImpl->learningHistoryCapacity = capacity;
}

void MetaplasticHomeostasis::setMetaplasticThreshold(float threshold) {
    pImpl->metaplasticThreshold = std::clamp(threshold, 0.0f, 0.1f);
}

float MetaplasticHomeostasis::getHomeostaticAdaptationRate() const {
    return pImpl->homeostaticAdaptationRate;
}

float MetaplasticHomeostasis::getStabilityTarget() const {
    return pImpl->stabilityTarget;
}

float MetaplasticHomeostasis::getFlexibilityTarget() const {
    return pImpl->flexibilityTarget;
}

size_t MetaplasticHomeostasis::getLearningHistoryCapacity() const {
    return pImpl->learningHistoryCapacity;
}

float MetaplasticHomeostasis::getMetaplasticThreshold() const {
    return pImpl->metaplasticThreshold;
}

void MetaplasticHomeostasis::recordHomeostaticEvent(float magnitude, float time) {
    // Update running statistics
    static float totalMagnitude = 0.0f;
    static size_t eventCount = 0;
    
    totalMagnitude += magnitude;
    eventCount++;
    
    if (eventCount > 0) {
        pImpl->averageLearningMagnitude = totalMagnitude / static_cast<float>(eventCount);
        // Calculate volatility (simplified)
        pImpl->learningVolatility = std::sqrt(pImpl->averageLearningMagnitude * 0.01f);
        
        // Calculate stability index
        pImpl->stabilityIndex = 1.0f / (1.0f + pImpl->learningVolatility);
    }
}

float MetaplasticHomeostasis::calculateHomeostaticAdaptation(float learningHistory) const {
    // Adapt homeostatic parameters based on learning history
    float adaptation = 1.0f;
    
    if (learningHistory > pImpl->metaplasticThreshold) {
        // High learning volatility: increase flexibility, decrease stability
        adaptation *= (1.0f + pImpl->learningVolatility * 0.5f);
    } else {
        // Low learning volatility: increase stability
        adaptation /= (1.0f + pImpl->learningVolatility * 0.5f);
    }
    
    return adaptation;
}

float MetaplasticHomeostasis::getSystemStability() const {
    return pImpl->stabilityIndex;
}

float MetaplasticHomeostasis::getSystemFlexibility() const {
    return 1.0f - pImpl->stabilityIndex;
}

void MetaplasticHomeostasis::updateMetaplasticState(float dt) {
    // Update metaplastic state based on learning history
    float adaptiveRate = getAdaptiveLearningRate();
    // In a real implementation, this would update homeostatic parameters
    (void)adaptiveRate; (void)dt; // For consistency with interface
}

} // namespace nlm

