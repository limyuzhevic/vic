// Benchmark measurement utilities
// Provides timing and resource measurement for performance testing

#pragma once

#include <chrono>
#include <memory>
#include <string>
#include <vector>
#include <unordered_map>
#include <thread>
#include <sys/resource.h>  // For memory usage
#include <unistd.h>        // For getrusage

// Simple memory tracker
class MemoryTracker {
public:
    static size_t getCurrentMemoryUsage() {
        struct rusage usage;
        getrusage(RUSAGE_SELF, &usage);
        return usage.ru_maxrss * 1024;  // Convert KB to bytes on Linux
    }
    
    static size_t getPeakMemoryUsage() {
        return getCurrentMemoryUsage();  // ru_maxrss tracks peak
    }
};

// Simple CPU cycle counter (platform-specific implementation)
class CPUCycleCounter {
public:
    static uint64_t getCurrentCycleCount() {
        // Use RDTSC on x86 or platform-specific counter
        // This is a simplified implementation
#ifdef __x86_64__
        uint32_t lo, hi;
        asm volatile ("rdtsc" : "=a"(lo), "=d"(hi));
        return (static_cast<uint64_t>(hi) << 32) | lo;
#else
        return 0;  // Not implemented for other architectures
#endif
    }
};

// Timer for measuring elapsed time
class Timer {
private:
    std::chrono::high_resolution_clock::time_point startTime;
    std::chrono::high_resolution_clock::time_point pauseTime;
    bool isRunning;
    bool isPaused;
    std::chrono::duration<double> totalElapsed;
    std::chrono::duration<double> pausedTime;
    
public:
    Timer() : isRunning(false), isPaused(false), totalElapsed(0.0), pausedTime(0.0) {}
    
    void start() {
        if (!isRunning) {
            startTime = std::chrono::high_resolution_clock::now();
            isRunning = true;
            isPaused = false;
        }
    }
    
    void pause() {
        if (isRunning && !isPaused) {
            pauseTime = std::chrono::high_resolution_clock::now();
            isPaused = true;
        }
    }
    
    void resume() {
        if (isRunning && isPaused) {
            auto pauseDuration = std::chrono::high_resolution_clock::now() - pauseTime;
            pausedTime += pauseDuration;
            isPaused = false;
        }
    }
    
    void stop() {
        if (isRunning) {
            auto endTime = std::chrono::high_resolution_clock::now();
            auto elapsed = endTime - startTime - pausedTime;
            totalElapsed += elapsed;
            isRunning = false;
            isPaused = false;
        }
    }
    
    double getElapsed() const {
        if (isRunning && !isPaused) {
            auto current = std::chrono::high_resolution_clock::now();
            return (current - startTime - pausedTime).count();
        }
        return totalElapsed.count();
    }
    
    bool isRunning() const { return isRunning && !isPaused; }
    bool isPaused() const { return isPaused; }
    void reset() {
        isRunning = false;
        isPaused = false;
        totalElapsed = std::chrono::duration<double>(0.0);
        pausedTime = std::chrono::duration<double>(0.0);
    }
};

// Comprehensive benchmark measurement
struct BenchmarkMetrics {
    std::string testName;
    std::string component;
    size_t scaleFactor;
    
    // Timing metrics
    double wallTime;      // Wall clock time
    double cpuTime;       // CPU time
    double stepsPerSecond;
    
    // Resource metrics
    size_t memoryUsage;   // Peak memory usage in bytes
    uint64_t cpuCycles;   // CPU cycles
    
    // Operation metrics
    size_t operations;    // Number of operations
    double opsPerSecond;  // Operations per second
    
    // Additional metrics
    double memoryPerOp;   // Memory usage per operation
    double timePerOp;     // Time per operation
    double efficiency;    // Operations per cycle
    
    BenchmarkMetrics() : scaleFactor(1), wallTime(0.0), cpuTime(0.0),
                        stepsPerSecond(0.0), memoryUsage(0), cpuCycles(0),
                        operations(0), opsPerSecond(0.0), memoryPerOp(0.0),
                        timePerOp(0.0), efficiency(0.0) {}
};

// Comprehensive benchmark collector
class BenchmarkCollector {
private:
    std::vector<BenchmarkMetrics> benchmarks;
    std::unordered_map<std::string, std::vector<BenchmarkMetrics>> benchmarksByComponent;
    std::unordered_map<size_t, std::vector<BenchmarkMetrics>> benchmarksByScale;
    
public:
    void addBenchmark(const BenchmarkMetrics& metrics) {
        benchmarks.push_back(metrics);
        benchmarksByComponent[metrics.component].push_back(metrics);
        benchmarksByScale[metrics.scaleFactor].push_back(metrics);
    }
    
    const std::vector<BenchmarkMetrics>& getAllBenchmarks() const {
        return benchmarks;
    }
    
