#include "performance/EventQueue.hpp"

namespace nlm {

EventQueue::EventQueue(size_t maxEvents) {
    events_.reserve(maxEvents);
}

EventQueue::~EventQueue() {
    // Clear all events
    while (!front_) {
        Event* next = front_->next;
        delete front_;
        front_ = next;
    }
}

Event* EventQueue::pop() {
    Event* result = nullptr;
    
    // Try atomic exchange
    if (front_.compare_exchange_strong(result, nullptr,
                                         std::memory_order_release,
                                         std::memory_order_relaxed)) {
        return result;
    }
    
    // Fallback: locked pop
    std::lock_guard<std::mutex> lock(mutex_);
    result = front_;
    if (result) {
        front_ = result->next;
    }
    return result;
}

void EventQueue::push(Event* event) {
    event->next = nullptr;
    
    // Try atomic push
    Event* current = front_.load(std::memory_order_acquire);
    if (event->next.compare_exchange_weak(current, front_.load(std::memory_order_relaxed),
                                           std::memory_order_release,
                                           std::memory_order_relaxed)) {
        event->next = current;
        front_.store(event, std::memory_order_release);
    } else {
        // Locked push
        std::lock_guard<std::mutex> lock(mutex_);
        event->next = front_;
        front_ = event;
    }
}

bool EventQueue::empty() const {
    return front_.load(std::memory_order_acquire) == nullptr;
}

void EventQueue::clear() {
    std::lock_guard<std::mutex> lock(mutex_);
    while (front_) {
        Event* next = front_->next;
        delete front_;
        front_ = next;
    }
}

} // namespace nlm