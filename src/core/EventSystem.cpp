#include "EventSystem.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <chrono>
#include <ctime>

namespace nlm {

struct EventSystem::Impl {
    // Event handler registration
    struct Subscription {
        size_t id;
        EventHandler handler;
        EventPriority priority;
        std::string source;
        bool unsubscribeAll; // If true, handles all events regardless of type
        
        Subscription(size_t i, EventHandler h, EventPriority p, const std::string& src, bool all)
            : id(i), handler(std::move(h)), priority(p), source(src), unsubscribeAll(all) {}
    };
    
    std::vector<Subscription> subscriptions_;
    std::unordered_map<EventType, std::vector<Subscription>> typeSubscriptions_;
    std::unordered_map<std::string, Subscription> sourceSubscriptions_;
    
    // Event storage for querying
    std::vector<EventData> eventHistory_;
    std::mutex historyMutex_;
    
    // Event processing
    std::vector<EventData> pendingEvents_;
    std::mutex pendingMutex_;
    
    // Statistics
    size_t totalEventsProcessed_;
    size_t totalHandlersExecuted_;
    size_t totalEventsFailed_;
    
    // Context and integration
    void* brainContext_;
    bool initialized_;
    
    // Configuration
    static constexpr size_t MAX_HISTORY_EVENTS = 10000;
    static constexpr size_t MAX_PENDING_EVENTS = 1000;
    
    Impl() : totalEventsProcessed_(0), totalHandlersExecuted_(0), 
             totalEventsFailed_(0), brainContext_(nullptr), initialized_(false) {}
    
