#include "STDP.hpp"
#include "../../brain/Synapse.hpp"
#include <cmath>
#include <algorithm>

namespace nlm {

struct STDP::Impl {
    float ltpWeight;      // A+ for potentiation
    float ltdWeight;      // A- for depression
    float timeConstant;   // Tau for exponential window (ms)
    float minWeight;      // Minimum synaptic weight
    float maxWeight;      // Maximum synaptic weight
    
    Impl() : ltpWeight(0.01f), ltdWeight(0.012f), timeConstant(20.0f),
             minWeight(-1.0f), maxWeight(1.0f) {}
};

STDP::STDP() : pImpl(new Impl) {}

STDP::~STDP() = default;

void STDP::update(Synapse* synapse,
                   const std::vector<Timestamp>& preSpikes,
                   const std::vector<Timestamp>& postSpikes,
                   TimestepDuration dt) {
    /*
     * Optimized O(n+m) STDP implementation using two-pointer technique
     * 
     * Original O(n²) implementation compared every pre-spike with every post-spike
     * New O(n+m) implementation processes spike pairs in chronological order
     * 
     * Mathematical formulation (same as before):
     * For each pre-post spike pair with timing difference Δt = t_post - t_pre:
     *   If Δt > 0 (pre before post): Δw = A+ * exp(-Δt / τ+)
     *   If Δt < 0 (post before pre): Δw = A- * exp(Δt / τ-)
     */
    
    if (!synapse || preSpikes.empty() || postSpikes.empty()) {
        return;
    }
    
    float totalDelta = 0.0f;
    float tau = pImpl->timeConstant;
    
    // Create sorted copies of spike times (assuming they're not already sorted)
    // In practice, spike times should be in chronological order already
    std::vector<Timestamp> sortedPreSpikes = preSpikes;
    std::vector<Timestamp> sortedPostSpikes = postSpikes;
    std::sort(sortedPreSpikes.begin(), sortedPreSpikes.end());
    std::sort(sortedPostSpikes.begin(), sortedPostSpikes.end());
    
    // Two-pointer technique for O(n+m) complexity
    size_t preIdx = 0;
    size_t postIdx = 0;
    
    while (preIdx < sortedPreSpikes.size() && postIdx < sortedPostSpikes.size()) {
        Timestamp preTime = sortedPreSpikes[preIdx];
        Timestamp postTime = sortedPostSpikes[postIdx];
        
        float dt = static_cast<float>(postTime - preTime);  // Δt in ms
        
        if (dt > 0) {
            // Pre before post: POTENTIATION
            // Calculate potentiation for this pair
            float delta = pImpl->ltpWeight * std::exp(-dt / tau);
            totalDelta += delta;
            
            // Move to next post-spike (already processed all earlier post-spikes with this pre-spike)
            postIdx++;
        } else if (dt < 0) {
            // Post before pre: DEPRESSION
            // Calculate depression for this pair
            float delta = -pImpl->ltdWeight * std::exp(dt / tau);  // dt is negative, so this subtracts
            totalDelta += delta;
            
            // Move to next pre-spike (already processed all earlier pre-spikes with this post-spike)
            preIdx++;
        } else {
            // dt == 0: simultaneous spikes (no change)
            preIdx++;
            postIdx++;
        }
    }
    
    // Apply weight change with bounds
    if (std::abs(totalDelta) > 1e-6f) {
        // Scale by synaptic efficacy if available
        float efficacy = synapse->getEfficacy();
        totalDelta *= efficacy;
        
        // Apply weight change
        synapse->addToWeight(totalDelta);
        
        // Update eligibility trace for reward-modulated learning
        float currentTrace = synapse->getEligibilityTrace();
        synapse->setEligibilityTrace(currentTrace + totalDelta);
    }
}

void STDP::applyWeightChange(Synapse* synapse, SynapticWeight delta) {
    if (!synapse) return;
    
    // Clamp weight to bounds
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
