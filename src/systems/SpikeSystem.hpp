#pragma once

#include "BrainSystemBase.hpp"
#include "brain/Neuron.hpp"
#include "brain/Synapse.hpp"
#include "brain/Brain.hpp"
#include "core/Types/Types.hpp"
#include <vector>
#include <queue>
#include <functional>
#include <unordered_map>
#include <algorithm>
#include <memory>

namespace nlm {

// Forward declarations for event types
class SpikeEvent;
class DelayedSpikeEvent;
class DetailedSpikeEvent;

/**
 * @class SpikeSystem
 * @brief System for managing neural spiking and spike propagation
 * 
 * The SpikeSystem handles event-driven spike propagation throughout the brain.
 * It manages:
 * - Spike event queuing and processing
 * - Delayed spike delivery (synaptic transmission)
 * - Spike statistics and monitoring
 * - Event-driven processing for efficient neural simulation
 */
class SpikeSystem : public BrainSystemBase<SpikeSystem> {
public:
    SpikeSystem(Brain* brain);
    ~SpikeSystem() override;
    
    const char* getNameImpl() const override { return "SpikeSystem"; }
    
    // BrainSystemBase interface
    void doInitialize() override;
    void doShutdown() override;
    void doStep(SimulationStep step, Timestamp time) override;
    bool doConfigure(const std::shared_ptr<Config>& config) override;
    void doSetBrainReference(Brain* brain) override;
    
    // Event management
    void queueSpike(const SpikeEvent& event);
    void queueDelayedSpike(const DelayedSpikeEvent& event);
    void processSpikes(SimulationStep currentStep);
    void processDelayedSpikes(SimulationStep currentStep, Timestamp currentTime);
    
    // Event handlers for system integration
    using SpikeHandler = std::function<void(const DetailedSpikeEvent&)>;
    using DelayedSpikeHandler = std::function<void(const DelayedSpikeEvent&)>;
    
    void registerHandler(SpikeHandler handler);
    void registerDelayedHandler(DelayedSpikeHandler handler);
    
    // Statistics and monitoring
    size_t getSpikeCount() const;
    float getAverageSpikeRate() const;
    std::vector<NeuronId> getMostActiveNeurons(size_t count) const;
    
    // System state
    void reset() override;
    
    // Event statistics
    size_t getTotalSpikeEvents() const { return totalSpikeEvents_; }
    size_t getTotalDelayedSpikeEvents() const { return totalDelayedSpikeEvents_; }
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Helper methods for event processing
    void deliverSynapticInput(const DelayedSpikeEvent& event);
    void updateSpikeStatistics(const DetailedSpikeEvent& event);
    
    // Brain reference (already available via base class)
    Brain* getBrain() { return static_cast<Brain*>(getBrain()); }
    const Brain* getBrain() const { return static_cast<const Brain*>(getBrain()); }
};

} // namespace nlm
