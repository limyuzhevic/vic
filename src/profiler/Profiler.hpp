// ============================================================================
// Profiler Implementation
// ============================================================================

#pragma once

#include "../core/Logger/Logger.hpp"
#include <chrono>
#include <thread>
#include <vector>
#include <map>
#include <string>
#include <memory>
#include <atomic>
#include <mutex>

namespace nlm {

struct ProfilerEvent {
    std::string name;
    std::chrono::high_resolution_clock::time_point startTime;
    std::chrono::duration<double> duration;
    std::thread::id threadId;
    uint64_t callCount;
    double totalDuration;
    double minDuration;
    double maxDuration;
    
    ProfilerEvent() : callCount(0), totalDuration(0.0), minDuration(1e9), maxDuration(0.0) {}
};

struct ProfilerStats {
    uint64_t totalEvents;
    double totalTime;
    uint64_t memoryUsageBytes;
    uint64_t cpuTimeNanos;
    std::map<std::string, double> percentiles;
    std::vector<std::string> topEvents;
    
    ProfilerStats() : totalEvents(0), totalTime(0.0), memoryUsageBytes(0), cpuTimeNanos(0) {}
};

class Profiler {
public:
    Profiler();
    ~Profiler();
    
    // Start/stop profiling
    void start(const std::string& name);
    void stop(const std::string& name);
    
    // Utility profiling functions
    void profileFunction(const std::string& name, std::function<void()> func);
    
    // Time measurement
    double measureTime(const std::string& name, std::function<void()> func);
    
    // Memory profiling
    void startMemoryProfiling();
    double getMemoryUsageMB() const;
    std::vector<std::pair<std::string, uint64_t>> getMemoryAllocationStack() const;
    
    // Performance statistics
    ProfilerStats getStats() const;
    std::vector<ProfilerEvent> getEvents(const std::string& name = "") const;
    
    // Event management
    void clearEvents();
    void setMaxEvents(size_t maxEvents);
    size_t getMaxEvents() const;
    
    // Filtering and analysis
    void setEventFilter(const std::string& pattern);
    bool matchesFilter(const std::string& name) const;
    
    // Export
    std::string exportToJSON(const std::string& filename = "") const;
    std::string exportToCSV(const std::string& filename = "") const;
    std::string exportToText() const;
    
    // Configuration
    void setSamplingRate(double samplesPerSecond);
    void setEnabled(bool enabled);
    bool isEnabled() const;
    
    // Neural activity profiling
    void startNeuralActivityProfiling();
    void stopNeuralActivityProfiling();
    double getAverageFiringRate() const;
    uint64_t getTotalSpikesProcessed() const;
    std::vector<float> getNeuralActivityHistogram() const;
    
    // Bottleneck detection
    std::vector<std::string> detectBottlenecks() const;
    std::vector<std::string> getPerformanceRecommendations() const;
    
    // Real-time monitoring
    void startRealTimeMonitoring(double intervalSeconds);
    void stopRealTimeMonitoring();
    std::string getRealTimeReport() const;
    
    // Thread profiling
    void startThreadProfiling();
    void stopThreadProfiling();
    std::map<std::thread::id, std::string> getThreadNames() const;
    std::vector<std::string> getThreadActivity() const;
    
    // Integration with brain systems
    void integrateWithBrain(const std::shared_ptr<Brain>& brain);
    void startBrainProfiling();
    void stopBrainProfiling();
    
    // Singleton access
    static Profiler& getInstance();
    static void initialize();
    static void shutdown();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    static Profiler* instance_;
    static std::mutex initializationMutex_;
};

} // namespace nlm
