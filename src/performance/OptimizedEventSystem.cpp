#include "OptimizedEventSystem.hpp"
#include "../core/SimulationClock/SimulationClock.hpp"
#include "<algorithm>"
#include "<numeric>"

namespace nlm {

// OptimizedEventQueue implementation
OptimizedEventQueue::OptimizedEventQueue(size_t capacity)
    : ringBuffer_(capacity)
    , delayedQueue_(capacity / 2, 100)
    , priorityQueue_(capacity / 4)
    , spikeStats_(capacity)
    , enabled_(true)
    , compressionEnabled_(true)
    , compressionThreshold_(1000)  // Compress events longer than this
{
}

OptimizedEventQueue::~OptimizedEventQueue() = default;

bool OptimizedEventQueue::queueSpike(SpikeEvent event) {
    if (!enabled_) return false;
    
    // Compress spike events if enabled
    if (compressionEnabled_) {
        // Check if we should compress (event type, timing, etc.)
        if (shouldCompress(event)) {
            event = compressSpikeEvent(event);
        }
    }
    
    // Try to queue in ring buffer first
    QueuedSpikeEvent queuedEvent(
        event.source_neuron,
        event.destination_neuron,
        event.synapse_id,
        event.weight,
        event.is_excitatory
    );
    
    // Record for statistics
    spikeStats_.recordSpike(event.source_neuron, event.destination_neuron);
    
    return ringBuffer_.push(queuedEvent);
}

bool OptimizedEventQueue::queueDelayedSpike(DelayedSpikeEvent event) {
    if (!enabled_) return false;
    
    spikeStats_.recordDelayedSpike(event.destination_neuron);
    delayedQueue_.schedule(
        event.source_neuron,
        event.destination_neuron,
        event.synapse_id,
        event.weight,
        event.is_excitatory,
        event.delivery_step
    );
    
    return true;
}

bool OptimizedEventQueue::queuePriorityEvent(PriorityEvent event) {
    if (!enabled_) return false;
    
    priorityQueue_.push(event.timestamp, event.priority, event.type, event.data);
    return true;
}

bool OptimizedEventQueue::processEvents(SimulationStep currentStep, SimulationClock& clock) {
    bool eventsProcessed = false;
    
    // Process immediate spikes
    while (auto event = ringBuffer_.pop()) {
        eventsProcessed = true;
        // Deliver spike to destination neuron
        onSpikeDelivered(event->sourceNeuron, event->destNeuron, event->weight, event->isExcitatory);
    }
    
    // Process delayed spikes due at this step
    auto delayedEvents = delayedQueue_.getDueSpikes(currentStep);
    for (const auto& event : delayedEvents) {
        eventsProcessed = true;
        onSpikeDelivered(event.sourceNeuron, event.destNeuron, event.weight, event.isExcitatory);
    }
    
    // Process priority events
    while (auto event = priorityQueue_.pop()) {
        eventsProcessed = true;
        onPriorityEvent(*event);
    }
    
    // Advance time buckets in delayed queue
    delayedQueue_.advanceTime(currentStep);
    
    return eventsProcessed;
}

void OptimizedEventQueue::advanceTime(SimulationStep currentStep) {
    delayedQueue_.advanceTime(currentStep);
    spikeStats_.advanceTime(currentStep);
}

void OptimizedEventQueue::clear() {
    ringBuffer_.clear();
    delayedQueue_.clear();
    priorityQueue_.clear();
    spikeStats_.reset();
}

EventQueueStats OptimizedEventQueue::getStats() const {
    return spikeStats_.getStats();
}

void OptimizedEventQueue::enable(bool enabled) {
    enabled_ = enabled;
    if (!enabled_) {
        clear();
    }
}

void OptimizedEventQueue::enableCompression(bool enabled, size_t threshold) {
    compressionEnabled_ = enabled;
    compressionThreshold_ = threshold;
}

bool OptimizedEventQueue::shouldCompress(const SpikeEvent& event) const {
    // Simple compression heuristic: compress similar spike patterns
    // In a real implementation, this would be more sophisticated
    return event.weight < 0.1f && event.is_excitatory && 
           spikeStats_.getSpikeCount(event.source_neuron) > compressionThreshold_;
}

SpikeEvent OptimizedEventQueue::compressSpikeEvent(const SpikeEvent& event) {
    // Compress spike event by reducing precision or combining events
    SpikeEvent compressed = event;
    // For example, quantize weight to save memory
    compressed.weight = std::round(compressed.weight * 100.0f) / 100.0f;
    return compressed;
}

// OptimizedSpikeCompressor implementation
OptimizedSpikeCompressor::OptimizedSpikeCompressor(size_t maxBatchSize)
    : maxBatchSize_(maxBatchSize)
{
}

std::vector<SpikeEvent> OptimizedSpikeCompressor::compress(const std::vector<SpikeEvent>& events) {
    std::vector<SpikeEvent> compressed;
    compressed.reserve(events.size());
    
    // Group spikes by characteristics for compression
    std::unordered_map<size_t, std::vector<SpikeEvent>> groups;
    
    for (const auto& event : events) {
        // Create hash based on key characteristics
        size_t hash = event.source_neuron + (event.destination_neuron * 31) + 
                     (std::hash<float>{}(event.weight) * 7) + 
                     (event.is_excitatory ? 13 : 29);
        groups[hash].push_back(event);
    }
    
    // Compress each group
    for (auto& [hash, groupEvents] : groups) {
        if (groupEvents.size() >= maxBatchSize_) {
            // Compress by averaging weights and using representative
            float avgWeight = std::accumulate(groupEvents.begin(), groupEvents.end(), 0.0f,
                                             [](float sum, const SpikeEvent& e) { return sum + e.weight; }) / groupEvents.size();
            
            SpikeEvent representative = groupEvents[0];
            representative.weight = avgWeight;
            representative.is_excitatory = 
                std::count_if(groupEvents.begin(), groupEvents.end(), 
                             [](const SpikeEvent& e) { return e.is_excitatory; }) > 
                groupEvents.size() / 2;
            
            compressed.push_back(representative);
        } else {
            // Keep individual events
            for (const auto& event : groupEvents) {
                compressed.push_back(event);
            }
        }
    }
    
    return compressed;
}

// OptimizedEventProcessor implementation
OptimizedEventProcessor::OptimizedEventProcessor(SimulationStep maxHistorySteps)
    : maxHistorySteps_(maxHistorySteps)
{
}

void OptimizedEventProcessor::processEventBatch(const std::vector<SpikeEvent>& events,
                                                const std::vector<DelayedSpikeEvent>& delayedEvents,
                                                const std::vector<PriorityEvent>& priorityEvents,
                                                SimulationClock& clock,
                                                const EventProcessorCallbacks& callbacks) {
    // Process events in optimized batches for better cache locality
    if (!events.empty()) {
        processSpikeBatch(events, clock, callbacks);
    }
    
    if (!delayedEvents.empty()) {
        processDelayedBatch(delayedEvents, clock, callbacks);
    }
    
    if (!priorityEvents.empty()) {
        processPriorityBatch(priorityEvents, clock, callbacks);
    }
}

void OptimizedEventProcessor::processSpikeBatch(const std::vector<SpikeEvent>& events,
                                                SimulationClock& clock,
                                                const EventProcessorCallbacks& callbacks) {
    // Process spikes in order for better cache locality
    std::vector<SpikeEvent> sortedEvents = events;
    std::sort(sortedEvents.begin(), sortedEvents.end(),
              [](const SpikeEvent& a, const SpikeEvent& b) {
                  return a.source_neuron < b.source_neuron;
              });
    
    for (const auto& event : sortedEvents) {
        if (callbacks.onSpike) {
            callbacks.onSpike(event);
        }
        
        // Check if we need to update history
        if (maxHistorySteps_ > 0) {
            spikeHistory_.push_back(event);
            if (spikeHistory_.size() > maxHistorySteps_) {
                spikeHistory_.pop_front();
            }
        }
    }
}

void OptimizedEventProcessor::processDelayedBatch(const std::vector<DelayedSpikeEvent>& events,
                                                  SimulationClock& clock,
                                                  const EventProcessorCallbacks& callbacks) {
    for (const auto& event : events) {
        if (callbacks.onDelayedSpike) {
            callbacks.onDelayedSpike(event);
        }
        
        if (maxHistorySteps_ > 0) {
            delayedHistory_.push_back(event);
            if (delayedHistory_.size() > maxHistorySteps_) {
                delayedHistory_.pop_front();
            }
        }
    }
}

void OptimizedEventProcessor::processPriorityBatch(const std::vector<PriorityEvent>& events,
                                                   SimulationClock& clock,
                                                   const EventProcessorCallbacks& callbacks) {
    // Process priority events in order
    std::vector<PriorityEvent> sortedEvents = events;
    std::sort(sortedEvents.begin(), sortedEvents.end(),
              [](const PriorityEvent& a, const PriorityEvent& b) {
                  return a.priority < b.priority; // Lower priority number = higher priority
              });
    
    for (const auto& event : sortedEvents) {
        if (callbacks.onPriorityEvent) {
            callbacks.onPriorityEvent(event);
        }
        
        if (maxHistorySteps_ > 0) {
            priorityHistory_.push_back(event);
            if (priorityHistory_.size() > maxHistorySteps_) {
                priorityHistory_.pop_front();
            }
        }
    }
}

void OptimizedEventProcessor::getRecentEvents(std::vector<SpikeEvent>& spikes,
                                              std::vector<DelayedSpikeEvent>& delayed,
                                              std::vector<PriorityEvent>& priority) const {
    spikes = std::vector<SpikeEvent>(spikeHistory_.begin(), spikeHistory_.end());
    delayed = std::vector<DelayedSpikeEvent>(delayedHistory_.begin(), delayedHistory_.end());
    priority = std::vector<PriorityEvent>(priorityHistory_.begin(), priorityHistory_.end());
}

void OptimizedEventProcessor::clearHistory() {
    spikeHistory_.clear();
    delayedHistory_.clear();
    priorityHistory_.clear();
}

} // namespace nlm
