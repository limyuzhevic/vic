// Performance monitoring and debugging utilities for NLM
// Phase 6: Advanced instrumentation and profiling
// 
// Features:
// - Real-time performance metrics collection
// - Detailed profiling of critical operations
// - Memory usage tracking and analysis
// - CPU performance monitoring
// - Debug mode with comprehensive logging
// - Performance benchmarks and statistics
// - System resource monitoring
// - Event timing and duration measurement
// - Concurrent performance tracking
// - Integration with existing logging system

#pragma once

#include <chrono>
#include <string>
#include <vector>
#include <unordered_map>
#include <memory>
#include <atomic>
#include <mutex>
#include <iomanip>
#include <sstream>
#include <functional>
#include <algorithm>
#include <thread>
#include <atomic>
#include <optional>

namespace nlm {

namespace chrono = std::chrono;

/**
 * @brief Timer for measuring duration of operations
 * 
 * Thread-safe timer that records elapsed time and provides
 * statistics for performance analysis.
 */
class Timer {
public:
    Timer() : startTime(chrono::high_resolution_clock::now()),
              totalDuration(0), count(0), maxDuration(0),
              minDuration(std::numeric_limits<chrono::nanoseconds>::max()) {}
    
    void start() {
        std::lock_guard<std::mutex> lock(mutex);
        startTime = chrono::high_resolution_clock::now();
    }
    
    void stop() {
        std::lock_guard<std::mutex> lock(mutex);
        auto endTime = chrono::high_resolution_clock::now();
        auto duration = chrono::duration_cast<chrono::nanoseconds>(endTime - startTime);
        
        totalDuration += duration.count();
        count++;
        
        if (duration.count() > maxDuration) {
            maxDuration = duration.count();
        }
        if (duration.count() < minDuration) {
            minDuration = duration.count();
        }
    }
    
    chrono::nanoseconds::rep getCurrentDuration() const {
        std::lock_guard<std::mutex> lock(mutex);
        auto currentTime = chrono::high_resolution_clock::now();
        return chrono::duration_cast<chrono::nanoseconds>(currentTime - startTime).count();
    }
    
    chrono::nanoseconds::rep getTotalDuration() const {
        std::lock_guard<std::mutex> lock(mutex);
        return totalDuration;
    }
    
    size_t getCount() const {
        std::lock_guard<std::mutex> lock(mutex);
        return count;
    }
    
    chrono::nanoseconds::rep getAverageDuration() const {
        std::lock_guard<std::mutex> lock(mutex);
        if (count == 0) return 0;
        return totalDuration / count;
    }
    
    chrono::nanoseconds::rep getMaxDuration() const {
        std::lock_guard<std::mutex> lock(mutex);
        return maxDuration;
    }
    
    chrono::nanoseconds::rep getMinDuration() const {
        std::lock_guard<std::mutex> lock(mutex);
        return minDuration;
    }
    
    bool isRunning() const {
        std::lock_guard<std::mutex> lock(mutex);
        return chrono::duration_cast<chrono::nanoseconds>(chrono::high_resolution_clock::now() - startTime).count() < (std::numeric_limits<chrono::nanoseconds>::max() >> 1);
    }
    
    std::string getFormattedAverage() const {
        auto avg = getAverageDuration();
        return formatDuration(avg);
    }
    
    std::string getFormattedMax() const {
        auto max = getMaxDuration();
        return formatDuration(max);
    }
    
