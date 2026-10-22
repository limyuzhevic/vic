#pragma once

#include "../core/Types/Types.hpp"
#include "../brain/Neuron.hpp"
#include <vector>
#include <queue>
#include <functional>
#include <unordered_map>

namespace nlm {

// Enhanced SpikeEvent with additional metadata for better event tracking
struct DetailedSpikeEvent {
    NeuronId source;                    // Neuron that generated the spike
    Timestamp timestamp;               // Time when spike occurred
    SimulationStep step;               // Simulation step when spike occurred
    RegionId regionId;                  // Region ID for efficient lookup
    PopulationId populationId;          // Population ID for efficient lookup
    
    DetailedSpikeEvent() : source(), timestamp(0.0), step(0), regionId(), populationId() {}
    DetailedSpikeEvent(NeuronId nid, Timestamp ts, SimulationStep s, RegionId rid, PopulationId pid)
        : source(nid), timestamp(ts), step(s), regionId(rid), populationId(pid) {}
    
    // Convenience constructor for backward compatibility
    DetailedSpikeEvent(NeuronId nid, Timestamp ts, SimulationStep s)
        : source(nid), timestamp(ts), step(s), regionId(INVALID_REGION_ID), populationId(INVALID_POPULATION_ID) {}
};

// Event-driven neural computation system
// Handles spike event processing and delayed synaptic transmission with optimized performance
class SpikeSystem {
public:
    SpikeSystem();
    ~SpikeSystem();
    
    // Disable copying, enable moving (for PIMPL pattern)
    SpikeSystem(const SpikeSystem&) = delete;
    SpikeSystem& operator=(const SpikeSystem&) = delete;
    SpikeSystem(SpikeSystem&&) noexcept;
    SpikeSystem& operator=(SpikeSystem&&) noexcept;
    
    // Queue a spike event for processing
    void queueSpike(const SpikeEvent& event);
    
    // Convenience overload for direct spike event creation
    void queueSpike(NeuronId neuron, Timestamp timestamp, SimulationStep step);
    
    // Queue a delayed spike event (for synaptic transmission with delay)
    void queueDelayedSpike(const DelayedSpikeEvent& event);
    
    // Process all queued spikes for current simulation step
    void processSpikes(SimulationStep currentStep);
    
    // Process pending delayed spikes that should be delivered now
    void processDelayedSpikes(SimulationStep currentStep, Timestamp currentTime);
    
    // Register spike handler callback (called when spikes are generated)
    // Using type alias for better readability and maintainability
    using SpikeHandler = std::function<void(const DetailedSpikeEvent&)>;
    void registerHandler(SpikeHandler handler);
    
    // Register delayed spike handler (called when delayed spikes are delivered)
    using DelayedSpikeHandler = std::function<void(const DelayedSpikeEvent&)>;
    void registerDelayedHandler(DelayedSpikeHandler handler);
    
    // Get spike history for analysis and debugging
    const std::vector<DetailedSpikeEvent>& getSpikeHistory() const;
    void clearHistory();
    
    // Get spike statistics for monitoring
    size_t getSpikeCount() const;
    size_t getPendingSpikeCount() const;
    size_t getPendingDelayedCount() const;
    
    // Performance metrics
    float getAverageSpikeRate() const;
    std::vector<NeuronId> getMostActiveNeurons(size_t count) const;
    
    // Reset system to initial state
    void reset();
    
    // Validate system state (for debugging and testing)
    bool validate() const;
    
    // Get memory usage statistics
    size_t getMemoryUsage() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;  // Use smart pointer for better memory management
};

} // namespace nlm
