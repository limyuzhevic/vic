#include "SynapseCore.hpp"
#include "../core/Random/Random.hpp"

namespace nlm {

void SynapseCore::setWeight(SynapticWeight weight) {
    pImpl->weight = weight;
}

void SynapseCore::addToWeight(SynapticWeight delta) {
    pImpl->weight += delta;
    // Clamp to reasonable bounds to prevent instability
    pImpl->weight = std::clamp(pImpl->weight, Impl::MIN_WEIGHT, Impl::MAX_WEIGHT);
}

void SynapseCore::setDelay(Delay delay) {
    pImpl->delay = delay;
}

void SynapseCore::setType(SynapseType type) {
    pImpl->type = type;
}

bool SynapseCore::isExcitatory() const {
    return pImpl->type == SynapseType::Excitatory;
}

bool SynapseCore::isInhibitory() const {
    return pImpl->type == SynapseType::Inhibitory;
}

void SynapseCore::setEfficacy(float efficacy) {
    pImpl->efficacy = std::clamp(efficacy, 0.0f, 2.0f);
}

float SynapseCore::getEfficacy() const {
    return pImpl->efficacy;
}

void SynapseCore::reset() {
    pImpl->weight = 0.0f;
    pImpl->preSpikeHistory.clear();
    pImpl->postSpikeHistory.clear();
    pImpl->eligibilityTrace = 0.0f;
    pImpl->efficacy = 1.0f;
    pImpl->shortTermDepression = 1.0f;
    pImpl->shortTermFacilitation = 0.0f;
    pImpl->lastPreSpikeTime = -1.0f;
    pImpl->lastPostSpikeTime = -1.0f;
}

void SynapseCore::initializeRandom(RandomGenerator& rng) {
    // Proper random initialization based on synapse type
    if (pImpl->type == SynapseType::Excitatory) {
        // Excitatory synapses: small positive weights
        pImpl->weight = rng.uniformReal(0.1f, 0.4f);
        // Excitatory synapses have moderate initial efficacy
        pImpl->efficacy = rng.uniformReal(0.8f, 1.0f);
    } else if (pImpl->type == SynapseType::Inhibitory) {
        // Inhibitory synapses: negative weights
        pImpl->weight = -rng.uniformReal(0.1f, 0.4f);
        pImpl->efficacy = rng.uniformReal(0.8f, 1.0f);
    } else {
        // Other types: small random weights
        pImpl->weight = rng.uniformReal(-0.1f, 0.1f);
        pImpl->efficacy = rng.uniformReal(0.9f, 1.0f);
    }
    
    // Random delay: 1-5 steps (1-5ms at 1ms timestep)
    pImpl->delay = static_cast<Delay>(rng.uniformInt(1, 5));
    
    // Initialize short-term plasticity state
    pImpl->shortTermDepression = 1.0f;  // Fully recovered
    pImpl->shortTermFacilitation = 0.0f;  // No initial facilitation
    
    // Initialize eligibility trace to 0
    pImpl->eligibilityTrace = 0.0f;
    
    // Initialize last spike times
    pImpl->lastPreSpikeTime = -1.0f;
    pImpl->lastPostSpikeTime = -1.0f;
}

} // namespace nlm

#include "SynapseSTDP.hpp"

namespace nlm {

void SynapseSTDP::recordPreSpike(Timestamp timestamp) {
    pImpl->preSpikeHistory.push_back(timestamp);
    if (pImpl->preSpikeHistory.size() > Impl::MAX_SPIKE_HISTORY) {
        pImpl->preSpikeHistory.erase(pImpl->preSpikeHistory.begin());
    }
}

void SynapseSTDP::recordPostSpike(Timestamp timestamp) {
    pImpl->postSpikeHistory.push_back(timestamp);
    if (pImpl->postSpikeHistory.size() > Impl::MAX_SPIKE_HISTORY) {
        pImpl->postSpikeHistory.erase(pImpl->postSpikeHistory.begin());
    }
}

void SynapseSTDP::clearHistory() {
    pImpl->preSpikeHistory.clear();
    pImpl->postSpikeHistory.clear();
}

void SynapseSTDP::enablePlasticity(bool enable) {
    pImpl->plasticityFlags.stdp = enable;
}

void SynapseSTDP::update(Synapse* syn, const std::vector<Timestamp>& preSpikes, const std::vector<Timestamp>& postSpikes, TimestepDuration dt) {
    // STDP learning rule implementation
    // This is a simplified version - real STDP would depend on spike timing differences
    // For now, just update the synapse weight based on spike history
    if (!preSpikes.empty() && !postSpikes.empty()) {
        // Calculate weight change based on spike timing
        float weightChange = 0.0f;
        for (size_t i = 0; i < preSpikes.size(); ++i) {
            for (size_t j = 0; j < postSpikes.size(); ++j) {
                float timeDiff = static_cast<float>(postSpikes[j] - preSpikes[i]);
                if (timeDiff > 0) {
                    // LTP for pre before post
                    weightChange += 0.001f * std::exp(-timeDiff / 20.0f);
                } else {
                    // LTD for post before pre
                    weightChange -= 0.0012f * std::exp(timeDiff / 20.0f);
                }
            }
        }
        float weight = syn->getWeight();
        weight += weightChange;
        syn->setWeight(weight);
    }
}

void SynapseSTDP::reset() {
    pImpl->preSpikeHistory.clear();
    pImpl->postSpikeHistory.clear();
}

const std::vector<Timestamp>& SynapseSTDP::getPreSpikeHistory() const {
    return pImpl->preSpikeHistory;
}

const std::vector<Timestamp>& SynapseSTDP::getPostSpikeHistory() const {
    return pImpl->postSpikeHistory;
}

} // namespace nlm

