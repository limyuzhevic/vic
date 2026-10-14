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
     * The implementation includes:
     *   - Realistic time constants (10-100ms range)
     *   - Activity-dependent scaling with synaptic efficacy
     *   - Eligibility trace for reward-modulated STDP
     */
    
    if (!synapse || preSpikes.empty() || postSpikes.empty()) {
        return;
    }
    
    float totalDelta = 0.0f;
    float tau = pImpl->timeConstant;
    
    // Compute STDP weight changes for all pre-post spike pairs
    for (Timestamp preTime : preSpikes) {
        for (Timestamp postTime : postSpikes) {
            float delta_t = static_cast<float>(postTime - preTime);  // Δt in ms
            
            if (delta_t > 0) {
                // Pre before post: LTP (long-term potentiation)
                // Time window: ~20ms (NMDA receptor kinetics)
                float delta = pImpl->ltpWeight * std::exp(-delta_t / tau);
                totalDelta += delta;
            } else if (delta_t < 0) {
                // Post before pre: LTD (long-term depression)
                // Time window: ~20ms (backpropagation)
                float delta = -pImpl->ltdWeight * std::exp(delta_t / tau);  // delta_t negative
                totalDelta += delta;
            }
        }
    }
    
    // Apply weight change with biological constraints
    if (std::abs(totalDelta) > 1e-6f) {
        // Activity-dependent scaling with synaptic efficacy
        float efficacy = std::max(0.0f, std::min(1.0f, synapse->getEfficacy()));
        float scaledDelta = totalDelta * efficacy;
        
        // Apply weight change with realistic bounds
        float currentWeight = synapse->getWeight();
        float newWeight = applyWeightChange(synapse, scaledDelta);
        
        // Update eligibility trace for reward-modulated learning
        // Eligibility trace represents synaptic modification potential
        float currentTrace = synapse->getEligibilityTrace();
        
        // Trace decay over time, then add new eligibility
        float traceDecay = 0.95f;  // 5% decay per timestep
        float updatedTrace = currentTrace * traceDecay + scaledDelta;
        synapse->setEligibilityTrace(updatedTrace);
        
        // Ensure eligibility trace is within reasonable bounds
        synapse->setEligibilityTrace(std::clamp(updatedTrace, -1.0f, 1.0f));
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
