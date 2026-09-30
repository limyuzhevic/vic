#pragma once

#include "../core/Types/Types.hpp"
#include <vector>

namespace nlm {

class SpikeHandler {
public:
    SpikeHandler();
    
    // Spike recording
    void recordSpike(Timestamp timestamp);
    const std::vector<Timestamp>& getSpikeHistory() const;
    void clearSpikeHistory();
    
    // Get recent spike count
    size_t getSpikeCount() const { return spikeHistory_.size(); }
    
    // Check if neuron fired recently
    bool firedRecently(Timestamp currentTime, float maxAge) const;
    
    // Get last spike time
    float getLastSpikeTime() const;
    
    // Spike-based signaling for plasticity
    void recordPreSpikeForPlasticity(Timestamp timestamp);
    void recordPostSpikeForPlasticity(Timestamp timestamp);
    const std::vector<Timestamp>& getPreSpikeHistory() const { return preSpikeHistory_; }
    const std::vector<Timestamp>& getPostSpikeHistory() const { return postSpikeHistory_; }
    void clearPlasticitySpikeHistory();
    
private:
    static constexpr size_t MAX_SPIKE_HISTORY = 100;
    
    std::vector<Timestamp> spikeHistory_;           // For simple spike tracking
    std::vector<Timestamp> preSpikeHistory_;       // For STDP - pre-synaptic spikes
    std::vector<Timestamp> postSpikeHistory_;      // For STDP - post-synaptic spikes
};

} // namespace nlm