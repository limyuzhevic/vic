#pragma once

#include <chrono>
#include <string>
#include <vector>
#include <unordered_map>
#include <mutex>
#include <iomanip>
#include <sstream>
#include <atomic>

namespace nlm {

/**
 * Real-time profiler for continuous performance monitoring
 * 
 * Provides real-time performance tracking with thread-safe operations,
 * customizable sampling rates, and automatic bottleneck detection.
 */

class RealTimeProfiler {
public:
    struct ThreadSample {
        uint64_t threadId;
        std::string threadName;
        double cpuUsage;
        double memoryUsage;
        std::chrono::duration<double> activeTime;
        std::chrono::duration<double> totalTime;
        uint64_t samples;
        std::chrono::high_resolution_clock::time_point lastSampleTime;
        
        ThreadSample(uint64_t id, const std::string& name)
            : threadId(id), threadName(name)
            , cpuUsage(0.0), memoryUsage(0.0)
            , activeTime(0.0), totalTime(0.0), samples(0) {}
    };

    struct SystemSample {
        double timestamp;
        double cpuUsage;
        double memoryUsage;
        double memoryAvailable;
        uint64_t threadCount;
        uint64_t activeThreadCount;
        std::chrono::duration<double> uptime;
        std::chrono::duration<double> idleTime;
        std::chrono::duration<double> totalTime;
        
        SystemSample()
            : timestamp(0.0), cpuUsage(0.0), memoryUsage(0.0), memoryAvailable(0.0)
            , threadCount(0), activeThreadCount(0)
            , uptime(0.0), idleTime(0.0), totalTime(0.0) {}
    };

    enum class ProfileTarget {
        ALL_THREADS,
        MAIN_THREAD_ONLY,
        USER_THREADS_ONLY,
        BACKGROUND_THREADS_ONLY
    };

    static RealTimeProfiler& getInstance() {
        static RealTimeProfiler instance;
        return instance;
    }

    void startProfiling(double sampleIntervalSec = 0.1, ProfileTarget target = ProfileTarget::ALL_THREADS);
    void stopProfiling();
    bool isProfiling() const { return isProfiling_; }
    
    void recordThreadSample(uint64_t threadId, const std::string& threadName,
                           double cpuUsage, double memoryUsage,
                           std::chrono::duration<double> activeTime,
                           std::chrono::duration<double> totalTime);
    
    void recordSystemSample(const SystemSample& sample);
    
    void setThreadFilter(const std::vector<std::string>& threadNames) {
        threadFilter_ = threadNames;
        filterEnabled_ = !threadFilter_.empty();
    }
    
    void clearThreadFilter() {
        threadFilter_.clear();
        filterEnabled_ = false;
    }
    
    const std::vector<ThreadSample>& getThreadSamples() const { return threadSamples_; }
    const std::vector<SystemSample>& getSystemSamples() const { return systemSamples_; }
    
    double getAverageCPUUsage() const;
    double getAverageMemoryUsage() const;
    double getPeakCPUUsage() const;
    double getPeakMemoryUsage() const;
    
    void setMaxSamples(size_t maxSamples) {
        maxThreadSamples_ = maxSamples;
        maxSystemSamples_ = maxSamples;
    }
    
    void clearSamples() {
        threadSamples_.clear();
        systemSamples_.clear();
    }
    
    std::string generateRealtimeReport() const;
    std::string generateTrendReport(size_t lastSamples = 100) const;
    
    void exportSamples(const std::string& filename);
    void exportCSV(const std::string& filename);
    
    double calculateCPUTrend(size_t samples) const;
    double calculateMemoryTrend(size_t samples) const;
    
    struct PerformanceAlert {
        std::chrono::high_resolution_clock::time_point timestamp;
        std::string type;
        std::string message;
        double value;
        double threshold;
        AlertLevel level;
        
        PerformanceAlert(const std::string& type, const std::string& message, 
                        double value, double threshold, AlertLevel level)
            : type(type), message(message), value(value), threshold(threshold)
            , level(level) {
            timestamp = std::chrono::high_resolution_clock::now();
        }
    };

    void addAlert(const PerformanceAlert& alert) {
        alerts_.push_back(alert);
        if (alerts_.size() > maxAlerts_) {
            alerts_.pop_front();
        }
    }
    
    const std::deque<PerformanceAlert>& getAlerts() const { return alerts_; }
    void clearAlerts() { alerts_.clear(); }
    
    bool hasHighCPUUsage(double threshold = 80.0) const;
    bool hasHighMemoryUsage(double threshold = 80.0) const;
    bool hasManyActiveThreads(size_t threshold = 10) const;

private:
    RealTimeProfiler()
        : isProfiling_(false)
        , samplingIntervalSec_(0.1)
        , target_(ProfileTarget::ALL_THREADS)
        , filterEnabled_(false)
        , maxThreadSamples_(10000)
        , maxSystemSamples_(10000)
        , maxAlerts_(1000)
    {
    }

    ~RealTimeProfiler() {
        stopProfiling();
    }

    void samplingLoop();
    void updateSystemSample();
    void updateThreadSamples();
    
    bool shouldSampleThread(uint64_t threadId, const std::string& threadName) const;
    bool shouldSampleSystem() const;

private:
    bool isProfiling_;
    double samplingIntervalSec_;
    ProfileTarget target_;
    bool filterEnabled_;
    std::vector<std::string> threadFilter_;
    
    size_t maxThreadSamples_;
    size_t maxSystemSamples_;
    size_t maxAlerts_;
    
    std::vector<ThreadSample> threadSamples_;
    std::vector<SystemSample> systemSamples_;
    std::deque<PerformanceAlert> alerts_;
    
    std::chrono::high_resolution_clock::time_point startTime_;
    std::thread samplingThread_;
    mutable std::mutex samplesMutex_;
    std::atomic<bool> stopRequested_;
};

} // namespace nlm
