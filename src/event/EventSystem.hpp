#pragma once

#include "../core/Types/Types.hpp"
#include <vector>
#include <memory>
#include <functional>
#include <string>
#include <unordered_map>
#include <unordered_set>

namespace nlm {

/**
 * Event types for the NLM brain
 */
enum class EventType {
    // Neuron events
    NEURON_SPIKE,
    NEURON_FIRE,
    
    // Synaptic events
    SYNAPSE_WEIGHT_CHANGE,
    SYNAPSE_FORMATION,
    SYNAPSE_PRUNING,
    
    // Memory events
    MEMORY_STORE,
    MEMORY_RETRIEVE,
    EPISODE_CREATED,
    EPISODE_REPLAY,
    
    // Neuromodulation events
    DOPAMINE_RELEASE,
    ACETYLCHOLINE_RELEASE,
    NOREPINEPHRINE_RELEASE,
    SEROTONIN_RELEASE,
    
    // Cognitive events
    PLANNING_START,
    PLANNING_COMPLETE,
    CONCEPT_FORMATION,
    ATTENTION_FOCUS,
    
    // Development events
    DEVELOPMENTAL_STAGE_CHANGE,
    STRUCTURAL_PLATENCY_UPDATE,
    
    // Environmental events
    ACTION_APPLIED,
    REWARD_RECEIVED,
    NOVELTY_DETECTED,
    
    // Simulation events
    SIMULATION_STEP,
    CHECKPOINT_SAVE,
    CHECKPOINT_LOAD,
    
    // Lifecycle events
    BRAIN_INITIALIZED,
    BRAIN_RESET,
    EXPERIMENT_COMPLETE
};

/**
 * Event data structures
 */
struct Event {
    EventType type;
    uint64_t timestamp;
    uint64_t simulationStep;
    std::string source;
    
    // Event data (union-like structure using variant)
    // For simplicity, we'll use a map for key-value data
    std::unordered_map<std::string, std::string> data;
    
    // Additional event-specific data
    NeuronId neuronId;
    SynapseId synapseId;
    RegionId regionId;
    float value;
    std::vector<float> vectorData;
    
    Event() : timestamp(0), simulationStep(0), value(0.0f) {}
    
    Event(EventType t, uint64_t ts, uint64_t step, const std::string& src = "")
        : type(t), timestamp(ts), simulationStep(step), source(src), value(0.0f) {}
    
    // Helper methods
    template<typename T>
    void setData(const std::string& key, const T& value) {
        data[key] = std::to_string(value);
    }
    
    template<typename T>
    T getData(const std::string& key, T defaultValue) const {
        auto it = data.find(key);
        if (it != data.end()) {
            return static_cast<T>(std::stof(it->second));
        }
        return defaultValue;
    }
    
    const std::string& getDataString(const std::string& key) const {
        auto it = data.find(key);
        static const std::string empty = "";
        return (it != data.end()) ? it->second : empty;
    }
};

/**
 * Event listener interface
 */
class EventListener {
public:
    virtual ~EventListener() = default;
    
    virtual void onEvent(const Event& event) = 0;
    virtual std::vector<EventType> getEventTypes() const = 0;
    virtual std::string getListenerName() const = 0;
};

/**
 * Event system for component communication
 */
class EventSystem {
public:
    EventSystem();
    ~EventSystem();
    
    // Event publishing
    void publishEvent(const Event& event);
    
    // Event subscription
    void subscribe(EventType eventType, std::shared_ptr<EventListener> listener);
    void unsubscribe(EventType eventType, std::shared_ptr<EventListener> listener);
    void unsubscribeAll(std::shared_ptr<EventListener> listener);
    
    // Event processing
    void processEvents();
    void setMaxQueueSize(size_t maxSize) { maxQueueSize_ = maxSize; }
    size_t getEventCount() const { return eventQueue_.size(); }
    
    // Listener management
    void addGlobalListener(std::shared_ptr<EventListener> listener);
    void removeGlobalListener(std::shared_ptr<EventListener> listener);
    
    // Configuration
    void setEventFilter(EventType eventType, bool enabled) {
        eventFilters_[eventType] = enabled;
    }
    bool isEventFiltered(EventType eventType) const {
        return eventFilters_.find(eventType) != eventFilters_.end() && !eventFilters_.at(eventType);
    }
    
    // Utility methods
    std::vector<EventType> getRegisteredEventTypes() const;
    std::vector<std::string> getRegisteredListeners() const;
    
    // Clear all events
    void clearEvents();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    size_t maxQueueSize_;
    std::unordered_map<EventType, bool> eventFilters_;
};

/**
 * BrainEventListener - Event listener for Brain class
 */
class BrainEventListener : public EventListener {
public:
    BrainEventListener(std::shared_ptr<Brain> brain);
    virtual ~BrainEventListener();
    
    void onEvent(const Event& event) override;
    std::vector<EventType> getEventTypes() const override;
    std::string getListenerName() const override;
    
    // Connect to brain
    void connectToBrain(std::shared_ptr<Brain> brain) { brain_ = brain; }
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    std::shared_ptr<Brain> brain_;
};

/**
 * MemoryEventListener - Event listener for Memory systems
 */
class MemoryEventListener : public EventListener {
public:
    MemoryEventListener(std::shared_ptr<NeuralWorkingMemory> workingMemory,
                       std::shared_ptr<NeuralEpisodicMemory> episodicMemory);
    virtual ~MemoryEventListener();
    
    void onEvent(const Event& event) override;
    std::vector<EventType> getEventTypes() const override;
    std::string getListenerName() const override;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    std::shared_ptr<NeuralWorkingMemory> workingMemory_;
    std::shared_ptr<NeuralEpisodicMemory> episodicMemory_;
};

/**
 * NeuromodulationEventListener - Event listener for Neuromodulation systems
 */
class NeuromodulationEventListener : public EventListener {
public:
    NeuromodulationEventListener(std::shared_ptr<Brain> brain);
    virtual ~NeuromodulationEventListener();
    
    void onEvent(const Event& event) override;
    std::vector<EventType> getEventTypes() const override;
    std::string getListenerName() const override;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    std::shared_ptr<Brain> brain_;
};

/**
 * CognitiveEventListener - Event listener for Cognitive systems
 */
class CognitiveEventListener : public EventListener {
public:
    CognitiveEventListener(std::shared_ptr<Brain> brain);
    virtual ~CognitiveEventListener();
    
    void onEvent(const Event& event) override;
    std::vector<EventType> getEventTypes() const override;
    std::string getListenerName() const override;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    std::shared_ptr<Brain> brain_;
};

} // namespace nlm
