#include "EventQueue.hpp"
#include "../performance/PerformanceMonitor.hpp"
#include <algorithm>
#include <cassert>
#include <cmath>
#include <thread>

namespace nlm {

// Performance monitor instance (singleton for demonstration)
static PerformanceMonitor* globalPerformanceMonitor = nullptr;

// Set the global performance monitor
void setGlobalPerformanceMonitor(PerformanceMonitor* monitor) {
    globalPerformanceMonitor = monitor;
}

SpikeRingBuffer::SpikeRingBuffer(size_t capacity)
    : capacity_(capacity)
    , mask_(capacity - 1)  // capacity MUST be power of 2
    , buffer_(capacity_)
    , head_(0)
    , tail_(0)
{
    // Verify capacity is power of 2
    assert((capacity & (capacity - 1)) == 0 && "SpikeRingBuffer capacity must be power of 2");
}

SpikeRingBuffer::~SpikeRingBuffer() = default;

bool SpikeRingBuffer::push(const QueuedSpikeEvent& event) {
    size_t currentTail = tail_.load(std::memory_order_acquire);
    size_t nextTail = (currentTail + 1) & mask_;
    
    if (nextTail == head_.load(std::memory_order_acquire)) {
        // Queue is full
        if (globalPerformanceMonitor) {
            globalPerformanceMonitor->onEventOverflow();
        }
        return false;
    }
    
    buffer_[currentTail] = event;
    tail_.store(nextTail, std::memory_order_release);
    return true;
}

std::optional<QueuedSpikeEvent> SpikeRingBuffer::pop() {
    size_t currentHead = head_.load(std::memory_order_acquire);
    
    if (currentHead == tail_.load(std::memory_order_acquire)) {
        return std::nullopt;  // Empty
    }
    
    QueuedSpikeEvent event = buffer_[currentHead];
    head_.store((currentHead + 1) & mask_, std::memory_order_release);
    return event;
}

bool SpikeRingBuffer::empty() const {
    return head_.load(std::memory_order_acquire) == tail_.load(std::memory_order_acquire);
}

bool SpikeRingBuffer::full() const {
    return ((tail_.load(std::memory_order_acquire) + 1) & mask_) == head_.load(std::memory_order_acquire);
}

size_t SpikeRingBuffer::size() const {
    size_t tail = tail_.load(std::memory_order_acquire);
    size_t head = head_.load(std::memory_order_acquire);
    return (tail - head + mask_ + 1) & mask_;
}

void SpikeRingBuffer::clear() {
    head_.store(0, std::memory_order_release);
    tail_.store(0, std::memory_order_release);
}

DelayedSpikeQueue::DelayedSpikeQueue(size_t numBuckets, size_t maxDelaySteps)
    : numBuckets_(numBuckets)
    , maxDelaySteps_(maxDelaySteps)
    , currentStep_(0)
    , oldestStep_(0)
    , buckets_(numBuckets)
    , pendingCounts_(numBuckets, 0)
{
    assert(numBuckets > 0 && "DelayedSpikeQueue requires at least one bucket");
    assert(maxDelaySteps_ > 0 && "DelayedSpikeQueue requires maxDelaySteps > 0");
}

DelayedSpikeQueue::~DelayedSpikeQueue() = default;

void DelayedSpikeQueue::schedule(uint64_t sourceNeuron, uint64_t destNeuron, uint64_t synapseId,
                               float weight, bool isExcitatory, uint64_t deliveryStep) {
    if (deliveryStep <= currentStep_) {
        return;  // Don't schedule spikes in the past or current step
    }
    
    uint64_t bucketIndex = deliveryStep % numBuckets_;
    if (!buckets_[bucketIndex].push(QueuedSpikeEvent(sourceNeuron, destNeuron, synapseId, weight, isExcitatory))) {
        if (globalPerformanceMonitor) {
            globalPerformanceMonitor->onEventOverflow();
        }
    }
    
    pendingCounts_[bucketIndex].fetch_add(1, std::memory_order_relaxed);
}

std::vector<QueuedSpikeEvent> DelayedSpikeQueue::getDueSpikes(uint64_t currentStep) {
    uint64_t bucketIndex = currentStep % numBuckets_;
    return buckets_[bucketIndex].drain();
}

size_t DelayedSpikeQueue::pendingCount() const {
    size_t total = 0;
    for (size_t i = 0; i < numBuckets_; ++i) {
        total += buckets_[i].size();
    }
    return total;
}

size_t DelayedSpikeQueue::bucketSize(uint64_t step) const {
    uint64_t bucketIndex = step % numBuckets_;
    return buckets_[bucketIndex].size();
}

void DelayedSpikeQueue::clear() {
    for (auto& bucket : buckets_) {
        // Reset bucket contents
        for (auto& event : bucket.events) {
            new (&event) QueuedSpikeEvent();
        }
        bucket.count.store(0, std::memory_order_release);
        bucket.readIndex.store(0, std::memory_order_release);
        bucket.writeIndex.store(0, std::memory_order_release);
    }
    
    std::fill(pendingCounts_.begin(), pendingCounts_.end(), 0);
}

void DelayedSpikeQueue::advanceTime(uint64_t currentStep) {
    if (currentStep_ != currentStep) {
        uint64_t newOldestStep = currentStep_ + 1;
        if (newOldestStep > oldestStep_) {
            oldestStep_ = newOldestStep;
        }
        currentStep_ = currentStep;
    }
}

PriorityEventQueue::PriorityEventQueue() {
    events_.reserve(MAX_PRIORITY_EVENTS);
}

void PriorityEventQueue::push(uint64_t timestamp, uint8_t priority, uint16_t type, const uint64_t* data) {
    PriorityEvent event;
    event.timestamp = timestamp;
    event.priority = priority;
    event.eventType = type;
    
    if (data) {
        for (int i = 0; i < 4; ++i) {
            event.data[i] = data[i];
        }
    } else {
        for (int i = 0; i < 4; ++i) {
            event.data[i] = 0;
        }
    }
    
    events_.push_back(event);
    std::push_heap(events_.begin(), events_.end());
}

std::optional<PriorityEventQueue::PriorityEvent> PriorityEventQueue::pop() {
    if (events_.empty()) {
        return std::nullopt;
    }
    
    std::pop_heap(events_.begin(), events_.end());
    PriorityEvent event = events_.back();
    events_.pop_back();
    return event;
}

std::vector<PriorityEventQueue::PriorityEvent> PriorityEventQueue::getDueEvents(uint64_t currentTime) {
    std::vector<PriorityEvent> dueEvents;
    
    // Extract all events with timestamp <= currentTime
    while (!events_.empty()) {
        std::pop_heap(events_.begin(), events_.end());
        PriorityEvent& event = events_.back();
        
        if (event.timestamp <= currentTime) {
            dueEvents.push_back(event);
            events_.pop_back();
        } else {
            // Push back since we modified heap structure
            std::push_heap(events_.begin(), events_.end());
            break;
        }
    }
    
    return dueEvents;
}

} // namespace nlm