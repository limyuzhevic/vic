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
     * Optimized STDP implementation with O(n log n) complexity
     * 
     * Instead of O(n²) pairwise comparison, we use:
     * 1. Sort spike times (O(n log n))
     * 2. Use binary search to find relevant spike pairs (O(log n) per spike)
     * 3. Accumulate contributions within the time window (O(n log n) total)
     * 
     * Biological inspiration:
     *   - Pre-before-post causes potentiation via NMDA receptor activation
     *   - Post-before-pre causes depression via backpropagating action potentials
     *   - Time window determined by synaptic dynamics (τ ≈ 20-50ms)
     *   
     * Mathematical formulation (same as before):
     * For each spike pair with timing difference Δt = t_post - t_pre:
     *   Δw = A+ * exp(-Δt / τ+) if Δt > 0 (pre before post)
     *   Δw = A- * exp(Δt / τ-)  if Δt < 0 (post before pre)
     */
    
    if (!synapse || preSpikes.empty() || postSpikes.empty()) {
        return;
    }
    
    float totalDelta = 0.0f;
    float tau = pImpl->timeConstant;
    
    // Create sorted copies of spike times for binary search
    std::vector<Timestamp> sortedPreSpikes = preSpikes;
    std::vector<Timestamp> sortedPostSpikes = postSpikes;
    
    std::sort(sortedPreSpikes.begin(), sortedPreSpikes.end());
    std::sort(sortedPostSpikes.begin(), sortedPostSpikes.end());
    
    // For each pre-synaptic spike, find post-synaptic spikes within time window
    for (Timestamp preTime : sortedPreSpikes) {
        // Find post-synaptic spikes that occur after preTime
        // Use binary search to find the first post-spike > preTime
        auto postIt = std::upper_bound(sortedPostSpikes.begin(), sortedPostSpikes.end(), preTime);
        
        // Process post-spikes within the time window (tau)
        // We can stop when the time difference exceeds tau
        size_t postIdx = std::distance(sortedPostSpikes.begin(), postIt);
        size_t maxPostIdx = postIdx;
        
        // Calculate the maximum index to process based on time window
        // We need to find how many post-spikes are within tau time of preTime
        while (postIdx < sortedPostSpikes.size()) {
            float dt = static_cast<float>(sortedPostSpikes[postIdx] - preTime);
            if (dt > tau) break;  // Outside time window
            postIdx++;
        }
        
        // Now process only the relevant post-spikes
        for (size_t i = maxPostIdx; i < postIdx; ++i) {
            Timestamp postTime = sortedPostSpikes[i];
            float delta = static_cast<float>(postTime - preTime);  // Δt
            
            if (delta > 0) {
                // Pre before post: POTENTIATION
                float weightChange = pImpl->ltpWeight * std::exp(-delta / tau);
                totalDelta += weightChange;
            } else if (delta < 0) {
                // Post before pre: DEPRESSION
                float weightChange = -pImpl->ltdWeight * std::exp(delta / tau);
                totalDelta += weightChange;
            }
            // delta == 0: no change (simultaneous spikes)
        }
    }
    
    // Process the reverse direction: post before pre (alternative approach)
    // For completeness, we process each post-spike and find pre-spikes before it
    for (Timestamp postTime : sortedPostSpikes) {
        // Find pre-synaptic spikes that occur before postTime
        auto preIt = std::lower_bound(sortedPreSpikes.begin(), sortedPreSpikes.end(), postTime);
        
        // Process pre-spikes within the time window
        size_t preIdx = std::distance(sortedPreSpikes.begin(), preIt);
        size_t maxPreIdx = preIdx;
        
        // Find how many pre-spikes are within tau time before postTime
        while (preIdx > 0) {
            preIdx--;
            float delta = static_cast<float>(postTime - sortedPreSpikes[preIdx]);
            if (delta > tau) break;  // Outside time window
        }
        
        // Process only the relevant pre-spikes
        for (size_t i = maxPreIdx - 1; i >= preIdx; --i) {
            Timestamp preTime = sortedPreSpikes[i];
            float delta = static_cast<float>(postTime - preTime);
            
            if (delta > 0) {
                // This was already processed in the first loop, skip to avoid double counting
                continue;
            } else if (delta < 0) {
                // Post before pre: DEPRESSION
                float weightChange = -pImpl->ltdWeight * std::exp(delta / tau);
                totalDelta += weightChange;
            }
            // delta == 0: no change
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
