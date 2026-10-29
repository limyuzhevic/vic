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
    // Null pointer safety checks
    if (!pImpl || !synapse || preSpikes.empty() || postSpikes.empty()) {
        return;
    }
    
    // Validate timestep duration
    if (dt <= 0.0) {
        return;
    }
    
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
     
     float totalDelta = 0.0f;
     float tau = pImpl->timeConstant;
     
     // Validate time constant before using it
     if (tau <= 0.0f) {
         return;
     }
     
     for (Timestamp preTime : preSpikes) {
         for (Timestamp postTime : postSpikes) {
             float deltaTime = static_cast<float>(postTime - preTime);  // Δt in ms
             
             if (deltaTime > 0) {
                 // Pre before post: POTENTIATION
                 // "Cells that fire together, wire together" - but only if pre fires before post
                 float delta = pImpl->ltpWeight * std::exp(-deltaTime / tau);
                 totalDelta += delta;
             } else if (deltaTime < 0) {
                 // Post before pre: DEPRESSION
                 // "Anti-Hebbian" - connection weakens if post fires without pre
                 float delta = -pImpl->ltdWeight * std::exp(deltaTime / tau);  // deltaTime is negative, so this subtracts
                 totalDelta += delta;
             }
             // deltaTime == 0: no change (simultaneous spikes - rare in practice)
         }
     }
     
     // Apply weight change with bounds
     if (std::abs(totalDelta) > 1e-6f) {
         // Scale by synaptic efficacy if available
         float efficacy = synapse->getEfficacy();
         
         // Validate efficacy before using it
         if (std::isnan(efficacy) || efficacy < 0.0f) {
             efficacy = 1.0f;
         }
         
         totalDelta *= efficacy;
         
         // Apply weight change
         synapse->addToWeight(totalDelta);
         
         // Update eligibility trace for reward-modulated learning
         float currentTrace = synapse->getEligibilityTrace();
         synapse->setEligibilityTrace(currentTrace + totalDelta);
     }
}

void STDP::applyWeightChange(Synapse* synapse, SynapticWeight delta) {
    // Null pointer safety checks
    if (!pImpl || !synapse) {
        return;
    }
    
    // Validate delta parameter
    if (std::isnan(delta)) {
        return;
    }
    
    // Clamp weight to bounds
    float currentWeight = synapse->getWeight();
    
    // Validate current weight
    if (std::isnan(currentWeight)) {
        currentWeight = 0.0f;
    }
    
    float newWeight = currentWeight + delta;
    newWeight = std::clamp(newWeight, pImpl->minWeight, pImpl->maxWeight);
    
    // Validate min/max weights before clamping
    if (pImpl->minWeight > pImpl->maxWeight) {
        // Swap min/max to ensure valid bounds
        std::swap(pImpl->minWeight, pImpl->maxWeight);
    }
    
    // Final validation before setting weight
    if (newWeight < pImpl->minWeight || newWeight > pImpl->maxWeight ||
        std::isnan(newWeight)) {
        return;
    }
    
    synapse->setWeight(newWeight);
}

const char* STDP::getName() const {
    return "STDP";
}

void STDP::setLTPWeight(float weight) {
    // Bounds checking: weight must be in valid range [0.0, 1.0]
    if (std::isnan(weight) || weight < 0.0f || weight > 1.0f) {
        return; // Silently reject invalid values
    }
    pImpl->ltpWeight = weight;
}

float STDP::getLTPWeight() const {
    return pImpl->ltpWeight;
}

void STDP::setLTDWeight(float weight) {
    // Bounds checking: weight must be in valid range [0.0, 1.0]
    if (std::isnan(weight) || weight < 0.0f || weight > 1.0f) {
        return; // Silently reject invalid values
    }
    pImpl->ltdWeight = weight;
}

float STDP::getLTDWeight() const {
    return pImpl->ltdWeight;
}

void STDP::setTimeConstant(float tau) {
    // Bounds checking: time constant must be in valid range [1.0, 100.0] ms
    if (std::isnan(tau) || tau < 1.0f || tau > 100.0f) {
        return; // Silently reject invalid values
    }
    pImpl->timeConstant = tau;
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
