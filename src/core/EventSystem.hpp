// Event system for decoupled communication between brain systems
#pragma once

#include <string>
#include <variant>
#include <vector>
#include <functional>

namespace nlm {

// Event types for brain system communication
enum class EventType {
    NeuronFiring,
    SynapseWeightChange,
    MemoryUpdated,
    NeuromodulationChanged,
    DevelopmentalStageChanged,
    WorkingMemoryUpdate,
    EpisodicMemoryUpdate,
    PredictionUpdate,
    CuriosityUpdate,
    NoveltyUpdate,
    DopamineUpdate,
    ConceptFormation,
    PlanningResult,
    AttentionUpdate,
    RewardReceived,
    ActionSelected,
    EnvironmentalChange,
    ErrorOccurred
};

// Base event class
class IEvent {
public:
    virtual ~IEvent() = default;
    virtual EventType getType() const = 0;
    virtual uint64_t getTimestamp() const = 0;
    virtual std::string getDescription() const = 0;
};

// Specific event implementations
struct NeuronFiringEvent : public IEvent {
    NeuronId neuronId;
    float firingRate;
    uint64_t spikeCount;
    
    EventType getType() const override { return EventType::NeuronFiring; }
    uint64_t getTimestamp() const override { return timestamp; }
    std::string getDescription() const override {
        return "Neuron " + std::to_string(neuronId.index()) + " fired"; }
    
    NeuronFiringEvent(NeuronId id, float rate, uint64_t count) 
        : neuronId(id), firingRate(rate), spikeCount(count), timestamp(0) {}
    
private:
    uint64_t timestamp = 0;
};

struct SynapseWeightChangeEvent : public IEvent {
    SynapseId synapseId;
    NeuronId sourceNeuron;
    NeuronId targetNeuron;
    float oldWeight;
    float newWeight;
    float weightChange;
    
    EventType getType() const override { return EventType::SynapseWeightChange; }
    uint64_t getTimestamp() const override { return timestamp; }
    std::string getDescription() const override {
        return "Synapse weight changed from " + std::to_string(oldWeight) + 
               " to " + std::to_string(newWeight);
    }
    
    SynapseWeightChangeEvent(SynapseId synId, NeuronId src, NeuronId tgt, 
                            float oldW, float newW)
        : synapseId(synId), sourceNeuron(src), targetNeuron(tgt),
          oldWeight(oldW), newWeight(newW), weightChange(newW - oldW) {}
    
private:
    uint64_t timestamp = 0;
};

struct MemoryEvent : public IEvent {
    enum class MemoryType { Working, Episodic, Associative };
    MemoryType type;
    std::vector<std::pair<NeuronId, float>> activeNeurons;
    float totalActivation;
    
    EventType getType() const override { return EventType::MemoryUpdated; }
    uint64_t getTimestamp() const override { return timestamp; }
    std::string getDescription() const override {
        return "Memory updated with " + std::to_string(activeNeurons.size()) + " active neurons";
    }
    
    MemoryEvent(MemoryType memType, const std::vector<std::pair<NeuronId, float>>& neurons)
        : type(memType), activeNeurons(neurons), totalActivation(0.0f) {
        for (const auto& pair : neurons) {
            totalActivation += pair.second;
        }
    }
    
private:
    uint64_t timestamp = 0;
};

struct NeuromodulationEvent : public IEvent {
    enum class NeuromodulatorType { Dopamine, Curiosity, Novelty, PredictionError };
    NeuromodulatorType type;
    float level;
    float change;
    
    EventType getType() const override { return EventType::NeuromodulationChanged; }
    uint64_t getTimestamp() const override { return timestamp; }
    std::string getDescription() const override {
        return "Neuromodulator " + std::to_string(static_cast<int>(type)) + " changed to " + std::to_string(level);
    }
    
    NeuromodulationEvent(NeuromodulatorType modType, float lvl, float change)
        : type(modType), level(lvl), change(change) {}
    
private:
    uint64_t timestamp = 0;
};

// Event handler function type
using EventHandler = std::function<void(const IEvent&)>;

// Event bus for publishing and subscribing to events
class EventBus {
public:
    static EventBus& getInstance() {
        static EventBus instance;
        return instance;
    }
    
    void publish(const IEvent& event) {
        uint64_t timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::system_clock::now().time_since_epoch()).count();
        
        // Add timestamp to event (hacky, but works for now)
        if (event.getType() == EventType::NeuronFiring) {
            // We can't modify the event directly, so we'll store it with timestamp
            // and create a new event with timestamp
            // This is a limitation of the current design
        }
        
        for (const auto& handler : eventHandlers_[event.getType()]) {
            handler(event);
        }
    }
    
    void subscribe(EventType type, EventHandler handler) {
        eventHandlers_[type].push_back(handler);
    }
    
    void unsubscribe(EventType type, EventHandler handler) {
        auto& handlers = eventHandlers_[type];
        handlers.erase(std::remove(handlers.begin(), handlers.end(), handler), handlers.end());
    }
    
    void unsubscribeAll(EventType type) {
        eventHandlers_[type].clear();
    }
    
    size_t handlerCount(EventType type) const {
        return eventHandlers_[type].size();
    }
    
private:
    EventBus() = default;
    ~EventBus() = default;
    EventBus(const EventBus&) = delete;
    EventBus& operator=(const EventBus&) = delete;
    
    std::unordered_map<EventType, std::vector<EventHandler>> eventHandlers_;
};

} // namespace nlm
