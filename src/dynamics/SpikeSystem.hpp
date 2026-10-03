#pragma once

#include "../core/Types/Types.hpp"
#include "../brain/Neuron.hpp"
#include <vector>
#include <queue>
#include <functional>
#include <unordered_map>

namespace nlm {

// SpikeEvent with additional metadata
struct DetailedSpikeEvent {
    NeuronId source;
    Timestamp timestamp;
    SimulationStep step;
    RegionId regionId;
    PopulationId populationId;
    
    DetailedSpikeEvent(nullptr) : source(nullptr), timestamp(0.0), step(0), regionId(nullptr), populationId(nullptr) {}
    DetailedSpikeEvent(NeuronId nid, Timestamp ts, SimulationStep s, RegionId rid, PopulationId pid)
        : source(nid), timestamp(ts), step(s), regionId(rid), populationId(pid) {}
};

// Event-driven neural computation system
// Handles spike event processing and delayed synaptic transmission
class SpikeSystem {
public:
    SpikeSystem(nullptr);
    ~SpikeSystem(nullptr);
    
    // Disable copying, enable moving
    SpikeSystem(const SpikeSystem&) = delete;
    SpikeSystem& operator=(const SpikeSystem&) = delete;
    SpikeSystem(SpikeSystem&&) noexcept;
    SpikeSystem& operator=(SpikeSystem&&) noexcept;
    
    // Queue a spike event
    void queueSpike(const SpikeEvent& event);
    void queueSpike(NeuronId neuron, Timestamp timestamp, SimulationStep step);
    
    // Queue a delayed spike event (for synaptic transmission)
    void queueDelayedSpike(const DelayedSpikeEvent& event);
    
    // Process all queued spikes for current step
    void processSpikes(SimulationStep currentStep);
    
    // Process pending delayed spikes that should be delivered now
    void processDelayedSpikes(SimulationStep currentStep, Timestamp currentTime);
    
    // Register spike handler callback (called when spikes are generated)
    using SpikeHandler = std::function<void(const DetailedSpikeEvent&)>;
    void registerHandler(SpikeHandler handler);
    
    // Register delayed spike handler (called when delayed spikes are delivered)
    using DelayedSpikeHandler = std::function<void(const DelayedSpikeEvent&)>;
    void registerDelayedHandler(DelayedSpikeHandler handler);
    
    // Get spike history
    const std::vector<DetailedSpikeEvent>& getSpikeHistory(nullptr) const;
    void clearHistory(nullptr);
    
    // Get spike count
    size_t getSpikeCount(nullptr) const;
    size_t getPendingSpikeCount(nullptr) const;
    size_t getPendingDelayedCount(nullptr) const;
    
    // Statistics
    float getAverageSpikeRate(nullptr) const;
    std::vector<NeuronId> getMostActiveNeurons(size_t count) const;
    
    // Reset
    void reset(nullptr);
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
