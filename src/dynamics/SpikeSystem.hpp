#pragma once

#include "../core/Types/Types.hpp"
#include "../brain/Neuron.hpp"
#include <vector>
#include <queue>
#include <functional>
#include <unordered_map>
#include <atomic>
#include <deque>

namespace nlm {

// Optimized spike event structure for memory efficiency
struct AlignedSpikeEvent {
    alignas(64) NeuronId source;        // Source neuron ID
    alignas(64) Timestamp timestamp;   // Event timestamp
    alignas(64) SimulationStep step;   // Simulation step
    alignas(64) RegionId regionId;     // Region ID
    alignas(64) PopulationId populationId;  // Population ID
    alignas(64) bool isExcitatory;     // Synaptic type
    alignas(64) float weight;           // Synaptic weight
    
    AlignedSpikeEvent() : source(), timestamp(0.0), step(0), regionId(), populationId(), 
                         isExcitatory(true), weight(1.0f) {}
    
    AlignedSpikeEvent(NeuronId nid, Timestamp ts, SimulationStep s, RegionId rid, PopulationId pid,
                     bool excit, float w) : source(nid), timestamp(ts), step(s), 
                                            regionId(rid), populationId(pid), 
                                            isExcitatory(excit), weight(w) {}
};

// Enhanced DetailedSpikeEvent with additional metadata
struct DetailedSpikeEvent {
    NeuronId source;
    Timestamp timestamp;
    SimulationStep step;
    RegionId regionId;
    PopulationId populationId;
    
    DetailedSpikeEvent() : source(), timestamp(0.0), step(0), regionId(), populationId() {}
    DetailedSpikeEvent(NeuronId nid, Timestamp ts, SimulationStep s, RegionId rid, PopulationId pid)
        : source(nid), timestamp(ts), step(s), regionId(rid), populationId(pid) {}
};

// Priority queue for time-critical spike processing
struct SpikeEventPriorityComparator {
    bool operator()(const AlignedSpikeEvent& a, const AlignedSpikeEvent& b) {
        // Higher priority for earlier events (smaller timestamp)
        return a.timestamp > b.timestamp;
    }
};

// Event-driven neural computation system
// Handles spike event processing and delayed synaptic transmission with optimizations
class SpikeSystem {
public:
    // Memory pool configuration
    static constexpr size_t SPIKE_EVENT_POOL_SIZE = 1024;
    static constexpr size_t DELAYED_SPIKE_POOL_SIZE = 512;
    
    SpikeSystem();
    ~SpikeSystem();
    
    // Disable copying, enable moving
    SpikeSystem(const SpikeSystem&) = delete;
    SpikeSystem& operator=(const SpikeSystem&) = delete;
    SpikeSystem(SpikeSystem&&) noexcept;
    SpikeSystem& operator=(SpikeSystem&&) noexcept;
    
    // Queue a spike event with metadata
    void queueSpike(const SpikeEvent& event);
    void queueSpike(NeuronId neuron, Timestamp timestamp, SimulationStep step, 
                   RegionId region, PopulationId population, bool excitatory, float weight);
    
    // Queue a delayed spike event (for synaptic transmission)
    void queueDelayedSpike(const DelayedSpikeEvent& event);
    
    // Process all queued spikes for current step with optimized event handling
    void processSpikes(SimulationStep currentStep);
    
    // Process pending delayed spikes that should be delivered now
    void processDelayedSpikes(SimulationStep currentStep, Timestamp currentTime);
    
    // Register spike handler callback (called when spikes are generated)
    using SpikeHandler = std::function<void(const DetailedSpikeEvent&)>;
    void registerHandler(SpikeHandler handler);
    
    // Register delayed spike handler (called when delayed spikes are delivered)
    using DelayedSpikeHandler = std::function<void(const DelayedSpikeEvent&)>;
    void registerDelayedHandler(DelayedSpikeHandler handler);
    
    // Get spike history with circular buffer optimization
    const std::vector<DetailedSpikeEvent>& getSpikeHistory() const;
    void clearHistory();
    
    // Get spike count
    size_t getSpikeCount() const;
    size_t getPendingSpikeCount() const;
    size_t getPendingDelayedCount() const;
    
    // Statistics with incremental updates
    float getAverageSpikeRate() const;
    std::vector<NeuronId> getMostActiveNeurons(size_t count) const;
    
    // Reset system
    void reset();
    
    // Performance optimization: enable/disable pooling
    void enableMemoryPooling(bool enabled) { pImpl->useMemoryPools = enabled; }
    bool isMemoryPoolingEnabled() const { return pImpl->useMemoryPools; }
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
