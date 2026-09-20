#pragma once

#include "STDP.hpp"
#include "../core/Logger/Logger.hpp"
#include <cmath>

namespace nlm {

struct STDP::Impl {
    float ltpWeight;  // Long-term potentiation weight
    float ltdWeight;  // Long-term depression weight
    float tauPlus;    // Time constant for potentiation (ms)
    float tauMinus;   // Time constant for depression (ms)
    float maxWeight;  // Maximum synaptic weight
    float minWeight;  // Minimum synaptic weight
    
    // Pre-synaptic spike history
    std::vector<Timestamp> preSpikeTimes;
    std::vector<float> preTrace;
    
    // Post-synaptic spike history
    std::vector<Timestamp> postSpikeTimes;
    std::vector<float> postTrace;
    
    Impl() : ltpWeight(0.01f), ltdWeight(0.012f), tauPlus(20.0f), tauMinus(20.0f),
             maxWeight(1.0f), minWeight(0.0f) {}
};

STDP::STDP() : pImpl(new Impl) {}

STDP::~STDP() = default;

void STDP::configure(float ltpWeight, float ltdWeight, float tau) {
    pImpl->ltpWeight = ltpWeight;
    pImpl->ltdWeight = ltdWeight;
    pImpl->tauPlus = tau;
    pImpl->tauMinus = tau;
}

void STDP::onPreSynapticSpike(Timestamp time) {
    // Clean up old spikes
    cleanSpikes(pImpl->preSpikeTimes, time);
    
    // Add new spike
    pImpl->preSpikeTimes.push_back(time);
    
    // Update post-synaptic trace
    for (auto& trace : pImpl->postTrace) {
        trace *= std::exp(-time / pImpl->tauMinus);
    }
}

void STDP::onPostSynapticSpike(Timestamp time) {
    // Clean up old spikes
    cleanSpikes(pImpl->postSpikeTimes, time);
    
    // Add new spike
    pImpl->postSpikeTimes.push_back(time);
    
    // Update pre-synaptic trace
    for (auto& trace : pImpl->preTrace) {
        trace *= std::exp(-time / pImpl->tauPlus);
    }
}

void STDP::applyPlasticity(float& weight, Timestamp preTime, Timestamp postTime) {
    if (preTime == 0.0f || postTime == 0.0f) {
        return;  // No spike timing information
    }
    
    float dt = postTime - preTime;
    
    if (dt > 0) {
        // Pre before post: Long-Term Potentiation (LTP)
        float dw = pImpl->ltpWeight * std::exp(-dt / pImpl->tauPlus);
        weight += dw;
        NLM_LOG_DEBUG("STDP LTP: dt = " + std::to_string(dt) + 
                     " delta = " + std::to_string(dw));
    } else {
        // Post before pre: Long-Term Depression (LTD)
        float dw = -pImpl->ltdWeight * std::exp(dt / pImpl->tauMinus);
        weight += dw;
        NLM_LOG_DEBUG("STDP LTD: dt = " + std::to_string(dt) + 
                     " delta = " + std::to_string(dw));
    }
    
    // Clamp weight to valid range
    weight = std::clamp(weight, pImpl->minWeight, pImpl->maxWeight);
}

void STDP::cleanSpikes(std::vector<Timestamp>& times, Timestamp currentTime) {
    auto it = std::remove_if(times.begin(), times.end(),
        [currentTime](Timestamp t) { return currentTime - t > 1000.0f; }); // Keep 1s of history
    times.erase(it, times.end());
}

} // namespace nlm