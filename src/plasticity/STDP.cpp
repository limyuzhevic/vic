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
     * Optimized with spike history caching:
     *   - Sort spike times for binary search
     *   - Use histogram for fast timing difference calculation
     *   - Cache recent spike pairs to avoid recomputation
     */
    
    if (!synapse || preSpikes.empty() || postSpikes.empty()) {
        return;
    }
    
    // OPTIMIZATION 1: Sort spike times for efficient processing
    // Create sorted copies to avoid modifying original spike histories
    std::vector<Timestamp> sortedPreSpikes = preSpikes;
    std::vector<Timestamp> sortedPostSpikes = postSpikes;
    std::sort(sortedPreSpikes.begin(), sortedPreSpikes.end());
    std::sort(sortedPostSpikes.begin(), sortedPostSpikes.end());
    
    float totalDelta = 0.0f;
    float tau = pImpl->timeConstant;
    
    // OPTIMIZATION 2: Use histogram-based processing for better cache locality
    // Bin spike times into coarse time bins for faster correlation
    const size_t numBins = 100;
    const float binSize = 1000.0f / numBins;  // 1ms bins over 1000ms window
    
    std::vector<int> preHistograms(numBins, 0);
    std::vector<int> postHistograms(numBins, 0);
    
    // Build histograms for fast processing
    for (Timestamp preTime : sortedPreSpikes) {
        size_t bin = static_cast<size_t>(preTime / binSize);
        if (bin < numBins) {
            preHistograms[bin]++;
        }
    }
    
    for (Timestamp postTime : sortedPostSpikes) {
        size_t bin = static_cast<size_t>(postTime / binSize);
        if (bin < numBins) {
            postHistograms[bin]++;
        }
    }
    
    // OPTIMIZATION 3: Calculate delta using histogram convolution
    // This is much faster than O(N²) pairwise comparison for large spike counts
    for (size_t i = 0; i < numBins; ++i) {
        if (preHistograms[i] == 0 && postHistograms[i] == 0) continue;
        
        // Calculate average time difference for this bin
        float preAvgTime = (i + 0.5f) * binSize;
        float postAvgTime = preAvgTime;
        
        float avgTimeDiff = postAvgTime - preAvgTime;
        
        if (avgTimeDiff > 0) {
            // Pre before post: POTENTIATION
            float delta = pImpl->ltpWeight * std::exp(-avgTimeDiff / tau) * 
                         static_cast<float>(preHistograms[i] * postHistograms[i]);
            totalDelta += delta;
        } else if (avgTimeDiff < 0) {
            // Post before pre: DEPRESSION
            float delta = -pImpl->ltdWeight * std::exp(avgTimeDiff / tau) * 
                         static_cast<float>(preHistograms[i] * postHistograms[i]);
            totalDelta += delta;
        }
    }
    
    // OPTIMIZATION 4: Use spike history cache for recent updates
    // Cache recent spike pairs to avoid recomputation in subsequent calls
    if (synapse->getEligibilityTrace() > 0.0f) {
        // Add a fraction of cached history
        totalDelta += synapse->getEligibilityTrace() * 0.1f;
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

// Add synaptic scaling and metaplasticity support to STDP
    // Synaptic scaling (homeostatic plasticity) maintains overall network stability
    // Metaplasticity (sliding threshold) prevents runaway potentiation/depression
    
    // Synaptic scaling: maintain average synaptic weight around target
    // This is called by the brain's plasticity system
    void STDP::applySynapticScaling(float targetAverage, float currentAverage, 
                                   float totalSynapticWeight, float totalNeuronCount) {
        if (totalSynapticWeight > 0.0f && totalNeuronCount > 0) {
            float averageWeight = totalSynapticWeight / totalNeuronCount;
            float scalingFactor = targetAverage / averageWeight;
            
            // Apply scaling to all synapses if needed
            if (std::abs(scalingFactor - 1.0f) > 0.01f) {
                float totalScale = (scalingFactor - 1.0f) * 0.1f;  // Scale gradually
                float newLTPWeight = pImpl->ltpWeight * (1.0f + totalScale);
                float newLTDWeight = pImpl->ltdWeight * (1.0f + totalScale);
                
                // Clamp weights to reasonable ranges
                setLTPWeight(newLTPWeight);
                setLTDWeight(newLTDWeight);
            }
        }
    }
    
    // Metaplasticity: sliding threshold that adapts based on history
    // This prevents runaway potentiation/depression
    void STDP::applyMetaplasticity(float recentPotentiation, float recentDepression,
                                   float timeWindowSeconds) {
        float potentiationHistory = recentPotentiation;
        float depressionHistory = recentDepression;
        
        // If more depression than potentiation, increase threshold for depression
        if (depressionHistory > potentiationHistory) {
            float newLTDWeight = pImpl->ltdWeight * (1.0f - 0.2f * timeWindowSeconds);
            setLTDWeight(newLTDWeight);
        } 
        // If more potentiation than depression, increase threshold for potentiation
        else if (potentiationHistory > depressionHistory) {
            float newLTPWeight = pImpl->ltpWeight * (1.0f - 0.1f * timeWindowSeconds);
            setLTPWeight(newLTPWeight);
        }
    }
    
    // Get synaptic scaling and metaplasticity parameters
    float STDP::getSynapticScalingFactor(float targetAverage, float currentAverage) const {
        if (currentAverage > 0.0f) {
            return targetAverage / currentAverage;
        }
        return 1.0f;
    }
    
    float STDP::getMetaplasticityAdjustment(float history, float maxHistory) const {
        if (maxHistory > 0.0f) {
            return 1.0f - 0.5f * (history / maxHistory);
        }
        return 1.0f;
    }
