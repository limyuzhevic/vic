//! Event System for NLM Neural Simulation Project
//! 
//! This module implements the EventSystem class which provides event-driven communication
//! between plugins and core NLM systems. The EventSystem enables:
//! - Inter-plugin communication through events
//! - Event broadcasting to all registered listeners
//! - Event filtering and routing
//! - Event prioritization and ordering
//! - Event history and replay
//! - Event subscriptions and unsubscriptions
//! - Event statistics and monitoring
//! 
//! The EventSystem uses an observer pattern to enable decoupled communication
//! between plugins, allowing plugins to react to events without direct dependencies.

#pragma once

#include "PluginBase.hpp"
#include <unordered_map>
#include <vector>
#include <memory>
#include <string>
#include <functional>
#include <mutex>
#include <any>
#include <deque>
#include <chrono>
#include <algorithm>
#include <unordered_set>
#include <set>
#include <typeindex>

namespace nlm {

class EventSystem {
public:
    EventSystem();
    ~EventSystem();
    
    // Disable copying
    EventSystem(const EventSystem&) = delete;
    EventSystem& operator=(const EventSystem&) = delete;
    
    // Enable moving
    EventSystem(EventSystem&&) noexcept = default;
    EventSystem& operator=(EventSystem&&) noexcept = default;
    
    // Event listener management
    bool subscribeEvent(const std::string& eventType, std::shared_ptr<IEventListener> listener,
                      const std::string& pluginName = "", int priority = 0);
    bool subscribeEvent(PluginEvent event, std::shared_ptr<IEventListener> listener,
                      const std::string& pluginName = "", int priority = 0);
    bool subscribeEvent(PluginHook hook, std::shared_ptr<IEventListener> listener,
                      const std::string& pluginName = "", int priority = 0);
    bool unsubscribeEvent(const std::string& eventType, const std::string& listenerName = "");
    bool unsubscribeEvent(PluginEvent event, const std::string& listenerName = "");
    bool unsubscribeEvent(PluginHook hook, const std::string& listenerName = "");
    bool unsubscribeAllEvents(const std::string& listenerName);
    
    // Event broadcasting
    void emitEvent(const PluginEventData& event);
    void emitEvent(PluginEvent event, const std::string& pluginName,
                  const std::unordered_map<std::string, std::any>& data = {});
    void emitEvent(PluginHook hook, const std::string& pluginName,
                  const std::unordered_map<std::string, std::any>& data = {});
    void broadcastEvent(const PluginEventData& event, bool includeSystemListeners = true);
    
    // Event history and replay
    std::deque<PluginEventData> getEventHistory(size_t maxEvents = 1000) const;
    std::deque<PluginEventData> getEventHistory(const std::string& pluginName, size_t maxEvents = 100) const;
    std::deque<PluginEventData> getEventHistory(PluginEvent event, size_t maxEvents = 100) const;
    std::deque<PluginEventData> getEventHistory(PluginHook hook, size_t maxEvents = 100) const;
    void clearEventHistory();
    void clearEventHistory(const std::string& pluginName);
    
    // Event filtering and routing
    std::vector<std::shared_ptr<IEventListener>> getEventListeners(const std::string& eventType) const;
    std::vector<std::shared_ptr<IEventListener>> getEventListeners(PluginEvent event) const;
    std::vector<std::shared_ptr<IEventListener>> getEventListeners(PluginHook hook) const;
    std::vector<std::shared_ptr<IEventListener>> getEventListeners(const std::string& pluginName) const;
    std::vector<std::shared_ptr<IEventListener>> getEventListeners(const std::string& pluginName, PluginEvent event) const;
    
    // Event filtering and routing utilities
    std::vector<std::shared_ptr<IEventListener>> filterEventListeners(const std::function<bool(const std::shared_ptr<IEventListener>&)>& predicate) const;
    std::vector<PluginEventData> filterEvents(const std::function<bool(const PluginEventData&)>& predicate) const;
    
    // Event listener information
    std::vector<std::string> getAllEventTypes() const;
    std::vector<std::string> getAllListenerNames() const;
    bool hasEventListeners(const std::string& eventType) const;
    bool hasEventListeners(PluginEvent event) const;
    bool hasEventListeners(PluginHook hook) const;
    bool hasEventListeners(const std::string& pluginName) const;
    
    // Event listener priority management
    int getEventListenerPriority(const std::string& listenerName, const std::string& eventType) const;
    bool setEventListenerPriority(const std::string& listenerName, const std::string& eventType, int priority);
    int getEventListenerPriority(const std::string& listenerName, PluginEvent event) const;
    bool setEventListenerPriority(const std::string& listenerName, PluginEvent event, int priority);
    int getEventListenerPriority(const std::string& listenerName, PluginHook hook) const;
    bool setEventListenerPriority(const std::string& listenerName, PluginHook hook, int priority);
    
    // Event listener management
    std::chrono::system_clock::time_point getEventListenerRegistrationTime(const std::string& listenerName) const;
    size_t getEventListenerEventCount(const std::string& listenerName) const;
    std::unordered_map<std::string, size_t> getListenerEventCounts(const std::string& listenerName) const;
    
    // Event routing
    bool routeEventToListeners(const PluginEventData& event,
                             const std::vector<std::shared_ptr<IEventListener>>& listeners);
    bool routeHookEventToListeners(const HookEventData& hook,
                                 const std::vector<std::shared_ptr<IEventListener>>& listeners);
    
