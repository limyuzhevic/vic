#include "RewardModulatedSTDP.hpp"
#include "../brain/Synapse.hpp"
#include <cmath>
#include <algorithm>

namespace nlm {

struct TripletSTDP::Impl {
    float aPlusPre;      // Pre-synaptic triplet potentiation (A+)
    float aMinusPost;    // Post-synaptic triplet depression (A-)
    float aTau;          // Time constant for triplet effects (τ)
    float minWeight;     // Minimum synaptic weight
    float maxWeight;     // Maximum synaptic weight
    
    Impl() : aPlusPre(0.005f), aMinusPost(0.005f), aTau(20.0f),
             minWeight(-1.0f), maxWeight(1.0f) {}
};

TripletSTDP::TripletSTDP() : pImpl(new Impl) {}

TripletSTDP::~TripletSTDP() = default;

void TripletSTDP::update(Synapse* synapse,
                         const std::vector<Timestamp>& preSpikes,
                         const std::vector<Timestamp>& postSpikes,
                         TimestepDuration dt) {
    if (!synapse || preSpikes.empty() || postSpikes.empty()) {
        return;
    }
    
    // Triplet STDP implementation based on Grabsky & Tsodyks (2016)
    // Considers contributions from spike triplets: pre-pre-post and pre-post-post
    
    // Get spike times
    Timestamp t_pre1 = preSpikes[preSpikes.size() - 1]; // Most recent pre
    Timestamp t_pre0 = (preSpikes.size() > 1) ? preSpikes[preSpikes.size() - 2] : t_pre1;
    Timestamp t_post0 = postSpikes[postSpikes.size() - 1]; // Most recent post
    
    float totalDelta = 0.0f;
    
    // Pre-pre-post triplet (3 spikes): potentiation
    if (preSpikes.size() >= 2 && postSpikes.size() >= 1) {
        float dt1 = static_cast<float>((t_pre0 - t_pre1) * 1000.0); // Inter-spike interval
        float dt2 = static_cast<float>((t_post0 - t_pre0) * 1000.0); // Time from second pre to post
        
        if (dt1 > 0 && dt2 > 0) {
            totalDelta += pImpl->aPlusPre * std::exp(-dt1 / pImpl->aTau) * std::exp(-dt2 / pImpl->aTau);
        }
    }
    
    // Pre-post-post triplet (3 spikes): depression
    if (postSpikes.size() >= 2 && preSpikes.size() >= 1) {
        Timestamp t_post1 = postSpikes[postSpikes.size() - 2]; // Previous post
        float dt1 = static_cast<float>((t_post0 - t_post1) * 1000.0); // Inter-spike interval
        float dt2 = static_cast<float>((t_post0 - t_pre1) * 1000.0); // Time from pre to second post
        
        if (dt1 > 0 && dt2 > 0) {
            totalDelta += -pImpl->aMinusPost * std::exp(-dt1 / pImpl->aTau) * std::exp(-dt2 / pImpl->aTau);
        }
    }
    
    // Classic pairwise STDP contribution
    float dtMs = static_cast<float>((t_post0 - t_pre1) * 1000.0);
    if (dtMs > 0) {
        totalDelta += pImpl->aPlusPre * 0.01f * std::exp(-dtMs / pImpl->aTau); // Reduced LTP
    } else if (dtMs < 0) {
        totalDelta += -pImpl->aMinusPost * 0.01f * std::exp(dtMs / pImpl->aTau); // Reduced LTD
    }
    
    // Apply triplet STDP contribution if significant
    if (std::abs(totalDelta) > 1e-6f) {
        synapse->setEligibilityTrace(totalDelta);
    }
}

void TripletSTDP::applyWeightChange(Synapse* synapse, SynapticWeight delta) {
    if (!synapse) return;
    
    // Get eligibility trace from triplet STDP
    float eligibilityTrace = synapse->getEligibilityTrace();
    if (std::abs(eligibilityTrace) < 1e-6f) {
        return;
    }
    
    // Apply weight change
    float newWeight = synapse->getWeight() + eligibilityTrace;
    newWeight = std::clamp(newWeight, pImpl->minWeight, pImpl->maxWeight);
    synapse->setWeight(newWeight);
    
    // Decay eligibility trace
    synapse->decayEligibilityTrace(0.95f);
}

const char* TripletSTDP::getName() const {
    return "TripletSTDP";
}

void TripletSTDP::setAPlusPre(float weight) {
    pImpl->aPlusPre = std::clamp(weight, 0.0f, 0.1f);
}

float TripletSTDP::getAPlusPre() const {
    return pImpl->aPlusPre;
}

void TripletSTDP::setAMinusPost(float weight) {
    pImpl->aMinusPost = std::clamp(weight, 0.0f, 0.1f);
}

float TripletSTDP::getAMinusPost() const {
    return pImpl->aMinusPost;
}

void TripletSTDP::setATau(float tau) {
    pImpl->aTau = std::clamp(tau, 1.0f, 100.0f);
}

float TripletSTDP::getATau() const {
    return pImpl->aTau;
}

} // namespace nlm
