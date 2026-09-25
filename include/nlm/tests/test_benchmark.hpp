// Test performance benchmark utilities
// Specialized benchmarking for neural network computations
#pragma once

#include <chrono>
#include <string>
#include <vector>
#include <memory>
#include <algorithm>
#include <numeric>
#include <functional>
#include <unordered_map>
#include <iomanip>
#include <sstream>
#include <cmath>
#include <mutex>
#include <atomic>
#include "test_utils.hpp"

namespace nlm {
namespace test {

// Performance measurement utilities for neural network components
class PerformanceProfiler {
public:
    struct BenchmarkMetrics {
        std::string operation;
        std::string component;
        std::chrono::nanoseconds totalTime;
        std::chrono::nanoseconds setupTime;
        std::chrono::nanoseconds teardownTime;
        size_t iterations;
        double opsPerSecond;
        double timePerOpMicroseconds;
        
        // Neural network specific metrics
        size_t neuronsProcessed;
        size_t synapsesActivated;
        double memoryBandwidthGBps;
        double arithmeticIntensity;
        
        // Statistical data
        double meanTime;
        double stdDevTime;
        double minTime;
        double maxTime;
        double p95Time;
        double p99Time;
        
        BenchmarkMetrics(const std::string& op, const std::string& comp) 
            : operation(op), component(comp), iterations(0), opsPerSecond(0.0), 
              timePerOpMicroseconds(0.0), neuronsProcessed(0), 
              synapsesActivated(0), memoryBandwidthGBps(0.0), arithmeticIntensity(0.0) {}
    };
    
    class BenchmarkSuite {
    private:
        std::vector<BenchmarkMetrics> benchmarks;
        std::vector<std::function<void()>> setupFunctions;
        std::vector<std::function<void()>> teardownFunctions;
        
    public:
        BenchmarkSuite() = default;
        
        void addSetup(std::function<void()> setup) {
            setupFunctions.push_back(setup);
        }
        
        void addTeardown(std::function<void()> teardown) {
            teardownFunctions.push_back(teardown);
        }
        
        template<typename Func>
        BenchmarkMetrics measure(const std::string& operation,
                                 const std::string& component,
                                 Func&& func,
                                 size_t iterations = 1000) {
            // Run setup functions
            for (const auto& setup : setupFunctions) {
                setup();
            }
            
            auto start = std::chrono::high_resolution_clock::now();
            
            for (size_t i = 0; i < iterations; ++i) {
                func();
            }
            
            auto end = std::chrono::high_resolution_clock::now();
            
            // Run teardown functions
            for (const auto& teardown : teardownFunctions) {
                teardown();
            }
            
            BenchmarkMetrics result(operation, component);
            auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start);
            
            result.iterations = iterations;
            result.totalTime = duration;
            result.opsPerSecond = (duration.count() > 0) ? 
                (iterations * 1.0e9) / duration.count() : 0.0;
            result.timePerOpMicroseconds = 
                (duration.count() > 0) ? (duration.count() * 1.0e-6) / iterations : 0.0;
            
            result.meanTime = result.timePerOpMicroseconds;
            result.minTime = result.timePerOpMicroseconds;
            result.maxTime = result.timePerOpMicroseconds;
            result.stdDevTime = 0.0;
            
            benchmarks.push_back(result);
            return result;
        }
        
        void addBenchmark(const BenchmarkMetrics& benchmark) {
            benchmarks.push_back(benchmark);
        }
        
        std::vector<BenchmarkMetrics>& getBenchmarks() {
            return benchmarks;
        }
        
        const std::vector<BenchmarkMetrics>& getBenchmarks() const {
            return benchmarks;
        }
        
        void clear() {
            benchmarks.clear();
        }
    };
    
    // Specialized benchmarks for neural network components
    static BenchmarkSuite createNeuronBenchmarks() {
        BenchmarkSuite suite;
        
        // Add neuron-specific setup/teardown if needed
        suite.addSetup([]() { /* Neuron setup */ });
        suite.addTeardown([]() { /* Neuron teardown */ });
        
        return suite;
    }
    
    static BenchmarkSuite createSynapseBenchmarks() {
        BenchmarkSuite suite;
        
        // Add synapse-specific setup/teardown if needed
        suite.addSetup([]() { /* Synapse setup */ });
        suite.addTeardown([]() { /* Synapse teardown */ });
        
        return suite;
    }
    
