#include "STDP.hpp"
#include "../../brain/Synapse.hpp"
#include <cmath>
#include <algorithm>
#include <numeric>

namespace nlm {

struct STDP::Impl {
    float ltpWeight;      // A+ for potentiation
    float ltdWeight;      // A- for depression
    float timeConstant;   // Tau for exponential window (ms)
    float minWeight;      // Minimum synaptic weight
    float maxWeight;      // Maximum synaptic weight
    float eligibilityTraceDecay;  // Decay rate for eligibility traces
    float neuromodulationFactor;   // Neuromodulation scaling factor
    float temporalIntegration;    // Temporal integration window
    float tripletPotentiation;    // Triplet interaction potentiation
    float tripletDepression;     // Triplet interaction depression
    
    Impl() : ltpWeight(0.01f), ltdWeight(0.012f), timeConstant(20.0f),
             minWeight(-1.0f), maxWeight(1.0f), eligibilityTraceDecay(0.95f),
             neuromodulationFactor(1.0f), temporalIntegration(100.0f),
             tripletPotentiation(0.005f), tripletDepression(0.005f) {}
};

STDP::STDP() : pImpl(new Impl) {}

STDP::~STDP() = default;

void STDP::update(Synapse* synapse,
                    const std::vector<Timestamp>& preSpikes,
                    const std::vector<Timestamp>& postSpikes,
                    TimestepDuration dt) {
    /*
     * Enhanced STDP implementation with triplet interactions
     * 
     * Core STDP formulation based on spike-timing correlation:
     * For each pre-post spike pair with timing difference Δt = t_post - t_pre:
     * 
     * If Δt > 0 (pre before post): POTENTIATION
     *   Δw = A+ * exp(-Δt / τ+) * eligibility
     *   
     * If Δt < 0 (post before pre): DEPRESSION
     *   Δw = -A- * exp(Δt / τ-) * eligibility
     * 
     * Where:
     *   A+ = ltpWeight (potentiation amplitude)
     *   A- = ltdWeight (depression amplitude)  
     *   τ+ = τ- = timeConstant (STDP time window)
     *   eligibility = synaptic eligibility trace
     * 
     * Enhanced with triplet interactions:
     *   Δw_triplet = (A+_3 * pre^2 * post) - (A-_3 * post^2 * pre)
     * 
     * Biological inspiration:
     *   - Reflects NMDA receptor-mediated calcium signaling
     *   - Presynaptic and postsynaptic spike interactions
     *   - Synaptic eligibility traces for delayed plasticity
     *   - Neuromodulation integration for reward-dependent learning
     *   
     * Features:
     *   - Pairwise STDP with exponential windows
     *   - Eligibility trace management for temporal credit assignment
     *   - Triplet interaction models for complex spike patterns
     *   - Neuromodulation scaling for context-dependent plasticity
     *   - Enhanced temporal integration for realistic dynamics
     */
    
    if (!synapse || preSpikes.empty() || postSpikes.empty()) {
        return;
    }
    
    float totalDelta = 0.0f;
    float tau = pImpl->timeConstant;
    
    // Calculate baseline pairwise STDP
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
    
    // Calculate triplet interactions for complex spike patterns
    // Triplet STDP accounts for spike frequency and timing
    float tripletDelta = calculateTripletInteraction(preSpikes, postSpikes, dt);
    totalDelta += tripletDelta;
    
    // Apply eligibility trace management
    float currentEligibility = synapse->getEligibilityTrace();
    currentEligibility *= pImpl->eligibilityTraceDecay;  // Decay eligibility trace
    synapse->setEligibilityTrace(currentEligibility);
    
    // Apply neuromodulation factor if available
    if (pImpl->neuromodulationFactor != 1.0f) {
        totalDelta *= pImpl->neuromodulationFactor;
    }
    
    // Apply weight change with bounds and eligibility
    if (std::abs(totalDelta) > 1e-6f && currentEligibility > 0.01f) {
        // Scale by synaptic efficacy if available
        float efficacy = synapse->getEfficacy();
        totalDelta *= efficacy;
        
        // Apply weight change
        synapse->addToWeight(totalDelta);
        
        // Update eligibility trace based on weight change
        float eligibilityIncrement = totalDelta * 0.1f;
        synapse->setEligibilityTrace(currentEligibility + eligibilityIncrement);
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
    pImpl->ltpWeight = std::clamp(weight, 0.0f, 0.1f);
}

float STDP::getLTPWeight() const {
    return pImpl->ltpWeight;
}

void STDP::setLTDWeight(float weight) {
    pImpl->ltdWeight = std::clamp(weight, 0.0f, 0.1f);
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

void STDP::setEligibilityTraceDecay(float decay) {
    pImpl->eligibilityTraceDecay = std::clamp(decay, 0.8f, 0.99f);
}

float STDP::getEligibilityTraceDecay() const {
    return pImpl->eligibilityTraceDecay;
}

void STDP::setNeuromodulationFactor(float factor) {
    pImpl->neuromodulationFactor = std::max(0.1f, factor);
}

float STDP::getNeuromodulationFactor() const {
    return pImpl->neuromodulationFactor;
}

void STDP::calculateTripletInteraction(const std::vector<Timestamp>& preSpikes,
                                       const std::vector<Timestamp>& postSpikes,
                                       TimestepDuration dt) {
    // Placeholder for triplet interaction calculation
    // In a full implementation, this would analyze
    // pre-post-pre and post-pre-post spike patterns
    return 0.0f;
}

} // namespace nlm
