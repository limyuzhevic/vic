#pragma once

#ifndef NLM_SPIKE_SYSTEM_HPP
#define NLM_SPIKE_SYSTEM_HPP

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
    
    DetailedSpikeEvent() : source(), timestamp(0.0), step(0), regionId(), populationId() {}
    DetailedSpikeEvent(NeuronId nid, Timestamp ts, SimulationStep s, RegionId rid, PopulationId pid)
        : source(nid), timestamp(ts), step(s), regionId(rid), populationId(pid) {}
};

// Event-driven neural computation system
// Handles spike event processing and delayed synaptic transmission with efficient queue management
// Provides high-performance spike delivery for large-scale neural simulations
class SpikeSystem {
public:
    SpikeSystem();
    ~SpikeSystem();
    
    // Disable copying, enable moving
    SpikeSystem(const SpikeSystem&) = delete;
    SpikeSystem& operator=(const SpikeSystem&) = delete;
    SpikeSystem(SpikeSystem&&) noexcept;
    SpikeSystem& operator=(SpikeSystem&&) noexcept;
    
    // Queue a spike event for immediate processing
    void queueSpike(const SpikeEvent& event);
    void queueSpike(NeuronId neuron, Timestamp timestamp, SimulationStep step);
    
    // Queue a delayed spike event for synaptic transmission
    // Handles transmission delays in neural connections
    void queueDelayedSpike(const DelayedSpikeEvent& event);
    
    // Process all queued spikes for current simulation step
    // Delivers spikes to their targets and updates neural state
    void processSpikes(SimulationStep currentStep);
    
    // Process pending delayed spikes that should be delivered now
    // Synchronizes delayed spike delivery with current simulation time
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
    size_t getPendingSpikeCount() const;
    size_t getPendingDelayedCount() const;
    
    // Statistics
    float getAverageSpikeRate() const;
    std::vector<NeuronId> getMostActiveNeurons(size_t count) const;
    
    // Reset system state
    void reset();
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
