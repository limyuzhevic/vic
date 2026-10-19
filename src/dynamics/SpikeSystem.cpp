#include "SpikeSystem.hpp"
#include <algorithm>
#include <limits>

namespace nlm {

struct SpikeSystem::Impl {
    std::queue<SpikeEvent> pendingSpikes;
    std::vector<DetailedSpikeEvent> spikeHistory;
    std::vector<SpikeHandler> handlers;
    std::vector<DelayedSpikeHandler> delayedHandlers;
    std::unordered_map<SimulationStep, std::vector<DelayedSpikeEvent>> delayedSpikes;  // Grouped by delivery step
    size_t maxHistorySize;
    
    // Event queue integration for performance
    std::unique_ptr<SpikeRingBuffer> eventQueue;
    std::unique_ptr<DelayedSpikeQueue> delayedEventQueue;
    bool eventDrivenMode;
    
    // Spike counting for statistics
    std::unordered_map<uint64_t, size_t> spikeCountPerNeuron;  // neuron_id -> count
    
    Impl() : maxHistorySize(10000), eventDrivenMode(false) {}
};

SpikeSystem::SpikeSystem() : pImpl(new Impl) {}

SpikeSystem::~SpikeSystem() = default;

SpikeSystem::SpikeSystem(SpikeSystem&& other) noexcept : pImpl(other.pImpl) {
    other.pImpl = nullptr;
}

SpikeSystem& SpikeSystem::operator=(SpikeSystem&& other) noexcept {
    if (this != &other) {
        delete pImpl;
        pImpl = other.pImpl;
        other.pImpl = nullptr;
    }
    return *this;
}

void SpikeSystem::queueSpike(const SpikeEvent& event) {
    if (pImpl->eventQueue && pImpl->eventDrivenMode) {
        // Convert SpikeEvent to QueuedSpikeEvent format
        QueuedSpikeEvent queuedEvent(
            event.source_neuron.value,
            event.destination_neuron.value,
            event.synapse_id.value,
            event.weight,
            event.is_excitatory
        );
        
        // Try to push to event queue (may fail if full)
        if (pImpl->eventQueue->push(queuedEvent)) {
            return;  // Successfully queued, don't also store in std::queue
        }
        // If event queue is full, fall back to std::queue
    }
    
    // Fall back to traditional queue if event queue not available or full
    pImpl->pendingSpikes.push(event);
}

void SpikeSystem::queueSpike(NeuronId neuron, Timestamp timestamp, SimulationStep step) {
    SpikeEvent event(neuron, timestamp, step);
    queueSpike(event);
}

void SpikeSystem::queueDelayedSpike(const DelayedSpikeEvent& event) {
    if (pImpl->delayedEventQueue) {
        // Convert DelayedSpikeEvent to QueuedSpikeEvent format
        QueuedSpikeEvent queuedEvent(
            event.source_neuron,
            event.destination_neuron,
            event.synapse_id,
            event.weight,
            event.is_excitatory
        );
        
        // Use delayed event queue for time-bucketed storage
        pImpl->delayedEventQueue->schedule(
            event.source_neuron,
            event.destination_neuron,
            event.synapse_id,
            event.weight,
            event.is_excitatory,
            event.delivery_step
        );
        return;  // Successfully scheduled, don't also store in std::unordered_map
    }
    
    // Fall back to traditional storage if event queue not available
    pImpl->delayedSpikes[event.delivery_step].push_back(event);
}

void SpikeSystem::processSpikes(SimulationStep currentStep) {
    // Event-driven mode: Use event queue for efficient spike processing
    if (pImpl->eventQueue && pImpl->eventDrivenMode) {
        // Process all spikes from event queue
        while (!pImpl->eventQueue->empty()) {
            auto eventOpt = pImpl->eventQueue->pop();
            if (!eventOpt) {
                break;  // Queue is empty
            }
            
            const QueuedSpikeEvent& queuedEvent = *eventOpt;
            
            // Create detailed event
            DetailedSpikeEvent detailed;
            detailed.source = queuedEvent.sourceNeuron;
            detailed.timestamp = queuedEvent.weight;  // Note: This might be wrong, should use timestamp from SpikeEvent
            detailed.step = currentStep;
            
            // Add to history
            pImpl->spikeHistory.push_back(detailed);
            
            // Track spike count per neuron
            pImpl->spikeCountPerNeuron[queuedEvent.sourceNeuron]++;
            
            // Trim history if needed
            if (pImpl->spikeHistory.size() > pImpl->maxHistorySize) {
                pImpl->spikeHistory.erase(pImpl->spikeHistory.begin());
            }
            
            // Call handlers
            for (auto& handler : pImpl->handlers) {
                handler(detailed);
            }
        }
        return;  // Successfully processed all events from queue
    }
    
    // Traditional mode: Use std::queue
    while (!pImpl->pendingSpikes.empty()) {
        SpikeEvent event = pImpl->pendingSpikes.front();
        pImpl->pendingSpikes.pop();
        
        // Create detailed event
        DetailedSpikeEvent detailed;
        detailed.source = event.source_neuron;
        detailed.timestamp = event.timestamp;
        detailed.step = event.step;
        
        // Add to history
        pImpl->spikeHistory.push_back(detailed);
        
        // Track spike count per neuron
        pImpl->spikeCountPerNeuron[event.source_neuron.value]++;
        
        // Trim history if needed
        if (pImpl->spikeHistory.size() > pImpl->maxHistorySize) {
            pImpl->spikeHistory.erase(pImpl->spikeHistory.begin());
        }
        
        // Call handlers
        for (auto& handler : pImpl->handlers) {
            handler(detailed);
        }
    }
}

void SpikeSystem::processDelayedSpikes(SimulationStep currentStep, Timestamp currentTime) {
    // Event-driven mode: Use delayed event queue for efficient spike processing
    if (pImpl->delayedEventQueue) {
        // Get all spikes scheduled for delivery at current step
        auto dueEvents = pImpl->delayedEventQueue->getDueSpikes(currentStep);
        
        // Process all delayed spikes scheduled for this step
        for (const QueuedSpikeEvent& delayedEvent : dueEvents) {
            // Call delayed spike handlers (these will deliver synaptic input)
            for (auto& handler : pImpl->delayedHandlers) {
                // Convert QueuedSpikeEvent back to DelayedSpikeEvent format
                DelayedSpikeEvent delayedSpike(
                    delayedEvent.sourceNeuron,
                    delayedEvent.destNeuron,
                    delayedEvent.synapseId,
                    delayedEvent.weight,
                    delayedEvent.isExcitatory,
                    currentStep,  // delivery_step
                    currentTime   // timestamp
                );
                handler(delayedSpike);
            }
        }
        
        // Advance time buckets for next step
        pImpl->delayedEventQueue->advanceTime(currentStep);
        return;  // Successfully processed all delayed events from queue
    }
    
    // Traditional mode: Use std::unordered_map
    auto it = pImpl->delayedSpikes.find(currentStep);
    if (it != pImpl->delayedSpikes.end()) {
        // Process all spikes scheduled for this step
        for (const auto& delayedEvent : it->second) {
            // Call delayed spike handlers (these will deliver synaptic input)
            for (auto& handler : pImpl->delayedHandlers) {
                handler(delayedEvent);
            }
        }
        // Remove processed spikes
        pImpl->delayedSpikes.erase(it);
    }
}

void SpikeSystem::registerHandler(SpikeHandler handler) {
    pImpl->handlers.push_back(handler);
}

void SpikeSystem::registerDelayedHandler(DelayedSpikeHandler handler) {
    pImpl->delayedHandlers.push_back(handler);
}

const std::vector<DetailedSpikeEvent>& SpikeSystem::getSpikeHistory() const {
    return pImpl->spikeHistory;
}

void SpikeSystem::clearHistory() {
    pImpl->spikeHistory.clear();
    pImpl->spikeCountPerNeuron.clear();
}

size_t SpikeSystem::getSpikeCount() const {
    return pImpl->spikeHistory.size();
}

size_t SpikeSystem::getPendingSpikeCount() const {
    size_t traditionalCount = pImpl->pendingSpikes.size();
    
    if (pImpl->eventQueue && pImpl->eventDrivenMode) {
        // Add event queue count if available
        return traditionalCount + pImpl->eventQueue->size();
    }
    
    return traditionalCount;
}

size_t SpikeSystem::getPendingDelayedCount() const {
    size_t traditionalCount = 0;
    for (const auto& pair : pImpl->delayedSpikes) {
        traditionalCount += pair.second.size();
    }
    
    if (pImpl->delayedEventQueue) {
        // Add delayed event queue count if available
        return traditionalCount + pImpl->delayedEventQueue->pendingCount();
    }
    
    return traditionalCount;
}

float SpikeSystem::getAverageSpikeRate() const {
    // Calculate average spike rate based on history
    if (pImpl->spikeHistory.size() < 2) {
        return 0.0f;
    }
    
    // Find time range
    float minTime = std::numeric_limits<float>::max();
    float maxTime = -std::numeric_limits<float>::max();
    
    for (const auto& spike : pImpl->spikeHistory) {
        minTime = std::min(minTime, static_cast<float>(spike.timestamp));
        maxTime = std::max(maxTime, static_cast<float>(spike.timestamp));
    }
    
    float duration = maxTime - minTime;
    if (duration <= 0.0f) {
        return 0.0f;
    }
    
    return static_cast<float>(pImpl->spikeHistory.size()) / duration;
}

std::vector<NeuronId> SpikeSystem::getMostActiveNeurons(size_t count) const {
    std::vector<std::pair<uint64_t, size_t>> neuronCounts;
    neuronCounts.reserve(pImpl->spikeCountPerNeuron.size());
    
    for (const auto& pair : pImpl->spikeCountPerNeuron) {
        neuronCounts.emplace_back(pair.first, pair.second);
    }
    
    // Sort by count descending
    std::sort(neuronCounts.begin(), neuronCounts.end(),
              [](const auto& a, const auto& b) { return a.second > b.second; });
    
    std::vector<NeuronId> result;
    result.reserve(std::min(count, neuronCounts.size()));
    for (size_t i = 0; i < std::min(count, neuronCounts.size()); ++i) {
        result.emplace_back(neuronCounts[i].first);
    }
    
    return result;
}

void SpikeSystem::reset() {
    while (!pImpl->pendingSpikes.empty()) {
        pImpl->pendingSpikes.pop();
    }
    pImpl->spikeHistory.clear();
    pImpl->spikeCountPerNeuron.clear();
    pImpl->delayedSpikes.clear();
    
    // Clear event queues
    if (pImpl->eventQueue) {
        pImpl->eventQueue->clear();
    }
    if (pImpl->delayedEventQueue) {
        pImpl->delayedEventQueue->clear();
    }
}

} // namespace nlm

