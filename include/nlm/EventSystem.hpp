#pragma once

#include "CommonBaseClasses.hpp"
#include <string>
#include <unordered_map>
#include <memory>

namespace nlm {

/**
 * Event system for the Observer pattern
 * Manages events and notifies observers
 */
class EventSystem {
public:
    using EventCallback = std::function<void(const std::string&, const std::string&)>;
    
    // Emit an event
    void emit(const std::string& event, const std::string& data = "");
    
    // Subscribe to an event
    void subscribe(const std::string& event, EventCallback callback);
    
    // Unsubscribe from an event
    void unsubscribe(const std::string& event, EventCallback callback);
    
    // Clear all subscribers for an event
    void clear(const std::string& event);
    
    // Check if there are subscribers for an event
    bool hasSubscribers(const std::string& event) const;
    
    // Get all events
    std::vector<std::string> getEvents() const;

private:
    std::unordered_map<std::string, std::vector<EventCallback>> subscriptions;
};

/**
 * Brain event system for brain state changes
 */
class BrainEventSystem : public EventSystem {
public:
    // Brain state events
    static constexpr const char* BRAIN_CREATED = "BrainCreated";
    static constexpr const char* BRAIN_DESTROYED = "BrainDestroyed";
    static constexpr const char* BRAIN_STATE_CHANGED = "BrainStateChanged";
    static constexpr const char* BRAIN_PLACEMENT_CHANGED = "BrainPlacementChanged";
    
    // Neuron events
    static constexpr const char* NEURON_CREATED = "NeuronCreated";
    static constexpr const char* NEURON_DESTROYED = "NeuronDestroyed";
    static constexpr const char* NEURON_SPIKED = "NeuronSpiked";
    static constexpr const char* NEURON_STATE_CHANGED = "NeuronStateChanged";
    
    // Synapse events
    static constexpr const char* SYNAPSE_CREATED = "SynapseCreated";
    static constexpr const char* SYNAPSE_DESTROYED = "SynapseBroken";
    static constexpr const char* SYNAPSE_WEIGHT_CHANGED = "SynapseWeightChanged";
    
    // Region events
    static constexpr const char* REGION_CREATED = "RegionCreated";
    static constexpr const char* REGION_DESTROYED = "RegionDestroyed";
    
    // Population events
    static constexpr const char* POPULATION_CREATED = "PopulationCreated";
    static constexpr const char* POPULATION_DESTROYED = "PopulationDestroyed";
};

/**
 * Plasticity event system for plasticity events
 */
class PlasticityEventSystem : public EventSystem {
public:
    // STDP events
    static constexpr const char* STDP_APPLY = "STDPApply";
    
    // Hebbian events
    static constexpr const char* HEBBIAN_APPLY = "HebbianApply";
    
    // Structural plasticity events
    static constexpr const char* SYNAPTOGENESIS = "Synaptogenesis";
    static constexpr const char* PRUNING = "Pruning";
    static constexpr const char* MATURATION = "Maturation";
};

/**
 * Simulation event system for simulation events
 */
class SimulationEventSystem : public EventSystem {
public:
    // Simulation lifecycle events
    static constexpr const char* SIMULATION_STARTED = "SimulationStarted";
    static constexpr const char* SIMULATION_STOPPED = "SimulationStopped";
    static constexpr const char* SIMULATION_STEP_COMPLETED = "SimulationStepCompleted";
    
    // Time-related events
    static constexpr const char* TIME_ADVANCED = "TimeAdvanced";
    
    // Error events
    static constexpr const char* ERROR_OCCURRED = "ErrorOccurred";
};
}