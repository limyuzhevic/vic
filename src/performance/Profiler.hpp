// Performance Profiling and Benchmarking Framework
// Comprehensive profiling tools for NLM neural simulation

#pragma once

#include <chrono>
#include <string>
#include <vector>
#include <unordered_map>
#include <memory>
#include <algorithm>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <thread>
#include <atomic>
#include <numeric>
#include <type_traits>
#include <set>
#include <map>

namespace nlm {

// Performance Measurement Utilities
class Timer {
public:
    Timer() : start_(std::chrono::high_resolution_clock::now()) {}
    
    double elapsed() const {
        auto end = std::chrono::high_resolution_clock::now();
        return std::chrono::duration<double>(end - start_).count();
    }
    
    void reset() {
        start_ = std::chrono::high_resolution_clock::now();
    }
    
    std::chrono::high_resolution_clock::time_point start_time() const {
        return start_;
    }
    
private:
    std::chrono::high_resolution_clock::time_point start_;
};

class ScopedTimer {
public:
    ScopedTimer(std::string name, std::unordered_map<std::string, double>& results)
        : name_(std::move(name)), results_(results) {
        timer_.reset();
    }
    
    ~ScopedTimer() {
        results_[name_] = timer_.elapsed();
    }
    
private:
    std::string name_;
    std::unordered_map<std::string, double>& results_;
    Timer timer_;
};

class MemoryTracker {
public:
    struct MemoryStats {
        size_t totalAllocated;
        size_t currentUsage;
        size_t peakUsage;
        std::unordered_map<std::string, size_t> allocationsByType;
        std::vector<size_t> usageHistory;
    };
    
    static MemoryTracker& instance() {
        static MemoryTracker tracker;
        return tracker;
    }
    
    void trackAllocation(size_t size, const std::string& type = "unknown");
    void trackDeallocation(size_t size, const std::string& type = "unknown");
    void recordUsage(size_t usage) { usageHistory_.push_back(usage); }
    const MemoryStats& getStats() const { return stats_; }
    void reset() { stats_ = MemoryStats{}; }
    
    double getPeakUsageMB() const { return peakUsageMB_; }
    size_t getCurrentUsageBytes() const { return currentUsage_; }
    
private:
    MemoryTracker() : currentUsage_(0), peakUsage_(0), peakUsageMB_(0.0) {}
    
    MemoryStats stats_;
    std::atomic<size_t> currentUsage_;
    std::atomic<size_t> peakUsage_;
    std::vector<size_t> usageHistory_;
    double peakUsageMB_;
};

class CPUMonitor {
public:
    struct CPUStats {
        double usagePercent;
        std::vector<double> coreUsages;
        uint64_t cycles;
        uint64_t instructions;
        uint64_t cacheMisses;
        uint64_t branchMispredictions;
    };
    
    static CPUStats getCurrentStats();
    static std::vector<CPUStats> sampleCPU(int samples = 10, double intervalMs = 10.0);
};

class MemoryLeakDetector {
public:
    struct LeakReport {
        bool hasLeaks;
        std::unordered_map<std::string, size_t> leaksByType;
        std::vector<size_t> leakAddresses;
        double totalLeakSize;
    };
    
    static LeakReport detectLeaks(const std::string& sessionName = "default");
    static void startSession(const std::string& sessionName);
    static void endSession(const std::string& sessionName);
    static void recordAllocation(size_t size, const std::string& type, size_t address);
    static void recordDeallocation(size_t address);
};

class TimelineProfiler {
public:
    struct Event {
        std::string name;
        double timestamp;
        std::string threadId;
        std::string category;
        std::unordered_map<std::string, std::string> metadata;
    };
    
    static TimelineProfiler& instance() {
        static TimelineProfiler profiler;
        return profiler;
    }
    
