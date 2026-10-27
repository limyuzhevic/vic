// Event System for NLM
// Extensible event-driven architecture for neural network events
// Enables third-party extensions, custom components, and event-based functionality

#ifndef NLM_EVENT_SYSTEM_HPP
#define NLM_EVENT_SYSTEM_HPP

#include <string>
#include <memory>
#include <vector>
#include <unordered_map>
#include <functional>
#include <stdexcept>
#include <any>
#include <chrono>
#include <unordered_set>

namespace nlm {

// Event System Error Types
enum class EventError {
    SUCCESS = 0,
    EVENT_TYPE_NOT_FOUND = 1,
    HANDLER_ALREADY_REGISTERED = 2,
    HANDLER_NOT_FOUND = 3,
    INVALID_EVENT_DATA = 4,
    EVENT_BUS_NOT_AVAILABLE = 5
};

class EventError : public std::runtime_error {
public:
    EventError(EventError code, const std::string& message)
        : std::runtime_error("Event error: " + message), code_(code) {}
    
    EventError errorCode() const { return code_; }
    
private:
    EventError code_;
};

// Event Types
enum class EventType {
    // Brain events
    BrainInitialized = 0,
    BrainReset = 1,
    BrainStep = 2,
    
    // Neuron events
    NeuronCreated = 3,
    NeuronDestroyed = 4,
    NeuronSpike = 5,
    NeuronMembranePotentialChanged = 6,
    NeuronRefractoryChanged = 7,
    
    // Synapse events
    SynapseCreated = 8,
    SynapseDestroyed = 9,
    SynapseWeightChanged = 10,
    SynapseSpiking = 11,
    
    // Memory events
    MemoryStored = 12,
    MemoryRetrieved = 13,
    MemoryUpdated = 14,
    MemoryEvicted = 15,
    
    // Neuromodulation events
    NeuromodulatorLevelChanged = 16,
    DopamineReleased = 17,
    CuriosityTriggered = 18,
    NoveltyDetected = 19,
    
    // Development events
    DevelopmentalStageChanged = 20,
    PlasticityRateChanged = 21,
    CriticalPeriodEntered = 22,
    
    // Prediction events
    PredictionMade = 23,
    PredictionErrorOccurred = 24,
    ConfidenceUpdated = 25,
    
    // Cognition events
    ConceptFormed = 26,
    PlanCreated = 27,
    AttentionSelected = 28,
    
    // Environment events
    SensoryInputReceived = 29,
    MotorActionProduced = 30,
    RewardReceived = 31,
    
    // System events
    SystemError = 32,
    SystemWarning = 33,
    SystemInfo = 34,
    SystemDebug = 35,
    
    // Custom events
    Custom = 255
};

// Event Priority
enum class EventPriority {
    LOW = 0,
    NORMAL = 1,
    HIGH = 2,
    CRITICAL = 3
};

// Event Structure
struct Event {
    EventType type;
    std::string source;
    std::chrono::high_resolution_clock::time_point timestamp;
    EventPriority priority;
    std::any data;
    
    Event(EventType eventType, const std::string& eventSource, EventPriority eventPriority = EventPriority::NORMAL)
        : type(eventType), source(eventSource), timestamp(std::chrono::high_resolution_clock::now()),
          priority(eventPriority) {}
    
    Event(EventType eventType, const std::string& eventSource, const std::any& eventData, EventPriority eventPriority = EventPriority::NORMAL)
        : type(eventType), source(eventSource), timestamp(std::chrono::high_resolution_clock::now()),
          priority(eventPriority), data(eventData) {}
    
    // Helper methods for common data types
    template<typename T>
    T getData() const {
        return std::any_cast<T>(data);
    }
    
    template<typename T>
    bool hasData() const {
        return data.type() == typeid(T);
    }
    
    std::string getDataAsString() const {
        if (!data.has_value()) return "";
        
        std::ostringstream oss;
        oss << "Event[type=" << static_cast<int>(type) << ",source=" << source << "]";
        return oss.str();
    }
};

// Event Handler Interface
class IEventHandler {
public:
    virtual ~IEventHandler() = default;
    
    // Check if this handler can process the given event type
    virtual bool canHandle(EventType eventType) const = 0;
    
    // Handle the event
    virtual void handleEvent(const Event& event) = 0;
    
    // Get handler name
    virtual std::string name() const = 0;
    
    // Get handler priority
    virtual EventPriority priority() const {
        return EventPriority::NORMAL;
    }
};

// Event Filter Interface
class IEventFilter {
public:
    virtual ~IEventFilter() = default;
    
