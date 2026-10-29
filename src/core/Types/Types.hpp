#pragma once

#include <cstdint>
#include <type_traits>

namespace nlm {

// Strongly-typed identifiers for safety

// Neuron identifier
struct NeuronId {
    uint64_t value;
    
    constexpr NeuronId() : value(0) {}
    explicit constexpr NeuronId(uint64_t v) : value(v) {}
    
    constexpr bool operator==(const NeuronId& other) const = default;
    constexpr bool operator!=(const NeuronId& other) const = default;
    constexpr bool operator<(const NeuronId& other) const { return value < other.value; }
    
    constexpr uint64_t index() const { return value; }
};

// Synapse identifier
struct SynapseId {
    uint64_t value;
    
    constexpr SynapseId() : value(0) {}
    explicit constexpr SynapseId(uint64_t v) : value(v) {}
    
    constexpr bool operator==(const SynapseId& other) const = default;
    constexpr bool operator!=(const SynapseId& other) const = default;
    constexpr bool operator<(const SynapseId& other) const { return value < other.value; }
    
    constexpr uint64_t index() const { return value; }
};

// Region identifier
struct RegionId {
    uint64_t value;
    
    constexpr RegionId() : value(0) {}
    explicit constexpr RegionId(uint64_t v) : value(v) {}
    
    constexpr bool operator==(const RegionId& other) const = default;
    constexpr bool operator!=(const RegionId& other) const = default;
    constexpr bool operator<(const RegionId& other) const { return value < other.value; }
    
    constexpr uint64_t index() const { return value; }
};

// Population identifier
struct PopulationId {
    uint64_t value;
    
    constexpr PopulationId() : value(0) {}
    explicit constexpr PopulationId(uint64_t v) : value(v) {}
    
    constexpr bool operator==(const PopulationId& other) const = default;
    constexpr bool operator!=(const PopulationId& other) const = default;
    constexpr bool operator<(const PopulationId& other) const { return value < other.value; }
    
    constexpr uint64_t index() const { return value; }
};

// Simulation step counter
using SimulationStep = uint64_t;

// Simulation time in seconds (floating point)
using Timestamp = double;

// Timestep duration in seconds
using TimestepDuration = double;

// Index types for arrays
using NeuronIndex = uint64_t;
using SynapseIndex = uint64_t;
using PopulationIndex = uint64_t;
using RegionIndex = uint64_t;

// Synaptic weight type
using SynapticWeight = float;

// Membrane potential type
using MembranePotential = float;

// Neuron firing rate type
using FiringRate = float;

// Connection delay in simulation steps
using Delay = uint32_t;

// Typed enums for neuron types
enum class NeuronType : uint8_t {
    Excitatory,
    Inhibitory,
    Modulatory,
    Sensory,
    Motor,
    Internal
};

// Typed enums for synapse types
enum class SynapseType : uint8_t {
    Excitatory,
    Inhibitory,
    Modulatory,
    Electrical,
    GapJunction
};

// Synaptic plasticity state flags
struct PlasticityFlags {
    bool hebbian : 1;
    bool stdp : 1;
    bool reward_modulated : 1;
    bool structural : 1;
    bool eligible : 1;
    
    constexpr PlasticityFlags() : hebbian(false), stdp(false), 
                                  reward_modulated(false), structural(false), 
                                  eligible(false) {}
};

// Developmental state
enum class DevelopmentalStage : uint8_t {
    Initial,
    CriticalPeriod,
    Maturation,
    Adult,
    Aging
};

// Spike event for event-driven computation
struct SpikeEvent {
    NeuronId source_neuron;
    Timestamp timestamp;
    SimulationStep step;
    
    SpikeEvent() : source_neuron(), timestamp(0.0), step(0) {}
    SpikeEvent(NeuronId nid, Timestamp ts, SimulationStep s)
        : source_neuron(nid), timestamp(ts), step(s) {}
};

// Delayed spike event for synaptic transmission with delay
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

// Action potential state
enum class FiringState : uint8_t {
    Resting,
    Active,
    Refractory,
    Inhibited
};

// Generic handle types for efficiency
using NeuronHandle = uint64_t;
using SynapseHandle = uint64_t;

// Invalid ID sentinel
constexpr NeuronId INVALID_NEURON_ID = NeuronId(UINT64_MAX);
constexpr SynapseId INVALID_SYNAPSE_ID = SynapseId(UINT64_MAX);
constexpr RegionId INVALID_REGION_ID = RegionId(UINT64_MAX);
constexpr PopulationId INVALID_POPULATION_ID = PopulationId(UINT64_MAX);

// Neural event types for reactive brain systems
// Events trigger when specific neural conditions are met

enum class NeuralEventType {
    // Spike-related events
    NeuronFired,              // A neuron has fired a spike
    SpikePatternDetected,      // Specific spike pattern detected
    BurstEvent,               // High-frequency spike burst
    
    // State change events
    NeuronStateChanged,        // Neuron state has changed
    PopulationActivityChanged, // Overall population activity level changed
    RegionalActivityChanged,   // Activity in a specific region changed
    
    // Plasticity events
    SynapticWeightChanged,     // Synaptic weight has changed
    STDPTriggered,            // STDP plasticity event occurred
    HebbianLearning,          // Hebbian learning event
    
    // Memory-related events
    MemoryEncoding,           // New memory being encoded
    MemoryRetrieval,          // Memory retrieval event
    MemoryConsolidation,      // Memory consolidation event
    