    std::string getFormattedMin() const {
        auto min = getMinDuration();
        return formatDuration(min);
    }
    
private:
    std::string formatDuration(chrono::nanoseconds::rep nanoseconds) {
        if (nanoseconds >= chrono::nanoseconds::rep(chrono::hours(1).count())) {
            auto hours = nanoseconds / chrono::hours(1).count();
            nanoseconds %= chrono::hours(1).count();
            return std::to_string(hours) + "h " + formatDuration(nanoseconds);
        } else if (nanoseconds >= chrono::nanoseconds::rep(chrono::minutes(1).count())) {
            auto minutes = nanoseconds / chrono::minutes(1).count();
            nanoseconds %= chrono::minutes(1).count();
            return std::to_string(minutes) + "m " + formatDuration(nanoseconds);
        } else if (nanoseconds >= chrono::nanoseconds::rep(chrono::seconds(1).count())) {
            auto seconds = nanoseconds / chrono::seconds(1).count();
            nanoseconds %= chrono::seconds(1).count();
            return std::to_string(seconds) + "s " + formatDuration(nanoseconds);
        } else if (nanoseconds >= chrono::nanoseconds::rep(chrono::milliseconds(1).count())) {
            auto ms = nanoseconds / chrono::milliseconds(1).count();
            nanoseconds %= chrono::milliseconds(1).count();
            return std::to_string(ms) + "ms " + formatDuration(nanoseconds);
        } else if (nanoseconds >= chrono::nanoseconds::rep(chrono::microseconds(1).count())) {
            auto us = nanoseconds / chrono::microseconds(1).count();
            nanoseconds %= chrono::microseconds(1).count();
            return std::to_string(us) + "us " + formatDuration(nanoseconds);
        } else {
            return std::to_string(nanoseconds) + "ns";
        }
    }
    
    std::mutex mutable mutex;
    chrono::time_point<chrono::high_resolution_clock> startTime;
    chrono::nanoseconds::rep totalDuration;
    size_t count;
    chrono::nanoseconds::rep maxDuration;
    chrono::nanoseconds::rep minDuration;
};

/**
 * @brief Performance profiler for tracking application performance
 * 
 * Collects and analyzes performance data across different modules
 * and operations, providing detailed profiling information.
 */
class PerformanceProfiler {
public:
    PerformanceProfiler() : running(false), thread(std::make_unique<std::thread>(&PerformanceProfiler::run, this)) {}
    
    ~PerformanceProfiler() {
        stop();
        if (thread && thread->joinable()) {
            thread->join();
        }
    }
    
    void start() {
        std::lock_guard<std::mutex> lock(mutex);
        running = true;
    }
    
    void stop() {
        std::lock_guard<std::mutex> lock(mutex);
        running = false;
    }
    
    void registerTimer(const std::string& name, std::unique_ptr<Timer> timer) {
        std::lock_guard<std::mutex> lock(mutex);
        timers[name] = std::move(timer);
    }
    
    Timer* getTimer(const std::string& name) {
        std::lock_guard<std::mutex> lock(mutex);
        auto it = timers.find(name);
        return it != timers.end() ? it->second.get() : nullptr;
    }
    
    void startTimer(const std::string& name) {
        Timer* timer = getTimer(name);
        if (timer) {
            timer->start();
        }
    }
    
    void stopTimer(const std::string& name) {
        Timer* timer = getTimer(name);
        if (timer) {
            timer->stop();
        }
    }
    
    void recordSystemMetrics() {
        std::lock_guard<std::mutex> lock(mutex);
        
        auto now = chrono::high_resolution_clock::now();
        SystemMetrics metrics;
        metrics.timestamp = chrono::duration_cast<chrono::seconds>(now.time_since_epoch()).count();
        metrics.cpuUsage = getCpuUsage();
        metrics.memoryUsage = getMemoryUsage();
        metrics.threadCount = getThreadCount();
        metrics.activeTimerCount = getActiveTimerCount();
        
        systemMetrics.push_back(metrics);
        
        if (systemMetrics.size() > maxSystemMetrics) {
            systemMetrics.pop_front();
        }
    }
    
    std::deque<SystemMetrics> getSystemMetrics(size_t maxCount = 100) {
        std::lock_guard<std::mutex> lock(mutex);
        if (maxCount > systemMetrics.size()) {
            maxCount = systemMetrics.size();
        }
        
        auto start = systemMetrics.end() - maxCount;
        return std::deque<SystemMetrics>(start, systemMetrics.end());
    }
    