    // Check if the event should be processed
    virtual bool shouldProcess(const Event& event) const = 0;
    
    // Get filter name
    virtual std::string name() const = 0;
};

// Event Listener Interface
class IEventListener {
public:
    virtual ~IEventListener() = default;
    
    // Handle event
    virtual void onEvent(const Event& event) = 0;
    
    // Get listener name
    virtual std::string name() const = 0;
    
    // Get listener priority
    virtual EventPriority priority() const {
        return EventPriority::NORMAL;
    }
};

// Event Bus
class EventBus {
public:
    EventBus() = default;
    ~EventBus() = default;
    
    // Disable copying, enable moving
    EventBus(const EventBus&) = delete;
    EventBus& operator=(const EventBus&) = delete;
    EventBus(EventBus&&) noexcept = default;
    EventBus& operator=(EventBus&&) noexcept = default;
    
    // Singleton access
    static EventBus& instance() {
        static EventBus instance;
        return instance;
    }
    
    // Event publishing
    void publish(const Event& event) {
        // Apply filters
        if (!shouldProcessEvent(event)) {
            return;
        }
        
        // Dispatch event to handlers
        dispatchEvent(event);
        
        // Log event if logging is enabled
        if (eventLoggingEnabled_) {
            logEvent(event);
        }
    }
    
    // Subscribe event handler
    void subscribe(std::unique_ptr<IEventHandler> handler) {
        if (!handler) {
            throw EventError(EventError::INVALID_EVENT_DATA, "Null event handler");
        }
        
        // Check if handler with same name already exists
        for (const auto& existing : eventHandlers_) {
            if (existing->name() == handler->name()) {
                throw EventError(EventError::HANDLER_ALREADY_REGISTERED,
                               "Event handler already registered: " + handler->name());
            }
        }
        
        // Add handler (sorted by priority)
        eventHandlers_.push_back(std::move(handler));
        std::sort(eventHandlers_.begin(), eventHandlers_.end(),
                 [](const std::unique_ptr<IEventHandler>& a, const std::unique_ptr<IEventHandler>& b) {
                     return a->priority() > b->priority(); // Higher priority first
                 });
        
        NLM_LOG_INFO("Event handler registered: " + eventHandlers_.back()->name());
    }
    
    // Subscribe event listener
    void subscribe(std::unique_ptr<IEventListener> listener) {
        if (!listener) {
            throw EventError(EventError::INVALID_EVENT_DATA, "Null event listener");
        }
        
        // Check if listener with same name already exists
        for (const auto& existing : eventListeners_) {
            if (existing->name() == listener->name()) {
                throw EventError(EventError::HANDLER_ALREADY_REGISTERED,
                               "Event listener already registered: " + listener->name());
            }
        }
        
        eventListeners_.push_back(std::move(listener));
        std::sort(eventListeners_.begin(), eventListeners_.end(),
                 [](const std::unique_ptr<IEventListener>& a, const std::unique_ptr<IEventListener>& b) {
                     return a->priority() > b->priority();
                 });
        
        NLM_LOG_INFO("Event listener registered: " + eventListeners_.back()->name());
    }
    
    // Unsubscribe event handler
    bool unsubscribe(const std::string& handlerName) {
        auto it = std::find_if(eventHandlers_.begin(), eventHandlers_.end(),
                              [&handlerName](const std::unique_ptr<IEventHandler>& handler) {
                                  return handler->name() == handlerName;
                              });
        
        if (it == eventHandlers_.end()) {
            return false;
        }
        
        eventHandlers_.erase(it);
        NLM_LOG_INFO("Event handler unregistered: " + handlerName);
        
        return true;
    }
    
    // Unsubscribe event listener
    bool unsubscribeListener(const std::string& listenerName) {
        auto it = std::find_if(eventListeners_.begin(), eventListeners_.end(),
                              [&listenerName](const std::unique_ptr<IEventListener>& listener) {
                                  return listener->name() == listenerName;
                              });
        
        if (it == eventListeners_.end()) {
            return false;
        }
        
        eventListeners_.erase(it);
        NLM_LOG_INFO("Event listener unregistered: " + listenerName);
        
        return true;
    }
    
    // Subscribe filter
    void subscribeFilter(std::unique_ptr<IEventFilter> filter) {
        if (!filter) {
            throw EventError(EventError::INVALID_EVENT_DATA, "Null event filter");
        }
        
        eventFilters_.push_back(std::move(filter));
        NLM_LOG_INFO("Event filter registered: " + eventFilters_.back()->name());
    }
    
