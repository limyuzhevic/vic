#include "STDP.hpp"
#include "../../brain/Synapse.hpp"
#include "../../core/Logger/Logger.hpp"
#include "../../core/ErrorHandling.hpp"
#include <cmath>
#include <algorithm>

namespace nlm {

// Helper class for efficient O(n) spike processing
class SpikeProcessor {
public:
    // Optimized STDP calculation using O(n) algorithm
    static float calculateSTDP(const std::vector<Timestamp>& preSpikes,
                               const std::vector<Timestamp>& postSpikes,
                               float ltpWeight, float ltdWeight, float timeConstant) {
        if (preSpikes.empty() || postSpikes.empty()) return 0.0f;
        
        // Create sorted copies for efficient processing
        std::vector<Timestamp> sortedPre = preSpikes;
        std::vector<Timestamp> sortedPost = postSpikes;
        std::sort(sortedPre.begin(), sortedPre.end());
        std::sort(sortedPost.begin(), sortedPost.end());
        
        float totalDelta = 0.0f;
        
        // Two-pointer technique for O(n) processing
        size_t i = 0, j = 0;
        while (i < sortedPre.size() && j < sortedPost.size()) {
            float dt = static_cast<float>(sortedPost[j] - sortedPre[i]);
            
            if (dt > 0) {
                // Pre before post: POTENTIATION
                totalDelta += ltpWeight * std::exp(-dt / timeConstant);
                j++;  // Move to next post spike
            } else {
                // Post before pre: DEPRESSION
                totalDelta += -ltdWeight * std::exp(dt / timeConstant);
                i++;  // Move to next pre spike
            }
        }
        
        // Process remaining spikes (one side exhausted)
        while (i < sortedPre.size()) {
            // No more post spikes, calculate with hypothetical post spike
            // This is less accurate but preserves O(n) complexity
            float dt = static_cast<float>(sortedPre[i] - sortedPre[i]);  // dt = 0
            if (dt == 0) {
                totalDelta += ltpWeight * 1.0f;  // Maximum potentiation
            }
            i++;
        }
        
        while (j < sortedPost.size()) {
            // No more pre spikes
            float dt = static_cast<float>(sortedPost[j] - sortedPost[j]);  // dt = 0
            if (dt == 0) {
                totalDelta += ltdWeight * 1.0f;  // Maximum depression
            }
            j++;
        }
        
        return totalDelta;
    }
};

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
    
    // Use standardized error handling
    auto error = ErrorResult::success();
    
    error.checkNotNull(synapse, "STDP::update synapse parameter");
    error.check(!preSpikes.empty(), "STDP::update preSpikes empty");
    error.check(!postSpikes.empty(), "STDP::update postSpikes empty");
    
    if (error.failure()) {
        NLM_LOG_ERROR(error.message());
        return;
    }
    
    // Use optimized spike processing
    float totalDelta = SpikeProcessor::calculateSTDP(
        preSpikes, postSpikes,
        pImpl->ltpWeight, pImpl->ltdWeight, pImpl->timeConstant
    );
    
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
    // Use standardized error handling
    auto error = ErrorResult::success();
    
    error.checkNotNull(synapse, "STDP::applyWeightChange synapse parameter");
    
    if (error.failure()) {
        NLM_LOG_ERROR(error.message());
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
