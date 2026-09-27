#include "STDP.hpp"
#include "../../brain/Synapse.hpp"
#include <cmath>
#include <algorithm>

namespace nlm {

STDP::STDP() : pImpl(new Impl) {}

STDP::~STDP() = default;

void STDP::update(Synapse* synapse,
                   const std::vector<Timestamp>& preSpikes,
                   const std::vector<Timestamp>& postSpikes,
                   TimestepDuration dt) {
    if (!synapse || preSpikes.empty() || postSpikes.empty()) {
        return;
    }
    
    float totalDelta = 0.0f;
    float tau = pImpl->timeConstant;
    
    // STDP implementation based on spike-timing correlation
    // For each pre-post spike pair:
    // - If pre before post (Δt > 0): LTP proportional to exp(-Δt/τ+)
    // - If post before pre (Δt < 0): LTD proportional to exp(Δt/τ-)
    
    for (Timestamp preTime : preSpikes) {
        for (Timestamp postTime : postSpikes) {
            float dt = static_cast<float>(postTime - preTime);  // Δt = t_post - t_pre
            
            if (dt > 0) {
                // Pre before post: Long-term potentiation (LTP)
                // Based on NMDA receptor activation timing
                float delta = pImpl->ltpWeight * std::exp(-dt / tau);
                totalDelta += delta;
            } else if (dt < 0) {
                // Post before pre: Long-term depression (LTD)
                // Based on backpropagating action potential interactions
                float delta = -pImpl->ltdWeight * std::exp(dt / tau);  // dt is negative
                totalDelta += delta;
            }
            // dt == 0: simultaneous spikes, minimal effect (rare in practice)
        }
    }
    
    // Apply weight change with biological constraints
    if (std::abs(totalDelta) > 1e-6f) {
        // Scale by synaptic efficacy for activity-dependent plasticity
        float efficacy = synapse->getEfficacy();
        totalDelta *= efficacy;
        
        // Apply the weight change
        applyWeightChange(synapse, totalDelta);
        
        // Update eligibility trace for reward-modulated learning
        float currentTrace = synapse->getEligibilityTrace();
        synapse->setEligibilityTrace(currentTrace + totalDelta);
        
        // Decay eligibility trace for next update
        synapse->decayEligibilityTrace(0.1f);  // 10% decay per update
    }
}

void STDP::applyWeightChange(Synapse* synapse, SynapticWeight delta) {
    if (!synapse) return;
    
    // Clamp weight to biological bounds
    float newWeight = synapse->getWeight() + delta;
    newWeight = std::clamp(newWeight, pImpl->minWeight, pImpl->maxWeight);
    synapse->setWeight(newWeight);
}

const char* STDP::getName() const {
    return "STDP";
}

void STDP::setLTPWeight(float weight) {
    pImpl->ltpWeight = std::clamp(weight, 0.0f, 1.0f);
}

float STDP::getLTPWeight() const {
    return pImpl->ltpWeight;
}

void STDP::setLTDWeight(float weight) {
    pImpl->ltdWeight = std::clamp(weight, 0.0f, 1.0f);
}

float STDP::getLTDWeight() const {
    return pImpl->ltdWeight;
}

void STDP::setTimeConstant(float tau) {
    pImpl->timeConstant = std::clamp(tau, 1.0f, 100.0f);
}

float STDP::getTimeConstant() const {
    return pImpl->timeConstant;
}

void STDP::configure(float ltpWeight, float ltdWeight, float tau) {
    setLTPWeight(ltpWeight);
    setLTDWeight(ltdWeight);
    setTimeConstant(tau);
}

} // namespace nlm