    static BenchmarkSuite createBrainBenchmarks() {
        BenchmarkSuite suite;
        
        // Add brain-specific setup/teardown if needed
        suite.addSetup([]() { /* Brain setup */ });
        suite.addTeardown([]() { /* Brain teardown */ });
        
        return suite;
    }
    
    // Statistical analysis of benchmark results
    static struct BenchmarkAnalysis {
        double meanTime;
        double stdDevTime;
        double minTime;
        double maxTime;
        double p95Time;
        double p99Time;
        double cv; // Coefficient of variation
        size_t count;
    } analyzeBenchmarks(const std::vector<BenchmarkMetrics>& benchmarks) {
        BenchmarkAnalysis analysis;
        
        if (benchmarks.empty()) {
            return analysis;
        }
        
        std::vector<double> times;
        for (const auto& bench : benchmarks) {
            times.push_back(bench.timePerOpMicroseconds);
        }
        
        analysis.count = times.size();
        analysis.meanTime = std::accumulate(times.begin(), times.end(), 0.0) / analysis.count;
        
        double sumSquares = 0.0;
        double min = times[0];
        double max = times[0];
        
        for (double t : times) {
            sumSquares += (t - analysis.meanTime) * (t - analysis.meanTime);
            min = std::min(min, t);
            max = std::max(max, t);
        }
        
        analysis.stdDevTime = std::sqrt(sumSquares / analysis.count);
        analysis.minTime = min;
        analysis.maxTime = max;
        analysis.cv = (analysis.meanTime > 0) ? analysis.stdDevTime / analysis.meanTime : 0.0;
        
        std::sort(times.begin(), times.end());
        auto p95Index = static_cast<size_t>(0.95 * times.size());
        auto p99Index = static_cast<size_t>(0.99 * times.size());
        
        analysis.p95Time = (p95Index < times.size()) ? times[p95Index] : times.back();
        analysis.p99Time = (p99Index < times.size()) ? times[p99Index] : times.back();
        
        return analysis;
    }
    
    // Generate benchmark report
    static std::string generateBenchmarkReport(const BenchmarkSuite& suite) {
        std::stringstream ss;
        ss << "Neural Network Performance Benchmark Report" << std::endl;
        ss << "=========================================" << std::endl;
        ss << "Generated at: " << std::chrono::system_clock::now() << std::endl;
        ss << std::endl;
        
        for (const auto& benchmark : suite.getBenchmarks()) {
            ss << "Benchmark: " << benchmark.operation << " (" << benchmark.component << ")" << std::endl;
            ss << "  Iterations: " << benchmark.iterations << std::endl;
            ss << "  Total Time: " << std::chrono::duration_cast<std::chrono::milliseconds>(benchmark.totalTime).count() << "ms" << std::endl;
            ss << "  Ops/sec: " << std::fixed << std::setprecision(2) << benchmark.opsPerSecond << std::endl;
            ss << "  Time per Op: " << std::fixed << std::setprecision(6) << benchmark.timePerOpMicroseconds << "μs" << std::endl;
            ss << "  Mean Time: " << std::fixed << std::setprecision(6) << benchmark.meanTime << "μs" << std::endl;
            ss << "  Std Dev: " << std::fixed << std::setprecision(6) << benchmark.stdDevTime << "μs" << std::endl;
            ss << "  Min: " << std::fixed << std::setprecision(6) << benchmark.minTime << "μs" << std::endl;
            ss << "  Max: " << std::fixed << std::setprecision(6) << benchmark.maxTime << "μs" << std::endl;
            
            // Neural network specific metrics
            if (benchmark.neuronsProcessed > 0) {
                ss << "  Neurons Processed: " << benchmark.neuronsProcessed << std::endl;
                ss << "  Synapses Activated: " << benchmark.synapsesActivated << std::endl;
                ss << "  Memory Bandwidth: " << std::fixed << std::setprecision(2) << benchmark.memoryBandwidthGBps << "GB/s" << std::endl;
                ss << "  Arithmetic Intensity: " << std::fixed << std::setprecision(2) << benchmark.arithmeticIntensity << std::endl;
            }
            
            ss << std::endl;
        }
        
        return ss.str();
    }
};

// Timing utilities for stress and performance tests
class TestTimer {
public:
    struct TimeMeasurement {
        std::string testName;
        TestCategory category;
        std::chrono::nanoseconds elapsedTime;
        std::chrono::nanoseconds cpuTime;
        size_t memoryUsed;
        std::thread::id threadId;
        bool passed;
        std::string details;
    };
    
