#include "Profiler.hpp"
#include <nlohmann/json.hpp>
#include <fstream>
#include <algorithm>
#include <numeric>
#include <iomanip>
#include <unordered_set>

namespace nlm {

PerformanceProfiler::PerformanceProfiler()
    : mode_(Mode::OFF)
    , interval_(0.1)  // 100ms default interval
    , memoryMonitor_(nullptr)
    , eventProfiler_(nullptr) {
}

PerformanceProfiler::~PerformanceProfiler() {
    // Cleanup resources
}

void PerformanceProfiler::setMode(Mode mode) {
    mode_ = mode;
}

void PerformanceProfiler::enableOperationProfiling(const std::string& operationName, bool enable) {
    // Implementation would track if operation should be profiled
}

void PerformanceProfiler::setMaxHistorySize(size_t maxPoints) {
    // Implementation would manage history size
}

void PerformanceProfiler::setProfilingInterval(double intervalSeconds) {
    interval_ = std::chrono::duration<double>(intervalSeconds);
}

void PerformanceProfiler::recordTimePoint(const std::string& operation, const std::map<std::string, std::string>& metadata) {
    if (mode_ == Mode::OFF) return;

    TimePoint point;
    point.operation = operation;
    point.timestamp = std::chrono::high_resolution_clock::now();
    point.threadId = std::hash<std::thread::id>{}(std::this_thread::get_id());
    point.metadata = metadata;

    std::lock_guard<std::mutex> lock(timePointsMutex_);
    timePoints_.push_back(point);

    // Update statistics
    updateOperationStats(operation, point.duration);

    // Clean up old points if needed
    if (timePoints_.size() > 100000) {
        timePoints_.erase(timePoints_.begin(), timePoints_.begin() + 50000);
    }
}

void PerformanceProfiler::recordAllocation(size_t size, const std::string& component, const std::string& stackTrace) {
    if (mode_ == Mode::OFF || mode_ == Mode::BASIC) return;

    AllocationRecord record;
    record.address = nullptr;  // Would need actual pointer tracking
    record.size = size;
    record.allocationTime = std::chrono::high_resolution_clock::now();
    record.component = component;
    record.stackTrace = stackTrace;
    record.isFreed = false;

    std::lock_guard<std::mutex> lock(allocationsMutex_);
    allocations_.push_back(record);
}

void PerformanceProfiler::recordDeallocation(void* address, const std::string& component) {
    if (mode_ == Mode::OFF || mode_ == Mode::BASIC) return;

    std::lock_guard<std::mutex> lock(allocationsMutex_);
    for (auto& alloc : allocations_) {
        if (alloc.address == address && !alloc.isFreed) {
            alloc.isFreed = true;
            break;
        }
    }
}

void PerformanceProfiler::profileStepStart(SimulationStep step, Timestamp time) {
    recordTimePoint("step_start", {
        {"step", std::to_string(step)},
        {"time", std::to_string(time)}
    });
}

void PerformanceProfiler::profileStepEnd(SimulationStep step, Timestamp time) {
    recordTimePoint("step_end", {
        {"step", std::to_string(step)},
        {"time", std::to_string(time)}
    });
}

void PerformanceProfiler::profileNeuronProcessing(size_t neuronCount) {
    if (mode_ == Mode::OFF) return;
    recordTimePoint("neuron_processing", {
        {"neuron_count", std::to_string(neuronCount)}
    });
}

void PerformanceProfiler::profileSynapseProcessing(size_t synapseCount) {
    if (mode_ == Mode::OFF) return;
    recordTimePoint("synapse_processing", {
        {"synapse_count", std::to_string(synapseCount)}
    });
}

void PerformanceProfiler::profilePlasticityApplication(const std::string& ruleName, size_t synapseCount) {
    if (mode_ == Mode::OFF) return;
    recordTimePoint("plasticity_application", {
        {"rule", ruleName},
        {"synapse_count", std::to_string(synapseCount)}
    });
}

void PerformanceProfiler::profileMemorySystem(const std::string& systemName, size_t memoryBytes) {
    if (mode_ == Mode::OFF) return;
    recordTimePoint("memory_system", {
        {"system", systemName},
        {"memory_bytes", std::to_string(memoryBytes)}
    });
}

void PerformanceProfiler::profileEventQueue(const std::string& eventType, size_t queueSize, double processingTime) {
    if (mode_ == Mode::OFF) return;
    recordTimePoint("event_queue_processing", {
        {"event_type", eventType},
        {"queue_size", std::to_string(queueSize)},
        {"processing_time", std::to_string(procesingTime)}
    });
}

void PerformanceProfiler::profileSpikeProcessing(size_t spikeCount, double processingTime) {
    if (mode_ == Mode::OFF) return;
    recordTimePoint("spike_processing", {
        {"spike_count", std::to_string(spikeCount)},
        {"processing_time", std::to_string(processingTime)}
    });
}

void PerformanceProfiler::profileConcurrentOperation(const std::string& operation, int threadId, double duration) {
    if (mode_ == Mode::OFF) return;
    recordTimePoint("concurrent_operation", {
        {"operation", operation},
        {"thread_id", std::to_string(threadId)},
        {"duration", std::to_string(duration)}
    });

    updateThreadStats(threadId, std::chrono::duration<double>(duration));
}

void PerformanceProfiler::setMemoryMonitor(MemoryMonitor* monitor) {
    memoryMonitor_ = monitor;
}

void PerformanceProfiler::setEventProfiler(EventProfiler* eventProfiler) {
    eventProfiler_ = eventProfiler;
}

const OperationStats* PerformanceProfiler::getOperationStats(const std::string& operationName) const {
    std::lock_guard<std::mutex> lock(statsMutex_);
    auto it = operationStats_.find(operationName);
    if (it != operationStats_.end()) {
        return &it->second;
    }
    return nullptr;
}

double PerformanceProfiler::getTotalSimulationTime() const {
    if (!running_ || timePoints_.empty()) return 0.0;
    return (std::chrono::high_resolution_clock::now() - startTime_).count();
}

size_t PerformanceProfiler::getTotalTimePoints() const {
    std::lock_guard<std::mutex> lock(timePointsMutex_);
    return timePoints_.size();
}

double PerformanceProfiler::getMemoryLeakRate() const {
    std::lock_guard<std::mutex> lock(allocationsMutex_);
    size_t totalAllocations = allocations_.size();
    size_t leaked = 0;
    for (const auto& alloc : allocations_) {
        if (!alloc.isFreed) leaked++;
    }
    return totalAllocations > 0 ? static_cast<double>(leaked) / totalAllocations : 0.0;
}

std::vector<PerformanceProfiler::ThreadStats> PerformanceProfiler::getThreadStats() const {
    std::lock_guard<std::mutex> lock(threadStatsMutex_);
    return threadStats_;
}

void PerformanceProfiler::exportToJSON(const std::string& filename) const {
    std::ofstream file(filename);
    if (!file.is_open()) {
        throw std::runtime_error("Cannot open file for writing: " + filename);
    }
    exportToJSONInternal(file);
}

void PerformanceProfiler::exportToCSV(const std::string& filename) const {
    std::ofstream file(filename);
    if (!file.is_open()) {
        throw std::runtime_error("Cannot open file for writing: " + filename);
    }
    exportToCSVInternal(file);
}

std::string PerformanceProfiler::generateReport() const {
    std::stringstream ss;
    generateReportInternal(ss);
    return ss.str();
}

void PerformanceProfiler::start() {
    if (mode_ == Mode::OFF) return;
    running_ = true;
    startTime_ = std::chrono::high_resolution_clock::now();
}

void PerformanceProfiler::stop() {
    running_ = false;
}

void PerformanceProfiler::reset() {
    std::lock_guard<std::mutex> lock1(timePointsMutex_);
    std::lock_guard<std::mutex> lock2(statsMutex_);
    std::lock_guard<std::mutex> lock3(threadStatsMutex_);
    timePoints_.clear();
    operationStats_.clear();
    threadStats_.clear();
    allocations_.clear();
}

bool PerformanceProfiler::isRunning() const {
    return running_;
}

void PerformanceProfiler::updateOperationStats(const std::string& operation, std::chrono::duration<double> duration) {
    std::lock_guard<std::mutex> lock(statsMutex_);

    auto& stats = operationStats_[operation];
    stats.name = operation;
    stats.callCount++;
    stats.totalDuration += duration;
    stats.minDuration = std::min(stats.minDuration, duration);
    stats.maxDuration = std::max(stats.maxDuration, duration);
    stats.avgDuration = stats.totalDuration / stats.callCount;

    // Calculate standard deviation (simplified)
    if (stats.samples.size() > 1000) {
        stats.samples.erase(stats.samples.begin());
    }
    stats.samples.push_back(duration.count());
    if (stats.samples.size() >= 2) {
        double mean = stats.avgDuration.count();
        double sumSq = 0.0;
        for (double sample : stats.samples) {
            double diff = sample - mean;
            sumSq += diff * diff;
        }
        stats.stdDev = std::sqrt(sumSq / (stats.samples.size() - 1));
    }
}

void PerformanceProfiler::cleanupFreedAllocations() {
    std::lock_guard<std::mutex> lock(allocationsMutex_);
    // Remove freed allocations to prevent memory leaks in the profiler itself
    allocations_.erase(
        std::remove_if(allocations_.begin(), allocations_.end(),
            [](const AllocationRecord& r) { return r.isFreed; }),
        allocations_.end()
    );
}

void PerformanceProfiler::updateThreadStats(int threadId, std::chrono::duration<double> duration) {
    std::lock_guard<std::mutex> lock(threadStatsMutex_);

    // Find or create thread stats
    auto it = std::find_if(threadStats_.begin(), threadStats_.end(),
        [threadId](const ThreadStats& ts) { return ts.threadId == threadId; });n
    if (it == threadStats_.end()) {
        ThreadStats stats;
        stats.threadId = threadId;
        stats.activeTime = duration;
        stats.utilization = 100.0;  // Simplified
        threadStats_.push_back(stats);
        it = threadStats_.end() - 1;
    } else {
        it->activeTime += duration;
    }

    // Update utilization (simplified calculation)
    it->utilization = std::min(100.0, it->utilization + 10.0);
}

void PerformanceProfiler::exportToJSONInternal(std::ostream& output) const {
    nlohmann::json jsonData;

    // Export time points
    jsonData["time_points"] = nlohmann::json::array();
    for (const auto& point : timePoints_) {
        nlohmann::json pointJson;
        pointJson["operation"] = point.operation;
        pointJson["duration"] = point.duration.count();
        pointJson["thread_id"] = point.threadId;
        jsonData["time_points"].push_back(pointJson);
    }

    // Export operation statistics
    jsonData["operation_stats"] = nlohmann::json::array();
    for (const auto& [name, stats] : operationStats_) {
        nlohmann::json statsJson;
        statsJson["name"] = stats.name;
        statsJson["call_count"] = stats.callCount;
        statsJson["total_duration"] = stats.totalDuration.count();
        statsJson["avg_duration"] = stats.avgDuration.count();
        statsJson["min_duration"] = stats.minDuration.count();
        statsJson["max_duration"] = stats.maxDuration.count();
        statsJson["std_dev"] = stats.stdDev;
        statsJson["memory_allocated"] = stats.memoryAllocated;
        statsJson["memory_deallocated"] = stats.memoryDeallocated;
        jsonData["operation_stats"].push_back(statsJson);
    }

    // Export memory info
    jsonData["memory_allocations"] = nlohmann::json::array();
    for (const auto& alloc : allocations_) {
        nlohmann::json allocJson;
        allocJson["size"] = alloc.size;
        allocJson["component"] = alloc.component;
        allocJson["is_freed"] = alloc.isFreed;
        jsonData["memory_allocations"].push_back(allocJson);
    }

    output << jsonData.dump(2);
}

void PerformanceProfiler::exportToCSVInternal(std::ostream& output) const {
    // Export operation statistics in CSV format
    output << "Operation,Call Count,Total Duration,Average Duration,Min Duration,Max Duration,Std Dev\n";
    for (const auto& [name, stats] : operationStats_) {
        output << stats.name << "," << stats.callCount << ","
               << stats.totalDuration.count() << ","
               << stats.avgDuration.count() << ","
               << stats.minDuration.count() << ","
               << stats.maxDuration.count() << ","
               << stats.stdDev << "\n";
    }
}

void PerformanceProfiler::generateReportInternal(std::ostream& output) const {
    output << "=== Performance Profiling Report ===\n";
    output << "Mode: " << (mode_ == Mode::OFF ? "OFF" : mode_ == Mode::BASIC ? "BASIC" :
                                    mode_ == Mode::DETAILED ? "DETAILED" : "COMPREHENSIVE") << "\n";
    output << "Total Time Points: " << timePoints_.size() << "\n";
    output << "Total Simulation Time: " << getTotalSimulationTime() << " seconds\n";
    output << "Memory Leak Rate: " << getMemoryLeakRate() * 100 << "%\n\n";

    output << "=== Operation Statistics ===\n";
    for (const auto& [name, stats] : operationStats_) {
        output << "Operation: " << stats.name << "\n";
        output << "  Calls: " << stats.callCount << "\n";
        output << "  Total Duration: " << stats.totalDuration.count() << "s\n";
        output << "  Average Duration: " << stats.avgDuration.count() << "s\n";
        output << "  Min Duration: " << stats.minDuration.count() << "s\n";
        output << "  Max Duration: " << stats.maxDuration.count() << "s\n";
        output << "  Std Dev: " << stats.stdDev << "\n\n";
    }

    output << "=== Thread Statistics ===\n";
    for (const auto& thread : threadStats_) {
        output << "Thread " << thread.threadId << ":\n";
        output << "  Active Time: " << thread.activeTime.count() << "s\n";
        output << "  Utilization: " << thread.utilization << "%\n";
    }
}
