#include "NeuralEventSystem.hpp"
#include "../core/Types/Types.hpp"
#include <algorithm>
#include <chrono>
#include <numeric>

namespace nlm {

struct NeuralEventSystem::Impl {
    std::vector<NeuralEvent> events;
    std::vector<std::pair<EventFilter, EventCallback>> subscriptions;
    std::set<NeuronId> eventGenerators;
    std::map<NeuralEventType, EventPriority> eventPriorities;
    std::map<NeuralEventType, size_t> processedEventCounts;
    std::map<NeuralEventType, size_t> unprocessedEventCounts;
    
    size_t eventLimit;
    bool eventProcessingEnabled;
    bool processingEvents;
    size_t nextCallbackIndex;
    
    Impl() : eventLimit(10000), eventProcessingEnabled(true), 
             processingEvents(false), nextCallbackIndex(0) {
        // Initialize default event priorities
        for (int i = 0; i < static_cast<int>(NeuralEventType::CustomEvent) + 1; ++i) {
            NeuralEventType type = static_cast<NeuralEventType>(i);
            eventPriorities[type] = EventPriority::Normal;
            processedEventCounts[type] = 0;
            unprocessedEventCounts[type] = 0;
        }
    }
};

NeuralEventSystem::NeuralEventSystem() : pImpl(std::make_unique<Impl>()) {}
NeuralEventSystem::~NeuralEventSystem() = default;

void NeuralEventSystem::generateEvent(const NeuralEvent& event) {
    if (!eventProcessingEnabled) return;
    
    pImpl->events.push_back(event);
    pImpl->unprocessedEventCounts[event.type]++;
    
    // Update statistics
    if (pImpl->events.size() > pImpl->eventLimit) {
        pImpl->events.erase(pImpl->events.begin());
    }
}

void NeuralEventSystem::generateEvent(NeuralEventType type, double timestamp, 
                                     int source, int target, float data, 
                                     const std::string& desc) {
    NeuralEvent event(type, timestamp, source, target, data, desc);
    generateEvent(event);
}

void NeuralEventSystem::processEvents() {
    if (!eventProcessingEnabled || pImpl->events.empty()) return;
    
    std::vector<NeuralEvent> unprocessedEvents;
    
    // Separate processed and unprocessed events
    for (const auto& event : pImpl->events) {
        if (!event.isProcessed) {
            unprocessedEvents.push_back(event);
        }
    }
    
    // Sort by priority
    std::sort(unprocessedEvents.begin(), unprocessedEvents.end(),
              [this](const NeuralEvent& a, const NeuralEvent& b) {
                  EventPriority priorityA = pImpl->eventPriorities[a.type];
                  EventPriority priorityB = pImpl->eventPriorities[b.type];
                  return priorityA > priorityB;  // Higher priority first
              });
    
    // Process events
    pImpl->processingEvents = true;
    for (auto& event : unprocessedEvents) {
        processEvent(event);
    }
    pImpl->processingEvents = false;
    
    // Clear processed events
    pImpl->events.clear();
    for (auto& [type, count] : pImpl->unprocessedEventCounts) {
        count = 0;
    }
}

void NeuralEventSystem::processEvent(const NeuralEvent& event) {
    bool processed = false;
    
    // Notify all subscribed callbacks
    for (const auto& [filter, callback] : pImpl->subscriptions) {
        if (filter.matches(event)) {
            callback(event, processed);
        }
    }
    
    // Update event state
    NeuralEvent mutableEvent = event;
    mutableEvent.isProcessed = true;
    
    // Replace event in events list
    auto it = std::find(pImpl->events.begin(), pImpl->events.end(), event);
    if (it != pImpl->events.end()) {
        *it = mutableEvent;
    }
    
    // Update statistics
    pImpl->unprocessedEventCounts[event.type]--;
    pImpl->processedEventCounts[event.type]++;
}

void NeuralEventSystem::subscribe(const EventFilter& filter, EventCallback callback) {
    pImpl->subscriptions.push_back({filter, std::move(callback)});
}

void NeuralEventSystem::unsubscribe(EventCallback callback) {
    pImpl->subscriptions.erase(
        std::remove_if(pImpl->subscriptions.begin(), pImpl->subscriptions.end(),
                     [callback](const auto& sub) { return sub.callback == callback; }),
        pImpl->subscriptions.end()
    );
}

void NeuralEventSystem::unsubscribeAll() {
    pImpl->subscriptions.clear();
}

std::vector<NeuralEvent> NeuralEventSystem::getEvents(const EventFilter& filter) const {
    std::vector<NeuralEvent> result;
    for (const auto& event : pImpl->events) {
        if (filter.matches(event)) {
            result.push_back(event);
        }
    }
    return result;
}

std::vector<NeuralEvent> NeuralEventSystem::getUnprocessedEvents() const {
    std::vector<NeuralEvent> result;
    for (const auto& event : pImpl->events) {
        if (!event.isProcessed) {
            result.push_back(event);
        }
    }
    return result;
}

size_t NeuralEventSystem::getEventCount() const {
    return pImpl->events.size();
}

void NeuralEventSystem::clearEvents() {
    pImpl->events.clear();
    for (auto& [type, count] : pImpl->unprocessedEventCounts) {
        count = 0;
    }
}

void NeuralEventSystem::setEventLimit(size_t limit) {
    pImpl->eventLimit = limit;
    if (pImpl->events.size() > limit) {
        pImpl->events.erase(pImpl->events.begin(),
                          pImpl->events.begin() + (pImpl->events.size() - limit));
    }
}

size_t NeuralEventSystem::getEventLimit() const {
    return pImpl->eventLimit;
}

void NeuralEventSystem::addEventGenerator(NeuronId neuronId) {
    pImpl->eventGenerators.insert(neuronId);
}

void NeuralEventSystem::removeEventGenerator(NeuronId neuronId) {
    pImpl->eventGenerators.erase(neuronId);
}

std::set<NeuronId> NeuralEventSystem::getEventGenerators() const {
    return pImpl->eventGenerators;
}

void NeuralEventSystem::setEventPriority(NeuralEventType type, EventPriority priority) {
    pImpl->eventPriorities[type] = priority;
}

EventPriority NeuralEventSystem::getEventPriority(NeuralEventType type) const {
    return pImpl->eventPriorities[type];
}

size_t NeuralEventSystem::getProcessedEventCount(NeuralEventType type) const {
    return pImpl->processedEventCounts[type];
}

size_t NeuralEventSystem::getUnprocessedEventCount(NeuralEventType type) const {
    return pImpl->unprocessedEventCounts[type];
}

void NeuralEventSystem::integrateWithEventSources() {
    // This would integrate with various event sources like neuron dynamics,
    // plasticity rules, memory systems, etc.
    // For now, it's a placeholder for future implementation
}

void NeuralEventSystem::integrateWithEventSinks() {
    // This would integrate with event sinks like action selection,
    // learning systems, output systems, etc.
    // For now, it's a placeholder for future implementation
}

void NeuralEventSystem::enableEventProcessing(bool enabled) {
    pImpl->eventProcessingEnabled = enabled;
}

bool NeuralEventSystem::isEventProcessingEnabled() const {
    return pImpl->eventProcessingEnabled;
}

std::string NeuralEventSystem::getEventSystemStatus() const {
    std::ostringstream oss;
    oss << "Neural Event System Status:\n";
    oss << "  Events processed: " << std::accumulate(pImpl->processedEventCounts.begin(), 
                                                    pImpl->processedEventCounts.end(), 0) << "\n";
    oss << "  Events pending: " << std::accumulate(pImpl->unprocessedEventCounts.begin(), 
                                                pImpl->unprocessedEventCounts.end(), 0) << "\n";
    oss << "  Event generators: " << pImpl->eventGenerators.size() << "\n";
    oss << "  Subscriptions: " << pImpl->subscriptions.size() << "\n";
    oss << "  Event processing: " << (eventProcessingEnabled ? "Enabled" : "Disabled") << "\n";
    return oss.str();
}

} // namespace nlm
