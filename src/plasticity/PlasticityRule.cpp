#include "PlasticityRule.hpp"
#include "../core/Logger/Logger.hpp"
#include <cmath>
#include <algorithm>

namespace nlm {

bool PlasticityRule::isEnabled() const {
    return enabled_;
}

void PlasticityRule::setEnabled(bool enabled) {
    enabled_ = enabled;
}

void PlasticityRule::setNeuromodulator(const Neuromodulator& neuromodulator) {
    neuromodulator_ = &neuromodulator;
}

float PlasticityRule::getNeuromodulatorInfluence() const {
    if (neuromulator_ && neuromodulator_ != &nullNeuromodulator) {
        neuromodulatorInfluence_ = neuromodulator_->getPlasticityFactor();
    }
    return neuromodulatorInfluence_;
}

void PlasticityRule::applyCommonNeuromodulationFactors(SynapticWeight& delta,
                                                     const NeuromodulationModifier& modifier) const {
    // Apply multiple neuromodulation factors to delta
    float factor = 1.0f;
    
    // Base neuromodulator influence
    factor *= modifier.neuromodulatorMod;
    
    // Learning progress effect
    factor *= (1.0f + modifier.learningProgress * 0.2f);
    
    // Attention modulation
    factor *= modifier.attentionMod;
    
    // Motivation modulation
    factor *= modifier.motivationMod;
    
    // Confidence effect
    factor *= modifier.confidence;
    
    // Eligibility trace effect
    factor *= (1.0f + modifier.eligibilityTrace * 0.5f);
    
    // Temporal difference effect
    factor *= (1.0f + modifier.temporalDifference * 0.3f);
    
    // Apply factor to delta
    delta = delta * factor;
}

float PlasticityRule::calculateRewardPredictionErrorModulation(const NeuromodulationModifier& modifier) const {
    // Calculate RPE-based modulation
    float baseMod = 1.0f;
    
    // Neuromodulator influences on RPE
    float dopamineMod = modifier.neuromodulatorMod * 0.5f;
    float serotoninMod = (1.0f - modifier.learningProgress) * 0.3f; // Serotonin inversely related to learning
    float curiosityMod = modifier.attentionMod * 0.2f;
    
    float totalMod = baseMod + dopamineMod + serotoninMod + curiosityMod;
    
    return std::clamp(totalMod, 0.5f, 2.0f);
}

// Helper variable for null neuromodulator
const Neuromodulator& nullNeuromodulator = *new Neuromodulator(); // This will fail compilation, needs better design

struct HebbianRule::Impl {
    float learningRate;
    float noveltySensitivity;
    float curiositySensitivity;
    
