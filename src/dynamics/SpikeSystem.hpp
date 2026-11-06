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
    
    // Get spike history
    const std::vector<DetailedSpikeEvent>& getSpikeHistory() const;
    void clearHistory();
    
    // Get spike count
    size_t getSpikeCount() const;
    size_t getPendingSpikeCount() const;
    size_t getPendingDelayedCount() const;

    // Phase 4: pending delayed spikes are in-flight synaptic input —
    // without them, a checkpoint reload loses every spike scheduled
    // for delivery in the next 1-5 steps and cannot continue the saved
    // trajectory. Expose them for checkpointing (flat list; delivery
    // step grouping is restored by restorePendingDelayedSpikes).
    std::vector<DelayedSpikeEvent> getPendingDelayedSpikes() const;
    void restorePendingDelayedSpikes(const std::vector<DelayedSpikeEvent>& events);
    
    // Statistics
    float getAverageSpikeRate() const;
    std::vector<NeuronId> getMostActiveNeurons(size_t count) const;
    
    // Clear pending spikes and history
    void clear();

    // Get detailed spike information
    const DetailedSpikeEvent* getLastSpike() const;
    
    // Check if spike system is empty
    bool isEmpty() const;
    bool hasPendingSpikes() const;
    bool hasPendingDelayedSpikes() const;
    
    // Configuration
    void setMaxHistorySize(size_t size);
    size_t getMaxHistorySize() const;
    
    // Performance optimization
    void optimizeMemory();
    
    // Statistics enhancement
    float getStandardDeviationSpikeRate() const;
    float getMinSpikeRate() const;
    float getMaxSpikeRate() const;
    
    // Advanced event handling
    void setEventFilter(std::function<bool(const DetailedSpikeEvent&)> filter);
    void removeEventFilter();
    
    // Batch processing
    void processSpikeBatch(const std::vector<SpikeEvent>& batch);
    void processDelayedSpikeBatch(const std::vector<DelayedSpikeEvent>& batch);
    
    // Priority queue support
    void enablePriorityQueue(bool enable);
    bool isPriorityQueueEnabled() const;
    
    // Statistics caching
    void enableStatisticsCaching(bool enable);
    bool isStatisticsCachingEnabled() const;
    
    // Validation and error handling
    bool validateEvent(const SpikeEvent& event) const;
    bool validateDelayedEvent(const DelayedSpikeEvent& event) const;
    std::vector<std::string> getEventValidationErrors() const;
    
    // Configuration and monitoring
    void setLogLevel(LogLevel level);
    LogLevel getLogLevel() const;
    
    // Performance monitoring
    double getAverageProcessingTime() const;
    double getMemoryUsage() const;
    
    // Health check
    bool healthCheck() const;
    std::string getHealthStatus() const;
