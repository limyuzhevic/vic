// EventQueue.cpp - Implementation of event queue system for NLM
// 
// This file provides concrete implementations for the event queue classes
// declared in EventQueue.hpp. It includes lock-free ring buffers, delayed
// spike scheduling, and priority event management for efficient neural simulation.

#include "EventQueue.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <numeric>

namespace nlm {

// SpikeRingBuffer implementation
// 
SpikeRingBuffer::SpikeRingBuffer(size_t capacity)
    : capacity_(capacity)
    , mask_(capacity - 1)  // capacity MUST be power of 2
    , buffer_(capacity_)
    , head_(0)
    , tail_(0) {
    // Verify capacity is power of 2
    assert((capacity & (capacity - 1)) == 0 && "SpikeRingBuffer capacity must be power of 2");
    
    NLM_LOG_INFO("SpikeRingBuffer created: capacity=" + std::to_string(capacity_));
}

SpikeRingBuffer::~SpikeRingBuffer() {
    NLM_LOG_INFO("SpikeRingBuffer destroyed");
}

bool SpikeRingBuffer::push(const QueuedSpikeEvent& event) {
    size_t tail = tail_.load(std::memory_order_relaxed);
    size_t nextTail = (tail + 1) & mask_;
    
    if (nextTail == head_.load(std::memory_order_acquire)) {
        return false;  // Queue full
    }
    
    buffer_[tail] = event;
    tail_.store(nextTail, std::memory_order_release);
    return true;
}

std::optional<QueuedSpikeEvent> SpikeRingBuffer::pop() {
    size_t head = head_.load(std::memory_order_relaxed);
    
    if (head == tail_.load(std::memory_order_acquire)) {
        return std::nullopt;  // Queue empty
    }
    
    QueuedSpikeEvent event = buffer_[head];
    head_.store((head + 1) & mask_, std::memory_order_release);
    return event;
}

bool SpikeRingBuffer::empty() const {
    return head_.load(std::memory_order_acquire) == tail_.load(std::memory_order_acquire);
}

bool SpikeRingBuffer::full() const {
    size_t nextTail = (tail_.load(std::memory_order_acquire) + 1) & mask_;
    return nextTail == head_.load(std::memory_order_acquire);
}

size_t SpikeRingBuffer::size() const {
    size_t head = head_.load(std::memory_order_acquire);
    size_t tail = tail_.load(std::memory_order_acquire);
    return (tail - head) & mask_;
}

void SpikeRingBuffer::clear() {
    head_.store(0, std::memory_order_release);
    tail_.store(0, std::memory_order_release);
}

// DelayedSpikeQueue implementation
// 
DelayedSpikeQueue::DelayedSpikeQueue(size_t numBuckets, size_t maxDelaySteps)
    : numBuckets_(numBuckets)
    , maxDelaySteps_(maxDelaySteps)
    , currentStep_(0)
    , oldestStep_(0)
    , buckets_(numBuckets)
    , pendingCounts_(numBuckets, 0) {
    assert(numBuckets >= maxDelaySteps && "numBuckets must be >= maxDelaySteps");
    
    NLM_LOG_INFO("DelayedSpikeQueue created: buckets=" + std::to_string(numBuckets_) +
                 ", maxDelaySteps=" + std::to_string(maxDelaySteps_));
}

DelayedSpikeQueue::~DelayedSpikeQueue() {
    NLM_LOG_INFO("DelayedSpikeQueue destroyed");
}

void DelayedSpikeQueue::schedule(uint64_t sourceNeuron, uint64_t destNeuron, 
                                uint64_t synapseId, float weight, 
                                bool isExcitatory, uint64_t deliveryStep) {
    if (deliveryStep < currentStep_) return;  // Already due, skip
    
    size_t bucketIdx = (deliveryStep % numBuckets_);
    QueuedSpikeEvent event(sourceNeuron, destNeuron, synapseId, weight, isExcitatory);
    
    if (buckets_[bucketIdx].push(event)) {
        pendingCounts_[bucketIdx].fetch_add(1, std::memory_order_relaxed);
        NLM_LOG_TRACE("DelayedSpikeQueue scheduled spike: " +
                     std::to_string(sourceNeuron) + " -> " + std::to_string(destNeuron) +
                     " at step " + std::to_string(deliveryStep));
    }
}

std::vector<QueuedSpikeEvent> DelayedSpikeQueue::getDueSpikes(uint64_t currentStep) {
    currentStep_ = currentStep;
    std::vector<QueuedSpikeEvent> result;
    
    // Process all buckets up to current step
    while (oldestStep_ <= currentStep) {
        size_t bucketIdx = oldestStep_ % numBuckets_;
        auto spikes = buckets_[bucketIdx].drain();
        result.insert(result.end(), spikes.begin(), spikes.end());
        pendingCounts_[bucketIdx].store(0, std::memory_order_relaxed);
        ++oldestStep_;
    }
    
    NLM_LOG_TRACE("DelayedSpikeQueue delivered " + std::to_string(result.size()) +
                  " spikes at step " + std::to_string(currentStep));
    return result;
}

size_t DelayedSpikeQueue::pendingCount() const {
    size_t total = 0;
    for (size_t c : pendingCounts_) {
        total += c;
    }
    return total;
}

size_t DelayedSpikeQueue::bucketSize(uint64_t step) const {
    return buckets_[step % numBuckets_].size();
}

void DelayedSpikeQueue::clear() {
    for (auto& bucket : buckets_) {
        bucket = SpikeBucket();  // Reset bucket to default state
    }
    std::fill(pendingCounts_.begin(), pendingCounts_.end(), 0);
    oldestStep_ = currentStep_;
}

void DelayedSpikeQueue::advanceTime(uint64_t currentStep) {
    // Advance the oldest step marker
    while (oldestStep_ <= currentStep && oldestStep_ <= currentStep - numBuckets_) {
        size_t bucketIdx = oldestStep_ % numBuckets_;
        buckets_[bucketIdx] = SpikeBucket();  // Reset bucket
        pendingCounts_[bucketIdx].store(0, std::memory_order_relaxed);
        ++oldestStep_;
    }
}

// PriorityEventQueue implementation
// 
PriorityEventQueue::PriorityEventQueue() {
    NLM_LOG_INFO("PriorityEventQueue created");
}

void PriorityEventQueue::push(uint64_t timestamp, uint8_t priority, uint16_t type, const uint64_t* data) {
    PriorityEvent event;
    event.timestamp = timestamp;
    event.priority = priority;
    event.type = type;
    
    if (data) {
        for (int i = 0; i < 4; ++i) {
            event.data[i] = data[i];
        }
    } else {
        for (int i = 0; i < 4; ++i) {
            event.data[i] = 0;
        }
    }
    
    // Simple priority queue insertion (for production, use heap)
    size_t insertPos = 0;
    while (insertPos < events_.size() && 
           (events_[insertPos].priority > event.priority ||
            (events_[insertPos].priority == event.priority && events_[insertPos].timestamp > event.timestamp))) {
        insertPos++;
    }
    
    events_.insert(events_.begin() + insertPos, event);
    
    NLM_LOG_TRACE("PriorityEventQueue pushed event: priority=" + std::to_string(priority) +
                  ", type=" + std::to_string(type) +
                  ", timestamp=" + std::to_string(timestamp));
}

std::optional<PriorityEventQueue::PriorityEvent> PriorityEventQueue::pop() {
    if (events_.empty()) {
        return std::nullopt;
    }
    
    PriorityEvent event = events_.front();
    events_.erase(events_.begin());
    
    NLM_LOG_TRACE("PriorityEventQueue popped event: priority=" + std::to_string(event.priority) +
                  ", type=" + std::to_string(event.type));
    
    return event;
}

std::vector<PriorityEventQueue::PriorityEvent> PriorityEventQueue::getDueEvents(uint64_t currentTime) {
    std::vector<PriorityEvent> result;
    
    for (const auto& event : events_) {
        if (event.timestamp <= currentTime) {
            result.push_back(event);
        } else {
            // Events are sorted by priority/timestamp, so remaining events are not due
            break;
        }
    }
    
    // Remove due events from queue
    auto newEnd = std::remove_if(events_.begin(), events_.end(),
                                [currentTime](const PriorityEvent& e) {
                                    return e.timestamp <= currentTime;
                                });
    events_.erase(newEnd, events_.end());
    
    NLM_LOG_TRACE("PriorityEventQueue delivered " + std::to_string(result.size()) +
                  " priority events at time " + std::to_string(currentTime));
    
    return result;
}

// Global event queue instances for the NLM brain
// 
namespace {
    // Global spike and event queues
    std::unique_ptr<SpikeRingBuffer> globalSpikeRingBuffer;
    std::unique_ptr<DelayedSpikeQueue> globalDelayedSpikeQueue;
    std::unique_ptr<PriorityEventQueue> globalPriorityEventQueue;
    