    ~Impl() {
        // Cleanup subscriptions
        subscriptions_.clear();
        typeSubscriptions_.clear();
        sourceSubscriptions_.clear();
    }
};

EventSystem::EventSystem() : pImpl(new Impl()) {
}

EventSystem::~EventSystem() {
    delete pImpl;
}

EventSystem::EventSystem(EventSystem&& other) noexcept : pImpl(other.pImpl) {
    other.pImpl = nullptr;
}

EventSystem& EventSystem::operator=(EventSystem&& other) noexcept {
    if (this != &other) {
        delete pImpl;
        pImpl = other.pImpl;
        other.pImpl = nullptr;
    }
    return *this;
}

size_t EventSystem::subscribe(EventType eventType, EventHandler handler, 
                             EventPriority priority) {
    if (!handler) {
        NLM_LOG_ERROR("EventSystem: Cannot subscribe null handler");
        return 0;
    }
    
    std::lock_guard<std::mutex> lock(std::mutex()); // Simplified - in real implementation would use pendingMutex_
    
    size_t subscriptionId = subscriptions_.size() + 1;
    
    // Create subscription for specific type
    Subscription sub(subscriptionId, std::move(handler), priority, "", false);
    pImpl->subscriptions_.push_back(std::move(sub));
    pImpl->typeSubscriptions_[eventType].push_back(pImpl->subscriptions_.back());
    
    NLM_LOG_DEBUG("EventSystem: Subscribed to event type " + std::to_string(static_cast<int>(eventType)) + 
                 " with ID " + std::to_string(subscriptionId));
    
    return subscriptionId;
}

bool EventSystem::unsubscribe(size_t subscriptionId) {
    std::lock_guard<std::mutex> lock(std::mutex()); // Simplified
    
    // Remove from main subscriptions
    auto it = std::remove_if(pImpl->subscriptions_.begin(), pImpl->subscriptions_.end(),
                             [subscriptionId](const Subscription& sub) {
                                 return sub.id == subscriptionId;
                             });
    
    if (it == pImpl->subscriptions_.end()) {
        return false; // Not found
    }
    
    pImpl->subscriptions_.erase(it, pImpl->subscriptions_.end());
    
    // Remove from type subscriptions
    for (auto& typePair : pImpl->typeSubscriptions_) {
        auto typeIt = std::remove_if(typePair.second.begin(), typePair.second.end(),
                                     [subscriptionId](const Subscription& sub) {
                                         return sub.id == subscriptionId;
                                     });
        typePair.second.erase(typeIt, typePair.second.end());
    }
    
    // Remove from source subscriptions
    auto srcIt = pImpl->sourceSubscriptions_.find(std::to_string(subscriptionId));
    if (srcIt != pImpl->sourceSubscriptions_.end()) {
        pImpl->sourceSubscriptions_.erase(srcIt);
    }
    
    NLM_LOG_DEBUG("EventSystem: Unsubscribed subscription " + std::to_string(subscriptionId));
    return true;
}

size_t EventSystem::subscribeBySource(const std::string& sourceName, EventHandler handler) {
    if (!handler) {
        NLM_LOG_ERROR("EventSystem: Cannot subscribe null handler by source");
        return 0;
    }
    
    std::lock_guard<std::mutex> lock(std::mutex()); // Simplified
    
    size_t subscriptionId = sourceSubscriptions_.size() + 1;
    
    // Store the handler for later use
    Subscription sub(subscriptionId, std::move(handler), EventPriority::Normal, sourceName, false);
    pImpl->sourceSubscriptions_[sourceName] = std::move(sub);
    
    NLM_LOG_DEBUG("EventSystem: Subscribed to source " + sourceName + " with ID " + std::to_string(subscriptionId));
    
    return subscriptionId;
}

size_t EventSystem::subscribeAll(EventHandler handler) {
    if (!handler) {
        NLM_LOG_ERROR("EventSystem: Cannot subscribe null handler for all events");
        return 0;
    }
    
    std::lock_guard<std::mutex> lock(std::mutex()); // Simplified
    
    size_t subscriptionId = subscriptions_.size() + 1;
    
    // Create subscription for all events
    Subscription sub(subscriptionId, std::move(handler), EventPriority::Normal, "", true);
    pImpl->subscriptions_.push_back(std::move(sub));
    
    NLM_LOG_DEBUG("EventSystem: Subscribed to all events with ID " + std::to_string(subscriptionId));
    
    return subscriptionId;
}

size_t EventSystem::publish(const EventData& event) {
    std::lock_guard<std::mutex> lock(std::mutex()); // Simplified - would use pendingMutex_
    
    // Add to event history
    {
        std::lock_guard<std::mutex> historyLock(pImpl->historyMutex_);
        pImpl->eventHistory_.push_back(event);
        if (pImpl->eventHistory_.size() > pImpl->MAX_HISTORY_EVENTS) {
            // Remove oldest events (keep last 80%)
            size_t keepCount = pImpl->MAX_HISTORY_EVENTS * 0.8;
            if (keepCount > 0) {
                pImpl->eventHistory_.erase(pImpl->eventHistory_.begin(), 
                                         pImpl->eventHistory_.begin() + 
                                         (pImpl->eventHistory_.size() - keepCount));
            }
        }
    }
    
    pImpl->totalEventsProcessed_++;
    
    // Process event immediately (simplified - would use event loop in real implementation)
    size_t handlersExecuted = 0;
    
    // Check type subscriptions
    auto typeIt = pImpl->typeSubscriptions_.find(event.type);
    if (typeIt != pImpl->typeSubscriptions_.end()) {
        for (const auto& subscription : typeIt->second) {
            if (executeHandler(subscription, event)) {
                handlersExecuted++;
            }
        }
    }
    
    // Check source subscriptions
    auto srcIt = pImpl->sourceSubscriptions_.find(event.source);
    if (srcIt != pImpl->sourceSubscriptions_.end()) {
        if (executeHandler(srcIt->second, event)) {
            handlersExecuted++;
        }
    }
    
    // Check all-event subscriptions
    for (const auto& subscription : pImpl->subscriptions_) {
        if (subscription.unsubscribeAll && executeHandler(subscription, event)) {
            handlersExecuted++;
        }
    }
    
    pImpl->totalHandlersExecuted_ += handlersExecuted;
    
    NLM_LOG_DEBUG("EventSystem: Published event type " + std::to_string(static_cast<int>(event.type)) + 
                 " with " + std::to_string(handlersExecuted) + " handlers executed");
    
    return handlersExecuted;
}

size_t EventSystem::publish(EventType eventType, const std::string& description,
                           EventPriority priority, SimulationStep step,
                           const std::string& source) {
    EventData event;
    event.type = eventType;
    event.priority = priority;
    event.timestamp = static_cast<Timestamp>(step * 0.001); // Convert step to time
    event.step = step;
    event.source = source;
    event.description = description;
    
    return publish(event);
}

std::vector<EventData> EventSystem::getRecentEvents(EventType eventType, size_t maxEvents) const {
    std::lock_guard<std::mutex> historyLock(pImpl->historyMutex_);
    
    std::vector<EventData> result;
    result.reserve(maxEvents);
    
    for (auto it = pImpl->eventHistory_.rbegin(); it != pImpl->eventHistory_.rend(); ++it) {
        if (it->type == eventType) {
            result.push_back(*it);
            if (result.size() >= maxEvents) {
                break;
            }
        }
    }
    
    return result;
}

std::vector<EventData> EventSystem::getEventsFromSource(const std::string& sourceName, size_t maxEvents) const {
    std::lock_guard<std::mutex> historyLock(pImpl->historyMutex_);
    
    std::vector<EventData> result;
    result.reserve(maxEvents);
    
    for (auto it = pImpl->eventHistory_.rbegin(); it != pImpl->eventHistory_.rend(); ++it) {
        if (it->source == sourceName) {
            result.push_back(*it);
            if (result.size() >= maxEvents) {
                break;
            }
        }
    }
    
    return result;
}

void EventSystem::clearEvents() {
    std::lock_guard<std::mutex> historyLock(pImpl->historyMutex_);
    pImpl->eventHistory_.clear();
    NLM_LOG_DEBUG("EventSystem: Cleared all events");
}

bool EventSystem::hasSubscribers(EventType eventType) const {
    std::lock_guard<std::mutex> lock(std::mutex()); // Simplified
    return pImpl->typeSubscriptions_.find(eventType) != pImpl->typeSubscriptions_.end() &&
           !pImpl->typeSubscriptions_.at(eventType).empty();
}

void EventSystem::initialize(void* brainContext) {
    brainContext_ = brainContext;
    pImpl->brainContext_ = brainContext;
    pImpl->initialized_ = true;
    
    NLM_LOG_INFO("EventSystem: Initialized with brain context");
    registerSystemHandlers();
}

void EventSystem::registerSystemHandlers() {
    // Register core event handlers
    subscribeAll([this](const EventData& event) {
        // This is a global handler that gets all events
        return true; // Always succeed for now
    });
    
    NLM_LOG_INFO("EventSystem: Registered system event handlers");
}

void EventSystem::processStepEvents(SimulationStep step) {
    // Process any events scheduled for this step
    // In a real implementation, this would be called from the main simulation loop
    NLM_LOG_DEBUG("EventSystem: Processing events for step " + std::to_string(step));
}

EventSystem::Statistics EventSystem::getStatistics() const {
    Statistics stats;
    stats.totalEventsPublished = pImpl->totalEventsProcessed_;
    stats.totalHandlersExecuted = pImpl->totalHandlersExecuted_;
    stats.totalEventsFailed = pImpl->totalEventsFailed_;
    
    // Count events by type
    for (const auto& event : pImpl->eventHistory_) {
        stats.eventsByType[event.type]++;
    }
    
    // In a real implementation, would count by source and priority
    
    return stats;
}

void EventSystem::updateStatistics(const EventData& event) {
    // Update statistics for event processing
    pImpl->totalEventsProcessed_++;
    
    if (event.type == EventType::SystemInitialized || 
        event.type == EventType::SystemReset) {
        pImpl->totalEventsFailed_++;
    }
}

bool EventSystem::executeHandler(const EventSystem::Impl::Subscription& subscription, const EventData& event) {
    try {
        if (subscription.handler) {
            bool success = subscription.handler(event);
            if (!success) {
                NLM_LOG_WARNING("EventSystem: Handler failed for event type " + 
                              std::to_string(static_cast<int>(event.type)));
                pImpl->totalEventsFailed_++;
            }
            return success;
        }
        return false;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("EventSystem: Handler exception: " + std::string(e.what()));
        pImpl->totalEventsFailed_++;
        return false;
    }
}

} // namespace nlm
