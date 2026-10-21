// NLM (熙然) - Neural Learning Machine
// Event queue implementation for efficient event handling
// Part of performance optimization system

#include "EventQueue.hpp"
#include <algorithm>
#include <stdexcept>
#include <iostream>
#include <thread>

namespace nlm {

EventQueue::EventQueue(size_t capacity, EventProcessingMode mode)
    : capacity_(capacity)
    , mode_(mode)
    , currentSize_(0)
    , head_(0)
    , tail_(0)
    , processedCount_(0)
    , droppedEvents_(0)
    , enabled_(true)
    , debugMode_(false)
{
    if (capacity == 0) {
        throw std::invalid_argument("Event queue capacity must be greater than 0");
    }
    
    // Allocate circular buffer
    eventBuffer_ = std::make_unique<Event[]>(capacity);
    eventCount_.resize(capacity, 0);
}

EventQueue::~EventQueue() = default;

bool EventQueue::push(const Event& event) {
    return push(event, std::chrono::system_clock::now());
}

bool EventQueue::push(Event event, const std::chrono::system_clock::time_point& timestamp) {
    if (!enabled_) {
        return false;
    }
    
    std::lock_guard<std::mutex> lock(mutex_);
    
    // Check if queue is full
    if (isFull()) {
        if (mode_ == EventProcessingMode::DropOldest) {
            // Remove oldest event
            pop();
            droppedEvents_++;
            if (debugMode_) {
                std::cout << "[EventQueue] Dropped oldest event (queue full)" << std::endl;
            }
        } else if (mode_ == EventProcessingMode::DropNewest) {
            // Discard new event
            droppedEvents_++;
            if (debugMode_) {
                std::cout << "[EventQueue] Dropped newest event (queue full)" << std::endl;
            }
            return false;
        } else { // Block
            if (debugMode_) {
                std::cout << "[EventQueue] Blocking (queue full)" << std::endl;
            }
            // In blocking mode, we'd wait here
            // For now, drop the event
            droppedEvents_++;
            return false;
        }
    }
    
    // Add event to queue
    size_t index = tail_;
    eventBuffer_[index] = std::move(event);
    eventCount_[index] = 1;
    eventTimestamps_[index] = timestamp;
    
    tail_ = (tail_ + 1) % capacity_;
    currentSize_++;
    
    if (debugMode_) {
        std::cout << "[EventQueue] Pushed event to index " << index 
                 << " (current size: " << currentSize_ << ")" << std::endl;
    }
    
    return true;
}

bool EventQueue::pop(Event& event) {
    return pop(event, std::chrono::system_clock::now());
}

bool EventQueue::pop(Event& event, const std::chrono::system_clock::time_point& timestamp) {
    if (!enabled_) {
        return false;
    }
    
    std::lock_guard<std::mutex> lock(mutex_);
    
    // Check if queue is empty
    if (isEmpty()) {
        if (debugMode_) {
            std::cout << "[EventQueue] Queue is empty" << std::endl;
        }
        return false;
    }
    
    // Get event from head
    size_t index = head_;
    event = std::move(eventBuffer_[index]);
    eventCount_[index] = 0;
    eventTimestamps_[index] = timestamp;
    
    head_ = (head_ + 1) % capacity_;
    currentSize_--;
    processedCount_++;
    
    if (debugMode_) {
        std::cout << "[EventQueue] Popped event from index " << index 
                 << " (current size: " << currentSize_ << ")" << std::endl;
    }
    
    return true;
}

bool EventQueue::peek(Event& event) const {
    return peek(event, std::chrono::system_clock::now());
}

bool EventQueue::peek(Event& event, const std::chrono::system_clock::time_point& timestamp) const {
    if (!enabled_) {
        return false;
    }
    
    std::lock_guard<std::mutex> lock(mutex_);
    
    if (isEmpty()) {
        return false;
    }
    
    event = eventBuffer_[head_];
    return true;
}

bool EventQueue::isEmpty() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return currentSize_ == 0;
}

bool EventQueue::isFull() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return currentSize_ >= capacity_;
}

size_t EventQueue::size() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return currentSize_;
}

size_t EventQueue::capacity() const {
    return capacity_;
}

EventProcessingMode EventQueue::getProcessingMode() const {
    return mode_;
}

