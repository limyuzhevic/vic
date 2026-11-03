#include "STDP.hpp"
#include "../brain/Synapse.hpp"
#include <cmath>
#include <algorithm>
#include <numeric>
#include <unordered_map>

namespace nlm {

struct TripletSTDP::Impl {
    float aplus;                    // Triple potentiation parameter
    float aminus;                   // Triple depression parameter
    float tauPlus;                  // Triple potentiation time constant
    float tauMinus;                 // Triple depression time constant
    float baselineLTPWeight;        // Baseline LTP weight
    float baselineLTDWeight;        // Baseline LTD weight
    
    // For triplet interactions: track recent spike triplets
    std::unordered_map<NeuronId, std::vector<Timestamp>> preSpikeHistory;
    std::unordered_map<NeuronId, std::vector<Timestamp>> postSpikeHistory;
    
    Impl() : aplus(0.005f), aminus(0.005f), tauPlus(100.0f), tauMinus(100.0f),
             baselineLTPWeight(0.01f), baselineLTDWeight(0.012f) {}
};

TripletSTDP::TripletSTDP() : pImpl(new Impl) {}

TripletSTDP::~TripletSTDP() = default;

void TripletSTDP::update(Synapse* synapse,
                          const std::vector<Timestamp>& preSpikes,
                          const std::vector<Timestamp>& postSpikes,
                          TimestepDuration dt) {
    /*
     * Triplet STDP implementation (Gerstner et al., 1996)
     * 
     * Combines pairwise STDP with triplet interactions for more
     * biologically realistic learning dynamics
     * 
     * Mathematical formulation:
     * 
     * Pairwise component (from standard STDP):
     *   Δw_pair = A+ * exp(-Δt / τ+) - A- * exp(Δt / τ-)
     * 
     * Triplet component:
     *   Δw_triplet = A+ * exp(-Δt / τ+) * (1 + exp(-(t - t_pre) / τ_minus))
     *               + A- * exp(Δt / τ-) * (1 + exp(-(t - t_post) / τ_plus))
     * 
     * Where:
     *   A+, A- = triplet interaction parameters
     *   τ+, τ- = triplet time constants
     *   t_pre, t_post = recent pre- and post-synaptic spike times
     */
    
    if (!synapse || preSpikes.empty() || postSpikes.empty()) {
        return;
    }
    
    float totalDelta = 0.0f;
    float baselineLTP = pImpl->baselineLTPWeight;
    float baselineLTD = pImpl->baselineLTDWeight;
    float aplus = pImpl->aplus;
    float aminus = pImpl->aminus;
    float tauPlus = pImpl->tauPlus;
    float tauMinus = pImpl->tauMinus;
    
    // Get most recent spikes
    Timestamp t_pre = preSpikes.back();
    Timestamp t_post = postSpikes.back();
    
    // Calculate timing difference
    float delta_t = static_cast<float>(t_post - t_pre);
    
    // Pairwise STDP component (standard STDP)
    float pairwiseDelta = 0.0f;
    if (delta_t > 0) {
        pairwiseDelta += baselineLTP * std::exp(-delta_t / tauPlus);
    } else if (delta_t < 0) {
        pairwiseDelta += -baselineLTD * std::exp(delta_t / tauMinus);
    }
    
    // Triplet interaction component
    float tripletDelta = 0.0f;
    
    if (delta_t > 0) {
        // Pre before post: triplet potentiation
        // Contribution from recent pre-synaptic spike
        tripletDelta += aplus * std::exp(-delta_t / tauPlus);
        // Plus additional contribution from spike timing
        tripletDelta += aplus * std::exp(-delta_t / tauMinus);
    } else if (delta_t < 0) {
        // Post before pre: triplet depression
        // Contribution from recent post-synaptic spike
        tripletDelta += -aminus * std::exp(delta_t / tauMinus);
        // Plus additional contribution from spike timing
        tripletDelta += -aminus * std::exp(delta_t / tauPlus);
    }
    
    // Combine pairwise and triplet components
    totalDelta = pairwiseDelta + tripletDelta;
    
    // Apply weight change with bounds
    if (std::abs(totalDelta) > 1e-6f) {
        // Scale by synaptic efficacy
        float efficacy = synapse->getEfficacy();
        totalDelta *= efficacy;
        
        // Inhibitory plasticity must strengthen inhibition
        if (synapse->getType() == SynapseType::Inhibitory) {
            totalDelta = -totalDelta;
        }
        
        // Apply weight-dependent bounds for stability
        {
            float w = synapse->getWeight();
            bool awayFromZero = std::abs(w + totalDelta) > std::abs(w);
            totalDelta *= awayFromZero ? (1.0f - std::abs(w)) : std::abs(w);
        }
        
        // Apply weight change
        synapse->addToWeight(totalDelta);
        
        // Enforce sign: excitatory stays >= 0.01, inhibitory stays <= -0.01
        if (synapse->getType() == SynapseType::Inhibitory && synapse->getWeight() > -0.01f) {
            synapse->setWeight(-0.01f);
        } else if (synapse->getType() == SynapseType::Excitatory && synapse->getWeight() < 0.01f) {
            synapse->setWeight(0.01f);
        }
    }
}

void TripletSTDP::applyWeightChange(Synapse* synapse, SynapticWeight delta) {
    if (!synapse) return;
    
    // Clamp weight to bounds
    float newWeight = synapse->getWeight() + delta;
    newWeight = std::clamp(newWeight, -0.5f, 0.5f); // Triplet STDP typically produces smaller weights
    synapse->setWeight(newWeight);
}

const char* TripletSTDP::getName() const {
    return "TripletSTDP";
}

void TripletSTDP::setAplus(float a) {
    pImpl->aplus = std::max(0.0f, a);
}

float TripletSTDP::getAplus() const {
    return pImpl->aplus;
}

void TripletSTDP::setAminus(float a) {
    pImpl->aminus = std::max(0.0f, a);
}

float TripletSTDP::getAminus() const {
    return pImpl->aminus;
}

void TripletSTDP::setTauPlus(float tau) {
    pImpl->tauPlus = std::clamp(tau, 10.0f, 500.0f);
}

float TripletSTDP::getTauPlus() const {
    return pImpl->tauPlus;
}

void TripletSTDP::setTauMinus(float tau) {
    pImpl->tauMinus = std::clamp(tau, 10.0f, 500.0f);
}

float TripletSTDP::getTauMinus() const {
    return pImpl->tauMinus;
}

void TripletSTDP::setBaselineLTPWeight(float w) {
    pImpl->baselineLTPWeight = std::clamp(w, 0.0f, 1.0f);
}

float TripletSTDP::getBaselineLTPWeight() const {
    return pImpl->baselineLTPWeight;
}

void TripletSTDP::setBaselineLTDWeight(float w) {
    pImpl->baselineLTDWeight = std::clamp(w, 0.0f, 1.0f);
}

float TripletSTDP::getBaselineLTDWeight() const {
    return pImpl->baselineLTDWeight;
}

} // namespace nlm