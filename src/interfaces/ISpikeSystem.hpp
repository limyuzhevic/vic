// ISpikeSystem.hpp
// Abstract interface for spike generation systems
// Manages neuronal spiking, spike events, and spike propagation
// Handles threshold detection and spike timing

#pragma once

#include "../core/Types/Types.hpp"
#include <vector>
#include <memory>
#include <functional>
#include <string>
#include <map>
#include <queue>

namespace nlm {

// Forward declarations
class Brain;
class Neuron;

// Spike event with metadata
struct SpikeEvent {
    NeuronId source_neuron;
    Timestamp timestamp;
    SimulationStep step;
    float synapticDrive;  // Drive level that caused spike
    bool isEmergent;      // True if spontaneous, false if driven
    std::vector<float> synapticWeights;  // Weights at time of spike
    
    SpikeEvent() : source_neuron(), timestamp(0.0), step(0), synapticDrive(0), isEmergent(false) {}
    SpikeEvent(NeuronId nid, Timestamp ts, SimulationStep s, float drive)
        : source_neuron(nid), timestamp(ts), step(s), synapticDrive(drive), isEmergent(false) {}
};

// Delayed spike event for synaptic transmission
struct DelayedSpikeEvent {
    NeuronId source_neuron;
    NeuronId destination_neuron;
    SynapseId synapse_id;
    SynapticWeight weight;  // Weight at time of spike (frozen)
    SynapseType synapse_type;
    Timestamp timestamp;      // When spike occurs
    Timestamp delivery_time;  // When spike reaches destination (timestamp + delay)
    SimulationStep step;      // Current simulation step
    SimulationStep delivery_step;  // Step when spike should be delivered
    bool is_excitatory;  // Cached for quick lookup
    
    DelayedSpikeEvent()
        : source_neuron(), destination_neuron(), synapse_id()
        , weight(0.0f), synapse_type(SynapseType::Excitatory)
        , timestamp(0.0), delivery_time(0.0), step(0), delivery_step(0)
        , is_excitatory(true) {}
    
    DelayedSpikeEvent(NeuronId src, NeuronId dst, SynapseId syn,
                      SynapticWeight w, SynapseType type,
                      Timestamp ts, Timestamp delivery, SimulationStep s, SimulationStep deliveryS)
        : source_neuron(src), destination_neuron(dst), synapse_id(syn)
        , weight(w), synapse_type(type)
        , timestamp(ts), delivery_time(delivery)
        , step(s), delivery_step(deliveryS)
        , is_excitatory(type == SynapseType::Excitatory) {}
};

// Spike statistics
struct SpikeStatistics {
    uint64_t totalSpikesGenerated;
    uint64_t spikesPerStep;
    float averageFiringRate;
    float averageSpikeInterval;
    uint64_t maxSpikesPerStep;
    
    SpikeStatistics() : totalSpikesGenerated(0), spikesPerStep(0), averageFiringRate(0),
                       averageSpikeInterval(0), maxSpikesPerStep(0) {}
};

class ISpikeSystem {
public:
    virtual ~ISpikeSystem() = default;

    // Initialize spike system with brain reference
    virtual bool initialize(Brain* brain) = 0;

    // Process neuron for spiking (called each timestep)
    virtual void processSpiking(Neuron* neuron, TimestepDuration dt) = 0;

    // Generate and manage spike events
    virtual void generateSpike(const SpikeEvent& spike) = 0;

    // Deliver delayed spike events
    virtual void deliverDelayedSpikes() = 0;

    // Get pending delayed spike events
    virtual std::vector<DelayedSpikeEvent> getPendingDelayedSpikes() const = 0;

    // Process spike events and update neurons
    virtual void processIncomingSpikes() = 0;

    // Get spike statistics
    virtual SpikeStatistics getStatistics() const = 0;

    // Get all generated spikes (for recording)
    virtual const std::vector<SpikeEvent>& getGeneratedSpikes() const = 0;
    virtual void clearGeneratedSpikes() = 0;

    // Set/Get spike threshold
    virtual float getSpikeThreshold() const = 0;
    virtual void setSpikeThreshold(float threshold) = 0;

    // Set/Get refractory period
    virtual uint32_t getRefractoryPeriod() const = 0;
    virtual void setRefractoryPeriod(uint32_t steps) = 0;

    // Update spike system
    virtual void update(TimestepDuration dt) = 0;

    // Reset spike system
    virtual void reset() = 0;

    // Get spike system statistics
    virtual std::string getSpikeSystemStats() const = 0;

    // Interface information
    virtual std::string getInterfaceName() const { return "ISpikeSystem"; }
    virtual std::string getVersion() const { return "1.0.0"; }

    // Error handling
    virtual bool hasError() const = 0;
    virtual std::string getErrorMessage() const = 0;
    virtual void clearError() = 0;

    // Callback pattern for spike events
    using SpikeGeneratedCallback = std::function<void(const SpikeEvent& spike)>;
    virtual void setSpikeGeneratedCallback(const SpikeGeneratedCallback& callback) = 0;

    using SpikeDeliveredCallback = std::function<void(const DelayedSpikeEvent& spike)>;
    virtual void setSpikeDeliveredCallback(const SpikeDeliveredCallback& callback) = 0;

    // Debug information
    virtual std::string getDebugInfo() const = 0;

    // Performance metrics
    virtual float getSpikeGenerationRate() const = 0;  // Spikes per second
    virtual float getAverageInterSpikeInterval() const = 0;
    virtual uint64_t getMaxConcurrentSpikes() const = 0;

    // Spike transmission parameters
    virtual float getAxonalTransmissionDelay() const = 0;
    virtual void setAxonalTransmissionDelay(float delay) = 0;
    virtual float getSynapticReleaseProbability() const = 0;
    virtual void setSynapticReleaseProbability(float prob) = 0;

protected:
    ISpikeSystem() = default;
};

} // namespace nlm
