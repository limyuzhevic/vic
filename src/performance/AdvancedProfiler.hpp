// Advanced performance monitoring and profiling
#pragma once

#include <chrono>
#include <thread>
#include <mutex>
#include <vector>
#include <unordered_map>
#include <iostream>
#include <iomanip>
#include <algorithm>
#include <atomic>
#include <functional>
#include <memory>
#include <string>
#include <sstream>

namespace nlm {

class PerformanceMonitor {
public:
    struct PerformanceMetrics {
        std::chrono::steady_clock::time_point timestamp;
        
        // CPU metrics
        double cpuUsage;
        size_t activeThreads;
        size_t totalThreads;
        
        // Memory metrics
        size_t memoryUsage;
        size_t peakMemoryUsage;
        size_t allocatedMemory;
        size_t deallocatedMemory;
        
        // Performance metrics
        double tasksPerSecond;
        double throughput;
        double efficiency;
        std::chrono::milliseconds averageTaskTime;
        std::chrono::milliseconds minTaskTime;
        std::chrono::milliseconds maxTaskTime;
        
        // Cache metrics
        double cacheHitRate;
        size_t cacheSize;
        
        // SIMD metrics
        bool simdEnabled;
        bool avxEnabled;
        bool avx512Enabled;
        
        // Monitoring metrics
        size_t monitoringSamples;
        double cpuTemperature;
        double memoryTemperature;
        
        PerformanceMetrics() : cpuUsage(0.0), activeThreads(0), totalThreads(0),
                               memoryUsage(0), peakMemoryUsage(0), allocatedMemory(0),
                               deallocatedMemory(0), tasksPerSecond(0.0), throughput(0.0),
                               efficiency(0.0), averageTaskTime(0ms), minTaskTime(0ms),
                               maxTaskTime(0ms), cacheHitRate(0.0), cacheSize(0),
                               simdEnabled(false), avxEnabled(false), avx512Enabled(false),
                               monitoringSamples(0), cpuTemperature(0.0), memoryTemperature(0.0) {}
    };
    
    struct EventMetrics {
        std::string eventName;
        std::chrono::steady_clock::time_point startTime;
        std::chrono::steady_clock::time_point endTime;
        std::chrono::milliseconds duration;
        size_t samples;
        double averageDuration;
        double minDuration;
        double maxDuration;
        double totalDuration;
        size_t threadId;
        std::string threadName;
        
        EventMetrics(const std::string& name, size_t id)
            : eventName(name), startTime(), endTime(), duration(0ms), samples(0),
              averageDuration(0.0), minDuration(0.0), maxDuration(0.0), totalDuration(0.0),
              threadId(id), threadName("Thread-" + std::to_string(id)) {}
    };
    
    struct SystemStats {
        std::chrono::steady_clock::time_point uptime;
        size_t totalTasksExecuted;
        size_t totalExceptionsThrown;
        double systemLoad;
        size_t memoryPressure;
        size_t cpuPressure;
        
        SystemStats() : totalTasksExecuted(0), totalExceptionsThrown(0),
                       systemLoad(0.0), memoryPressure(0), cpuPressure(0) {
            uptime = std::chrono::steady_clock::now();
        }
    };
    
    class EventProfiler {
    public:
        EventProfiler(size_t maxSamples = 10000) : maxSamples(maxSamples) {}
        
        void startEvent(const std::string& name, size_t threadId = std::this_thread::get_id()) {
            std::lock_guard<std::mutex> lock(mutex);
            
            EventMetrics metrics(name, threadId);
            metrics.startTime = std::chrono::steady_clock::now();
            events.push_back(std::move(metrics));
        }
        
