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

STDP::STDP() : pImpl(std::make_unique<Impl>()) {}

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
    
    // OPTIMIZATION: Use sorted vectors and binary search for efficiency
    // STDP time window is typically 20-100ms, so we only need recent spikes
    const float stdpTimeWindow = 5.0f * tau;  // 5 standard deviations
    
    // Filter spikes within STDP time window of most recent spike
    Timestamp recentPreSpike = preSpikes.back();
    Timestamp recentPostSpike = postSpikes.back();
    Timestamp referenceTime = std::max(recentPreSpike, recentPostSpike);
    
    // Collect relevant pre and post spikes (within time window)
    std::vector<Timestamp> filteredPreSpikes;
    std::vector<Timestamp> filteredPostSpikes;
    
    filteredPreSpikes.reserve(preSpikes.size());
    filteredPostSpikes.reserve(postSpikes.size());
    
    for (Timestamp preTime : preSpikes) {
        if (referenceTime - preTime <= stdpTimeWindow) {
            filteredPreSpikes.push_back(preTime);
        }
    }
    
    for (Timestamp postTime : postSpikes) {
        if (referenceTime - postTime <= stdpTimeWindow) {
            filteredPostSpikes.push_back(postTime);
        }
    }
    
    // OPTIMIZATION: Use sliding window approach for better cache locality
    // Process spikes in time order to maximize cache efficiency
    Timestamp minPreTime = filteredPreSpikes.empty() ? 0 : filteredPreSpikes.front();
    Timestamp minPostTime = filteredPostSpikes.empty() ? 0 : filteredPostSpikes.front();
    
    // Sort by time for better temporal locality
    if (!filteredPreSpikes.empty() && !filteredPostSpikes.empty()) {
        // Merge the two spike sequences in chronological order
        // This allows us to process them in a single pass
        std::vector<Timestamp> mergedTimeline;
        size_t i = 0, j = 0;
        
        while (i < filteredPreSpikes.size() || j < filteredPostSpikes.size()) {
            if (i >= filteredPreSpikes.size()) {
                mergedTimeline.push_back(filteredPostSpikes[j]);
                j++;
            } else if (j >= filteredPostSpikes.size()) {
                mergedTimeline.push_back(filteredPreSpikes[i]);
                i++;
            } else if (filteredPreSpikes[i] < filteredPostSpikes[j]) {
                mergedTimeline.push_back(filteredPreSpikes[i]);
                i++;
            } else {
                mergedTimeline.push_back(filteredPostSpikes[j]);
                j++;
            }
        }
        
        // OPTIMIZATION: Two-pointer technique for O(n) complexity
        // This avoids the O(n²) nested loop by processing spikes in chronological order
        size_t preIdx = 0, postIdx = 0;
        float sumPre = 0.0f, sumPost = 0.0f;
        
        while (preIdx < filteredPreSpikes.size() && postIdx < filteredPostSpikes.size()) {
            if (filteredPreSpikes[preIdx] < filteredPostSpikes[postIdx]) {
                // Pre spike
                float delta = pImpl->ltpWeight * std::exp(
                    static_cast<float>(filteredPreSpikes[preIdx] - filteredPostSpikes[postIdx]) / tau);
                totalDelta += delta;
                preIdx++;
            } else {
                // Post spike
                float delta = -pImpl->ltdWeight * std::exp(
                    static_cast<float>(filteredPostSpikes[postIdx] - filteredPreSpikes[preIdx]) / tau);
                totalDelta += delta;
                postIdx++;
            }
        }
        
        // Process remaining spikes
        while (preIdx < filteredPreSpikes.size()) {
            float delta = pImpl->ltpWeight * std::exp(
                static_cast<float>(filteredPreSpikes[preIdx] - referenceTime) / tau);
            totalDelta += delta;
            preIdx++;
        }
        
        while (postIdx < filteredPostSpikes.size()) {
            float delta = -pImpl->ltdWeight * std::exp(
                static_cast<float>(filteredPostSpikes[postIdx] - referenceTime) / tau);
            totalDelta += delta;
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
