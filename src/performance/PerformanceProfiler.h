// Performance profiling for NLM
// Provides timing and performance measurement capabilities

#include <chrono>
#include <string>
#include <vector>
#include <map>
#include <iomanip>
#include <sstream>
#include <algorithm>
#include <functional>

/**
 * Performance measurement utilities
 */
class PerformanceProfiler {
public:
    // Simple timer for measuring elapsed time
    class Timer {
    public:
        Timer() : startTime(std::chrono::high_resolution_clock::now()) {}
        
        // Get elapsed time in seconds as double
        double elapsed() const {
            auto endTime = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime);
            return duration.count() / 1000000.0; // Convert to seconds
        }
        
        // Reset timer
        void reset() {
            startTime = std::chrono::high_resolution_clock::now();
        }
        
        // Get elapsed time in microseconds
        long long elapsedMicroseconds() const {
            auto endTime = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime);
            return duration.count();
        }
        
        // Get elapsed time in milliseconds
        long long elapsedMilliseconds() const {
            auto endTime = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
            return duration.count();
        }
        
    private:
        std::chrono::time_point<std::chrono::high_resolution_clock> startTime;
    };
    
    // Scoped timer that automatically reports on destruction
    class ScopedTimer {
    public:
        ScopedTimer(const std::string& name, std::map<std::string, double>& results,
                   bool verbose = false, double threshold = 0.0)
            : name(name), results(results), verbose(verbose), threshold(threshold) {
            timer.start();
        }
        
        ~ScopedTimer() {
            double elapsed = timer.elapsed();
            results[name] = elapsed;
            
            if (verbose || elapsed >= threshold) {
                std::cout << "  " << name << ": " << std::fixed << std::setprecision(4)
                         << elapsed << "s" << std::endl;
            }
        }
        
        double elapsed() const {
            return timer.elapsed();
        }
        
    private:
        std::string name;
        std::map<std::string, double>& results;
        bool verbose;
        double threshold;
        Timer timer;
    };
    
    // Performance measurement results
    struct PerformanceResult {
        std::string name;
        double time;           // Total time in seconds
        double cpuTime;        // CPU time (if available)
        size_t memoryUsed;     // Memory used in bytes
        size_t calls;          // Number of function calls
        double callsPerSecond; // Calls per second
        
        PerformanceResult(const std::string& n, double t) 
            : name(n), time(t), cpuTime(0.0), memoryUsed(0), calls(0), callsPerSecond(0.0) {}
    };
    
    // Performance benchmark suite
    class BenchmarkSuite {
    public:
        BenchmarkSuite(const std::string& name) : suiteName(name) {}
        
        // Add a benchmark case
        void addBenchmark(const std::string& name, std::function<void()> benchmark,
                         size_t iterations = 1, double warmupTime = 0.5) {
            BenchmarkCase bench;
            bench.name = name;
            bench.benchmark = benchmark;
            bench.iterations = iterations;
            bench.warmupTime = warmupTime;
            
            benchmarks.push_back(bench);
        }
        
        // Run all benchmarks
        std::vector<PerformanceResult> runAll() {
            std::cout << "Running benchmark suite: " << suiteName << std::endl;
            std::cout << std::endl;
            
            std::vector<PerformanceResult> results;
            
            for (const auto& bench : benchmarks) {
                std::cout << "Benchmark: " << bench.name << " (" << bench.iterations << " iterations)" << std::endl;
                
                // Warmup
                if (bench.warmupTime > 0.0) {
                    std::cout << "  Warming up..." << std::endl;
                    Timer warmupTimer;
                    while (warmupTimer.elapsed() < bench.warmupTime) {
                        // Run a few iterations for warmup
                        for (size_t i = 0; i < 10 && warmupTimer.elapsed() < bench.warmupTime; ++i) {
                            bench.benchmark();
                        }
                    }
                }
                
                // Run benchmark
                Timer timer;
                for (size_t i = 0; i < bench.iterations; ++i) {
                    bench.benchmark();
                }
                
                double totalTime = timer.elapsed();
                double avgTime = totalTime / bench.iterations;
                double callsPerSec = bench.iterations / totalTime;
                
                PerformanceResult result(bench.name, avgTime);
                result.calls = bench.iterations;
                result.callsPerSecond = callsPerSec;
                
                results.push_back(result);
                
                std::cout << "  Average time: " << std::fixed << std::setprecision(4) << avgTime << "s" << std::endl;
                std::cout << "  Total time:   " << std::fixed << std::setprecision(4) << totalTime << "s" << std::endl;
                std::cout << "  Calls/sec:    " << std::fixed << std::setprecision(0) << callsPerSec << std::endl;
                std::cout << std::endl;
            }
            
            return results;
        }
        
        // Print benchmark results
        static void printResults(const std::vector<PerformanceResult>& results, const std::string& title = "Benchmark Results") {
            std::cout << title << std::endl;
            std::cout << std::string(title.length(), '=') << std::endl;
            std::cout << std::endl;
            
            std::cout << "Benchmark Name            | Average Time | Total Time | Calls/Sec | Iterations" << std::endl;
            std::cout << "--------------------------|--------------|------------|-----------|-----------" << std::endl;
            
            for (const auto& result : results) {
                std::cout << std::left << std::setw(26) << result.name
                         << " | " << std::right << std::setw(12) << std::fixed << std::setprecision(4) << result.time
                         << " | " << std::right << std::setw(10) << std::fixed << std::setprecision(4) << result.time * result.calls
                         << " | " << std::right << std::setw(9) << std::fixed << std::setprecision(0) << result.callsPerSecond
                         << " | " << std::right << std::setw(9) << result.calls << std::endl;
            }
            
            std::cout << std::endl;
        }
        
    private:
        std::string suiteName;
        std::vector<BenchmarkCase> benchmarks;
        
        struct BenchmarkCase {
            std::string name;
            std::function<void()> benchmark;
            size_t iterations;
            double warmupTime;
        };
    };
    
    // Performance monitoring for simulation steps
    class SimulationPerformanceMonitor {
    public:
        struct StepPerformance {
            size_t step;
            double elapsedTime;
            size_t spikesGenerated;
            size_t neuronsActive;
        };
        
        SimulationPerformanceMonitor(size_t totalSteps = 0)
            : totalSteps(totalSteps), currentStep(0), startTime(), totalSpikes(0) {}
        
        // Record step performance
        void recordStep(size_t step, double elapsedTime, size_t spikesGenerated, size_t neuronsActive = 0) {
            StepPerformance perf;
            perf.step = step;
            perf.elapsedTime = elapsedTime;
            perf.spikesGenerated = spikesGenerated;
            perf.neuronsActive = neuronsActive;
            
            stepPerformances.push_back(perf);
            totalSpikes += spikesGenerated;
            currentStep = step + 1;
        }
        
        // Get average time per step
        double getAverageStepTime() const {
            if (stepPerformances.empty()) return 0.0;
            
            double totalTime = 0.0;
            for (const auto& perf : stepPerformances) {
                totalTime += perf.elapsedTime;
            }
            
            return totalTime / stepPerformances.size();
        }
        
        // Get steps per second
        double getStepsPerSecond() const {
            if (totalTime <= 0.0) return 0.0;
            return stepPerformances.size() / totalTime;
        }
        
        // Get spikes per second
        double getSpikesPerSecond() const {
            if (totalTime <= 0.0) return 0.0;
            return static_cast<double>(totalSpikes) / totalTime;
        }
        
        // Get average spikes per step
        double getAverageSpikesPerStep() const {
            if (stepPerformances.empty()) return 0.0;
            return static_cast<double>(totalSpikes) / stepPerformances.size();
        }
        
        // Print performance summary
        void printPerformanceSummary() const {
            if (stepPerformances.empty()) {
                std::cout << "No performance data available." << std::endl;
                return;
            }
            
            std::cout << "=== Simulation Performance Summary ===" << std::endl;
            std::cout << std::endl;
            
            // Time metrics
            std::cout << "Time Performance:" << std::endl;
            std::cout << "  Total simulation time: " << std::fixed << std::setprecision(4) << totalTime << "s" << std::endl;
            std::cout << "  Average step time:     " << std::fixed << std::setprecision(4) << getAverageStepTime() << "s" << std::endl;
            std::cout << "  Steps per second:      " << std::fixed << std::setprecision(2) << getStepsPerSecond() << std::endl;
            std::cout << std::endl;
            
            // Activity metrics
            std::cout << "Activity Performance:" << std::endl;
            std::cout << "  Total spikes:           " << totalSpikes << std::endl;
            std::cout << "  Spikes per second:     " << std::fixed << std::setprecision(2) << getSpikesPerSecond() << std::endl;
            std::cout << "  Average spikes/step:    " << std::fixed << std::setprecision(2) << getAverageSpikesPerStep() << std::endl;
            std::cout << std::endl;
            
            // Network metrics
            if (!stepPerformances.empty()) {
                size_t maxActiveNeurons = 0;
                for (const auto& perf : stepPerformances) {
                    if (perf.neuronsActive > maxActiveNeurons) {
                        maxActiveNeurons = perf.neuronsActive;
                    }
                }
                std::cout << "Network Performance:" << std::endl;
                std::cout << "  Max neurons active:    " << maxActiveNeurons << std::endl;
                if (maxActiveNeurons > 0) {
                    std::cout << "  Avg neurons active:    " << std::fixed << std::setprecision(1)
                             << (getAverageStepTime() > 0.0 ? (totalSpikes * 10.0) / (totalTime * maxActiveNeurons) : 0.0) << std::endl;
                }
            }
            
            // Performance metrics
            std::cout << std::endl;
            std::cout << "Performance Characteristics:" << std::endl;
            std::cout << "  Performance efficiency: " << std::fixed << std::setprecision(2)
                     << (getAverageStepTime() > 0.0 ? (1.0 / getAverageStepTime()) : 0.0) << " steps/s" << std::endl;
        }
        
        // Get detailed step-by-step performance
        const std::vector<StepPerformance>& getStepPerformances() const {
            return stepPerformances;
        }
        
    private:
        size_t totalSteps;
        size_t currentStep;
        Timer startTime;
        double totalTime;
        size_t totalSpikes;
        std::vector<StepPerformance> stepPerformances;
    };
    
    // Memory usage monitoring
    class MemoryMonitor {
    public:
        // Get current memory usage in bytes
        static size_t getCurrentMemoryUsage() {
            // Platform-specific implementation would go here
            // For now, return 0 as placeholder
            return 0;
        }
        
        // Get peak memory usage
        static size_t getPeakMemoryUsage() {
            // Platform-specific implementation would go here
            // For now, return 0 as placeholder
            return 0;
        }
        
        // Memory profiling wrapper
        template<typename Func>
        static auto profileMemory(const std::string& name, Func&& func) {
            Timer timer;
            size_t beforeMemory = getCurrentMemoryUsage();
            
            auto result = func();
            
            size_t afterMemory = getCurrentMemoryUsage();
            double elapsed = timer.elapsed();
            
            std::cout << "Memory profile for " << name << ":" << std::endl;
            std::cout << "  Time:      " << std::fixed << std::setprecision(4) << elapsed << "s" << std::endl;
            std::cout << "  Memory:    " << (afterMemory - beforeMemory) << " bytes" << std::endl;
            std::cout << "  Time/Mem:  " << std::fixed << std::setprecision(2)
                     << (elapsed > 0.0 ? (afterMemory - beforeMemory) / elapsed : 0.0) << " bytes/s" << std::endl;
            std::cout << std::endl;
            
            return result;
        }
    };
    
    // Simple performance report generator
    class PerformanceReporter {
    public:
        // Generate a performance report
        static std::string generateReport(const std::string& title,
                                         const std::map<std::string, double>& metrics,
                                         const std::string& format = "text") {
            if (format == "text") {
                std::ostringstream oss;
                oss << title << std::endl;
                oss << std::string(title.length(), '=') << std::endl;
                oss << std::endl;
                
                for (const auto& pair : metrics) {
                    oss << std::left << std::setw(30) << pair.first
                        << ": " << std::fixed << std::setprecision(6) << pair.second << std::endl;
                }
                
                return oss.str();
            } else if (format == "json") {
                std::ostringstream oss;
                oss << "{\n  \"title\": \"" << title << "\"," << std::endl;
                oss << "  \"metrics\": {" << std::endl;
                
                bool first = true;
                for (const auto& pair : metrics) {
                    if (!first) oss << "," << std::endl;
                    oss << "    \"" << pair.first << "\": " << pair.second;
                    first = false;
                }
                
                oss << std::endl << "  }" << std::endl << "}";
                
                return oss.str();
            } else {
                return "Unknown format: " + format;
            }
        }
    };
};