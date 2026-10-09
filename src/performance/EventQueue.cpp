// EventQueue.cpp implementation

#include "performance/EventQueue.hpp"
#include <algorithm>
#include <stdexcept>

namespace nlm {

SpikeRingBuffer::SpikeRingBuffer(size_t capacity)
    : capacity_(capacity)
    , mask_(capacity - 1)  // capacity MUST be power of 2
    , buffer_(capacity_)
    , head_(0)
    , tail_(0) {
    assert((capacity & (capacity - 1)) == 0 && "SpikeRingBuffer capacity must be power of 2");
}

SpikeRingBuffer::~SpikeRingBuffer() = default;

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

void DelayedSpikeQueue::schedule(uint64_t sourceNeuron, uint64_t destNeuron,
                                 uint64_t synapseId, float weight,
                                 bool isExcitatory, uint64_t deliveryStep) {
    if (deliveryStep < currentStep_) return;  // Already due, skip
    
    size_t bucketIdx = deliveryStep % numBuckets_;
    QueuedSpikeEvent event(sourceNeuron, destNeuron, synapseId, weight, isExcitatory);
    
    if (buckets_[bucketIdx].push(event)) {
        pendingCounts_[bucketIdx].fetch_add(1, std::memory_order_relaxed);
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

PriorityEventQueue::PriorityEventQueue() = default;

void PriorityEventQueue::push(uint64_t timestamp, uint8_t priority, uint16_t type, const uint64_t* data) {
    if (events_.size() >= MAX_PRIORITY_EVENTS) {
        // Remove lowest priority event to make room
        size_t minPriority = std::numeric_limits<uint8_t>::max();
        size_t minIndex = 0;
        for (size_t i = 0; i < events_.size(); ++i) {
            if (events_[i].priority < minPriority) {
                minPriority = events_[i].priority;
                minIndex = i;
            }
        }
        events_.erase(events_.begin() + minIndex);
    }
    
    PriorityEvent event;
    event.timestamp = timestamp;
    event.priority = priority;
    event.type = type;
    if (data) {
        std::copy(data, data + 4, std::begin(event.data));
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
    std::vector<PriorityEvent> result;
    
    for (auto it = events_.begin(); it != events_.end();) {
        if (it->timestamp > currentTime) {
            // Events are ordered by priority, then time, so we're done
            break;
        }
        
        result.push_back(*it);
        it = events_.erase(it);
    }
    
    // Restore heap property for remaining events
    std::make_heap(events_.begin(), events_.end());
    
    return result;
}

} // namespace nlm
