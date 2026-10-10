#include "PlasticityRule.hpp"
#include <algorithm>
#include <cmath>
#include <memory>
#include <random>

namespace nlm {

// Hebbian plasticity rule implementation
struct HebbianRule::Impl {
    // Learning parameters based on biological models
    float learningRate;           // Base learning rate
    float synapticModulation;     // Neuromodulatory modulation factor
    float historySize;            // Size of spike history buffer
    float maxWeightChange;        // Maximum allowed weight change per timestep
    
    // Spike history for STDP
    struct SpikeHistory {
        std::vector<Timestamp> preSpikeTimes;
        std::vector<Timestamp> postSpikeTimes;
        std::deque<Timestamp> recentPre;   // Rolling window for efficiency
        std::deque<Timestamp> recentPost;
        
        SpikeHistory() : recentPre(10), recentPost(10) {}
    };
    
    SpikeHistory spikeHistory;
    
    Impl() : learningRate(0.01f), synapticModulation(1.0f), historySize(50.0f),
             maxWeightChange(0.1f) {}
};

HebbianRule::HebbianRule() : pImpl(new Impl) {}

HebbianRule::~HebbianRule() {
    delete pImpl;
}

void HebbianRule::update(Synapse* synapse,
                         const std::vector<Timestamp>& preSpikes,
                         const std::vector<Timestamp>& postSpikes,
                         TimestepDuration dt) {
    if (!synapse || !pImpl) return;
    if (!pImpl->isEnabled()) return;
    
    // Record new spikes
    pImpl->spikeHistory.preSpikeTimes.insert(pImpl->spikeHistory.preSpikeTimes.end(),
                                           preSpikes.begin(), preSpikes.end());
    pImpl->spikeHistory.postSpikeTimes.insert(pImpl->spikeHistory.postSpikeTimes.end(),
                                            postSpikes.begin(), postSpikes.end());
    
    // Keep only recent spikes to manage memory
    while (pImpl->spikeHistory.preSpikeTimes.size() > pImpl->historySize) {
        pImpl->spikeHistory.preSpikeTimes.erase(pImpl->spikeHistory.preSpikeTimes.begin());
    }
    while (pImpl->spikeHistory.postSpikeTimes.size() > pImpl->historySize) {
        pImpl->spikeHistory.postSpikeTimes.erase(pImpl->spikeHistory.postSpikeTimes.begin());
    }
    
    // Maintain rolling window for efficiency
    pImpl->spikeHistory.recentPre.push_back(0);
    pImpl->spikeHistory.recentPost.push_back(0);
    if (pImpl->spikeHistory.recentPre.size() > 10) {
        pImpl->spikeHistory.recentPre.pop_front();
        pImpl->spikeHistory.recentPost.pop_front();
    }
    
    // Compute synaptic weight changes using STDP
    computeSTDPWeightChanges(synapse, dt);
}

void HebbianRule::applyWeightChange(Synapse* synapse, SynapticWeight delta) {
    if (!synapse) return;
    
    // Clamp weight change to prevent runaway excitation/inhibition
    delta = std::clamp(delta, -pImpl->maxWeightChange, pImpl->maxWeightChange);
    
    // Apply weight change with biological constraints
    float currentWeight = synapse->getWeight();
    float newWeight = currentWeight + delta;
    
    // Enforce weight bounds
    float minWeight = synapse->getMinWeight();
    float maxWeight = synapse->getMaxWeight();
    
    newWeight = std::max(minWeight, std::min(maxWeight, newWeight));
    
    synapse->setWeight(newWeight);
}

const char* HebbianRule::getName() const {
    return "Hebbian";
}

void HebbianRule::setLearningRate(float rate) {
    if (pImpl) {
        pImpl->learningRate = std::max(0.0f, rate);
    }
}

float HebbianRule::getLearningRate() const {
    return pImpl ? pImpl->learningRate : 0.0f;
}

// Helper method to compute STDP weight changes
void HebbianRule::computeSTDPWeightChanges(Synapse* synapse, TimestepDuration dt) {
    if (!synapse || !pImpl) return;
    
    // Implement Spike-Timing-Dependent Plasticity (STDP)
    // Pre-before-post = Long-Term Potentiation (LTP)
    // Post-before-pre = Long-Term Depression (LTD)
    
    float totalDelta = 0.0f;
    float alpha_plus = pImpl->learningRate * 1.0f;  // LTP magnitude
    float alpha_minus = pImpl->learningRate * 0.8f; // LTD magnitude
    float tau_plus = 20.0f;   // LTP time constant (ms)
    float tau_minus = 20.0f;  // LTD time constant (ms)
    
    // Iterate through all pre-post spike pairs
    for (Timestamp preTime : pImpl->spikeHistory.preSpikeTimes) {
        for (Timestamp postTime : pImpl->spikeHistory.postSpikeTimes) {
            float timeDiff = postTime - preTime; // Post minus Pre
            
            if (timeDiff > 0) {
                // Pre before Post: LTP
                float eligibility = std::exp(-timeDiff / tau_plus);
                totalDelta += alpha_plus * eligibility;
            } else {
                // Post before Pre: LTD
                float eligibility = std::exp(timeDiff / tau_minus);
                totalDelta -= alpha_minus * eligibility;
            }
        }
    }
    
    // Apply neuromodulatory modulation
    totalDelta *= pImpl->synapticModulation;
    
    // Apply synaptic eligibility trace (for reward-modulated STDP)
    float eligibilityTrace = synapse->getEligibilityTrace();
    if (std::abs(eligibilityTrace) > 0.001f) {
        totalDelta += eligibilityTrace * pImpl->synapticModulation * 0.1f;
        synapse->decayEligibilityTrace(0.1f); // Decay eligibility trace
    }
    
    // Apply weight change
    if (totalDelta != 0.0f) {
        applyWeightChange(synapse, totalDelta);
    }
}

} // namespace nlm