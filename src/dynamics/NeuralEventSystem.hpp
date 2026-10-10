// NeuralEventSystem.hpp - Event-driven neural dynamics integration
#pragma once

#include "../core/Types/Types.hpp"
#include "../brain/Neuron.hpp"
#include "../brain/Synapse.hpp"
#include "../dynamics/NeuralDynamics.hpp"
#include <memory>
#include <vector>
#include <functional>

namespace nlm {

// Forward declarations for event handlers from existing systems
struct DetailedSpikeEvent;
struct SpikeEvent;
struct DelayedSpikeEvent;
struct SpikeHandler;
struct DelayedSpikeHandler;

// NeuralEventSystem - Integrates neural dynamics with event-driven processing
class NeuralEventSystem {
public:
    NeuralEventSystem();
    ~NeuralEventSystem();
    
    // Disable copying
    NeuralEventSystem(const NeuralEventSystem&) = delete;
    NeuralEventSystem& operator=(const NeuralEventSystem&) = delete;
    
    // Register a neural dynamics instance with the event system
    void registerDynamics(std::unique_ptr<NeuralDynamics> dynamics);
    
    // Process events for neurons (time-driven)
    void processNeuronEvents(const std::vector<Neuron*>& neurons, 
                            SimulationStep step, Timestamp time);
    
    // Process synaptic events
    void processSynapticEvents(const std::vector<Synapse*>& synapses,
                               SimulationStep step, Timestamp time);
    
    // Get current dynamics instance
    const NeuralDynamics* getCurrentDynamics() const { return currentDynamics_.get(); }
    
    // Set current dynamics
    void setCurrentDynamics(std::unique_ptr<NeuralDynamics> dynamics) {
        currentDynamics_ = std::move(dynamics);
    }
    
    // Event handlers for neuromodulation and plasticity
    using NeuromodulationHandler = std::function<void(Neuron*, float, Timestamp)>;
    using PlasticityHandler = std::function<void(Synapse*, Neuron*, Neuron*, Timestamp, float)>;
    
    void registerNeuromodulationHandler(NeuromodulationHandler handler);
    void registerPlasticityHandler(PlasticityHandler handler);
    
    // Integration with existing spike system
    using SpikeHandler = std::function<void(const DetailedSpikeEvent&)>
    using DelayedSpikeHandler = std::function<void(const DelayedSpikeEvent&)>
    
    void registerSpikeHandler(SpikeHandler handler);
    void registerDelayedSpikeHandler(DelayedSpikeHandler handler);
    
    // Integration helper
    void integrateWithSpikeSystem(class SpikeSystem& spikeSystem);
    
    // Statistics
    uint64_t getEventsProcessed() const { return pImpl_->eventsProcessed; }
    uint64_t getNeuronsUpdated() const { return pImpl_->neuronsUpdated; }
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl_;
    std::unique_ptr<NeuralDynamics> currentDynamics_;
};

} // namespace nlm
