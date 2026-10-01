// Performance benchmarking utilities for NLM neural simulation

#pragma once

#include <chrono>
#include <string>
#include <vector>
#include <map>
#include <functional>
#include <memory>
#include <iostream>
#include <iomanip>
#include <sstream>

namespace nlm {

// Benchmarking result structure
struct BenchmarkResult {
    std::string name;                     // Benchmark name
    std::chrono::duration<double> elapsed; // Total execution time
    std::chrono::duration<double> user;   // User CPU time
    std::chrono::duration<double> system; // System CPU time
    uint64_t memoryUsage;                  // Peak memory usage in bytes
    size_t iterations;                    // Number of iterations
    double opsPerSecond;                  // Operations per second
    std::string error;                    // Error message if any
    
    BenchmarkResult(const std::string& name, size_t iterations = 1)
        : name(name), elapsed(0), user(0), system(0), memoryUsage(0), 
          iterations(iterations), opsPerSecond(0.0), error() {}
    
    double getTimeMs() const {
        return elapsed.count() * 1000.0;
    }
    
    double getTimeUs() const {
        return elapsed.count() * 1000000.0;
    }
    
    std::string toString() const {
        std::ostringstream oss;
        oss << "Benchmark: " << name << "\n";
        oss << "  Time: " << std::fixed << std::setprecision(3) << getTimeMs() << " ms\n";
        oss << "  Iterations: " << iterations << "\n";
        oss << "  Average per iteration: " << std::fixed << std::setprecision(6) << elapsed.count() / iterations * 1000.0 << " ms\n";
        oss << "  OPS/sec: " << std::fixed << std::setprecision(0) << opsPerSecond << "\n";
        if (!error.empty()) {
            oss << "  Error: " << error << "\n";
        }
        return oss.str();
    }
};

// Performance counter for measuring time
class PerformanceCounter {
public:
    using Clock = std::chrono::high_resolution_clock;
    using TimePoint = Clock::time_point;
    
    PerformanceCounter() : startTime(Clock::now()), pausedTime(0), isRunning(true) {}
    
    void start() {
        if (!isRunning) {
            startTime = Clock::now() - pausedTime;
            isRunning = true;
        }
    }
    
    void stop() {
        if (isRunning) {
            pausedTime = Clock::now() - startTime;
            isRunning = false;
        }
    }
    
    void reset() {
        startTime = Clock::now();
        pausedTime = std::chrono::duration<double>::zero();
        isRunning = true;
    }
    
    double elapsed() const {
        if (isRunning) {
            return (Clock::now() - startTime).count();
        } else {
            return pausedTime.count();
        }
    }
    
    double elapsedMs() const {
        return elapsed() * 1000.0;
    }
    
    double elapsedUs() const {
        return elapsed() * 1000000.0;
    }
    
private:
    TimePoint startTime;
    std::chrono::duration<double> pausedTime;
    bool isRunning;
};

// Memory usage tracking (platform-specific)
class MemoryTracker {
public:
    virtual ~MemoryTracker() = default;
    virtual uint64_t getPeakMemoryUsage() const = 0;
    virtual uint64_t getCurrentMemoryUsage() const = 0;
};

#ifdef __linux__
class LinuxMemoryTracker : public MemoryTracker {
public:
    uint64_t getPeakMemoryUsage() const override {
        std::ifstream file("/proc/self/status");
        std::string line;
        uint64_t peak = 0;
        while (std::getline(file, line)) {
            if (line.find("VmPeak:") == 0) {
                size_t pos = line.find(':') + 1;
                std::string value = line.substr(pos);
                value.erase(0, value.find_first_not_of(" \t"));
                try {
                    peak = static_cast<uint64_t>(std::stol(value));
                } catch (...) {
                    peak = 0;
                }
                break;
            }
        }
        return peak * 1024; // Convert KB to bytes
    }
    
