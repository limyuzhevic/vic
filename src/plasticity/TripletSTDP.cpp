#include "TripletSTDP.hpp"
#include "../../brain/Synapse.hpp"
#include <cmath>
#include <algorithm>
#include <deque>

namespace nlm {

struct TripletSTDP::Impl {
    float ltpWeight;          // A+ for potentiation (first-order)
    float ltdWeight;          // A- for depression (first-order)
    float timeConstant;       // τ for exponential window (ms)
    float a2Weight;           // A2 for triple potentiation (second-order)
    float b2Weight;           // B2 for triple depression (second-order)
    float minWeight;          // Minimum synaptic weight
    float maxWeight;          // Maximum synaptic weight
    float nmdaTheta;          // NMDA threshold voltage (mV)
    float nmdaMgBlock;        // Magnesium block factor
    
    // Spike history for triplet interactions
    std::deque<Timestamp> preSpikeHistory;
    std::deque<Timestamp> postSpikeHistory;
    
    Impl() : ltpWeight(0.01f), ltdWeight(0.012f), timeConstant(20.0f),
             a2Weight(0.005f), b2Weight(0.006f), minWeight(-1.0f), maxWeight(1.0f),
             nmdaTheta(0.0f), nmdaMgBlock(1.0f) {}
};

TripletSTDP::TripletSTDP() : pImpl(new Impl) {}

TripletSTDP::~TripletSTDP() = default;

void TripletSTDP::update(Synapse* synapse,
                         const std::vector<Timestamp>& preSpikes,
                         const std::vector<Timestamp>& postSpikes,
                         TimestepDuration dt) {
    /*
     * Triplet STDP Implementation
     * 
     * Biological inspiration:
     *   - Captures NMDA receptor saturation and calcium dynamics
     *   - Accounts for spike timing beyond single pairs (triple interactions)
     *   - Implemented in cortical and hippocampal synapses
     *   - Based on experimental data from Debanne et al. (2006)
     *   - Reflects synaptic plasticity in learning with delayed rewards
     *   
     * Mathematical formulation (Gerstner et al., Neural Computation 1996):
     *   
     * For post after pre (potentiation):
     *   Δw = A+ * [exp(-(t_post - t_pre)/τ+) + A2 * exp(-(t_pre2 - t_pre)/τ+)]
     *   
     * For post before pre (depression):
     *   Δw = -A- * [exp((t_pre - t_post)/τ-) + B2 * exp(-(t_post2 - t_post)/τ-)]
     *   
     * Where:
     *   A+ = ltpWeight, A- = ltdWeight (first-order coefficients)
     *   A2 = a2Weight, B2 = b2Weight (second-order coefficients)
     *   τ = timeConstant (time window)
     *   t_pre, t_post = timing of pre- and post-synaptic spikes
     *   t_pre2 = second most recent pre-synaptic spike
     *   t_post2 = second most recent post-synaptic spike
     *   
     * Biological mechanisms:
     *   - NMDA receptor saturation creates non-linear interaction
     *   - Calcium dynamics have delayed and prolonged components
     *   - Triple spike interactions capture temporal clustering effects
     *   - Simulated by maintaining spike history
     *   
     * Features:
     *   - Extends standard STDP with second-order interactions
     *   - Captures spike timing beyond single pairs
     *   - Implements non-linear saturation effects
     *   - Maintains spike history for triplet computations
     */
    
    if (!synapse) {
        return;
    }
    
    // Update spike history (keep last 3 spikes for triplet computations)
    for (Timestamp preTime : preSpikes) {
        pImpl->preSpikeHistory.push_back(preTime);
        // Keep only the most recent 3 pre spikes
        if (pImpl->preSpikeHistory.size() > 3) {
            pImpl->preSpikeHistory.pop_front();
        }
    }
    
    for (Timestamp postTime : postSpikes) {
        pImpl->postSpikeHistory.push_back(postTime);
        // Keep only the most recent 3 post spikes
        if (pImpl->postSpikeHistory.size() > 3) {
            pImpl->postSpikeHistory.pop_front();
        }
    }
    
    float totalDelta = 0.0f;
    float tau = pImpl->timeConstant;
    
    // Compute triplet interactions
    // First-order (single spike pair) interactions
    for (Timestamp preTime : preSpikes) {
        for (Timestamp postTime : postSpikes) {
            float dt = static_cast<float>(postTime - preTime);  // Δt in ms
            
            if (dt > 0) {
                // Post after pre: potentiation
                // First-order contribution
                float firstOrder = pImpl->ltpWeight * std::exp(-dt / tau);
                totalDelta += firstOrder;
                
                // Second-order contribution: check for triple pre -> post interaction
                // Need at least 2 previous pre spikes before the current pre spike
                if (pImpl->preSpikeHistory.size() >= 3) {
                    // Get the second most recent pre spike (pre2)
                    auto it = pImpl->preSpikeHistory.rbegin();
                    std::advance(it, 1);  // Second from end
                    Timestamp pre2Time = *it;
                    
                    // Compute time difference between pre2 and current pre
                    float dtPre2 = static_cast<float>(preTime - pre2Time);
                    
                    if (dtPre2 > 0 && dtPre2 < 200.0f) {  // Within reasonable window
                        float secondOrder = pImpl->a2Weight * std::exp(-dtPre2 / tau);
                        totalDelta += secondOrder;
                    }
                }
            } else if (dt < 0) {
                // Post before pre: depression
                // First-order contribution
                float firstOrder = -pImpl->ltdWeight * std::exp(dt / tau);
                totalDelta += firstOrder;
                
                // Second-order contribution: check for triple post -> pre interaction
                // Need at least 2 previous post spikes before the current post spike
                if (pImpl->postSpikeHistory.size() >= 3) {
                    // Get the second most recent post spike (post2)
                    auto it = pImpl->postSpikeHistory.rbegin();
                    std::advance(it, 1);  // Second from end
                    Timestamp post2Time = *it;
                    
                    // Compute time difference between post2 and current post
                    float dtPost2 = static_cast<float>(postTime - post2Time);
                    
                    if (dtPost2 > 0 && dtPost2 < 200.0f) {  // Within reasonable window
                        float secondOrder = -pImpl->b2Weight * std::exp(-dtPost2 / tau);
                        totalDelta += secondOrder;
                    }
                }
            }
            // dt == 0: simultaneous spikes - minimal effect
        }
    }
    
    // Apply weight change with bounds and non-linear saturation
    if (std::abs(totalDelta) > 1e-6f) {
        // Apply non-linear saturation based on current weight
        float currentWeight = synapse->getWeight();
        float saturationFactor = 1.0f / (1.0f + std::abs(currentWeight) * 2.0f);
        float saturatedDelta = totalDelta * saturationFactor;
        
        // Scale by synaptic efficacy if available
        float efficacy = synapse->getEfficacy();
        saturatedDelta *= efficacy;
        
        // Apply weight change
        applyWeightChange(synapse, saturatedDelta);
    }
}

void TripletSTDP::applyWeightChange(Synapse* synapse, SynapticWeight delta) {
    if (!synapse) return;
    
    // Clamp weight to bounds
    float newWeight = synapse->getWeight() + delta;
    newWeight = std::clamp(newWeight, pImpl->minWeight, pImpl->maxWeight);
    synapse->setWeight(newWeight);
}

const char* TripletSTDP::getName() const {
    return "TripletSTDP";
}

void TripletSTDP::setLTPWeight(float weight) {
    pImpl->ltpWeight = std::clamp(weight, 0.0f, 1.0f);
}

float TripletSTDP::getLTPWeight() const {
    return pImpl->ltpWeight;
}

void TripletSTDP::setLTDWeight(float weight) {
    pImpl->ltdWeight = std::clamp(weight, 0.0f, 1.0f);
}

float TripletSTDP::getLTDWeight() const {
    return pImpl->ltdWeight;
}

void TripletSTDP::setTimeConstant(float tau) {
    pImpl->timeConstant = std::clamp(tau, 5.0f, 100.0f);
}

float TripletSTDP::getTimeConstant() const {
    return pImpl->timeConstant;
}

void TripletSTDP::setA2Weight(float weight) {
    pImpl->a2Weight = std::clamp(weight, 0.0f, 0.1f);
}

float TripletSTDP::getA2Weight() const {
    return pImpl->a2Weight;
}

void TripletSTDP::setB2Weight(float weight) {
    pImpl->b2Weight = std::clamp(weight, 0.0f, 0.1f);
}

float TripletSTDP::getB2Weight() const {
    return pImpl->b2Weight;
}

void TripletSTDP::setNMDATheta(float theta) {
    pImpl->nmdaTheta = std::clamp(theta, -70.0f, 50.0f);
}

float TripletSTDP::getNMDATheta() const {
    return pImpl->nmdaTheta;
}

void TripletSTDP::configure(float ltpWeight, float ltdWeight, float tau, float a2Weight, float b2Weight) {
    setLTPWeight(ltpWeight);
    setLTDWeight(ltdWeight);
    setTimeConstant(tau);
    setA2Weight(a2Weight);
    setB2Weight(b2Weight);
}

} // namespace nlm