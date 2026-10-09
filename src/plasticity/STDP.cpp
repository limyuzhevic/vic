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
    
    for (Timestamp preTime : preSpikes) {
        for (Timestamp postTime : postSpikes) {
            float dt = static_cast<float>(postTime - preTime);  // Δt in ms
            
            if (dt > 0) {
                // Pre before post: POTENTIATION
                // "Cells that fire together, wire together" - but only if pre fires before post
                float delta = pImpl->ltpWeight * std::exp(-dt / tau);
                totalDelta += delta;
            } else if (dt < 0) {
                // Post before pre: DEPRESSION
                // "Anti-Hebbian" - connection weakens if post fires without pre
                float delta = -pImpl->ltdWeight * std::exp(dt / tau);  // dt is negative, so this subtracts
                totalDelta += delta;
            }
            // dt == 0: no change (simultaneous spikes - rare in practice)
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
    if (!synapse) {
        NLM_LOG_WARNING("Attempted to apply weight change to null synapse");
        return;
    }
    
    // Validate delta value to prevent extreme changes
    if (std::abs(delta) > 1.0f) {
        NLM_LOG_WARNING("Extreme weight delta: " + std::to_string(delta) + ", clamping to [-1.0, 1.0]");
        delta = std::clamp(delta, -1.0f, 1.0f);
    }
    
    // Clamp weight to bounds with better error handling
    float currentWeight = synapse->getWeight();
    float newWeight = currentWeight + delta;
    
    // Check for potential NaN or infinite values
    if (std::isnan(newWeight) || std::isinf(newWeight)) {
        NLM_LOG_ERROR("Invalid weight calculation: NaN or infinite value detected");
        return;
    }
    
    newWeight = std::clamp(newWeight, pImpl->minWeight, pImpl->maxWeight);
    
    // Only update if weight actually changed
    if (std::abs(newWeight - currentWeight) > 1e-6f) {
        synapse->setWeight(newWeight);
        NLM_LOG_DEBUG("Applied STDP weight change: " + std::to_string(currentWeight) + 
                     " -> " + std::to_string(newWeight) + " (delta=" + std::to_string(delta) + ")");
    } else {
        NLM_LOG_DEBUG("STDP weight change too small to apply: " + std::to_string(delta));
    }
}

const char* STDP::getName() const {
    return "STDP";
}

void STDP::setLTPWeight(float weight) {
    // Enhanced validation for LTP weight
    if (std::isnan(weight) || std::isinf(weight)) {
        NLM_LOG_ERROR("Invalid LTP weight: NaN or infinite value");
        return;
    }
    
    pImpl->ltpWeight = std::clamp(weight, 0.0f, 1.0f);
    NLM_LOG_DEBUG("Set STDP LTP weight to: " + std::to_string(pImpl->ltpWeight));
}

float STDP::getLTPWeight() const {
    return pImpl->ltpWeight;
}

void STDP::setLTDWeight(float weight) {
    // Enhanced validation for LTD weight
    if (std::isnan(weight) || std::isinf(weight)) {
        NLM_LOG_ERROR("Invalid LTD weight: NaN or infinite value");
        return;
    }
    
    pImpl->ltdWeight = std::clamp(weight, 0.0f, 1.0f);
    NLM_LOG_DEBUG("Set STDP LTD weight to: " + std::to_string(pImpl->ltdWeight));
}

float STDP::getLTDWeight() const {
    return pImpl->ltdWeight;
}

void STDP::setTimeConstant(float tau) {
    // Enhanced validation for time constant
    if (std::isnan(tau) || std::isinf(tau)) {
        NLM_LOG_ERROR("Invalid time constant: NaN or infinite value");
        return;
    }
    
    pImpl->timeConstant = std::clamp(tau, 1.0f, 100.0f);
    NLM_LOG_DEBUG("Set STDP time constant to: " + std::to_string(pImpl->timeConstant) + " ms");
}

float STDP::getTimeConstant() const {
    return pImpl->timeConstant;
}

void STDP::configure(float ltpWeight, float ltdWeight, float tau) {
    // Validate all configuration parameters
    if (std::isnan(ltpWeight) || std::isinf(ltpWeight) ||
        std::isnan(ltdWeight) || std::isinf(ltdWeight) ||
        std::isnan(tau) || std::isinf(tau)) {
        NLM_LOG_ERROR("Invalid STDP configuration parameters");
        return;
    }
    
    setLTPWeight(ltpWeight);
    setLTDWeight(ltdWeight);
    setTimeConstant(tau);
    
    NLM_LOG_INFO("Configured STDP: LTP=" + std::to_string(pImpl->ltpWeight) +
                 ", LTD=" + std::to_string(pImpl->ltdWeight) +
                 ", tau=" + std::to_string(pImpl->timeConstant));
}

} // namespace nlm
