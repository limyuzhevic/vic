#include "TripletSTDP.hpp"
#include "../../brain/Synapse.hpp"
#include <cmath>
#include <algorithm>
#include <deque>

namespace nlm {

struct TripletSTDP::Impl {
    // Single spike STDP parameters (baseline)
    float ltpWeight;           // LTP amplitude (A+)
    float ltdWeight;           // LTD amplitude (A-)
    float timeConstant;        // STDP time constant (tau)
    
    // Triplet rule parameters
    float a1plus;              // Pre-pre-post potentiation
    float a1minus;             // Pre-pre-post depression
    float a2plus;              // Pre-post-post potentiation
    float a2minus;             // Pre-post-post depression
    float tauPlus;             // Potentiation time constant
    float tauMinus;            // Depression time constant
    
    // Weight bounds
    float minWeight;           // Minimum synaptic weight
    float maxWeight;           // Maximum synaptic weight
    
    // History management
    static constexpr size_t MAX_SPIKE_HISTORY = 100;
    
    Impl() : ltpWeight(0.01f), ltdWeight(0.012f), timeConstant(20.0f),
             a1plus(0.005f), a1minus(0.002f), a2plus(0.005f), a2minus(0.002f),
             tauPlus(20.0f), tauMinus(20.0f),
             minWeight(-1.0f), maxWeight(1.0f) {}
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
     * Mathematical formulation:
     * 
     * SINGLE SPIKE STDP (baseline):
     * For each pre-post pair with timing difference Δt = t_post - t_pre:
     *   If Δt > 0: Δw = A+ * exp(-Δt / τ+)
     *   If Δt < 0: Δw = -A- * exp(Δt / τ-)
     * 
     * TRIPLET RULES:
     * 
     * Pre-pre-post potentiation (1+):
     *   Weight increase if pre fires twice close together, then post fires
     *   Δw = A1+ * exp(-Δt_pre1 / τ1) * exp(-Δt_post1 / τ1)
     *   Where Δt_post1 is time since first pre spike
     * 
     * Pre-pre-post depression (1-):
     *   Weight decrease if pre fires twice close together, then another pre fires
     *   Δw = -A1- * exp(-Δt_pre2 / τ1) * exp(-Δt_pre3 / τ1)
     *   Where Δt_pre2 and Δt_pre3 are times between pre spikes
     * 
     * Pre-post-post potentiation (2+):
     *   Weight increase if pre fires, then post fires, then post fires again
     *   Δw = A2+ * exp(-Δt_post2 / τ2) * exp(-Δt_post3 / τ2)
     *   Where Δt_post2 and Δt_post3 are times between post spikes
     * 
     * Pre-post-post depression (2-):
     *   Weight decrease if post fires twice close together, then pre fires
     *   Δw = -A2- * exp(-Δt_pre2 / τ2) * exp(-Δt_post2 / τ2)
     *   Where Δt_pre2 and Δt_post2 are times between spikes
     */
    
    if (!synapse) {
        return;
    }
    
    float totalDelta = 0.0f;
    
    // Extract current spike history from synapse
    const std::vector<Timestamp>& preHistory = synapse->getPreSpikeHistory();
    const std::vector<Timestamp>& postHistory = synapse->getPostSpikeHistory();
    
    // Apply single spike STDP (baseline)
    float tau = pImpl->timeConstant;
    for (Timestamp preTime : preSpikes) {
        for (Timestamp postTime : postSpikes) {
            float dt = static_cast<float>(postTime - preTime);
            if (dt > 0) {
                float delta = pImpl->ltpWeight * std::exp(-dt / tau);
                totalDelta += delta;
            } else if (dt < 0) {
                float delta = -pImpl->ltdWeight * std::exp(dt / tau);
                totalDelta += delta;
            }
        }
    }
    
    // Apply triplet rules
    totalDelta += computeTripletContributions(preHistory, postHistory);
    
