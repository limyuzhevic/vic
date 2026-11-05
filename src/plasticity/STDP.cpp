#include "STDP.hpp"
#include "../brain/Synapse.hpp"
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

    // Phase 6: Enhanced STDP learning with competition and activity-dependent rules
    // Improved to prevent learning saturation and promote selective pattern learning
    
    if (!synapse || preSpikes.empty() || postSpikes.empty()) {
        return;
    }
    
    float totalDelta = 0.0f;
    float tau = pImpl->timeConstant;

    // Nearest-neighbour STDP: only the most recent pre/post pair is used.
    // The old all-pairs loop re-applied every historic pair on every step,
    // so weights saturated to their bounds in a few steps and the cost was
    // O(history^2) per synapse per step. Timestamps here are in seconds
    // (Brain steps at 1ms), so convert to ms for the tau window.
    float dtMs = static_cast<float>((postSpikes.back() - preSpikes.back()) * 1000.0);
    
    // Enhanced STDP: only learn when timing is appropriate and synapse is effective
    bool shouldLearn = false;
    if (dtMs > 0) {
        // Pre-before-post potentiation: stronger when timing is optimal
        if (dtMs > 1.0f && dtMs < 50.0f) {  // Optimal STDP window
            shouldLearn = true;
            totalDelta += pImpl->ltpWeight * std::exp(-dtMs / tau);
        }
    } else if (dtMs < 0) {
        // Post-before-pre depression: stronger when timing is optimal
        if (dtMs > -50.0f && dtMs < -1.0f) {  // Optimal LTD window
            shouldLearn = true;
            totalDelta += -pImpl->ltdWeight * std::exp(dtMs / tau);
        }
    }
    
    if (!shouldLearn) {
        return;
    }
    
    // Apply weight change with enhanced competition mechanism
    if (std::abs(totalDelta) > 1e-6f) {
        // Scale by synaptic efficacy if available
        float efficacy = synapse->getEfficacy();
        totalDelta *= efficacy;

        // Inhibitory plasticity must strengthen inhibition, not flip it:
        // pre-before-post on an inhibitory synapse should make its weight
        // more negative, not positive. Without this, every inhibitory
        // synapse drifted through zero and became excitatory.
        if (synapse->getType() == SynapseType::Inhibitory) {
            totalDelta = -totalDelta;
        }

        // Enhanced soft (weight-dependent) bounds with competition
        float w = synapse->getWeight();
        bool awayFromZero = std::abs(w + totalDelta) > std::abs(w);
        
        // Competition-based scaling: learning strength depends on relative effectiveness
        // Strong synapses (near bounds) learn slower, weak synapses learn faster
        float learningScale = 1.0f;
        
        if (awayFromZero) {
            // Potentiation (away from zero): scale by (1 - |w|) as before
            learningScale = (1.0f - std::abs(w));
            
            // Additional competition: if synapse is strong, reduce learning
            if (std::abs(w) > 0.7f) {
                learningScale *= 0.5f + 0.5f * (1.0f - std::abs(w));
            }
        } else {
            // Depression (toward zero): scale by |w| as before
            learningScale = std::abs(w);
            
            // Additional competition: if synapse is weak, reduce learning to prevent too much depression
            if (std::abs(w) < 0.3f) {
                learningScale *= 0.5f + std::abs(w);
            }
        }
        
        totalDelta *= learningScale;

        // Apply weight change
        synapse->addToWeight(totalDelta);
        // Enforce sign: excitatory stays >= 0.01, inhibitory stays <= -0.01.
        if (synapse->getType() == SynapseType::Inhibitory && synapse->getWeight() > -0.01f) {
            synapse->setWeight(-0.01f);
        } else if (synapse->getType() == SynapseType::Excitatory && synapse->getWeight() < 0.01f) {
            synapse->setWeight(0.01f);
        }
        
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
