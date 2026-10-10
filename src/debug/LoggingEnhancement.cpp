#include "LoggingEnhancement.hpp"

namespace nlm {

// StructuredLogger implementation
StructuredLogger::StructuredLogger() {
}

StructuredLogger::~StructuredLogger() {
}

void StructuredLogger::logStructured(const std::string& category, const std::string& event,
                                    const std::unordered_map<std::string, std::string>& data,
                                    int severity) {
    std::lock_guard<std::mutex> lock(mutex_);
    structuredLogs_.emplace_back(category, event, data, severity);
}

void StructuredLogger::logPerformance(const std::string& operation,
                                     const std::unordered_map<std::string, double>& metrics,
                                     const std::string& context) {
    std::lock_guard<std::mutex> lock(mutex_);
    performanceLogs_.emplace_back(operation, metrics, context);
}

void StructuredLogger::logErrorWithContext(const std::string& errorMessage,
                                         const std::string& file,
                                         int line,
                                         const std::string& function,
                                         const std::unordered_map<std::string, std::string>& context) {
    std::ostringstream oss;
    oss << "Error: " << errorMessage;
    if (!file.empty()) oss << " (File: " << file << ")";
    if (line > 0) oss << " (Line: " << line << ")";
    if (!function.empty()) oss << " (Function: " << function << ")";
    
    for (const auto& ctx : context) {
        oss << " [" << ctx.first << ": " << ctx.second << "]";
    }
    
    // Log stack trace
    logStackTrace(oss.str(), "ERROR");
}

void StructuredLogger::logStackTrace(const std::string& message, const std::string& level) {
    std::ostringstream oss;
    oss << "[" << level << "?] " << message << "\nStack Trace:\n";
    
    // Get stack trace
    void* stack[100];
    int frames = backtrace(stack, 100);
    char** symbols = backtrace_symbols(stack, frames);
    
    for (int i = 0; i < frames; ++i) {
        oss << "  " << i << ": " << symbols[i] << "\n";
    }
    
    // Free symbols array
    free(symbols);
    
    oss << "End Stack Trace\n";
    
    // Log the stack trace
    logStructured("System", "StackTrace", {
        {"message", message},
        {"level", level},
        {"trace", oss.str()}
    }, 3); // Error level
}

std::string StructuredLogger::exportStructuredLogs() const {
    std::ostringstream oss;
    oss << "Structured Logs:\n";
    
    for (const auto& entry : structuredLogs_) {
        oss << "Timestamp: " << std::chrono::system_clock::to_time_t(entry.timestamp)
            << " Category: " << entry.category
            << " Event: " << entry.event
            << " Severity: " << entry.severity << "\n";
        
        for (const auto& data : entry.data) {
            oss << "  " << data.first << ": " << data.second << "\n";
        }
        
        oss << "\n";
    }
    
    return oss.str();
}

std::string StructuredLogger::exportPerformanceLogs() const {
    std::ostringstream oss;
    oss << "Performance Logs:\n";
    
    for (const auto& entry : performanceLogs_) {
        oss << "Timestamp: " << std::chrono::system_clock::to_time_t(entry.timestamp)
            << " Operation: " << entry.operation
            << " Context: " << entry.context << "\n";
        
        for (const auto& metric : entry.metrics) {
            oss << "  " << metric.first << ": " << metric.second << "\n";
        }
        
        oss << "\n";
    }
    
    return oss.str();
}

// DebugVisualizer implementation
DebugVisualizer::DebugVisualizer() {
}

DebugVisualizer::~DebugVisualizer() {
}

std::string DebugVisualizer::generateHTMLDashboard(const std::string& title) const {
    std::ostringstream oss;
    oss << "<!DOCTYPE html>\n<html>\n<head><title>" << title << "</title></head>\n";
    oss << "<body>\n<h1>" << title << "</h1>\n";
    oss << "<h2>Data Points</h2>\n<ul>\n";
    
    for (const auto& point : dataPoints_) {
        oss << "<li>" << point.category << " - " << point.name
            << ": " << point.value << " " << point.unit
            << " (at " << std::chrono::system_clock::to_time_t(point.timestamp) << ")</li>\n";
    }
    
    oss << "</ul>\n<h2>Debug Snapshots</h2>\n<ul>\n";
    
    for (const auto& snapshot : debugSnapshots_) {
        oss << "<li>" << snapshot.name << " - "
            << snapshot.timestamp.time_since_epoch().count() << "\n";
        oss << "<pre>" << snapshot.data << "</pre>\n</li>\n";
    }
    
    oss << "</ul>\n</body>\n</html>\n";
    
    return oss.str();
}

std::string DebugVisualizer::generateTextVisualization() const {
    std::ostringstream oss;
    oss << "Debug Visualization:\n";
    oss << "===================\n\n";
    
    for (const auto& point : dataPoints_) {
        oss << point.category << "." << point.name << " = "
            << point.value << " " << point.unit << "\n";
    }
    
    oss << "\nDebug Snapshots:\n";
    for (const auto& snapshot : debugSnapshots_) {
        oss << "- " << snapshot.name << " ("
            << std::chrono::system_clock::to_time_t(snapshot.timestamp) << ")\n";
    }
    
    return oss.str();
}

std::string DebugVisualizer::generatePerformanceChartData() const {
    std::ostringstream oss;
    oss << "Performance Chart Data:\n";
    
    for (const auto& point : dataPoints_) {
        if (point.category == "Performance") {
            oss << point.name << ", " << point.value << "\n";
        }
    }
    
    return oss.str();
}

bool DebugVisualizer::exportDebugData(const std::string& filename) const {
    std::ofstream file(filename);
    if (!file.is_open()) {
        return false;
    }
    
    file << generateTextVisualization() << "\n\n";
    file << "Debug Snapshots:\n";
    for (const auto& snapshot : debugSnapshots_) {
        file << "- " << snapshot.name << "\n";
        file << snapshot.data << "\n\n";
    }
    
    return true;
}

void DebugVisualizer::addDataPoint(const std::string& category, const std::string& name,
                                  double value, const std::string& unit) {
    std::lock_guard<std::mutex> lock(mutex_);
    dataPoints_.emplace_back(category, name, value, unit);
}

void DebugVisualizer::addDebugSnapshot(const std::string& name, const std::string& data) {
    std::lock_guard<std::mutex> lock(mutex_);
    debugSnapshots_.emplace_back(name, data);
}

// AdvancedPerformanceMonitor implementation
AdvancedPerformanceMonitor::AdvancedPerformanceMonitor() : totalCPUTime_(0.0), totalBytesTransferred_(0) {
    sessionStartTime_ = std::chrono::system_clock::now();
}

void AdvancedPerformanceMonitor::recordOperationStart(const std::string& operationName) {
    std::lock_guard<std::mutex> lock(mutex_);
    if (operationStats_.find(operationName) == operationStats_.end()) {
        operationStats_[operationName] = OperationStats(operationName);
    }
    operationStats_[operationName].startTime = std::chrono::system_clock::now();
}

void AdvancedPerformanceMonitor::recordOperationEnd(const std::string& operationName) {
    std::lock_guard<std::mutex> lock(mutex_);
    if (operationStats_.find(operationName) == operationStats_.end()) {
        return;
    }
    
    auto now = std::chrono::system_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(now - operationStats_[operationName].startTime).count();
    
    OperationStats& stats = operationStats_[operationName];
    stats.totalTime += duration;
    stats.callCount++;
    stats.avgTime = stats.totalTime / stats.callCount;
    stats.minTime = std::min(stats.minTime, static_cast<double>(duration));
    stats.maxTime = std::max(stats.maxTime, static_cast<double>(duration));
}

void AdvancedPerformanceMonitor::recordMemoryAllocation(size_t size, const std::string& context) {
    std::lock_guard<std::mutex> lock(mutex_);
    memoryStats_.totalAllocated += size;
    memoryStats_.currentUsage += size;
    memoryStats_.allocationCounts[context]++;
}

void AdvancedPerformanceMonitor::recordMemoryDeallocation(size_t size, const std::string& context) {
    std::lock_guard<std::mutex> lock(mutex_);
    memoryStats_.currentUsage = std::max(memoryStats_.currentUsage - size, static_cast<size_t>(0));
}

void AdvancedPerformanceMonitor::recordCPUOperation(const std::string& operation, double cpuTime) {
    std::lock_guard<std::mutex> lock(mutex_);
    totalCPUTime_ += cpuTime;
}

void AdvancedPerformanceMonitor::recordIOOperation(const std::string& operation, size_t bytes, double time) {
    std::lock_guard<std::mutex> lock(mutex_);
    totalBytesTransferred_ += bytes;
}

AdvancedPerformanceMonitor::PerformanceStats AdvancedPerformanceMonitor::getPerformanceStats(const std::string& operation) const {
    std::lock_guard<std::mutex> lock(mutex_);
    
    PerformanceStats stats;
    stats.totalOperations = 0;
    stats.totalTime = 0.0;
    
    for (const auto& pair : operationStats_) {
        if (operation.empty() || pair.first == operation) {
            stats.totalOperations += pair.second.callCount;
            stats.totalTime += pair.second.totalTime;
        }
    }
    
    stats.totalMemoryAllocated = memoryStats_.totalAllocated;
    stats.currentMemoryUsage = memoryStats_.currentUsage;
    stats.totalCPUTime = totalCPUTime_;
    stats.totalBytesTransferred = totalBytesTransferred_;
    
    if (stats.totalOperations > 0) {
        stats.averageOperationTime = stats.totalTime / stats.totalOperations;
        stats.throughput = stats.totalOperations / (std::chrono::duration_cast<std::chrono::seconds>(
            std::chrono::system_clock::now() - sessionStartTime_).count() + 1);
    } else {
        stats.averageOperationTime = 0.0;
        stats.throughput = 0.0;
    }
    
    return stats;
}

std::string AdvancedPerformanceMonitor::exportPerformanceAnalysis(const std::string& operation) const {
    std::ostringstream oss;
    oss << "Performance Analysis";
    if (!operation.empty()) {
        oss << " for " << operation;
    }
    oss << ":\n";
    
    PerformanceStats stats = getPerformanceStats(operation);
    oss << "Total Operations: " << stats.totalOperations << "\n";
    oss << "Total Time: " << stats.totalTime << " ms\n";
    oss << "Total Memory Allocated: " << stats.totalMemoryAllocated << " bytes\n";
    oss << "Current Memory Usage: " << stats.currentMemoryUsage << " bytes\n";
    oss << "Total CPU Time: " << stats.totalCPUTime << " seconds\n";
    oss << "Total Bytes Transferred: " << stats.totalBytesTransferred << " bytes\n";
    oss << "Average Operation Time: " << stats.averageOperationTime << " ms\n";
    oss << "Throughput: " << stats.throughput << " operations/second\n";
    
    // Detailed operation statistics
    oss << "\nDetailed Statistics:\n";
    for (const auto& pair : operationStats_) {
        if (operation.empty() || pair.first == operation) {
            oss << pair.first << ":\n";
            oss << "  Calls: " << pair.second.callCount << "\n";
            oss << "  Total Time: " << pair.second.totalTime << " ms\n";
            oss << "  Average Time: " << pair.second.avgTime << " ms\n";
            oss << "  Min Time: " << pair.second.minTime << " ms\n";
            oss << "  Max Time: " << pair.second.maxTime << " ms\n";
        }
    }
    
    return oss.str();
}

std::string AdvancedPerformanceMonitor::generatePerformanceReport() const {
    return exportPerformanceAnalysis();
}

// MemoryPoolProfiler implementation
MemoryPoolProfiler::MemoryPoolProfiler() {
    sessionStartTime_ = std::chrono::system_clock::now();
}

void MemoryPoolProfiler::trackAllocation(const std::string& poolName, size_t size, const std::string& context) {
    std::lock_guard<std::mutex> lock(mutex_);
    if (poolStats_.find(poolName) == poolStats_.end()) {
        poolStats_[poolName] = PoolStats(poolName);
    }
    
    PoolStats& stats = poolStats_[poolName];
    stats.totalAllocated += size;
    stats.currentUsage += size;
    stats.allocationCount++;
    stats.maxUsage = std::max(stats.maxUsage, stats.currentUsage);
}

void MemoryPoolProfiler::trackDeallocation(const std::string& poolName, size_t size, const std::string& context) {
    std::lock_guard<std::mutex> lock(mutex_);
    if (poolStats_.find(poolName) == poolStats_.end()) {
        return;
    }
    
    PoolStats& stats = poolStats_[poolName];
    stats.currentUsage = std::max(stats.currentUsage - size, static_cast<size_t>(0));
    stats.deallocationCount++;
}

std::vector<MemoryPoolProfiler::PoolStats> MemoryPoolProfiler::getPoolStats() const {
    std::lock_guard<std::mutex> lock(mutex_);
    std::vector<PoolStats> stats;
    
    for (const auto& pair : poolStats_) {
        stats.push_back(pair.second);
    }
    
    return stats;
}

std::string MemoryPoolProfiler::exportMemoryPoolAnalysis() const {
    std::ostringstream oss;
    oss << "Memory Pool Analysis:\n";
    
    for (const auto& pair : poolStats_) {
        const PoolStats& stats = pair.second;
        oss << "Pool: " << stats.name << "\n";
        oss << "  Total Allocated: " << stats.totalAllocated << " bytes\n";
        oss << "  Current Usage: " << stats.currentUsage << " bytes\n";
        oss << "  Allocation Count: " << stats.allocationCount << "\n";
        oss << "  Deallocation Count: " << stats.deallocationCount << "\n";
        oss << "  Max Usage: " << stats.maxUsage << " bytes\n";
        
        if (stats.allocationCount > 0) {
            oss << "  Allocation Rate: " << (stats.allocationCount / std::chrono::duration_cast<std::chrono::seconds>(
                std::chrono::system_clock::now() - sessionStartTime_).count() + 1) << " alloc/sec\n";
        }
        
        oss << "\n";
    }
    
    return oss.str();
}

std::string MemoryPoolProfiler::generateMemoryPoolReport() const {
    return exportMemoryPoolAnalysis();
}

// ThreadProfiler implementation
ThreadProfiler::ThreadProfiler() {
    sessionStartTime_ = std::chrono::system_clock::now();
}

void ThreadProfiler::recordThreadStart(const std::string& threadName, const std::string& threadId) {
    std::lock_guard<std::mutex> lock(mutex_);
    if (threadStats_.find(threadId) == threadStats_.end()) {
        threadStats_[threadId] = ThreadStats(threadName, threadId);
    }
}

void ThreadProfiler::recordThreadEnd(const std::string& threadName, const std::string& threadId) {
    std::lock_guard<std::mutex> lock(mutex_);
    if (threadStats_.find(threadId) == threadStats_.end()) {
        return;
    }
    
    // Thread is ending, no further activity expected
}

void ThreadProfiler::recordThreadActivity(const std::string& threadName, const std::string& activity,
                                         double duration, const std::string& context) {
    std::lock_guard<std::mutex> lock(mutex_);
    // Find thread by ID or name
    for (auto& pair : threadStats_) {
        if (pair.second.name == threadName) {
            pair.second.totalActiveTime += duration;
            pair.second.activityCount++;
            pair.second.activityTimes[activity + " (" + context + ")"] += duration;
            return;
        }
    }
}

std::vector<ThreadProfiler::ThreadStats> ThreadProfiler::getThreadStats() const {
    std::lock_guard<std::mutex> lock(mutex_);
    std::vector<ThreadStats> stats;
    
    for (const auto& pair : threadStats_) {
        stats.push_back(pair.second);
    }
    
    return stats;
}

std::string ThreadProfiler::exportThreadProfilingData() const {
    std::ostringstream oss;
    oss << "Thread Profiling Data:\n";
    
    for (const auto& pair : threadStats_) {
        const ThreadStats& stats = pair.second;
        oss << "Thread: " << stats.name << " (ID: " << stats.id << ")\n";
        oss << "  Total Active Time: " << stats.totalActiveTime << " seconds\n";
        oss << "  Activity Count: " << stats.activityCount << "\n";
        oss << "  Activities:\n";
        
        for (const auto& activity : stats.activityTimes) {
            oss << "    " << activity.first << ": " << activity.second << " seconds\n";
        }
        
        oss << "\n";
    }
    
    return oss.str();
}

std::string ThreadProfiler::generateThreadAnalysisReport() const {
    return exportThreadProfilingData();
}

// ProfilingOutputFormatter implementation
std::string ProfilingOutputFormatter::toJSON(const std::string& data) {
    std::ostringstream oss;
    oss << "{\"data\": \"" << escapeJSON(data) << "\"}";
    return oss.str();
}

std::string ProfilingOutputFormatter::toCSV(const std::string& data) {
    std::ostringstream oss;
    oss << "Data\n";
    oss << "Value\n";
    oss << escapeCSV(data);
    return oss.str();
}

std::string ProfilingOutputFormatter::toText(const std::string& data) {
    return data;
}

std::string ProfilingOutputFormatter::toHTML(const std::string& data) {
    std::ostringstream oss;
    oss << "<html><body><pre>" << escapeHTML(data) << "</pre></body></html>";
    return oss.str();
}

std::string ProfilingOutputFormatter::formatPerformanceReport(const std::string& title, const std::string& data) {
    std::ostringstream oss;
    oss << "Performance Report: " << title << "\n";
    oss << "========================\n\n";
    oss << data << "\n";
    return oss.str();
}

std::string ProfilingOutputFormatter::formatDebugDashboard(const std::string& title, const std::string& data) {
    std::ostringstream oss;
    oss << "Debug Dashboard: " << title << "\n";
    oss << "======================\n\n";
    oss << data << "\n";
    return oss.str();
}

std::string ProfilingOutputFormatter::formatMemoryLeakReport(const std::string& title, const std::string& data) {
    std::ostringstream oss;
    oss << "Memory Leak Report: " << title << "\n";
    oss << "==========================\n\n";
    oss << data << "\n";
    return oss.str();
}

std::string ProfilingOutputFormatter::formatSystemHealthReport(const std::string& title, const std::string& data) {
    std::ostringstream oss;
    oss << "System Health Report: " << title << "\n";
    oss << "============================\n\n";
    oss << data << "\n";
    return oss.str();
}

std::string ProfilingOutputFormatter::escapeJSON(const std::string& input) {
    std::ostringstream oss;
    for (char c : input) {
        switch (c) {
            case '\\': oss << "\\\\"; break;
            case '\"': oss << "\\\""; break;
            case '\\': oss << "\\\\"; break;
            case '/': oss << "\\\\/"; break;
            case '\\': oss << "\\\\b"; break;
            case '\\': oss << "\\\\f"; break;
            case '\\': oss << "\\\\n"; break;
            case '\\': oss << "\\\\r"; break;
            case '\\': oss << "\\\\t"; break;
            default: oss << c; break;
        }
    }
    return oss.str();
}

std::string ProfilingOutputFormatter::escapeHTML(const std::string& input) {
    std::ostringstream oss;
    for (char c : input) {
        switch (c) {
            case '\\': oss << "&lt;"; break;
            case '\\': oss << "&gt;"; break;
            case '\\': oss << "&amp;"; break;
            case '\"': oss << "&quot;"; break;
            case '\'': oss << "&#x27;"; break;
            case '/': oss << "&frasl;"; break;
            default: oss << c; break;
        }
    }
    return oss.str();
}

std::string ProfilingOutputFormatter::escapeCSV(const std::string& input) {
    std::ostringstream oss;
    oss << "\"" << input << "\"";
    return oss.str();
}

} // namespace nlm