    class Timer {
    private:
        std::chrono::high_resolution_clock::time_point startTime;
        std::chrono::high_resolution_clock::time_point cpuStartTime;
        std::string name;
        
    public:
        Timer(const std::string& timerName = "Timer") : name(timerName) {
            start();
        }
        
        void start() {
            startTime = std::chrono::high_resolution_clock::now();
            cpuStartTime = std::chrono::high_resolution_clock::now();
        }
        
        TimeMeasurement stop() {
            auto end = std::chrono::high_resolution_clock::now();
            TimeMeasurement result;
            result.testName = name;
            result.elapsedTime = std::chrono::duration_cast<std::chrono::nanoseconds>(end - startTime);
            result.cpuTime = std::chrono::duration_cast<std::chrono::nanoseconds>(end - cpuStartTime);
            result.memoryUsed = getMemoryUsage(); // TODO: Implement
            result.threadId = std::this_thread::get_id();
            result.passed = true;
            return result;
        }
        
        std::chrono::nanoseconds elapsed() const {
            auto now = std::chrono::high_resolution_clock::now();
            return std::chrono::duration_cast<std::chrono::nanoseconds>(now - startTime);
        }
        
        double elapsedMilliseconds() const {
            return std::chrono::duration_cast<std::chrono::milliseconds>(elapsed()).count();
        }
    };
    
    // Average execution time over multiple runs
    template<typename Func>
    static TimeMeasurement averageTime(Func&& func, size_t iterations, const std::string& testName) {
        TimeMeasurement total;
        total.testName = testName;
        
        std::vector<std::chrono::nanoseconds> times;
        for (size_t i = 0; i < iterations; ++i) {
            Timer timer(testName + "_run_" + std::to_string(i));
            func();
            auto measurement = timer.stop();
            times.push_back(measurement.elapsedTime);
            
            if (i == 0) {
                total = measurement;
            } else {
                total.elapsedTime += measurement.elapsedTime;
                total.cpuTime += measurement.cpuTime;
                total.memoryUsed = std::max(total.memoryUsed, measurement.memoryUsed);
            }
        }
        
        if (iterations > 0) {
            total.elapsedTime = std::chrono::nanoseconds(total.elapsedTime.count() / iterations);
            total.cpuTime = std::chrono::nanoseconds(total.cpuTime.count() / iterations);
        }
        
        return total;
    }
    
    // Stress test timing
    template<typename Func>
    static std::vector<TimeMeasurement> stressTest(Func&& func, 
                                                   std::chrono::nanoseconds duration,
                                                   const std::string& testName) {
        std::vector<TimeMeasurement> measurements;
        auto startTime = std::chrono::system_clock::now();
        
        while (std::chrono::system_clock::now() - startTime < duration) {
            Timer timer(testName);
            func();
            measurements.push_back(timer.stop());
        }
        
        return measurements;
    }
    
    // Compare execution times
    static ComparisonResult comparePerformance(const std::vector<TimeMeasurement>& baseline,
                                               const std::vector<TimeMeasurement>& test,
                                               const std::string& testName = "") {
        ComparisonResult result;
        
        if (baseline.empty() || test.empty()) {
            result.passed = false;
            result.message = "Insufficient data for comparison";
            return result;
        }
        
        // Calculate average times
        double baselineAvg = 0.0, testAvg = 0.0;
        for (const auto& m : baseline) {
            baselineAvg += m.elapsedTime.count();
        }
        baselineAvg /= baseline.size();
        
        for (const auto& m : test) {
            testAvg += m.elapsedTime.count();
        }
        testAvg /= test.size();
        
        double improvement = ((baselineAvg - testAvg) / baselineAvg) * 100.0;
        
        // Check if performance is within acceptable range (within 5%)
        result.maxDifference = std::abs(baselineAvg - testAvg);
        result.relativeError = (baselineAvg > 1e-12) ? result.maxDifference / baselineAvg : 0.0;
        result.passed = result.relativeError <= 0.05; // 5% tolerance
        
        std::stringstream ss;
        ss << "Performance comparison for '" << testName << "'\n"
           << "Baseline avg: " << baselineAvg << "ns\n"
           << "Test avg: " << testAvg << "ns\n"
           << "Improvement: " << std::fixed << std::setprecision(2) << improvement << "%\n"
           << "Within tolerance: " << (result.passed ? "YES" : "NO") << " (5%)";
        
        result.message = ss.str();
        return result;
    }
    
private:
    static size_t getMemoryUsage() { return 0; } // TODO: Implement platform-specific memory measurement
};

}}} // namespace nlm::test