    // Apply weight change
    if (std::abs(totalDelta) > 1e-6f) {
        applyWeightChange(synapse, totalDelta);
    }
}

float TripletSTDP::computeTripletContributions(const std::vector<Timestamp>& preHistory,
                                               const std::vector<Timestamp>& postHistory) const {
    float totalTripletDelta = 0.0f;
    
    if (preHistory.size() < 2 || postHistory.size() < 2) {
        return totalTripletDelta;  // Need at least 2 spikes of each type
    }
    
    // Pre-pre-post potentiation (1+)
    if (preHistory.size() >= 2 && postHistory.size() >= 1) {
        Timestamp pre1 = preHistory[preHistory.size() - 2]; // Second-to-last pre
        Timestamp pre2 = preHistory[preHistory.size() - 1];  // Most recent pre
        Timestamp post = postHistory[postHistory.size() - 1]; // Most recent post
        
        float dt_pre1 = static_cast<float>(post - pre1);
        float dt_pre2 = static_cast<float>(pre2 - pre1);
        
        if (dt_pre1 > 0 && dt_pre2 > 0 && dt_pre2 < 100.0f) { // Close pre spikes
            float contribution = pImpl->a1plus * 
                                 std::exp(-dt_pre1 / pImpl->tauPlus) *
                                 std::exp(-dt_pre2 / pImpl->tauPlus);
            totalTripletDelta += contribution;
        }
    }
    
    // Pre-pre-post depression (1-)
    if (preHistory.size() >= 3) {
        Timestamp pre1 = preHistory[preHistory.size() - 3]; // Third-to-last pre
        Timestamp pre2 = preHistory[preHistory.size() - 2]; // Second-to-last pre
        Timestamp pre3 = preHistory[preHistory.size() - 1];  // Most recent pre
        
        float dt_pre2 = static_cast<float>(pre2 - pre1);
        float dt_pre3 = static_cast<float>(pre3 - pre2);
        
        if (dt_pre2 > 0 && dt_pre3 > 0 && dt_pre3 < 100.0f) {
            float contribution = -pImpl->a1minus * 
                                 std::exp(-dt_pre2 / pImpl->tauMinus) *
                                 std::exp(-dt_pre3 / pImpl->tauMinus);
            totalTripletDelta += contribution;
        }
    }
    
    // Pre-post-post potentiation (2+)
    if (preHistory.size() >= 1 && postHistory.size() >= 2) {
        Timestamp pre = preHistory[preHistory.size() - 1];    // Most recent pre
        Timestamp post1 = postHistory[postHistory.size() - 2]; // Second-to-last post
        Timestamp post2 = postHistory[postHistory.size() - 1]; // Most recent post
        
        float dt_post1 = static_cast<float>(post1 - pre);
        float dt_post2 = static_cast<float>(post2 - post1);
        
        if (dt_post1 > 0 && dt_post2 > 0 && dt_post2 < 100.0f) {
            float contribution = pImpl->a2plus * 
                                 std::exp(-dt_post1 / pImpl->tauPlus) *
                                 std::exp(-dt_post2 / pImpl->tauPlus);
            totalTripletDelta += contribution;
        }
    }
    
    // Pre-post-post depression (2-)
    if (postHistory.size() >= 2 && preHistory.size() >= 1) {
        Timestamp post1 = postHistory[postHistory.size() - 2]; // Second-to-last post
        Timestamp post2 = postHistory[postHistory.size() - 1]; // Most recent post
        Timestamp pre = preHistory[preHistory.size() - 1];    // Most recent pre
        
        float dt_post1 = static_cast<float>(pre - post1);
        float dt_post2 = static_cast<float>(post2 - post1);
        
        if (dt_post1 > 0 && dt_post2 > 0 && dt_post2 < 100.0f) {
            float contribution = -pImpl->a2minus * 
                                 std::exp(-dt_post1 / pImpl->tauMinus) *
                                 std::exp(-dt_post2 / pImpl->tauMinus);
            totalTripletDelta += contribution;
        }
    }
    
    return totalTripletDelta;
}

void TripletSTDP::applyWeightChange(Synapse* synapse, SynapticWeight delta) {
    if (!synapse) return;
    
    // Calculate new weight with delta
    float newWeight = synapse->getWeight() + delta;
    
    // Apply weight change with bounds - single clamping point
    newWeight = std::clamp(newWeight, pImpl->minWeight, pImpl->maxWeight);
    synapse->setWeight(newWeight);
}

const char* TripletSTDP::getName() const {
    return "TripletSTDP";
}

void TripletSTDP::setA1Plus(float a1plus) {
    pImpl->a1plus = std::clamp(a1plus, 0.0f, 0.1f);
}

float TripletSTDP::getA1Plus() const {
    return pImpl->a1plus;
}

void TripletSTDP::setA1Minus(float a1minus) {
    pImpl->a1minus = std::clamp(a1minus, 0.0f, 0.1f);
}

float TripletSTDP::getA1Minus() const {
    return pImpl->a1minus;
}

void TripletSTDP::setA2Plus(float a2plus) {
    pImpl->a2plus = std::clamp(a2plus, 0.0f, 0.1f);
}

float TripletSTDP::getA2Plus() const {
    return pImpl->a2plus;
}

void TripletSTDP::setA2Minus(float a2minus) {
    pImpl->a2minus = std::clamp(a2minus, 0.0f, 0.1f);
}

float TripletSTDP::getA2Minus() const {
    return pImpl->a2minus;
}

void TripletSTDP::setTauPlus(float tauPlus) {
    pImpl->tauPlus = std::clamp(tauPlus, 5.0f, 100.0f);
}

float TripletSTDP::getTauPlus() const {
    return pImpl->tauPlus;
}

void TripletSTDP::setTauMinus(float tauMinus) {
    pImpl->tauMinus = std::clamp(tauMinus, 5.0f, 100.0f);
}

float TripletSTDP::getTauMinus() const {
    return pImpl->tauMinus;
}

void TripletSTDP::configure(float a1plus, float a1minus, float a2plus, float a2minus,
                           float tauPlus, float tauMinus) {
    setA1Plus(a1plus);
    setA1Minus(a1minus);
    setA2Plus(a2plus);
    setA2Minus(a2minus);
    setTauPlus(tauPlus);
    setTauMinus(tauMinus);
}

void TripletSTDP::recordPreSpike(Synapse* synapse, Timestamp timestamp) {
    if (synapse) {
        synapse->recordPreSpike(timestamp);
    }
}

void TripletSTDP::recordPostSpike(Synapse* synapse, Timestamp timestamp) {
    if (synapse) {
        synapse->recordPostSpike(timestamp);
    }
}

void TripletSTDP::setMinWeight(float minWeight) {
    pImpl->minWeight = minWeight;
}

void TripletSTDP::setMaxWeight(float maxWeight) {
    pImpl->maxWeight = maxWeight;
}

} // namespace nlm
