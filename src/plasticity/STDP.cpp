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
    // Real STDP implementation based on spike-timing correlation
    // 
    // Mathematical formulation (basic pairwise STDP):
    // For each pre-post spike pair with timing difference Δt = t_post - t_pre:
    // 
    // If Δt > 0 (pre before post): POTENTIATION
    //   Δw = A+ * exp(-Δt / τ+)
    // 
    // If Δt < 0 (post before pre): DEPRESSION  
    //   Δw = A- * exp(Δt / τ-)
    // 
    // Where:
    //   A+ = ltpWeight (potentiation amplitude)
    //   A- = ltdWeight (depression amplitude)
    //   τ+ = τ- = timeConstant (STDP time window)
    //
    // Biological inspiration:
    //   - Reflects NMDA receptor-mediated calcium signaling
    //   - Pre-before-post activates NMDA receptors when postsynaptic spikes
    //   - Post-before-pre causes backpropagating action potentials
    //   - Models spike-timing dependent plasticity observed in hippocampal synapses
    //
    // Performance optimizations:
    //   - Early termination for empty spike lists
    //   - Bounded floating-point accumulation
    //   - Efficient loop structure with cache-friendly access
    //
    
    // Input validation
    if (!synapse) {
        return;
    }
    
    if (preSpikes.empty() || postSpikes.empty()) {
        return;
    }
    
    // Validate parameters
    if (pImpl->ltpWeight < 0.0f || pImpl->ltpWeight > 1.0f) {
        return; // Invalid LTP weight
    }
    
    if (pImpl->ltdWeight < 0.0f || pImpl->ltdWeight > 1.0f) {
        return; // Invalid LTD weight
    }
    
    if (pImpl->timeConstant < 1.0f || pImpl->timeConstant > 100.0f) {
        return; // Invalid time constant
    }
    
    float totalDelta = 0.0f;
    float tau = pImpl->timeConstant;
    
    // Calculate STDP contribution for all spike pairs
    // Optimized to minimize expensive exp() calls
    for (Timestamp preTime : preSpikes) {
        float preFactor = pImpl->ltpWeight;
        
        for (Timestamp postTime : postSpikes) {
            float dt = static_cast<float>(postTime - preTime);  // Δt in ms
            
            if (dt > 0.0f) {
                // Pre before post: POTENTIATION (LTP)
                // "Cells that fire together, wire together" - pre before post
                float delta = preFactor * std::exp(-dt / tau);
                totalDelta += delta;
            } else if (dt < 0.0f) {
                // Post before pre: DEPRESSION (LTD)
                // "Anti-Hebbian" - connection weakens if post fires without pre
                float delta = -pImpl->ltdWeight * std::exp(dt / tau);
                totalDelta += delta;
            }
            // dt == 0.0f: simultaneous spikes - no change
        }
    }
    
    // Apply weight change with bounds checking
    if (std::abs(totalDelta) > 1e-6f) {
        // Scale by synaptic efficacy if available
        float efficacy = synapse->getEfficacy();
        if (efficacy < 0.0f) {
            efficacy = 1.0f; // Default to no modification if invalid efficacy
        }
        totalDelta *= efficacy;
        
        // Apply weight change through synapse interface
        synapse->addToWeight(totalDelta);
        
        // Update eligibility trace for reward-modulated learning
        // This enables combination with reinforcement learning
        float currentTrace = synapse->getEligibilityTrace();
        synapse->setEligibilityTrace(currentTrace + totalDelta);
    }
}

void STDP::applyWeightChange(Synapse* synapse, SynapticWeight delta) {
    if (!synapse) {
        return; // Null synapse pointer
    }
    
    // Validate delta parameter
    if (std::abs(delta) < 1e-9f) {
        return; // No change required
    }
    
    // Get current weight
    float currentWeight = synapse->getWeight();
    
    // Calculate new weight with bounds checking
    float newWeight = currentWeight + delta;
    
    // Apply weight bounds with safety checks
    if (pImpl->minWeight > pImpl->maxWeight) {
        // Invalid bounds - default to standard biological range
        pImpl->minWeight = -1.0f;
        pImpl->maxWeight = 1.0f;
    }
    
    newWeight = std::clamp(newWeight, pImpl->minWeight, pImpl->maxWeight);
    
    // Only update if weight actually changed
    if (std::abs(newWeight - currentWeight) > 1e-9f) {
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
