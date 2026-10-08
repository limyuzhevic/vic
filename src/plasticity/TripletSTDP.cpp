// Triplet STDP implementation
#include "STDP.hpp"
#include "../brain/Synapse.hpp"
#include <cmath>
#include <algorithm>

namespace nlm {

struct TripletSTDP::Impl {
    float Aplus;                  // Pre-post-pre triplet potentiation
    float Aminus;                 // Post-pre-post triplet depression
    float Atimes;                 // Pre-post interaction term
    float Bpre;                   // Pre-only depression
    float Bpost;                  // Post-only depression
    float tau1;                   // Time constant for pre spikes
    float tau2;                   // Time constant for post spikes
    float minWeight;              // Minimum synaptic weight
    float maxWeight;              // Maximum synaptic weight
    
    // Triplet state variables
    float x1;                     // Pre spike trace
    float x2;                     // Post spike trace
    float y;                      // Post spike trace for triplet interactions
    
    Impl() : Aplus(0.005f), Aminus(0.004f), Atimes(0.002f),
             Bpre(0.001f), Bpost(0.001f), tau1(20.0f), tau2(20.0f),
             minWeight(-1.0f), maxWeight(1.0f), x1(0.0f), x2(0.0f), y(0.0f) {}
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
    
    float dtFloat = static_cast<float>(dt);
    
    // Update traces for each spike
    for (Timestamp preTime : preSpikes) {
        // Decay pre trace
        pImpl->x1 *= std::exp(-dtFloat / pImpl->tau1);
        pImpl->y *= std::exp(-dtFloat / pImpl->tau2);
        
        // Pre spike increases traces
        pImpl->x1 += 1.0f;
        pImpl->y += 1.0f;
    }
    
    for (Timestamp postTime : postSpikes) {
        // Decay traces
        pImpl->x1 *= std::exp(-dtFloat / pImpl->tau1);
        pImpl->x2 *= std::exp(-dtFloat / pImpl->tau2);
        
        // Post spike increases post trace
        pImpl->x2 += 1.0f;
    }
    
    // Calculate weight change from different triplet contributions
    float totalDelta = 0.0f;
    
    // Pre-post interaction
    totalDelta += pImpl->Atimes * pImpl->x1 * pImpl->x2;
    
    // Pre-post-pre triplet (pre before pre before post)
    for (size_t i = 0; i < preSpikes.size(); ++i) {
        for (size_t j = i; j < preSpikes.size(); ++j) {
            for (size_t k = 0; k < postSpikes.size(); ++k) {
                // Ensure temporal order: pre[i] < pre[j] < post[k]
                if (preSpikes[i] < preSpikes[j] && preSpikes[j] < postSpikes[k]) {
                    float timeDiff1 = static_cast<float>(preSpikes[j] - preSpikes[i]);
                    float timeDiff2 = static_cast<float>(postSpikes[k] - preSpikes[j]);
                    if (timeDiff1 < 100.0f && timeDiff2 < 100.0f) {
                        float tripletEffect = pImpl->Aplus * 
                                            std::exp(-timeDiff1 / pImpl->tau1) *
                                            std::exp(-timeDiff2 / pImpl->tau2);
                        totalDelta += tripletEffect;
                    }
                }
            }
        }
    }
    
    // Post-pre-post triplet (post before pre before post)
    for (size_t i = 0; i < postSpikes.size(); ++i) {
        for (size_t j = 0; j < preSpikes.size(); ++j) {
            for (size_t k = 0; k < postSpikes.size(); ++k) {
                // Ensure temporal order: post[i] < pre[j] < post[k]
                if (postSpikes[i] < preSpikes[j] && preSpikes[j] < postSpikes[k]) {
                    float timeDiff1 = static_cast<float>(preSpikes[j] - postSpikes[i]);
                    float timeDiff2 = static_cast<float>(postSpikes[k] - preSpikes[j]);
                    if (timeDiff1 < 100.0f && timeDiff2 < 100.0f) {
                        float tripletEffect = -pImpl->Aminus * 
                                            std::exp(-timeDiff1 / pImpl->tau1) *
                                            std::exp(-timeDiff2 / pImpl->tau2);
                        totalDelta += tripletEffect;
                    }
                }
            }
        }
    }
    
    // Pre-only depression (ongoing depression without post)
    totalDelta += -pImpl->Bpre * pImpl->x1;
    
    // Post-only depression (ongoing depression without pre)
    totalDelta += -pImpl->Bpost * pImpl->x2;
    
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

void TripletSTDP::setAplus(float weight) {
    pImpl->Aplus = std::clamp(weight, 0.0f, 1.0f);
}

float TripletSTDP::getAplus() const {
    return pImpl->Aplus;
}

void TripletSTDP::setAminus(float weight) {
    pImpl->Aminus = std::clamp(weight, 0.0f, 1.0f);
}

float TripletSTDP::getAminus() const {
    return pImpl->Aminus;
}

void TripletSTDP::setAtimes(float weight) {
    pImpl->Atimes = std::clamp(weight, 0.0f, 1.0f);
}

float TripletSTDP::getAtimes() const {
    return pImpl->Atimes;
}

void TripletSTDP::setBpre(float weight) {
    pImpl->Bpre = std::clamp(weight, 0.0f, 1.0f);
}

float TripletSTDP::getBpre() const {
    return pImpl->Bpre;
}

void TripletSTDP::setBpost(float weight) {
    pImpl->Bpost = std::clamp(weight, 0.0f, 1.0f);
}

float TripletSTDP::getBpost() const {
    return pImpl->Bpost;
}

void TripletSTDP::setTau1(float tau) {
    pImpl->tau1 = std::clamp(tau, 1.0f, 100.0f);
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

void TripletSTDP::setMinWeight(float minWeight) {
    pImpl->minWeight = std::clamp(minWeight, -10.0f, 0.0f);
}

float TripletSTDP::getMinWeight() const {
    return pImpl->minWeight;
}

void TripletSTDP::setMaxWeight(float maxWeight) {
    pImpl->maxWeight = std::clamp(maxWeight, 0.0f, 10.0f);
}

float TripletSTDP::getMaxWeight() const {
    return pImpl->maxWeight;
}

} // namespace nlm