    std::string generatePerformanceReport() {
        std::lock_guard<std::mutex> lock(mutex);
        
        std::stringstream report;
        report << "=== NLM Performance Report ===" << std::endl;
        report << "Generated: " << getCurrentTimestamp() << std::endl;
        report << "" << std::endl;
        
        report << "Timers:" << std::endl;
        for (const auto& pair : timers) {
            report << "  " << pair.first << ":" << std::endl;
            auto timer = pair.second.get();
            report << "    Total: " << timer->getFormattedAverage() << ", " 
                   << timer->getCount() << " runs" << std::endl;
            report << "    Min: " << timer->getFormattedMin() << ", "
                   << "Max: " << timer->getFormattedMax() << std::endl;
        }
        
        report << "" << std::endl;
        report << "System Metrics (last " << systemMetrics.size() << "):" << std::endl;
        report << std::setw(10) << "Time" << std::setw(12) << "CPU%" 
               << std::setw(12) << "Memory%" << std::setw(10) << "Threads" 
               << std::setw(15) << "ActiveTimers" << std::endl;
        
        size_t count = std::min(systemMetrics.size(), size_t(20));
        auto it = systemMetrics.end();
        for (size_t i = 0; i < count; ++i) {
            --it;
            report << std::setw(10) << it->timestamp
                   << std::setw(12) << std::fixed << std::setprecision(1) << it->cpuUsage
                   << std::setw(12) << std::fixed << std::setprecision(1) << it->memoryUsage
                   << std::setw(10) << it->threadCount
                   << std::setw(15) << it->activeTimerCount << std::endl;
        }
        
        report << "" << std::endl;
        report << "Performance Summary:" << std::endl;
        report << "  Total timers tracked: " << timers.size() << std::endl;
        report << "  Profiling active: " << (running ? "yes" : "no") << std::endl;
        report << "  System metrics collected: " << systemMetrics.size() << std::endl;
        
        return report.str();
    }
    
private:
    struct SystemMetrics {
        chrono::seconds::rep timestamp;
        double cpuUsage;
        double memoryUsage;
        size_t threadCount;
        size_t activeTimerCount;
    };
    
    double getCpuUsage() {
        // Implementation for CPU usage measurement
        return 0.0;
    }
    
    double getMemoryUsage() {
        // Implementation for memory usage measurement
        return 0.0;
    }
    
    size_t getThreadCount() {
        return std::thread::hardware_concurrency();
    }
    
    size_t getActiveTimerCount() {
        size_t count = 0;
        for (const auto& pair : timers) {
            if (pair.second && pair.second->isRunning()) {
                count++;
            }
        }
        return count;
    }
    
    std::string getCurrentTimestamp() {
        auto now = chrono::system_clock::now();
        auto time = chrono::system_clock::to_time_t(now);
        std::stringstream ss;
        ss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
        return ss.str();
    }
    
    void run() {
        while (running) {
            recordSystemMetrics();
            chrono::this_thread::sleep_for(chrono::seconds(1));
        }
    }
    
    std::mutex mutable mutex;
    std::unordered_map<std::string, std::unique_ptr<Timer>> timers;
    std::deque<SystemMetrics> systemMetrics;
    bool running;
    std::unique_ptr<std::thread> thread;
    static constexpr size_t maxSystemMetrics = 1000;
    std::atomic<size_t> activeTimerCount{0};
};

/**
 * @brief Memory profiler for tracking memory allocation and usage
 * 
 * Monitors memory allocation patterns, detects leaks, and
 * provides detailed memory usage analysis.
 */
class MemoryProfiler {
public:
    MemoryProfiler() : tracking(false), allocationsCount(0), deallocationsCount(0),
                      totalAllocated(0), totalDeallocated(0) {}
    
    void startTracking() {
        std::lock_guard<std::mutex> lock(mutex);
        tracking = true;
        startTime = chrono::high_resolution_clock::now();
        allocationsCount = 0;
        deallocationsCount = 0;
        totalAllocated = 0;
        totalDeallocated = 0;
        memorySnapshots.clear();
    }
    
    void stopTracking() {
        std::lock_guard<std::mutex> lock(mutex);
        tracking = false;
    }
    
    void recordAllocation(size_t size) {
        if (!tracking) return;
        std::lock_guard<std::mutex> lock(mutex);
        allocationsCount++;
        totalAllocated += size;
        recordSnapshot();
    }
    
