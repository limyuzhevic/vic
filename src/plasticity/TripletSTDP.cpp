// Triplet STDP (Intrinsic Plasticity)
// Implementation of TripletSTDP

#include "STDP.hpp"
#include "../../brain/Synapse.hpp"
#include <cmath>
#include <algorithm>

namespace nlm {

struct TripletSTDP::Impl {
    float a_plus;       // Presynaptic triplet potentiation factor
    float a_minus;      // Postsynaptic triplet depression factor
    float b_plus;       // Presynaptic facilitation factor
    float b_minus;      // Postsynaptic depression factor
    float tau_plus;     // Presynaptic facilitation time constant (ms)
    float tau_minus;    // Postsynaptic depression time constant (ms)
    float minWeight;    // Minimum synaptic weight
    float maxWeight;    // Maximum synaptic weight
    
    // Triplet state variables
    float x_plus;       // Presynaptic facilitation variable
    float x_minus;      // Postsynaptic depression variable
    
    Impl() : a_plus(0.01f), a_minus(0.012f), b_plus(0.5f), b_minus(0.5f),
             tau_plus(100.0f), tau_minus(100.0f), minWeight(-1.0f), maxWeight(1.0f),
             x_plus(0.0f), x_minus(0.0f) {}
};

TripletSTDP::TripletSTDP() : pImpl(new Impl) {}

TripletSTDP::~TripletSTDP() = default;

void TripletSTDP::update(Synapse* synapse,
                        const std::vector<Timestamp>& preSpikes,
                        const std::vector<Timestamp>& postSpikes,
                        TimestepDuration dt) {
    /*
     * Triplet STDP (Intrinsic Plasticity)
     * 
     * Implements triplet-based synaptic plasticity that considers:
     *   - Presynaptic spike triplets (facilitation)
     *   - Postsynaptic spike triplets (depression)
     *   - Interactions between different spike timing patterns
     *   
     * Mathematical formulation (from Pfister et al., 2006):
     * 
     * Presynaptic facilitation:
     *   x_plus(t) = x_plus(t-1) * exp(-dt/tau_plus) + b_plus * sum_of_presynaptic_spikes
     *   Δw += a_plus * x_plus(t) * sum_of_postsynaptic_spikes
     *   
     * Postsynaptic depression:
     *   x_minus(t) = x_minus(t-1) * exp(-dt/tau_minus) + b_minus * sum_of_postsynaptic_spikes
     *   Δw -= a_minus * sum_of_presynaptic_spikes * x_minus(t)
     *   
     * Biological inspiration:
     *   - Captures synaptic dynamics at the molecular level
     *   - Includes short-term plasticity effects
     *   - Models calcium-dependent processes
     */
    
    if (!synapse || preSpikes.empty() || postSpikes.empty()) {
        return;
    }
    
    float dtMs = static_cast<float>(dt) * 1000.0f;
    
    // Update presynaptic facilitation variable
    float preSpikeCount = static_cast<float>(preSpikes.size());
    pImpl->x_plus *= std::exp(-dtMs / pImpl->tau_plus);
    pImpl->x_plus += b_plus * preSpikeCount;
    
    // Update postsynaptic depression variable
    float postSpikeCount = static_cast<float>(postSpikes.size());
    pImpl->x_minus *= std::exp(-dtMs / pImpl->tau_minus);
    pImpl->x_minus += b_minus * postSpikeCount;
    
    // Compute weight change from triplet interactions
    float totalDelta = 0.0f;
    
    // Presynaptic potentiation: depends on presynaptic facilitation and postsynaptic spikes
    totalDelta += pImpl->a_plus * pImpl->x_plus * postSpikeCount;
    
    // Postsynaptic depression: depends on postsynaptic depression and presynaptic spikes
    totalDelta -= pImpl->a_minus * preSpikeCount * pImpl->x_minus;
    
    // Apply weight change
    if (std::abs(totalDelta) > 1e-6f) {
        // Scale by synaptic efficacy
        float efficacy = synapse->getEfficacy();
        totalDelta *= efficacy;
        
        // Apply weight change
        synapse->addToWeight(totalDelta);
        
        // Update eligibility trace
        float currentTrace = synapse->getEligibilityTrace();
        synapse->setEligibilityTrace(currentTrace + totalDelta);
    }
}

void TripletSTDP::applyWeightChange(Synapse* synapse, SynapticWeight delta) {
    if (!synapse) return;
    
    // Clamp weight to reasonable bounds
    float newWeight = synapse->getWeight() + delta;
    newWeight = std::clamp(newWeight, -1.0f, 1.0f);
    synapse->setWeight(newWeight);
}

const char* TripletSTDP::getName() const {
    return "TripletSTDP";
}

} // namespace nlm
