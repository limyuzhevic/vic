// EventDrivenProcessor.hpp - Enhanced event-driven processing
#pragma once

#include "Performance.hpp"
#include "EventQueue.hpp"
#include <queue>
#include <chrono>
#include <unordered_map>
#include <algorithm>
#include <functional>

namespace nlm {

class EventDrivenProcessor {
public:
    EventDrivenProcessor();
    ~EventDrivenProcessor() = default;
    
    // Disable copying
    EventDrivenProcessor(const EventDrivenProcessor&) = delete;
    EventDrivenProcessor& operator=(const EventDrivenProcessor&) = delete;
    
    // Configuration
    void configure(const RuntimePerformanceConfig& config) { config_ = config; }
    
    // Event processing
    void scheduleEvent(uint64_t neuronId, uint64_t eventType, const uint64_t* data, uint64_t timestamp);
    void processEvents(uint64_t currentTime);
    
    // Event filtering and prioritization
    void setEventFilter(std::function<bool(const QueuedSpikeEvent&)> filter) { eventFilter_ = filter; }
    void setEventPrioritizer(std::function<uint8_t(const QueuedSpikeEvent&)> prioritizer) { eventPrioritizer_ = prioritizer; }
    
    // Event batching for cache locality
    void enableEventBatching(bool enable) { enableEventBatching_ = enable; }
    void setBatchSize(size_t size) { batchSize_ = size; }
    
    // Event compression
    void enableEventCompression(bool enable) { enableEventCompression_ = enable; }
    void setCompressionThreshold(float threshold) { compressionThreshold_ = threshold; }
    
    // Statistics
    size_t getPendingEvents() const { return pendingEvents_; }
    size_t getProcessedEvents() const { return processedEvents_; }
    size_t getCompressedEvents() const { return compressedEvents_; }
    
    // Performance monitoring
    const PerformanceMetrics& getMetrics() const { return metrics_; }
    
    // Real-time scheduling
    void enableRealTimeScheduling(bool enable) { enableRealTimeScheduling_ = enable; }
    void setRealTimePriority(int priority) { realTimePriority_ = priority; }
    
    // Get singleton instance
    static EventDrivenProcessor& getInstance() {
        static EventDrivenProcessor instance;
        return instance;
    }
    
private:
    bool enabled_;
    RuntimePerformanceConfig config_;
    std::function<bool(const QueuedSpikeEvent&)> eventFilter_;
    std::function<uint8_t(const QueuedSpikeEvent&)> eventPrioritizer_;
    bool enableEventBatching_;
    size_t batchSize_;
    bool enableEventCompression_;
    float compressionThreshold_;
    bool enableRealTimeScheduling_;
    int realTimePriority_;
    
    // Event storage
    std::priority_queue<QueuedSpikeEvent, std::vector<QueuedSpikeEvent>, 
                       std::function<bool(const QueuedSpikeEvent&, const QueuedSpikeEvent&)>> eventQueue_;
    std::unordered_map<uint64_t, std::vector<QueuedSpikeEvent>> eventBatches_;
    std::vector<QueuedSpikeEvent> eventCompressionBuffer_;
    
    // Statistics
    size_t pendingEvents_;
    size_t processedEvents_;
    size_t compressedEvents_;
    PerformanceMetrics metrics_;
    
    // Real-time scheduling
    std::chrono::steady_clock::time_point lastEventProcessTime_;
};

} // namespace nlm