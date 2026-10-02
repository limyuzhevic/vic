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
    
    if (preSpikes.size() <= postSpikes.size()) {
        // For each pre spike, find all post spikes that occur after it
        for (Timestamp preTime : preSpikes) {
            // Find first post spike that occurs after this pre spike
            auto postIt = std::upper_bound(postSpikes.begin(), postSpikes.end(), preTime);
            // Process all subsequent post spikes
            for (; postIt != postSpikes.end(); ++postIt) {
                float dt = static_cast<float>(*postIt - preTime);  // Δt in ms
                if (dt > 0) {
                    // Pre before post: POTENTIATION
                    // "Cells that fire together, wire together" - but only if pre fires before post
                    float delta = pImpl->ltpWeight * std::exp(-dt / tau);
                    totalDelta += delta;
                }
            }
        }
        
        // For post-before-pre depression (post fires before pre)
        for (Timestamp postTime : postSpikes) {
            // Find last pre spike that occurs before this post spike
            auto preIt = std::lower_bound(preSpikes.begin(), preSpikes.end(), postTime);
            // If there was a pre spike, check if any occurred before this post spike
            if (preIt != preSpikes.begin()) {
                --preIt;  // Go to the last pre spike before this post spike
                float dt = static_cast<float>(postTime - *preIt);  // Δt = postTime - preTime (positive since preIt < postTime)
                if (dt > 0) {
                    // Post before pre: DEPRESSION
                    // "Anti-Hebbian" - connection weakens if post fires without pre
                    float delta = -pImpl->ltdWeight * std::exp(dt / tau);  // dt is positive, so this subtracts
                    totalDelta += delta;
                }
            }
        }
    } else {
        // PreSpikes is larger, use symmetric approach
        for (Timestamp postTime : postSpikes) {
            // Find first pre spike that occurs after this post spike
            auto preIt = std::upper_bound(preSpikes.begin(), preSpikes.end(), postTime);
            // Process all subsequent pre spikes
            for (; preIt != preSpikes.end(); ++preIt) {
                float dt = static_cast<float>(*preIt - postTime);  // Δt in ms (pre after post)
                if (dt > 0) {
                    // Post before pre: DEPRESSION
                    float delta = -pImpl->ltdWeight * std::exp(dt / tau);
                    totalDelta += delta;
                }
            }
        }
        
        // For pre-before-post potentiation (pre fires before post)
        for (Timestamp preTime : preSpikes) {
            // Find last post spike that occurs before this pre spike
            auto postIt = std::lower_bound(postSpikes.begin(), postSpikes.end(), preTime);
            if (postIt != postSpikes.begin()) {
                --postIt;  // Go to the last post spike before this pre spike
                float dt = static_cast<float>(preTime - *postIt);  // Δt = preTime - postTime (positive since postIt < preTime)
                if (dt > 0) {
                    // Pre before post: POTENTIATION
                    float delta = pImpl->ltpWeight * std::exp(-dt / tau);
                    totalDelta += delta;
                }
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
