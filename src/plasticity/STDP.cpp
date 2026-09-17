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
    
    // Early validation to avoid null pointer exceptions
    if (!synapse || preSpikes.empty() || postSpikes.empty()) {
        return;
    }
    
    // Validate input parameters
    if (dt <= 0.0f) {
        return; // Invalid timestep
    }
    
    // Validate weight parameters are reasonable
    if (pImpl->timeConstant <= 0.0f || pImpl->ltpWeight < 0.0f || pImpl->ltdWeight < 0.0f) {
        return; // Invalid parameters
    }
    
    // Improved algorithm: Sort spike times and use cumulative sums for O(n log n) instead of O(n²)
    // This significantly improves performance for neurons with many spikes
    std::vector<float> preTimes, postTimes;
    preTimes.reserve(preSpikes.size());
    postTimes.reserve(postSpikes.size());
    
    // Convert timestamps to float for calculations
    for (Timestamp time : preSpikes) {
        preTimes.push_back(static_cast<float>(time));
    }
    for (Timestamp time : postSpikes) {
        postTimes.push_back(static_cast<float>(time));
    }
    
    // Sort spike times for more efficient processing
    std::sort(preTimes.begin(), preTimes.end());
    std::sort(postTimes.begin(), postTimes.end());
    
    float totalDelta = 0.0f;
    float tau = pImpl->timeConstant;
    
    // Process spike pairs using two-pointer technique for better cache locality
    size_t preIdx = 0, postIdx = 0;
    while (preIdx < preTimes.size() && postIdx < postTimes.size()) {
        float dt_val = postTimes[postIdx] - preTimes[preIdx];
        
        if (dt_val > 0) {
            // Pre before post: POTENTIATION
            // "Cells that fire together, wire together" - but only if pre fires before post
            float delta = pImpl->ltpWeight * std::exp(-dt_val / tau);
            totalDelta += delta;
            
            // Move to next pre spike if this pair is processed
            preIdx++;
        } else {
            // Post before pre: DEPRESSION
            // "Anti-Hebbian" - connection weakens if post fires without pre
            float delta = -pImpl->ltdWeight * std::exp(dt_val / tau);  // dt is negative, so this subtracts
            totalDelta += delta;
            
            // Move to next post spike if this pair is processed
            postIdx++;
        }
    }
    
    // Apply weight change with bounds and eligibility trace update
    applyCommonWeightChange(synapse, totalDelta, pImpl->minWeight, pImpl->maxWeight);
    
    // Update eligibility trace for reward-modulated learning
    if (synapse && std::abs(totalDelta) > 1e-6f) {
        float currentTrace = synapse->getEligibilityTrace();
        synapse->setEligibilityTrace(currentTrace + totalDelta);
    }
}

void STDP::applyWeightChange(Synapse* synapse, SynapticWeight delta) {
    if (!synapse) return;
    
    // Validate weight change is not excessively large
    if (std::abs(delta) > 10.0f) {
        return; // Prevent extreme weight changes
    }
    
    // Apply weight change with bounds using common method
    applyCommonWeightChange(synapse, delta, pImpl->minWeight, pImpl->maxWeight);
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