    const std::vector<BenchmarkMetrics>& getBenchmarksByComponent(const std::string& component) const {
        auto it = benchmarksByComponent.find(component);
        return (it != benchmarksByComponent.end()) ? it->second : benchmarksByComponent[""];
    }
    
    const std::vector<BenchmarkMetrics>& getBenchmarksByScale(size_t scale) const {
        auto it = benchmarksByScale.find(scale);
        return (it != benchmarksByScale.end()) ? it->second : benchmarksByScale[1];
    }
    
    BenchmarkMetrics getAverageByComponent(const std::string& component) const {
        BenchmarkMetrics avg;
        auto& componentBenchmarks = getBenchmarksByComponent(component);
        if (componentBenchmarks.empty()) return avg;
        
        size_t count = componentBenchmarks.size();
        for (const auto& bench : componentBenchmarks) {
            avg.wallTime += bench.wallTime;
            avg.cpuTime += bench.cpuTime;
            avg.stepsPerSecond += bench.stepsPerSecond;
            avg.memoryUsage += bench.memoryUsage;
            avg.cpuCycles += bench.cpuCycles;
            avg.operations += bench.operations;
            avg.opsPerSecond += bench.opsPerSecond;
        }
        
        avg.wallTime /= count;
        avg.cpuTime /= count;
        avg.stepsPerSecond /= count;
        avg.memoryUsage /= count;
        avg.cpuCycles /= count;
        avg.operations /= count;
        avg.opsPerSecond /= count;
        
        // Calculate derived metrics
        avg.memoryPerOp = (avg.operations > 0) ? static_cast<double>(avg.memoryUsage) / avg.operations : 0.0;
        avg.timePerOp = (avg.operations > 0) ? avg.wallTime / avg.operations : 0.0;
        avg.efficiency = (avg.cpuCycles > 0) ? static_cast<double>(avg.operations) / avg.cpuCycles : 0.0;
        
        return avg;
    }
    
    size_t getNumBenchmarks() const {
        return benchmarks.size();
    }
    
    void printSummary() const {
        std::cout << "=== Benchmark Summary ===" << std::endl;
        std::cout << "Total benchmarks: " << getNumBenchmarks() << std::endl;
        
        // Print averages by component
        std::cout << "\nAverages by component:" << std::endl;
        for (const auto& pair : benchmarksByComponent) {
            const auto& component = pair.first;
            if (component.empty()) continue;
            
            auto avg = getAverageByComponent(component);
            std::cout << "  " << component << ": "
                     << "Wall Time: " << std::scientific << avg.wallTime << "s, "
                     << "Ops/sec: " << std::scientific << avg.opsPerSecond << ", "
                     << "Memory/Op: " << avg.memoryPerOp << " bytes" << std::endl;
        }
    }
};

// Scoped timer for easy timing
class ScopedTimer {
private:
    std::string name;
    Timer timer;
    BenchmarkMetrics metrics;
    BenchmarkCollector& collector;
    
public:
    ScopedTimer(const std::string& testName, const std::string& component, 
                size_t scale, BenchmarkCollector& collectorRef)
        : name(testName), component(component), scale(scale), collector(collectorRef) {
        timer.start();
        metrics.testName = testName;
        metrics.component = component;
        metrics.scaleFactor = scale;
    }
    
    ~ScopedTimer() {
        timer.stop();
        
        // Get measurements
        metrics.wallTime = timer.getElapsed();
        metrics.cpuTime = metrics.wallTime;  // Simplified - in reality need separate CPU timer
        metrics.memoryUsage = MemoryTracker::getPeakMemoryUsage();
        metrics.cpuCycles = CPUCycleCounter::getCurrentCycleCount();
        
        // Note: operations count should be set by the caller
        metrics.opsPerSecond = (metrics.operations > 0) ? 
            static_cast<double>(metrics.operations) / metrics.wallTime : 0.0;
        metrics.stepsPerSecond = metrics.opsPerSecond;  // For most neural operations
        
        // Calculate derived metrics
        metrics.memoryPerOp = (metrics.operations > 0) ? 
            static_cast<double>(metrics.memoryUsage) / metrics.operations : 0.0;
        metrics.timePerOp = (metrics.operations > 0) ? 
            metrics.wallTime / metrics.operations : 0.0;
        metrics.efficiency = (metrics.cpuCycles > 0) ? 
            static_cast<double>(metrics.operations) / metrics.cpuCycles : 0.0;
        
        collector.addBenchmark(metrics);
        
        std::cout << "  " << name << " completed in " << std::scientific 
                 << metrics.wallTime << "s (" << metrics.opsPerSecond << " ops/sec)" << std::endl;
    }
    
    void setOperations(size_t ops) {
        metrics.operations = ops;
    }
    
    void setStepsPerSecond(double steps) {
        metrics.stepsPerSecond = steps;
    }
};
