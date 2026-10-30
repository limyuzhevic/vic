#pragma once

#include <chrono>
#include <string>
#include <vector>
#include <map>
#include <functional>
#include <memory>
#include <algorithm>
#include <fstream>
#include <sstream>
#include <iomanip>

namespace nlm {

// Timing utilities for profiling
class HighResolutionTimer {
public:
    HighResolutionTimer() : start_time_(std::chrono::high_resolution_clock::now()) {}
    
    double elapsed() const {
        auto end_time = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time_);
        return duration.count() / 1000.0; // Convert to milliseconds
    }
    
    void reset() {
        start_time_ = std::chrono::high_resolution_clock::now();
    }
    
private:
    std::chrono::high_resolution_clock::time_point start_time_;
};

// Performance profiling data
struct PerformanceMetrics {
    std::string name;
    double duration; // milliseconds
    size_t memoryUsed; // bytes
    size_t peakMemory; // bytes
    size_t cpuTime; // microseconds
    size_t stepCount;
    double fps; // frames per second
    
    PerformanceMetrics(const std::string& n = "", double d = 0.0, 
                      size_t mu = 0, size_t pu = 0, size_t ct = 0,
                      size_t sc = 0, double f = 0.0)
        : name(n), duration(d), memoryUsed(mu), peakMemory(pu), 
          cpuTime(ct), stepCount(sc), fps(f) {}
};

// Memory tracking
class MemoryTracker {
public:
    MemoryTracker() : peak_memory_(0), current_memory_(0) {}
    
    void update(size_t memory) {
        current_memory_ = memory;
        if (memory > peak_memory_) {
            peak_memory_ = memory;
        }
    }
    
    size_t getCurrent() const { return current_memory_; }
    size_t getPeak() const { return peak_memory_; }
    void reset() { peak_memory_ = current_memory_ = 0; }
    
private:
    size_t peak_memory_;
    size_t current_memory_;
};

// Statistics collector
class StatisticsCollector {
public:
    StatisticsCollector() : total_steps_(0), total_spikes_(0), 
                           start_time_(std::chrono::high_resolution_clock::now()) {}
    
    void recordStep(size_t spikes, size_t activeNeurons, 
                   double timestep, size_t memoryUsage);
    
    void recordOperation(const std::string& operation, double duration);
    
    double getElapsedTime() const;
    size_t getTotalSteps() const { return total_steps_; }
    size_t getTotalSpikes() const { return total_spikes_; }
    double getAverageFiringRate() const;
    size_t getAverageActiveNeurons() const;
    double getAverageStepTime() const;
    
    const std::vector<PerformanceMetrics>& getPerformanceHistory() const { return performance_history_; }
    const std::vector<PerformanceMetrics>& getStepPerformance() const { return step_performance_; }
    
    std::string generateReport() const;
    std::string generateDetailedReport() const;
    
    void saveReport(const std::string& filename) const;
    
    void reset();
    
private:
    std::chrono::high_resolution_clock::time_point start_time_;
    size_t total_steps_;
    size_t total_spikes_;
    size_t total_active_neurons_;
    std::vector<PerformanceMetrics> performance_history_;
    std::vector<PerformanceMetrics> step_performance_;
    std::map<std::string, std::vector<double>> operation_timings_;
    MemoryTracker memory_tracker_;
    HighResolutionTimer step_timer_;
};

// Advanced brain statistics
class AdvancedBrainStatistics {
public:
    AdvancedBrainStatistics() = default;
    
    void collectStatistics(const class Brain& brain, 
                          const StatisticsCollector& collector,
                          size_t step_number);
    
    std::string generateAdvancedReport(const Brain& brain) const;
    
    struct NeuralHealthMetrics {
        double excitationInhibitionRatio;
        double averageMembranePotential;
        double averageFiringRate;
        size_t healthyNeurons;
        size_t activeNeurons;
        double synchronyIndex;
        
        NeuralHealthMetrics() : excitationInhibitionRatio(0.0), 
                               averageMembranePotential(0.0), 
                               averageFiringRate(0.0), 
                               healthyNeurons(0), activeNeurons(0),
                               synchronyIndex(0.0) {}
    };
    
    const NeuralHealthMetrics& getNeuralHealth() const { return neural_health_; }
    
private:
    NeuralHealthMetrics neural_health_;
    std::vector<double> membrane_potential_history_;
    std::vector<size_t> active_neuron_history_;
    
    double calculateSynchronyIndex(const std::vector<size_t>& active_neurons) const;
};

// Performance profiler
class PerformanceProfiler {
public:
    PerformanceProfiler() : active_(false) {}
    
    void startProfiling(const std::string& name);
    void stopProfiling(const std::string& name);
    
    void startStepProfiling();
    void endStepProfiling(size_t spikes, size_t activeNeurons,
                         double timestep, size_t memoryUsage);
    
    void enableMemoryTracking() { memory_tracker_.update(0); }
    void updateMemoryUsage(size_t memory) { memory_tracker_.update(memory); }
    
    const StatisticsCollector& getStatistics() const { return statistics_; }
    const std::vector<PerformanceMetrics>& getPerformanceHistory() const { 
        return statistics_.getPerformanceHistory(); 
    }
    
    std::string generateProfilingReport() const;
    std::string generatePerformanceReport() const;
    
    void saveProfilingData(const std::string& filename) const;
    
    void reset();
    
private:
    bool active_;
    StatisticsCollector statistics_;
    std::map<std::string, HighResolutionTimer> operation_timers_;
    MemoryTracker memory_tracker_;
    HighResolutionTimer step_timer_;
};

// Benchmarking utilities
class Benchmarker {
public:
    Benchmarker() = default;
    
    struct BenchmarkResult {
        std::string testName;
        double duration; // milliseconds
        size_t iterations;
        double avgDuration; // milliseconds
        double minDuration; // milliseconds
        double maxDuration; // milliseconds
        double stdDeviation; // milliseconds
    };
    
    BenchmarkResult runBenchmark(const std::string& name,
                                std::function<void()> benchmark_func,
                                size_t iterations = 100);
    
    std::vector<BenchmarkResult> getAllResults() const { return results_; }
    std::string generateBenchmarkReport() const;
    
    void saveBenchmarkResults(const std::string& filename) const;
    
    void reset() { results_.clear(); }
    
private:
    std::vector<BenchmarkResult> results_;
};

// CLI commands for profiling
class ProfilingCommands {
public:
    static void registerProfilingCommands();
    
    static bool handleProfilingCommand(const std::string& command,
                                     PerformanceProfiler& profiler,
                                     const class Brain* brain = nullptr);
    
private:
    static void printProfilingStatus(const PerformanceProfiler& profiler);
    static void printPerformanceReport(const PerformanceProfiler& profiler);
    static void saveProfilingReport(const PerformanceProfiler& profiler,
                                   const std::string& filename);
    static void saveBenchmarkReport(const Benchmarker& benchmarker,
                                   const std::string& filename);
};

} // namespace nlm