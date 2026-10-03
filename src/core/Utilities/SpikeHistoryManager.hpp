// SpikeHistoryManager - Common spike history management for neural components
// Eliminates code duplication between Neuron and Synapse classes

#ifndef SPIKE_HISTORY_MANAGER_HPP
#define SPIKE_HISTORY_MANAGER_HPP

#include <vector>
#include <algorithm>
#include <cstddef>

namespace nlm {

using Timestamp = double;

class SpikeHistoryManager {
protected:
    std::vector<Timestamp> spikeHistory;
    static constexpr size_t MAX_SPIKE_HISTORY = 100;
    
public:
    SpikeHistoryManager() = default;
    
    // Record a new spike timestamp with automatic history management
    void recordSpike(Timestamp timestamp) {
        spikeHistory.push_back(timestamp);
        if (spikeHistory.size() > MAX_SPIKE_HISTORY) {
            spikeHistory.erase(spikeHistory.begin());
        }
    }
    
    // Clear all recorded spikes
    void clearHistory() {
        spikeHistory.clear();
    }
    
    // Get access to the spike history vector
    const std::vector<Timestamp>& getHistory() const {
        return spikeHistory;
    }
    
    // Get the number of recorded spikes
    size_t getHistorySize() const {
        return spikeHistory.size();
    }
    
    // Check if history is empty
    bool isEmpty() const {
        return spikeHistory.empty();
    }
};

} // namespace nlm

#endif // SPIKE_HISTORY_MANAGER_HPP
