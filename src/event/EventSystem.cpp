#include "EventSystem.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <sstream>

namespace nlm {

struct EventSystem::Impl {
    // Event queue
    std::vector<Event> eventQueue;
    
    // Listener maps
    std::unordered_map<EventType, std::vector<std::shared_ptr<EventListener>>> listeners;
    
    // Global listeners (receive all events)
    std::vector<std::shared_ptr<EventListener>> globalListeners;
    
    // Event counters
    std::unordered_map<EventType, uint64_t> eventCounters;
    
    // Processing statistics
    uint64_t totalEventsProcessed;
    uint64_t eventsFiltered;
    
    Impl() : totalEventsProcessed(0), eventsFiltered(0) {}
};

EventSystem::EventSystem() : pImpl(new Impl), maxQueueSize_(10000) {
    NLM_LOG_INFO("Event system initialized");
}

EventSystem::~EventSystem() = default;

void EventSystem::publishEvent(const Event& event) {
    // Check if event is filtered
    if (isEventFiltered(event.type)) {
        pImpl->eventsFiltered++;
        return;
    }
    
    // Check queue size
    if (pImpl->eventQueue.size() >= maxQueueSize_) {
        // Remove oldest events (simple FIFO)
        size_t removeCount = pImpl->eventQueue.size() - maxQueueSize_ + 1;
        pImpl->eventQueue.erase(pImpl->eventQueue.begin(), 
                               pImpl->eventQueue.begin() + removeCount);
    }
    
    // Add event to queue
    pImpl->eventQueue.push_back(event);
    
    // Increment event counter
    pImpl->eventCounters[event.type]++;
    
    // Log event if needed (for debugging)
    NLM_LOG_DEBUG("Event published: " + std::to_string(static_cast<int>(event.type)) + 
                 " from " + event.source + " at step " + std::to_string(event.simulationStep));
}

void EventSystem::subscribe(EventType eventType, std::shared_ptr<EventListener> listener) {
    if (!listener) return;
    
    pImpl->listeners[eventType].push_back(listener);
    
    NLM_LOG_INFO("Listener " + listener->getListenerName() + 
                 " subscribed to event type " + std::to_string(static_cast<int>(eventType)));
}

void EventSystem::unsubscribe(EventType eventType, std::shared_ptr<EventListener> listener) {
    if (!listener) return;
    
    auto& listenersList = pImpl->listeners[eventType];
    listenersList.erase(
        std::remove_if(listenersList.begin(), listenersList.end(),
                      [&listener](const std::shared_ptr<EventListener>& l) {
                          return l.get() == listener.get();
                      }),
        listenersList.end()
    );
}

void EventSystem::unsubscribeAll(std::shared_ptr<EventListener> listener) {
    if (!listener) return;
    
    for (auto& pair : pImpl->listeners) {
        auto& listenersList = pair.second;
        listenersList.erase(
            std::remove_if(listenersList.begin(), listenersList.end(),
                          [&listener](const std::shared_ptr<EventListener>& l) {
                              return l.get() == listener.get();
                          }),
            listenersList.end()
        );
    }
    
    // Also remove from global listeners
    pImpl->globalListeners.erase(
        std::remove_if(pImpl->globalListeners.begin(), pImpl->globalListeners.end(),
                      [&listener](const std::shared_ptr<EventListener>& l) {
                          return l.get() == listener.get();
                      }),
        pImpl->globalListeners.end()
    );
}

void EventSystem::processEvents() {
    if (pImpl->eventQueue.empty()) {
        return;
    }
    
    // Process events in FIFO order
    std::vector<Event> eventsToProcess = std::move(pImpl->eventQueue);
    pImpl->eventQueue.clear();
    
    for (const Event& event : eventsToProcess) {
        // Notify specific listeners for this event type
        auto it = pImpl->listeners.find(event.type);
        if (it != pImpl->listeners.end()) {
            for (const auto& listener : it->second) {
                if (listener) {
                    try {
                        listener->onEvent(event);
                    } catch (const std::exception& e) {
                        NLM_LOG_ERROR("Exception in event listener " + listener->getListenerName() + ": " + std::string(e.what()));
                    }
                }
            }
        }
        
        // Notify global listeners
        for (const auto& listener : pImpl->globalListeners) {
            if (listener) {
                try {
                    listener->onEvent(event);
                } catch (const std::exception& e) {
                    NLM_LOG_ERROR("Exception in global event listener " + listener->getListenerName() + ": " + std::string(e.what()));
                }
            }
        }
        
        pImpl->totalEventsProcessed++;
    }
}

void EventSystem::addGlobalListener(std::shared_ptr<EventListener> listener) {
    if (!listener) return;
    
    pImpl->globalListeners.push_back(listener);
    
    NLM_LOG_INFO("Global listener " + listener->getListenerName() + " added");
}

void EventSystem::removeGlobalListener(std::shared_ptr<EventListener> listener) {
    if (!listener) return;
    
    pImpl->globalListeners.erase(
        std::remove_if(pImpl->globalListeners.begin(), pImpl->globalListeners.end(),
                      [&listener](const std::shared_ptr<EventListener>& l) {
                          return l.get() == listener.get();
                      }),
        pImpl->globalListeners.end()
    );
}

std::vector<EventType> EventSystem::getRegisteredEventTypes() const {
    std::vector<EventType> types;
    types.reserve(pImpl->listeners.size());
    for (const auto& pair : pImpl->listeners) {
        types.push_back(pair.first);
    }
    return types;
}

std::vector<std::string> EventSystem::getRegisteredListeners() const {
    std::vector<std::string> names;
    
    for (const auto& pair : pImpl->listeners) {
        for (const auto& listener : pair.second) {
            if (listener) {
                names.push_back(listener->getListenerName() + " (" + std::to_string(static_cast<int>(pair.first)) + ")");
            }
        }
    }
    
    for (const auto& listener : pImpl->globalListeners) {
        if (listener) {
            names.push_back(listener->getListenerName() + " (GLOBAL)");
        }
    }
    
    return names;
}

void EventSystem::clearEvents() {
    pImpl->eventQueue.clear();
    pImpl->eventsFiltered = 0;
    NLM_LOG_INFO("Event queue cleared");
}

// Event listener implementations

BrainEventListener::BrainEventListener(std::shared_ptr<Brain> brain) : brain_(brain) {
    // Initialization will happen when connectToBrain is called or onEvent is first called
}

BrainEventListener::~BrainEventListener() = default;

void BrainEventListener::onEvent(const Event& event) {
    if (!brain_) return;
    
    // Route event to appropriate brain systems based on type
    switch (event.type) {
        case EventType::NEURON_SPIKE:
            // Handle neuron spike events
            NLM_LOG_DEBUG("BrainEventListener: Neuron spike from " + event.source);
            break;
            
        case EventType::NEURON_FIRE:
            // Handle neuron fire events
            NLM_LOG_DEBUG("BrainEventListener: Neuron fire from " + event.source);
            break;
            
        case EventType::SYNAPSE_WEIGHT_CHANGE:
            // Handle synaptic weight changes
            NLM_LOG_DEBUG("BrainEventListener: Synapse weight change from " + event.source);
            break;
            
        case EventType::MEMORY_STORE:
            // Handle memory storage events
            NLM_LOG_DEBUG("BrainEventListener: Memory store from " + event.source);
            break;
            
        case EventType::EPISODE_CREATED:
            // Handle episode creation events
            NLM_LOG_DEBUG("BrainEventListener: Episode created from " + event.source);
            break;
            
        case EventType::REWARD_RECEIVED:
            // Handle reward events
            NLM_LOG_DEBUG("BrainEventListener: Reward received from " + event.source);
            break;
            
        case EventType::NOVELTY_DETECTED:
            // Handle novelty events
            NLM_LOG_DEBUG("BrainEventListener: Novelty detected from " + event.source);
            break;
            
        case EventType::DEVELOPMENTAL_STAGE_CHANGE:
            // Handle development stage changes
            NLM_LOG_DEBUG("BrainEventListener: Developmental stage change from " + event.source);
            break;
            
        default:
            // Ignore unknown event types
            break;
    }
}

std::vector<EventType> BrainEventListener::getEventTypes() const {
    return {
        EventType::NEURON_SPIKE,
        EventType::NEURON_FIRE,
        EventType::SYNAPSE_WEIGHT_CHANGE,
        EventType::MEMORY_STORE,
        EventType::EPISODE_CREATED,
        EventType::REWARD_RECEIVED,
        EventType::NOVELTY_DETECTED,
        EventType::DEVELOPMENTAL_STAGE_CHANGE
    };
}

std::string BrainEventListener::getListenerName() const {
    return "BrainEventListener";
}

MemoryEventListener::MemoryEventListener(std::shared_ptr<NeuralWorkingMemory> workingMemory,
                                         std::shared_ptr<NeuralEpisodicMemory> episodicMemory)
    : workingMemory_(workingMemory), episodicMemory_(episodicMemory) {
    // Initialization will happen when events are received
}

MemoryEventListener::~MemoryEventListener() = default;

void MemoryEventListener::onEvent(const Event& event) {
    if (!workingMemory_ && !episodicMemory_) return;
    
    switch (event.type) {
        case EventType::MEMORY_STORE:
            // Handle memory store events
            NLM_LOG_DEBUG("MemoryEventListener: Memory store event from " + event.source);
            break;
            
        case EventType::EPISODE_CREATED:
            // Handle episode creation events
            NLM_LOG_DEBUG("MemoryEventListener: Episode created from " + event.source);
            break;
            
        default:
            // Ignore unknown event types
            break;
    }
}

std::vector<EventType> MemoryEventListener::getEventTypes() const {
    return {
        EventType::MEMORY_STORE,
        EventType::EPISODE_CREATED
    };
}

std::string MemoryEventListener::getListenerName() const {
    return "MemoryEventListener";
}

NeuromodulationEventListener::NeuromodulationEventListener(std::shared_ptr<Brain> brain) : brain_(brain) {
    // Initialization will happen when events are received
}

NeuromodulationEventListener::~NeuromodulationEventListener() = default;

void NeuromodulationEventListener::onEvent(const Event& event) {
    if (!brain_) return;
    
    switch (event.type) {
        case EventType::DOPAMINE_RELEASE:
            // Handle dopamine release events
            NLM_LOG_DEBUG("NeuromodulationEventListener: Dopamine release from " + event.source);
            break;
            
        case EventType::ACETYLCHOLINE_RELEASE:
            // Handle acetylcholine release events
            NLM_LOG_DEBUG("NeuromodulationEventListener: Acetylcholine release from " + event.source);
            break;
            
        case EventType::NOREPINEPHRINE_RELEASE:
            // Handle norepinephrine release events
            NLM_LOG_DEBUG("NeuromodulationEventListener: Norepinephrine release from " + event.source);
            break;
            
        case EventType::SEROTONIN_RELEASE:
            // Handle serotonin release events
            NLM_LOG_DEBUG("NeuromodulationEventListener: Serotonin release from " + event.source);
            break;
            
        default:
            // Ignore unknown event types
            break;
    }
}

std::vector<EventType> NeuromodulationEventListener::getEventTypes() const {
    return {
        EventType::DOPAMINE_RELEASE,
        EventType::ACETYLCHOLINE_RELEASE,
        EventType::NOREPINEPHRINE_RELEASE,
        EventType::SEROTONIN_RELEASE
    };
}

std::string NeuromodulationEventListener::getListenerName() const {
    return "NeuromodulationEventListener";
}

CognitiveEventListener::CognitiveEventListener(std::shared_ptr<Brain> brain) : brain_(brain) {
    // Initialization will happen when events are received
}

CognitiveEventListener::~CognitiveEventListener() = default;

void CognitiveEventListener::onEvent(const Event& event) {
    if (!brain_) return;
    
    switch (event.type) {
        case EventType::PLANNING_START:
            // Handle planning start events
            NLM_LOG_DEBUG("CognitiveEventListener: Planning started from " + event.source);
            break;
            
        case EventType::PLANNING_COMPLETE:
            // Handle planning complete events
            NLM_LOG_DEBUG("CognitiveEventListener: Planning completed from " + event.source);
            break;
            
        case EventType::CONCEPT_FORMATION:
            // Handle concept formation events
            NLM_LOG_DEBUG("CognitiveEventListener: Concept formation from " + event.source);
            break;
            
        case EventType::ATTENTION_FOCUS:
            // Handle attention focus events
            NLM_LOG_DEBUG("CognitiveEventListener: Attention focused from " + event.source);
            break;
            
        default:
            // Ignore unknown event types
            break;
    }
}

std::vector<EventType> CognitiveEventListener::getEventTypes() const {
    return {
        EventType::PLANNING_START,
        EventType::PLANNING_COMPLETE,
        EventType::CONCEPT_FORMATION,
        EventType::ATTENTION_FOCUS
    };
}

std::string CognitiveEventListener::getListenerName() const {
    return "CognitiveEventListener";
}

} // namespace nlm