    Impl() : learningRate(0.01f), noveltySensitivity(0.5f), curiositySensitivity(0.3f) {}
};

HebbianRule::HebbianRule() : pImpl(new Impl) {}

HebbianRule::~HebbianRule() = default;

void HebbianRule::update(Synapse* synapse,
                         const std::vector<Timestamp>& preSpikes,
                         const std::vector<Timestamp>& postSpikes,
                         TimestepDuration dt,
                         const NeuromodulationModifier& modifier) {
    if (preSpikes.empty() || postSpikes.empty()) {
        return;
    }
    
    // Calculate eligibility traces for pre and post spikes
    float eligibilityTrace = calculateEligibilityTrace(postSpikes.back(), 
                                                      static_cast<Timestamp>(0), 0.01f);
    
    // Count coincident spikes (simplified)
    size_t coincidences = 0;
    for (Timestamp pre : preSpikes) {
        for (Timestamp post : postSpikes) {
            if (std::abs(pre - post) < 10.0) {  // 10ms window
                ++coincidences;
            }
        }
    }
    
    // Calculate base weight change
    float baseDelta = pImpl->learningRate * static_cast<float>(coincidences);
    
    // Apply neuromodulation factors
    float plasticityFactor = getPlasticityFactor(modifier);
    float rewardModulation = calculateRewardPredictionErrorModulation(modifier);
    
    // Apply eligibility trace
    baseDelta *= eligibilityTrace;
    
    // Apply neuromodulation
    baseDelta *= plasticityFactor;
    baseDelta *= rewardModulation;
    
    // Apply common neuromodulation factors
    applyCommonNeuromodulationFactors(baseDelta, modifier);
    
    // Apply weight change
    applyWeightChange(synapse, baseDelta, modifier);
}

void HebbianRule::applyWeightChange(Synapse* synapse, SynapticWeight delta,
                                   const NeuromodulationModifier& modifier) {
    // Apply novelty sensitivity
    float noveltyMod = 1.0f + pImpl->noveltySensitivity * modifier.neuromodulatorMod;
    
    // Apply curiosity sensitivity
    float curiosityMod = 1.0f + pImpl->curiositySensitivity * modifier.attentionMod;
    
    // Combined neuromodulation factors
    float totalMod = noveltyMod * curiosityMod;
    
    delta *= totalMod;
    synapse->addToWeight(delta);
}

const char* HebbianRule::getName() const {
    return "Hebbian";
}

const char* HebbianRule::getType() const {
    return "Hebbian";
}

void HebbianRule::setLearningRate(float rate) {
    pImpl->learningRate = std::max(0.0f, rate);
}

float HebbianRule::getLearningRate() const {
    return pImpl->learningRate;
}

void HebbianRule::setNoveltySensitivity(float sensitivity) {
    pImpl->noveltySensitivity = std::clamp(sensitivity, 0.0f, 1.0f);
}

float HebbianRule::getNoveltySensitivity() const {
    return pImpl->noveltySensitivity;
}

void HebbianRule::setCuriositySensitivity(float sensitivity) {
    pImpl->curiositySensitivity = std::clamp(sensitivity, 0.0f, 1.0f);
}

float HebbianRule::getCuriositySensitivity() const {
    return pImpl->curiositySensitivity;
}

float HebbianRule::calculateEligibilityTrace(const Timestamp& spikeTime,
                                            const Timestamp& currentTime,
                                            float decayRate) const {
    float elapsed = static_cast<float>(currentTime - spikeTime);
    return std::exp(-elapsed / decayRate);
}

float HebbianRule::getPlasticityFactor(const NeuromodulationModifier& modifier) const {
    // Advanced Hebbian plasticity with neuromodulation
    float baseFactor = 1.0f;
    
    // Neuromodulator influences
    float dopamineInf = modifier.neuromodulatorMod * 0.4f;
    float serotoninInf = (1.0f - modifier.learningProgress) * 0.3f;
    float curiosityInf = modifier.attentionMod * 0.2f;
    float noveltyInf = modifier.attentionMod * 0.2f;
    float endorphinsInf = (1.0f - modifier.motivationMod) * 0.1f;
    
    // Combine influences
    float totalInfluence = dopamineInf + serotoninInf + curiosityInf + noveltyInf + endorphinsInf;
    
    float plasticityFactor = baseFactor + totalInfluence;
    
    // Confidence modulation
    plasticityFactor *= modifier.confidence;
    
    return std::clamp(plasticityFactor, 0.5f, 3.0f);
}

void AntiHebbianRule::update(Synapse* synapse,
                             const std::vector<Timestamp>& preSpikes,
                             const std::vector<Timestamp>& postSpikes,
                             TimestepDuration dt,
                             const NeuromodulationModifier& modifier) {
    // Anti-Hebbian: decrease weight when neurons fire together
    if (preSpikes.empty() || postSpikes.empty()) {
        return;
    }
    
    // Calculate eligibility traces
    float eligibilityTrace = calculateEligibilityTrace(postSpikes.back(),
                                                      static_cast<Timestamp>(0), 0.01f);
    
    // Count coincident spikes
    size_t coincidences = 0;
    for (Timestamp pre : preSpikes) {
        for (Timestamp post : postSpikes) {
            if (std::abs(pre - post) < 10.0) {
                ++coincidences;
            }
        }
    }
    
    // Calculate base weight change (negative for anti-Hebbian)
    float baseDelta = -pImpl->learningRate * static_cast<float>(coincidences);
    
    // Apply neuromodulation
    float plasticityFactor = getPlasticityFactor(modifier);
    float rewardModulation = calculateRewardPredictionErrorModulation(modifier);
    
    baseDelta *= eligibilityTrace * plasticityFactor * rewardModulation;
    
    applyCommonNeuromodulationFactors(baseDelta, modifier);
    
    applyWeightChange(synapse, baseDelta, modifier);
}

void AntiHebbianRule::applyWeightChange(Synapse* synapse, SynapticWeight delta,
                                       const NeuromodulationModifier& modifier) {
    synapse->addToWeight(delta);
}

float AntiHebbianRule::calculateEligibilityTrace(const Timestamp& spikeTime,
                                                const Timestamp& currentTime,
                                                float decayRate) const {
    float elapsed = static_cast<float>(currentTime - spikeTime);
    return std::exp(-elapsed / decayRate);
}

float AntiHebbianRule::getPlasticityFactor(const NeuromodulationModifier& modifier) const {
    // Anti-Hebbian with neuromodulation
    float baseFactor = 0.5f; // Lower baseline for anti-Hebbian
    
    // Neuromodulator influences
    float dopamineInf = modifier.neuromodulatorMod * 0.3f;
    float serotoninInf = modifier.learningProgress * 0.4f; // Serotonin enhances anti-Hebbian
    float curiosityInf = modifier.attentionMod * 0.2f;
    
    float totalInfluence = dopamineInf + serotoninInf + curiosityInf;
    
    float plasticityFactor = baseFactor + totalInfluence;
    
    plasticityFactor *= modifier.confidence;
    
    return std::clamp(plasticityFactor, 0.2f, 2.0f);
}

struct BCMRule::Impl {
    float learningRate;
    float theta;           // BCM threshold
    float sigma;           // Modulation strength
    float noveltySensitivity;
    
