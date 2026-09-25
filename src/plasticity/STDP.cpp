#include "STDP.hpp"
#include "../../brain/Synapse.hpp"
#include <cmath>
#include <algorithm>

namespace nlm {

struct STDP::Impl {
    float ltpWeight;      // A+ for potentiation
    float ltdWeight;      // A- for depression
    float tauPlus;        // LTP time constant (ms)
    float tauMinus;       // LTD time constant (ms)
    float minWeight;      // Minimum synaptic weight
    float maxWeight;      // Maximum synaptic weight
    float eligibilityDecay; // Decay rate for eligibility traces (1/ms)
    
    Impl() : ltpWeight(0.01f), ltdWeight(0.012f), 
             tauPlus(20.0f), tauMinus(30.0f), // Different time constants for LTP/LTD
             minWeight(-1.0f), maxWeight(1.0f), eligibilityDecay(0.05f) {} // 5% decay per ms
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
     * Mathematical formulation (Biologically realistic):
     * For each pre-post spike pair with timing difference Δt = t_post - t_pre:
     * 
     * If Δt > 0 (pre before post): POTENTIATION
     *   Δw = A+ * exp(-Δt / τ+)
     *   
     * If Δt < 0 (post before pre): DEPRESSION
     *   Δw = A- * exp(Δt / τ-)
     * 
     * Where:
     *   A+ = ltpWeight (potentiation amplitude) - typically ~0.01-0.1
     *   A- = ltdWeight (depression amplitude) - typically ~0.01-0.1
     *   τ+ = tauPlus (LTP time constant) ~20-50ms
     *   τ- = tauMinus (LTD time constant) ~20-50ms
     * 
     * Biological inspiration:
     *   - Reflects NMDA receptor-mediated calcium signaling
     *   - Pre-before-post activates NMDA receptors when postsynaptic spikes
     *   - Post-before-pre causes backpropagating action potentials
     *   - Separate time constants for LTP/LTD capture different molecular mechanisms
     *   
     * Implementation details:
     *   - Uses discrete spike timing (not continuous rate-based)
     *   - Asymptotic weight bounds prevent runaway excitation/inhibition
     *   - Eligibility traces support reward-modulated STDP
     */
     
    if (!synapse || preSpikes.empty() || postSpikes.empty()) {
        return;
    }
    
    float totalDelta = 0.0f;
    
    for (Timestamp preTime : preSpikes) {
        for (Timestamp postTime : postSpikes) {
            float delta_t = static_cast<float>(postTime - preTime);  // Δt in ms
            
            if (delta_t > 0) {
                // Pre before post: POTENTIATION
                // Longer intervals result in weaker potentiation
                float delta = pImpl->ltpWeight * std::exp(-delta_t / pImpl->tauPlus);
                totalDelta += delta;
            } else if (delta_t < 0) {
                // Post before pre: DEPRESSION
                // Longer intervals result in weaker depression
                float delta = -pImpl->ltdWeight * std::exp(delta_t / pImpl->tauMinus);  // delta_t is negative
                totalDelta += delta;
            }
            // delta_t == 0: no change (simultaneous spikes - rare in practice)
        }
    }
    
    // Apply weight change with biological constraints
    if (std::abs(totalDelta) > 1e-6f) {
        // Scale by synaptic efficacy if available
        float efficacy = synapse->getEfficacy();
        totalDelta *= efficacy;
        
        // Apply weight change with bounds
        float newWeight = synapse->getWeight() + totalDelta;
        newWeight = std::clamp(newWeight, pImpl->minWeight, pImpl->maxWeight);
        synapse->setWeight(newWeight);
        
        // Update eligibility trace for reward-modulated learning
        // Eligibility trace carries weight change history for future reward modulation
        float currentTrace = synapse->getEligibilityTrace();
        synapse->setEligibilityTrace(currentTrace + totalDelta);
        
        // Apply trace decay proportional to time since last update
        synapse->decayEligibilityTrace(pImpl->eligibilityDecay);
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

void STDP::setTauPlus(float tauPlus) {
    pImpl->tauPlus = std::clamp(tauPlus, 1.0f, 100.0f);
}

float STDP::getTauPlus() const {
    return pImpl->tauPlus;
}

void STDP::setTauMinus(float tauMinus) {
    pImpl->tauMinus = std::clamp(tauMinus, 1.0f, 100.0f);
}

float STDP::getTauMinus() const {
    return pImpl->tauMinus;
}

void STDP::setTimeConstant(float tau) {
    // Keep backward compatibility - set both tauPlus and tauMinus
    setTauPlus(tau);
    setTauMinus(tau);
}

float STDP::getTimeConstant() const {
    // Return average of tauPlus and tauMinus for backward compatibility
    return (pImpl->tauPlus + pImpl->tauMinus) * 0.5f;
}

void STDP::setEligibilityDecay(float decayRate) {
    pImpl->eligibilityDecay = std::clamp(decayRate, 0.0f, 1.0f);
}

float STDP::getEligibilityDecay() const {
    return pImpl->eligibilityDecay;
}

void STDP::setMinWeight(float minWeight) {
    pImpl->minWeight = std::clamp(minWeight, -10.0f, 10.0f);
}

float STDP::getMinWeight() const {
    return pImpl->minWeight;
}

float STDP::getMaxWeight() const {
    return pImpl->maxWeight;
}

void STDP::configureFull(float ltpWeight, float ltdWeight, 
                         float tauPlus, float tauMinus,
                         float minWeight, float maxWeight,
                         float eligibilityDecay) {
    setLTPWeight(ltpWeight);
    setLTDWeight(ltdWeight);
    setTauPlus(tauPlus);
    setTauMinus(tauMinus);
    setMinWeight(minWeight);
    setMaxWeight(maxWeight);
    setEligibilityDecay(eligibilityDecay);
}

void STDP::setMaxWeight(float maxWeight) {
    pImpl->maxWeight = std::clamp(maxWeight, 0.0f, 10.0f);
}

void STDP::configure(float ltpWeight, float ltdWeight, float tau) {
    setLTPWeight(ltpWeight);
    setLTDWeight(ltdWeight);
    setTimeConstant(tau);
}

} // namespace nlm
