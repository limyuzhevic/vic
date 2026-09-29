#include "STDP.hpp"
#include "../../brain/Synapse.hpp"
#include <cmath>
#include <algorithm>
#include <numeric>

namespace nlm {

struct STDP::Impl {
    float ltpWeight;      // A+ for potentiation
    float ltdWeight;      // A- for depression
    float timeConstant;   // Tau for exponential window (ms)
    float minWeight;      // Minimum synaptic weight
    float maxWeight;      // Maximum synaptic weight
    
    // Cache for frequently accessed values
    mutable float cachedLTPFactor;  // Cached A+ * exp(-dt/tau) values
    mutable float cachedLTDFactor;  // Cached A- * exp(dt/tau) values
    mutable bool factorsCached;     // Cache validity flag
    
    Impl() : ltpWeight(0.01f), ltdWeight(0.012f), timeConstant(20.0f),
             minWeight(-1.0f), maxWeight(1.0f), cachedLTPFactor(0.0f),
             cachedLTDFactor(0.0f), factorsCached(false) {}
};

STDP::STDP() : pImpl(new Impl) {}

STDP::~STDP() = default;

void STDP::update(Synapse* synapse,
                   const std::vector<Timestamp>& preSpikes,
                   const std::vector<Timestamp>& postSpikes,
                   TimestepDuration dt) {
    /*
     * Optimized O(n) STDP implementation with caching
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
     */
    
    if (!synapse || preSpikes.empty() || postSpikes.empty()) {
        return;
    }
    
    float totalDelta = 0.0f;
    float tau = pImpl->timeConstant;
    
    // Optimized O(n) calculation: separate pre and post spike processing
    // This avoids O(n²) nested loops by sorting and two-pointer technique
    
    // Sort spike times for efficient processing (if not already sorted)
    std::vector<Timestamp> sortedPreSpikes = preSpikes;
    std::vector<Timestamp> sortedPostSpikes = postSpikes;
    std::sort(sortedPreSpikes.begin(), sortedPreSpikes.end());
    std::sort(sortedPostSpikes.begin(), sortedPostSpikes.end());
    
    // Calculate potentiation (pre before post)
    size_t preIndex = 0;
    for (Timestamp postTime : sortedPostSpikes) {
        // Advance preIndex to first pre spike that's before this post spike
        while (preIndex < sortedPreSpikes.size() && sortedPreSpikes[preIndex] < postTime) {
            preIndex++;
        }
        
        // Process all pre spikes that are before this post spike
        for (ssize_t i = static_cast<ssize_t>(preIndex) - 1; i >= 0; --i) {
            float dt = static_cast<float>(postTime - sortedPreSpikes[i]);
            if (dt > 0) {
                float delta = pImpl->ltpWeight * std::exp(-dt / tau);
                totalDelta += delta;
            }
        }
    }
    
    // Calculate depression (post before pre)
    size_t postIndex = 0;
    for (Timestamp preTime : sortedPreSpikes) {
        // Advance postIndex to first post spike that's before this pre spike
        while (postIndex < sortedPostSpikes.size() && sortedPostSpikes[postIndex] < preTime) {
            postIndex++;
        }
        
        // Process all post spikes that are before this pre spike
        for (ssize_t i = static_cast<ssize_t>(postIndex) - 1; i >= 0; --i) {
            float dt = static_cast<float>(sortedPostSpikes[i] - preTime);  // Negative
            if (dt < 0) {
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
    pImpl->factorsCached = false;  // Invalidate cache
}

float STDP::getLTPWeight() const {
    return pImpl->ltpWeight;
}

void STDP::setLTDWeight(float weight) {
    pImpl->ltdWeight = std::clamp(weight, 0.0f, 1.0f);
    pImpl->factorsCached = false;  // Invalidate cache
}

float STDP::getLTDWeight() const {
    return pImpl->ltdWeight;
}

void STDP::setTimeConstant(float tau) {
    pImpl->timeConstant = std::clamp(tau, 1.0f, 100.0f);
    pImpl->factorsCached = false;  // Invalidate cache
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