    uint64_t getCurrentMemoryUsage() const override {
        std::ifstream file("/proc/self/status");
        std::string line;
        uint64_t current = 0;
        while (std::getline(file, line)) {
            if (line.find("VmHWM:") == 0) {
                size_t pos = line.find(':') + 1;
                std::string value = line.substr(pos);
                value.erase(0, value.find_first_not_of(" \t"));
                try {
                    current = static_cast<uint64_t>(std::stol(value));
                } catch (...) {
                    current = 0;
                }
                break;
            }
        }
        return current * 1024; // Convert KB to bytes
    }
};
#endif

// Benchmark suite for running multiple benchmarks
class BenchmarkSuite {
public:
    BenchmarkSuite() = default;
    
    void addBenchmark(const std::string& name, std::function<void()> benchmark) {
        benchmarks[name] = benchmark;
    }
    
    BenchmarkResult runBenchmark(const std::string& name, size_t iterations = 1) {
        if (benchmarks.find(name) == benchmarks.end()) {
            return BenchmarkResult(name + " (not found)", iterations);
        }
        
        BenchmarkResult result(name, iterations);
        auto& tracker = getMemoryTracker();
        
        for (size_t i = 0; i < iterations; ++i) {
            // Reset memory tracking
            tracker.reset();
            
            // Time the benchmark
            PerformanceCounter counter;
            try {
                benchmarks[name]();
                counter.stop();
                
                // Update result
                result.elapsed += counter.elapsed();
                result.memoryUsage = std::max(result.memoryUsage, tracker.getPeakMemoryUsage());
                
            } catch (const std::exception& e) {
                result.error = e.what();
                break;
            }
        }
        
        if (result.elapsed.count() > 0) {
            result.opsPerSecond = static_cast<double>(1000.0 * iterations) / result.elapsed.count();
        }
        
        return result;
    }
    
    std::vector<BenchmarkResult> runAllBenchmarks(size_t iterations = 1) {
        std::vector<BenchmarkResult> results;
        for (const auto& pair : benchmarks) {
            results.push_back(runBenchmark(pair.first, iterations));
        }
        return results;
    }
    
    void printResults(const std::vector<BenchmarkResult>& results) const {
        std::cout << "=== NLM Performance Benchmarks ===" << std::endl;
        std::cout << std::fixed << std::setprecision(3);
        
        for (const auto& result : results) {
            std::cout << result.toString() << std::endl;
        }
    }
    
    std::string resultsToString(const std::vector<BenchmarkResult>& results) const {
        std::ostringstream oss;
        oss << "=== NLM Performance Benchmarks ===" << std::endl;
        oss << std::fixed << std::setprecision(3);
        
        for (const auto& result : results) {
            oss << result.toString() << std::endl;
        }
        return oss.str();
    }
    
    static MemoryTracker& getMemoryTracker() {
        static LinuxMemoryTracker tracker;
        return tracker;
    }
    
    static void resetMemoryTracker() {
        // Reset is no-op for current implementation
    }

private:
    std::map<std::string, std::function<void()>> benchmarks;
};

// Neural simulation specific benchmarks
namespace neural_benchmarks {
    
    // Benchmark neuron state updates
    inline void neuronStateUpdateBenchmark() {
        std::cout << "Benchmark: Neuron state update" << std::endl;
        // This would be implemented with actual neuron simulation
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
    
    // Benchmark synaptic connections
    inline void synapticConnectionBenchmark() {
        std::cout << "Benchmark: Synaptic connections" << std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(5));
    }
    
    // Benchmark memory operations
    inline void memoryBenchmark() {
        std::cout << "Benchmark: Memory operations" << std::endl;
        std::vector<double> data(1000000, 1.0);
        double sum = std::accumulate(data.begin(), data.end(), 0.0);
        std::cout << "Sum: " << sum << std::endl;
    }
    
    // Benchmark spike propagation
    inline void spikePropagationBenchmark() {
        std::cout << "Benchmark: Spike propagation" << std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(8));
    }
    
    // Benchmark plasticity computation
    inline void plasticityBenchmark() {
        std::cout << "Benchmark: Plasticity computation" << std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(12));
    }
}

} // namespace nlm
