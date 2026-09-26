#include "TripletSTDP.hpp"
#include "../../brain/Synapse.hpp"
#include <cmath>
#include <algorithm>

namespace nlm {

struct TripletSTDP::Impl {
    // Triplet model parameters (Wang et al., 2002)
    float a1;                // First spike interaction term (A+)
    float a2;                // Second spike interaction term (A-)
    float b1;                // Second spike facilitation
    float b2;                // First spike facilitation
    float tau1;              // Time constant for facilitation
    float tau2;              // Time constant for depression
    float minWeight;         // Minimum synaptic weight
    float maxWeight;         // Maximum synaptic weight
    
    // State variables for triplet model
    float facilitation1;    // First spike facilitation trace
    float facilitation2;    // Second spike facilitation trace
    Timestamp lastPreSpike; // Time of last pre-synaptic spike
    Timestamp lastPostSpike; // Time of last post-synaptic spike
    
    Impl() : a1(0.01f), a2(0.012f), b1(0.5f), b2(0.5f), 
             tau1(10.0f), tau2(20.0f), minWeight(-1.0f), maxWeight(1.0f),
             facilitation1(0.0f), facilitation2(0.0f), 
             lastPreSpike(-1.0f), lastPostSpike(-1.0f) {}
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
    
    // Update facilitation traces
    Timestamp currentTime = 0; // Note: This should come from actual simulation time
    float currentTrace1 = pImpl->facilitation1;
    float currentTrace2 = pImpl->facilitation2;
    
    // Decay facilitation traces
    currentTrace1 *= std::exp(-dt / pImpl->tau1);
    currentTrace2 *= std::exp(-dt / pImpl->tau2);
    
    // Update facilitation based on most recent spikes
    if (!preSpikes.empty()) {
        float timeSinceLastPre = static_cast<float>(currentTime - pImpl->lastPreSpike);
        currentTrace2 += std::exp(-timeSinceLastPre / pImpl->tau1);
    }
    
    if (!postSpikes.empty()) {
        float timeSinceLastPost = static_cast<float>(currentTime - pImpl->lastPostSpike);
        currentTrace1 += std::exp(-timeSinceLastPost / pImpl->tau2);
    }
    
    // Update last spike times
    if (!preSpikes.empty()) {
        pImpl->lastPreSpike = preSpikes.back();
    }
    if (!postSpikes.empty()) {
        pImpl->lastPostSpike = postSpikes.back();
    }
    
    pImpl->facilitation1 = currentTrace1;
    pImpl->facilitation2 = currentTrace2;
    
    // Compute weight changes using triplet model
    float totalDelta = 0.0f;
    
    // LTP contributions (pre before post)
    for (size_t i = 0; i < preSpikes.size(); ++i) {
        for (size_t j = 0; j < postSpikes.size(); ++j) {
            float dt = static_cast<float>(postSpikes[j] - preSpikes[i]);
            if (dt > 0) {
                // Single spike LTP
                totalDelta += pImpl->a1 * std::exp(-dt / pImpl->tau1);
                
                // Triple spike LTP (post-after-pre)
                if (j > 0 && i > 0) {
                    float dt_post = static_cast<float>(postSpikes[j] - postSpikes[j-1]);
                    float dt_pre = static_cast<float>(preSpikes[i] - preSpikes[i-1]);
                    if (dt_post > 0 && dt_pre > 0) {
                        totalDelta += pImpl->b1 * pImpl->facilitation2 * std::exp(-dt_post / pImpl->tau1);
                    }
                }
            }
        }
    }
    
    // LTD contributions (post before pre)
    for (size_t i = 0; i < preSpikes.size(); ++i) {
        for (size_t j = 0; j < postSpikes.size(); ++j) {
            float dt = static_cast<float>(postSpikes[j] - preSpikes[i]);
            if (dt < 0) {
                // Single spike LTD
                totalDelta -= pImpl->a2 * std::exp(dt / pImpl->tau2);
                
                // Triple spike LTD (pre-after-post)
                if (i > 0 && j > 0) {
                    float dt_post = static_cast<float>(postSpikes[j] - postSpikes[j-1]);
                    float dt_pre = static_cast<float>(preSpikes[i] - preSpikes[i-1]);
                    if (dt_post > 0 && dt_pre > 0) {
                        totalDelta -= pImpl->b2 * pImpl->facilitation1 * std::exp(-dt_post / pImpl->tau2);
                    }
                }
            }
        }
    }
    
    // Apply weight change with bounds
    if (std::abs(totalDelta) > 1e-6f) {
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

void TripletSTDP::setTau1(float tau) {
    pImpl->tau1 = std::clamp(tau, 1.0f, 50.0f);
}

float TripletSTDP::getTau1() const {
    return pImpl->tau1;
}

void TripletSTDP::setTau2(float tau) {
    pImpl->tau2 = std::clamp(tau, 1.0f, 100.0f);
}

float TripletSTDP::getTau2() const {
    return pImpl->tau2;
}

void TripletSTDP::setBaseSTDP(float ltpWeight, float ltdWeight, float tau) {
    setA1(ltpWeight);
    setA2(ltdWeight);
    setTau1(tau);
    setTau2(tau);
}

} // namespace nlm