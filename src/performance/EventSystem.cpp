#include "EventSystem.hpp"
#include "../core/Config/Config.hpp"
#include <algorithm>
#include <cstring>
#include <thread>
#include <atomic>
#include <chrono>
#include <iostream>

namespace nlm {

EventSystem::EventSystem(size_t maxEvents)
    : maxEvents_(maxEvents)
    , eventCount_(0)
    , processingTimeTotal_(0.0)
    , processingCount_(0)
    , enableZeroCopy_(true)
    , enableParallelProcessing_(true)
    , eventBuffer_(nullptr)
    , eventBufferSize_(0)
    , head_(0)
    , tail_(0)
    , bufferFull_(false)
    , pendingEvents_(0)
    , currentThreadId_(0)
    , maxThreadCount_(std::thread::hardware_concurrency())
    , eventPoolSize_(maxEvents / 10)
    , eventReuseCount_(0)
{
    // Calculate thread-local buffer sizes
    threadLocalBuffers_.resize(maxThreadCount_);
    
    // Initialize event pool
    eventPool_.resize(eventPoolSize_);
    eventPoolIndices_.resize(eventPoolSize_);
    for (size_t i = 0; i < eventPoolSize_; ++i) {
        eventPool_[i] = new Event();
        eventPoolIndices_[i] = i;
    }
    freeEventIndices_.assign(eventPoolIndices_.begin(), eventPoolIndices_.end());
    
    // Create thread-local queues
    for (auto& threadQueue : threadLocalQueues_) {
        threadQueue.reserve(1024);
    }
}

EventSystem::~EventSystem() {
    // Cleanup event pool
    for (auto* event : eventPool_) {
        delete event;
    }
    eventPool_.clear();
    
    // Cleanup thread-local buffers
    for (auto& buffer : threadLocalBuffers_) {
        delete[] buffer;
    }
}

void EventSystem::scheduleEvent(const Event& event) {
    std::lock_guard<std::mutex> lock(queueMutex_);
    
    // Check if we can use thread-local queue for better performance
    if (enableParallelProcessing_ && event.type != EventType::Spike) {
        // For non-spike events, use thread-local queue
        size_t threadId = currentThreadId_ % threadLocalQueues_.size();
        if (threadLocalQueues_[threadId].size() < threadLocalQueues_[threadId].capacity()) {
            threadLocalQueues_[threadId].push_back(event);
            eventCount_++;
            return;
        }
    }
    
    // Use main queue for spike events or if thread-local queue is full
    if (mainQueue_.size() < maxEvents_) {
        mainQueue_.push(event);
        eventCount_++;
    }
    // If main queue is also full, drop event (configurable)
}

Event EventSystem::processNextEvent() {
    // Try thread-local queue first
    if (enableParallelProcessing_) {
        size_t threadId = currentThreadId_ % threadLocalQueues_.size();
        if (!threadLocalQueues_[threadId].empty()) {
            Event event = threadLocalQueues_[threadId].front();
            threadLocalQueues_[threadId].pop_front();
            eventCount_--;
            return event;
        }
    }
    
    // Use main queue
    if (!mainQueue_.empty()) {
        Event event = mainQueue_.top();
        mainQueue_.pop();
        eventCount_--;
        return event;
    }
    
    // Return invalid event
    Event invalidEvent;
    invalidEvent.type = EventType::Spike; // Default type
    invalidEvent.neuronId = -1;
    return invalidEvent;
}

bool EventSystem::hasEvents() const {
    if (!mainQueue_.empty()) return true;
    
    if (enableParallelProcessing_) {
        for (size_t i = 0; i < threadLocalQueues_.size(); ++i) {
            if (!threadLocalQueues_[i].empty()) return true;
        }
    }
    
    return false;
}

void EventSystem::clearCompletedEvents() {
    std::lock_guard<std::mutex> lock(queueMutex_);
    
    // Clear main queue
    while (!mainQueue_.empty() && mainQueue_.top().time <= currentSimulationTime_) {
        mainQueue_.pop();
        eventCount_--;
    }
    
    // Clear thread-local queues
    if (enableParallelProcessing_) {
        for (auto& threadQueue : threadLocalQueues_) {
            threadQueue.clear();
        }
    }
}

void EventSystem::optimizeMemoryLayout() {
    // Optimize memory layout for better cache performance
    // Pre-allocate and organize events for better locality
    
    // Copy all events to contiguous memory
    std::vector<Event> allEvents;
    allEvents.reserve(mainQueue_.size());
    
    // Extract from main queue (destroying original heap)
    while (!mainQueue_.empty()) {
        allEvents.push_back(mainQueue_.top());
        mainQueue_.pop();
    }
    
    // Sort by time for better processing order
    std::sort(allEvents.begin(), allEvents.end(),
              [](const Event& a, const Event& b) { return a.time < b.time; });
    
    // Rebuild heap
    for (const auto& event : allEvents) {
        mainQueue_.push(event);
    }
    
    // Compact thread-local queues
    if (enableParallelProcessing_) {
        for (auto& threadQueue : threadLocalQueues_) {
            if (threadQueue.size() > threadQueue.capacity() / 2) {
                std::vector<Event> compact;
                compact.reserve(threadQueue.size());
                compact.assign(threadQueue.begin(), threadQueue.end());
                threadQueue.clear();
                threadQueue.shrink_to_fit();
                threadQueue.reserve(compact.size());
                threadQueue.assign(compact.begin(), compact.end());
            }
        }
    }
}

size_t EventSystem::getEventCount() const {
    return eventCount_;
}

double EventSystem::getAverageProcessingTime() const {
    if (processingCount_ == 0) return 0.0;
    return processingTimeTotal_ / processingCount_;
}

void EventSystem::setCurrentTime(Timestamp time) {
    currentSimulationTime_ = time;
}

void EventSystem::setParallelProcessing(bool enable) {
    enableParallelProcessing_ = enable;
    if (!enable) {
        // Merge thread-local queues into main queue
        std::lock_guard<std::mutex> lock(queueMutex_);
        for (auto& threadQueue : threadLocalQueues_) {
            while (!threadQueue.empty()) {
                mainQueue_.push(threadQueue.front());
                threadQueue.pop_front();
            }
        }
    }
}

void EventSystem::setZeroCopy(bool enable) {
    enableZeroCopy_ = enable;
}

void EventSystem::setThreadCount(size_t count) {
    maxThreadCount_ = count;
    threadLocalQueues_.resize(count);
    threadLocalBuffers_.resize(count);
    
    // Resize buffers for new thread count
    for (size_t i = 0; i < count; ++i) {
        size_t bufferSize = std::min(eventPoolSize_ / 4, static_cast<size_t>(1024));
        threadLocalBuffers_[i] = new Event[bufferSize];
    }
}

void EventSystem::enableEventRecycling(bool enable) {
    eventReuseCount_ = enable ? 10 : 0;
}

void EventSystem::setMaxEvents(size_t maxEvents) {
    std::lock_guard<std::mutex> lock(queueMutex_);
    maxEvents_ = maxEvents;
    
    // Resize main queue
    std::priority_queue<Event> newQueue;
    newQueue.reserve(maxEvents_);
    while (!mainQueue_.empty()) {
        if (newQueue.size() < maxEvents_) {
            newQueue.push(mainQueue_.top());
        }
        mainQueue_.pop();
    }
    mainQueue_ = std::move(newQueue);
}

void EventSystem::updateCurrentThread() {
    currentThreadId_ = std::this_thread::get_id() % maxThreadCount_;
}

EventSystem::EventPoolIterator EventSystem::begin() {
    return EventPoolIterator(eventPool_.begin());
}

EventSystem::EventPoolIterator EventSystem::end() {
    return EventPoolIterator(eventPool_.end());
}

EventSystem::EventPoolIterator EventSystem::getFreeEvent() {
    std::lock_guard<std::mutex> lock(poolMutex_);
    
    if (freeEventIndices_.empty()) {
        // Grow pool
        size_t oldSize = eventPool_.size();
        eventPool_.resize(oldSize * 2);
        eventPoolIndices_.resize(oldSize * 2);
        
        for (size_t i = oldSize; i < eventPool_.size(); ++i) {
            eventPool_[i] = new Event();
            eventPoolIndices_[i] = i;
        }
        
        freeEventIndices_.assign(eventPoolIndices_.begin() + oldSize, eventPoolIndices_.end());
    }
    
    size_t index = freeEventIndices_.back();
    freeEventIndices_.pop_back();
    return EventPoolIterator(&eventPool_[index]);
}

void EventSystem::returnEvent(EventPoolIterator iterator) {
    if (iterator.ptr_ && iterator.index_ < eventPool_.size()) {
        std::lock_guard<std::mutex> lock(poolMutex_);
        freeEventIndices_.push_back(iterator.index_);
        
        if (eventReuseCount_ > 0) {
            // Zero out the event data for reuse
            Event* event = iterator.ptr_;
            new (event) Event(); // Placement new for zero initialization
        }
    }
}

EventSystem::EventPoolIterator::EventPoolIterator(Event** ptr) : ptr_(ptr), index_(0) {
    if (ptr_) {
        index_ = ptr_ - &eventSystem_.eventPool_[0];
    }
}

EventSystem::EventPoolIterator::EventPoolIterator(std::vector<Event*>::iterator it) : ptr_(nullptr), index_(0) {
    if (it != std::vector<Event*>::end()) {
        ptr_ = *it;
        if (&eventSystem_.eventPool_[0] <= ptr_ && ptr_ < &eventSystem_.eventPool_[0] + eventSystem_.eventPool_.size()) {
            index_ = ptr_ - &eventSystem_.eventPool_[0];
        }
    }
}

EventSystem::EventPoolIterator& EventSystem::EventPoolIterator::operator++() {
    if (ptr_ && index_ < eventSystem_.eventPool_.size()) {
        ptr_ = eventSystem_.eventPool_[++index_];
    } else {
        ptr_ = nullptr;
    }
    return *this;
}

EventSystem::EventPoolIterator EventSystem::EventPoolIterator::operator++(int) {
    EventPoolIterator temp = *this;
    ++(*this);
    return temp;
}

Event& EventSystem::EventPoolIterator::operator*() {
    return *ptr_;
}

Event* EventSystem::EventPoolIterator::operator->() {
    return ptr_;
}

bool EventSystem::EventPoolIterator::operator==(const EventPoolIterator& other) const {
    return ptr_ == other.ptr_;
}

bool EventSystem::EventPoolIterator::operator!=(const EventPoolIterator& other) const {
    return ptr_ != other.ptr_;
}

EventSystem::EventPoolIterator EventSystem::EventPoolIterator::create(Event* event) {
    EventPoolIterator it;
    if (event) {
        // Find index in event pool
        for (size_t i = 0; i < eventSystem_.eventPool_.size(); ++i) {
            if (eventSystem_.eventPool_[i] == event) {
                it.ptr_ = event;
                it.index_ = i;
                return it;
            }
        }
    }
    return it;
}

} // namespace nlm