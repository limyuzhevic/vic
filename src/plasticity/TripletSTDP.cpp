#include "STDP.hpp"
#include "../../brain/Synapse.hpp"
#include <cmath>
#include <algorithm>

namespace nlm {

struct TripletSTDP::Impl {
    float ltpWeight;      // A+ for potentiation (pre-before-post)
    float ltdWeight;      // A- for depression (post-before-pre)
    float timeConstant;   // Tau for exponential window (ms)
    float a1;             // Pre-before-post potentiation (triplet)
    float a2;             // Post-after-pre potentiation (triplet)
    float b1;             // Pre-before-post depression (triplet)
    float b2;             // Post-after-pre depression (triplet)
    float minWeight;      // Minimum synaptic weight
    float maxWeight;      // Maximum synaptic weight
    
    // History tracking for triplet interactions
    struct SpikeHistory {
        Timestamp preTime;
        Timestamp postTime;
        float eligibilityPre;   // Presynaptic eligibility trace
        float eligibilityPost;  // Postsynaptic eligibility trace
    };
    
    std::vector<SpikeHistory> spikeHistory;
    
    Impl() : ltpWeight(0.01f), ltdWeight(0.012f), timeConstant(20.0f),
             a1(0.005f), a2(0.002f), b1(0.01f), b2(0.012f),
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
    
    float totalDelta = 0.0f;
    float tau = pImpl->timeConstant;
    Timestamp currentTime = 0;  // Would be provided by simulation
    
    // Add current spikes to history
    for (Timestamp preTime : preSpikes) {
        for (Timestamp postTime : postSpikes) {
            pImpl->spikeHistory.push_back({preTime, postTime, 1.0f, 1.0f});
        }
    }
    
    // Process spike history to compute triplet interactions
    float tracePre = 0.0f;
    float tracePost = 0.0f;
    
    for (const auto& spike : pImpl->spikeHistory) {
        float age = static_cast<float>(currentTime - spike.preTime);
        
        if (age < tau * 3) {  // Trace duration
            // Update traces based on spike timing
            if (spike.postTime > spike.preTime) {
                // Pre-before-post interaction
                tracePre += pImpl->a1 * std::exp(-age / tau);
                tracePost += pImpl->a2 * std::exp(-age / tau);
            } else {
                // Post-before-pre interaction
                tracePre += pImpl->b1 * std::exp(-age / tau);
                tracePost += pImpl->b2 * std::exp(-age / tau);
            }
        }
    }
    
    // Compute weight change from traces
    totalDelta = (tracePre - tracePost) * 0.01f;  // Scaling factor
    
    // Clean old spike history (keep only recent spikes)
    auto& history = pImpl->spikeHistory;
    history.erase(std::remove_if(history.begin(), history.end(),
                                 [this, currentTime](const auto& spike) {
                                     return static_cast<float>(currentTime - spike.preTime) > tau * 3;
                                 }),
                  history.end());
    
    // Apply weight change with bounds
    if (std::abs(totalDelta) > 1e-6f) {
        applyWeightChange(synapse, totalDelta);
    }
}

void TripletSTDP::applyWeightChange(Synapse* synapse, SynapticWeight delta) {
    if (!synapse) return;
    
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
    pImpl->timeConstant = std::clamp(tau, 1.0f, 100.0f);
}

float TripletSTDP::getTimeConstant() const {
    return pImpl->timeConstant;
}

void TripletSTDP::setA1(float a1) {
    pImpl->a1 = std::clamp(a1, 0.0f, 1.0f);
}

float TripletSTDP::getA1() const {
    return pImpl->a1;
}

void TripletSTDP::setA2(float a2) {
    pImpl->a2 = std::clamp(a2, 0.0f, 1.0f);
}

float TripletSTDP::getA2() const {
    return pImpl->a2;
}

void TripletSTDP::setB1(float b1) {
    pImpl->b1 = std::clamp(b1, 0.0f, 1.0f);
}

float TripletSTDP::getB1() const {
    return pImpl->b1;
}

void TripletSTDP::setB2(float b2) {
    pImpl->b2 = std::clamp(b2, 0.0f, 1.0f);
}

float TripletSTDP::getB2() const {
    return pImpl->b2;
}

} // namespace nlm