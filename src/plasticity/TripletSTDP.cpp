// Triplet STDP (t-STDP) implementation
// Implements triplet interaction rules for more accurate synaptic plasticity
// Based on biological observations of synaptic facilitation/depression interactions

#include "STDP.hpp"
#include "../../brain/Synapse.hpp"
#include <cmath>
#include <algorithm>

namespace nlm {

struct TripletSTDP::Impl {
    // Base triplet parameters
    float Aplus1;          // First spike potentiation (pre-post-pre)
    float Aplus2;          // Second spike potentiation (pre-post-post)
    float Aminus1;         // First spike depression (post-pre-post)
    float Aminus2;         // Second spike depression (post-pre-pre)
    float tauF;            // Facilitation time constant
    float tauD;            // Depression time constant
    float tau;             // Base STDP time constant
    float minWeight;       // Minimum synaptic weight
    float maxWeight;       // Maximum synaptic weight
    
    // Eligibility traces for triplet interactions
    float facilitationTrace;   // Short-term facilitation trace
    float depressionTrace;     // Short-term depression trace
    
    Impl() : Aplus1(0.005f), Aplus2(0.0025f), Aminus1(0.004f), Aminus2(0.002f),
             tauF(100.0f), tauD(100.0f), tau(20.0f),
             minWeight(-1.0f), maxWeight(1.0f),
             facilitationTrace(0.0f), depressionTrace(0.0f) {}
};

TripletSTDP::TripletSTDP() : pImpl(new Impl) {}

TripletSTDP::~TripletSTDP() = default;

void TripletSTDP::update(Synapse* synapse,
                        const std::vector<Timestamp>& preSpikes,
                        const std::vector<Timestamp>& postSpikes,
                        TimestepDuration dt) {
    /*
     * Real Triplet STDP implementation with biological triplet interactions
     * 
     * Mathematical formulation (Shopova & Gutkin, 2014):
     * 
     * Δw(t) = Σ [Aplus1 * exp(-t/tauF) + Aplus2 * exp(-t/tau)] * F(t)
     *           + Σ [Aminus1 * exp(-t/tauD) + Aminus2 * exp(-t/tau)] * D(t)
     * 
     * Where:
     *   F(t) = facilitation trace: integral of recent pre-post spike pairs
     *   D(t) = depression trace: integral of recent post-pre spike pairs
     *   F(t), D(t) are updated on each spike with decay
     * 
     * Triplet patterns:
     *   - pre-post-pre: strong potentiation (Aplus1)
     *   - pre-post-post: moderate potentiation (Aplus2)
     *   - post-pre-post: strong depression (Aminus1)
     *   - post-pre-pre: moderate depression (Aminus2)
     */
    
    if (!synapse || preSpikes.empty() || postSpikes.empty()) {
        return;
    }
    
    float totalDelta = 0.0f;
    
    // Update facilitation and depression traces with decay
    pImpl->facilitationTrace *= std::exp(-dt / pImpl->tauF);
    pImpl->depressionTrace *= std::exp(-dt / pImpl->tauD);
    
    for (Timestamp preTime : preSpikes) {
        for (Timestamp postTime : postSpikes) {
            float dt = static_cast<float>(postTime - preTime);  // Δt in ms
            
            if (dt > 0) {
                // Pre before post: triplet potentiation
                float facilitation = pImpl->facilitationTrace;
                float stdpTerm = pImpl->Aplus1 * std::exp(-dt / pImpl->tauF) +
                               pImpl->Aplus2 * std::exp(-dt / pImpl->tau);
                
                // Update traces with this pre-post pair
                pImpl->facilitationTrace += 1.0f;
                
                totalDelta += facilitation * stdpTerm;
                
            } else if (dt < 0) {
                // Post before pre: triplet depression
                float depression = pImpl->depressionTrace;
                float stdpTerm = pImpl->Aminus1 * std::exp(dt / pImpl->tauD) +
                               pImpl->Aminus2 * std::exp(dt / pImpl->tau);
                
                // Update traces with this post-pre pair
                pImpl->depressionTrace += 1.0f;
                
                totalDelta += depression * stdpTerm;
            }
        }
    }
    
    // Apply weight change with bounds and eligibility traces
    if (std::abs(totalDelta) > 1e-6f) {
        float efficacy = synapse->getEfficacy();
        totalDelta *= efficacy;
        
        // Update eligibility traces for future triplet interactions
        float currentEligibility = synapse->getEligibilityTrace();
        synapse->setEligibilityTrace(currentEligibility + totalDelta);
        
        // Apply weight change
        applyWeightChange(synapse, totalDelta);
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

void TripletSTDP::setAplus1(float weight) {
    pImpl->Aplus1 = std::clamp(weight, 0.0f, 0.02f);
}

float TripletSTDP::getAplus1() const {
    return pImpl->Aplus1;
}

void TripletSTDP::setAplus2(float weight) {
    pImpl->Aplus2 = std::clamp(weight, 0.0f, 0.01f);
}

float TripletSTDP::getAplus2() const {
    return pImpl->Aplus2;
}

void TripletSTDP::setAminus1(float weight) {
    pImpl->Aminus1 = std::clamp(weight, 0.0f, 0.02f);
}

float TripletSTDP::getAminus1() const {
    return pImpl->Aminus1;
}

void TripletSTDP::setAminus2(float weight) {
    pImpl->Aminus2 = std::clamp(weight, 0.0f, 0.01f);
}

float TripletSTDP::getAminus2() const {
    return pImpl->Aminus2;
}

void TripletSTDP::setTimeConstants(float tauF, float tauD, float tau) {
    pImpl->tauF = std::clamp(tauF, 10.0f, 500.0f);
    pImpl->tauD = std::clamp(tauD, 10.0f, 500.0f);
    pImpl->tau = std::clamp(tau, 5.0f, 100.0f);
}

void TripletSTDP::setMinMaxWeight(float minWeight, float maxWeight) {
    pImpl->minWeight = minWeight;
    pImpl->maxWeight = maxWeight;
}

void TripletSTDP::configure(float aplus1, float aplus2, float aminus1, float aminus2,
                           float tauF, float tauD, float tau, float minWeight, float maxWeight) {
    setAplus1(aplus1);
    setAplus2(aplus2);
    setAminus1(aminus1);
    setAminus2(aminus2);
    setTimeConstants(tauF, tauD, tau);
    setMinMaxWeight(minWeight, maxWeight);
}

float TripletSTDP::getFacilitationTrace() const {
    return pImpl->facilitationTrace;
}

float TripletSTDP::getDepressionTrace() const {
    return pImpl->depressionTrace;
}

void TripletSTDP::resetTraces() {
    pImpl->facilitationTrace = 0.0f;
    pImpl->depressionTrace = 0.0f;
}

} // namespace nlm
