#include "EventProfiler.hpp"
#include <nlohmann/json.hpp>
#include <fstream>
#include <algorithm>
#include <numeric>
#include <iomanip>
#include <unordered_map>

namespace nlm {

EventProfiler::EventProfiler()
    : enabled_(false)
    , running_(false)
    , samplingInterval_(0.1)
    , brain_(nullptr)
    , profiler_(nullptr) {
}

EventProfiler::~EventProfiler() {
    stop();
}

void EventProfiler::setEnabled(bool enabled) {
    enabled_ = enabled;
    if (!enabled_) {
        stop();
    }
}

void EventProfiler::setMaxEventHistorySize(size_t maxEvents) {
    // Implementation would manage history size
}

void EventProfiler::setSamplingInterval(double interval) {
    samplingInterval_ = interval;
}

void EventProfiler::recordEvent(EventType type, uint64_t timestamp, SimulationStep step,
                                const std::string& component, double processingTime,
                                size_t queueSize, int threadId,
                                const std::map<std::string, std::string>& metadata) {
    if (!enabled_) return;

    EventRecord record;
    record.type = type;
    record.timestamp = timestamp;
    record.step = step;
    record.component = component;
    record.processingTime = processingTime;
    record.queueSize = queueSize;
    record.threadId = threadId;
    record.metadata = metadata;
    record.eventId = nextEventId_.fetch_add(1);

    std::lock_guard<std::mutex> lock(eventHistoryMutex_);
    eventHistory_.push_back(record);

    // Update statistics
    updateQueueStats("default", queueSize, true, false, processingTime);
    updateSpikeDeliveryStats(record);
    updatePriorityStats(static_cast<int>(type));

    // Clean up old events if needed
    cleanupOldEvents();
}

void EventProfiler::recordSpikeEvent(uint64_t sourceNeuron, uint64_t destNeuron,
                                     uint64_t synapseId, float weight, bool isExcitatory,
                                     SimulationStep step, Timestamp time,
                                     size_t delaySteps) {
    if (!enabled_) return;

    recordEvent(EventType::SPIKE, static_cast<uint64_t>(time), step, "spike_system",
               0.0, 0, -1, {
                   {"source_neuron", std::to_string(sourceNeuron)},
                   {"dest_neuron", std::to_string(destNeuron)},
                   {"synapse_id", std::to_string(synapseId)},
                   {"weight", std::to_string(weight)},
                   {"is_excitatory", isExcitatory ? "true" : "false"},
                   {"delay_steps", std::to_string(delaySteps)}
               });
}

void EventProfiler::recordDelayedSpikeEvent(uint64_t sourceNeuron, uint64_t destNeuron,
                                            uint64_t synapseId, float weight, bool isExcitatory,
                                            SimulationStep step, Timestamp time,
                                            SimulationStep deliveryStep) {
    if (!enabled_) return;

    recordEvent(EventType::DELAYED_SPIKE, static_cast<uint64_t>(time), step, "spike_system",
               0.0, 0, -1, {
                   {"source_neuron", std::to_string(sourceNeuron)},
                   {"dest_neuron", std::to_string(destNeuron)},
                   {"synapse_id", std::to_string(synapseId)},
                   {"weight", std::to_string(weight)},
                   {"is_excitatory", isExcitatory ? "true" : "false"},
                   {"delivery_step", std::to_string(deliveryStep)}
               });
}

void EventProfiler::recordQueueOperation(const std::string& queueName,
                                         size_t queueSize, bool isPush, bool isPop,
                                         double processingTime) {
    if (!enabled_) return;

    updateQueueStats(queueName, queueSize, isPush, isPop, processingTime);
}

void EventProfiler::recordPriorityEvent(int priority, uint64_t timestamp,
                                        uint16_t eventType, const uint64_t* data) {
    if (!enabled_) return;

    recordEvent(EventType::SYSTEM_EVENT, timestamp, 0, "priority_queue",
              0.0, 0, -1, {
                   {"priority", std::to_string(priority)},
                   {"event_type", std::to_string(eventType)}
               });
}

void EventProfiler::setBrainIntegration(std::shared_ptr<Brain> brain) {
    brain_ = brain;
}

void EventProfiler::setProfiler(PerformanceProfiler* profiler) {
    profiler_ = profiler;
}

const EventProfiler::EventRecord* EventProfiler::getLastEvent() const {
    std::lock_guard<std::mutex> lock(eventHistoryMutex_);
    if (eventHistory_.empty()) return nullptr;
    return &eventHistory_.back();
}

std::vector<EventProfiler::EventRecord> EventProfiler::getEventsInTimeRange(uint64_t startTime, uint64_t endTime) const {
    std::lock_guard<std::mutex> lock(eventHistoryMutex_);
    std::vector<EventRecord> result;
    for (const auto& event : eventHistory_) {
        if (event.timestamp >= startTime && event.timestamp <= endTime) {
            result.push_back(event);
        }
    }
    return result;
}

const EventProfiler::QueueStats* EventProfiler::getQueueStats(const std::string& queueName) const {
    std::lock_guard<std::mutex> lock(queueStatsMutex_);
    auto it = queueStats_.find(queueName);
    if (it != queueStats_.end()) {
        return &it->second;
    }
    return nullptr;
}

const EventProfiler::SpikeDeliveryStats* EventProfiler::getSpikeDeliveryStats(SimulationStep step) const {
    std::lock_guard<std::mutex> lock(spikeStatsMutex_);
    auto it = spikeDeliveryStats_.find(step);
    if (it != spikeDeliveryStats_.end()) {
        return &it->second;
    }
    return nullptr;
}

const EventProfiler::PriorityStats* EventProfiler::getPriorityStats(int priority) const {
    std::lock_guard<std::mutex> lock(priorityStatsMutex_);
    auto it = priorityStats_.find(priority);
    if (it != priorityStats_.end()) {
        return &it->second;
    }
    return nullptr;
}

double EventProfiler::getAverageProcessingTime(EventType type) const {
    std::lock_guard<std::mutex> lock(typeTimesMutex_);
    auto it = typeProcessingTimes_.find(type);
    if (it == typeProcessingTimes_.end() || it->second.empty()) {
        return 0.0;
    }

    double sum = std::accumulate(it->second.begin(), it->second.end(), 0.0);
    return sum / it->second.size();
}

double EventProfiler::getQueueOverflowRate(const std::string& queueName) const {
    const QueueStats* stats = getQueueStats(queueName);
    if (!stats || stats->maxSize == 0) return 0.0;
    return static_cast<double>(stats->droppedCount) / stats->maxSize;
}

void EventProfiler::exportToJSON(const std::string& filename) const {
    nlohmann::json jsonData;
    exportToJSONInternal(jsonData);

    std::ofstream file(filename);
    if (!file.is_open()) {
        throw std::runtime_error("Cannot open file for writing: " + filename);
    }
    file << jsonData.dump(2);
}

void EventProfiler::exportToCSV(const std::string& filename) const {
    std::ofstream file(filename);
    if (!file.is_open()) {
        throw std::runtime_error("Cannot open file for writing: " + filename);
    }
    exportToCSVInternal(file);
}

std::string EventProfiler::generateReport() const {
    std::stringstream ss;
    generateReportInternal(ss);
    return ss.str();
}

void EventProfiler::start() {
    if (!enabled_) return;
    running_ = true;
    startTime_ = std::chrono::high_resolution_clock::now();
}

void EventProfiler::stop() {
    running_ = false;
}

void EventProfiler::reset() {
    std::lock_guard<std::mutex> lock1(eventHistoryMutex_);
    std::lock_guard<std::mutex> lock2(queueStatsMutex_);
    std::lock_guard<std::mutex> lock3(spikeStatsMutex_);
    std::lock_guard<std::mutex> lock4(priorityStatsMutex_);
    std::lock_guard<std::mutex> lock5(typeTimesMutex_);

    eventHistory_.clear();
    queueStats_.clear();
    spikeDeliveryStats_.clear();
    priorityStats_.clear();
    typeProcessingTimes_.clear();
}

bool EventProfiler::isRunning() const {
    return running_;
}

void EventProfiler::cleanupOldEvents() {
    auto cutoff = std::chrono::high_resolution_clock::now() -
                  std::chrono::hours(24);  // Keep 24 hours of events

    std::lock_guard<std::mutex> lock(eventHistoryMutex_);
    eventHistory_.erase(
        std::remove_if(eventHistory_.begin(), eventHistory_.end(),
            [cutoff](const EventRecord& r) {
                // Convert timestamp back to time_point for comparison
                // This is simplified - in practice you'd store time_points
                return false;  // TODO: Implement proper time comparison
            }),
        eventHistory_.end()
    );
}

void EventProfiler::updateQueueStats(const std::string& queueName, size_t size, bool isPush, bool isPop, double processingTime) {
    std::lock_guard<std::mutex> lock(queueStatsMutex_);

    auto& stats = queueStats_[queueName];
    stats.queueName = queueName;
    stats.currentSize = size;
    stats.maxSize = std::max(stats.maxSize, size);

    if (isPush && !isPop) {
        stats.currentSize++;
    } else if (isPop && !isPush) {
        stats.currentSize--;
    }

    stats.totalProcessingTime += processingTime;
    if (stats.processedCount > 0) {
        stats.avgProcessingTime = stats.totalProcessingTime / stats.processedCount;
    }

    if (size >= stats.maxSize) {
        stats.droppedCount++;
    }
}

void EventProfiler::updateSpikeDeliveryStats(const EventRecord& event) {
    std::lock_guard<std::mutex> lock(spikeStatsMutex_);

    auto& stats = spikeDeliveryStats_[event.step];
    stats.step = event.step;

    if (event.type == EventType::SPIKE) {
        stats.spikesScheduled++;
    } else if (event.type == EventType::DELAYED_SPIKE) {
        stats.spikesDelivered++;
    }

    // Extract delay from metadata if available
    auto it = event.metadata.find("delay_steps");
    if (it != event.metadata.end()) {
        double delay = std::stod(it->second);
        stats.delays.push_back(delay);
        if (!stats.delays.empty()) {
            double sum = std::accumulate(stats.delays.begin(), stats.delays.end(), 0.0);
            stats.avgDelay = sum / stats.delays.size();
        }
    }

    stats.processingTime += event.processingTime;
}

void EventProfiler::updatePriorityStats(int priority) {
    std::lock_guard<std::mutex> lock(priorityStatsMutex_);

    auto& stats = priorityStats_[priority];
    stats.priorityLevel = priority;
    stats.totalEvents++;

    if (priority <= 1) {
        stats.highPriorityEvents++;
    } else if (priority <= 3) {
        stats.normalPriorityEvents++;
    } else {
        stats.lowPriorityEvents++;
    }
}

void EventProfiler::generateReportInternal(std::ostream& output) const {
    output << "=== Event Profiling Report ===\n";
    output << "Enabled: " << (enabled_ ? "Yes" : "No") << "\n";
    output << "Running: " << (running_ ? "Yes" : "No") << "\n";
    output << "Total Events Recorded: " << eventHistory_.size() << "\n\n";

    output << "=== Queue Statistics ===\n";
    for (const auto& [name, stats] : queueStats_) {
        output << "Queue: " << stats.queueName << "\n";
        output << "  Current Size: " << stats.currentSize << "\n";
        output << "  Max Size: " << stats.maxSize << "\n";
        output << "  Avg Processing Time: " << stats.avgProcessingTime << "s\n";
        output << "  Total Processed: " << stats.processedCount << "\n";
        output << "  Dropped: " << stats.droppedCount << "\n";
        output << "  Overflow Rate: " << stats.overflowRate * 100 << "%\n\n";
    }

    output << "=== Spike Delivery Statistics ===\n";
    for (const auto& [step, stats] : spikeDeliveryStats_) {
        output << "Step: " << stats.step << "\n";
        output << "  Spikes Scheduled: " << stats.spikesScheduled << "\n";
        output << "  Spikes Delivered: " << stats.spikesDelivered << "\n";
        output << "  Average Delay: " << stats.avgDelay << " steps\n";
        output << "  Processing Time: " << stats.processingTime << "s\n\n";
    }

    output << "=== Priority Statistics ===\n";
    for (const auto& [priority, stats] : priorityStats_) {
        output << "Priority " << stats.priorityLevel << ":\n";
        output << "  Total Events: " << stats.totalEvents << "\n";
        output << "  High Priority: " << stats.highPriorityEvents << "\n";
        output << "  Normal Priority: " << stats.normalPriorityEvents << "\n";
        output << "  Low Priority: " << stats.lowPriorityEvents << "\n\n";
    }
}

void EventProfiler::exportToJSONInternal(nlohmann::json& json) const {
    json["enabled"] = enabled_;
    json["running"] = running_;
    json["total_events"] = eventHistory_.size();

    // Event history
    json["events"] = nlohmann::json::array();
    for (const auto& event : eventHistory_) {
        nlohmann::json eventJson;
        eventJson["id"] = event.eventId;
        eventJson["type"] = static_cast<int>(event.type);
        eventJson["timestamp"] = event.timestamp;
        eventJson["step"] = event.step;
        eventJson["component"] = event.component;
        eventJson["processing_time"] = event.processingTime;
        eventJson["queue_size"] = event.queueSize;
        eventJson["thread_id"] = event.threadId;
        eventJson["metadata"] = event.metadata;
        json["events"].push_back(eventJson);
    }

    // Queue stats
    json["queue_stats"] = nlohmann::json::array();
    for (const auto& [name, stats] : queueStats_) {
        nlohmann::json statsJson;
        statsJson["queue_name"] = stats.queueName;
        statsJson["current_size"] = stats.currentSize;
        statsJson["max_size"] = stats.maxSize;
        statsJson["avg_processing_time"] = stats.avgProcessingTime;
        statsJson["processed_count"] = stats.processedCount;
        statsJson["dropped_count"] = stats.droppedCount;
        statsJson["overflow_rate"] = stats.overflowRate;
        json["queue_stats"].push_back(statsJson);
    }
}

void EventProfiler::exportToCSVInternal(std::ostream& output) const {
    output << "Event ID,Type,Timestamp,Step,Component,Processing Time,Queue Size,Thread ID\n";
    for (const auto& event : eventHistory_) {
        output << event.eventId << "," << static_cast<int>(event.type) << ","
               << event.timestamp << "," << event.step << "," << event.component << ","
               << event.processingTime << "," << event.queueSize << "," << event.threadId << "\n";
    }
}
