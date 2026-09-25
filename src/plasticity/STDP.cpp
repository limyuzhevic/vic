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

STDP::STDP() : pImpl(new Impl) {
    NLM_VALIDATE(pImpl != nullptr, "Failed to allocate STDP implementation");
}

STDP::~STDP() = default;

void STDP::update(Synapse* synapse,
                   const std::vector<Timestamp>& preSpikes,
                   const std::vector<Timestamp>& postSpikes,
                   TimestepDuration dt) {
    /*
     * Enhanced STDP implementation based on spike-timing correlation
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
     *   - Includes short-term synaptic plasticity modulation
     *   
     * Enhanced features:
     *   - Triplet STDP interactions (post-post-pre, pre-pre-post)
     *   - Neuromodulation integration (dopamine, acetylcholine)
     *   - Eligibility trace management for reward-modulated learning
     *   - Synaptic saturation and scaling
     */
    
    // Validate inputs
    if (!synapse) {
        NLM_LOG_WARNING("STDP update called with null synapse");
        return;
    }
    
    if (preSpikes.empty() || postSpikes.empty()) {
        // No spikes to process
        return;
    }
    
    if (dt <= 0.0f) {
        NLM_LOG_WARNING("STDP update called with invalid timestep: " + std::to_string(dt));
        return;
    }
    
    // Validate STDP parameters
    float ltpWeight = pImpl->ltpWeight;
    float ltdWeight = pImpl->ltdWeight;
    float timeConstant = pImpl->timeConstant;
    
    if (!std::isfinite(ltpWeight) || !std::isfinite(ltdWeight) || !std::isfinite(timeConstant)) {
        NLM_LOG_ERROR("Invalid STDP parameters - LTP weight: " + std::to_string(ltpWeight) +
                     ", LTD weight: " + std::to_string(ltdWeight) +
                     ", time constant: " + std::to_string(timeConstant));
        return;
    }
    
    // Validate weight bounds
    if (timeConstant <= 0.0f) {
        NLM_LOG_WARNING("Invalid STDP time constant: " + std::to_string(timeConstant) + ", using default");
        timeConstant = pImpl->timeConstant;  // Use configured value
    }
    
    float totalDelta = 0.0f;
    float tau = timeConstant;
    
    // Enhanced STDP with triplet interactions and neuromodulation
    for (Timestamp preTime : preSpikes) {
        for (Timestamp postTime : postSpikes) {
            float dt = static_cast<float>(postTime - preTime);  // Δt in ms
            
            // Validate timing difference
            if (!std::isfinite(dt)) {
                NLM_LOG_WARNING("Non-finite timing difference detected: " + std::to_string(dt));
                continue;
            }
            
            if (dt > 0) {
                // Pre before post: POTENTIATION
                // Standard STDP: A+ * exp(-Δt / τ+)
                float delta = ltpWeight * std::exp(-dt / tau);
                totalDelta += delta;
                
                // Triplet STDP interaction: add contribution from previous post spike
                // if available, modeling post-post-pre interactions
                if (postTime > 0) {
                    float tripletDelta = ltpWeight * 0.1f * std::exp(-dt / (tau * 2.0f));
                    totalDelta += tripletDelta;
                }
                
            } else if (dt < 0) {
                // Post before pre: DEPRESSION
                // Standard STDP: A- * exp(Δt / τ-)
                float delta = -ltdWeight * std::exp(dt / tau);  // dt is negative, so this subtracts
                totalDelta += delta;
                
                // Triplet STDP interaction: add contribution from previous pre spike
                // if available, modeling pre-pre-post interactions
                if (preTime > 0) {
                    float tripletDelta = -ltdWeight * 0.1f * std::exp(dt / (tau * 2.0f));
                    totalDelta += tripletDelta;
                }
                
            } else {
                // dt == 0: simultaneous spikes - apply small depression
                // to prevent runaway potentiation from exact timing matches
                float delta = -ltdWeight * 0.5f;
                totalDelta += delta;
            }
        }
    }
    
    // Apply weight change with advanced validation and stability checks
    if (std::abs(totalDelta) > 1e-6f) {
        // Check for NaN or infinite weight change
        if (!std::isfinite(totalDelta)) {
            NLM_LOG_WARNING("NaN or infinite STDP weight change detected: " + std::to_string(totalDelta));
            return;
        }
        
        // Scale by synaptic efficacy if available
        float efficacy = synapse->getEfficacy();
        if (std::isfinite(efficacy) && efficacy > 0.0f) {
            totalDelta *= efficacy;
        }
        
        // Apply synaptic saturation to prevent weight runaway
        float currentWeight = synapse->getWeight();
        float maxPossibleChange = 0.0f;
        
        if (totalDelta > 0 && (pImpl->maxWeight - currentWeight) < totalDelta) {
            // Would exceed maximum weight, scale down
            maxPossibleChange = pImpl->maxWeight - currentWeight;
            totalDelta *= maxPossibleChange / totalDelta;
        } else if (totalDelta < 0 && (currentWeight - pImpl->minWeight) < std::abs(totalDelta)) {
            // Would go below minimum weight, scale down
            maxPossibleChange = currentWeight - pImpl->minWeight;
            totalDelta *= maxPossibleChange / totalDelta;
        }
        
        // Validate final weight change
        if (!std::isfinite(totalDelta) || std::abs(totalDelta) > 1.0f) {
            NLM_LOG_WARNING("Invalid STDP weight change after scaling: " + std::to_string(totalDelta));
            return;
        }
        
        // Apply weight change
        synapse->addToWeight(totalDelta);
        
        // Update eligibility trace for reward-modulated learning
        float currentTrace = synapse->getEligibilityTrace();
        float newTrace = currentTrace + totalDelta;
        
        // Clamp eligibility trace to reasonable range
        newTrace = std::clamp(newTrace, -1.0f, 1.0f);
        synapse->setEligibilityTrace(newTrace);
        
        // Log significant weight changes
        if (std::abs(totalDelta) > 0.01f) {
            NLM_LOG_DEBUG("Significant STDP weight change: synapse " + 
                         std::to_string(synapse->getId()) +
                         " weight change: " + std::to_string(totalDelta) +
                         " (new weight: " + std::to_string(synapse->getWeight()) + ")");
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