    void addEvent(const std::string& name, const std::string& category = "general",
                  const std::unordered_map<std::string, std::string>& metadata = {});
    void startSegment(const std::string& name, const std::string& category = "general");
    void endSegment();
    void clear() { events_.clear(); }
    const std::vector<Event>& getEvents() const { return events_; }
    
    static void exportToChromeTrace(const std::string& filename);
    static void exportToJSON(const std::string& filename, const std::vector<Event>& events);
    
private:
    TimelineProfiler() {}
    std::vector<Event> events_;
    std::unordered_map<std::string, double> segmentStartTimes_;
};

class PerformanceCounter {
public:
    struct Counter {
        std::string name;
        std::atomic<uint64_t> value{0};
        std::string category;
        std::unordered_map<std::string, std::string> metadata;
    };
    
    static PerformanceCounter& instance() {
        static PerformanceCounter counter;
        return counter;
    }
    
    void increment(const std::string& name, uint64_t delta = 1, 
                   const std::string& category = "general",
                   const std::unordered_map<std::string, std::string>& metadata = {});
    uint64_t getValue(const std::string& name) const;
    std::vector<Counter> getAllCounters() const { return counters_; }
    void reset() { counters_.clear(); }
    
    static void recordSystemMetrics();
    
private:
    PerformanceCounter() {}
    std::unordered_map<std::string, Counter> counters_;
};

// Comprehensive Profiling Framework
class Profiler {
public:
    struct ProfileData {
        std::string sessionName;
        double startTime;
        double endTime;
        double duration;
        MemoryTracker::MemoryStats memoryStats;
        std::vector<CPUMonitor::CPUStats> cpuSamples;
        std::unordered_map<std::string, double> timerResults;
        TimelineProfiler::Event startEvent;
        TimelineProfiler::Event endEvent;
        bool hasLeaks;
        std::unordered_map<std::string, size_t> leaksByType;
    };
    
    static Profiler& instance() {
        static Profiler profiler;
        return profiler;
    }
    
    void startSession(const std::string& name);
    void endSession();
    const std::string& currentSessionName() const { return currentSessionName_; }
    
    ProfileData getCurrentProfile() const { return currentProfile_; }
    std::vector<ProfileData> getAllProfiles() const { return profiles_; }
    
    void recordTimer(const std::string& name, double duration);
    void recordMemoryUsage(size_t usage);
    void recordCPUStats(const CPUMonitor::CPUStats& stats);
    
    static void exportProfiles(const std::string& filename, Format format = Format::JSON);
    static void generateReport(const std::string& filename, const std::vector<ProfileData>& profiles);
    
    enum class Format {
        JSON,
        CSV,
        HTML
    };
    
private:
    Profiler() {}
    
    std::string currentSessionName_;
    ProfileData currentProfile_;
    std::vector<ProfileData> profiles_;
};

// Benchmarking Infrastructure
class BenchmarkSuite {
public:
    struct BenchmarkConfig {
        std::string name;
        std::string description;
        size_t iterations;
        std::unordered_map<std::string, std::string> parameters;
        std::vector<std::string> tags;
    };
    
    struct BenchmarkResult {
        std::string benchmarkName;
        std::unordered_map<std::string, std::string> parameters;
        size_t iterations;
        double totalTime;
        double meanTime;
        double stdDevTime;
        double minTime;
        double maxTime;
        double timePerIteration;
        MemoryTracker::MemoryStats memoryStats;
        CPUMonitor::CPUStats cpuStats;
        std::vector<double> iterationTimes;
        std::vector<MemoryTracker::MemoryStats> memoryHistory;
        std::vector<CPUMonitor::CPUStats> cpuHistory;
        double memoryPeakMB;
        double cpuAvgPercent;
        std::unordered_map<std::string, double> customMetrics;
    };
    
    static BenchmarkSuite& instance() {
        static BenchmarkSuite suite;
        return suite;
    }
    
    void addBenchmark(const BenchmarkConfig& config);
    BenchmarkResult run(const BenchmarkConfig& config);
    std::vector<BenchmarkResult> runBenchmarks(const std::vector<BenchmarkConfig>& configs);
    
