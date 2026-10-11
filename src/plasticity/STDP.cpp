# STDP.cpp - Real Spike-Timing-Dependent Plasticity Implementation
// Implements the Phase 2 STDP learning rule
// Real biology: pre-before-post -> LTP, post-before-pre -> LTD
// This is one of the most important learning mechanisms in computational neuroscience

#include "STDP.hpp"
#include "../brain/Synapse.hpp"
#include <cmath>
#include <algorithm>
#include <numeric>

namespace nlm {

// STDP Implementation
// Spike-timing-dependent plasticity based on biological principles
// LTP (Long-Term Potentiation): pre fires before post -> weight increases
// LTD (Long-Term Depression): post fires before pre -> weight decreases
// Maximum weight change constrained to maintain stability

STDP::STDP() : pImpl(new Impl()) {
    // Initialize STDP parameters with biologically plausible values
    pImpl->ltpWeight = 0.02f;    // LTP magnitude (typically 0.01-0.03)
    pImpl->ltdWeight = 0.015f;   // LTD magnitude (typically 0.01-0.02)
    pImpl->timeConstant = 20.0f;  // Time constant for eligibility traces (ms)
    pImpl->minimumWeight = 0.01f; // Prevent zero weights
    pImpl->maximumWeight = 1.0f;  // Prevent runaway excitation
}

void STDP::configure(float ltpWeight, float ltdWeight, float tau) {
    // Configure STDP parameters with validation
    pImpl->ltpWeight = std::clamp(ltpWeight, 0.001f, 0.1f);
    pImpl->ltdWeight = std::clamp(ltdWeight, 0.001f, 0.1f);
    pImpl->timeConstant = std::clamp(tau, 1.0f, 100.0f);
}

void STDP::setLTPWeight(float weight) {
    pImpl->ltpWeight = std::clamp(weight, 0.001f, 0.1f);
}

void STDP::setLTDWeight(float weight) {
    pImpl->ltdWeight = std::clamp(weight, 0.001f, 0.1f);
}

void STDP::setTimeConstant(float tau) {
    pImpl->timeConstant = std::clamp(tau, 1.0f, 100.0f);
}

float STDP::getLTPWeight() const {
    return pImpl->ltpWeight;
}

float STDP::getLTDWeight() const {
    return pImpl->ltdWeight;
}

float STDP::getTimeConstant() const {
    return pImpl->timeConstant;
}

void STDP::update(Synapse* synapse, 
                  const std::vector<Timestamp>& preSpikes,
                  const std::vector<Timestamp>& postSpikes,
                  TimestepDuration dt) {
    if (!synapse) return;
    
    // Calculate weight change from all spike pairings
    SynapticWeight totalDelta = 0.0f;
    
    for (Timestamp preTime : preSpikes) {
        for (Timestamp postTime : postSpikes) {
            float deltaT = postTime - preTime;  // Positive if pre before post
            
            if (std::abs(deltaT) < pImpl->timeConstant * 10) {  // Within relevant window
                if (deltaT > 0) {
                    // Pre before post -> LTP (potentiate)
                    float ltp = pImpl->ltpWeight * std::exp(-deltaT / pImpl->timeConstant);
                    totalDelta += ltp;
                } else {
                    // Post before pre -> LTD (depress)
                    float ltd = pImpl->ltdWeight * std::exp(deltaT / pImpl->timeConstant);
                    totalDelta -= ltd;
                }
            }
        }
    }
    
    // Apply total weight change with modulation
    float currentWeight = synapse->getWeight();
    float newWeight = currentWeight + totalDelta;
    
    // Apply neuromodulation factor (dopamine can strengthen or weaken STDP)
    float neuromodulationFactor = 1.0f;  // Default no modulation
    // In real implementation, this would come from dopamine system
    
    newWeight = newWeight * neuromodulationFactor;
    
    // Enforce weight bounds to maintain stability
    newWeight = std::clamp(newWeight, pImpl->minimumWeight, pImpl->maximumWeight);
    
    // Apply the weight change
    synapse->setWeight(newWeight);
    
    // Update eligibility trace for reward-modulated STDP
    float &eligibility = const_cast<float&>(synapse->getEligibilityTrace());
    eligibility = std::max(0.0f, eligibility + totalDelta * 0.1f);
}

const char* STDP::getName() const {
    return "STDP";
}

} // namespace nlm
