#pragma once

#include "OptimizedSpikeSystem.hpp"
#include <vector>
#include <unordered_map>
#include <algorithm>
#include <memory>
#include <mutex>
#include <chrono>

namespace nlm {

class OptimizedSpikeSystem {
public:
    struct SpikeEvent {
        NeuronId source;
        Timestamp timestamp;
        SimulationStep step;
        bool processed;
        
        SpikeEvent() : source(), timestamp(0.0), step(0), processed(false) {}
        SpikeEvent(NeuronId src, Timestamp ts, SimulationStep s)
            : source(src), timestamp(ts), step(s), processed(false) {}
    };

    struct DelayedSpikeEvent {
        NeuronId source;
        NeuronId destination;
        SynapseId synapseId;
        float weight;
        bool isExcitatory;
        Timestamp spikeTime;
        Timestamp deliveryTime;
        SimulationStep spikeStep;
        SimulationStep deliveryStep;
        bool processed;
        
        DelayedSpikeEvent() : processed(false) {}
    };

    OptimizedSpikeSystem();
    ~OptimizedSpikeSystem();

    // Disable copying, enable moving
    OptimizedSpikeSystem(const OptimizedSpikeSystem&) = delete;
    OptimizedSpikeSystem& operator=(const OptimizedSpikeSystem&) = delete;
    OptimizedSpikeSystem(OptimizedSpikeSystem&&) noexcept;
    OptimizedSpikeSystem& operator=(OptimizedSpikeSystem&&) noexcept;

    // Event management
    void queueSpike(const SpikeEvent& event);
    void queueSpike(NeuronId neuron, Timestamp timestamp, SimulationStep step);
    void queueDelayedSpike(const DelayedSpikeEvent& event);

    // Processing
    void processSpikes(SimulationStep currentStep);
    void processDelayedSpikes(SimulationStep currentStep, Timestamp currentTime);
    void batchProcessSpikes(const std::vector<SpikeEvent>& events);

    // Integration with spike index
    void setSynapticIndex(SynapticIndex* index);
    void setConnectionGraph(ConnectionGraph* graph);

    // Callbacks
    using SpikeHandler = std::function<void(const DetailedSpikeEvent&)>;
    using DelayedSpikeHandler = std::function<void(const DelayedSpikeEvent&)>;
    
    void registerHandler(SpikeHandler handler);
    void registerDelayedHandler(DelayedSpikeHandler handler);

    // Statistics
    size_t getPendingSpikeCount() const;
    size_t getPendingDelayedCount() const;
    float getAverageSpikeRate() const;
    std::string getStatistics() const;

    // Optimization
    void enableLazyEvaluation(bool enable);
    void setProcessingBatchSize(size_t size);
    void enableVectorizedDelivery(bool enable);

    // Reset
    void reset();

private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
