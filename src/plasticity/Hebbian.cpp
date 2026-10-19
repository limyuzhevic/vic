#include "Hebbian.hpp"
#include "../../brain/Synapse.hpp"
#include <cmath>
#include <algorithm>
#include <numeric>
#include <algorithm>

namespace nlm
{

// Comprehensive Hebbian implementation with multiple learning rules
struct Hebbian::Impl {
    float learningRate;
    float baselineThreshold;
    float homeostaticTarget;
    float maxWeight;
    float minWeight;
    bool useCovarianceRule;
    bool useBCMTheory;
    bool useAntiHebbian;
    bool useHomeostaticPlasticity;
    
    // Activity statistics for rule selection
    float preSpikeHistory;
    float postSpikeHistory;
    float preFiringRate;
    float postFiringRate;
    float timeConstant;
    
    // Homeostatic regulation
    float globalActivityTrend;
    float homeostaticError;
    
    Impl() : 
        learningRate(0.01f),
        baselineThreshold(0.5f),
        homeostaticTarget(0.1f),
        maxWeight(1.0f),
        minWeight(-1.0f),
        useCovarianceRule(true),
        useBCMTheory(false),
        useAntiHebbian(false),
        useHomeostaticPlasticity(true),
        preSpikeHistory(0.0f),
        postSpikeHistory(0.0f),
        preFiringRate(0.0f),
        postFiringRate(0.0f),
        timeConstant(20.0f),
        globalActivityTrend(0.0f),
        homeostaticError(0.0f) {}
};

Hebbian::Hebbian() : pImpl(std::make_unique<Impl>()) {}

Hebbian::~Hebbian() = default;

void Hebbian::update(Synapse* synapse,
                     const std::vector<Timestamp>& preSpikes,
                     const std::vector<Timestamp>& postSpikes,
                     TimestepDuration dt) {
    if (!synapse || preSpikes.empty() || postSpikes.empty()) {
        return;
    }
    
    // Calculate firing rates from spike history
    float preRate = static_cast<float>(preSpikes.size()) / std::max(dt, 1.0);
    float postRate = static_cast<float>(postSpikes.size()) / std::max(dt, 1.0);
    
    // Update activity history
    pImpl->preSpikeHistory = preRate;
    pImpl->postSpikeHistory = postRate;
    pImpl->preFiringRate = preRate;
    pImpl->postFiringRate = postRate;
    
    float delta = 0.0f;
    
    // Select and apply the appropriate Hebbian rule
    if (pImpl->useCovarianceRule) {
        // Covariance-based Hebbian learning (classical)
        // Δw = η × (⟨x·y⟩ - ⟨x⟩⟨y⟩)
        float baselinePre = std::max(pImpl->preFiringRate, pImpl->postFiringRate) * 0.1f;
        float baselinePost = std::max(pImpl->postFiringRate, pImpl->preFiringRate) * 0.1f;
        
        delta = PlasticityRule::calculateCovarianceChange(
            preRate, postRate, baselinePre, baselinePost,
            pImpl->learningRate, pImpl->timeConstant);
        
    } else if (pImpl->useBCMTheory) {
        // BCM theory would be handled by separate BCM class
        // This is a simplified version
        float theta = pImpl->baselineThreshold;
        delta = pImpl->learningRate * postRate * (postRate - theta);
        
    } else {
        // Classic Hebbian: "fire together, wire together"
        delta = pImpl->learningRate * preRate * postRate;
    }
    
    // Apply anti-Hebbian if enabled
    if (pImpl->useAntiHebbian) {
        delta *= -1.0f; // Invert sign for anti-Hebbian
    }
    
    // Apply homeostatic scaling if enabled
    if (pImpl->useHomeostaticPlasticity) {
        // Calculate homeostatic error
        float totalActivity = preRate + postRate;
        pImpl->homeostaticError = totalActivity - pImpl->homeostaticTarget;
        
        // Apply homeostatic regulation to learning rate
        applyHomeostaticScaling(totalActivity, pImpl->homeostaticTarget);
        
        // Scale the weight change based on homeostatic error
        delta *= (1.0f - std::abs(pImpl->homeostaticError) * 0.1f);
    }
    
    // Apply metaplasticity based on previous learning
    updateMetaplasticity(lastLearningMagnitude_, dt);
    
    // Apply eligibility trace for reward-modulated learning
    eligibilityTrace_ = calculateEligibilityTrace(delta, 0.9f);
    
    // Apply weight change with bounds
    if (std::abs(delta) > 1e-6f) {
        applyWeightChange(synapse, delta);
        lastLearningMagnitude_ = std::abs(delta);
    }
}

void Hebbian::applyWeightChange(Synapse* synapse, SynapticWeight delta) {
    if (!synapse) return;
    
    float newWeight = synapse->getWeight() + delta;
    newWeight = clampWeight(newWeight, pImpl->minWeight, pImpl->maxWeight);
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

void Hebbian::setBaselineThreshold(float threshold) {
    pImpl->baselineThreshold = std::clamp(threshold, 0.0f, 1.0f);
}

float Hebbian::getBaselineThreshold() const {
    return pImpl->baselineThreshold;
}

void Hebbian::setHomeostaticTarget(float target) {
    pImpl->homeostaticTarget = std::max(0.0f, target);
}

float Hebbian::getHomeostaticTarget() const {
    return pImpl->homeostaticTarget;
}

void Hebbian::setMaxWeight(float maxWeight) {
    pImpl->maxWeight = std::max(maxWeight, pImpl->minWeight + 0.01f);
}

float Hebbian::getMaxWeight() const {
    return pImpl->maxWeight;
}

void Hebbian::enableCovarianceRule(bool enable) {
    pImpl->useCovarianceRule = enable;
}

bool Hebbian::isCovarianceRuleEnabled() const {
    return pImpl->useCovarianceRule;
}

void Hebbian::enableBCMTheory(bool enable) {
    pImpl->useBCMTheory = enable;
}

bool Hebbian::isBCMTheoryEnabled() const {
    return pImpl->useBCMTheory;
}

void Hebbian::enableAntiHebbian(bool enable) {
    pImpl->useAntiHebbian = enable;
}

bool Hebbian::isAntiHebbianEnabled() const {
    return pImpl->useAntiHebbian;
}

void Hebbian::enableHomeostaticPlasticity(bool enable) {
    pImpl->useHomeostaticPlasticity = enable;
}

bool Hebbian::isHomeostaticPlasticityEnabled() const {
    return pImpl->useHomeostaticPlasticity;
}

float Hebbian::getPreSpikeHistory() const {
    return pImpl->preSpikeHistory;
}

float Hebbian::getPostSpikeHistory() const {
    return pImpl->postSpikeHistory;
}

void Hebbian::resetActivityHistory() {
    pImpl->preSpikeHistory = 0.0f;
    pImpl->postSpikeHistory = 0.0f;
    pImpl->preFiringRate = 0.0f;
    pImpl->postFiringRate = 0.0f;
}

// AntiHebbian implementation
struct AntiHebbian::Impl {
    float depressionRate;
    float baselineFiring;
    float minWeight;
    
    Impl() : depressionRate(0.01f), baselineFiring(0.01f), minWeight(-0.5f) {}
};

AntiHebbian::AntiHebbian() : pImpl(std::make_unique<Impl>()) {}

AntiHebbian::~AntiHebbian() = default;

void AntiHebbian::update(Synapse* synapse,
                         const std::vector<Timestamp>& preSpikes,
                         const std::vector<Timestamp>& postSpikes,
                         TimestepDuration dt) {
    if (!synapse || preSpikes.empty() || postSpikes.empty()) {
        return;
    }
    
    // Calculate firing rates
    float preRate = static_cast<float>(preSpikes.size()) / std::max(dt, 1.0);
    float postRate = static_cast<float>(postSpikes.size()) / std::max(dt, 1.0);
    
    // Anti-Hebbian: decrease weight when both neurons fire
    // Δw = -η × preRate × postRate
    float delta = -pImpl->depressionRate * preRate * postRate;
    
    // Limit depression magnitude
    float maxDepression = pImpl->depressionRate * 0.5f;
    if (std::abs(delta) > maxDepression) {
        delta = std::copysign(maxDepression, delta);
    }
    
    // Ensure we don't go below minimum weight
    float currentWeight = synapse->getWeight();
    float potentialWeight = currentWeight + delta;
    if (potentialWeight < pImpl->minWeight) {
        delta = pImpl->minWeight - currentWeight;
    }
    
    applyWeightChange(synapse, delta);
}

void AntiHebbian::applyWeightChange(Synapse* synapse, SynapticWeight delta) {
    if (!synapse) return;
    
    float newWeight = synapse->getWeight() + delta;
    newWeight = clampWeight(newWeight, -1.0f, pImpl->minWeight);
    synapse->setWeight(newWeight);
}

const char* AntiHebbian::getName() const {
    return "AntiHebbian";
}

void AntiHebbian::setDepressionRate(float rate) {
    pImpl->depressionRate = std::clamp(rate, 0.0f, 1.0f);
}

float AntiHebbian::getDepressionRate() const {
    return pImpl->depressionRate;
}

void AntiHebbian::setBaselineFiring(float firing) {
    pImpl->baselineFiring = std::max(0.0f, firing);
}

float AntiHebbian::getBaselineFiring() const {
    return pImpl->baselineFiring;
}

void AntiHebbian::setMinimumWeight(float weight) {
    pImpl->minWeight = std::clamp(weight, -1.0f, 1.0f);
}

float AntiHebbian::getMinimumWeight() const {
    return pImpl->minWeight;
}

// BCM implementation
struct BCM::Impl {
    float theta;                    // Sliding threshold
    float sigma;                    // Threshold sensitivity
    float learningRate;             // Learning rate
    float maxWeight;                // Maximum weight
    float minWeight;                // Minimum weight
    float preSpikeHistory;          // Pre-synaptic spike history
    float postSpikeHistory;         // Post-synaptic spike history
    float slidingThreshold;         // Current BCM threshold (θ)
    float adaptationRate;           // Rate of threshold adaptation
    
    Impl() : theta(0.5f), sigma(0.1f), learningRate(0.01f),
             maxWeight(1.0f), minWeight(-1.0f),
             preSpikeHistory(0.0f), postSpikeHistory(0.0f),
             slidingThreshold(0.5f), adaptationRate(0.001f) {}
};

BCM::BCM() : pImpl(std::make_unique<Impl>()) {}

BCM::~BCM() = default;

void BCM::update(Synapse* synapse,
                 const std::vector<Timestamp>& preSpikes,
                 const std::vector<Timestamp>& postSpikes,
                 TimestepDuration dt) {
    if (!synapse || postSpikes.empty()) {
        return;
    }
    
    // Update spike history
    float postRate = static_cast<float>(postSpikes.size()) / std::max(dt, 1.0);
    pImpl->postSpikeHistory = postRate;
    pImpl->preSpikeHistory = preSpikeHistory > 0 ? pImpl->preSpikeHistory * 0.9f : 0.0f;
    
    // Update sliding threshold (θ)
    updateSlidingThreshold(postRate);
    
    // BCM learning rule: Δw = η × (post × (post - θ))
    float postMinusTheta = postRate - pImpl->slidingThreshold;
    float delta = pImpl->learningRate * postRate * postMinusTheta;
    
    // Limit the change magnitude
    delta = std::clamp(delta, -pImpl->learningRate, pImpl->learningRate);
    
    applyWeightChange(synapse, delta);
}

void BCM::applyWeightChange(Synapse* synapse, SynapticWeight delta) {
    if (!synapse) return;
    
    float newWeight = synapse->getWeight() + delta;
    newWeight = std::clamp(newWeight, pImpl->minWeight, pImpl->maxWeight);
    synapse->setWeight(newWeight);
}

const char* BCM::getName() const {
    return "BCM";
}

void BCM::setTheta(float theta) {
    pImpl->theta = std::clamp(theta, 0.0f, 1.0f);
    pImpl->slidingThreshold = pImpl->theta;
}

void BCM::setSigma(float sigma) {
    pImpl->sigma = std::clamp(sigma, 0.0f, 1.0f);
}

void BCM::setLearningRate(float rate) {
    pImpl->learningRate = std::clamp(rate, 0.0f, 1.0f);
}

void BCM::setMaximumWeight(float maxWeight) {
    pImpl->maxWeight = std::clamp(maxWeight, 0.0f, 2.0f);
}

void BCM::setMinimumWeight(float minWeight) {
    pImpl->minWeight = std::clamp(minWeight, -2.0f, 0.0f);
    if (pImpl->maxWeight <= pImpl->minWeight) {
        pImpl->maxWeight = pImpl->minWeight + 0.1f;
    }
}

float BCM::getTheta() const {
    return pImpl->theta;
}

float BCM::getSigma() const {
    return pImpl->sigma;
}

float BCM::getLearningRate() const {
    return pImpl->learningRate;
}

float BCM::getMaximumWeight() const {
    return pImpl->maxWeight;
}

float BCM::getMinimumWeight() const {
    return pImpl->minWeight;
}

void BCM::updateSlidingThreshold(float postActivity) {
    // BCM sliding threshold adaptation
    // θ = θ × (1 - σ) + σ × post²
    float newTheta = pImpl->slidingThreshold * (1.0f - pImpl->sigma) + 
                     pImpl->sigma * postActivity * postActivity;
    
    // Constrain threshold
    newTheta = std::clamp(newTheta, 0.0f, 1.0f);
    pImpl->slidingThreshold = newTheta;
}

float BCM::getSlidingThreshold() const {
    return pImpl->slidingThreshold;
}

// HomeostaticPlasticity implementation
struct HomeostaticPlasticity::Impl {
    float targetFiringRate;
    float plasticityExponent;
    float learningRate;
    float regulationStrength;
    float currentFiringRate;
    float averageFiringRate;
    float firingRateVariance;
    bool synapticScalingEnabled;
    bool intrinsicPlasticityEnabled;
    
    Impl() : targetFiringRate(0.1f), plasticityExponent(2.0f), learningRate(0.01f),
             regulationStrength(1.0f), currentFiringRate(0.0f),
             averageFiringRate(0.0f), firingRateVariance(0.0f),
             synapticScalingEnabled(true), intrinsicPlasticityEnabled(true) {}
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
        float scalingFactor = pImpl->targetFiringRate / std::max(pImpl->averageFiringRate, 0.001f);
        homeostaticDelta += pImpl->learningRate * (scalingFactor - 1.0f);
    }
    
    if (pImpl->intrinsicPlasticityEnabled) {
        // Intrinsic plasticity: adjust intrinsic excitability
        float rateError = pImpl->currentFiringRate - pImpl->targetFiringRate;
        float plasticityChange = pImpl->learningRate * rateError * pImpl->regulationStrength;
        
        // Apply power-law dependence
        homeostaticDelta += plasticityChange * std::pow(std::abs(rateError), pImpl->plasticityExponent);
    }
    
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

void HomeostaticPlasticity::setRegulationStrength(float strength) {
    pImpl->regulationStrength = std::max(0.0f, strength);
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

float HomeostaticPlasticity::getRegulationStrength() const {
    return pImpl->regulationStrength;
}

void HomeostaticPlasticity::setSynapticScaling(bool enable) {
    pImpl->synapticScalingEnabled = enable;
}

void HomeostaticPlasticity::setIntrinsicPlasticity(bool enable) {
    pImpl->intrinsicPlasticityEnabled = enable;
}

bool HomeostaticPlasticity::isSynapticScalingEnabled() const {
    return pImpl->synapticScalingEnabled;
}

bool HomeostaticPlasticity::isIntrinsicPlasticityEnabled() const {
    return pImpl->intrinsicPlasticityEnabled;
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

float HomeostaticPlasticity::getAverageFiringRate() const {
    return pImpl->averageFiringRate;
}

float HomeostaticPlasticity::getFiringRateVariance() const {
    return pImpl->firingRateVariance;
}

float HomeostaticPlasticity::getActivityFluctuation() const {
    return pImpl->firingRateVariance / std::max(pImpl->averageFiringRate, 0.001f);
}

} // namespace nlm


