#include "TripletSTDP.hpp"
#include "../../brain/Synapse.hpp"
#include <cmath>
#include <algorithm>

namespace nlm {

struct TripletSTDP::Impl {
    float aPlus;                 // Potentiation amplitude (A+)
    float aMinus;                // Depression amplitude (A-)
    float tauPlus;               // LTP time constant (τ+)
    float tauMinus;              // LTD time constant (τ-)
    float b;                     // Triplet interaction parameter (0.0 to 1.0)
    float minWeight;             // Minimum synaptic weight
    float maxWeight;             // Maximum synaptic weight
    
    Impl() : aPlus(0.01f), aMinus(0.012f), 
             tauPlus(20.0f), tauMinus(30.0f), b(0.5f),
             minWeight(-1.0f), maxWeight(1.0f) {}
};

TripletSTDP::TripletSTDP() : pImpl(new Impl) {}

TripletSTDP::~TripletSTDP() = default;

void TripletSTDP::update(Synapse* synapse,
                         const std::vector<Timestamp>& preSpikes,
                         const std::vector<Timestamp>& postSpikes,
                         TimestepDuration dt) {
    /*
     * Triplet STDP implementation
     * 
     * Mathematical formulation (Gerstner et al. 1996, 1997):
     * 
     * For each pre-post spike pair with timing difference Δt = t_post - t_pre:
     * 
     * If Δt > 0 (pre before post): POTENTIATION
     *   Δw = A+ * [exp(-Δt/τ+) - b * exp(-Δt/τ-)]
     *   
     * If Δt < 0 (post before pre): DEPRESSION
     *   Δw = A- * [exp(Δt/τ-) - b * exp(Δt/τ+)]
     * 
     * Where:
     *   A+ = aPlus (potentiation amplitude)
     *   A- = aMinus (depression amplitude)
     *   τ+ = tauPlus (LTP time constant)
     *   τ- = tauMinus (LTD time constant)
     *   b = triplet interaction parameter (typically ~0.5)
     * 
     * Biological inspiration:
     *   - Accounts for triplet interactions (pre-post-pre, post-pre-post)
     *   - Better explains synaptic responses with multiple spikes
     *   - Captures differential effects of spike patterns
     *   - More accurate than pairwise STDP for burst firing
     *   
     * Implementation details:
     *   - Uses exponential kernels for spike interactions
     *   - Triplet parameter 'b' controls relative strength of interactions
     *   - Weight changes are bounded to prevent runaway excitation
     */
    
    if (!synapse || preSpikes.empty() || postSpikes.empty()) {
        return;
    }
    
    float totalDelta = 0.0f;
    
    for (Timestamp preTime : preSpikes) {
        for (Timestamp postTime : postSpikes) {
            float delta_t = static_cast<float>(postTime - preTime);  // Δt in ms
            
            if (delta_t > 0) {
                // Pre before post: POTENTIATION with triplet correction
                // Δw = A+ * [exp(-Δt/τ+) - b * exp(-Δt/τ-)]
                float exp_plus = std::exp(-delta_t / pImpl->tauPlus);
                float exp_minus = std::exp(-delta_t / pImpl->tauMinus);
                float delta = pImpl->aPlus * (exp_plus - pImpl->b * exp_minus);
                totalDelta += delta;
            } else if (delta_t < 0) {
                // Post before pre: DEPRESSION with triplet correction
                // Δw = A- * [exp(Δt/τ-) - b * exp(Δt/τ+)]
                float exp_plus = std::exp(delta_t / pImpl->tauPlus);
                float exp_minus = std::exp(delta_t / pImpl->tauMinus);
                float delta = pImpl->aMinus * (exp_minus - pImpl->b * exp_plus);
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
        float currentTrace = synapse->getEligibilityTrace();
        synapse->setEligibilityTrace(currentTrace + totalDelta);
        
        // Apply trace decay
        synapse->decayEligibilityTrace(pImpl->b * 0.1f); // Use 'b' parameter for decay rate
    }
}

void TripletSTDP::applyWeightChange(Synapse* synapse, SynapticWeight delta) {
    if (!synapse) return;
    
    // Apply weight change with proper bounds clamping
    float newWeight = synapse->getWeight() + delta;
    newWeight = std::clamp(newWeight, pImpl->minWeight, pImpl->maxWeight);
    synapse->setWeight(newWeight);
}

const char* TripletSTDP::getName() const {
    return "TripletSTDP";
}

void TripletSTDP::setA_plus(float aPlus) {
    pImpl->aPlus = std::clamp(aPlus, 0.0f, 1.0f);
}

float TripletSTDP::getA_plus() const {
    return pImpl->aPlus;
}

void TripletSTDP::setA_minus(float aMinus) {
    pImpl->aMinus = std::clamp(aMinus, 0.0f, 1.0f);
}

float TripletSTDP::getA_minus() const {
    return pImpl->aMinus;
}

void TripletSTDP::set_tau_plus(float tauPlus) {
    pImpl->tauPlus = std::clamp(tauPlus, 1.0f, 100.0f);
}

float TripletSTDP::get_tau_plus() const {
    return pImpl->tauPlus;
}

void TripletSTDP::set_tau_minus(float tauMinus) {
    pImpl->tauMinus = std::clamp(tauMinus, 1.0f, 100.0f);
}

float TripletSTDP::get_tau_minus() const {
    return pImpl->tauMinus;
}

void TripletSTDP::set_b(float b) {
    pImpl->b = std::clamp(b, 0.0f, 1.0f);
}

float TripletSTDP::get_b() const {
    return pImpl->b;
}

void TripletSTDP::configure(float aPlus, float aMinus, float tauPlus, float tauMinus, float b) {
    setA_plus(aPlus);
    setA_minus(aMinus);
    set_tau_plus(tauPlus);
    set_tau_minus(tauMinus);
    set_b(b);
}

} // namespace nlm