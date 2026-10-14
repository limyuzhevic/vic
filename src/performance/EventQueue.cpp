// Stub implementation for EventQueue.cpp
// This is a placeholder to satisfy CMake build requirements
// Phase 2 implementations will replace this stub

#include "EventQueue.hpp"
#include <iostream>
#include <queue>
#include <mutex>
#include <condition_variable>

namespace nlm {

// EventQueue implementation (stub)
EventQueue::EventQueue(size_t maxSize) {
    std::cout << "EventQueue initialized with " << maxSize << " max size (stub)" << std::endl;
}

EventQueue::~EventQueue() = default;

void EventQueue::push(const SpikeEvent& event) {
    // Stub implementation
    std::cout << "EventQueue::push called (stub)" << std::endl;
    std::lock_guard<std::mutex> lock(mutex_);
    if (events_.size() < maxSize_) {
        events_.push(event);
    }
}

bool EventQueue::pop(SpikeEvent& event) {
    // Stub implementation
    std::cout << "EventQueue::pop called (stub)" << std::endl;
    std::lock_guard<std::mutex> lock(mutex_);
    if (!events_.empty()) {
        event = events_.front();
        events_.pop();
        return true;
    }
    return false;
}

bool EventQueue::empty() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return events_.empty();
}

size_t EventQueue::size() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return events_.size();
}

} // namespace nlm