#pragma once

#include <string>
#include <vector>
#include <memory>
#include <chrono>
#include <map>
#include <unordered_map>
#include <functional>
#include "../core/Types/Types.hpp"

namespace nlm {

// Event tracking system for neural simulation
enum class EventCategory {
    Spike,
    Synapse,
    Plasticity,
    Memory,
    Neuromodulation,
    Development,
    Input,
    Output,
    Performance,
    Error,
    Custom
};

enum class EventType {
    Unknown,
    Fired,
    Refired,
    InputReceived,
    OutputProduced,
    STDPUpdate,
    HebbianUpdate,
    Synaptogenesis,
    Pruning,
    MemoryStore,
    MemoryRecall,
    RewardReceived,
    NoveltyDetected,
    PredictionError,
    CuriositySignal,
    DevelopmentStageChange,
    ConfigurationChange,
    CheckpointSaved,
    CheckpointLoaded,
    BatchStarted,
    BatchCompleted,
    ErrorOccurred,
    WarningIssued,
    Custom
};

enum class EventPriority {
    Low,
    Normal,
    High,
    Critical,
    Urgent
};

// Timeline event for neural simulation
struct TimelineEvent {
    std::chrono::steady_clock::time_point timestamp;
    EventCategory category;
    EventType type;
    EventPriority priority;
    std::string details;
    std::string source;  // "neuron_42", "synapse_17", "system", etc.
    std::string target;  // "neuron_105", "region_A", etc.
    std::map<std::string, std::string> metadata;
    double value;        // Numeric value (weight, amplitude, etc.)
    bool processed;       // Whether event has been processed
    size_t step;          // Simulation step when event occurred
    double simTime;       // Simulation time when event occurred (ms)
};

// Event handler function type
using EventHandler = std::function<void(const TimelineEvent&)>;

// Event filter for selective event processing
class EventFilter {
public:
    virtual ~EventFilter() = default;
    virtual bool matches(const TimelineEvent& event) const = 0;
    virtual std::string getDescription() const = 0;
};

// Detailed spike event
struct DetailedSpikeEvent {
    NeuronId neuronId;
    SimulationStep step;
    Timestamp time;
    MembranePotential amplitude;
    bool isExcitatory;
    float weight;
    SynapseId presynapticSynapse;  // Valid only for presynaptic spikes
    SynapseId postsynapticSynapse; // Valid only for postsynaptic spikes
    std::string origin;            // "neuron", "external", "pattern"
    std::string details;           // Additional event details
};

// Plasticity update event
struct PlasticityUpdateEvent {
    SynapseId synapseId;
    NeuronId presynapticId;
    NeuronId postsynapticId;
    SimulationStep step;
    Timestamp time;
    float preWeight;
    float postWeight;
    float newWeight;
    float weightChange;
    float learningRate;
    std::string plasticityType;  // "STDP", "Hebbian", "Reward-modulated"
    std::string details;
    float eligibilityTrace;
};

// Memory system events
struct MemoryEvent {
    std::string eventType;  // "store", "recall", "consolidate", "clear"
    NeuronId neuronId;
    SimulationStep step;
    Timestamp time;
    float value;
    std::vector<NeuronId> associatedNeurons;
    std::string memoryType;  // "working", "episodic", "associative"
    std::string details;
};

// Neuromodulation events
struct NeuromodulationEvent {
    std::string neuromodulator;  // "dopamine", "serotonin", "acetylcholine"
    NeuronId neuronId;
    SimulationStep step;
    Timestamp time;
    float level;
    float effectOnExcitability;
    float effectOnPlasticity;
    std::string trigger;  // "reward", "prediction_error", "novelty"
    std::string details;
};

// Developmental events
struct DevelopmentEvent {
    DevelopmentalStage stage;
    SimulationStep step;
    Timestamp time;
    std::vector<NeuronId> affectedNeurons;
    std::vector<SynapseId> affectedSynapses;
    std::string changeType;  // "growth", "pruning", "maturation"
    float magnitude;
    std::string details;
};

// Event manager for neural simulation event tracking
class EventTrackingManager {
public:
    EventTrackingManager();
    ~EventTrackingManager();
    
    // Event recording
    void recordEvent(const TimelineEvent& event);
    void recordSpikeEvent(const DetailedSpikeEvent& event);
    void recordPlasticityUpdate(const PlasticityUpdateEvent& event);
    void recordMemoryEvent(const MemoryEvent& event);
    void recordNeuromodulationEvent(const NeuromodulationEvent& event);
    void recordDevelopmentEvent(const DevelopmentEvent& event);
    
    // Event querying and filtering
    std::vector<TimelineEvent> queryEvents(EventCategory category, 
                                          EventType type = EventType::Unknown,
                                          SimulationStep startStep = 0,
                                          SimulationStep endStep = static_cast<SimulationStep>(-1),
                                          const std::string& sourceFilter = "",
                                          const std::string& targetFilter = "") const;
    
    std::vector<DetailedSpikeEvent> getSpikeEvents(const std::string& source = "",
                                                  const std::string& target = "") const;
    std::vector<PlasticityUpdateEvent> getPlasticityEvents(const std::string& synapse = "") const;
    
    // Event analysis
    size_t getEventCount(EventCategory category, EventType type) const;
    double getEventRate(EventCategory category, EventType type, 
                       double timeWindowSeconds) const;
    std::vector<std::pair<EventCategory, EventType>> getMostFrequentEvents(
        size_t limit = 10) const;
    std::vector<TimelineEvent> findEventSequence(const std::vector<EventType>& sequence,
                                                SimulationStep maxGap = 10) const;
    
    // Event filtering and processing
    void addEventFilter(std::shared_ptr<EventFilter> filter);
    void removeEventFilter(std::shared_ptr<EventFilter> filter);
    void addEventHandler(EventCategory category, EventType type, EventHandler handler);
    void removeEventHandler(EventCategory category, EventType type, EventHandler handler);
    
    // Timeline navigation
    std::vector<TimelineEvent> getTimeline(SimulationStep startStep = 0,
                                          SimulationStep endStep = static_cast<SimulationStep>(-1),
                                          EventCategory category = EventCategory::Unknown) const;
    SimulationStep getFirstEventStep() const;
    SimulationStep getLastEventStep() const;
    double getTimelineDuration() const;
    
    // Export and import
    bool exportEvents(const std::string& filepath, 
                     const std::string& format = "json",
                     EventCategory category = EventCategory::Unknown) const;
    bool importEvents(const std::string& filepath, 
                     const std::string& format = "json");
    
    // Analysis utilities
    std::string generateEventStatistics() const;
    std::string generateSpikeStatistics() const;
    std::string generatePlasticityStatistics() const;
    std::string generateNetworkActivityReport() const;
    
    // Cleanup
    void clearEvents();
    void reset();
    
    // Configuration
    void setMaxEvents(size_t maxEvents);
    void setEventRetentionTime(double retentionSeconds);
    void enableRealTimeProcessing(bool enable);
    bool isProcessingPaused() const;
    void pauseProcessing(bool pause);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