    Impl() : learningRate(0.01f), theta(0.5f), sigma(0.1f),
             noveltySensitivity(0.4f) {}
};

BCMRule::BCMRule() : pImpl(new Impl) {}

BCMRule::~BCMRule() = default;

void BCMRule::update(Synapse* synapse,
                     const std::vector<Timestamp>& preSpikes,
                     const std::vector<Timestamp>& postSpikes,
                     TimestepDuration dt,
                     const NeuromodulationModifier& modifier) {
    if (preSpikes.empty() || postSpikes.empty()) {
        return;
    }
    
    // Calculate post-synaptic activity
    float postActivity = calculatePostSynapticActivity(postSpikes);
    
    // BCM modulation: weight change depends on post activity relative to theta
    float bcmMod = std::max(0.0f, postActivity - pImpl->theta);
    
    // Count pre-post coincidences
    size_t coincidences = calculateCoincidences(preSpikes, postSpikes);
    
    // Calculate base weight change
    float baseDelta = pImpl->learningRate * static_cast<float>(coincidences) * bcmMod;
    
    // Apply neuromodulation
    float plasticityFactor = getPlasticityFactor(modifier);
    float rewardModulation = calculateRewardPredictionErrorModulation(modifier);
    
    // Novelty sensitivity
    float noveltyMod = 1.0f + pImpl->noveltySensitivity * modifier.neuromodulatorMod;
    
    baseDelta *= plasticityFactor * rewardModulation * noveltyMod;
    
    applyCommonNeuromodulationFactors(baseDelta, modifier);
    
    applyWeightChange(synapse, baseDelta, modifier);
}

void BCMRule::applyWeightChange(Synapse* synapse, SynapticWeight delta,
                               const NeuromodulationModifier& modifier) {
    synapse->addToWeight(delta);
}

void BCMRule::setTheta(float theta) {
    pImpl->theta = std::clamp(theta, 0.1f, 2.0f);
}

void BCMRule::setSigma(float sigma) {
    pImpl->sigma = std::clamp(sigma, 0.0f, 1.0f);
}

void BCMRule::setLearningRate(float rate) {
    pImpl->learningRate = std::max(0.0f, rate);
}

float BCMRule::calculatePostSynapticActivity(const std::vector<Timestamp>& postSpikes) const {
    // Calculate average post-synaptic activity
    if (postSpikes.empty()) {
        return 0.0f;
    }
    
    float totalActivity = 0.0f;
    for (Timestamp spike : postSpikes) {
        totalActivity += static_cast<float>(spike);
    }
    
    return totalActivity / static_cast<float>(postSpikes.size());
}

float BCMRule::calculateCoincidences(const std::vector<Timestamp>& preSpikes,
                                    const std::vector<Timestamp>& postSpikes) const {
    size_t coincidences = 0;
    for (Timestamp pre : preSpikes) {
        for (Timestamp post : postSpikes) {
            if (std::abs(pre - post) < 10.0) {
                ++coincidences;
            }
        }
    }
    return static_cast<float>(coincidences);
}

float BCMRule::calculateEligibilityTrace(const Timestamp& spikeTime,
                                        const Timestamp& currentTime,
                                        float decayRate) const {
    float elapsed = static_cast<float>(currentTime - spikeTime);
    return std::exp(-elapsed / decayRate);
}

float BCMRule::getPlasticityFactor(const NeuromodulationModifier& modifier) const {
    // BCM rule with neuromodulation
    float baseFactor = 1.0f;
    
    // Neuromodulator influences
    float dopamineInf = modifier.neuromodulatorMod * 0.3f;
    float serotoninInf = modifier.learningProgress * 0.4f;
    float curiosityInf = modifier.attentionMod * 0.2f;
    
    float totalInfluence = dopamineInf + serotoninInf + curiosityInf;
    
    float plasticityFactor = baseFactor + totalInfluence;
    
    plasticityFactor *= modifier.confidence;
    
    return std::clamp(plasticityFactor, 0.5f, 3.0f);
}

struct RewardModulatedSTDP::Impl {
    float rewardWeight;
    float predictionErrorWeight;
    float learningProgressWeight;
    float baselineLTP;
    float baselineLTD;
    
