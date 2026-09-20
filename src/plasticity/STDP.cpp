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
     * Enhanced features:
     *   - Pair-based STDP with exponential windows
     *   - Eligibility traces for reward modulation
     *   - Weight normalization to maintain stability
     *   - Bounded plasticity for biological plausibility
     *   - Spike count normalization for rate-dependent learning
     */
    
    if (!synapse || preSpikes.empty() || postSpikes.empty()) {
        return;
    }
    
    // Calculate average firing rates for normalization
    float preRate = static_cast<float>(preSpikes.size()) / dt;
    float postRate = static_cast<float>(postSpikes.size()) / dt;
    float avgRate = (preRate + postRate) * 0.5f;
    
    float totalDelta = 0.0f;
    float tau = pImpl->timeConstant;
    
    // More efficient computation: process unique spike times
    std::vector<Timestamp> uniquePreSpikes = preSpikes;
    std::vector<Timestamp> uniquePostSpikes = postSpikes;
    std::sort(uniquePreSpikes.begin(), uniquePreSpikes.end());
    std::sort(uniquePostSpikes.begin(), uniquePostSpikes.end());
    
    // Remove duplicates within small tolerance
    auto removeDuplicates = [](std::vector<Timestamp>& spikes, Timestamp tolerance = 1.0f) {
        if (spikes.empty()) return;
        std::vector<Timestamp> unique;
        unique.push_back(spikes[0]);
        for (size_t i = 1; i < spikes.size(); ++i) {
            if (spikes[i] - unique.back() > tolerance) {
                unique.push_back(spikes[i]);
            }
        }
        spikes = unique;
    };
    
    removeDuplicates(uniquePreSpikes);
    removeDuplicates(uniquePostSpikes);
    
    // Compute STDP using optimized pair-wise approach
    for (Timestamp preTime : uniquePreSpikes) {
        for (Timestamp postTime : uniquePostSpikes) {
            float deltaT = static_cast<float>(postTime - preTime);  // Δt in ms
            
            if (std::abs(deltaT) < 1e-6f) {
                continue; // Skip simultaneous spikes
            }
            
            if (deltaT > 0) {
                // Pre before post: POTENTIATION
                // Biologically: NMDA receptor activation, calcium influx
                float delta = pImpl->ltpWeight * std::exp(-deltaT / tau);
                
                // Normalize by firing rates for rate-dependent plasticity
                float rateFactor = std::min(avgRate / 20.0f, 1.0f);  // Moderate rate dependence
                delta *= (1.0f + 0.5f * rateFactor);
                
                totalDelta += delta;
            } else {
                // Post before pre: DEPRESSION
                // Biologically: backpropagating action potentials without NMDA
                float delta = -pImpl->ltdWeight * std::exp(deltaT / tau);  // deltaT is negative
                
                // Normalize by firing rates
                float rateFactor = std::min(avgRate / 20.0f, 1.0f);
                delta *= (1.0f + 0.5f * rateFactor);
                
                totalDelta += delta;
            }
        }
    }
    
    // Apply weight change with bounds and normalization
    if (std::abs(totalDelta) > 1e-6f) {
        // Scale by synaptic efficacy if available
        float efficacy = synapse->getEfficacy();
        if (efficacy > 0.0f) {
            totalDelta *= efficacy;
        }
        
        // Normalize total change based on current weight
        float currentWeight = synapse->getWeight();
        float maxChange = pImpl->maxWeight - pImpl->minWeight;
        float weightChangeLimit = maxChange * 0.1f;  // Limit change to 10% of range
        
        if (std::abs(totalDelta) > weightChangeLimit) {
            totalDelta = (totalDelta > 0) ? weightChangeLimit : -weightChangeLimit;
        }
        
        // Apply weight change
        float newWeight = currentWeight + totalDelta;
        newWeight = std::clamp(newWeight, pImpl->minWeight, pImpl->maxWeight);
        synapse->setWeight(newWeight);
        
        // Update eligibility trace for reward-modulated learning
        float eligibilityTrace = synapse->getEligibilityTrace();
        eligibilityTrace += totalDelta * 0.01f;  // Decay factor
        synapse->setEligibilityTrace(std::clamp(eligibilityTrace, -1.0f, 1.0f));
    }
}

void STDP::applyWeightChange(Synapse* synapse, SynapticWeight delta) {
    if (!synapse) return;
    
    // Clamp weight to bounds with gradual adjustment
    float currentWeight = synapse->getWeight();
    float newWeight = currentWeight + delta;
    newWeight = std::clamp(newWeight, pImpl->minWeight, pImpl->maxWeight);
    
    // Apply gradual weight change to prevent sudden shifts
    float maxChange = pImpl->maxWeight - pImpl->minWeight;
    float weightChange = newWeight - currentWeight;
    
    // Allow gradual weight change but still achieve target if reasonable
    if (std::abs(weightChange) > 1e-6f) {
        float changeStep = std::min(std::abs(weightChange), maxChange * 0.1f);
        if (weightChange > 0) {
            newWeight = std::min(currentWeight + changeStep, pImpl->maxWeight);
        } else {
            newWeight = std::max(currentWeight - changeStep, pImpl->minWeight);
        }
        synapse->setWeight(newWeight);
    }
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