void EventQueue::setProcessingMode(EventProcessingMode mode) {
    std::lock_guard<std::mutex> lock(mutex_);
    mode_ = mode;
}

bool EventQueue::isEnabled() const {
    return enabled_;
}

void EventQueue::setEnabled(bool enabled) {
    std::lock_guard<std::mutex> lock(mutex_);
    enabled_ = enabled;
}

bool EventQueue::isDebugMode() const {
    return debugMode_;
}

void EventQueue::setDebugMode(bool debug) {
    debugMode_ = debug;
}

EventStats EventQueue::getStats() const {
    std::lock_guard<std::mutex> lock(mutex_);
    
    EventStats stats;
    stats.capacity = capacity_;
    stats.currentSize = currentSize_;
    stats.processedCount = processedCount_;
    stats.droppedEvents = droppedEvents_;
    stats.enabled = enabled_;
    
    // Calculate queue utilization
    stats.queueUtilization = (capacity_ > 0) ? 
        (static_cast<double>(currentSize_) / capacity_) * 100.0 : 0.0;
    
    // Calculate processing efficiency
    stats.processingEfficiency = (droppedEvents_ + processedCount_ > 0) ?
        (static_cast<double>(processedCount_) / (droppedEvents_ + processedCount_)) * 100.0 : 0.0;
    
    // Calculate average wait time (simplified)
    if (!eventTimestamps_.empty()) {
        auto now = std::chrono::system_clock::now();
        auto totalWait = 0.0;
        size_t count = 0;
        
        for (size_t i = 0; i < capacity_; ++i) {
            if (eventTimestamps_[i].time_since_epoch().count() > 0) {
                auto waitTime = std::chrono::duration<double>(
                    now - eventTimestamps_[i]).count() * 1000.0; // Convert to milliseconds
                totalWait += waitTime;
                count++;
            }
        }
        
        stats.averageWaitTime = (count > 0) ? totalWait / count : 0.0;
    }
    
    return stats;
}

void EventQueue::resetStats() {
    std::lock_guard<std::mutex> lock(mutex_);
    processedCount_ = 0;
    droppedEvents_ = 0;
}

void EventQueue::clear() {
    std::lock_guard<std::mutex> lock(mutex_);
    
    // Clear all events
    for (size_t i = 0; i < capacity_; ++i) {
        eventCount_[i] = 0;
        eventTimestamps_[i] = std::chrono::system_clock::time_point();
    }
    
    head_ = 0;
    tail_ = 0;
    currentSize_ = 0;
    
    if (debugMode_) {
        std::cout << "[EventQueue] Cleared all events" << std::endl;
    }
}

bool EventQueue::removeEvents(const std::function<bool(const Event&)>& predicate) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    bool removed = false;
    size_t newHead = head_;
    
    for (size_t i = 0; i < currentSize_; ++i) {
        size_t index = (head_ + i) % capacity_;
        
        if (predicate(eventBuffer_[index])) {
            // Mark as removed
            eventCount_[index] = 0;
            eventTimestamps_[index] = std::chrono::system_clock::time_point();
            removed = true;
        } else {
            // Move valid event to new position
            if (i != newHead - head_) {
                eventBuffer_[newHead] = std::move(eventBuffer_[index]);
                eventCount_[newHead] = eventCount_[index];
                eventTimestamps_[newHead] = eventTimestamps_[index];
            }
            newHead = (newHead + 1) % capacity_;
        }
    }
    
    head_ = newHead;
    currentSize_ = removed ? currentSize_ : 0; // Simplified
    
    return removed;
}

bool EventQueue::filterEvents(const std::function<bool(const Event&)>& predicate) {
    return removeEvents(predicate);
}

std::vector<Event> EventQueue::extractAllEvents() {
    std::lock_guard<std::mutex> lock(mutex_);
    
    std::vector<Event> events;
    events.reserve(currentSize_);
    
    // Extract all events
    for (size_t i = 0; i < currentSize_; ++i) {
        size_t index = (head_ + i) % capacity_;
        events.push_back(std::move(eventBuffer_[index]));
        eventCount_[index] = 0;
        eventTimestamps_[index] = std::chrono::system_clock::time_point();
    }
    
    head_ = 0;
    tail_ = 0;
    currentSize_ = 0;
    
    return events;
}

EventQueue::EventQueue() : EventQueue(1000) {}  // Default: 1000 events

} // namespace nlm