    Impl() : rewardWeight(1.0f), predictionErrorWeight(1.0f),
             learningProgressWeight(0.5f), baselineLTP(0.01f), baselineLTD(0.012f) {}
};

RewardModulatedSTDP::RewardModulatedSTDP() : pImpl(new Impl) {}

RewardModulatedSTDP::~RewardModulatedSTDP() = default;

void RewardModulatedSTDP::update(Synapse* synapse,
                                 const std::vector<Timestamp>& preSpikes,
                                 const std::vector<Timestamp>& postSpikes,
                                 TimestepDuration dt,
                                 const NeuromodulationModifier& modifier) {
    if (preSpikes.empty() || postSpikes.empty()) {
        return;
    }
    
    // Calculate STDP contributions
    float stdpContribution = calculateSTDPContribution(preSpikes, postSpikes);
    
    // Apply reward modulation
    float rewardMod = 1.0f + modifier.neuromodulatorMod * pImpl->rewardWeight;
    rewardMod *= (1.0f + modifier.temporalDifference * pImpl->predictionErrorWeight);
    rewardMod *= (1.0f + modifier.learningProgress * pImpl->learningProgressWeight);
    
    // Calculate final weight change
    float delta = stdpContribution * rewardMod;
    
    applyCommonNeuromodulationFactors(delta, modifier);
    
    applyWeightChange(synapse, delta, modifier);
}

void RewardModulatedSTDP::applyWeightChange(Synapse* synapse, SynapticWeight delta,
                                           const NeuromodulationModifier& modifier) {
    synapse->addToWeight(delta);
}

void RewardModulatedSTDP::setRewardWeight(float weight) {
    pImpl->rewardWeight = std::clamp(weight, 0.0f, 2.0f);
}

void RewardModulatedSTDP::setPredictionErrorWeight(float weight) {
    pImpl->predictionErrorWeight = std::clamp(weight, 0.0f, 2.0f);
}

void RewardModulatedSTDP::setLearningProgressWeight(float weight) {
    pImpl->learningProgressWeight = std::clamp(weight, 0.0f, 1.0f);
}

float RewardModulatedSTDP::calculateSTDPContribution(const std::vector<Timestamp>& preSpikes,
                                                    const std::vector<Timestamp>& postSpikes) const {
    float stdp = 0.0f;
    
    // Calculate STDP window
    float dt = 0.0f;
    if (!preSpikes.empty() && !postSpikes.empty()) {
        dt = static_cast<float>(postSpikes.back() - preSpikes.front());
    }
    
    // STDP learning rules
    if (dt < 20.0f) { // LTP (post after pre)
        stdp = pImpl->baselineLTP * dt;
    } else if (dt > 20.0f) { // LTD (post before pre)
        stdp = -pImpl->baselineLTD * (dt - 40.0f);
    }
    
    return stdp;
}

float RewardModulatedSTDP::calculateEligibilityTrace(const Timestamp& spikeTime,
                                                    const Timestamp& currentTime,
                                                    float decayRate) const {
    float elapsed = static_cast<float>(currentTime - spikeTime);
    return std::exp(-elapsed / decayRate);
}

float RewardModulatedSTDP::getPlasticityFactor(const NeuromodulationModifier& modifier) const {
    // R-STDP with neuromodulation
    float baseFactor = 1.0f;
    
    // Neuromodulator influences
    float rewardInf = modifier.neuromodulatorMod * pImpl->rewardWeight;
    float curiosityInf = modifier.attentionMod * 0.3f;
    
    float totalInfluence = rewardInf + curiosityInf;
    
    float plasticityFactor = baseFactor + totalInfluence;
    
    plasticityFactor *= modifier.confidence;
    
    return std::clamp(plasticityFactor, 0.5f, 3.0f);
}

struct TripletSTDP::Impl {
    float baselineLTP1;
    float baselineLTD1;
    float baselineLTP2;
    float baselineLTD2;
    float tau1;
    float tau2;
    
