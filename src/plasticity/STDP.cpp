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

STDP::~STDP() {
    if (pImpl != nullptr) {
        delete pImpl;
        pImpl = nullptr;
    }
}

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
    
    // Validate input parameters
    if (!synapse) {
        NLM_LOG_ERROR("Null synapse pointer in STDP::update");
        return;
    }
    
    if (preSpikes.empty()) {
        NLM_LOG_DEBUG("No pre-spikes provided to STDP update");
    }
    
    if (postSpikes.empty()) {
        NLM_LOG_DEBUG("No post-spikes provided to STDP update");
    }
    
    if (dt <= 0.0f) {
        NLM_LOG_ERROR("Invalid timestep duration in STDP::update: " + std::to_string(dt));
        return;
    }
    
    float totalDelta = 0.0f;
    float tau = pImpl->timeConstant;
    
    // Validate time constant
    if (tau <= 0.0f) {
        NLM_LOG_ERROR("Invalid time constant in STDP::update: " + std::to_string(tau));
        return;
    }
    
    for (Timestamp preTime : preSpikes) {
        for (Timestamp postTime : postSpikes) {
            float dt_calc = static_cast<float>(postTime - preTime);  // Δt in ms
            
            // Protect against numerical overflow in exp
            if (std::abs(dt_calc) > 1000.0f) {
                NLM_LOG_WARNING("Extremely large time difference in STDP update: " + std::to_string(dt_calc));
            }
            
            if (dt_calc > 0) {
                // Pre before post: POTENTIATION
                // "Cells that fire together, wire together" - but only if pre fires before post
                float delta = pImpl->ltpWeight * std::exp(-dt_calc / tau);
                totalDelta += delta;
            } else if (dt_calc < 0) {
                // Post before pre: DEPRESSION
                // "Anti-Hebbian" - connection weakens if post fires without pre
                float delta = -pImpl->ltdWeight * std::exp(dt_calc / tau);  // dt_calc is negative, so this subtracts
                totalDelta += delta;
            }
            // dt_calc == 0: no change (simultaneous spikes - rare in practice)
        }
    }
    
    // Apply weight change with bounds
    if (std::abs(totalDelta) > 1e-6f) {
        // Scale by synaptic efficacy if available
        float efficacy = synapse->getEfficacy();
        totalDelta *= efficacy;
        
        // Protect against numerical overflow
        if (std::abs(totalDelta) > 10.0f) {
            NLM_LOG_WARNING("Large STDP weight change detected: " + std::to_string(totalDelta));
            totalDelta = std::clamp(totalDelta, -10.0f, 10.0f);
        }
        
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