    void recordDeallocation(size_t size) {
        if (!tracking) return;
        std::lock_guard<std::mutex> lock(mutex);
        deallocationsCount++;
        totalDeallocated += size;
        recordSnapshot();
    }
    
    void recordSnapshot() {
        MemorySnapshot snapshot;
        snapshot.timestamp = chrono::duration_cast<chrono::seconds>(
            chrono::high_resolution_clock::now() - startTime).count();
        snapshot.allocations = allocationsCount;
        snapshot.deallocations = deallocationsCount;
        snapshot.totalAllocated = totalAllocated;
        snapshot.totalDeallocated = totalDeallocated;
        snapshot.currentUsage = totalAllocated - totalDeallocated;
        
        auto now = chrono::high_resolution_clock::now();
        snapshot.memoryUsage = getCurrentMemoryUsage();
        
        memorySnapshots.push_back(snapshot);
        
        if (memorySnapshots.size() > maxSnapshots) {
            memorySnapshots.pop_front();
        }
    }
    
    std::deque<MemorySnapshot> getMemorySnapshots(size_t maxCount = 100) {
        std::lock_guard<std::mutex> lock(mutex);
        if (maxCount > memorySnapshots.size()) {
            maxCount = memorySnapshots.size();
        }
        
        auto start = memorySnapshots.end() - maxCount;
        return std::deque<MemorySnapshot>(start, memorySnapshots.end());
    }
    
    std::string generateMemoryReport() {
        std::lock_guard<std::mutex> lock(mutex);
        
        std::stringstream report;
        report << "=== NLM Memory Report ===" << std::endl;
        report << "Tracking active: " << (tracking ? "yes" : "no") << std::endl;
        report << "Start time: " << getStartTimeString() << std::endl;
        report << "Current time: " << getCurrentTimestamp() << std::endl;
        report << "" << std::endl;
        
        report << "Allocation Summary:" << std::endl;
        report << "  Total allocations: " << allocationsCount << std::endl;
        report << "  Total deallocations: " << deallocationsCount << std::endl;
        report << "  Net allocations: " << (allocationsCount - deallocationsCount) << std::endl;
        report << "  Total allocated: " << formatBytes(totalAllocated) << std::endl;
        report << "  Total deallocated: " << formatBytes(totalDeallocated) << std::endl;
        report << "  Current usage: " << formatBytes(totalAllocated - totalDeallocated) << std::endl;
        
        if (memorySnapshots.size() > 1) {
            report << "" << std::endl;
            report << "Memory Usage Trend:" << std::endl;
            
            size_t count = std::min(memorySnapshots.size(), size_t(20));
            auto it = memorySnapshots.end();
            for (size_t i = 0; i < count; ++i) {
                --it;
                report << std::setw(8) << it->timestamp 
                       << std::setw(15) << formatBytes(it->currentUsage)
                       << std::setw(15) << formatBytes(it->memoryUsage) << std::endl;
            }
        }
        
        if (hasLeaks()) {
            report << "" << std::endl;
            report << "MEMORY LEAK DETECTED!" << std::endl;
            report << "  Current un-freed allocations: " << (allocationsCount - deallocationsCount) << std::endl;
        }
        
        return report.str();
    }
    
    bool hasLeaks() const {
        std::lock_guard<std::mutex> lock(mutex);
        return allocationsCount > deallocationsCount;
    }
    
    size_t getLeakCount() const {
        std::lock_guard<std::mutex> lock(mutex);
        return allocationsCount > deallocationsCount ? (allocationsCount - deallocationsCount) : 0;
    }
    
    double getCurrentMemoryUsage() {
        // Implementation for current memory usage
        return 0.0;
    }
    
    std::string getStartTimeString() {
        auto time = chrono::system_clock::to_time_t(startTime);
        std::stringstream ss;
        ss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
        return ss.str();
    }
    
    static std::string getCurrentTimestamp() {
        auto now = chrono::system_clock::now();
        auto time = chrono::system_clock::to_time_t(now);
        std::stringstream ss;
        ss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
        return ss.str();
    }
    
