#pragma once

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>
#include <functional>
#include <chrono>

namespace nlm {

// Forward declarations
class Observer;

// Notification types for different events
enum class NotificationType {
    Spike,                    // Neuron spike event
    MemoryUpdate,             // Memory system update
    PredictionError,          // Prediction error signal
    Neuromodulation,          // Neuromodulator signal
    SensoryInput,             // Sensory input event
    MotorOutput,              // Motor action
    DevelopmentUpdate,        // Developmental change
    PlasticityUpdate,         // Plasticity rule change
    WorkingMemoryUpdate,      // Working memory state change
    EpisodicMemoryUpdate,     // Episodic memory update
    ConceptFormation,         // Concept formation event
    PlanningUpdate,           // Planning state change
    AttentionShift,           // Attention shift
    CuriosityActivation,      // Curiosity-driven behavior
    RewardReceived,           // Reward signal
    NoveltyDetected,          // Novelty detection
    IntegrationComplete,      // System integration event
    SystemReady,              // System initialization complete
    SystemShutdown,           // System shutdown
    ConfigurationUpdate       // Configuration change
};

// Notification data structure - can be extended for different event types
struct Notification {
    NotificationType type;
    std::string sourceSystem;
    std::chrono::system_clock::time_point timestamp;
    std::string data;  // Generic data field (JSON, serialized, or structured)
    
    Notification(NotificationType t, const std::string& source)
        : type(t), sourceSystem(source), 
          timestamp(std::chrono::system_clock::now()), data() {}
    
    Notification(NotificationType t, const std::string& source, const std::string& eventData)
        : type(t), sourceSystem(source), 
          timestamp(std::chrono::system_clock::now()), data(eventData) {}
};

// Observer interface - all observers must implement these methods
class Observer {
public:
    virtual ~Observer() = default;
    
    // Called when a notification is received
    virtual void onNotify(const Notification& notification) = 0;
    
    // Called to initialize the observer with system state
    virtual void onInitialize() {}
    
    // Called to clean up the observer
    virtual void onShutdown() {}
    
    // Get the system name that this observer monitors
    virtual std::string getSystemName() const = 0;
    
    // Check if observer is interested in this notification type
    virtual bool isInterested(const NotificationType& type) const = 0;
};

// Subject interface - notifies all registered observers
class Subject {
public:
    virtual ~Subject() = default;
    
    // Attach an observer to this subject
    virtual void attach(std::shared_ptr<Observer> observer) = 0;
    
    // Detach an observer from this subject
    virtual void detach(const std::shared_ptr<Observer>& observer) = 0;
    
    // Notify all observers about an event
    virtual void notify(const Notification& notification) = 0;
    
    // Get the system name that this subject manages
    virtual std::string getSystemName() const = 0;
    
    // Get list of registered observers
    virtual std::vector<std::shared_ptr<Observer>> getObservers() const = 0;
};

// Template utility for notification data with type safety
class NotificationHelper {
public:
    template<typename T>
    static std::string serializeData(const T& data) {
        // In a real implementation, this would use proper serialization
        // For now, we just return a string representation
        std::ostringstream oss;
        oss << data;
        return oss.str();
    }
    
    template<typename T>
    static T deserializeData(const std::string& data) {
        // In a real implementation, this would use proper deserialization
        // For now, we'll throw an exception as this is not implemented
        throw std::runtime_error("Deserialization not implemented for generic type");
    }
};

// Macro for easy notification creation
#define CREATE_NOTIFICATION(type, source, data) \
    Notification(type, source, data)

// Macro for easy observer registration
#define REGISTER_OBSERVER(subject, observer) \
    subject->attach(std::make_shared<observer>())

} // namespace nlm
