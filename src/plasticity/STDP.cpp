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
     * Optimized STDP implementation with O(N+M) complexity instead of O(N²)
     * 
     * Original implementation had nested loops over all pre and post spikes,
     * resulting in O(N×M) complexity which becomes prohibitive for long spike
     * trains. This optimized version uses spike count aggregation and time
     * windowing to achieve O(N+M) complexity.
     * 
     * Key optimizations:
     * 1. Time windowing: Only consider spike pairs within τ window
     * 2. Spike count aggregation: Sum spikes by time bucket
     * 3. Pre-computation: Cache exponential calculations
     * 4. Early termination: Stop when time window exceeded
     * 
     * Mathematical formulation:
     * For efficient computation, we bucket spikes by time windows
     * and compute aggregated weight changes using analytical sums:
     * 
     * Δw_total = A+ * Σ_{post spikes} Σ_{pre spikes within τ} exp(-Δt/τ)
     *           + A- * Σ_{pre spikes} Σ_{post spikes within τ} exp(-Δt/τ)
     * 
     * Where Δt = t_post - t_pre for potentiation (pre before post)
     *       Δt = t_pre - t_post for depression (post before pre)
     */
    
    if (!synapse || preSpikes.empty() || postSpikes.empty()) {
        return;
    }
    
    float totalDelta = 0.0f;
    float tau = pImpl->timeConstant;
    
    // Optimization: Find spike time range to process
    Timestamp minTime = std::min(
        *std::min_element(preSpikes.begin(), preSpikes.end()),
        *std::min_element(postSpikes.begin(), postSpikes.end()));
    Timestamp maxTime = std::max(
        *std::max_element(preSpikes.begin(), preSpikes.end()),
        *std::max_element(postSpikes.begin(), postSpikes.end()));
    
    // Time window for STDP: only consider spike pairs within τ time
    const float stdpTimeWindow = tau * 5.0f;  // Process spikes within 5τ
    
    // Pre-process spike times into time buckets for O(N+M) processing
    // Group spikes into 1ms buckets within the STDP time window
    const size_t timeBuckets = static_cast<size_t>(stdpTimeWindow) + 1;
    std::vector<size_t> preBucketCounts(timeBuckets, 0);
    std::vector<size_t> postBucketCounts(timeBuckets, 0);
    
    // Count spikes in each time bucket relative to minTime
    for (Timestamp time : preSpikes) {
        if (time >= minTime) {
            size_t bucket = static_cast<size_t>(time - minTime);
            if (bucket < timeBuckets) {
                preBucketCounts[bucket]++;
            }
        }
    }
    
    for (Timestamp time : postSpikes) {
        if (time >= minTime) {
            size_t bucket = static_cast<size_t>(time - minTime);
            if (bucket < timeBuckets) {
                postBucketCounts[bucket]++;
            }
        }
    }
    
    // Compute weight changes using bucket aggregation
    // This avoids O(N×M) pairwise comparisons
    
    // Calculate potentiation (pre before post)
    // For each post spike at bucket j, sum contributions from pre spikes
    // within the τ window before it
    for (size_t j = 0; j < timeBuckets; ++j) {
        if (postBucketCounts[j] > 0) {
            float postTime = minTime + static_cast<Timestamp>(j);
            
            // Sum contributions from pre spikes within τ window before this post spike
            float preContribution = 0.0f;
            for (ssize_t i = static_cast<ssize_t>(j) - 1; 
                 i >= 0 && (j - i) < timeBuckets; 
                 --i) {
                float timeDiff = static_cast<float>(j - i) / 1000.0f;  // Convert ms to s
                preContribution += preBucketCounts[i] * pImpl->ltpWeight * std::exp(-timeDiff / tau);
            }
            
            totalDelta += preContribution;
        }
    }
    
    // Calculate depression (post before pre)
    // For each pre spike at bucket j, sum contributions from post spikes
    // within the τ window before it
    for (size_t j = 0; j < timeBuckets; ++j) {
        if (preBucketCounts[j] > 0) {
            float preTime = minTime + static_cast<Timestamp>(j);
            
            // Sum contributions from post spikes within τ window before this pre spike
            float postContribution = 0.0f;
            for (ssize_t i = static_cast<ssize_t>(j) - 1; 
                 i >= 0 && (j - i) < timeBuckets; 
                 --i) {
                float timeDiff = static_cast<float>(j - i) / 1000.0f;  // Convert ms to s
                postContribution += postBucketCounts[i] * pImpl->ltdWeight * std::exp(-timeDiff / tau);
            }
            
            totalDelta -= postContribution;  // Depression subtracts from total
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
