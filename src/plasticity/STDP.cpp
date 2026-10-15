#include "STDP.hpp"
#include "../../brain/Synapse.hpp"
#include "SpikeCorrelationCache.hpp"
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
     * OPTIMIZED: Uses SpikeCorrelationCache to avoid redundant calculations
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
     * OPTIMIZATION:
     * 1. Use SpikeCorrelationCache for spike storage and statistics
     * 2. Sort spike times for efficient window processing (O(n log n))
     * 3. Use sliding windows to process spikes within timeConstant (O(n))
     * 4. Only consider spike pairs within timeConstant window
     * 5. Cache correlation statistics to avoid recomputation
     */
    
    if (!synapse || preSpikes.empty() || postSpikes.empty()) {
        return;
    }
    
    // Get synapse ID for cache operations
    SynapseId synapseId = synapse->getId();
    
    // Try to get cached spike data
    SpikeCorrelationCache cache;
    auto cachedSpikes = cache.getSpikes(synapseId);
    
    // Get spike data from either cache or directly
    std::vector<Timestamp> actualPreSpikes = preSpikes;
    std::vector<Timestamp> actualPostSpikes = postSpikes;
    
    if (!cachedSpikes.first.empty() && !cachedSpikes.second.empty()) {
        // Use cached spikes (they should be equivalent)
        actualPreSpikes = cachedSpikes.first;
        actualPostSpikes = cachedSpikes.second;
    }
    
    // Store spikes in cache for future use
    cache.storeSpikes(synapseId, actualPreSpikes, actualPostSpikes);
    
    float totalDelta = 0.0f;
    float tau = pImpl->timeConstant;
    
    // OPTIMIZATION: Sort spikes for efficient window processing
    std::vector<Timestamp> sortedPre = actualPreSpikes;
    std::vector<Timestamp> sortedPost = actualPostSpikes;
    std::sort(sortedPre.begin(), sortedPre.end());
    std::sort(sortedPost.begin(), sortedPost.end());
    
    // OPTIMIZATION: Sliding window approach for each spike type
    size_t preIdx = 0, postIdx = 0;
    
    // Process pre-before-post pairings (positive Δt)
    for (size_t i = 0; i < sortedPre.size(); ++i) {
        Timestamp preTime = sortedPre[i];
        
        // Find posts that occur after this pre spike
        // Only consider posts within timeConstant window
        while (postIdx < sortedPost.size() && 
               sortedPost[postIdx] < preTime + tau) {
            Timestamp postTime = sortedPost[postIdx];
            float dt = static_cast<float>(postTime - preTime);  // Δt in ms
            
            if (dt > 0) {
                // Pre before post: POTENTIATION
                float delta = pImpl->ltpWeight * std::exp(-dt / tau);
                totalDelta += delta;
            }
            ++postIdx;
        }
    }
    
    // Reset post index for depression calculation
    postIdx = 0;
    
    // Process post-before-pre pairings (negative Δt)
    for (size_t i = 0; i < sortedPost.size(); ++i) {
        Timestamp postTime = sortedPost[i];
        
        // Find pres that occur after this post spike
        // Only consider pres within timeConstant window
        while (preIdx < sortedPre.size() &&
               sortedPre[preIdx] < postTime + tau) {
            Timestamp preTime = sortedPre[preIdx];
            float dt = static_cast<float>(postTime - preTime);  // Δt in ms
            
            if (dt < 0) {
                // Post before pre: DEPRESSION
                float delta = -pImpl->ltdWeight * std::exp(dt / tau);  // dt is negative
                totalDelta += delta;
            }
            ++preIdx;
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