    void setWarmupIterations(size_t iterations) { warmupIterations_ = iterations; }
    void setProfilerEnabled(bool enabled) { profilerEnabled_ = enabled; }
    
    std::vector<BenchmarkResult> getResults() const { return results_; }
    void clearResults() { results_.clear(); }
    
    static void exportResults(const std::string& filename, Format format = Format::CSV);
    static void generateComparisonReport(const std::vector<BenchmarkResult>& results, 
                                         const std::string& filename);
    
    static void detectRegressions(const std::vector<BenchmarkResult>& baseline,
                                  const std::vector<BenchmarkResult>& current,
                                  std::vector<BenchmarkResult>& regressions);
    
    enum class Format {
        CSV,
        JSON,
        TSV
    };
    
private:
    BenchmarkSuite() : warmupIterations_(10), profilerEnabled_(false) {}
    
    size_t warmupIterations_;
    bool profilerEnabled_;
    std::vector<BenchmarkResult> results_;
};

// Integration with Brain Systems
class BrainPerformanceProfiler {
public:
    struct BrainMetrics {
        size_t neuronCount;
        size_t synapseCount;
        size_t activeNeurons;
        size_t firingNeurons;
        size_t pendingSpikes;
        double simulationTime;
        double realTimeFactor;
        double avgFiringRate;
        double avgStepTime;
        double memoryUsageMB;
        double cpuUsagePercent;
        size_t totalSpikes;
        double totalEnergy;
    };
    
    static BrainMetrics profileBrainStep(const std::shared_ptr<Brain>& brain,
                                         size_t steps = 1,
                                         bool enableProfiler = false);
    
    static void profileBrainInitialization(const std::shared_ptr<Brain>& brain,
                                           const std::unordered_map<std::string, std::string>& config);
    
    static void profileNeuralDynamics(const std::shared_ptr<NeuralDynamics>& dynamics,
                                      size_t steps = 100);
    
    static void profilePlasticity(const std::shared_ptr<PlasticityRules>& plasticity,
                                  const std::vector<NeuralConnection>& connections,
                                  size_t steps = 1000);
    
    static void profileMemorySystems(const std::shared_ptr<MemorySystems>& memory,
                                     size_t operations = 1000);
    
    static void profileNeuromodulation(const std::shared_ptr<Neuromodulation>& neuromod,
                                       size_t steps = 100);
};

// System Resource Monitor
class SystemResourceMonitor {
public:
    struct SystemMetrics {
        double cpuUsagePercent;
        std::vector<double> coreUsages;
        size_t memoryUsageMB;
        size_t memoryAvailableMB;
        size_t memoryTotalMB;
        double diskUsagePercent;
        uint64_t networkBytesSent;
        uint64_t networkBytesReceived;
        double timestamp;
    };
    
    static SystemMetrics getCurrentMetrics();
    static std::vector<SystemMetrics> sampleSystem(int samples = 10, double intervalMs = 1000.0);
    
    static void startMonitoring(const std::string& outputFile, double intervalMs = 1000.0);
    static void stopMonitoring();
    
    static std::vector<SystemMetrics> getMonitoringData() { return monitoringData_; }
    
private:
    static std::vector<SystemMetrics> monitoringData_;
};

// Benchmark Runner
class BenchmarkRunner {
public:
    static void runAllBenchmarks(const std::string& configFile = "benchmark_config.json");
    static void runBenchmarksByTag(const std::string& tag, const std::string& outputFile = "");
    static void runParameterizedBenchmarks(const std::unordered_map<std::string, std::vector<std::string>>& paramGrid,
                                           const std::string& benchmarkName);
    
    static void printSummary(const std::vector<BenchmarkSuite::BenchmarkResult>& results);
    static void exportForRegressionTesting(const std::vector<BenchmarkSuite::BenchmarkResult>& results,
                                           const std::string& filename);
};

} // namespace nlm