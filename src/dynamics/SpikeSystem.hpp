#pragma once

#include "../core/Types/Types.hpp"
#include "../brain/Neuron.hpp"
#include <vector>
#include <functional>
#include <memory>

namespace nlm {

// Detailed spike event for complete spike tracking with metadata
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

// Event-driven neural computation system
// Handles spike event processing and delayed synaptic transmission
// 
// Key Design Changes:
// - Spikes are processed immediately when queued (true event-driven behavior)
// - No synchronous batching or queues for immediate spike processing
// - Delayed spikes use priority queue for exact scheduled delivery times
// - Each spike triggers its handlers individually at the moment of occurrence
// 
// Benefits:
// - Biologically realistic timing (spikes processed immediately)
// - Higher performance (no batch processing overhead)
// - Deterministic spike delivery for delayed events
// - Reduced memory footprint (no intermediate queues)
class SpikeSystem {
public:
    SpikeSystem();
    ~SpikeSystem();
    
    // Disable copying, enable moving
    SpikeSystem(const SpikeSystem&) = delete;
    SpikeSystem& operator=(const SpikeSystem&) = delete;
    SpikeSystem(SpikeSystem&&) noexcept;
    SpikeSystem& operator=(SpikeSystem&&) noexcept;
    
    // Queue a spike event - now processes immediately for true event-driven behavior
    // This method processes the spike synchronously at the moment of queuing
    void queueSpike(const SpikeEvent& event);
    
    // Convenience overload for creating and queuing a spike
    void queueSpike(NeuronId neuron, Timestamp timestamp, SimulationStep step);
    
    // Queue a delayed spike event (for synaptic transmission)
    // Spikes are stored in a priority queue ordered by delivery_time
    void queueDelayedSpike(const DelayedSpikeEvent& event);
    
    // Process pending delayed spikes that should be delivered now
    // Scans the priority queue and delivers all spikes due at or before currentTime
    void processDelayedSpikes(SimulationStep currentStep, Timestamp currentTime);
    
    // Register spike handler callback (called when spikes are generated)
    using SpikeHandler = std::function<void(const DetailedSpikeEvent&)>;
    void registerHandler(SpikeHandler handler);
    
    // Register delayed spike handler (called when delayed spikes are delivered)
    using DelayedSpikeHandler = std::function<void(const DelayedSpikeEvent&)>;
    void registerDelayedHandler(DelayedSpikeHandler handler);
    
    // Get spike history
    const std::vector<DetailedSpikeEvent>& getSpikeHistory() const;
    void clearHistory();
    
    // Get spike count
    size_t getSpikeCount() const;
    
    // Statistics
    float getAverageSpikeRate() const;
    std::vector<NeuronId> getMostActiveNeurons(size_t count) const;
    
    // Reset
    void reset();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
