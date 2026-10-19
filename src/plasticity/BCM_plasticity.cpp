#include "BCM_plasticity.hpp"
#include "../../brain/Synapse.hpp"
#include <cmath>
#include <algorithm>
#include <numeric>

namespace nlm
{

// BCMPlasticity implementation
struct BCMPlasticity::Impl {
    float theta;                    // Sliding threshold (θ)
    float sigma;                    // Threshold sensitivity (σ)
    float learningRate;             // Learning rate
    float maxWeight;                // Maximum weight
    float minWeight;                // Minimum weight
    float thetaAdaptationRate;      // Rate of threshold adaptation
    float slidingThreshold;         // Current BCM threshold (θ)
    float postSynapticGain;         // Post-synaptic gain
    
    bool antiHebbianEnabled;        // Anti-Hebbian BCM enabled
    bool homeostaticEnabled;        // Homeostatic BCM enabled
    bool neuromodulatorEnabled;      // Neuromodulator BCM enabled
    
    float averagePostActivity;       // Average post-synaptic activity
    float thetaHistory;              // Theta history for volatility calculation
    float bcmLearningRate;           // Adaptive BCM learning rate
    
    Impl() : theta(0.5f), sigma(0.1f), learningRate(0.01f),
             maxWeight(1.0f), minWeight(-1.0f), thetaAdaptationRate(0.001f),
             slidingThreshold(0.5f), postSynapticGain(1.0f),
             antiHebbianEnabled(false), homeostaticEnabled(false),
             neuromodulatorEnabled(false),
             averagePostActivity(0.0f), thetaHistory(0.0f),
             bcmLearningRate(0.01f) {}
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
    
    // Update average post-synaptic activity
    pImpl->averagePostActivity = 0.9f * pImpl->averagePostActivity + 0.1f * postRate;
    
    // Update sliding threshold
    updateSlidingThreshold(postRate);
    
    // Calculate BCM change
    float delta = calculateBCMChange(postRate, pImpl->slidingThreshold);
    
    // Apply anti-Hebbian if enabled
    if (pImpl->antiHebbianEnabled) {
        float antiHebbianDelta = calculateAntiHebbianBCMChange(postRate, pImpl->slidingThreshold);
        delta += antiHebbianDelta;
    }
    
    // Apply homeostatic if enabled
    if (pImpl->homeostaticEnabled) {
        float homeostaticDelta = calculateHomeostaticBCMChange(postRate, pImpl->slidingThreshold);
        delta += homeostaticDelta;
    }
    
    // Apply neuromodulator if enabled
    if (pImpl->neuromodulatorEnabled) {
        float neuromodulatorDelta = calculateNeuromodulatorBCMChange(postRate, pImpl->slidingThreshold);
        delta += neuromodulatorDelta;
    }
    
    // Update theta history for volatility calculation
    pImpl->thetaHistory = 0.9f * pImpl->thetaHistory + 0.1f * std::abs(pImpl->slidingThreshold - pImpl->theta);
    
    // Update BCM learning rate based on stability
    pImpl->bcmLearningRate = learningRate * (1.0f - pImpl->thetaHistory * 2.0f);
    pImpl->bcmLearningRate = std::clamp(pImpl->bcmLearningRate, 0.001f, 0.05f);
    
    // Apply weight change
    float scaledDelta = delta * pImpl->bcmLearningRate;
    applyWeightChange(synapse, scaledDelta);
    