        void endEvent(const std::string& name) {
            std::lock_guard<std::mutex> lock(mutex);
            
            auto it = std::find_if(events.begin(), events.end(), [&](const EventMetrics& e) {
                return e.eventName == name && e.endTime == std::chrono::steady_clock::time_point();
            });
            
            if (it != events.end()) {
                auto endTime = std::chrono::steady_clock::now();
                it->endTime = endTime;
                it->duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - it->startTime);
                it->samples++;
                it->totalDuration += it->duration.count();
                
                if (it->samples == 1 || it->duration < it->minDuration) {
                    it->minDuration = it->duration.count();
                }
                if (it->duration > it->maxDuration) {
                    it->maxDuration = it->duration.count();
                }
                
                if (it->totalDuration > 0.0) {
                    it->averageDuration = it->totalDuration / it->samples;
                }
            }
        }
        
        std::vector<EventMetrics> getEvents(const std::string& name) {
            std::lock_guard<std::mutex> lock(mutex);
            std::vector<EventMetrics> result;
            for (const auto& event : events) {
                if (event.eventName == name) {
                    result.push_back(event);
                }
            }
            return result;
        }
        
        void clearEvents(const std::string& name) {
            std::lock_guard<std::mutex> lock(mutex);
            events.erase(std::remove_if(events.begin(), events.end(),
                                       [&](const EventMetrics& e) { return e.eventName == name; }),
                         events.end());
        }
        
        void clearAllEvents() {
            std::lock_guard<std::mutex> lock(mutex);
            events.clear();
        }
        
        size_t getEventCount(const std::string& name) {
            std::lock_guard<std::mutex> lock(mutex);
            size_t count = 0;
            for (const auto& event : events) {
                if (event.eventName == name) {
                    count++;
                }
            }
            return count;
        }
        
        std::string generateReport() {
            std::stringstream ss;
            
            ss << "=== Event Profiling Report ===" << std::endl;
            ss << "Total events recorded: " << events.size() << std::endl;
            
            // Group events by name
            std::unordered_map<std::string, std::vector<EventMetrics>> groupedEvents;
            for (const auto& event : events) {
                groupedEvents[event.eventName].push_back(event);
            }
            
            ss << "\nEvents by type:" << std::endl;
            for (const auto& pair : groupedEvents) {
                const auto& eventName = pair.first;
                const auto& eventList = pair.second;
                
                double totalDuration = 0.0;
                for (const auto& event : eventList) {
                    totalDuration += event.duration.count();
                }
                
                ss << "  " << eventName << ": " << eventList.size() << " samples, "
                   << std::fixed << std::setprecision(2) << totalDuration << "ms total, "
                   << std::setprecision(2) << (totalDuration / eventList.size()) << "ms average" << std::endl;
            }
            
            return ss.str();
        }
        
    private:
        std::vector<EventMetrics> events;
        std::mutex mutex;
        size_t maxSamples;
    };
    
    class MemoryProfiler {
    public:
        struct MemoryUsage {
            std::chrono::steady_clock::time_point timestamp;
            size_t totalAllocated;
            size_t totalDeallocated;
            size_t currentUsage;
            size_t peakUsage;
            double allocationRate;
            double deallocationRate;
            size_t allocationCount;
            size_t deallocationCount;
            
            MemoryUsage() : totalAllocated(0), totalDeallocated(0), currentUsage(0),
                           peakUsage(0), allocationRate(0.0), deallocationRate(0.0),
                           allocationCount(0), deallocationCount(0) {}
        };
        
        MemoryProfiler(size_t sampleInterval = 1000) : sampleInterval(sampleInterval) {}
        
        void recordMemoryUsage(const MemoryUsage& usage) {
            std::lock_guard<std::mutex> lock(mutex);
            memoryUsageHistory.push_back(usage);
            
            // Keep only recent samples
            if (memoryUsageHistory.size() > 1000) {
                memoryUsageHistory.erase(memoryUsageHistory.begin());
            }
        }
        
        MemoryUsage getCurrentMemoryUsage() const {
            std::lock_guard<std::mutex> lock(mutex);
            if (memoryUsageHistory.empty()) {
                return MemoryUsage();
            }
            return memoryUsageHistory.back();
        }
        
