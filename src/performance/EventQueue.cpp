#include "EventQueue.hpp"
#include <algorithm>
#include <stdexcept>

namespace nlm {

template<typename T>
EventQueue<T>::EventQueue(size_t maxSize) : maxSize_(maxSize), currentSize_(0) {}

template<typename T>
EventQueue<T>::~EventQueue() = default;

template<typename T>
void EventQueue<T>::push(const T& event, SimulationStep scheduledStep) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    if (currentSize_ >= maxSize_) {
        throw std::runtime_error("EventQueue is full");
    }
    
    eventQueue_.emplace_back(event, scheduledStep);
    std::push_heap(eventQueue_.begin(), eventQueue_.end(), 
                   [](const EventWrapper& a, const EventWrapper& b) {
                       return a.scheduledStep > b.scheduledStep;  // Max-heap: highest step first
                   });
    currentSize_++;
}

template<typename T>
std::optional<T> EventQueue<T>::pop(SimulationStep currentStep) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    if (eventQueue_.empty() || eventQueue_.front().scheduledStep > currentStep) {
        return std::nullopt;
    }
    
    std::pop_heap(eventQueue_.begin(), eventQueue_.end(),
                  [](const EventWrapper& a, const EventWrapper& b) {
                      return a.scheduledStep > b.scheduledStep;
                  });
    
    T event = std::move(eventQueue_.back().event);
    eventQueue_.pop_back();
    currentSize_--;
    
    return event;
}

template<typename T>
size_t EventQueue<T>::getCurrentSize() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return currentSize_;
}

template<typename T>
size_t EventQueue<T>::getMaxSize() const {
    return maxSize_;
}

template<typename T>
bool EventQueue<T>::hasEvents(SimulationStep currentStep) const {
    std::lock_guard<std::mutex> lock(mutex_);
    return !eventQueue_.empty() && eventQueue_.front().scheduledStep <= currentStep;
}

template<typename T>
void EventQueue<T>::clear() {
    std::lock_guard<std::mutex> lock(mutex_);
    eventQueue_.clear();
    currentSize_ = 0;
}

template<typename T>
void EventQueue<T>::update(SimulationStep currentStep, std::function<void(const T&)>) {
    // Process all events scheduled for this step
    while (hasEvents(currentStep)) {
        auto event = pop(currentStep);
        if (event) {
            processor_(event.value());
        }
    }
}

// Explicit instantiations for common event types
template class EventQueue<NeuronActivationEvent>;
template class EventQueue<SynapticInputEvent>;
template class EventQueue<SpikeEvent>;
template class EventQueue<PlasticityUpdateEvent>;

} // namespace nlm