    // Record BCM event
    recordBCMEvent(postRate, pImpl->slidingThreshold, scaledDelta);
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

void BCMPlasticity::setPostSynapticGain(float gain) {
    pImpl->postSynapticGain = std::clamp(gain, 0.1f, 10.0f);
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

float BCMPlasticity::getPostSynapticGain() const {
    return pImpl->postSynapticGain;
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

void BCMPlasticity::enableAntiHebbianBCM(bool enable) {
    pImpl->antiHebbianEnabled = enable;
}

void BCMPlasticity::enableHomeostaticBCM(bool enable) {
    pImpl->homeostaticEnabled = enable;
}

void BCMPlasticity::enableNeuromodulatorBCM(bool enable) {
    pImpl->neuromodulatorEnabled = enable;
}

bool BCMPlasticity::isAntiHebbianBCMEnabled() const {
    return pImpl->antiHebbianEnabled;
}

bool BCMPlasticity::isHomeostaticBCMEnabled() const {
    return pImpl->homeostaticEnabled;
}

bool BCMPlasticity::isNeuromodulatorBCMEnabled() const {
    return pImpl->neuromodulatorEnabled;
}

void BCMPlasticity::recordBCMEvent(float postActivity, float theta, float delta) {
    // Record BCM event for statistics
}

float BCMPlasticity::getAveragePostActivity() const {
    return pImpl->averagePostActivity;
}

float BCMPlasticity::getThetaVolatility() const {
    return pImpl->thetaHistory;
}

float BCMPlasticity::getBCMLearningRate() const {
    return pImpl->bcmLearningRate;
}

// AntiHebbianBCM implementation
struct AntiHebbianBCM::Impl {
    float antiHebbianRate;
    float antiHebbianThreshold;
    float antiHebbianGain;
    
    Impl() : antiHebbianRate(0.005f), antiHebbianThreshold(0.2f), antiHebbianGain(1.0f) {}
};

AntiHebbianBCM::AntiHebbianBCM() : pImpl(std::make_unique<Impl>()) {}

AntiHebbianBCM::~AntiHebbianBCM() = default;

void AntiHebbianBCM::update(Synapse* synapse,
                            const std::vector<Timestamp>& preSpikes,
                            const std::vector<Timestamp>& postSpikes,
                            TimestepDuration dt) {
    if (!synapse) return;
    
    float delta = calculateAntiHebbianBCMChange(0.0f, 0.0f); // Placeholder
    applyWeightChange(synapse, delta);
}

void AntiHebbianBCM::applyWeightChange(Synapse* synapse, SynapticWeight delta) {
    if (!synapse) return;
    
    float newWeight = synapse->getWeight() + delta;
    newWeight = std::clamp(newWeight, -1.0f, 1.0f);
    synapse->setWeight(newWeight);
}

const char* AntiHebbianBCM::getName() const {
    return "AntiHebbianBCM";
}

void AntiHebbianBCM::setAntiHebbianRate(float rate) {
    pImpl->antiHebbianRate = std::clamp(rate, 0.0f, 1.0f);
}

void AntiHebbianBCM::setAntiHebbianThreshold(float threshold) {
    pImpl->antiHebbianThreshold = std::clamp(threshold, 0.0f, 1.0f);
}

void AntiHebbianBCM::setAntiHebbianGain(float gain) {
    pImpl->antiHebbianGain = std::clamp(gain, 0.0f, 2.0f);
}

float AntiHebbianBCM::getAntiHebbianRate() const {
    return pImpl->antiHebbianRate;
}

float AntiHebbianBCM::getAntiHebbianThreshold() const {
    return pImpl->antiHebbianThreshold;
}

float AntiHebbianBCM::getAntiHebbianGain() const {
    return pImpl->antiHebbianGain;
}

float AntiHebbianBCM::calculateAntiHebbianBCMChange(float postActivity, float theta) const {
    // Anti-Hebbian BCM component
    float antiHebbianChange = -pImpl->antiHebbianRate * postActivity;
    return antiHebbianChange;
}

// HomeostaticBCM implementation
struct HomeostaticBCM::Impl {
    float homeostaticTarget;
    float homeostaticDeviation;
    float bcmHomeostaticRate;
    
    Impl() : homeostaticTarget(0.5f), homeostaticDeviation(0.1f), bcmHomeostaticRate(0.001f) {}
};

HomeostaticBCM::HomeostaticBCM() : pImpl(std::make_unique<Impl>()) {}

HomeostaticBCM::~HomeostaticBCM() = default;

void HomeostaticBCM::update(Synapse* synapse,
                             const std::vector<Timestamp>& preSpikes,
                             const std::vector<Timestamp>& postSpikes,
                             TimestepDuration dt) {
    if (!synapse) return;
    
    float delta = calculateHomeostaticBCMChange(0.0f, 0.0f); // Placeholder
    applyWeightChange(synapse, delta);
}

void HomeostaticBCM::applyWeightChange(Synapse* synapse, SynapticWeight delta) {
    if (!synapse) return;
    
    float newWeight = synapse->getWeight() + delta;
    newWeight = std::clamp(newWeight, -1.0f, 1.0f);
    synapse->setWeight(newWeight);
}

const char* HomeostaticBCM::getName() const {
    return "HomeostaticBCM";
}

void HomeostaticBCM::setHomeostaticTarget(float target) {
    pImpl->homeostaticTarget = std::clamp(target, 0.0f, 1.0f);
}

void HomeostaticBCM::setHomeostaticDeviation(float deviation) {
    pImpl->homeostaticDeviation = std::clamp(deviation, 0.0f, 1.0f);
}

void HomeostaticBCM::setBCMHomeostaticRate(float rate) {
    pImpl->bcmHomeostaticRate = std::clamp(rate, 0.0f, 0.01f);
}

float HomeostaticBCM::getHomeostaticTarget() const {
    return pImpl->homeostaticTarget;
}

float HomeostaticBCM::getHomeostaticDeviation() const {
    return pImpl->homeostaticDeviation;
}

float HomeostaticBCM::getBCMHomeostaticRate() const {
    return pImpl->bcmHomeostaticRate;
}

float HomeostaticBCM::calculateHomeostaticBCMChange(float postActivity, float theta) const {
    // Homeostatic BCM component
    float homeostaticChange = 0.0f;
    
    // Add homeostatic regulation
    homeostaticChange = pImpl->bcmHomeostaticRate * (pImpl->homeostaticTarget - postActivity);
    
    return homeostaticChange;
}

// NeuromodulatorBCM implementation
struct NeuromodulatorBCM::Impl {
    float dopamineLevel;
    float acetylcholineLevel;
    float norepinephrineLevel;
    float serotoninLevel;
    
    float dopamineBCMModulation;
    float acetylcholineBCMModulation;
    float norepinephrineBCMModulation;
    float serotoninBCMModulation;
    
    Impl() : dopamineLevel(1.0f), acetylcholineLevel(1.0f), norepinephrineLevel(1.0f),
             serotoninLevel(1.0f),
             dopamineBCMModulation(1.0f), acetylcholineBCMModulation(1.0f),
             norepinephrineBCMModulation(1.0f), serotoninBCMModulation(1.0f) {}
};

NeuromodulatorBCM::NeuromodulatorBCM() : pImpl(std::make_unique<Impl>()) {}

NeuromodulatorBCM::~NeuromodulatorBCM() = default;

void NeuromodulatorBCM::update(Synapse* synapse,
                               const std::vector<Timestamp>& preSpikes,
                               const std::vector<Timestamp>& postSpikes,
                               TimestepDuration dt) {
    if (!synapse) return;
    
    float delta = calculateNeuromodulatorBCMChange(0.0f, 0.0f); // Placeholder
    applyWeightChange(synapse, delta);
}

void NeuromodulatorBCM::applyWeightChange(Synapse* synapse, SynapticWeight delta) {
    if (!synapse) return;
    
    float newWeight = synapse->getWeight() + delta;
    newWeight = std::clamp(newWeight, -1.0f, 1.0f);
    synapse->setWeight(newWeight);
}

const char* NeuromodulatorBCM::getName() const {
    return "NeuromodulatorBCM";
}

void NeuromodulatorBCM::setDopamineLevel(float level) {
    pImpl->dopamineLevel = std::clamp(level, 0.0f, 2.0f);
}

void NeuromodulatorBCM::setAcetylcholineLevel(float level) {
    pImpl->acetylcholineLevel = std::clamp(level, 0.0f, 2.0f);
}

void NeuromodulatorBCM::setNorepinephrineLevel(float level) {
    pImpl->norepinephrineLevel = std::clamp(level, 0.0f, 2.0f);
}

void NeuromodulatorBCM::setSerotoninLevel(float level) {
    pImpl->serotoninLevel = std::clamp(level, 0.0f, 2.0f);
}

void NeuromodulatorBCM::setDopamineBCMModulation(float modulation) {
    pImpl->dopamineBCMModulation = std::clamp(modulation, 0.0f, 2.0f);
}

void NeuromodulatorBCM::setAcetylcholineBCMModulation(float modulation) {
    pImpl->acetylcholineBCMModulation = std::clamp(modulation, 0.0f, 2.0f);
}

void NeuromodulatorBCM::setNorepinephrineBCMModulation(float modulation) {
    pImpl->norepinephrineBCMModulation = std::clamp(modulation, 0.0f, 2.0f);
}

void NeuromodulatorBCM::setSerotoninBCMModulation(float modulation) {
    pImpl->serotoninBCMModulation = std::clamp(modulation, 0.0f, 2.0f);
}

float NeuromodulatorBCM::getDopamineLevel() const {
    return pImpl->dopamineLevel;
}

float NeuromodulatorBCM::getAcetylcholineLevel() const {
    return pImpl->acetylcholineLevel;
}

float NeuromodulatorBCM::getNorepinephrineLevel() const {
    return pImpl->norepinephrineLevel;
}

float NeuromodulatorBCM::getSerotoninLevel() const {
    return pImpl->serotoninLevel;
}

float NeuromodulatorBCM::getDopamineBCMModulation() const {
    return pImpl->dopamineBCMModulation;
}

float NeuromodulatorBCM::getAcetylcholineBCMModulation() const {
    return pImpl->acetylcholineBCMModulation;
}

float NeuromodulatorBCM::getNorepinephrineBCMModulation() const {
    return pImpl->norepinephrineBCMModulation;
}

float NeuromodulatorBCM::getSerotoninBCMModulation() const {
    return pImpl->serotoninBCMModulation;
}

float NeuromodulatorBCM::calculateNeuromodulatorBCMChange(float postActivity, float theta) const {
    // Neuromodulator-enhanced BCM component
    float neuromodulatorChange = 1.0f;
    
    // Dopamine modulation
    neuromodulatorChange *= pImpl->dopamineBCMModulation;
    
    // Acetylcholine modulation
    neuromodulatorChange *= (1.0f + (pImpl->acetylcholineBCMModulation - 1.0f) * 0.3f);
    
    // Norepinephrine modulation
    neuromodulatorChange *= (1.0f + (pImpl->norepinephrineBCMModulation - 1.0f) * 0.2f);
    
    // Serotonin modulation
    neuromodulatorChange *= (1.0f - 0.1f * (pImpl->serotoninBCMModulation - 1.0f));
    
    return neuromodulatorChange;
}

} // namespace nlm