        std::vector<MemoryUsage> getMemoryUsageHistory() const {
            std::lock_guard<std::mutex> lock(mutex);
            return memoryUsageHistory;
        }
        
        double getAverageMemoryUsage() const {
            std::lock_guard<std::mutex> lock(mutex);
            if (memoryUsageHistory.empty()) {
                return 0.0;
            }
            
            double total = 0.0;
            for (const auto& usage : memoryUsageHistory) {
                total += usage.currentUsage;
            }
            
            return total / memoryUsageHistory.size();
        }
        
        size_t getPeakMemoryUsage() const {
            std::lock_guard<std::mutex> lock(mutex);
            size_t peak = 0;
            for (const auto& usage : memoryUsageHistory) {
                peak = std::max(peak, usage.peakUsage);
            }
            return peak;
        }
        
        std::string generateReport() {
            std::stringstream ss;
            
            ss << "=== Memory Profiling Report ===" << std::endl;
            ss << "Total samples: " << memoryUsageHistory.size() << std::endl;
            ss << "Current memory usage: " << getCurrentMemoryUsage().currentUsage << " bytes" << std::endl;
            ss << "Peak memory usage: " << getPeakMemoryUsage() << " bytes" << std::endl;
            ss << "Average memory usage: " << getAverageMemoryUsage() << " bytes" << std::endl;
            
            if (memoryUsageHistory.size() > 1) {
                ss << "\nMemory usage trend:" << std::endl;
                size_t sampleStep = std::max(1, static_cast<size_t>(memoryUsageHistory.size() / 10));
                
                for (size_t i = 0; i < memoryUsageHistory.size(); i += sampleStep) {
                    const auto& usage = memoryUsageHistory[i];
                    ss << "  Sample " << i << ": " << usage.currentUsage << " bytes ("
                       << (usage.currentUsage / 1024 / 1024) << " MB)" << std::endl;
                }
            }
            
            return ss.str();
        }
        
    private:
        std::vector<MemoryUsage> memoryUsageHistory;
        std::mutex mutex;
        size_t sampleInterval;
    };
    
    class ThreadProfiler {
    public:
        struct ThreadStats {
            std::thread::id threadId;
            std::string threadName;
            std::chrono::steady_clock::time_point startTime;
            size_t tasksExecuted;
            double averageTaskTime;
            double totalExecutionTime;
            size_t cacheHits;
            size_t cacheMisses;
            double cacheHitRate;
            std::chrono::milliseconds idleTime;
            std::chrono::milliseconds lastTaskTime;
            
            ThreadStats(std::thread::id id, const std::string& name)
                : threadId(id), threadName(name), startTime(std::chrono::steady_clock::now()),
                  tasksExecuted(0), averageTaskTime(0.0), totalExecutionTime(0.0),
                  cacheHits(0), cacheMisses(0), cacheHitRate(0.0), idleTime(0ms),
                  lastTaskTime(0ms) {}
        };
        
        ThreadProfiler() {}
        
        void registerThread(const std::string& name = "") {
            std::lock_guard<std::mutex> lock(mutex);
            
            std::thread::id id = std::this_thread::get_id();
            std::string threadName = name.empty() ? "Thread-" + std::to_string(threadId++) : name;
            
            ThreadStats stats(id, threadName);
            threadStats[id] = stats;
        }
        
        void unregisterThread() {
            std::lock_guard<std::mutex> lock(mutex);
            
            std::thread::id id = std::this_thread::get_id();
            threadStats.erase(id);
        }
        