    // Neuromodulation events
    DopamineRelease,          // Dopamine signal released
    SerotoninRelease,         // Serotonin signal released
    AcetylcholineRelease,     // Acetylcholine signal released
    
    // Environmental events
    SensoryInputReceived,     // New sensory input received
    RewardReceived,           // Reward signal received
    PunishmentReceived,       // Punishment signal received
    
    // Developmental events
    CriticalPeriodStarted,    // Critical period for learning began
    DevelopmentStageChanged,  // Developmental stage changed
    MaturationCompleted,      // Maturation process completed
    
    // Prediction events
    PredictionError,          // Prediction error detected
    PredictionCorrect,        // Prediction was correct
    SurpriseEvent,            // Unexpected event occurred
    
    // Cognitive events
    AttentionShifted,         // Attention shifted to different focus
    ConceptFormation,         // New concept formed
    PlanningCompleted,        // Planning completed
    
    // System events
    SimulationStepCompleted,  // Simulation step completed
    SystemReset,              // System was reset
    CheckpointReached,        // Checkpoint/save point reached
    
    // Custom events
    CustomEvent              // User-defined event type
};

// Neural event data structure
struct NeuralEvent {
    NeuralEventType type;            // Type of event
    double timestamp;                // When event occurred
    int sourceNeuronId;              // ID of neuron that generated event (if applicable)
    int targetNeuronId;              // ID of target neuron (if applicable)
    float eventData;                 // Additional event-specific data
    std::string eventDescription;    // Human-readable description
    bool isProcessed;                // Whether event has been processed
    
    NeuralEvent() 
        : type(NeuralEventType::CustomEvent), timestamp(0.0), sourceNeuronId(-1), 
          targetNeuronId(-1), eventData(0.0), isProcessed(false) {}
        
    NeuralEvent(NeuralEventType t, double ts, int source = -1, int target = -1, 
                float data = 0.0, const std::string& desc = "")
        : type(t), timestamp(ts), sourceNeuronId(source), targetNeuronId(target),
          eventData(data), eventDescription(desc), isProcessed(false) {}
};

// Event handler function type
using EventHandler = std::function<void(const NeuralEvent&)>;

// Event callback for synchronous processing
using EventCallback = std::function<void(const NeuralEvent&, bool& processed)>;

// Event priority levels
enum class EventPriority {
    Low = 1,
    Normal = 2,
    High = 3,
    Critical = 4
};

// Event filtering criteria
struct EventFilter {
    std::vector<NeuralEventType> eventTypes;     // Event types to filter for
    std::set<int> sourceNeurons;                // Source neurons to filter by
    std::set<int> targetNeurons;                // Target neurons to filter by
    double minTimestamp;                        // Minimum timestamp for events
    double maxTimestamp;                        // Maximum timestamp for events
    std::string keywordFilter;                  // Keyword filter for descriptions
    
    EventFilter() : minTimestamp(0.0), maxTimestamp(std::numeric_limits<double>::max()) {}
    
    bool matches(const NeuralEvent& event) const {
        // Filter by event type
        if (!eventTypes.empty() && 
            std::find(eventTypes.begin(), eventTypes.end(), event.type) == eventTypes.end()) {
            return false;
        }
        
        // Filter by source neuron
        if (!sourceNeurons.empty() && 
            sourceNeurons.find(event.sourceNeuronId) == sourceNeurons.end()) {
            return false;
        }
        
        // Filter by target neuron
        if (!targetNeurons.empty() && 
            targetNeurons.find(event.targetNeuronId) == targetNeurons.end()) {
            return false;
        }
        
        // Filter by timestamp range
        if (event.timestamp < minTimestamp || event.timestamp > maxTimestamp) {
            return false;
        }
        
        // Filter by keywords
        if (!keywordFilter.empty() && 
            event.eventDescription.find(keywordFilter) == std::string::npos) {
            return false;
        }
        
        return true;
    }
};

// Neural event system for reactive brain processing
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
    inline NeuralEvent createNeuronFiredEvent(NeuronId neuronId, Timestamp timestamp, 
                                             SimulationStep step) {
        return NeuralEvent(NeuralEventType::NeuronFired, timestamp, 
                          neuronId.index(), -1, 1.0f, 
                          "Neuron " + std::to_string(neuronId.index()) + " fired at step " + 
                          std::to_string(step));
    }
    
    inline NeuralEvent createSensoryInputEvent(int sensoryNeuronId, float intensity, 
                                               double timestamp) {
        return NeuralEvent(NeuralEventType::SensoryInputReceived, timestamp,
                          sensoryNeuronId, -1, intensity,
                          "Sensory input received with intensity: " + std::to_string(intensity));
    }
    
    inline NeuralEvent createPlasticityEvent(NeuronId preNeuron, NeuronId postNeuron,
                                            float weightChange, double timestamp) {
        return NeuralEvent(NeuralEventType::SynapticWeightChanged, timestamp,
                          preNeuron.index(), postNeuron.index(), weightChange,
                          "Synaptic weight changed by: " + std::to_string(weightChange));
    }
    
    inline NeuralEvent createRewardEvent(float reward, int sourceNeuronId, double timestamp) {
        return NeuralEvent(NeuralEventType::RewardReceived, timestamp,
                          sourceNeuronId, -1, reward,
                          "Reward received: " + std::to_string(reward));
    }
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