#include "SynapseHebbian.hpp"

namespace nlm {

void SynapseHebbian::recordPreSpike(Timestamp timestamp) {
    pImpl->preSpikeHistory.push_back(timestamp);
    if (pImpl->preSpikeHistory.size() > Impl::MAX_SPIKE_HISTORY) {
        pImpl->preSpikeHistory.erase(pImpl->preSpikeHistory.begin());
    }
}

void SynapseHebbian::recordPostSpike(Timestamp timestamp) {
    pImpl->postSpikeHistory.push_back(timestamp);
    if (pImpl->postSpikeHistory.size() > Impl::MAX_SPIKE_HISTORY) {
        pImpl->postSpikeHistory.erase(pImpl->postSpikeHistory.begin());
    }
}

void SynapseHebbian::clearHistory() {
    pImpl->preSpikeHistory.clear();
    pImpl->postSpikeHistory.clear();
}

void SynapseHebbian::enablePlasticity(bool enable) {
    pImpl->plasticityFlags.hebbian = enable;
}

void SynapseHebbian::update(Synapse* syn, const std::vector<Timestamp>& preSpikes, const std::vector<Timestamp>& postSpikes, TimestepDuration dt) {
    // Hebbian learning rule: strengthen synapses when both pre and post neurons fire
    if (!preSpikes.empty() && !postSpikes.empty()) {
        float weight = syn->getWeight();
        // Increase weight based on concurrent activity
        float weightIncrease = 0.0005f * static_cast<float>(preSpikes.size() * postSpikes.size());
        weight += weightIncrease;
        syn->setWeight(weight);
    }
}

void SynapseHebbian::reset() {
    pImpl->preSpikeHistory.clear();
    pImpl->postSpikeHistory.clear();
}

} // namespace nlm

#include "SynapseShortTermPlasticity.hpp"

namespace nlm {

void SynapseShortTermPlasticity::step(Timestamp currentTime) {
    // Real synaptic dynamics:
    // 1. Decay short-term facilitation (Tsodyks-Markram model)
    // 2. Decay short-term depression
    
    // Decay short-term facilitation (Tsodyks-Markram model)
    if (pImpl->lastPreSpikeTime >= 0.0f) {
        float timeSincePre = static_cast<float>(currentTime - pImpl->lastPreSpikeTime);
        pImpl->shortTermFacilitation *= std::exp(-timeSincePre / Impl::STP_FACILITATION_TAU);
    }
    
    // Decay short-term depression
    if (pImpl->lastPostSpikeTime >= 0.0f || pImpl->lastPreSpikeTime >= 0.0f) {
        float timeSinceActivity = std::max(
            pImpl->lastPostSpikeTime >= 0.0f ? static_cast<float>(currentTime - pImpl->lastPostSpikeTime) : 0.0f,
            pImpl->lastPreSpikeTime >= 0.0f ? static_cast<float>(currentTime - pImpl->lastPreSpikeTime) : 0.0f
        );
        // Recovery from depression toward 1.0
        pImpl->shortTermDepression += (1.0f - pImpl->shortTermDepression) * (1.0f - std::exp(-timeSinceActivity / Impl::STP_DEPRESSION_TAU));
    }
}

void SynapseShortTermPlasticity::reset() {
    pImpl->shortTermDepression = 1.0f;  // Fully recovered
    pImpl->shortTermFacilitation = 0.0f;  // No initial facilitation
    pImpl->lastPreSpikeTime = -1.0f;
    pImpl->lastPostSpikeTime = -1.0f;
}

} // namespace nlm

#include "SynapseEligibility.hpp"

namespace nlm {

void SynapseEligibility::recordPreSpike(Timestamp timestamp) {
    pImpl->preSpikeHistory.push_back(timestamp);
    if (pImpl->preSpikeHistory.size() > Impl::MAX_SPIKE_HISTORY) {
        pImpl->preSpikeHistory.erase(pImpl->preSpikeHistory.begin());
    }
    // Update eligibility trace based on pre-synaptic spike
    pImpl->eligibilityTrace += 0.1f;
}

void SynapseEligibility::recordPostSpike(Timestamp timestamp) {
    pImpl->postSpikeHistory.push_back(timestamp);
    if (pImpl->postSpikeHistory.size() > Impl::MAX_SPIKE_HISTORY) {
        pImpl->postSpikeHistory.erase(pImpl->postSpikeHistory.begin());
    }
    // Update eligibility trace based on post-synaptic spike
    pImpl->eligibilityTrace += 0.1f;
}

void SynapseEligibility::clearHistory() {
    pImpl->preSpikeHistory.clear();
    pImpl->postSpikeHistory.clear();
    pImpl->eligibilityTrace = 0.0f;
}

void SynapseEligibility::enablePlasticity(bool enable) {
    // Reward-modulated plasticity flag
}

void SynapseEligibility::setEligibilityTrace(float trace) {
    pImpl->eligibilityTrace = trace;
}

float SynapseEligibility::getEligibilityTrace() const {
    return pImpl->eligibilityTrace;
}

void SynapseEligibility::decayEligibilityTrace(float decayRate) {
    pImpl->eligibilityTrace *= (1.0f - decayRate);
    if (std::abs(pImpl->eligibilityTrace) < 0.001f) {
        pImpl->eligibilityTrace = 0.0f;
    }
}

void SynapseEligibility::step(Timestamp currentTime) {
    // Decay eligibility trace for reward-modulated learning
    decayEligibilityTrace(0.001f);  // Fast decay
}

void SynapseEligibility::reset() {
    pImpl->preSpikeHistory.clear();
    pImpl->postSpikeHistory.clear();
    pImpl->eligibilityTrace = 0.0f;
}

} // namespace nlm