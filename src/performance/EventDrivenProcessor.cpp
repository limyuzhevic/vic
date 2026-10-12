#include "EventDrivenProcessor.hpp"
#include <iostream>
#include <algorithm>
#include <chrono>
#include <thread>

namespace nlm {

EventDrivenProcessor::EventDrivenProcessor()
    : enabled_(true)
    , eventFilter_([](const QueuedSpikeEvent&) { return true; })
    , eventPrioritizer_([](const QueuedSpikeEvent&) { return 0; })
    , enableEventBatching_(true)
    , batchSize_(256)
    , enableEventCompression_(true)
    , compressionThreshold_(0.5)
    , enableRealTimeScheduling_(false)
    , realTimePriority_(0)
    , pendingEvents_(0)
    , processedEvents_(0)
    , compressedEvents_(0) {
    
    // Initialize priority queue with custom comparator
    eventQueue_ = std::priority_queue<QueuedSpikeEvent, std::vector<QueuedSpikeEvent>, 
                                     std::function<bool(const QueuedSpikeEvent&, const QueuedSpikeEvent&)>>(
        std::less<QueuedSpikeEvent>(), std::vector<QueuedSpikeEvent>());
    
    if (EventDrivenProcessor::getInstance().isEnabled()) {
        std::cout << "[EVENT] Enhanced EventDrivenProcessor initialized" << std::endl;
    }
}

void EventDrivenProcessor::scheduleEvent(uint64_t neuronId, uint64_t eventType, const uint64_t* data, uint64_t timestamp) {
    // Create event from data (simplified - would use actual event data structures)
    QueuedSpikeEvent event;
    event.sourceNeuron = data[0];
    event.destNeuron = data[1];
    event.synapseId = data[2];
    event.weight = static_cast<float>(data[3]);
    event.isExcitatory = static_cast<uint8_t>(data[4]);
    
    // Filter events if filter is set
    if (eventFilter_ && !eventFilter_(event)) {
        return;
    }
    
    // Apply prioritization
    uint8_t priority = eventPrioritizer_(event);
    
    // Schedule with priority
    eventQueue_.push(event);
    pendingEvents_++;
    
    if (enableEventCompression_ && eventQueue_.size() > batchSize_) {
        compressEvents();
    }
}

void EventDrivenProcessor::processEvents(uint64_t currentTime) {
    auto processStartTime = std::chrono::steady_clock::now();
    size_t eventsProcessedThisStep = 0;
    
    if (enableEventBatching_ && eventBatches_.empty()) {
        // Batch events for better cache locality
        std::vector<QueuedSpikeEvent> batch;
        batch.reserve(batchSize_);
        
        while (!eventQueue_.empty() && batch.size() < batchSize_) {
            batch.push_back(eventQueue_.top());
            eventQueue_.pop();
        }
        
        if (!batch.empty()) {
            eventBatches_[currentTime] = std::move(batch);
        }
    }
    
    // Process batched events
    for (auto& [time, batch] : eventBatches_) {
        for (const auto& event : batch) {
            // Process event (simplified - would use actual event processing logic)
            eventsProcessedThisStep++;
        }
        
        // Remove old batches
        if (time < currentTime - config_.maxDelayedSpikeBuckets) {
            eventBatches_.erase(time);
        }
    }
    
    // Real-time scheduling
    if (enableRealTimeScheduling_) {
        auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::steady_clock::now() - processStartTime).count();
        
        if (elapsed > 10) {  // If processing takes too long
            // Prioritize critical events
            eventQueue_.push(QueuedSpikeEvent{0, 0, 0, 0.0f, 1}); // Simplified critical event
        }
    }
    
    // Update statistics
    processedEvents_ += eventsProcessedThisStep;
    metrics_.spikesProcessed += eventsProcessedThisStep;
    metrics_.eventQueueSize = eventQueue_.size();
    
    // Update metrics
    auto processEndTime = std::chrono::steady_clock::now();
    metrics_.stepTimeMs = std::chrono::duration<double, std::milli>(processEndTime - processStartTime).count();
}

void EventDrivenProcessor::compressEvents() {
    if (eventCompressionBuffer_.size() < 2) return;
    
    // Simple compression: merge similar events
    std::sort(eventCompressionBuffer_.begin(), eventCompressionBuffer_.end(),
              [](const QueuedSpikeEvent& a, const QueuedSpikeEvent& b) {
                  return a.sourceNeuron < b.sourceNeuron &&
                         a.destNeuron < b.destNeuron &&
                         a.synapseId < b.synapseId;
              });
    
    // Merge identical events
    size_t writePos = 0;
    for (size_t i = 0; i < eventCompressionBuffer_.size(); ++i) {
        if (writePos == 0 || 
            eventCompressionBuffer_[i].sourceNeuron != eventCompressionBuffer_[writePos - 1].sourceNeuron ||
            eventCompressionBuffer_[i].destNeuron != eventCompressionBuffer_[writePos - 1].destNeuron ||
            eventCompressionBuffer_[i].synapseId != eventCompressionBuffer_[writePos - 1].synapseId) {
            eventCompressionBuffer_[writePos++] = eventCompressionBuffer_[i];
        } else {
            // Merge - sum weights
            eventCompressionBuffer_[writePos - 1].weight += eventCompressionBuffer_[i].weight;
            compressedEvents_++;
        }
    }
    
    eventCompressionBuffer_.resize(writePos);
    
    // Restore to priority queue
    eventQueue_ = std::priority_queue<QueuedSpikeEvent, std::vector<QueuedSpikeEvent>, 
                                     std::function<bool(const QueuedSpikeEvent&, const QueuedSpikeEvent&)>>(
        std::less<QueuedSpikeEvent>(), std::move(eventCompressionBuffer_));
}

} // namespace nlm