    // Unsubscribe filter
    bool unsubscribeFilter(const std::string& filterName) {
        auto it = std::find_if(eventFilters_.begin(), eventFilters_.end(),
                              [&filterName](const std::unique_ptr<IEventFilter>& filter) {
                                  return filter->name() == filterName;
                              });
        
        if (it == eventFilters_.end()) {
            return false;
        }
        
        eventFilters_.erase(it);
        NLM_LOG_INFO("Event filter unregistered: " + filterName);
        
        return true;
    }
    
    // Get statistics
    std::unordered_map<std::string, double> getStatistics() const {
        std::unordered_map<std::string, double> stats;
        
        stats["event_handlers"] = static_cast<double>(eventHandlers_.size());
        stats["event_listeners"] = static_cast<double>(eventListeners_.size());
        stats["event_filters"] = static_cast<double>(eventFilters_.size());
        stats["event_logging_enabled"] = eventLoggingEnabled_ ? 1.0 : 0.0;
        
        return stats;
    }
    
    // Enable/disable event logging
    void setEventLogging(bool enabled) {
        eventLoggingEnabled_ = enabled;
    }
    
    // Get event log
    std::vector<Event> getEventLog(size_t maxEvents = 1000) const {
        std::vector<Event> events;
        
        if (eventLog_.size() > maxEvents) {
            size_t startIndex = eventLog_.size() - maxEvents;
            events.assign(eventLog_.begin() + startIndex, eventLog_.end());
        } else {
            events = eventLog_;
        }
        
        return events;
    }
    
    // Clear event log
    void clearEventLog() {
        eventLog_.clear();
    }
    
private:
    // Check if event should be processed based on filters
    bool shouldProcessEvent(const Event& event) const {
        for (const auto& filter : eventFilters_) {
            if (!filter->shouldProcess(event)) {
                return false;
            }
        }
        return true;
    }
    
    // Dispatch event to handlers
    void dispatchEvent(const Event& event) {
        // Notify listeners
        for (const auto& listener : eventListeners_) {
            listener->onEvent(event);
        }
        
        // Notify handlers
        for (const auto& handler : eventHandlers_) {
            if (handler->canHandle(event.type)) {
                handler->handleEvent(event);
            }
        }
    }
    
    // Log event
    void logEvent(const Event& event) {
        eventLog_.push_back(event);
        
        // Limit log size
        if (eventLog_.size() > 10000) {
            size_t excess = eventLog_.size() - 10000;
            eventLog_.erase(eventLog_.begin(), eventLog_.begin() + excess);
        }
        
        NLM_LOG_DEBUG("Event logged: " + event.getDataAsString());
    }
    
    std::vector<std::unique_ptr<IEventHandler>> eventHandlers_;
    std::vector<std::unique_ptr<IEventListener>> eventListeners_;
    std::vector<std::unique_ptr<IEventFilter>> eventFilters_;
    std::vector<Event> eventLog_;
    bool eventLoggingEnabled_ = true;
};

// Convenience functions
namespace event {
    // Publish event
    inline void publish(const Event& event) {
        EventBus::instance().publish(event);
    }
    
    // Publish brain step event
    inline void publishBrainStep(size_t step) {
        Event event(EventType::BrainStep, "Brain", EventPriority::HIGH);
        event.data = step;
        EventBus::instance().publish(event);
    }
    
    // Publish neuron spike event
    inline void publishNeuronSpike(size_t neuronId, float voltage) {
        std::unordered_map<std::string, double> data;
        data["neuron_id"] = static_cast<double>(neuronId);
        data["voltage"] = voltage;
        
        Event event(EventType::NeuronSpike, "Brain", data, EventPriority::HIGH);
        EventBus::instance().publish(event);
    }
    
    // Publish synapse weight change event
    inline void publishSynapseWeightChange(size_t synapseId, float oldWeight, float newWeight) {
        std::unordered_map<std::string, double> data;
        data["synapse_id"] = static_cast<double>(synapseId);
        data["old_weight"] = oldWeight;
        data["new_weight"] = newWeight;
        
        Event event(EventType::SynapseWeightChanged, "Brain", data, EventPriority::NORMAL);
        EventBus::instance().publish(event);
    }
    
    // Publish development stage change event
    inline void publishDevelopmentStageChange(int fromStage, int toStage) {
        std::unordered_map<std::string, double> data;
        data["from_stage"] = static_cast<double>(fromStage);
        data["to_stage"] = static_cast<double>(toStage);
        
        Event event(EventType::DevelopmentalStageChanged, "Brain", data, EventPriority::NORMAL);
        EventBus::instance().publish(event);
    }
}

} // namespace nlm

#endif // NLM_EVENT_SYSTEM_HPP