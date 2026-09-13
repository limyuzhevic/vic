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
    
    // Advanced STDP features
    float aPlus;          // Pairing rule parameter A+
    float aMinus;         // Pairing rule parameter A-
    bool tripletLearning; // Whether triplet learning is enabled
    
    Impl() : ltpWeight(0.01f), ltdWeight(0.012f), timeConstant(20.0f),
             minWeight(-1.0f), maxWeight(1.0f),
             aPlus(0.01f), aMinus(0.012f), tripletLearning(false) {}
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
     *   A+ = aPlus (potentiation amplitude)
     *   A- = aMinus (depression amplitude)  
     *   τ+ = τ- = timeConstant (STDP time window)
     * 
     * Biological inspiration:
     *   - Reflects NMDA receptor-mediated calcium signaling
     *   - Pre-before-post activates NMDA receptors when postsynaptic spikes
     *   - Post-before-pre causes backpropagating action potentials
     *   
     * Advanced features:
     *   - Pairing rule implementation for weight-dependent STDP
     *   - Optional triplet learning for spontaneous activity
     */
    
    if (!synapse || preSpikes.empty() || postSpikes.empty()) {
        return;
    }
    
    float totalDelta = 0.0f;
    float tau = pImpl->timeConstant;
    
    // Calculate current weight for pairing rule
    float currentWeight = synapse->getWeight();
    
    for (Timestamp preTime : preSpikes) {
        for (Timestamp postTime : postSpikes) {
            float dt = static_cast<float>(postTime - preTime);  // Δt in ms
            
            if (dt > 0) {
                // Pre before post: POTENTIATION
                // "Cells that fire together, wire together" - but only if pre fires before post
                
                // Basic STDP with exponential decay
                float delta = pImpl->ltpWeight * std::exp(-dt / tau);
                
                // Apply pairing rule (weight-dependent modulation)
                if (pImpl->tripletLearning) {
                    // Pair of pre, post (and possibly neighboring spikes)
                    float pairingDelta = pImpl->aPlus * std::exp(-dt / tau) * 
                                       (1.0f + 0.5f * currentWeight);  // Weight-dependent
                    delta += pairingDelta * 0.1f;  // Weight-dependent contribution
                }
                
                totalDelta += delta;
            } else if (dt < 0) {
                // Post before pre: DEPRESSION
                // "Anti-Hebbian" - connection weakens if post fires without pre
                
                // Basic STDP with exponential decay
                float delta = -pImpl->ltdWeight * std::exp(dt / tau);  // dt is negative, so this subtracts
                
                // Apply pairing rule (weight-dependent modulation)
                if (pImpl->tripletLearning) {
                    // Pair of post, pre (and possibly neighboring spikes)
                    float pairingDelta = -pImpl->aMinus * std::exp(dt / tau) * 
                                        (1.0f - 0.5f * currentWeight);  // Weight-dependent
                    delta += pairingDelta * 0.1f;  // Weight-dependent contribution
                }
                
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
    // Also update pairing rule parameter
    pImpl->aPlus = pImpl->ltpWeight;
}

float STDP::getLTPWeight() const {
    return pImpl->ltpWeight;
}

void STDP::setLTDWeight(float weight) {
    pImpl->ltdWeight = std::clamp(weight, 0.0f, 1.0f);
    // Also update pairing rule parameter
    pImpl->aMinus = pImpl->ltdWeight;
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

void STDP::setPairingRule(float a_plus, float a_minus, float w_max, float w_min) {
    pImpl->aPlus = std::clamp(a_plus, 0.0f, 0.1f);
    pImpl->aMinus = std::clamp(a_minus, 0.0f, 0.1f);
    pImpl->maxWeight = std::clamp(w_max, 0.0f, 2.0f);
    pImpl->minWeight = std::clamp(w_min, -2.0f, 0.0f);
}

void STDP::enableTripletLearning(bool enable) {
    pImpl->tripletLearning = enable;
}

} // namespace nlm
