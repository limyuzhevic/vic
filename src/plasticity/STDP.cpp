#include "STDP.hpp"
#include "../../brain/Synapse.hpp"
#include <cmath>
#include <algorithm>
#include <vector>
#include <numeric>

namespace nlm {

struct STDP::Impl {
    float ltpWeight;      // A+ for potentiation
    float ltdWeight;      // A- for depression
    float timeConstant;   // Tau for exponential window (ms)
    float minWeight;      // Minimum synaptic weight
    float maxWeight;      // Maximum synaptic weight
    float rewardModulation;    // Reward modulation strength
    float homeostaticTarget;  // Target firing rate for homeostatic plasticity
    float metaplasticRate;     // Metaplasticity rate
    
    Impl() : ltpWeight(0.01f), ltdWeight(0.012f), timeConstant(20.0f),
             minWeight(-1.0f), maxWeight(1.0f), rewardModulation(1.0f),
             homeostaticTarget(0.02f), metaplasticRate(0.01f) {}
};

STDP::STDP() : pImpl(new Impl) {}

STDP::~STDP() = default;

void STDP::update(Synapse* synapse,
                   const std::vector<Timestamp>& preSpikes,
                   const std::vector<Timestamp>& postSpikes,
                   TimestepDuration dt) {
    /*
     * Real STDP implementation based on spike-timing correlation
     * 
     * Mathematical formulation:
     * For each pre-post spike pair with timing difference Δt = t_post - t_pre:
     * 
     * If Δt > 0 (pre before post): POTENTIATION
     *   Δw = A+ * exp(-Δt / τ+)
     *   
     * If Δt < 0 (post before pre): DEPRESSION
     *   Δw = A- * exp(Δt / τ-)
     * 
     * Where:
     *   A+ = ltpWeight (potentiation amplitude)
     *   A- = ltdWeight (depression amplitude)  
     *   τ+ = τ- = timeConstant (STDP time window)
     * 
     * Biological inspiration:
     *   - Reflects NMDA receptor-mediated calcium signaling
     *   - Pre-before-post activates NMDA receptors when postsynaptic spikes
     *   - Post-before-pre causes backpropagating action potentials
     *   
     * Limitations:
     *   - Simplified pairwise rule (doesn't capture triplet interactions)
     */
    
    float totalDelta = 0.0f;
    
    // Apply reward modulation factor
    float rewardFactor = 1.0f + pImpl->rewardModulation * 0.1f;
    
    // Apply STDP for each pre-post spike pair
    for (const auto& preTime : preSpikes) {
        for (const auto& postTime : postSpikes) {
            float deltaT = postTime - preTime;  // Δt = t_post - t_pre
            
            if (deltaT > 0.0f) {
                // Pre-before-post: potentiation (LTP)
                float weightChange = pImpl->ltpWeight * std::exp(-deltaT / pImpl->timeConstant);
                totalDelta += weightChange;
            } else if (deltaT < 0.0f) {
                // Post-before-pre: depression (LTD)
                float weightChange = -pImpl->ltdWeight * std::exp(deltaT / pImpl->timeConstant);
                totalDelta += weightChange;
            }
        }
    }
    
    // Apply homeostatic regulation
    float homeostaticDelta = calculateHomeostaticCorrection(synapse, dt);
    totalDelta += homeostaticDelta * rewardFactor;
    
    // Apply metaplasticity
    float metaplasticDelta = calculateMetaplasticCorrection(synapse, totalDelta);
    totalDelta += metaplasticDelta;
    
    // Clamp to weight bounds
    float currentWeight = synapse->getWeight();
    float newWeight = std::clamp(currentWeight + totalDelta, pImpl->minWeight, pImpl->maxWeight);
    
    // Apply the weight change
    synapse->setWeight(newWeight);
    
    // Update eligibility trace for future plasticity
    synapse->setEligibilityTrace(std::min(synapse->getEligibilityTrace() + 0.1f, 1.0f));
}

void STDP::applyWeightChange(Synapse* synapse, SynapticWeight delta) {
    float currentWeight = synapse->getWeight();
    float newWeight = std::clamp(currentWeight + delta, pImpl->minWeight, pImpl->maxWeight);
    synapse->setWeight(newWeight);
}

const char* STDP::getName() const {
    return "STDP";
}

void STDP::setLTPWeight(float weight) {
    pImpl->ltpWeight = std::max(0.0f, weight);
}

float STDP::getLTPWeight() const {
    return pImpl->ltpWeight;
}

void STDP::setLTDWeight(float weight) {
    pImpl->ltdWeight = std::max(0.0f, weight);
}

float STDP::getLTDWeight() const {
    return pImpl->ltdWeight;
}

void STDP::setTimeConstant(float tau) {
    pImpl->timeConstant = std::max(1.0f, std::min(tau, 1000.0f));
}

float STDP::getTimeConstant() const {
    return pImpl->timeConstant;
}

void STDP::configure(float ltpWeight, float ltdWeight, float tau) {
    setLTPWeight(ltpWeight);
    setLTDWeight(ltdWeight);
    setTimeConstant(tau);
}

void STDP::setRewardModulation(float strength) {
    pImpl->rewardModulation = std::max(0.0f, std::min(strength, 10.0f));
}

float STDP::getRewardModulation() const {
    return pImpl->rewardModulation;
}

void STDP::setHomeostaticTarget(float target) {
    pImpl->homeostaticTarget = std::max(0.0f, std::min(target, 0.1f));
}

float STDP::getHomeostaticTarget() const {
    return pImpl->homeostaticTarget;
}

void STDP::setMetaplasticRate(float rate) {
    pImpl->metaplasticRate = std::max(0.0f, std::min(rate, 0.1f));
}

float STDP::getMetaplasticRate() const {
    return pImpl->metaplasticRate;
}

// Helper functions for STDP
float STDP::calculateHomeostaticCorrection(Synapse* synapse, TimestepDuration dt) {
    // Calculate firing rate from spike history
    float firingRate = 0.0f;
    const auto& postSpikes = synapse->getPostSpikeHistory();
    
    if (!postSpikes.empty()) {
        Timestamp firstSpike = postSpikes.front();
        Timestamp lastSpike = postSpikes.back();
        firingRate = static_cast<float>(postSpikes.size()) / (lastSpike - firstSpike);
    }
    
    // Homeostatic correction towards target rate
    float homeostaticCorrection = (firingRate - pImpl->homeostaticTarget) * pImpl->metaplasticRate;
    return homeostaticCorrection;
}

float STDP::calculateMetaplasticCorrection(Synapse* synapse, float delta) {
    // Metaplasticity that reduces future plasticity with large weight changes
    float eligibilityTrace = synapse->getEligibilityTrace();
    float metaplasticCorrection = -delta * pImpl->metaplasticRate * eligibilityTrace;
    
    // Decay eligibility trace
    synapse->decayEligibilityTrace(0.01f);
    
    return metaplasticCorrection;
}

} // namespace nlm
