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
     * Real STDP implementation based on synaptic biology
     * 
     * This implements the classic STDP learning rule derived from 
     * experimental observations in biological synapses.
     * 
     * Mathematical formulation (derived from empirical data):
     * 
     * For pre-before-post spike pairs (Δt = t_post - t_pre > 0):
     *   Δw = A+ * f(Δt) where f(Δt) = exp(-Δt / τ+)
     *   - This reflects LTP (long-term potentiation)
     *   - Mediated by Ca²⁺ influx through NMDA receptors
     *   - Typically A+ = 0.01-0.05, τ+ = 15-35ms
     *   
     * For post-before-pre spike pairs (Δt < 0):
     *   Δw = -A- * g(-Δt) where g(-Δt) = exp(-|Δt| / τ-)
     *   - This reflects LTD (long-term depression)
     *   - Mediated by backpropagating action potentials and Ca²⁺ spikes
     *   - Typically A- = 0.012-0.025, τ- = 30-40ms
     *   
     * Key biological features:
     *   - Asymmetric learning windows
     *   - Spike timing dependence (not just coincidence)
     *   - Activity-dependent modulation (dopamine, etc.)
     *   - Limited range of effectiveness
     */
    
    if (!synapse || preSpikes.empty() || postSpikes.empty()) {
        return;
    }
    
    float totalDelta = 0.0f;
    float tau = pImpl->timeConstant;
    
    // Process all spike pairs
    for (Timestamp preTime : preSpikes) {
        for (Timestamp postTime : postSpikes) {
            float dt = static_cast<float>(postTime - preTime);  // Δt in ms
            
            if (dt > 0) {
                // Pre before post: POTENTIATION
                // NMDA receptor activation: more effective when pre fires before post
                // Time window: typically up to 50-100ms
                if (dt <= 100.0f) {  // Limit to biologically realistic window
                    float delta = pImpl->ltpWeight * std::exp(-dt / tau);
                    totalDelta += delta;
                }
            } else if (dt < 0) {
                // Post before pre: DEPRESSION
                // Backpropagating action potentials cause LTD
                // Time window: typically up to 50-100ms
                float negDt = -dt;
                if (negDt <= 100.0f) {  // Limit to biologically realistic window
                    float delta = -pImpl->ltdWeight * std::exp(negDt / tau);
                    totalDelta += delta;
                }
            }
            // dt == 0: no change (simultaneous spikes - minimal effect)
        }
    }
    
    // Apply weight change with biological constraints
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
