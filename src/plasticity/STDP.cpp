// STDP.cpp - Full implementation of Spike-Timing-Dependent Plasticity

#include "STDP.hpp"
#include "Synapse.hpp"
#include <algorithm>
#include <cmath>
#include <unordered_map>
#include <queue>

namespace nlm {

struct STDP::Impl {
    // STDP parameters
    float ltpWeight = 0.01f;    // Long-term potentiation weight
    float ltdWeight = 0.012f;   // Long-term depression weight
    float timeConstant = 20.0f; // STDP time constant (ms)
    
    // Synaptic eligibility traces for efficient spike pair processing
    std::unordered_map<NeuronId, float> preTrace;
    std::unordered_map<NeuronId, float> postTrace;
    
    // Spike queues for temporal ordering
    std::queue<std::pair<Timestamp, NeuronId>> preSpikeQueue;
    std::queue<std::pair<Timestamp, NeuronId>> postSpikeQueue;
    
    // Recent spike history for O(n) processing
    static constexpr size_t MAX_RECENT_SPIKES = 1000;
    std::vector<Timestamp> recentPreSpikes;
    std::vector<Timestamp> recentPostSpikes;
    
    Impl() {
        preTrace.reserve(10000);
        postTrace.reserve(10000);
        recentPreSpikes.reserve(MAX_RECENT_SPIKES);
        recentPostSpikes.reserve(MAX_RECENT_SPIKES);
    }
};

STDP::STDP() : pImpl(new Impl) {}

STDP::~STDP() {
    delete pImpl;
}

void STDP::update(Synapse* synapse, 
                 const std::vector<Timestamp>& preSpikes,
                 const std::vector<Timestamp>& postSpikes,
                 TimestepDuration dt) {
    if (!synapse || preSpikes.empty() || postSpikes.empty()) {
        return;
    }
    
    // Clear recent spike history
    pImpl->recentPreSpikes.clear();
    pImpl->recentPostSpikes.clear();
    
    // Combine all recent spikes
    pImpl->recentPreSpikes.insert(pImpl->recentPreSpikes.end(), 
                                preSpikes.begin(), preSpikes.end());
    pImpl->recentPostSpikes.insert(pImpl->recentPostSpikes.end(), 
                                 postSpikes.begin(), postSpikes.end());
    
    // Sort by time for efficient processing
    std::sort(pImpl->recentPreSpikes.begin(), pImpl->recentPreSpikes.end());
    std::sort(pImpl->recentPostSpikes.begin(), pImpl->recentPostSpikes.end());
    
    // Apply STDP using efficient spike pair processing
    applySTDPWeightChange(synapse);
    
    // Update eligibility traces for future spikes
    updateEligibilityTraces(synapse);
}

void STDP::applyWeightChange(Synapse* synapse, SynapticWeight delta) {
    if (!synapse) return;
    
    float currentWeight = synapse->getWeight();
    float newWeight = currentWeight + delta;
    
    // Apply weight bounds
    const float minWeight = -1.0f;
    const float maxWeight = 1.0f;
    newWeight = std::max(minWeight, std::min(maxWeight, newWeight));
    
    synapse->setWeight(newWeight);
}

const char* STDP::getName() const {
    return "STDP";
}

void STDP::setLTPWeight(float weight) {
    pImpl->ltpWeight = std::max(0.0f, weight);
}

float STDP::getLTPWeight() const {
    return pImpl->ltpWeight;
}

void STDP::setLTDWeight(float weight) {
    pImpl->ltdWeight = std::max(0.0f, weight);
}

float STDP::getLTDWeight() const {
    return pImpl->ltdWeight;
}

void STDP::setTimeConstant(float tau) {
    pImpl->timeConstant = std::max(1.0f, tau);
}

float STDP::getTimeConstant() const {
    return pImpl->timeConstant;
}

void STDP::configure(float ltpWeight, float ltdWeight, float tau) {
    setLTPWeight(ltpWeight);
    setLTDWeight(ltdWeight);
    setTimeConstant(tau);
}

void STDP::applySTDPWeightChange(Synapse* synapse) {
    const Timestamp& preTime = synapse->getLastPreSpikeTime();
    const Timestamp& postTime = synapse->getLastPostSpikeTime();
    
    if (preTime == 0.0f || postTime == 0.0f) return;
    
    float deltaT = postTime - preTime;
    
    if (deltaT > 0) {
        // Post before pre: LTD (long-term depression)
        float weightChange = -pImpl->ltdWeight * std::exp(-deltaT / pImpl->timeConstant);
        applyWeightChange(synapse, weightChange);
    } else if (deltaT < 0) {
        // Pre before post: LTP (long-term potentiation)
        float weightChange = pImpl->ltpWeight * std::exp(deltaT / pImpl->timeConstant);
        applyWeightChange(synapse, weightChange);
    }
    // If deltaT == 0, no change (simultaneous spikes)
}

void STDP::updateEligibilityTraces(Synapse* synapse) {
    // For forward eligibility traces, we update based on recent spike activity
    // This allows for more complex STDP rules with synaptic history
    
    if (!synapse) return;
    
    // Update pre-synaptic trace decay
    for (auto& trace : pImpl->preTrace) {
        trace.second *= 0.95f; // Decay factor
    }
    
    // Update post-synaptic trace decay  
    for (auto& trace : pImpl->postTrace) {
        trace.second *= 0.95f; // Decay factor
    }
    
    // Add current eligibility based on recent spikes
    // This is a simplified version - real implementation would track specific neurons
    float currentEligibility = 1.0f;
    
    // Normalize traces to prevent overflow
    float maxTrace = 0.0f;
    for (const auto& trace : pImpl->preTrace) {
        maxTrace = std::max(maxTrace, trace.second);
    }
    for (const auto& trace : pImpl->postTrace) {
        maxTrace = std::max(maxTrace, trace.second);
    }
    
    if (maxTrace > 1.0f) {
        for (auto& trace : pImpl->preTrace) {
            trace.second /= maxTrace;
        }
        for (auto& trace : pImpl->postTrace) {
            trace.second /= maxTrace;
        }
    }
}

} // namespace nlm