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
    
    // Register delayed spike handler (called when delayed spikes are delivered)
    using DelayedSpikeHandler = std::function<void(const DelayedSpikeEvent&)>;
    void registerDelayedHandler(DelayedSpikeHandler handler);
    
    // Set function to lookup neurons by ID for spike delivery
    // This is needed for delayed spike handlers to deliver synaptic input
    using NeuronLookupFunc = std::function<Neuron*(const NeuronId&)>;
    void setNeuronLookupFunc(NeuronLookupFunc lookupFunc);
    
    // Set maximum number of delayed spikes to store
    void setMaxDelayedSpikes(size_t maxDelayedSpikes);
    
    // Get delivery statistics
    struct DeliveryStats {
        uint64_t totalDelivered;
        uint64_t totalDropped;
        float deliveryEfficiency;
        float averageDeliveryTime;
        uint64_t spikesProcessed;
        uint64_t spikesDropped;
    };
    DeliveryStats getDeliveryStats() const;
    
    // Get current delivery efficiency (0.0 to 1.0)
    float getDeliveryEfficiency() const;
    
    // Get average time for spike delivery (in seconds)
    float getAverageDeliveryTime() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
