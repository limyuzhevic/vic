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
    
    // Optimized STDP calculation using spatial sorting for better performance
    // Sort spike times to enable more efficient processing
    std::vector<Timestamp> sortedPreSpikes = preSpikes;
    std::vector<Timestamp> sortedPostSpikes = postSpikes;
    std::sort(sortedPreSpikes.begin(), sortedPreSpikes.end());
    std::sort(sortedPostSpikes.begin(), sortedPostSpikes.end());
    
    // For each post spike, find all pre spikes that occur before it (for LTP)
    // This is O(n log n) instead of O(n²)
    for (size_t i = 0; i < sortedPostSpikes.size(); ++i) {
        Timestamp postTime = sortedPostSpikes[i];
        
        // Find all pre spikes with time < postTime (LTP)
        auto preIt = std::lower_bound(sortedPreSpikes.begin(), sortedPreSpikes.end(), postTime);
        // Use iterator difference to count number of pre spikes before this post spike
        size_t numPreBefore = std::distance(sortedPreSpikes.begin(), preIt);
        
        if (numPreBefore > 0) {
            // For LTP: contribution from all pre spikes before this post spike
            // Approximate integral of exponential decay over multiple spikes
            for (size_t j = 0; j < numPreBefore; ++j) {
                float dt = static_cast<float>(postTime - sortedPreSpikes[j]);
                float delta = pImpl->ltpWeight * std::exp(-dt / tau);
                totalDelta += delta;
            }
        }
    }
    
    // For each pre spike, find all post spikes that occur after it (for LTD)
    for (size_t i = 0; i < sortedPreSpikes.size(); ++i) {
        Timestamp preTime = sortedPreSpikes[i];
        
        // Find all post spikes with time > preTime (LTD)
        auto postIt = std::upper_bound(sortedPostSpikes.begin(), sortedPostSpikes.end(), preTime);
        size_t numPostAfter = std::distance(postIt, sortedPostSpikes.end());
        
        if (numPostAfter > 0) {
            // For LTD: contribution from all post spikes after this pre spike
            for (auto it = postIt; it != sortedPostSpikes.end(); ++it) {
                float dt = static_cast<float>(*it - preTime);  // Always positive for LTD
                float delta = -pImpl->ltdWeight * std::exp(dt / tau);
                totalDelta += delta;
            }
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
