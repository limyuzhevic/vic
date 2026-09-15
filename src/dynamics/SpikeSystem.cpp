#include "SpikeSystem.hpp"
#include "../performance/MemoryPool.hpp"
#include <algorithm>
#include <limits>
#include <chrono>
#include <queue>

namespace nlm {

struct SpikeSystem::Impl {
    // Spike history for statistics and replay
    std::vector<DetailedSpikeEvent> spikeHistory;
    
    // Event handlers for spike and delayed spike processing
    std::vector<SpikeHandler> handlers;
    std::vector<DelayedSpikeHandler> delayedHandlers;
    
    // Priority queue for true event-driven delayed spike delivery
    // Uses delivery_time as the sort key (earliest delivery first)
    std::priority_queue<DelayedSpikeEvent, std::vector<DelayedSpikeEvent>, std::greater<DelayedSpikeEvent>> delayedSpikes;
    
    // Memory pool for spike events
    MemoryPool<DetailedSpikeEvent> spikeEventPool;
    MemoryPool<DelayedSpikeEvent> delayedSpikeEventPool;
    
    // Maximum size of spike history before oldest spikes are evicted
    size_t maxHistorySize;
    
    // Statistics: count of spikes per neuron
    std::unordered_map<uint64_t, size_t> spikeCountPerNeuron;
    
    Impl() : maxHistorySize(10000), spikeEventPool(1024, 8), delayedSpikeEventPool(512, 4) {}
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
    // Process spike immediately for true event-driven behavior
    DetailedSpikeEvent* detailed = pImpl->spikeEventPool.allocate();
    if (!detailed) {
        // Fallback to heap allocation if pool is empty
        detailed = new DetailedSpikeEvent();
    } else {
        // Initialize the allocated memory
        new (detailed) DetailedSpikeEvent();
    }
    
    detailed->source = event.source_neuron;
    detailed->timestamp = event.timestamp;
    detailed->step = event.step;
    
    // Add to history
    pImpl->spikeHistory.push_back(*detailed);
    
    // Track spike count per neuron
    pImpl->spikeCountPerNeuron[event.source_neuron.value]++;
    
    // Trim history if needed
    if (pImpl->spikeHistory.size() > pImpl->maxHistorySize) {
        pImpl->spikeHistory.erase(pImpl->spikeHistory.begin());
    }
    
    // Call handlers immediately (event-driven)
    for (auto& handler : pImpl->handlers) {
        handler(*detailed);
    }
    
    // Deallocate if we allocated (not for heap fallback)
    if (detailed != new DetailedSpikeEvent()) {
        pImpl->spikeEventPool.deallocate(detailed);
    }
}

void SpikeSystem::queueSpike(NeuronId neuron, Timestamp timestamp, SimulationStep step) {
    // Create a spike event and process it immediately
    SpikeEvent event(neuron, timestamp, step);
    queueSpike(event);
}

void SpikeSystem::queueDelayedSpike(const DelayedSpikeEvent& event) {
    // Try to allocate from pool first
    DelayedSpikeEvent* delayedEvent = pImpl->delayedSpikeEventPool.allocate();
    if (!delayedEvent) {
        // Fallback to heap allocation
        delayedEvent = new DelayedSpikeEvent();
        // Copy the event
        *delayedEvent = event;
    } else {
        // Initialize with copy constructor
        new (delayedEvent) DelayedSpikeEvent(event);
    }
    
    // Store in priority queue for true event-driven delivery
    pImpl->delayedSpikes.push(*delayedEvent);
}

void SpikeSystem::processDelayedSpikes(SimulationStep currentStep, Timestamp currentTime) {
    // Process delayed spikes that are due at or before current time
    while (!pImpl->delayedSpikes.empty()) {
        const DelayedSpikeEvent& delayedEvent = pImpl->delayedSpikes.top();
        
        // Check if this spike should be delivered now
        if (delayedEvent.delivery_time <= currentTime) {
            // Call delayed spike handlers (these will deliver synaptic input)
            for (auto& handler : pImpl->delayedHandlers) {
                handler(delayedEvent);
            }
            
            // Remove delivered spike from queue
            pImpl->delayedSpikes.pop();
        } else {
            // All remaining spikes are in the future
            break;
        }
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

void SpikeSystem::reset() {
    pImpl->spikeHistory.clear();
    pImpl->spikeCountPerNeuron.clear();
    while (!pImpl->delayedSpikes.empty()) {
        pImpl->delayedSpikes.pop();
    }
}

// Additional optimization: Batch processing of similar events
void SpikeSystem::processSpikesBatch(SimulationStep currentStep) {
    // Process all spikes that occurred in this step in a batch
    // This is more efficient than processing them one by one
    
    // For now, just delegate to the existing mechanism
    // In a more optimized version, this would process all spikes at once
    // using SIMD vectorization and batch processing
    
    // The existing queueSpike already processes events immediately,
    // so this is mainly for statistics and potential future optimizations
    
    // Clear the statistics for this step
    pImpl->totalSpikesThisStep = 0;
}

// Get memory usage statistics
void SpikeSystem::logMemoryUsage() const {
    // Log memory usage of spike event pools
    // This helps with monitoring and tuning
}

} // namespace nlm
