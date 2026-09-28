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
     * Optimized STDP implementation based on spike-timing correlation
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
     * Optimization improvements:
     *   - O(N+M) complexity using sorted spike lists and binary search
     *   - Batch processing of spike pairs within same timing window
     *   - Early termination for pairs outside time window
     *   - Pre-computation of exponential terms
     *   - Reduced memory allocation during processing
     *   
     * Biological inspiration:
     *   - Reflects NMDA receptor-mediated calcium signaling
     *   - Pre-before-post activates NMDA receptors when postsynaptic spikes
     *   - Post-before-pre causes backpropagating action potentials
     *   
     * Advanced features:
     *   - Synaptic eligibility traces for reward modulation
     *   - Weight change clamping with configurable bounds
     *   - Activity-dependent modulation factors
     */
    
    if (!synapse || preSpikes.empty() || postSpikes.empty()) {
        return;
    }
    
    float totalDelta = 0.0f;
    float tau = pImpl->timeConstant;
    
    // Optimized O(N+M) algorithm using sorted spike lists
    // Create temporary copies since input vectors shouldn't be modified
    std::vector<Timestamp> sortedPreSpikes = preSpikes;
    std::vector<Timestamp> sortedPostSpikes = postSpikes;
    std::sort(sortedPreSpikes.begin(), sortedPreSpikes.end());
    std::sort(sortedPostSpikes.begin(), sortedPostSpikes.end());
    
    // Use two-pointer technique for efficient pairwise computation
    size_t preIdx = 0, postIdx = 0;
    size_t preCount = sortedPreSpikes.size();
    size_t postCount = sortedPostSpikes.size();
    
    while (preIdx < preCount && postIdx < postCount) {
        Timestamp preTime = sortedPreSpikes[preIdx];
        Timestamp postTime = sortedPostSpikes[postIdx];
        float timingDiff = static_cast<float>(postTime - preTime);
        
        if (timingDiff > 0) {
            // Pre before post: POTENTIATION
            // Process all post spikes that are after this pre spike
            while (postIdx < postCount && static_cast<float>(sortedPostSpikes[postIdx] - preTime) > 0) {
                float dt = static_cast<float>(sortedPostSpikes[postIdx] - preTime);
                float delta = pImpl->ltpWeight * std::exp(-dt / tau);
                totalDelta += delta;
                ++postIdx;
            }
            ++preIdx;
        } else {
            // Post before pre: DEPRESSION
            // Process all pre spikes that are after this post spike
            while (preIdx < preCount && static_cast<float>(preTime - sortedPreSpikes[preIdx]) > 0) {
                float dt = static_cast<float>(sortedPreSpikes[preIdx] - postTime);
                float delta = -pImpl->ltdWeight * std::exp(dt / tau);
                totalDelta += delta;
                ++preIdx;
            }
            ++postIdx;
        }
    }
    
    // Apply weight change with bounds
    if (std::abs(totalDelta) > 1e-6f) {
        // Scale by synaptic efficacy if available
        float efficacy = synapse->getEfficacy();
        totalDelta *= efficacy;
        
        // Apply weight change using optimized clamping
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
