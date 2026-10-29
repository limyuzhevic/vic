#pragma once

#include "../core/Types/Types.hpp"
#include <memory>
#include <string>

namespace nlm {

// Neural event system for reactive brain processing
// Manages event generation, filtering, and callback processing
// Integrates with all brain subsystems to enable reactive behavior

class NeuralEventSystem {
public:
    NeuralEventSystem();
    ~NeuralEventSystem();
    
    // Event generation methods
    void generateEvent(const NeuralEvent& event);
    void generateEvent(NeuralEventType type, double timestamp, 
                      int source = -1, int target = -1, 
                      float data = 0.0, const std::string& desc = "");
    
    // Event processing methods
    void processEvents();
    void processEvent(const NeuralEvent& event);
    
    // Event subscription/management
    void subscribe(const EventFilter& filter, EventCallback callback);
    void unsubscribe(EventCallback callback);
    void unsubscribeAll();
    
    // Event filtering and querying
    std::vector<NeuralEvent> getEvents(const EventFilter& filter) const;
    std::vector<NeuralEvent> getUnprocessedEvents() const;
    size_t getEventCount() const;
    
    // Event management
    void clearEvents();
    void setEventLimit(size_t limit);
    size_t getEventLimit() const;
    
    // Event filtering for specific components
    void addEventGenerator(NeuronId neuronId);
    void removeEventGenerator(NeuronId neuronId);
    std::set<NeuronId> getEventGenerators() const;
    
    // Priority-based processing
    void setEventPriority(NeuralEventType type, EventPriority priority);
    EventPriority getEventPriority(NeuralEventType type) const;
    
    // Statistics and monitoring
    size_t getProcessedEventCount(NeuralEventType type) const;
    size_t getUnprocessedEventCount(NeuralEventType type) const;
    
    // Integration with other systems
    void integrateWithEventSources();
    void integrateWithEventSinks();
    
    // Configuration
    void enableEventProcessing(bool enabled);
    bool isEventProcessingEnabled() const;
    
    // Debug and monitoring
    std::string getEventSystemStatus() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Helper functions for event creation and management
namespace EventHelpers {
    // Create common event types
    inline NeuralEvent createNeuronFiredEvent(NeuronId neuronId, Timestamp timestamp, 
                                             SimulationStep step);
    
    inline NeuralEvent createSensoryInputEvent(int sensoryNeuronId, float intensity, 
                                               double timestamp);
    
    inline NeuralEvent createPlasticityEvent(NeuronId preNeuron, NeuronId postNeuron,
                                            float weightChange, double timestamp);
    
    inline NeuralEvent createRewardEvent(float reward, int sourceNeuronId, double timestamp);
}

// Event system integration macros
#define REGISTER_NEURON_EVENTS(neuron_id) \
    extern "C" void register_neural_events_for_neuron(uint64_t neuron_id) \
    { \
        get_global_event_system().addEventGenerator(NeuronId(neuron_id)); \
    } \
    __attribute__((constructor)) \
    void register_neuron_events_ ## neuron_id() { \
        register_neural_events_for_neuron(neuron_id); \
    }

#define GENERATE_NEURAL_EVENT(event_type, timestamp, source, target, data) \
    get_global_event_system().generateEvent(event_type, timestamp, source, target, data)

// Global event system access
NeuralEventSystem& get_global_event_system();

} // namespace nlm