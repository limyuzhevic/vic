// Performance Profiler - Tracks simulation performance and identifies bottlenecks
// Provides detailed profiling information for optimization

#ifndef PERFORMANCE_PROFILER_HPP
#define PERFORMANCE_PROFILER_HPP

#include <chrono>
#include <string>
#include <vector>
#include <map>
#include <memory>
#include <mutex>
#include <iostream>
#include <iomanip>

namespace nlm {
namespace profiling {

struct PerformanceMetrics {
    std::string name;
    std::chrono::duration<double> elapsedTime;
    std::chrono::duration<double> cpuTime;
    size_t memoryUsage;
    uint64_t callCount;
    double averageTime;
    double minTime;
    double maxTime;
    
    PerformanceMetrics(const std::string& n) 
        : name(n), elapsedTime(0), cpuTime(0), memoryUsage(0), 
          callCount(0), averageTime(0), minTime(0), maxTime(0) {}
};

class PerformanceProfiler {
public:
    PerformanceProfiler();
    ~PerformanceProfiler();
    
    // Start profiling a section
    void startProfile(const std::string& sectionName);
    
    // Stop profiling a section
    void stopProfile(const std::string& sectionName);
    
    // Get metrics for a specific section
    const PerformanceMetrics* getMetrics(const std::string& sectionName) const;
    
    // Get all metrics
    const std::vector<PerformanceMetrics>& getAllMetrics() const { return metrics_; }
    
    // Clear all metrics
    void clearMetrics();
    
    // Print performance report
    void printReport(std::ostream& out = std::cout) const;
    
    // Print detailed timing information
    void printDetailedReport(std::ostream& out = std::cout) const;
    
    // Get total simulation time
    double getTotalSimulationTime() const { return totalTime_; }
    
    // Set profiling enabled/disabled
    void setEnabled(bool enabled) { enabled_ = enabled; }
    bool isEnabled() const { return enabled_; }
    
    // Reset for a new simulation run
    void reset();
    
    // Get singleton instance
    static PerformanceProfiler& getInstance();
    
private:
    struct ProfileContext {
        std::chrono::high_resolution_clock::time_point startTime;
        std::chrono::high_resolution_clock::time_point cpuStartTime;
        size_t initialMemory;
    };
    
    std::map<std::string, ProfileContext> activeProfiles_;
    std::map<std::string, PerformanceMetrics> completedMetrics_;
    std::vector<PerformanceMetrics> metrics_;
    
    double totalTime_;
    bool enabled_;
    mutable std::mutex mutex_;
    
    // Helper functions
    void addMetric(const std::string& name, 
                   const std::chrono::duration<double>& elapsed,
                   const std::chrono::duration<double>& cpu,
                   size_t memoryDelta,
                   uint64_t callCount);
    
    size_t getCurrentMemoryUsage() const;
};

// Memory profiler - Tracks memory usage patterns
class MemoryProfiler {
public:
    MemoryProfiler();
    ~MemoryProfiler();
    
    // Record memory snapshot
    void recordSnapshot(const std::string& name);
    
    // Get memory growth for a snapshot
    double getMemoryGrowth(const std::string& beforeName, 
                           const std::string& afterName) const;
    
    // Print memory report
    void printMemoryReport(std::ostream& out = std::cout) const;
    
    // Clear all snapshots
    void clearSnapshots();
    
    // Get memory usage in MB
    double getMemoryUsageMB() const;
    
private:
    struct MemorySnapshot {
        std::string name;
        std::chrono::high_resolution_clock::time_point timestamp;
        size_t memoryUsage;
    };
    
    std::vector<MemorySnapshot> snapshots_;
    mutable std::mutex mutex_;
};

// Benchmark utility - Compares performance of different configurations
class Benchmark {
public:
    Benchmark();
    ~Benchmark();
    
    // Add a benchmark test
    void addBenchmark(const std::string& name, 
                      std::function<void()> test,
                      const std::map<std::string, std::string>& params = {});
    
    // Run all benchmarks
    void runBenchmarks();
    
    // Print benchmark results
    void printBenchmarkResults(std::ostream& out = std::cout) const;
    
    // Get benchmark results
    const std::vector<std::pair<std::string, std::vector<double>>> getResults() const { return results_; }
    
private:
    struct BenchmarkTest {
        std::string name;
        std::function<void()> test;
        std::map<std::string, std::string> params;
        std::vector<double> results;
        double avgTime;
    };
    
    std::vector<BenchmarkTest> benchmarks_;
    std::vector<std::pair<std::string, std::vector<double>>> results_;
};

// Performance monitoring integration with simulation
class SimulationProfiler {
public:
    SimulationProfiler();
    ~SimulationProfiler();
    
    // Start simulation profiling
    void startSimulationProfiling(const std::string& simulationName);
    
    // End simulation profiling
    void stopSimulationProfiling();
    
    // Record simulation step metrics
    void recordStepMetrics(uint64_t step, 
                           size_t spikeCount,
                           size_t neuronCount,
                           double activityLevel);
    
    // Print simulation performance summary
    void printSimulationPerformance(std::ostream& out = std::cout) const;
    
    // Get simulation statistics
    const std::vector<std::pair<uint64_t, size_t>> getSpikeHistory() const { return spikeHistory_; }
    size_t getMaxNeurons() const { return maxNeurons_; }
    double getAverageActivityLevel() const { return avgActivityLevel_; }
    
private:
    std::string simulationName_;
    std::chrono::high_resolution_clock::time_point startTime_;
    std::vector<std::pair<uint64_t, size_t>> spikeHistory_;
    std::vector<double> activityHistory_;
    size_t maxNeurons_;
    double totalActivityLevel_;
    uint64_t totalSteps_;
    mutable std::mutex mutex_;
};

// Global performance monitoring functions
inline void startProfiling(const std::string& section) {
    PerformanceProfiler::getInstance().startProfile(section);
}

inline void stopProfiling(const std::string& section) {
    PerformanceProfiler::getInstance().stopProfile(section);
}

inline void printPerformanceReport() {
    PerformanceProfiler::getInstance().printReport();
}

inline void resetPerformanceProfiler() {
    PerformanceProfiler::getInstance().reset();
}

// Convenience macros for easy profiling
#define PROFILE_SECTION(name) nlm::profiling::PerformanceProfiler::getInstance().startProfile(name)
#define END_PROFILE_SECTION(name) nlm::profiling::PerformanceProfiler::getInstance().stopProfile(name)
#define PROFILE_FUNCTION() PROFILE_SECTION(__FUNCTION__)
#define END_PROFILE_FUNCTION() END_PROFILE_SECTION(__FUNCTION__)

} // namespace profiling
} // namespace nlm

#endif // PERFORMANCE_PROFILER_HPP