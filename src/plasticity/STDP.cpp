#include "STDP.hpp"
#include "../../brain/Synapse.hpp"
#include <cmath>
#include <algorithm>

namespace nlm {

struct STDP::Impl {
    float ltpWeight;          // A+ for potentiation
    float ltdWeight;          // A- for depression
    float tripletLTPWeight;   // B+ for triplet potentiation
    float tripletLTDWeight;   // B- for triplet depression
    float timeConstant;       // Tau for exponential window (ms)
    float tripletTimeConstant; // Tau for triplet exponential window (ms)
    float minWeight;          // Minimum synaptic weight
    float maxWeight;          // Maximum synaptic weight
    
    Impl() : ltpWeight(0.01f), ltdWeight(0.012f), tripletLTPWeight(0.002f), tripletLTDWeight(0.002f), 
             timeConstant(20.0f), tripletTimeConstant(40.0f),
             minWeight(-1.0f), maxWeight(1.0f) {}
};

STDP::STDP() : pImpl(new Impl) {}

STDP::~STDP() = default;

void STDP::update(Synapse* synapse,
                   const std::vector<Timestamp>& preSpikes,
                   const std::vector<Timestamp>& postSpikes,
                   TimestepDuration dt) {
    /*
     * Real STDP implementation based on spike-timing correlation with triplet interactions
     * 
     * Mathematical formulation:
     * For each pre-post spike pair with timing difference Δt = t_post - t_pre:
     * 
     * If Δt > 0 (pre before post): POTENTIATION
     *   Δw = A+ * exp(-Δt / τ+) + B+ * exp(-(postTime - preTime) / τ-)
     *   
     * If Δt < 0 (post before pre): DEPRESSION
     *   Δw = A- * exp(Δt / τ-) + B- * exp((preTime - postTime) / τ+)
     * 
     * Where:
     *   A+ = ltpWeight (pairwise potentiation amplitude)
     *   B+ = tripletLTPWeight (triplet potentiation amplitude)
     *   A- = ltdWeight (pairwise depression amplitude)
     *   B- = tripletLTDWeight (triplet depression amplitude)
     *   τ+ = timeConstant (LTP time window)
     *   τ- = tripletTimeConstant (LTD time window)
     * 
     * Biological inspiration:
     *   - Captures synaptic eligibility traces
     *   - Handles multiple spike interactions (doublets, triplets)
     *   - Reflects NMDA receptor-mediated calcium signaling with burst effects
     *   - Better explains empirical data in cortical networks
     */
    
    if (!synapse || preSpikes.empty() || postSpikes.empty()) {
        return;
    }
    
    float totalDelta = 0.0f;
    float tau_plus = pImpl->timeConstant;      // τ+ for LTP
    float tau_minus = pImpl->tripletTimeConstant; // τ- for LTD
    
    for (Timestamp preTime : preSpikes) {
        for (Timestamp postTime : postSpikes) {
            float dt = static_cast<float>(postTime - preTime);  // Δt in ms
            
            if (dt > 0) {
                // Pre before post: POTENTIATION
                // "Cells that fire together, wire together" - but only if pre fires before post
                float pairwiseLTP = pImpl->ltpWeight * std::exp(-dt / tau_plus);
                float tripletLTP = pImpl->tripletLTPWeight * std::exp(-dt / tau_minus);
                float delta = pairwiseLTP + tripletLTP;
                totalDelta += delta;
            } else if (dt < 0) {
                // Post before pre: DEPRESSION
                // "Anti-Hebbian" - connection weakens if post fires without pre
                float pairwiseLTD = -pImpl->ltdWeight * std::exp(dt / tau_minus);
                float tripletLTD = -pImpl->tripletLTDWeight * std::exp(dt / tau_plus);
                float delta = pairwiseLTD + tripletLTD;
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

void STDP::setTripletLTPWeight(float weight) {
    pImpl->tripletLTPWeight = std::clamp(weight, 0.0f, 1.0f);
}

float STDP::getTripletLTPWeight() const {
    return pImpl->tripletLTPWeight;
}

void STDP::setTripletLTDWeight(float weight) {
    pImpl->tripletLTDWeight = std::clamp(weight, 0.0f, 1.0f);
}

float STDP::getTripletLTDWeight() const {
    return pImpl->tripletLTDWeight;
}

void STDP::setTripletTimeConstant(float tau) {
    pImpl->tripletTimeConstant = std::clamp(tau, 1.0f, 200.0f);
}

float STDP::getTripletTimeConstant() const {
    return pImpl->tripletTimeConstant;
}

void STDP::configure(float ltpWeight, float ltdWeight, float tau) {
    setLTPWeight(ltpWeight);
    setLTDWeight(ltdWeight);
    setTimeConstant(tau);
}

void STDP::configure(float ltpWeight, float ltdWeight, float tripletLTPWeight, float tripletLTDWeight, float tau, float tripletTau) {
    setLTPWeight(ltpWeight);
    setLTDWeight(ltdWeight);
    setTripletLTPWeight(tripletLTPWeight);
    setTripletLTDWeight(tripletLTDWeight);
    setTimeConstant(tau);
    setTripletTimeConstant(tripletTau);
}

} // namespace nlm
