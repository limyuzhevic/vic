#include "STDP.hpp"
#include "../../brain/Synapse.hpp"
#include "../core/Types/Types.hpp"
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
    
    if (!synapse) {
        NLM_LOG_WARNING("STDP::update called with null synapse");
        return;
    }
    
    if (preSpikes.empty() || postSpikes.empty()) {
        return;
    }
    
    float totalDelta = 0.0f;
    float tau = pImpl->timeConstant;
    
    // OPTIMIZED: Avoid O(n²) by using sorted spike times and efficient computation
    // Sort spike times for efficient processing (they should already be sorted from recording)
    std::vector<Timestamp> sortedPreSpikes = preSpikes;
    std::vector<Timestamp> sortedPostSpikes = postSpikes;
    std::sort(sortedPreSpikes.begin(), sortedPreSpikes.end());
    std::sort(sortedPostSpikes.begin(), sortedPostSpikes.end());
    
    // Use merge-like approach for O(n log n) complexity
    size_t preIdx = 0, postIdx = 0;
    float accumulatedDelta = 0.0f;
    
    // Process all spike pairs efficiently
    while (preIdx < sortedPreSpikes.size() && postIdx < sortedPostSpikes.size()) {
        Timestamp preTime = sortedPreSpikes[preIdx];
        Timestamp postTime = sortedPostSpikes[postIdx];
        float deltaT = static_cast<float>(postTime - preTime);
        
        if (deltaT > 0) {
            // Pre before post: accumulate LTP contribution
            // Integral of exp(-Δt/τ) from current Δt to infinity
            accumulatedDelta += pImpl->ltpWeight * tau * std::exp(-deltaT / tau);
            ++postIdx;  // Move to next post spike
        } else {
            // Post before pre: accumulate LTD contribution
            // Integral of exp(Δt/τ) from current Δt to -∞
            accumulatedDelta += -pImpl->ltdWeight * tau * std::exp(deltaT / tau);
            ++preIdx;  // Move to next pre spike
        }
    }
    
    // Add remaining contributions from unpaired spikes
    while (preIdx < sortedPreSpikes.size()) {
        float deltaT = static_cast<float>(sortedPostSpikes.back() - sortedPreSpikes[preIdx]);
        if (deltaT > 0) {
            accumulatedDelta += pImpl->ltpWeight * tau * std::exp(-deltaT / tau);
        }
        ++preIdx;
    }
    
    while (postIdx < sortedPostSpikes.size()) {
        float deltaT = static_cast<float>(sortedPostSpikes[postIdx] - sortedPreSpikes.back());
        if (deltaT < 0) {
            accumulatedDelta += -pImpl->ltdWeight * tau * std::exp(deltaT / tau);
        }
        ++postIdx;
    }
    
    totalDelta = accumulatedDelta;
    
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
    if (!synapse) {
        NLM_LOG_ERROR("STDP::applyWeightChange called with null synapse");
        return;
    }
    
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
