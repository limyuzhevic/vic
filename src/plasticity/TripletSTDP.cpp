#include "STDP.hpp"
#include "../../brain/Synapse.hpp"
#include <cmath>
#include <algorithm>

namespace nlm {

struct TripletSTDP::Impl {
    float apWeight;        // Autapical potentiation weight
    float pspWeight;       // Postsynaptic potentiation weight
    float ahpWeight;       // Afterhyperpolarization depression weight
    float tripletWindow;   // Time window for triplet interactions
    float minWeight;       // Minimum synaptic weight
    float maxWeight;       // Maximum synaptic weight
    
    Impl() : apWeight(0.005f), pspWeight(0.005f), ahpWeight(0.005f), 
             tripletWindow(100.0f), minWeight(-1.0f), maxWeight(1.0f) {}
};

TripletSTDP::TripletSTDP() : pImpl(new Impl) {}

TripletSTDP::~TripletSTDP() = default;

void TripletSTDP::update(Synapse* synapse,
                          const std::vector<Timestamp>& preSpikes,
                          const std::vector<Timestamp>& postSpikes,
                          TimestepDuration dt) {
    if (!synapse || preSpikes.size() < 2 || postSpikes.size() < 2) {
        return; // Need at least 2 spikes for triplet interactions
    }
    
    // Triplet STDP captures:
    // 1. Pre before post (standard STDP)
    // 2. Presynaptic spike + postsynaptic spike within triplet window
    // 3. Postsynaptic spike + presynaptic spike within triplet window
    
    float totalDelta = 0.0f;
    float tau = pImpl->tripletWindow;
    
    // 1. Standard pairwise STDP (first-order interaction)
    float pairDelta = computePairwiseSTDP(preSpikes, postSpikes, pImpl->apWeight, pImpl->ahpWeight, tau);
    totalDelta += pairDelta * 0.5f;  // Weight this contribution
    
    // 2. Autapical potentiation (pre + post within window)
    float apDelta = computeAutapicalPotentiation(preSpikes, postSpikes, pImpl->apWeight, tau);
    totalDelta += apDelta * 0.3f;
    
    // 3. Postsynaptic potentiation (post + pre within window, reversed timing)
    float pspDelta = computePostsynapticPotentiation(postSpikes, preSpikes, pImpl->pspWeight, tau);
    totalDelta += pspDelta * 0.2f;
    
    // 4. Afterhyperpolarization depression (stronger inhibition)
    float ahpDelta = computeAfterhyperpolarizationDepression(preSpikes, postSpikes, pImpl->ahpWeight, tau);
    totalDelta -= ahpDelta * 0.4f;  // Negative contribution
    
    // Apply weight change with bounds
    if (std::abs(totalDelta) > 1e-6f) {
        float efficacy = synapse->getEfficacy();
        totalDelta *= efficacy;
        synapse->addToWeight(totalDelta);
        
        // Update eligibility trace
        float currentTrace = synapse->getEligibilityTrace();
        synapse->setEligibilityTrace(currentTrace + totalDelta);
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

void TripletSTDP::setAPWeight(float weight) {
    pImpl->apWeight = std::clamp(weight, 0.0f, 0.1f);
}

float TripletSTDP::getAPWeight() const {
    return pImpl->apWeight;
}

void TripletSTDP::setPSPWeight(float weight) {
    pImpl->pspWeight = std::clamp(weight, 0.0f, 0.1f);
}

float TripletSTDP::getPSPWeight() const {
    return pImpl->pspWeight;
}

void TripletSTDP::setAHPWeight(float weight) {
    pImpl->ahpWeight = std::clamp(weight, 0.0f, 0.1f);
}

float TripletSTDP::getAHPWeight() const {
    return pImpl->ahpWeight;
}

// Helper functions for triplet STDP computation

float TripletSTDP::computePairwiseSTDP(const std::vector<Timestamp>& preSpikes,
                                         const std::vector<Timestamp>& postSpikes,
                                         float apWeight, float ahpWeight, float tau) {
    float total = 0.0f;
    
    for (Timestamp preTime : preSpikes) {
        for (Timestamp postTime : postSpikes) {
            float dt = static_cast<float>(postTime - preTime);
            
            if (dt > 0) {
                // Potentiation
                total += apWeight * std::exp(-dt / tau);
            } else if (dt < 0) {
                // Depression
                total += -ahpWeight * std::exp(dt / tau);
            }
        }
    }
    
    return total;
}

float TripletSTDP::computeAutapicalPotentiation(const std::vector<Timestamp>& preSpikes,
                                                 const std::vector<Timestamp>& postSpikes,
                                                 float weight, float tau) {
    float total = 0.0f;
    
    for (size_t i = 0; i < preSpikes.size(); ++i) {
        for (size_t j = 0; j < postSpikes.size(); ++j) {
            // Count pre-post pairs within triplet window
            float dt = static_cast<float>(postSpikes[j] - preSpikes[i]);
            if (dt > 0 && dt <= tau) {
                // Strong potentiation for pre-post within window
                total += weight * std::exp(-dt / tau);
            }
        }
    }
    
    return total;
}

float TripletSTDP::computePostsynapticPotentiation(const std::vector<Timestamp>& postSpikes,
                                                   const std::vector<Timestamp>& preSpikes,
                                                   float weight, float tau) {
    float total = 0.0f;
    
    for (size_t i = 0; i < postSpikes.size(); ++i) {
        for (size_t j = 0; j < preSpikes.size(); ++j) {
            // Count post-pre pairs (reversed timing) within triplet window
            float dt = static_cast<float>(preSpikes[j] - postSpikes[i]);
            if (dt > 0 && dt <= tau) {
                // Postsynaptic potentiation for post-pre within window
                total += weight * std::exp(-dt / tau);
            }
        }
    }
    
    return total;
}

float TripletSTDP::computeAfterhyperpolarizationDepression(const std::vector<Timestamp>& preSpikes,
                                                            const std::vector<Timestamp>& postSpikes,
                                                            float weight, float tau) {
    float total = 0.0f;
    
    for (Timestamp preTime : preSpikes) {
        for (Timestamp postTime : postSpikes) {
            float dt = static_cast<float>(postTime - preTime);
            
            // AHP depression is stronger for larger negative dt (post before pre)
            if (dt < 0 && dt <= -tau * 0.1f) {  // Only significant depression for strong anti-correlation
                total += weight * std::exp(dt / tau);
            }
        }
    }
    
    return total;
}

} // namespace nlm
