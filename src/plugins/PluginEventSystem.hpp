#pragma once

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <mutex>
#include <unordered_map>
#include <functional>
#include <chrono>
#include <atomic>

namespace nlm {
namespace plugins {

// Event system types and utilities
enum class PluginEventPriority {
    LOW,
    NORMAL,
    HIGH,
    CRITICAL
};

enum class EventDeliveryMode {
    SYNCHRONOUS,
    ASYNCHRONOUS,
    QUEUED
};

// Event metadata
struct EventMetadata {
    std::string eventId;
    double timestamp;
    std::string source;
    PluginEventPriority priority;
    EventDeliveryMode deliveryMode;
    std::map<std::string, std::string> attributes;
    
    EventMetadata(const std::string& id, const std::string& src)
        : eventId(id), timestamp(0.0), source(src), priority(PluginEventPriority::NORMAL), 
          deliveryMode(EventDeliveryMode::SYNCHRONOUS) {}
};

// Event filter
class EventFilter {
public:
    virtual ~EventFilter() = default;
    virtual bool matches(const EventMetadata& metadata, const std::map<std::string, std::string>& data) const = 0;
    virtual std::string getFilterName() const = 0;
};

// Simple event filter implementation
class SimpleEventFilter : public EventFilter {
public:
    SimpleEventFilter(PluginEventPriority minPriority = PluginEventPriority::LOW,
                     const std::vector<std::string>& eventTypes = {});
    bool matches(const EventMetadata& metadata, const std::map<std::string, std::string>& data) const override;
    std::string getFilterName() const override;
    
    void setMinPriority(PluginEventPriority priority);
    void setAllowedEventTypes(const std::vector<std::string>& eventTypes);
    
private:
    PluginEventPriority minPriority_;
    std::vector<std::string> allowedEventTypes_;
};

// Event handler
using EventHandler = std::function<void(const EventMetadata& metadata, const std::map<std::string, std::string>& data)>;

// Event queue for asynchronous processing
class EventQueue {
public:
    struct QueuedEvent {
        EventMetadata metadata;
        std::map<std::string, std::string> data;
        EventHandler handler;
        std::chrono::steady_clock::time_point enqueueTime;
        
        QueuedEvent(const EventMetadata& meta, const std::map<std::string, std::string>& d, const EventHandler& h)
            : metadata(meta), data(d), handler(h), enqueueTime(std::chrono::steady_clock::now()) {}
    };
    
    EventQueue(size_t maxQueueSize = 1000);
    ~EventQueue();
    
    // Queue management
    void enqueue(const EventMetadata& metadata, const std::map<std::string, std::string>& data, const EventHandler& handler);
    bool dequeue(QueuedEvent& event);
    size_t getQueueSize() const;
    bool isFull() const;
    void clear();
    
    // Thread safety
    void lock() const;
    void unlock() const;
    bool tryLock() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Main event system
class PluginEventSystem {
public:
    PluginEventSystem();
    ~PluginEventSystem();
    
    // Event publishing
    std::string publishEvent(PluginEventType type, const std::string& pluginName, 
                           const std::map<std::string, std::string>& data = {});
    std::string publishEvent(const EventMetadata& metadata, const std::map<std::string, std::string>& data);
    
    // Event subscription
    using EventCallback = std::function<void(const EventMetadata& metadata, const std::map<std::string, std::string>& data)>;
    std::string subscribeToEvent(PluginEventType eventType, const EventCallback& callback,
                                PluginEventPriority priority = PluginEventPriority::NORMAL,
                                EventDeliveryMode deliveryMode = EventDeliveryMode::SYNCHRONOUS);
    std::string subscribeToEvent(const std::string& eventName, const EventCallback& callback,
                                PluginEventPriority priority = PluginEventPriority::NORMAL,
                                EventDeliveryMode deliveryMode = EventDeliveryMode::SYNCHRONOUS);
    bool unsubscribeFromEvent(const std::string& subscriptionId);
    bool unsubscribeFromEvent(PluginEventType eventType, const EventCallback& callback);
    
    // Plugin-specific event subscription
    std::string subscribeToPluginEvent(const std::string& pluginName, PluginEventType eventType, 
                                      const EventCallback& callback,
                                      PluginEventPriority priority = PluginEventPriority::NORMAL);
    bool unsubscribeFromPluginEvent(const std::string& pluginName, PluginEventType eventType,
                                   const EventCallback& callback);
    
    // Event filtering and routing
    void addEventFilter(const std::shared_ptr<EventFilter>& filter);
    void removeEventFilter(const EventFilter* filter);
    void addEventRouter(const std::string& eventName, const EventHandler& router);
    void removeEventRouter(const std::string& eventName);
    
    // Event history and statistics
    std::vector<std::pair<EventMetadata, std::map<std::string, std::string>>> getEventHistory(PluginEventType eventType, 
                                                                                          size_t limit = 100) const;
    std::vector<std::pair<EventMetadata, std::map<std::string, std::string>>> getPluginEventHistory(const std::string& pluginName,
                                                                                               PluginEventType eventType, size_t limit = 100) const;
    void clearEventHistory();
    
    // Event statistics
    size_t getEventCount(PluginEventType eventType) const;
    double getAverageEventInterval(PluginEventType eventType) const;
    std::map<PluginEventType, size_t> getEventStatistics() const;
    
    // Event prioritization
    void setEventPriority(const std::string& eventName, PluginEventPriority priority);
    PluginEventPriority getEventPriority(const std::string& eventName) const;
    void setEventDeliveryMode(const std::string& eventName, EventDeliveryMode mode);
    EventDeliveryMode getEventDeliveryMode(const std::string& eventName) const;
    
    // Event metadata
    EventMetadata getEventMetadata(const std::string& eventId) const;
    bool updateEventMetadata(const std::string& eventId, const EventMetadata& metadata);
    
    // Thread safety
    void lock() const;
    void unlock() const;
    bool tryLock() const;
    
    // Configuration
    void setMaxQueueSize(size_t maxSize);
    size_t getMaxQueueSize() const;
    void setAsyncProcessingEnabled(bool enabled);
    bool isAsyncProcessingEnabled() const;
    
    // Cleanup
    void flushEventQueue();
    void cleanupOldEvents(double maxAgeSeconds);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Event utilities
namespace EventUtils {
    std::string generateEventId();
    EventMetadata createEventMetadata(PluginEventType type, const std::string& pluginName);
    std::string eventTypeToString(PluginEventType type);
    PluginEventType stringToEventType(const std::string& type);
    
    // Event data utilities
    template<typename T>
    void setEventData(std::map<std::string, std::string>& data, const std::string& key, const T& value);
    
    template<typename T>
    T getEventData(const std::map<std::string, std::string>& data, const std::string& key, const T& defaultValue);
    
    std::vector<std::string> getAllEventKeys(const std::map<std::string, std::string>& data);
    bool hasEventData(const std::map<std::string, std::string>& data, const std::string& key);
    void removeEventData(std::map<std::string, std::string>& data, const std::string& key);
    void clearEventData(std::map<std::string, std::string>& data);
}

} // namespace plugins
} // namespace nlm
