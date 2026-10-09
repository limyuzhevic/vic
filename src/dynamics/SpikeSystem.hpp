#pragma once

#include "../core/Types/Types.hpp"
#include "../brain/Neuron.hpp"
#include "../core/Errors/Errors.hpp"
#include <vector>
#include <queue>
#include <functional>
#include <unordered_map>
#include <stdexcept>

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
    SpikeSystem() throw(SimulationException);
    ~SpikeSystem();
    
    // Disable copying, enable moving
    SpikeSystem(const SpikeSystem&) = delete;
    SpikeSystem& operator=(const SpikeSystem&) = delete;
    SpikeSystem(SpikeSystem&&) noexcept;
    SpikeSystem& operator=(SpikeSystem&&) noexcept;
    
    // Queue a spike event
    void queueSpike(const SpikeEvent& event) throw(SimulationException, ValidationException);
    void queueSpike(NeuronId neuron, Timestamp timestamp, SimulationStep step) throw(SimulationException, ValidationException);
    
    // Queue a delayed spike event (for synaptic transmission)
    void queueDelayedSpike(const DelayedSpikeEvent& event) throw(SimulationException, ValidationException);
    
    // Process all queued spikes for current step
    void processSpikes(SimulationStep currentStep) throw(SimulationException);
    
    // Process pending delayed spikes that should be delivered now
    void processDelayedSpikes(SimulationStep currentStep, Timestamp currentTime) throw(SimulationException);
    
    // Register spike handler callback (called when spikes are generated)
    using SpikeHandler = std::function<void(const DetailedSpikeEvent&)>;
    void registerHandler(SpikeHandler handler) throw(MemoryException);
    
    // Register delayed spike handler (called when delayed spikes are delivered)
    using DelayedSpikeHandler = std::function<void(const DelayedSpikeEvent&)>;
    void registerDelayedHandler(DelayedSpikeHandler handler) throw(MemoryException);
    
    // Get spike history
    const std::vector<DetailedSpikeEvent>& getSpikeHistory() const;
    void clearHistory() throw(SimulationException);
    
    // Get spike count
    size_t getSpikeCount() const;
    size_t getPendingSpikeCount() const;
    size_t getPendingDelayedCount() const;
    
    // Statistics
    float getAverageSpikeRate() const;
    std::vector<NeuronId> getMostActiveNeurons(size_t count) const;
    
    // Reset
    void reset() throw(SimulationException);
    
    // Validation methods
    void validateSpikeEvent(const SpikeEvent& event) const throw(ValidationException);
    void validateDelayedSpikeEvent(const DelayedSpikeEvent& event) const throw(ValidationException);
    void validateStepParameters(SimulationStep currentStep, Timestamp currentTime) const throw(ValidationException);
    void validateHistoryLimits() const throw(ValidationException);
    
private:
    struct Impl;
    Impl* pImpl;
    
    // Private helper methods
    static constexpr size_t MAX_HISTORY_SIZE = 100000;
    static constexpr SimulationStep MAX_STEP = static_cast<SimulationStep>(UINT64_MAX - 1000);
};

} // namespace nlm