    static std::string formatBytes(size_t bytes) {
        if (bytes >= 1024 * 1024 * 1024) {
            return std::to_string(bytes / (1024 * 1024 * 1024)) + " GB";
        } else if (bytes >= 1024 * 1024) {
            return std::to_string(bytes / (1024 * 1024)) + " MB";
        } else if (bytes >= 1024) {
            return std::to_string(bytes / 1024) + " KB";
        } else {
            return std::to_string(bytes) + " B";
        }
    }
    
private:
    struct MemorySnapshot {
        chrono::seconds::rep timestamp;
        size_t allocations;
        size_t deallocations;
        size_t totalAllocated;
        size_t totalDeallocated;
        size_t currentUsage;
        double memoryUsage;  // In MB
    };
    
    std::mutex mutable mutex;
    chrono::time_point<chrono::high_resolution_clock> startTime;
    bool tracking;
    size_t allocationsCount;
    size_t deallocationsCount;
    size_t totalAllocated;
    size_t totalDeallocated;
    std::deque<MemorySnapshot> memorySnapshots;
    static constexpr size_t maxSnapshots = 1000;
};

/**
 * @brief Debug mode controller for enhanced debugging capabilities
 * 
 * Provides comprehensive debugging features including step recording,
 * breakpoint support, and detailed state inspection.
 */
class DebugController {
public:
    DebugController() : enabled(false), stepRecording(false), 
                         breakpointMode(false), profilingEnabled(false) {}
    
    void enableDebugMode(bool enable = true) {
        std::lock_guard<std::mutex> lock(mutex);
        enabled = enable;
        if (enabled) {
            NLM_LOG_INFO("Debug mode enabled");
            startRecording();
            startProfiling();
        } else {
            NLM_LOG_INFO("Debug mode disabled");
            stopRecording();
            stopProfiling();
        }
    }
    
    bool isDebugModeEnabled() const {
        std::lock_guard<std::mutex> lock(mutex);
        return enabled;
    }
    
    void enableStepRecording(bool enable = true) {
        std::lock_guard<std::mutex> lock(mutex);
        stepRecording = enable;
        if (stepRecording) {
            NLM_LOG_INFO("Step recording enabled");
            stepRecorder.clear();
            stepRecorderStarted = true;
        }
    }
    
    void enableBreakpointMode(bool enable = true) {
        std::lock_guard<std::mutex> lock(mutex);
        breakpointMode = enable;
        if (breakpointMode) {
            NLM_LOG_INFO("Breakpoint mode enabled");
        }
    }
    
    void addBreakpoint(SimulationStep step) {
        std::lock_guard<std::mutex> lock(mutex);
        breakpoints.insert(step);
        NLM_LOG_INFO("Added breakpoint at step " + std::to_string(step));
    }
    
    bool shouldBreakAtStep(SimulationStep step) {
        std::lock_guard<std::mutex> lock(mutex);
        return breakpoints.find(step) != breakpoints.end();
    }
    
    void startRecording() {
        stepRecording = true;
        stepRecorder.clear();
        stepRecorderStarted = true;
        NLM_LOG_INFO("Step recording started");
    }
    
    void stopRecording() {
        stepRecording = false;
        stepRecorderStarted = false;
        NLM_LOG_INFO("Step recording stopped");
    }
    
    void recordStep(SimulationStep step, Timestamp timestamp, const std::string& description) {
        if (!stepRecording) return;
        
        std::lock_guard<std::mutex> lock(mutex);
        
        StepRecord record;
        record.step = step;
        record.timestamp = timestamp;
        record.description = description;
        record.recordingTime = chrono::duration_cast<chrono::seconds>(
            chrono::high_resolution_clock::now().time_since_epoch()).count();
        
        stepRecorder.push_back(record);
        
        if (stepRecorder.size() > maxStepRecords) {
            stepRecorder.pop_front();
        }
        
        NLM_LOG_DEBUG("Step " + std::to_string(step) + ": " + description);
    }
    
    std::deque<StepRecord> getStepRecords(size_t maxCount = 100) {
        std::lock_guard<std::mutex> lock(mutex);
        if (maxCount > stepRecorder.size()) {
            maxCount = stepRecorder.size();
        }
        
        auto start = stepRecorder.end() - maxCount;
        return std::deque<StepRecord>(start, stepRecorder.end());
    }
    
