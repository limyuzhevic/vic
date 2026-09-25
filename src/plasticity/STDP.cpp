#include "STDP.hpp"
#include "../../brain/Synapse.hpp"
#include "core/Constants.hpp"
#include "core/Utils.hpp"
#include <cmath>
#include <algorithm>

namespace nlm {

struct STDP::Impl {
    float ltp_weight;      // A+ for potentiation
    float ltd_weight;      // A- for depression
    float time_constant;   // Tau for exponential window (ms)
    float min_weight;      // Minimum synaptic weight
    float max_weight;      // Maximum synaptic weight
    
    Impl() : ltp_weight(Constants::STDP_LTP_WEIGHT), 
             ltd_weight(Constants::STDP_LTD_WEIGHT), 
             time_constant(Constants::STDP_TIME_WINDOW_MS),
             min_weight(Constants::MIN_WEIGHT), 
             max_weight(Constants::MAX_WEIGHT) {}
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
     *   A+ = ltp_weight (potentiation amplitude)
     *   A- = ltd_weight (depression amplitude)  
     *   τ+ = τ- = time_constant (STDP time window)
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
    
    // Validate inputs using common utility
    if (!Utils::validateUpdateInputs(synapse, preSpikes, postSpikes, dt)) {
        return;
    }
    
    float total_delta = 0.0f;
    float tau = pImpl->time_constant;
    
    for (Timestamp pre_time : preSpikes) {
        for (Timestamp post_time : postSpikes) {
            float dt_val = static_cast<float>(post_time - pre_time);  // Δt in ms
            
            if (dt_val > 0) {
                // Pre before post: POTENTIATION
                // "Cells that fire together, wire together" - but only if pre fires before post
                float delta = pImpl->ltp_weight * std::exp(-dt_val / tau);
                total_delta += delta;
            } else if (dt_val < 0) {
                // Post before pre: DEPRESSION
                // "Anti-Hebbian" - connection weakens if post fires without pre
                float delta = -pImpl->ltd_weight * std::exp(dt_val / tau);  // dt is negative, so this subtracts
                total_delta += delta;
            }
            // dt == 0: no change (simultaneous spikes - rare in practice)
        }
    }
    
    // Apply weight change with bounds
    if (std::abs(total_delta) > Constants::EPSILON_FOR_NORMALS) {
        // Scale by synaptic efficacy if available
        float efficacy = synapse->getEfficacy();
        total_delta *= efficacy;
        
        // Apply weight change using common utility
        synapse->addToWeight(total_delta);
        
        // Update eligibility trace for reward-modulated learning
        float current_trace = synapse->getEligibilityTrace();
        synapse->setEligibilityTrace(Utils::clampWeight(current_trace + total_delta));
    }
}

void STDP::applyWeightChange(Synapse* synapse, SynapticWeight delta) {
    if (!synapse) return;
    
    // Clamp weight to bounds using common utility
    float new_weight = synapse->getWeight() + delta;
    new_weight = Utils::clampWeight(new_weight);
    synapse->setWeight(new_weight);
}

const char* STDP::getName() const {
    return "STDP";
}

void STDP::setLTPWeight(float weight) {
    pImpl->ltp_weight = Utils::clampLearningRate(weight);
}

float STDP::getLTPWeight() const {
    return pImpl->ltp_weight;
}

void STDP::setLTDWeight(float weight) {
    pImpl->ltd_weight = Utils::clampLearningRate(weight);
}

float STDP::getLTDWeight() const {
    return pImpl->ltd_weight;
}

void STDP::setTimeConstant(float tau) {
    pImpl->time_constant = std::clamp(tau, 1.0f, 100.0f);
}

float STDP::getTimeConstant() const {
    return pImpl->time_constant;
}

void STDP::configure(float ltpWeight, float ltdWeight, float tau) {
    setLTPWeight(ltpWeight);
    setLTDWeight(ltdWeight);
    setTimeConstant(tau);
}

} // namespace nlm