        void recordTaskCompletion(std::chrono::milliseconds duration, size_t cacheHits = 0, size_t cacheMisses = 0) {
            std::lock_guard<std::mutex> lock(mutex);
            
            std::thread::id id = std::this_thread::get_id();
            auto it = threadStats.find(id);
            
            if (it != threadStats.end()) {
                it->second.tasksExecuted++;
                it->second.totalExecutionTime += duration.count();
                it->second.cacheHits += cacheHits;
                it->second.cacheMisses += cacheMisses;
                it->second.lastTaskTime = duration;
                
                if (it->second.tasksExecuted > 0) {
                    it->second.averageTaskTime = 
                        it->second.totalExecutionTime / it->second.tasksExecuted;
                }
                
                if (it->second.cacheHits + it->second.cacheMisses > 0) {
                    it->second.cacheHitRate = 
                        static_cast<double>(it->second.cacheHits) / 
                        (it->second.cacheHits + it->second.cacheMisses);
                }
            }
        }
        
        void recordIdleTime(std::chrono::milliseconds idleTime) {
            std::lock_guard<std::mutex> lock(mutex);
            
            std::thread::id id = std::this_thread::get_id();
            auto it = threadStats.find(id);
            
            if (it != threadStats.end()) {
                it->second.idleTime += idleTime;
            }
        }
        
        std::vector<ThreadStats> getThreadStats() const {
            std::lock_guard<std::mutex> lock(mutex);
            std::vector<ThreadStats> result;
            for (const auto& pair : threadStats) {
                result.push_back(pair.second);
            }
            return result;
        }
        
        ThreadStats getCurrentThreadStats() const {
            std::lock_guard<std::mutex> lock(mutex);
            std::thread::id id = std::this_thread::get_id();
            auto it = threadStats.find(id);
            
            if (it != threadStats.end()) {
                return it->second;
            }
            
            return ThreadStats(id, "Thread-" + std::to_string(id));
        }
        
        std::string generateReport() {
            std::stringstream ss;
            
            ss << "=== Thread Profiling Report ===" << std::endl;
            ss << "Total threads: " << threadStats.size() << std::endl;
            ss << "Total tasks executed: " << getTotalTasksExecuted() << std::endl;
            ss << "Average cache hit rate: " << getAverageCacheHitRate() << std::endl;
            
            ss << "\nDetailed thread statistics:" << std::endl;
            for (const auto& pair : threadStats) {
                const auto& stats = pair.second;
                ss << "  " << stats.threadName << " (ID: " << stats.threadId << "):" << std::endl;
                ss << "    Tasks executed: " << stats.tasksExecuted << std::endl;
                ss << "    Average task time: " << std::fixed << std::setprecision(2) 
                   << stats.averageTaskTime << "ms" << std::endl;
                ss << "    Cache hit rate: " << std::fixed << std::setprecision(2) 
                   << stats.cacheHitRate * 100 << "%" << std::endl;
                ss << "    Idle time: " << stats.idleTime.count() << "ms" << std::endl;
            }
            
            return ss.str();
        }
        
        size_t getTotalTasksExecuted() const {
            std::lock_guard<std::mutex> lock(mutex);
            size_t total = 0;
            for (const auto& pair : threadStats) {
                total += pair.second.tasksExecuted;
            }
            return total;
        }
        
        double getAverageCacheHitRate() const {
            std::lock_guard<std::mutex> lock(mutex);
            if (threadStats.empty()) {
                return 0.0;
            }
            
            double totalHitRate = 0.0;
            size_t count = 0;
            for (const auto& pair : threadStats) {
                totalHitRate += pair.second.cacheHitRate;
                count++;
            }
            
            return totalHitRate / count;
        }
        
    private:
        std::unordered_map<std::thread::id, ThreadStats> threadStats;
        std::mutex mutex;
        std::atomic<size_t> threadId{0};
    };
    
    PerformanceMonitor() : samplesPerSecond(10), enableSIMDMonitoring(true), 
                          enableMemoryMonitoring(true), enableThreadMonitoring(true),
                          enableEventMonitoring(true) {
        startMonitoringThread();
    }
    
    ~PerformanceMonitor() {
        stopMonitoringThread();
    }
    
    void startEvent(const std::string& name) {
        if (eventProfiler) {
            eventProfiler->startEvent(name, std::this_thread::get_id());
        }
    }
    