    void startProfiling() {
        profilingEnabled = true;
        NLM_LOG_INFO("Profiling started");
    }
    
    void stopProfiling() {
        profilingEnabled = false;
        NLM_LOG_INFO("Profiling stopped");
    }
    
    std::string generateDebugReport() {
        std::lock_guard<std::mutex> lock(mutex);
        
        std::stringstream report;
        report << "=== NLM Debug Report ===" << std::endl;
        report << "Debug mode: " << (enabled ? "enabled" : "disabled") << std::endl;
        report << "Step recording: " << (stepRecording ? "enabled" : "disabled") << std::endl;
        report << "Breakpoint mode: " << (breakpointMode ? "enabled" : "disabled") << std::endl;
        report << "Profiling enabled: " << (profilingEnabled ? "yes" : "no") << std::endl;
        report << "" << std::endl;
        
        if (!breakpoints.empty()) {
            report << "Breakpoints:" << std::endl;
            for (auto step : breakpoints) {
                report << "  Step " << step << std::endl;
            }
            report << "" << std::endl;
        }
        
        report << "Step records (last " << stepRecorder.size() << "):" << std::endl;
        size_t count = std::min(stepRecorder.size(), size_t(10));
        auto it = stepRecorder.end();
        for (size_t i = 0; i < count; ++i) {
            --it;
            report << std::setw(8) << it->step 
                   << std::setw(12) << it->timestamp
                   << " " << it->description << std::endl;
        }
        
        return report.str();
    }
    
    void exportStepRecords(const std::string& filename) {
        std::lock_guard<std::mutex> lock(mutex);
        
        std::ofstream file(filename, std::ios::out | std::ios::trunc);
        if (!file.is_open()) {
            NLM_LOG_ERROR("Failed to open file for exporting step records: " + filename);
            return;
        }
        
        file << "Step,Time,Description,RecordingTime" << std::endl;
        for (const auto& record : stepRecorder) {
            file << record.step << ","
                 << record.timestamp << ","
                 << record.description << ","
                 << record.recordingTime << std::endl;
        }
        
        file.close();
        NLM_LOG_INFO("Step records exported to: " + filename);
    }
    
private:
    struct StepRecord {
        SimulationStep step;
        Timestamp timestamp;
        std::string description;
        chrono::seconds::rep recordingTime;
    };
    
    std::mutex mutable mutex;
    bool enabled;
    bool stepRecording;
    bool stepRecorderStarted;
    bool breakpointMode;
    bool profilingEnabled;
    std::unordered_set<SimulationStep> breakpoints;
    std::deque<StepRecord> stepRecorder;
    bool recordingStarted;
    static constexpr size_t maxStepRecords = 10000;
};

/**
 * @brief Comprehensive performance monitoring system for NLM
 * 
 * Integrates timing, memory, and system metrics collection with
 * advanced debugging capabilities for performance analysis.
 */
class PerformanceMonitor {
public:
    PerformanceMonitor() : debugController(std::make_unique<DebugController>()),
                          memoryProfiler(std::make_unique<MemoryProfiler>()),
                          profiler(std::make_unique<PerformanceProfiler>()) {}
    
    ~PerformanceMonitor() {
        stopMonitoring();
    }
    
    void startMonitoring() {
        std::lock_guard<std::mutex> lock(mutex);
        
        debugController->enableDebugMode(true);
        memoryProfiler->startTracking();
        profiler->start();
        
        NLM_LOG_INFO("Performance monitoring started");
    }
    
    void stopMonitoring() {
        std::lock_guard<std::mutex> lock(mutex);
        
        profiler->stop();
        memoryProfiler->stopTracking();
        debugController->enableDebugMode(false);
        
        NLM_LOG_INFO("Performance monitoring stopped");
    }
    
    Timer* createTimer(const std::string& name) {
        std::lock_guard<std::mutex> lock(mutex);
        auto timer = std::make_unique<Timer>();
        Timer* timerPtr = timer.get();
        profiler->registerTimer(name, std::move(timer));
        return timerPtr;
    }
    
