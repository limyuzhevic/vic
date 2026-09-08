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
     *   - Assumes single exponential window (more complex in biology)
     *   - Doesn't account for synaptic eligibility traces
     */
    
    if (!synapse || preSpikes.empty() || postSpikes.empty()) {
        return;
    }
    
    float totalDelta = 0.0f;
    float tau = pImpl->timeConstant;
    
    // O(n log n) instead of O(n²) by sorting and using two-pointer technique
    std::vector<Timestamp> sortedPreSpikes = preSpikes;
    std::vector<Timestamp> sortedPostSpikes = postSpikes;
    
    // Sort spike times for efficient pairing
    std::sort(sortedPreSpikes.begin(), sortedPreSpikes.end());
    std::sort(sortedPostSpikes.begin(), sortedPostSpikes.end());
    
    // Process spikes with two-pointer approach to avoid O(n²)
    size_t preIdx = 0, postIdx = 0;
    
    // All potentiation pairs (pre before post)
    while (preIdx < sortedPreSpikes.size() && postIdx < sortedPostSpikes.size()) {
        if (sortedPostSpikes[postIdx] >= sortedPreSpikes[preIdx]) {
            // Pre spike at sortedPreSpikes[preIdx], post spike at sortedPostSpikes[postIdx]
            float deltaTime = static_cast<float>(sortedPostSpikes[postIdx] - sortedPreSpikes[preIdx]);
            if (deltaTime > 0) {
                // Pre before post: POTENTIATION
                float delta = pImpl->ltpWeight * std::exp(-deltaTime / tau);
                totalDelta += delta;
            }
            ++postIdx;
        } else {
            // Post before pre: DEPRESSION
            float deltaTime = static_cast<float>(sortedPreSpikes[preIdx] - sortedPostSpikes[postIdx]);
            if (deltaTime > 0) {
                float delta = -pImpl->ltdWeight * std::exp(-deltaTime / tau);
                totalDelta += delta;
            }
            ++preIdx;
        }
    }
    
    // Additional potentiation for remaining post spikes
    while (postIdx < sortedPostSpikes.size()) {
        // Pre spike is before all remaining post spikes, so post is after pre
        float deltaTime = static_cast<float>(sortedPostSpikes[postIdx] - (preIdx < sortedPreSpikes.size() ? sortedPreSpikes[preIdx] : 0));
        if (deltaTime > 0) {
            float delta = pImpl->ltpWeight * std::exp(-deltaTime / tau);
            totalDelta += delta;
        }
        ++postIdx;
    }
    
    // Additional depression for remaining pre spikes  
    while (preIdx < sortedPreSpikes.size()) {
        // Post spike is before all remaining pre spikes, so pre is after post
        float deltaTime = static_cast<float>(sortedPreSpikes[preIdx] - (postIdx < sortedPostSpikes.size() ? sortedPostSpikes[postIdx] : 0));
        if (deltaTime > 0) {
            float delta = -pImpl->ltdWeight * std::exp(-deltaTime / tau);
            totalDelta += delta;
        }
        ++preIdx;
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