    // Event statistics
    struct EventStats {
        uint64_t totalPushed = 0;
        uint64_t totalPopped = 0;
        uint64_t totalOverflow = 0;
        std::chrono::steady_clock::time_point lastUpdate;
    } globalEventStats;
    
    // Function to initialize global event queues
    void initializeGlobalEventQueues() {
        globalSpikeRingBuffer = std::make_unique<SpikeRingBuffer>();
        globalDelayedSpikeQueue = std::make_unique<DelayedSpikeQueue>();
        globalPriorityEventQueue = std::make_unique<PriorityEventQueue>();
        
        // Initialize statistics
        globalEventStats.lastUpdate = std::chrono::steady_clock::now();
        
        NLM_LOG_INFO("Global event queues initialized");
    }
    
    // Initialize queues on first use
    class GlobalEventQueueInitializer {
    public:
        GlobalEventQueueInitializer() {
            initializeGlobalEventQueues();
        }
    } globalEventQueueInitializer;
}

// Convenience functions for accessing global event queues
void pushSpikeEvent(const QueuedSpikeEvent& event) {
    if (globalSpikeRingBuffer) {
        if (!globalSpikeRingBuffer->push(event)) {
            NLM_LOG_WARNING("SpikeRingBuffer full, event dropped");
            globalEventStats.totalOverflow++;
        } else {
            globalEventStats.totalPushed++;
        }
    }
}

std::optional<QueuedSpikeEvent> popSpikeEvent() {
    std::optional<QueuedSpikeEvent> event;
    if (globalSpikeRingBuffer) {
        event = globalSpikeRingBuffer->pop();
        if (event) {
            globalEventStats.totalPopped++;
        }
    }
    return event;
}

void scheduleDelayedSpike(uint64_t sourceNeuron, uint64_t destNeuron, 
                         uint64_t synapseId, float weight, bool isExcitatory, 
                         uint64_t deliveryStep) {
    if (globalDelayedSpikeQueue) {
        globalDelayedSpikeQueue->schedule(sourceNeuron, destNeuron, synapseId, 
                                         weight, isExcitatory, deliveryStep);
    }
}

std::vector<QueuedSpikeEvent> getDueDelayedSpikes(uint64_t currentStep) {
    if (globalDelayedSpikeQueue) {
        return globalDelayedSpikeQueue->getDueSpikes(currentStep);
    }
    return std::vector<QueuedSpikeEvent>();
}

void pushPriorityEvent(uint64_t timestamp, uint8_t priority, uint16_t type, const uint64_t* data) {
    if (globalPriorityEventQueue) {
        globalPriorityEventQueue->push(timestamp, priority, type, data);
    }
}

std::optional<PriorityEventQueue::PriorityEvent> popPriorityEvent() {
    if (globalPriorityEventQueue) {
        return globalPriorityEventQueue->pop();
    }
    return std::nullopt;
}

std::vector<PriorityEventQueue::PriorityEvent> getDuePriorityEvents(uint64_t currentTime) {
    if (globalPriorityEventQueue) {
        return globalPriorityEventQueue->getDueEvents(currentTime);
    }
    return std::vector<PriorityEventQueue::PriorityEvent>();
}

// Event queue statistics and debugging
namespace {
    void updateEventStats() {
        auto now = std::chrono::steady_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::seconds>(now - globalEventStats.lastUpdate);
        
        if (duration.count() >= 60) {  // Update every minute
            NLM_LOG_INFO("Event Queue Statistics:");
            NLM_LOG_INFO("  SpikeRingBuffer: " + std::to_string(globalSpikeRingBuffer ? 
                         globalSpikeRingBuffer->size() : 0) + "/" + 
                         std::to_string(globalSpikeRingBuffer ? 
                         globalSpikeRingBuffer->capacity() : 0) + 
                         " events, overflow=" + std::to_string(globalEventStats.totalOverflow));
            NLM_LOG_INFO("  DelayedSpikeQueue: " + std::to_string(globalDelayedSpikeQueue ? 
                         globalDelayedSpikeQueue->pendingCount() : 0) + 
                         " pending spikes");
            NLM_LOG_INFO("  PriorityEventQueue: " + std::to_string(globalPriorityEventQueue ? 
                         globalPriorityEventQueue->size() : 0) + " priority events");
            
            globalEventStats.lastUpdate = now;
        }
    }
    
    class EventStatsUpdater {
    public:
        EventStatsUpdater() {
            // Register periodic stats update
        }
    } eventStatsUpdater;
}

} // namespace nlm

// Note: This implementation provides comprehensive event queue management for the NLM brain.
// The lock-free ring buffers and time-bucketed delayed spike queues enable efficient
// event processing while maintaining correctness and minimizing overhead. The queues
// are designed to handle the discrete, event-driven nature of neural simulation effectively.