    // Event statistics
    struct EventStats {
        std::string eventType;
        size_t totalEvents;
        size_t successfulEvents;
        size_t failedEvents;
        double averageProcessingTime;
        std::chrono::system_clock::time_point firstEventTime;
        std::chrono::system_clock::time_point lastEventTime;
        
        EventStats() : eventType(""), totalEvents(0), successfulEvents(0),
            failedEvents(0), averageProcessingTime(0.0),
            firstEventTime(std::chrono::system_clock::now()),
            lastEventTime(std::chrono::system_clock::now()) {}
    };
    
    EventStats getEventStats(const std::string& eventType) const;
    std::vector<EventStats> getAllEventStats() const;
    
    // Event system statistics
    struct SystemEventStats {
        size_t totalEvents;
        size_t totalListeners;
        size_t totalListenersByEvent;
        double systemUptime;
        size_t activeEventTypes;
        size_t activeHookTypes;
        
        SystemEventStats() : totalEvents(0), totalListeners(0), totalListenersByEvent(0),
            systemUptime(0.0), activeEventTypes(0), activeHookTypes(0) {}
    };
    
    SystemEventStats getSystemEventStats() const;
    
    // Event filtering and querying
    std::vector<PluginEventData> getEventsSince(const std::chrono::system_clock::time_point& since) const;
    std::vector<PluginEventData> getEventsByPlugin(const std::string& pluginName) const;
    std::vector<PluginEventData> getEventsByEventType(const std::string& eventType) const;
    std::vector<PluginEventData> getEventsByEvent(PluginEvent event) const;
    std::vector<PluginEventData> getEventsByHook(PluginHook hook) const;
    
    // Event replay and simulation
    void replayEvents(const std::vector<PluginEventData>& events);
    void replayEvents(const std::vector<PluginEventData>& events,
                     const std::function<void(const PluginEventData&)>& callback);
    void simulateEvents(const std::vector<PluginEventData>& events,
                       std::vector<PluginEventData>& outputEvents);
    
    // Event listener lifecycle
    void pauseAllEventListeners();
    void resumeAllEventListeners();
    void shutdownAllEventListeners();
    bool areEventListenersActive() const;
    
    // Event system configuration
    void setMaxEventHistorySize(size_t maxHistory);
    size_t getMaxEventHistorySize() const;
    void setEventProcessingTimeout(double timeout);
    double getEventProcessingTimeout() const;
    void setEnableEventReplay(bool enable);
    bool isEventReplayEnabled() const;
    void setAsyncEventProcessing(bool enable);
    bool isAsyncEventProcessingEnabled() const;
    
    // Event system integration
    void setPluginManager(std::shared_ptr<PluginManager> pluginManager);
    void setRegistry(std::shared_ptr<PluginRegistry> registry);
    void setSecurityManager(std::shared_ptr<PluginSecurityManager> securityManager);
    void setUpdateManager(std::shared_ptr<PluginUpdateManager> updateManager);
    void setHookSystem(std::shared_ptr<HookSystem> hookSystem);
    void setFactory(std::shared_ptr<ComponentFactory> factory);
    
    // Event system debugging and monitoring
    void enableDebugLogging(bool enable);
    bool isDebugLoggingEnabled() const;
    void logEventEmission(const PluginEventData& event, bool success, const std::string& error = "") const;
    void logEventListenerRegistration(const std::string& listenerName, const std::string& eventType, bool success) const;
    void logEventSystemError(const std::string& error) const;
    
    // Event system optimization
    void optimizeEventRouting();
    void consolidateEventListeners();
    void optimizeEventHistory();
    void balanceEventPriorities();
    
    // Event system export/import
    bool exportEventConfiguration(const std::string& filePath) const;
    bool importEventConfiguration(const std::string& filePath);
    std::string exportEventConfigurationAsJSON() const;
    bool importEventConfigurationFromJSON(const std::string& jsonData);
    
    // Event system testing
    bool testAllEventListeners() const;
    std::vector<std::string> getEventListenerTestResults() const;
    bool testEventListener(const std::string& listenerName, const std::string& eventType) const;
    
    // Event system statistics collection
    void updateEventStatistics();
    void resetEventStatistics();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    void initializeImpl();
    void cleanupImpl();
    
    // Thread-safe operations
    template<typename T>
    T withEventLock(T func) {
        std::unique_lock<std::mutex> lock(eventMutex_);
        return func();
    }
    
    // Event listener comparison for priority sorting
    static bool compareEventListenersByPriority(const std::shared_ptr<IEventListener>& a,
                                               const std::shared_ptr<IEventListener>& b) {
        // Need to access priority, which is stored elsewhere
        return false; // Placeholder
    }
    
    // Event routing optimization
    void optimizeEventListenerRouting();
    void consolidateDuplicateEventListeners();
    
    // Event history management
    void trimEventHistory();
    
    // Event processing
    void processEvent(const PluginEventData& event);
    void processHookEvent(const HookEventData& hook);
    
    // Event listener validation
    bool validateEventListener(const std::shared_ptr<IEventListener>& listener) const;
    bool validateEvent(const PluginEventData& event) const;
    
    // Event statistics updates
    void updateEventStats(const PluginEventData& event, bool success, double processingTime);
    void updateHookStats(const HookEventData& hook, bool success, double processingTime);
    
    // Event cleanup
    void cleanupInactiveEventListeners();
}; // namespace nlm