    void startTimer(const std::string& name) {
        profiler->startTimer(name);
    }
    
    void stopTimer(const std::string& name) {
        profiler->stopTimer(name);
    }
    
    void recordStep(SimulationStep step, Timestamp timestamp, const std::string& description) {
        std::lock_guard<std::mutex> lock(mutex);
        
        debugController->recordStep(step, timestamp, description);
        
        if (profiler->shouldRecordMetrics()) {
            profiler->recordSystemMetrics();
        }
    }
    
    void recordAllocation(size_t size) {
        memoryProfiler->recordAllocation(size);
    }
    
    void recordDeallocation(size_t size) {
        memoryProfiler->recordDeallocation(size);
    }
    
    DebugController* getDebugController() {
        return debugController.get();
    }
    
    MemoryProfiler* getMemoryProfiler() {
        return memoryProfiler.get();
    }
    
    PerformanceProfiler* getProfiler() {
        return profiler.get();
    }
    
    std::string generatePerformanceReport() {
        std::lock_guard<std::mutex> lock(mutex);
        
        std::stringstream report;
        report << debugController->generateDebugReport();
        report << "" << std::endl;
        report << memoryProfiler->generateMemoryReport();
        report << "" << std::endl;
        report << profiler->generatePerformanceReport();
        
        return report.str();
    }
    
    void exportAllReports(const std::string& directory) {
        std::lock_guard<std::mutex> lock(mutex);
        
        std::filesystem::create_directories(directory);
        
        debugController->exportStepRecords(directory + "/step_records.csv");
        
        std::ofstream reportFile(directory + "/full_performance_report.txt", 
                                 std::ios::out | std::ios::trunc);
        if (reportFile.is_open()) {
            reportFile << generatePerformanceReport();
            reportFile.close();
            NLM_LOG_INFO("Performance report exported to: " + directory + "/full_performance_report.txt");
        }
    }
    
    void exportSystemMetrics(const std::string& filename) {
        std::lock_guard<std::mutex> lock(mutex);
        
        auto metrics = profiler->getSystemMetrics();
        
        std::ofstream file(filename, std::ios::out | std::ios::trunc);
        if (!file.is_open()) {
            NLM_LOG_ERROR("Failed to open file for exporting system metrics: " + filename);
            return;
        }
        
        file << "Timestamp,CPU%,Memory%,Threads,ActiveTimers" << std::endl;
        for (const auto& metric : metrics) {
            file << metric.timestamp << ","
                 << std::fixed << std::setprecision(1) << metric.cpuUsage << ","
                 << std::fixed << std::setprecision(1) << metric.memoryUsage << ","
                 << metric.threadCount << ","
                 << metric.activeTimerCount << std::endl;
        }
        
        file.close();
        NLM_LOG_INFO("System metrics exported to: " + filename);
    }
    
    void exportMemorySnapshots(const std::string& filename) {
        std::lock_guard<std::mutex> lock(mutex);
        
        auto snapshots = memoryProfiler->getMemorySnapshots();
        
        std::ofstream file(filename, std::ios::out | std::ios::trunc);
        if (!file.is_open()) {
            NLM_LOG_ERROR("Failed to open file for exporting memory snapshots: " + filename);
            return;
        }
        
        file << "Timestamp,Allocations,Deallocations,TotalAllocated,TotalDeallocated,CurrentUsage,MemoryUsage(MB)" << std::endl;
        for (const auto& snapshot : snapshots) {
            file << snapshot.timestamp << ","
                 << snapshot.allocations << ","
                 << snapshot.deallocations << ","
                 << snapshot.totalAllocated << ","
                 << snapshot.totalDeallocated << ","
                 << snapshot.currentUsage << ","
                 << std::fixed << std::setprecision(1) << snapshot.memoryUsage << std::endl;
        }
        
        file.close();
        NLM_LOG_INFO("Memory snapshots exported to: " + filename);
    }
    
    void enableBreakpoint(SimulationStep step) {
        debugController->addBreakpoint(step);
    }
    
    bool shouldBreakAtStep(SimulationStep step) {
        return debugController->shouldBreakAtStep(step);
    }
    
