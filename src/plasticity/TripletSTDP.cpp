#include "STDP.hpp"
#include "../../brain/Synapse.hpp"
#include <cmath>
#include <algorithm>
#include <map>

namespace nlm {

struct TripletSTDP::Impl {
    float a1;              // A1: pre-before-post triple contribution
    float a2;              // A2: post-before-pre triple contribution
    float a3;              // A3: post-post-pre triple contribution
    float b1;              // B1: pre-post-post triple contribution
    float b2;              // B2: post-pre-pre triple contribution
    float tau1;            // τ1: fast window (~10-20ms)
    float tau2;            // τ2: slow window (~100-200ms)
    float tau3;            // τ3: very slow window (~500-1000ms)
    float minWeight;       // Minimum synaptic weight
    float maxWeight;       // Maximum synaptic weight
    float tripletStrength; // Overall triplet interaction scaling
    float baselineTrace;   // Baseline eligibility trace
    
    // Spike history for triplet calculations
    std::map<SynapseId, std::vector<Timestamp>> preSpikeHistory;
    std::map<SynapseId, std::vector<Timestamp>> postSpikeHistory;
    
    Impl() : a1(0.01f), a2(0.012f), a3(0.005f), b1(0.005f), b2(0.012f),
             tau1(15.0f), tau2(150.0f), tau3(800.0f),
             minWeight(-1.0f), maxWeight(1.0f),
             tripletStrength(1.0f), baselineTrace(0.0f) {}
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
    
    // Update spike history
    for (Timestamp spike : preSpikes) {
        pImpl->preSpikeHistory[synapse->getId()].push_back(spike);
        // Keep only recent spikes for efficiency
        if (pImpl->preSpikeHistory[synapse->getId()].size() > 100) {
            pImpl->preSpikeHistory[synapse->getId()].erase(
                pImpl->preSpikeHistory[synapse->getId()].begin());
        }
    }
    
    for (Timestamp spike : postSpikes) {
        pImpl->postSpikeHistory[synapse->getId()].push_back(spike);
        if (pImpl->postSpikeHistory[synapse->getId()].size() > 100) {
            pImpl->postSpikeHistory[synapse->getId()].erase(
                pImpl->postSpikeHistory[synapse->getId()].begin());
        }
    }
    
    float totalDelta = 0.0f;
    
    // Calculate triplet contributions
    // Using recent spike history for efficiency
    const auto& preHistory = pImpl->preSpikeHistory[synapse->getId()];
    const auto& postHistory = pImpl->postSpikeHistory[synapse->getId()];
    
    // For each pre spike, calculate contributions with recent post spikes
    for (Timestamp preTime : preHistory) {
        float sumA = 0.0f, sumB = 0.0f;
        
        for (Timestamp postTime : postHistory) {
            float deltaT = static_cast<float>(postTime - preTime);
            
            if (deltaT > 0 && deltaT <= pImpl->tau1) {
                // Pre before post: A1 contribution (fast)
                sumA += std::exp(-deltaT / pImpl->tau1);
            } else if (deltaT < 0 && std::abs(deltaT) <= pImpl->tau1) {
                // Post before pre: A2 contribution (fast)
                sumA += std::exp(deltaT / pImpl->tau1);
            } else if (deltaT > 0 && deltaT <= pImpl->tau2) {
                // Post before pre: A3 contribution (slow)
                sumA += std::exp(-deltaT / pImpl->tau2);
            } else if (deltaT < 0 && std::abs(deltaT) <= pImpl->tau2) {
                // Pre before post: B1 contribution (slow)
                sumB += std::exp(deltaT / pImpl->tau2);
            }
        }
        
        // Add contributions for each pre spike
        totalDelta += pImpl->a1 * sumA + pImpl->b1 * sumB;
    }
    
    // Calculate post spike contributions
    for (Timestamp postTime : postHistory) {
        float sumA2 = 0.0f, sumB2 = 0.0f;
        
        for (Timestamp preTime : preHistory) {
            float deltaT = static_cast<float>(postTime - preTime);
            
            if (deltaT > 0 && deltaT <= pImpl->tau2) {
                // Pre before post: A2 contribution (slow)
                sumA2 += std::exp(-deltaT / pImpl->tau2);
            } else if (deltaT < 0 && std::abs(deltaT) <= pImpl->tau1) {
                // Post before pre: B2 contribution (fast)
                sumA2 += std::exp(deltaT / pImpl->tau1);
            }
        }
        
        // Add contributions for each post spike
        totalDelta += pImpl->a2 * sumA2 + pImpl->b2 * sumB2;
    }
    
    // Apply triplet strength factor
    totalDelta *= pImpl->tripletStrength;
    
    // Apply synaptic normalization
    float totalDeltasq = totalDelta * totalDelta;
    if (totalDeltasq > 0.001f) {
        float weightMagnitude = std::abs(synapse->getWeight());
        if (weightMagnitude > 0.1f) {
            totalDelta *= 0.1f / weightMagnitude;
        }
    }
    
    // Apply weight change with bounds
    if (std::abs(totalDelta) > 1e-6f) {
        float efficacy = synapse->getEfficacy();
        totalDelta *= efficacy;
        
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

void TripletSTDP::setA1(float a1) {
    pImpl->a1 = std::clamp(a1, 0.0f, 0.1f);
}

void TripletSTDP::setA2(float a2) {
    pImpl->a2 = std::clamp(a2, 0.0f, 0.1f);
}

void TripletSTDP::setA3(float a3) {
    pImpl->a3 = std::clamp(a3, 0.0f, 0.1f);
}

void TripletSTDP::setB1(float b1) {
    pImpl->b1 = std::clamp(b1, 0.0f, 0.1f);
}

void TripletSTDP::setB2(float b2) {
    pImpl->b2 = std::clamp(b2, 0.0f, 0.1f);
}

void TripletSTDP::setTau1(float tau1) {
    pImpl->tau1 = std::clamp(tau1, 1.0f, 50.0f);
}

void TripletSTDP::setTau2(float tau2) {
    pImpl->tau2 = std::clamp(tau2, 50.0f, 500.0f);
}

void TripletSTDP::setTau3(float tau3) {
    pImpl->tau3 = std::clamp(tau3, 100.0f, 2000.0f);
}

void TripletSTDP::setMinWeight(float min) {
    pImpl->minWeight = min;
}

void TripletSTDP::setMaxWeight(float max) {
    pImpl->maxWeight = max;
}

void TripletSTDP::setTripletInteractionStrength(float strength) {
    pImpl->tripletStrength = std::clamp(strength, 0.0f, 10.0f);
}

void TripletSTDP::setSynapticEligibilityTrace(float trace) {
    pImpl->baselineTrace = trace;
}

void TripletSTDP::configure(float a1, float a2, float a3, float b1, float b2,
                             float tau1, float tau2, float tau3, float strength) {
    setA1(a1); setA2(a2); setA3(a3); setB1(b1); setB2(b2);
    setTau1(tau1); setTau2(tau2); setTau3(tau3);
    setTripletInteractionStrength(strength);
}

float TripletSTDP::getTripletSum(const std::vector<Timestamp>& spikes, float tau) const {
    float sum = 0.0f;
    Timestamp lastTime = -1.0;
    
    for (Timestamp spike : spikes) {
        if (lastTime > 0.0) {
            float deltaT = static_cast<float>(spike - lastTime);
            sum += std::exp(-std::abs(deltaT) / tau);
        }
        lastTime = spike;
    }
    
    return sum;
}

} // namespace nlm