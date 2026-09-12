#include "EventQueue.hpp"
#include <cassert>
#include <chrono>
#include <iostream>
#include <thread>

namespace nlm {

// SpikeRingBuffer implementation - constructor

SpikeRingBuffer::SpikeRingBuffer(size_t capacity) 
    : capacity_(capacity)
    , mask_(capacity - 1)  // capacity MUST be power of 2
    , buffer_(capacity_)
    , head_(0)
    , tail_(0) {
    // Verify capacity is power of 2
    assert((capacity & (capacity - 1)) == 0 && "SpikeRingBuffer capacity must be power of 2");
}

SpikeRingBuffer::~SpikeRingBuffer() = default;

// SpikeRingBuffer implementation - push()

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

// SpikeRingBuffer implementation - pop()

std::optional<QueuedSpikeEvent> SpikeRingBuffer::pop() {
    size_t head = head_.load(std::memory_order_relaxed);
    
    if (head == tail_.load(std::memory_order_acquire)) {
        return std::nullopt;  // Queue empty
    }
    
    QueuedSpikeEvent event = buffer_[head];
    head_.store((head + 1) & mask_, std::memory_order_release);
    return event;
}

// SpikeRingBuffer implementation - empty(), full(), size(), clear()

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

// DelayedSpikeQueue implementation - constructor

DelayedSpikeQueue::DelayedSpikeQueue(size_t numBuckets, size_t maxDelaySteps)
    : numBuckets_(numBuckets)
    , maxDelaySteps_(maxDelaySteps)
    , currentStep_(0)
    , oldestStep_(0)
    , buckets_(numBuckets)
    , pendingCounts_(numBuckets, 0) {
    assert(numBuckets >= maxDelaySteps && "numBuckets must be >= maxDelaySteps");
}

DelayedSpikeQueue::~DelayedSpikeQueue() = default;

// DelayedSpikeQueue implementation - schedule()

void DelayedSpikeQueue::schedule(uint64_t sourceNeuron, uint64_t destNeuron, 
                                uint64_t synapseId, float weight, 
                                bool isExcitatory, uint64_t deliveryStep) {
    if (deliveryStep < currentStep_) return;  // Already due, skip
    
    size_t bucketIdx = (deliveryStep % numBuckets_);
    QueuedSpikeEvent event(sourceNeuron, destNeuron, synapseId, weight, isExcitatory);
    
    if (buckets_[bucketIdx].push(event)) {
        pendingCounts_[bucketIdx].fetch_add(1, std::memory_order_relaxed);
    }
}

// DelayedSpikeQueue implementation - getDueSpikes()

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
    
    return result;
}

// DelayedSpikeQueue implementation - pendingCount(), bucketSize()

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

// DelayedSpikeQueue implementation - clear(), advanceTime()

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

// PriorityEventQueue implementation - constructor

PriorityEventQueue::PriorityEventQueue() {
    events_.reserve(MAX_PRIORITY_EVENTS);
}

// PriorityEventQueue implementation - push(), pop()

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
    // Simple push_back, could use heap for true priority
    std::sort(events_.begin(), events_.end(), std::greater<PriorityEvent>());
}

std::optional<PriorityEventQueue::PriorityEvent> PriorityEventQueue::pop() {
    if (events_.empty()) {
        return std::nullopt;
    }
    
    PriorityEvent event = events_.back();
    events_.pop_back();
    return event;
}

// PriorityEventQueue implementation - getDueEvents()

std::vector<PriorityEventQueue::PriorityEvent> PriorityEventQueue::getDueEvents(uint64_t currentTime) {
    std::vector<PriorityEvent> result;
    
    for (const auto& event : events_) {
        if (event.timestamp <= currentTime) {
            result.push_back(event);
        }
    }
    
    // Remove processed events
    events_.erase(std::remove_if(events_.begin(), events_.end(),
                                 [currentTime](const PriorityEvent& e) { return e.timestamp <= currentTime; }),
                  events_.end());
    
    return result;
}

} // namespace nlm

// Timing benchmarks

inline void benchmarkEventQueue() {
    std::cout << "EventQueue benchmark not yet implemented" << std::endl;
}

// Output for debugging

inline void logEventQueueStats(const SpikeRingBuffer& queue) {
    std::cout << "SpikeRingBuffer: size=" << queue.size()
              << ", capacity=" << queue.capacity()
              << ", empty=" << queue.empty()
              << ", full=" << queue.full() << std::endl;
}

inline void logEventQueueStats(const DelayedSpikeQueue& queue) {
    std::cout << "DelayedSpikeQueue: pending=" << queue.pendingCount()
              << ", oldestStep=" << queue.oldestStep_ << std::endl;
}

inline void logEventQueueStats(const PriorityEventQueue& queue) {
    std::cout << "PriorityEventQueue: size=" << queue.size()
              << ", events=" << queue.size() << std::endl;
}

} // namespace nlm