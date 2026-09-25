#include "Performance.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <numeric>
#include <iomanip>
#include <nlohmann/json.hpp>
#include <thread>
#include <cstring>

namespace nlm {

// PerformanceMonitor implementation

PerformanceMonitor::PerformanceMonitor()
    : enabled_(false)
    , isRecording_(false)
    , sampleInterval_(NLM_PERF_METRICS_SAMPLE_INTERVAL)
    , nextSampleId_(0)
    , nextEventId_(0)
{
    if (isEnabled()) {
        initialize();
    }
}

PerformanceMonitor::~PerformanceMonitor() {
    shutdown();
}

bool PerformanceMonitor::initialize() {
    if (!enabled_) return false;
    
    clearHistory();
    return true;
}

void PerformanceMonitor::shutdown() {
    isRecording_ = false;
    clearHistory();
}

void PerformanceMonitor::startRecording() {
    if (!enabled_) return;
    isRecording_ = true;
}

void PerformanceMonitor::stopRecording() {
    isRecording_ = false;
}

PerfSample PerformanceMonitor::takeSnapshot() {
    PerfSample sample(nextSampleId++, 0);  // TODO: Get actual thread ID
    
    if (enabled_) {
        updateCurrentMetrics();
        sample.metrics = currentMetrics_;
    }
    
    std::lock_guard<std::mutex> lock(metricsMutex_);
    history_.push_back(sample);
    
    // Limit history size
    if (history_.size() > NLM_PERF_MAX_HISTORY_POINTS) {
        history_.erase(history_.begin());
    }
    
    return sample;
}

void PerformanceMonitor::recordEvent(const PerfEvent& event) {
    if (!enabled_) return;
    
    std::lock_guard<std::mutex> lock(eventsMutex_);
    events_.push_back(event);
    
    // Update current metrics based on event
    updateCurrentMetricsFromEvent(event);
    
    // Limit events size
    if (events_.size() > 100000) {
        events_.erase(events_.begin(), events_.begin() + 10000);
    }
}

void PerformanceMonitor::updateCurrentMetrics() {
    if (!enabled_) return;
    
    std::lock_guard<std::mutex> lock(metricsMutex_);
    
    // Update metrics from registered updaters
    for (const auto& updater : metricsUpdaters_) {
        updater(currentMetrics_);
    }
    
    // Add thread-local metrics
    for (size_t threadId = 0; threadId < 32; ++threadId) {
        auto& threadPerfData = ThreadLocalPerfData::getInstance();
        auto threadMetrics = threadPerfData.getMetrics();
        
        currentMetrics_.allocations += threadMetrics.allocations;
        currentMetrics_.deallocations += threadMetrics.deallocations;
        currentMetrics_.totalAllocationTime += threadMetrics.totalAllocationTime;
        currentMetrics_.totalDeallocationTime += threadMetrics.totalDeallocationTime;
        currentMetrics_.totalMemoryUsed += threadMetrics.totalMemoryUsed;
        if (threadMetrics.peakMemoryUsage > currentMetrics_.peakMemoryUsage) {
            currentMetrics_.peakMemoryUsage = threadMetrics.peakMemoryUsage;
        }
        currentMetrics_.lockContentionCount += threadMetrics.lockContentionCount;
        currentMetrics_.cacheMisses += threadMetrics.cacheMisses;
        currentMetrics_.simdOperations += threadMetrics.simdOperations;
        currentMetrics_.scalarFallbacks += threadMetrics.scalarFallbacks;
    }
}

void PerformanceMonitor::updateCurrentMetricsFromEvent(const PerfEvent& event) {
    // Update metrics based on event type
    switch (event.type) {
        case PerfEventType::Allocation:
            currentMetrics_.allocations++;
            break;
        case PerfEventType::Deallocation:
            currentMetrics_.deallocations++;
            break;
        case PerfEventType::LockAcquire:
            currentMetrics_.lockContentionCount++;
            break;
        case PerfEventType::SIMDOperation:
            currentMetrics_.simdOperations++;
            break;
        case PerfEventType::CacheMiss:
            currentMetrics_.cacheMisses++;
            break;
        default:
            break;
    }
}

std::vector<PerfSample> PerformanceMonitor::getHistory(size_t maxSamples) const {
    std::lock_guard<std::mutex> lock(metricsMutex_);
    
    if (maxSamples >= history_.size()) {
        return history_;
    }
    
    size_t startIndex = history_.size() - maxSamples;
    return std::vector<PerfSample>(history_.begin() + startIndex, history_.end());
}

std::vector<PerfEvent> PerformanceMonitor::getEvents(const std::string& nameFilter,
                                                    PerfEventType typeFilter,
                                                    size_t maxEvents) const {
    std::lock_guard<std::mutex> lock(eventsMutex_);
    
    std::vector<PerfEvent> filteredEvents;
    filteredEvents.reserve(events_.size());
    
    for (const auto& event : events_) {
        if (nameFilter.empty() || event.name.find(nameFilter) != std::string::npos) {
            if (typeFilter == PerfEventType::Custom || event.type == typeFilter) {
                filteredEvents.push_back(event);
            }
        }
    }
    
    // Limit events
    if (filteredEvents.size() > maxEvents) {
        filteredEvents.resize(maxEvents);
    }
    
    return filteredEvents;
}

void PerformanceMonitor::clearHistory() {
    std::lock_guard<std::mutex> lock(metricsMutex_);
    history_.clear();
    events_.clear();
    currentMetrics_.reset();
}

std::string PerformanceMonitor::exportToJSON() const {
    nlohmann::json jsonData;
    
    jsonData["version"] = "1.0";
    jsonData["timestamp"] = std::chrono::high_resolution_clock::now().time_since_epoch().count();
    jsonData["recording"] = isRecording_;
    
    // Add metrics
    jsonData["metrics"] = {
        {"allocations", currentMetrics_.allocations},
        {"deallocations", currentMetrics_.deallocations},
        {"allocationsFailed", currentMetrics_.allocationsFailed},
        {"totalAllocationTime", currentMetrics_.totalAllocationTime},
        {"totalDeallocationTime", currentMetrics_.totalDeallocationTime},
        {"maxAllocationTime", currentMetrics_.maxAllocationTime},
        {"maxDeallocationTime", currentMetrics_.maxDeallocationTime},
        {"totalMemoryUsed", currentMetrics_.totalMemoryUsed},
        {"peakMemoryUsage", currentMetrics_.peakMemoryUsage},
        {"fragmentationRatio", currentMetrics_.fragmentationRatio},
        {"activeThreads", currentMetrics_.activeThreads},
        {"lockContentionCount", currentMetrics_.lockContentionCount},
        {"cacheMisses", currentMetrics_.cacheMisses},
        {"simdOperations", currentMetrics_.simdOperations},
        {"scalarFallbacks", currentMetrics_.scalarFallbacks},
        {"operationsPerSecond", currentMetrics_.operationsPerSecond},
        {"memoryBandwidthMBps", currentMetrics_.memoryBandwidthMBps},
        {"healthy", currentMetrics_.healthy},
        {"errorCount", currentMetrics_.errorCount}
    };
    
    // Add history samples (last 100)
    auto samples = getHistory(100);
    nlohmann::json samplesJson = nlohmann::json::array();
    for (const auto& sample : samples) {
        nlohmann::json sampleJson;
        sampleJson["sampleId"] = sample.sampleId;
        sampleJson["threadId"] = sample.threadId;
        sampleJson["threadName"] = sample.threadName;
        sampleJson["timestamp"] = sample.timestamp.time_since_epoch().count();
        sampleJson["allocations"] = sample.metrics.allocations;
        sampleJson["deallocations"] = sample.metrics.deallocations;
        sampleJson["memoryUsage"] = sample.metrics.totalMemoryUsed;
        samplesJson.push_back(sampleJson);
    }
    jsonData["samples"] = samplesJson;
    
    // Add recent events
    auto events = getEvents("", PerfEventType::Custom, 50);
    nlohmann::json eventsJson = nlohmann::json::array();
    for (const auto& event : events) {
        nlohmann::json eventJson;
        eventJson["eventId"] = event.name;
        eventJson["type"] = static_cast<int>(event.type);
        eventJson["name"] = event.name;
        eventJson["threadId"] = event.threadId;
        eventJson["timestamp"] = event.timestamp.time_since_epoch().count();
        eventsJson.push_back(eventJson);
    }
    jsonData["events"] = eventsJson;
    
    return jsonData.dump(2);
}

std::string PerformanceMonitor::exportToCSV() const {
    std::ostringstream csv;
    
    // Header
    csv << "SampleID,Timestamp,ThreadID,ThreadName,Allocations,Deallocations,";
    csv << "TotalMemoryUsed,PeakMemoryUsage,LockContention,CacheMisses,";
    csv << "SIMDOperations,ScalarFallbacks,OperationsPerSec,MemoryBandwidthMBps,\n";
    
    // Data
    auto samples = getHistory();
    for (const auto& sample : samples) {
        csv << sample.sampleId << ",";
        csv << sample.timestamp.time_since_epoch().count() << ",";
        csv << sample.threadId << ",";
        csv << sample.threadName << ",";
        csv << sample.metrics.allocations << ",";
        csv << sample.metrics.deallocations << ",";
        csv << sample.metrics.totalMemoryUsed << ",";
        csv << sample.metrics.peakMemoryUsage << ",";
        csv << sample.metrics.lockContentionCount << ",";
        csv << sample.metrics.cacheMisses << ",";
        csv << sample.metrics.simdOperations << ",";
        csv << sample.metrics.scalarFallbacks << ",";
        csv << sample.metrics.operationsPerSecond << ",";
        csv << sample.metrics.memoryBandwidthMBps << ",";
        csv << "\n";
    }
    
    return csv.str();
}

std::string PerformanceMonitor::generateReport() const {
    std::ostringstream report;
    
    report << "=== Performance Report ===\n";
    report << "Generated at: " << std::chrono::high_resolution_clock::now().time_since_epoch().count() << "\n";
    report << "Recording active: " << (isRecording_ ? "Yes" : "No") << "\n\n";
    
    report << "=== Current Metrics ===\n";
    report << "Allocation Count: " << currentMetrics_.allocations << "\n";
    report << "Deallocation Count: " << currentMetrics_.deallocations << "\n";
    report << "Allocation Failure Rate: " << std::fixed << std::setprecision(2)
           << currentMetrics_.allocationFailureRate() << "%\n";
    report << "Total Memory Usage: " << currentMetrics_.totalMemoryUsed << " bytes\n";
    report << "Peak Memory Usage: " << currentMetrics_.peakMemoryUsage << " bytes\n";
    report << "Fragmentation Ratio: " << std::fixed << std::setprecision(4)
           << currentMetrics_.fragmentationRatio / 10000.0 << "\n";
    report << "Active Threads: " << currentMetrics_.activeThreads << "\n";
    report << "Lock Contention Events: " << currentMetrics_.lockContentionCount << "\n";
    report << "Cache Misses: " << currentMetrics_.cacheMisses << "\n";
    report << "SIMD Operations: " << currentMetrics_.simdOperations << "\n";
    report << "Scalar Fallbacks: " << currentMetrics_.scalarFallbacks << "\n";
    report << "SIMD Efficiency: " << std::fixed << std::setprecision(2)
           << currentMetrics_.simdEfficiency() << "%\n";
    report << "Operations Per Second: " << std::fixed << std::setprecision(2)
           << currentMetrics_.operationsPerSecond << "\n";
    report << "Memory Bandwidth: " << std::fixed << std::setprecision(2)
           << currentMetrics_.memoryBandwidthMBps << " MB/s\n";
    report << "Health Status: " << (currentMetrics_.healthy ? "Healthy" : "Unhealthy") << "\n";
    report << "Error Count: " << currentMetrics_.errorCount << "\n";
    
    report << "\n=== Historical Data ===\n";
    auto samples = getHistory();
    if (samples.size() > 0) {
        report << "Total Samples: " << samples.size() << "\n";
        report << "Time Range: " << (samples.front().timestamp.time_since_epoch().count())
               << " - " << (samples.back().timestamp.time_since_epoch().count()) << "\n";
        
        // Calculate averages
        double totalAllocations = 0.0, totalDeallocations = 0.0, totalMemory = 0.0;
        for (const auto& sample : samples) {
            totalAllocations += sample.metrics.allocations;
            totalDeallocations += sample.metrics.deallocations;
            totalMemory += sample.metrics.totalMemoryUsed;
        }
        
        report << "Average Allocations: " << totalAllocations / samples.size() << "\n";
        report << "Average Deallocations: " << totalDeallocations / samples.size() << "\n";
        report << "Average Memory Usage: " << totalMemory / samples.size() << " bytes\n";
    }
    
    report << "\n=== Performance Summary ===\n";
    
    if (currentMetrics_.healthy) {
        report << "✓ Performance is within acceptable ranges\n";
    } else {
        report << "✗ Performance issues detected\n";
    }
    
    if (currentMetrics_.allocationFailureRate() > 1.0) {
        report << "⚠ High allocation failure rate: " << currentMetrics_.allocationFailureRate() << "%\n";
    }
    
    if (currentMetrics_.simdEfficiency() < 80.0) {
        report << "⚠ Low SIMD efficiency: " << currentMetrics_.simdEfficiency() << "%\n";
    }
    
    if (currentMetrics_.peakMemoryUsage > 1024 * 1024 * 1024) {  // 1GB
        report << "⚠ High memory usage: " << currentMetrics_.peakMemoryUsage / (1024.0 * 1024.0 * 1024.0)
               << " GB\n";
    }
    
    return report.str();
}

PerformanceMonitor::PerformanceStats PerformanceMonitor::calculateStats(const std::vector<PerfSample>& samples) const {
    PerformanceStats stats;
    
    if (samples.empty()) return stats;
    
    // Calculate time range
    auto firstTime = samples.front().timestamp;
    auto lastTime = samples.back().timestamp;
    auto duration = std::chrono::duration<double>(lastTime - firstTime).count();
    stats.totalTime = duration;
    
    // Calculate averages
    double totalAllocations = 0.0, totalDeallocations = 0.0, totalMemory = 0.0;
    double totalLockContention = 0.0, totalCacheMisses = 0.0;
    double totalSIMD = 0.0, totalScalar = 0.0;
    
    for (const auto& sample : samples) {
        totalAllocations += sample.metrics.allocations;
        totalDeallocations += sample.metrics.deallocations;
        totalMemory += sample.metrics.totalMemoryUsed;
        totalLockContention += sample.metrics.lockContentionCount;
        totalCacheMisses += sample.metrics.cacheMisses;
        totalSIMD += sample.metrics.simdOperations;
        totalScalar += sample.metrics.scalarFallbacks;
    }
    
    size_t count = samples.size();
    
    stats.cpuUsage = (totalAllocations + totalDeallocations) / (duration * 1000.0); // Approximate
    stats.memoryUsageMB = totalMemory / (1024.0 * 1024.0);
    stats.allocationRate = totalAllocations / duration;
    stats.deallocationRate = totalDeallocations / duration;
    stats.throughput = (totalAllocations + totalDeallocations) / duration;
    
    if (totalLockContention + totalCacheMisses > 0) {
        double totalMisses = totalLockContention + totalCacheMisses;
        stats.cacheHitRate = (totalMisses - totalMisses) / totalMisses; // Placeholder
    }
    
    if (totalSIMD + totalScalar > 0) {
        stats.simdEfficiency = totalSIMD / (totalSIMD + totalScalar);
    }
    
    stats.peakActiveThreads = 0; // Would need to track maximum
    stats.totalEvents = events_.size();
    
    return stats;
}

PerformanceMonitor::PerformanceComparison PerformanceMonitor::comparePerformance(size_t window1Samples,
                                                                                   size_t window2Samples) const {
    PerformanceComparison comparison;
    
    if (window1Samples == 0 || window2Samples == 0) {
        return comparison;
    }
    
    auto samples = getHistory();
    if (samples.size() < window1Samples + window2Samples) {
        return comparison;
    }
    
    // Calculate statistics for both windows
    auto stats1 = calculateStats(std::vector<PerfSample>(samples.end() - window1Samples - window2Samples,
                                                        samples.end() - window2Samples));
    auto stats2 = calculateStats(std::vector<PerfSample>(samples.end() - window2Samples,
                                                        samples.end()));
    
    // Calculate changes
    if (stats1.totalTime > 0.0) {
        comparison.performanceChange = (stats2.throughput - stats1.throughput) / stats1.throughput * 100.0;
        comparison.throughputChange = (stats2.throughput - stats1.throughput) / stats1.throughput * 100.0;
        comparison.memoryChange = (stats2.memoryUsageMB - stats1.memoryUsageMB) / stats1.memoryUsageMB * 100.0;
    }
    
    // Determine trend
    if (comparison.performanceChange > 5.0) {
        comparison.trend = "improving";
    } else if (comparison.performanceChange < -5.0) {
        comparison.trend = "degrading";
    } else {
        comparison.trend = "stable";
    }
    
    return comparison;
}

PerformanceMonitor::ScopedProfiler::ScopedProfiler(PerformanceMonitor& monitor, const std::string& regionName)
    : monitor_(monitor)
    , regionName_(regionName)
    , duration_(0.0)
    , stopped_(false) {
    startTime_ = std::chrono::high_resolution_clock::now();
}

PerformanceMonitor::ScopedProfiler::~ScopedProfiler() {
    if (!stopped_) {
        stop();
    }
}

void PerformanceMonitor::ScopedProfiler::stop() {
    if (stopped_) return;
    
    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration<double, std::milli>(endTime - startTime_).count();
    duration_ = duration;
    stopped_ = true;
    
    // Record the profiling event
    PerfEvent event(PerfEventType::Custom, regionName_);
    event.data.custom.customData = "Duration: " + std::to_string(duration) + " ms";
    
    monitor_.recordEvent(event);
}

void PerformanceMonitor::registerMetricsUpdater(const std::string& name, MetricsUpdater updater) {
    metricsUpdaters_.push_back(updater);
}

void PerformanceMonitor::updateMetrics() {
    updateCurrentMetrics();
}

// MemoryTracker implementation

MemoryTracker::MemoryTracker()
    : leakDetectionEnabled_(true) {
}

MemoryTracker::~MemoryTracker() {
    clear();
}

void* MemoryTracker::trackAllocation(size_t size, const std::string& sourceFile,
                                    size_t sourceLine, const std::string& allocationType) {
    if (!leakDetectionEnabled_) return nullptr;
    
    AllocationInfo info;
    info.address = reinterpret_cast<void*>(new char[size]);
    info.size = size;
    info.sourceFile = sourceFile;
    info.sourceLine = sourceLine;
    info.allocationType = allocationType;
    info.timestamp = std::chrono::high_resolution_clock::now();
    
    std::lock_guard<std::mutex> lock(mutex_);
    allocations_.push_back(info);
    allocatedAddresses_.insert(info.address);
    
    return info.address;
}

void MemoryTracker::trackDeallocation(void* address, const std::string& sourceFile,
                                     size_t sourceLine, const std::string& allocationType) {
    if (!leakDetectionEnabled_) return;
    
    std::lock_guard<std::mutex> lock(mutex_);
    
    auto it = std::find_if(allocations_.begin(), allocations_.end(),
                          [address](const AllocationInfo& info) { return info.address == address; });n
    if (it != allocations_.end()) {
        allocatedAddresses_.erase(address);
        allocations_.erase(it);
    }
}

MemoryTracker::LeakReport MemoryTracker::checkLeaks() const {
    LeakReport report;
    
    std::lock_guard<std::mutex> lock(mutex_);
    
    report.leakedPointers.reserve(allocatedAddresses_.size());
    report.totalLeakedBytes = 0;
    report.leakTypes.reserve(allocatedAddresses_.size());
    
    for (const auto& allocInfo : allocations_) {
        report.leakedPointers.push_back(allocInfo.address);
        report.totalLeakedBytes += allocInfo.size;
        report.leakTypes.push_back(allocInfo.allocationType);
    }
    
    return report;
}

MemoryTracker::AllocationStats MemoryTracker::getAllocationStats() const {
    AllocationStats stats;
    
    std::lock_guard<std::mutex> lock(mutex_);
    
    stats.totalAllocations = allocations_.size();
    stats.currentAllocated = allocatedAddresses_.size();
    stats.totalAllocatedBytes = 0;
    
    std::unordered_map<std::string, size_t> typeCounts;
    
    for (const auto& allocInfo : allocations_) {
        stats.totalAllocatedBytes += allocInfo.size;
        typeCounts[allocInfo.allocationType]++;
    }
    
    stats.allocationsByType = std::move(typeCounts);
    
    if (stats.totalAllocations > 0) {
        stats.averageAllocationSize = (double)stats.totalAllocatedBytes / stats.totalAllocations;
    }
    
    return stats;
}

void MemoryTracker::clear() {
    std::lock_guard<std::mutex> lock(mutex_);
    allocations_.clear();
    allocatedAddresses_.clear();
}

} // namespace nlm