    Impl() : baselineLTP1(0.5f), baselineLTD1(0.7f), baselineLTP2(0.3f),
             baselineLTD2(0.5f), tau1(20.0f), tau2(100.0f) {}
};

TripletSTDP::TripletSTDP() : pImpl(new Impl) {}

TripletSTDP::~TripletSTDP() = default;

void TripletSTDP::update(Synapse* synapse,
                         const std::vector<Timestamp>& preSpikes,
                         const std::vector<Timestamp>& postSpikes,
                         TimestepDuration dt,
                         const NeuromodulationModifier& modifier) {
    if (preSpikes.empty() || postSpikes.empty()) {
        return;
    }
    
    // Calculate triplet STDP contributions
    float tripletContribution = calculateTripletSTDPContribution(preSpikes, postSpikes);
    
    // Apply neuromodulation
    float neuromodulationFactor = 1.0f;
    neuromodulationFactor += modifier.neuromodulatorMod * 0.3f;
    neuromodulationFactor += modifier.attentionMod * 0.2f;
    neuromodulationFactor -= modifier.endorphinsMod * 0.1f;
    
    float delta = tripletContribution * neuromodulationFactor;
    
    applyCommonNeuromodulationFactors(delta, modifier);
    
    applyWeightChange(synapse, delta, modifier);
}

void TripletSTDP::applyWeightChange(Synapse* synapse, SynapticWeight delta,
                                   const NeuromodulationModifier& modifier) {
    synapse->addToWeight(delta);
}

float TripletSTDP::calculateTripletSTDPContribution(const std::vector<Timestamp>& preSpikes,
                                                   const std::vector<Timestamp>& postSpikes) const {
    float tripletSTDP = 0.0f;
    
    // Simplified triplet STDP calculation
    if (preSpikes.size() >= 2 && postSpikes.size() >= 1) {
        // Calculate time differences
        float t_pre1 = static_cast<float>(preSpikes[preSpikes.size() - 2]);
        float t_pre2 = static_cast<float>(preSpikes.back());
        float t_post = static_cast<float>(postSpikes.back());
        
        float dt1 = t_post - t_pre1; // First spike pair
        float dt2 = t_post - t_pre2; // Second spike pair
        
        // Triplet STDP formulas
        if (dt1 > 0.0f && dt1 < pImpl->tau1) {
            tripletSTDP += pImpl->baselineLTP1 * std::exp(-dt1 / pImpl->tau1);
        }
        
        if (dt2 > 0.0f && dt2 < pImpl->tau2) {
            tripletSTDP += pImpl->baselineLTP2 * std::exp(-dt2 / pImpl->tau2);
        }
        
        // LTD contributions from previous spikes
        if (dt1 > pImpl->tau1 && dt1 < 40.0f) {
            tripletSTDP -= pImpl->baselineLTD1 * std::exp(-(dt1 - pImpl->tau1) / 20.0f);
        }
        
        if (dt2 > pImpl->tau2 && dt2 < 100.0f) {
            tripletSTDP -= pImpl->baselineLTD2 * std::exp(-(dt2 - pImpl->tau2) / 50.0f);
        }
    }
    
    return tripletSTDP;
}

float TripletSTDP::calculateEligibilityTrace(const Timestamp& spikeTime,
                                            const Timestamp& currentTime,
                                            float decayRate) const {
    float elapsed = static_cast<float>(currentTime - spikeTime);
    return std::exp(-elapsed / decayRate);
}

float TripletSTDP::getPlasticityFactor(const NeuromodulationModifier& modifier) const {
    // Triplet STDP with neuromodulation
    float baseFactor = 1.0f;
    
    // Neuromodulator influences
    float dopamineInf = modifier.neuromodulatorMod * 0.2f;
    float curiosityInf = modifier.attentionMod * 0.3f;
    float noveltyInf = modifier.attentionMod * 0.1f;
    
    float totalInfluence = dopamineInf + curiosityInf + noveltyInf;
    
    float plasticityFactor = baseFactor + totalInfluence;
    
    plasticityFactor *= modifier.confidence;
    
    return std::clamp(plasticityFactor, 0.5f, 3.0f);
}

} // namespace nlm