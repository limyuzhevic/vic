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
    
    // PROTECTION: Ensure time constant is positive and reasonable
    if (tau <= 0.0f || tau > 10000.0f) {
        // Use default value
        tau = pImpl->timeConstant = STDP::DEFAULT_TIME_CONSTANT;
    }
    
    // OPTIMIZED STDP: Use spike time histograms instead of O(n²) pairwise calculation
    // Create histograms of spike times (quantized to 0.1ms bins)
    const float binSize = 0.1f;  // 100 bins per ms
    const size_t maxBins = static_cast<size_t>(tau * 10.0f) + 100;  // Cover the time window + buffer
    
    // PROTECTION: Prevent excessive memory allocation
    const size_t MAX_BINS = 100000;  // Cap at 100k bins
    if (maxBins > MAX_BINS) {
        return;  // Too many bins, abort calculation
    }
    
    std::vector<size_t> preHistogram(maxBins, 0);
    std::vector<size_t> postHistogram(maxBins, 0);
    
    // Fill histograms with spike counts
    for (Timestamp preTime : preSpikes) {
        size_t bin = static_cast<size_t>(preTime / binSize);
        if (bin < maxBins) {
            preHistogram[bin]++;
        }
    }
    
    for (Timestamp postTime : postSpikes) {
        size_t bin = static_cast<size_t>(postTime / binSize);
        if (bin < maxBins) {
            postHistogram[bin]++;
        }
    }
    
    // Calculate STDP contribution using convolution-like operation
    // Pre-before-post (LTP) for dt > 0
    for (size_t i = 0; i < maxBins; ++i) {
        if (preHistogram[i] == 0) continue;
        
        for (size_t j = i + 1; j < maxBins; ++j) {
            if (postHistogram[j] == 0) continue;
            
            float dt = static_cast<float>((j - i) * binSize);  // Δt in ms
            
            // PROTECTION: Prevent overflow in exponential calculation
            float exponent = -dt / tau;
            if (exponent > 700.0f) {  // exp(709) is near max float
                // Exponential would overflow, treat as effectively infinite
                float contribution = pImpl->ltpWeight;  // Max contribution
                totalDelta += contribution * static_cast<float>(preHistogram[i] * postHistogram[j]);
            } else if (exponent < -700.0f) {
                // Exponential would underflow, treat as 0
                continue;
            } else {
                float contribution = pImpl->ltpWeight * std::exp(exponent);
                
                // PROTECTION: Prevent overflow in multiplication
                size_t product = preHistogram[i] * postHistogram[j];
                if (product > 1000000) {  // Cap product to prevent overflow
                    product = 1000000;
                }
                totalDelta += contribution * static_cast<float>(product);
            }
        }
    }
    
    // Post-before-pre (LTD) for dt < 0
    for (size_t i = 1; i < maxBins; ++i) {
        if (postHistogram[i] == 0) continue;
        
        for (size_t j = 0; j < i; ++j) {
            if (preHistogram[j] == 0) continue;
            
            float dt = static_cast<float>((j - i) * binSize);  // Δt in ms (negative)
            
            // PROTECTION: Prevent overflow in exponential calculation
            float exponent = dt / tau;  // dt is negative, so this is negative
            if (exponent > 700.0f) {  // exp(709) is near max float
                // Exponential would overflow
                continue;
            } else if (exponent < -700.0f) {
                // Exponential would underflow, treat as 0
                continue;
            } else {
                float contribution = -pImpl->ltdWeight * std::exp(exponent);
                
                // PROTECTION: Prevent overflow in multiplication
                size_t product = postHistogram[i] * preHistogram[j];
                if (product > 1000000) {  // Cap product to prevent overflow
                    product = 1000000;
                }
                totalDelta += contribution * static_cast<float>(product);
            }
        }
    }
    
    if (totalDelta == 0.0f) {
        return;
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
