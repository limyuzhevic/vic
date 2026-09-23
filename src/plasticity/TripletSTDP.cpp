#include "TripletSTDP.hpp"
#include "../../brain/Synapse.hpp"
#include <cmath>
#include <algorithm>

namespace nlm {

struct TripletSTDP::Impl {
    float a1;           // First amplitude (A1)
    float a2;           // Second amplitude (A2) 
    float a3;           // Third amplitude (A3)
    float tau1;         // First time constant (τ1)
    float tau2;         // Second time constant (τ2)
    float w0;           // Baseline weight
    float minWeight;    // Minimum synaptic weight
    float maxWeight;    // Maximum synaptic weight
    
    Impl() : a1(0.01f), a2(0.01f), a3(0.01f), tau1(20.0f), tau2(200.0f),
             w0(0.0f), minWeight(-1.0f), maxWeight(1.0f) {}
};

TripletSTDP::TripletSTDP() : pImpl(new Impl) {}

TripletSTDP::~TripletSTDP() = default;

void TripletSTDP::update(Synapse* synapse,
                         const std::vector<Timestamp>& preSpikes,
                         const std::vector<Timestamp>& postSpikes,
                         TimestepDuration dt) {
    /*
     * Triplet STDP with pre-post pairing and post-post interactions
     * 
     * More sophisticated than pairwise STDP, captures synaptic dynamics
     * over longer time windows and more complex interaction patterns
     * 
     * Mathematical formulation:
     * For each pre-post spike pair and triplet interactions:
     * 
     * Potentiation (pre before post):
     * Δw = A1 * exp(-Δt/τ1) + A2 * (pre-post) * exp(-Δt/τ2)
     * + A3 * (pre-post-post) * exp(-Δt/τ2) if three spikes detected
     * 
     * Depression (post before pre):
     * Δw = -A1 * exp(-Δt/τ1) + A2 * (post-pre) * exp(-Δt/τ2)
     * + A3 * (post-pre-post) * exp(-Δt/τ2) if three spikes detected
     * 
     * Biological inspiration:
     *   - Captures calcium dynamics and spike timing interactions
     *   - Models multiple phases of synaptic plasticity
     *   - Accounts for presynaptic and postsynaptic spike bursts
     *   - Reflects NMDA receptor and calcium signaling dynamics
     *   
     * Applications:
     *   - Burst-based learning in hippocampal circuits
     *   - Complex spike timing interactions
     *   - Stable weight changes over longer time scales
     *   - Burst-pause-burst patterns in learning
     */
    
    if (!synapse || preSpikes.empty() || postSpikes.empty()) {
        return;
    }
    
    float totalDelta = 0.0f;
    float tau1 = pImpl->tau1;
    float tau2 = pImpl->tau2;
    
    // Process each pre-post spike pair
    for (Timestamp preTime : preSpikes) {
        for (Timestamp postTime : postSpikes) {
            float dt = static_cast<float>(postTime - preTime);  // Δt in ms
            
            if (dt > 0) {
                // Pre before post: POTENTIATION
                
                // First-order effect: exponential decay
                float delta1 = pImpl->a1 * std::exp(-dt / tau1);
                
                // Second-order effect: depends on recent pre-synaptic activity
                size_t preCount = countRecentSpikes(preTime, preSpikes, tau2);
                float delta2 = pImpl->a2 * static_cast<float>(preCount) * std::exp(-dt / tau2);
                
                // Third-order effect: depends on burst patterns
                size_t tripletCount = countTripletPatterns(preTime, postTime, preSpikes, postSpikes, tau2);
                float delta3 = pImpl->a3 * static_cast<float>(tripletCount) * std::exp(-dt / tau2);
                
                totalDelta += delta1 + delta2 + delta3;
            } else if (dt < 0) {
                // Post before pre: DEPRESSION
                
                // First-order effect: exponential decay
                float delta1 = -pImpl->a1 * std::exp(-dt / tau1);
                
                // Second-order effect: depends on recent post-synaptic activity
                size_t postCount = countRecentSpikes(postTime, postSpikes, tau2);
                float delta2 = -pImpl->a2 * static_cast<float>(postCount) * std::exp(-dt / tau2);
                
                // Third-order effect: depends on burst patterns
                size_t tripletCount = countTripletPatterns(postTime, preTime, postSpikes, preSpikes, tau2);
                float delta3 = -pImpl->a3 * static_cast<float>(tripletCount) * std::exp(-dt / tau2);
                
                totalDelta += delta1 + delta2 + delta3;
            }
            // dt == 0: no change (simultaneous spikes)
        }
    }
    
    // Apply weight change with bounds
    if (std::abs(totalDelta) > 1e-6f) {
        // Scale by synaptic efficacy if available
        float efficacy = synapse->getEfficacy();
        totalDelta *= efficacy;
        
        // Apply weight change
        synapse->addToWeight(totalDelta);
        
        // Update eligibility trace for future learning
        float currentTrace = synapse->getEligibilityTrace();
        synapse->setEligibilityTrace(currentTrace + totalDelta);
    }
}

void TripletSTDP::applyWeightChange(Synapse* synapse, SynapticWeight delta) {
    if (!synapse) return;
    
    // Clamp weight to bounds
    float newWeight = synapse->getWeight() + delta;
    newWeight = std::clamp(newWeight, pImpl->minWeight, pImpl->maxWeight);
    synapse->setWeight(newWeight);
}

const char* TripletSTDP::getName() const {
    return "TripletSTDP";
}

void TripletSTDP::setA1(float a1) {
    pImpl->a1 = std::clamp(a1, 0.0f, 0.1f);
}

float TripletSTDP::getA1() const {
    return pImpl->a1;
}

void TripletSTDP::setA2(float a2) {
    pImpl->a2 = std::clamp(a2, 0.0f, 0.1f);
}

float TripletSTDP::getA2() const {
    return pImpl->a2;
}

void TripletSTDP::setA3(float a3) {
    pImpl->a3 = std::clamp(a3, 0.0f, 0.1f);
}

float TripletSTDP::getA3() const {
    return pImpl->a3;
}

void TripletSTDP::setTau1(float tau1) {
    pImpl->tau1 = std::clamp(tau1, 5.0f, 100.0f);
}

float TripletSTDP::getTau1() const {
    return pImpl->tau1;
}

void TripletSTDP::setTau2(float tau2) {
    pImpl->tau2 = std::clamp(tau2, 50.0f, 500.0f);
}

float TripletSTDP::getTau2() const {
    return pImpl->tau2;
}

void TripletSTDP::setBaselineWeight(float w0) {
    pImpl->w0 = w0;
}

float TripletSTDP::getBaselineWeight() const {
    return pImpl->w0;
}

void TripletSTDP::configure(float a1, float a2, float a3, float tau1, float tau2) {
    setA1(a1);
    setA2(a2);
    setA3(a3);
    setTau1(tau1);
    setTau2(tau2);
}

// Helper functions for triplet detection

// Count recent spikes within time window
inline size_t TripletSTDP::countRecentSpikes(Timestamp spikeTime, 
                                            const std::vector<Timestamp>& spikes,
                                            float timeWindow) const {
    size_t count = 0;
    for (Timestamp spike : spikes) {
        if (std::abs(static_cast<float>(spike - spikeTime)) < timeWindow) {
            ++count;
        }
    }
    return count;
}

// Count triplet patterns (pre-post-post or post-pre-pre)
inline size_t TripletSTDP::countTripletPatterns(Timestamp firstTime,
                                                 Timestamp middleTime,
                                                 const std::vector<Timestamp>& firstSpikes,
                                                 const std::vector<Timestamp>& middleSpikes,
                                                 float timeWindow) const {
    size_t count = 0;
    
    // Look for pattern: first -> middle -> second middle spike
    for (Timestamp secondMiddle : middleSpikes) {
        if (std::abs(static_cast<float>(secondMiddle - firstTime)) < timeWindow &&
            std::abs(static_cast<float>(middleTime - secondMiddle)) < timeWindow) {
            ++count;
        }
    }
    
    return count;
}

} // namespace nlm