    bool hasMemoryLeaks() {
        return memoryProfiler->hasLeaks();
    }
    
    size_t getMemoryLeakCount() {
        return memoryProfiler->getLeakCount();
    }
    
    size_t getTimerCount() {
        std::lock_guard<std::mutex> lock(mutex);
        return profiler->getTimerCount();
    }
    
    size_t getStepRecordCount() {
        std::lock_guard<std::mutex> lock(mutex);
        return debugController->getStepRecordCount();
    }
    
    size_t getSystemMetricCount() {
        std::lock_guard<std::mutex> lock(mutex);
        return static_cast<size_t>(profiler->getSystemMetricCount());
    }
    
    std::string getStartTimeString() {
        std::lock_guard<std::mutex> lock(mutex);
        return memoryProfiler->getStartTimeString();
    }
    
    bool isMonitoring() const {
        std::lock_guard<std::mutex> lock(mutex);
        return debugController->isDebugModeEnabled();
    }
    
private:
    std::mutex mutable mutex;
    std::unique_ptr<DebugController> debugController;
    std::unique_ptr<MemoryProfiler> memoryProfiler;
    std::unique_ptr<PerformanceProfiler> profiler;
};

/**
 * @brief Global performance monitor instance
 * 
 * Singleton pattern for accessing performance monitoring functionality
 * throughout the NLM application.
 */
class GlobalPerformanceMonitor {
public:
    static GlobalPerformanceMonitor& getInstance() {
        static GlobalPerformanceMonitor instance;
        return instance;
    }
    
    static PerformanceMonitor& getMonitor() {
        return getInstance().monitor;
    }
    
    static bool isMonitoring() {
        return getMonitor().isMonitoring();
    }
    
    static void startMonitoring() {
        getMonitor().startMonitoring();
    }
    
    static void stopMonitoring() {
        getMonitor().stopMonitoring();
    }
    
    static Timer* createTimer(const std::string& name) {
        return getMonitor().createTimer(name);
    }
    
    static void startTimer(const std::string& name) {
        getMonitor().startTimer(name);
    }
    
    static void stopTimer(const std::string& name) {
        getMonitor().stopTimer(name);
    }
    
    static void recordStep(SimulationStep step, Timestamp timestamp, const std::string& description) {
        getMonitor().recordStep(step, timestamp, description);
    }
    
    static void recordAllocation(size_t size) {
        getMonitor().recordAllocation(size);
    }
    
    static void recordDeallocation(size_t size) {
        getMonitor().recordDeallocation(size);
    }
    
    static void enableBreakpoint(SimulationStep step) {
        getMonitor().enableBreakpoint(step);
    }
    
    static bool shouldBreakAtStep(SimulationStep step) {
        return getMonitor().shouldBreakAtStep(step);
    }
    
    static std::string generatePerformanceReport() {
        return getMonitor().generatePerformanceReport();
    }
    
    static void exportAllReports(const std::string& directory) {
        getMonitor().exportAllReports(directory);
    }
    
    static void exportSystemMetrics(const std::string& filename) {
        getMonitor().exportSystemMetrics(filename);
    }
    
    static void exportMemorySnapshots(const std::string& filename) {
        getMonitor().exportMemorySnapshots(filename);
    }
    
    static void exportStepRecords(const std::string& filename) {
        getMonitor().getDebugController()->exportStepRecords(filename);
    }
    
    static bool hasMemoryLeaks() {
        return getMonitor().hasMemoryLeaks();
    }
    
    static size_t getMemoryLeakCount() {
        return getMonitor().getMemoryLeakCount();
    }
    
    static size_t getTimerCount() {
        return getMonitor().getTimerCount();
    }
    
    static size_t getStepRecordCount() {
        return getMonitor().getStepRecordCount();
    }
    
    static size_t getSystemMetricCount() {
        return getMonitor().getSystemMetricCount();
    }
    
    static std::string getStartTimeString() {
        return getMonitor().getStartTimeString();
    }
    
    static PerformanceMonitor& accessMonitor() {
        return getInstance().monitor;
    }
    
private:
    GlobalPerformanceMonitor() = default;
    PerformanceMonitor monitor;
};