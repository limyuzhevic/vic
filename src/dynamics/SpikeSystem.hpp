#pragma once

#include "../performance/EventQueue.hpp"

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
// Handles spike event processing and delayed synaptic transmission
class SpikeSystem {
public:
    SpikeSystem();
    ~SpikeSystem();
    
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
    
    // Event queue for immediate spike processing
    using SpikeRingBuffer = ::nlm::SpikeRingBuffer;
    
    // Event queue for delayed spike processing
    using DelayedSpikeQueue = ::nlm::DelayedSpikeQueue;
    
    // Set event queue for immediate spikes
    void setEventQueue(std::unique_ptr<SpikeRingBuffer> queue);
    
    // Set event queue for delayed spikes
    void setDelayedQueue(std::unique_ptr<DelayedSpikeQueue> queue);
    
    // Configure for event-driven processing
    void configureForEventDrivenProcessing();
    
    // Get event queue for immediate spikes
    SpikeRingBuffer* getEventQueue() const;
    
    // Get event queue for delayed spikes
    DelayedSpikeQueue* getDelayedQueue() const;
};

} // namespace nlm