    void endEvent(const std::string& name) {
        if (eventProfiler) {
            eventProfiler->endEvent(name);
        }
    }
    
    void recordMemoryUsage(const MemoryProfiler::MemoryUsage& usage) {
        if (memoryProfiler) {
            memoryProfiler->recordMemoryUsage(usage);
        }
    }
    
    void recordTaskCompletion(std::chrono::milliseconds duration, size_t cacheHits = 0, size_t cacheMisses = 0) {
        if (threadProfiler) {
            threadProfiler->recordTaskCompletion(duration, cacheHits, cacheMisses);
        }
    }
    
    void recordIdleTime(std::chrono::milliseconds idleTime) {
        if (threadProfiler) {
            threadProfiler->recordIdleTime(idleTime);
        }
    }
    
    PerformanceMetrics getCurrentMetrics() {
        PerformanceMetrics metrics;
        
        auto now = std::chrono::steady_clock::now();
        metrics.timestamp = now;
        
        // Update metrics from all profilers
        if (eventProfiler) {
            // Update event metrics
        }
        
        if (memoryProfiler) {
            metrics.memoryUsage = memoryProfiler->getCurrentMemoryUsage().currentUsage;
            metrics.peakMemoryUsage = memoryProfiler->getPeakMemoryUsage();
        }
        
        if (threadProfiler) {
            auto threadStats = threadProfiler->getCurrentThreadStats();
            metrics.activeThreads = threadStats.tasksExecuted; // Approximation
            metrics.cacheHitRate = threadStats.cacheHitRate;
        }
        
        return metrics;
    }
    
    std::string generateFullReport() {
        std::stringstream ss;
        
        ss << "=== Complete Performance Monitoring Report ===" << std::endl;
        ss << "Generated at: " << std::chrono::system_clock::now() << std::endl;
        
        if (eventProfiler) {
            ss << eventProfiler->generateReport() << std::endl;
        }
        
        if (memoryProfiler) {
            ss << memoryProfiler->generateReport() << std::endl;
        }
        
        if (threadProfiler) {
            ss << threadProfiler->generateReport() << std::endl;
        }
        
        return ss.str();
    }
    
    void setMonitoringInterval(size_t samplesPerSecond) {
        this->samplesPerSecond = samplesPerSecond;
    }
    
    void enableSIMDMonitoring(bool enable) {
        enableSIMDMonitoring = enable;
    }
    
    void enableMemoryMonitoring(bool enable) {
        enableMemoryMonitoring = enable;
    }
    
    void enableThreadMonitoring(bool enable) {
        enableThreadMonitoring = enable;
    }
    
    void enableEventMonitoring(bool enable) {
        enableEventMonitoring = enable;
    }
    
private:
    void startMonitoringThread() {
        monitoringThread = std::thread(&PerformanceMonitor::monitoringLoop, this);
    }
    
    void stopMonitoringThread() {
        stopMonitoring = true;
        if (monitoringThread.joinable()) {
            monitoringThread.join();
        }
    }
    
    void monitoringLoop() {
        auto interval = std::chrono::milliseconds(1000 / samplesPerSecond);
        
        while (!stopMonitoring) {
            // Collect metrics
            auto metrics = getCurrentMetrics();
            
            // Record metrics
            // In a real implementation, this would store metrics to a database or file
            
            std::this_thread::sleep_for(interval);
        }
    }
    
    std::atomic<bool> stopMonitoring{false};
    size_t samplesPerSecond;
    bool enableSIMDMonitoring;
    bool enableMemoryMonitoring;
    bool enableThreadMonitoring;
    bool enableEventMonitoring;
    
    std::unique_ptr<EventProfiler> eventProfiler;
    std::unique_ptr<MemoryProfiler> memoryProfiler;
    std::unique_ptr<ThreadProfiler> threadProfiler;
    std::thread monitoringThread;
};

} // namespace nlm