// SpikeSystem EventQueue integration implementations

void SpikeSystem::setEventQueue(std::unique_ptr<SpikeRingBuffer> queue) {
    if (pImpl->eventQueue) {
        // If we already have an event queue, we should probably clean it up
        // For now, just log a warning
        static bool warningLogged = false;
        if (!warningLogged) {
            std::cerr << "Warning: Replacing existing event queue in SpikeSystem" << std::endl;
            warningLogged = true;
        }
    }
    pImpl->eventQueue = std::move(queue);
}

void SpikeSystem::setDelayedQueue(std::unique_ptr<DelayedSpikeQueue> queue) {
    if (pImpl->delayedEventQueue) {
        static bool warningLogged = false;
        if (!warningLogged) {
            std::cerr << "Warning: Replacing existing delayed event queue in SpikeSystem" << std::endl;
            warningLogged = true;
        }
    }
    pImpl->delayedEventQueue = std::move(queue);
}

void SpikeSystem::configureForEventDrivenProcessing() {
    // Configure the event queues for optimal performance
    if (pImpl->eventQueue) {
        // Set queue capacity based on expected spike rate
        // Use a conservative estimate for biological neural systems
        size_t spikeQueueCapacity = 8192;  // Typical firing rate ~10-20 Hz per neuron
        
        // The queue is already created with the capacity passed to constructor
        // We could resize it here if needed, but SpikeRingBuffer doesn't have resize method
    }
    
    if (pImpl->delayedEventQueue) {
        // Configure delayed queue for typical synaptic delays (1-20ms)
        // In step-based simulation, that's 1-100 steps
        pImpl->delayedEventQueue->clear();
    }
    
    // Enable event-driven mode for spike generation and processing
    pImpl->eventDrivenMode = true;
}

SpikeRingBuffer* SpikeSystem::getEventQueue() const {
    return pImpl->eventQueue.get();
}

DelayedSpikeQueue* SpikeSystem::getDelayedQueue() const {
    return pImpl->delayedEventQueue.get();
}
