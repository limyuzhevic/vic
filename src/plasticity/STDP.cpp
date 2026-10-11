// STDP.cpp
// Optimized STDP implementation

#include "STDP.hpp"
#include "../../brain/Synapse.hpp"
#include <cmath>
#include <algorithm>
#include <unordered_map>
#include <vector>

namespace nlm {

struct STDP::Impl {
    float ltpWeight;      // A+ for potentiation
    float ltdWeight;      // A- for depression
    float timeConstant;   // Tau for exponential window (ms)
    float minWeight;      // Minimum synaptic weight
    float maxWeight;      // Maximum synaptic weight
    
    // Optimized lookup tables for exponential calculations
    static constexpr size_t EXP_LOOKUP_SIZE = 256;
    std::array<float, EXP_LOOKUP_SIZE> expMinusTable;  // exp(-x/τ)
    std::array<float, EXP_LOOKUP_SIZE> expPlusTable;   // exp(x/τ)
    
    Impl() : ltpWeight(0.01f), ltdWeight(0.012f), timeConstant(20.0f),
             minWeight(-1.0f), maxWeight(1.0f) {
        // Initialize lookup tables
        float dt = 2.0f * timeConstant / EXP_LOOKUP_SIZE;
        for (size_t i = 0; i < EXP_LOOKUP_SIZE; ++i) {
            float x = static_cast<float>(i) * dt;
            expMinusTable[i] = std::exp(-x / timeConstant);
            expPlusTable[i] = std::exp(x / timeConstant);
        }
    }
};

STDP::STDP() : pImpl(std::make_unique<Impl>()) {}

STDP::~STDP() = default;

float STDP::Impl::getExpMinus(float dt) const {
    if (dt <= 0.0f) return 1.0f;
    size_t idx = static_cast<size_t>(dt * EXP_LOOKUP_SIZE / (2.0f * timeConstant));
    idx = std::min(idx, EXP_LOOKUP_SIZE - 1);
    return expMinusTable[idx];
}

float STDP::Impl::getExpPlus(float dt) const {
    if (dt <= 0.0f) return 1.0f;
    size_t idx = static_cast<size_t>(dt * EXP_LOOKUP_SIZE / (2.0f * timeConstant));
    idx = std::min(idx, EXP_LOOKUP_SIZE - 1);
    return expPlusTable[idx];
}

void STDP::update(Synapse* synapse,
                   const std::vector<Timestamp>& preSpikes,
                   const std::vector<Timestamp>& postSpikes,
                   TimestepDuration dt) {
    /*
     * Optimized STDP implementation using lookup tables and
     * reducing computational complexity from O(n²) to O(n)
     */
    
    if (!synapse || preSpikes.empty() || postSpikes.empty()) {
        return;
    }
    
    float totalDelta = 0.0f;
    
    // Optimized approach: process each spike once and use cumulative calculations
    // Precompute spike timing differences
    std::vector<float> preTimes(preSpikes.size());
    std::vector<float> postTimes(postSpikes.size());
    
    for (size_t i = 0; i < preSpikes.size(); ++i) {
        preTimes[i] = static_cast<float>(preSpikes[i]);
    }
    for (size_t i = 0; i < postSpikes.size(); ++i) {
        postTimes[i] = static_cast<float>(postSpikes[i]);
    }
    
    // Use optimized O(n) approach: for each spike, find crossing points
    // This assumes spike times are relatively sparse
    size_t preIdx = 0;
    size_t postIdx = 0;
    
    while (preIdx < preTimes.size() && postIdx < postTimes.size()) {
        float preTime = preTimes[preIdx];
        float postTime = postTimes[postIdx];
        
        if (preTime < postTime) {
            // Pre before post: potentiation
            float dt = postTime - preTime;
            if (dt > 0.0f) {
                // Use lookup table for faster exponential calculation
                float expFactor = pImpl->getExpMinus(dt);
                totalDelta += pImpl->ltpWeight * expFactor;
            }
            preIdx++;
        } else if (preTime > postTime) {
            // Post before pre: depression
            float dt = preTime - postTime;
            if (dt > 0.0f) {
                // Use lookup table for faster exponential calculation
                float expFactor = pImpl->getExpPlus(dt);
                totalDelta -= pImpl->ltdWeight * expFactor;
            }
            postIdx++;
        } else {
            // Simultaneous spikes: ignore (rare case)
            preIdx++;
            postIdx++;
        }
    }
    
    // Apply weight change with bounds
    if (std::abs(totalDelta) > 1e-6f) {
        // Scale by synaptic efficacy if available
        float efficacy = synapse->getEfficacy();
        totalDelta *= efficacy;
        
        // Apply weight change
        synapse->addToWeight(totalDelta);
        
        // Update eligibility trace for reward-modulated learning
        float currentTrace = synapse->getEligibilityTrace();
        synapse->setEligibilityTrace(currentTrace + totalDelta);
    }
}

void STDP::applyWeightChange(Synapse* synapse, SynapticWeight delta) {
    if (!synapse) return;
    
    // Clamp weight to bounds
    float newWeight = synapse->getWeight() + delta;
    newWeight = std::clamp(newWeight, pImpl->minWeight, pImpl->maxWeight);
    synapse->setWeight(newWeight);
}

const char* STDP::getName() const {
    return "STDP";
}

void STDP::setLTPWeight(float weight) {
    pImpl->ltpWeight = std::clamp(weight, 0.0f, 1.0f);
}

float STDP::getLTPWeight() const {
    return pImpl->ltpWeight;
}

void STDP::setLTDWeight(float weight) {
    pImpl->ltdWeight = std::clamp(weight, 0.0f, 1.0f);
}

float STDP::getLTDWeight() const {
    return pImpl->ltdWeight;
}

void STDP::setTimeConstant(float tau) {
    pImpl->timeConstant = std::clamp(tau, 1.0f, 100.0f);
    
    // Reinitialize lookup tables with new time constant
    float dt = 2.0f * pImpl->timeConstant / pImpl->expMinusTable.size();
    for (size_t i = 0; i < pImpl->expMinusTable.size(); ++i) {
        float x = static_cast<float>(i) * dt;
        pImpl->expMinusTable[i] = std::exp(-x / pImpl->timeConstant);
        pImpl->expPlusTable[i] = std::exp(x / pImpl->timeConstant);
    }
}

float STDP::getTimeConstant() const {
    return pImpl->timeConstant;
}

void STDP::configure(float ltpWeight, float ltdWeight, float tau) {
    setLTPWeight(ltpWeight);
    setLTDWeight(ltdWeight);
    setTimeConstant(tau);
}

} // namespace nlm