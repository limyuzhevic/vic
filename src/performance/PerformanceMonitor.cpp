// Performance Monitoring and Profiling System - Implementation File
#include "PerformanceMonitor.hpp"
#include <thread>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <numeric>
#include <sys/resource.h>
#include <unistd.h>

namespace nlm {

// PerformanceMonitor implementation
void PerformanceMonitor::startEvent(const std::string& operation) {
    if (!enabled_) return;
    
    PerformanceEvent event(operation);
    addEvent(event);
}

void PerformanceMonitor::endEvent(const std::string& operation, uint64_t memoryUsage) {
    if (!enabled_) return;
    
    auto it = events_.rbegin();
    while (it != events_.rend()) {
        if (it->operation == operation && it->endTime == Timestamp(0)) {
            it->endTime = getCurrentTimestamp();
            it->duration = (it->endTime - it->startTime).count() / 1000000.0;
            it->memoryUsage = memoryUsage;
            break;
        }
        ++it;
    }
}

void PerformanceMonitor::addEvent(const PerformanceEvent& event) {
    if (!enabled_) return;
    
    // Check if there's an existing event with the same operation that hasn't been ended
    auto it = std::find_if(events_.begin(), events_.end(),
        [&event](const PerformanceEvent& e) {
            return e.operation == event.operation && e.endTime == Timestamp(0);
        });
    
    if (it != events_.end()) {
        *it = event;
    } else {
        events_.push_back(event);
        eventsByOperation_[event.operation].push_back(event);
    }
    
    // Sample system metrics if sampling interval has passed
    auto currentTime = getCurrentTimestamp();
    if ((currentTime - lastSampleTime_).count() >= static_cast<Timestamp>(samplingInterval_ * 1000000)) {
        sampleSystemMetrics();
        lastSampleTime_ = currentTime;
    }
}

void PerformanceMonitor::recordMetric(MetricType type, const std::string& name, double value, const std::string& unit) {
    if (!enabled_) return;
    
    Metric metric(type, name, value, unit);
    metrics_.push_back(metric);
    
    auto currentTime = getCurrentTimestamp();
    metricsByTime_[currentTime].push_back(metric);
}

void PerformanceMonitor::recordSystemMetrics() {
    if (!enabled_) return;
    
    // Record CPU usage
    double cpuUsage = getSystemCPUUsage();
    recordMetric(MetricType::CPU_USAGE, "cpu_usage", cpuUsage, "%");
    
    // Record memory usage
    uint64_t memoryUsage = getSystemMemoryUsage();
    recordMetric(MetricType::MEMORY_USAGE, "memory_usage", memoryUsage, "bytes");
    
    // Record thread count
    uint32_t threadCount = getSystemThreadCount();
    recordMetric(MetricType::THREAD_COUNT, "thread_count", threadCount, "count");
    
    // Record process count
    uint32_t processCount = getSystemProcessCount();
    recordMetric(MetricType::PROCESS_COUNT, "process_count", processCount, "count");
}

void PerformanceMonitor::setCurrentThreadId(const std::string& threadId) {
    currentThreadId_ = threadId;
}

std::string PerformanceMonitor::getCurrentThreadId() const {
    if (currentThreadId_.empty()) {
        return getSystemThreadId();
    }
    return currentThreadId_;
}

PerformanceMonitor::PerformanceReport PerformanceMonitor::generateReport() const {
    PerformanceReport report;
    
    if (events_.empty()) return report;
    
    report.startTime = events_.front().startTime;
    report.endTime = getCurrentTimestamp();
    
    for (const auto& event : events_) {
        if (event.endTime == Timestamp(0)) continue;
        
        report.operationCount++;
        report.operationFrequency[event.operation]++;
        report.operationDurations[event.operation] += event.duration;
        report.operationMemoryUsage[event.operation] += event.memoryUsage;
        
        if (event.memoryUsage > report.peakMemoryUsage) {
            report.peakMemoryUsage = event.memoryUsage;
        }
        
        if (event.threadCount > report.peakThreadCount) {
            report.peakThreadCount = event.threadCount;
        }
        
        report.totalMemoryUsage += event.memoryUsage;
    }
    
    report.calculateSummary();
    return report;
}

std::vector<PerformanceMonitor::PerformanceEvent> PerformanceMonitor::getEvents(const std::string& operation) const {
    auto it = eventsByOperation_.find(operation);
    if (it != eventsByOperation_.end()) {
        return it->second;
    }
    return std::vector<PerformanceEvent>();
}

std::vector<PerformanceMonitor::PerformanceEvent> PerformanceMonitor::getAllEvents() const {
    return events_;
}

bool PerformanceMonitor::saveReport(const std::string& filename) const {
    PerformanceReport report = generateReport();
    
    std::ofstream file(filename);
    if (!file.is_open()) {
        return false;
    }
    
    file << report.generateReport();
    return true;
}

bool PerformanceMonitor::saveMetrics(const std::string& filename) const {
    std::ofstream file(filename);
    if (!file.is_open()) {
        return false;
    }
    
    file << "Timestamp,MetricType,Name,Value,Unit" << std::endl;
    for (const auto& metric : metrics_) {
        file << metric.timestamp.count() << ","
             << static_cast<int>(metric.type) << ","
             << metric.name << ","
             << metric.value << ","
             << metric.unit << std::endl;
    }
    
    return true;
}

bool PerformanceMonitor::exportData(const std::string& filename) const {
    // Export both events and metrics
    std::ofstream file(filename);
    if (!file.is_open()) {
        return false;
    }
    
    file << "Performance Data Export" << std::endl;
    file << "=======================" << std::endl;
    file << std::endl;
    
    file << "Events:" << std::endl;
    for (const auto& event : events_) {
        file << "  Operation: " << event.operation << std::endl;
        file << "    Start: " << event.startTime.count() << std::endl;
        file << "    End: " << (event.endTime == Timestamp(0) ? "In Progress" : std::to_string(event.endTime.count())) << std::endl;
        file << "    Duration: " << event.duration << "s" << std::endl;
        file << "    Memory Usage: " << event.memoryUsage << " bytes" << std::endl;
        file << "    Thread Count: " << event.threadCount << std::endl;
        file << "    Thread ID: " << event.threadId << std::endl;
        file << std::endl;
    }
    
    file << "Metrics:" << std::endl;
    for (const auto& metric : metrics_) {
        file << "  " << static_cast<int>(metric.type) << ": " << metric.name << " = " << metric.value << " " << metric.unit << std::endl;
    }
    
    return true;
}

void PerformanceMonitor::startMemoryTracking() {
    memoryTrackingEnabled_ = true;
    updateMemoryUsage();
}

void PerformanceMonitor::stopMemoryTracking() {
    memoryTrackingEnabled_ = false;
}

uint64_t PerformanceMonitor::getCurrentMemoryUsage() const {
    return currentMemoryUsage_;
}

uint64_t PerformanceMonitor::getPeakMemoryUsage() const {
    return peakMemoryUsage_;
}

void PerformanceMonitor::startThreadTracking() {
    threadTrackingEnabled_ = true;
    updateThreadCount();
}

void PerformanceMonitor::stopThreadTracking() {
    threadTrackingEnabled_ = false;
}

uint32_t PerformanceMonitor::getCurrentThreadCount() const {
    return currentThreadCount_;
}

void PerformanceMonitor::addPerformanceAlert(const std::string& condition, const std::string& message) {
    performanceAlerts_.push_back({condition, message});
}

std::vector<std::pair<std::string, std::string>> PerformanceMonitor::getPerformanceAlerts() const {
    return performanceAlerts_;
}

void PerformanceMonitor::clear() {
    events_.clear();
    eventsByOperation_.clear();
    metrics_.clear();
    metricsByTime_.clear();
    performanceAlerts_.clear();
    currentMemoryUsage_ = 0;
    peakMemoryUsage_ = 0;
    currentThreadCount_ = 0;
    peakThreadCount_ = 0;
}

void PerformanceMonitor::sampleSystemMetrics() {
    recordSystemMetrics();
    updateMemoryUsage();
    updateThreadCount();
}

void PerformanceMonitor::updateMemoryUsage() {
    if (!memoryTrackingEnabled_) return;
    
    uint64_t newMemoryUsage = getCurrentMemoryUsage();
    currentMemoryUsage_ = newMemoryUsage;
    
    if (newMemoryUsage > peakMemoryUsage_) {
        peakMemoryUsage_ = newMemoryUsage;
    }
}

void PerformanceMonitor::updateThreadCount() {
    if (!threadTrackingEnabled_) return;
    
    uint32_t newThreadCount = getSystemThreadCount();
    currentThreadCount_ = newThreadCount;
    
    if (newThreadCount > peakThreadCount_) {
        peakThreadCount_ = newThreadCount;
    }
}

PerformanceMonitor::Timestamp PerformanceMonitor::getCurrentTimestamp() const {
    return std::chrono::duration_cast<Timestamp>(
        std::chrono::high_resolution_clock::now().time_since_epoch());
}

// System-specific implementations
#ifdef _WIN32
// Windows-specific implementations
double PerformanceMonitor::getSystemCPUUsage() const {
    // Windows CPU usage implementation
    return 0.0; // Placeholder
}

uint64_t PerformanceMonitor::getSystemMemoryUsage() const {
    // Windows memory usage implementation
    return 0; // Placeholder
}

uint32_t PerformanceMonitor::getSystemThreadCount() const {
    // Windows thread count implementation
    return 0; // Placeholder
}

uint32_t PerformanceMonitor::getSystemProcessCount() const {
    // Windows process count implementation
    return 0; // Placeholder
}
#endif

#ifdef __linux__
double PerformanceMonitor::getSystemCPUUsage() const {
    // Linux CPU usage implementation
    return 0.0; // Placeholder
}

uint64_t PerformanceMonitor::getSystemMemoryUsage() const {
    // Linux memory usage implementation
    return 0; // Placeholder
}

uint32_t PerformanceMonitor::getSystemThreadCount() const {
    // Linux thread count implementation
    return 0; // Placeholder
}

uint32_t PerformanceMonitor::getSystemProcessCount() const {
    // Linux process count implementation
    return 0; // Placeholder
}
#endif

std::string PerformanceMonitor::getSystemThreadId() {
    // Thread ID implementation
    std::ostringstream oss;
    oss << std::this_thread::get_id();
    return oss.str();
}

// PerformanceAnalyzer implementation
std::vector<std::string> PerformanceAnalyzer::findBottlenecks(const PerformanceMonitor& monitor) const {
    std::vector<std::string> bottlenecks;
    
    auto events = monitor.getAllEvents();
    for (const auto& event : events) {
        if (event.endTime == Timestamp(0)) continue;
        
        if (event.duration > 10.0) { // More than 10 seconds
            bottlenecks.push_back("Operation \"" + event.operation + "\" takes " + 
                                std::to_string(event.duration) + "s");
        }
        
        if (event.memoryUsage > 1024 * 1024 * 1024) { // More than 1GB
            bottlenecks.push_back("Operation \"" + event.operation + "\" uses " + 
                                std::to_string(event.memoryUsage / (1024 * 1024)) + "MB");
        }
    }
    
    return bottlenecks;
}

std::vector<std::string> PerformanceAnalyzer::findMemoryIssues(const PerformanceMonitor& monitor) const {
    std::vector<std::string> issues;
    
    uint64_t peakMemory = monitor.getPeakMemoryUsage();
    if (peakMemory > 2 * 1024 * 1024 * 1024) { // More than 2GB
        issues.push_back("High memory usage detected: " + 
                        std::to_string(peakMemory / (1024 * 1024)) + "MB");
    }
    
    return issues;
}

std::vector<std::string> PerformanceAnalyzer::findThreadIssues(const PerformanceMonitor& monitor) const {
    std::vector<std::string> issues;
    
    uint32_t peakThreads = monitor.getCurrentThreadCount();
    if (peakThreads > 100) { // More than 100 threads
        issues.push_back("High thread count detected: " + std::to_string(peakThreads));
    }
    
    return issues;
}

std::vector<std::string> PerformanceAnalyzer::getOptimizationRecommendations(const PerformanceMonitor& monitor) const {
    std::vector<std::string> recommendations;
    
    auto bottlenecks = findBottlenecks(monitor);
    if (!bottlenecks.empty()) {
        recommendations.push_back("Optimize long-running operations:");
        for (const auto& bottleneck : bottlenecks) {
            recommendations.push_back("  - " + bottleneck);
        }
    }
    
    auto memoryIssues = findMemoryIssues(monitor);
    if (!memoryIssues.empty()) {
        recommendations.push_back("Optimize memory usage:");
        for (const auto& issue : memoryIssues) {
            recommendations.push_back("  - " + issue);
        }
    }
    
    auto threadIssues = findThreadIssues(monitor);
    if (!threadIssues.empty()) {
        recommendations.push_back("Optimize threading:");
        for (const auto& issue : threadIssues) {
            recommendations.push_back("  - " + issue);
        }
    }
    
    return recommendations;
}

double PerformanceAnalyzer::comparePerformance(const PerformanceMonitor& monitor1, const PerformanceMonitor& monitor2) const {
    auto report1 = monitor1.generateReport();
    auto report2 = monitor2.generateReport();
    
    if (report1.totalDuration == 0.0 || report2.totalDuration == 0.0) {
        return 0.0;
    }
    
    double durationRatio = report2.totalDuration / report1.totalDuration;
    double memoryRatio = report2.peakMemoryUsage / report1.peakMemoryUsage;
    
    return (durationRatio + memoryRatio) / 2.0;
}

std::map<std::string, double> PerformanceAnalyzer::profileOperationFrequency(const PerformanceMonitor& monitor) const {
    auto report = monitor.generateReport();
    return report.operationFrequency;
}

std::map<std::string, double> PerformanceAnalyzer::profileMemoryUsage(const PerformanceMonitor& monitor) const {
    auto report = monitor.generateReport();
    return report.operationMemoryUsage;
}

std::map<std::string, double> PerformanceAnalyzer::profileCPUUsage(const PerformanceMonitor& monitor) const {
    // Extract CPU usage from metrics
    std::map<std::string, double> cpuUsage;
    
    auto events = monitor.getAllEvents();
    for (const auto& event : events) {
        if (event.endTime == Timestamp(0)) continue;
        
        // Calculate CPU usage based on duration
        cpuUsage[event.operation] = event.duration;
    }
    
    return cpuUsage;
}

bool PerformanceAnalyzer::validatePerformanceThresholds(const PerformanceMonitor& monitor,
                                                         const std::map<std::string, double>& thresholds) const {
    auto report = monitor.generateReport();
    
    for (const auto& pair : thresholds) {
        const std::string& operation = pair.first;
        double threshold = pair.second;
        
        auto it = report.operationDurations.find(operation);
        if (it != report.operationDurations.end()) {
            if (it->second > threshold) {
                return false;
            }
        }
    }
    
    return true;
}

std::string PerformanceAnalyzer::generatePerformanceSummary(const PerformanceMonitor& monitor) const {
    auto report = monitor.generateReport();
    
    std::ostringstream oss;
    oss << "Performance Summary:" << std::endl;
    oss << "===================" << std::endl;
    oss << "Total Operations: " << report.operationCount << std::endl;
    oss << "Total Duration: " << report.totalDuration << "s" << std::endl;
    oss << "Average Duration: " << (report.operationCount > 0 ? 
                                 report.totalDuration / report.operationCount : 0.0) << "s" << std::endl;
    oss << "Peak Memory Usage: " << (report.peakMemoryUsage / (1024 * 1024)) << "MB" << std::endl;
    oss << "Peak Thread Count: " << report.peakThreadCount << std::endl;
    
    oss << std::endl;
    oss << "Operation Statistics:" << std::endl;
    for (const auto& pair : report.operationFrequency) {
        oss << "  " << pair.first << ": " << pair.second << " calls, "
            << (report.operationDurations.count(pair.first) ? 
                report.operationDurations.at(pair.first) / pair.second : 0.0) << " avg duration" << std::endl;
    }
    
    return oss.str();
}

std::string PerformanceAnalyzer::generatePerformanceDetails(const PerformanceMonitor& monitor) const {
    std::ostringstream oss;
    oss << "Performance Details:" << std::endl;
    oss << "==================" << std::endl;
    
    auto events = monitor.getAllEvents();
    for (const auto& event : events) {
        if (event.endTime == Timestamp(0)) continue;
        
        oss << "Operation: " << event.operation << std::endl;
        oss << "  Duration: " << event.duration << "s" << std::endl;
        oss << "  Memory Usage: " << event.memoryUsage << " bytes" << std::endl;
        oss << "  Thread Count: " << event.threadCount << std::endl;
        oss << "  Thread ID: " << event.threadId << std::endl;
        oss << std::endl;
    }
    
    return oss.str();
}

// MemoryProfiler implementation
void MemoryProfiler::trackAllocation(size_t size, const std::string& location) {
    if (!allocationTrackingEnabled_) return;
    
    AllocationRecord record;
    record.size = size;
    record.location = location;
    record.timestamp = std::chrono::duration_cast<Timestamp>(
        std::chrono::high_resolution_clock::now().time_since_epoch());
    record.threadId = std::to_string(std::hash<std::thread::id>{}(std::this_thread::get_id()));
    
    allocationRecords_.push_back(record);
    
    // Update memory usage by location
    memoryUsageByLocation_[location] += size;
}

void MemoryProfiler::trackDeallocation(size_t size, const std::string& location) {
    if (!allocationTrackingEnabled_) return;
    
    // Update memory usage by location
    if (memoryUsageByLocation_.find(location) != memoryUsageByLocation_.end()) {
        memoryUsageByLocation_[location] -= size;
    }
}

void MemoryProfiler::trackMemoryUsage() {
    // Calculate current memory usage
    size_t currentUsage = calculateTotalMemoryUsage();
    // In real implementation, would update current usage
}

std::map<std::string, size_t> MemoryProfiler::getMemoryUsageByLocation() const {
    return memoryUsageByLocation_;
}

std::vector<std::pair<size_t, std::string>> MemoryProfiler::getMemoryLeakCandidates() const {
    return findMemoryLeaks();
}

double MemoryProfiler::getMemoryUsageRatio() const {
    size_t totalUsage = calculateTotalMemoryUsage();
    // In real implementation, would get total system memory
    return static_cast<double>(totalUsage) / 1024.0 / 1024.0; // MB
}

std::string MemoryProfiler::generateMemoryUsageReport() const {
    std::ostringstream oss;
    oss << "Memory Usage Report:" << std::endl;
    oss << "====================" << std::endl;
    oss << "Total Allocations: " << allocationRecords_.size() << std::endl;
    oss << "Current Memory Usage: " << getMemoryUsageRatio() << " MB" << std::endl;
    oss << std::endl;
    oss << "Memory Usage by Location:" << std::endl;
    
    for (const auto& pair : memoryUsageByLocation_) {
        oss << "  " << pair.first << ": " << pair.second << " bytes" << std::endl;
    }
    
    return oss.str();
}

std::string MemoryProfiler::generateMemoryHeatmap() const {
    std::ostringstream oss;
    oss << "Memory Heatmap:" << std::endl;
    oss << "================" << std::endl;
    
    // Group allocations by location
    std::map<std::string, size_t> locationTotal;
    for (const auto& record : allocationRecords_) {
        locationTotal[record.location] += record.size;
    }
    
    // Find maximum for scaling
    size_t maxSize = 0;
    for (const auto& pair : locationTotal) {
        if (pair.second > maxSize) maxSize = pair.second;
    }
    
    // Generate heatmap
    for (const auto& pair : locationTotal) {
        size_t size = pair.second;
        int barLength = (maxSize > 0) ? (size * 50 / maxSize) : 0;
        
        oss << pair.first << ": ";
        for (int i = 0; i < barLength; ++i) {
            oss << "#";
        }
        oss << " (" << size << " bytes)" << std::endl;
    }
    
    return oss.str();
}

std::vector<std::pair<size_t, std::string>> MemoryProfiler::findMemoryLeaks() const {
    std::vector<std::pair<size_t, std::string>> leaks;
    
    // Simple leak detection - find allocations without corresponding deallocations
    std::map<std::string, size_t> allocationByLocation;
    for (const auto& record : allocationRecords_) {
        allocationByLocation[record.location] += record.size;
    }
    
    for (const auto& pair : allocationByLocation) {
        if (pair.second > leakDetectionThreshold_) {
            leaks.push_back({pair.second, pair.first});
        }
    }
    
    return leaks;
}

size_t MemoryProfiler::calculateTotalMemoryUsage() const {
    size_t total = 0;
    for (const auto& pair : memoryUsageByLocation_) {
        total += pair.second;
    }
    return total;
}

void MemoryProfiler::clear() {
    allocationRecords_.clear();
    memoryUsageByLocation_.clear();
}

// ThreadProfiler implementation
void ThreadProfiler::trackThreadCreation(const std::string& threadId) {
    if (!threadTrackingEnabled_) return;
    
    threadCreationTimes_[threadId] = std::chrono::duration_cast<Timestamp>(
        std::chrono::high_resolution_clock::now().time_since_epoch());
}

void ThreadProfiler::trackThreadDestruction(const std::string& threadId) {
    if (!threadTrackingEnabled_) return;
    
    threadCreationTimes_.erase(threadId);
}

void ThreadProfiler::trackThreadActivity(const std::string& threadId, const std::string& activity) {
    if (!threadTrackingEnabled_) return;
    
    // In real implementation, would track thread activities
}

std::map<std::string, size_t> ThreadProfiler::getThreadActivityCount() const {
    return threadActivities_;
}

std::vector<std::string> ThreadProfiler::getLongRunningThreads() const {
    std::vector<std::string> longThreads;
    auto now = std::chrono::duration_cast<Timestamp>(
        std::chrono::high_resolution_clock::now().time_since_epoch());
    
    for (const auto& pair : threadCreationTimes_) {
        double duration = (now - pair.second).count() / 1000000.0; // Convert to seconds
        if (duration > threadTimeout_) {
            longThreads.push_back(pair.first);
        }
    }
    
    return longThreads;
}

std::vector<std::string> ThreadProfiler::getIdleThreads() const {
    // In real implementation, would track thread activity
    return std::vector<std::string>();
}

std::string ThreadProfiler::generateThreadReport() const {
    std::ostringstream oss;
    oss << "Thread Report:" << std::endl;
    oss << "=============" << std::endl;
    oss << "Tracked Threads: " << threadCreationTimes_.size() << std::endl;
    oss << std::endl;
    
    for (const auto& pair : threadCreationTimes_) {
        auto now = std::chrono::duration_cast<Timestamp>(
            std::chrono::high_resolution_clock::now().time_since_epoch());
        double duration = (now - pair.second).count() / 1000000.0;
        
        oss << "Thread: " << pair.first << std::endl;
        oss << "  Created: " << duration << " seconds ago" << std::endl;
    }
    
    return oss.str();
}

std::string ThreadProfiler::generateThreadTimeline() const {
    std::ostringstream oss;
    oss << "Thread Timeline:" << std::endl;
    oss << "===============" << std::endl;
    
    // In real implementation, would generate timeline
    return oss.str();
}

void ThreadProfiler::clear() {
    threadCreationTimes_.clear();
    threadActivities_.clear();
}

} // namespace nlm