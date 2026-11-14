// NLM Performance Benchmarking System
// Comprehensive performance measurement, metrics collection, and analysis

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <chrono>
#include <thread>
#include <atomic>
#include <mutex>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <numeric>
#include <random>
#include <functional>
#include <unordered_map>
#include <memory>

// Forward declarations for NLM components
namespace nlm {
    class Brain;
    class Config;
    class Logger;
}

using namespace std;
using namespace chrono;

// Performance measurement types
enum class BenchmarkMetric {
    ExecutionTime,
    MemoryUsage,
    CPUUtilization,
    SpikeCount,
    NeuronCount,
    SynapseCount,
    FiringRate,
    LearningSpeed,
    ConvergenceRate,
    EnergyConsumption,
    NetworkDiameter,
    ClusteringCoefficient,
    BetweennessCentrality,
    CustomMetric
};

// Benchmark configuration
struct BenchmarkConfig {
    string name;
    string description;
    double duration; // seconds
    int warmupSteps; // steps to skip for warm-up
    int minSteps; // minimum number of steps to run
    int maxSteps; // maximum number of steps to run
    bool parallel; // enable parallel execution
    int threadCount; // number of threads for parallel execution
    map<string, string> customSettings;
    vector<BenchmarkMetric> metrics; // metrics to measure
    string outputFormat; // "json", "csv", "text", "xml"
    string outputFile; // output file path
    bool verbose; // verbose output
    bool saveIntermediate; // save intermediate results
    double targetFps; // target frames per second
    string measurementMode; // "steps", "time", "event"
    
    BenchmarkConfig() : duration(60.0), warmupSteps(100), minSteps(1000), maxSteps(100000),
                       parallel(false), threadCount(1), verbose(true), saveIntermediate(false),
                       targetFps(30.0), measurementMode("steps") {}
};

// Benchmark result structure
struct BenchmarkResult {
    string benchmarkName;
    string benchmarkDescription;
    double executionTime; // seconds
    size_t stepsExecuted;
    double stepsPerSecond;
    double memoryUsageMB; // peak memory usage
    double cpuUtilization; // average CPU utilization (0-1)
    map<string, double> metrics; // custom metric name -> value
    vector<double> metricHistory; // history of metric values
    map<string, string> metadata; // additional metadata
    double wallClockTime; // actual wall clock time
    double cpuTime; // CPU time used
    vector<pair<double, map<string, double>>> stepResults; // step-by-step results
    
    BenchmarkResult() : executionTime(0.0), stepsExecuted(0), stepsPerSecond(0.0),
                       memoryUsageMB(0.0), cpuUtilization(0.0) {}
};

// Performance profiler
class PerformanceProfiler {
private:
    mutex profilingMutex;
    atomic<double> currentMemoryUsage;
    atomic<double> peakMemoryUsage;
    atomic<double> cpuUsage;
    steady_clock::time_point sessionStart;
    map<string, steady_clock::time_point> timerMap;

public:
    PerformanceProfiler() {
        sessionStart = steady_clock::now();
        currentMemoryUsage = 0.0;
        peakMemoryUsage = 0.0;
        cpuUsage = 0.0;
    }

    void startTimer(const string& name) {
        lock_guard<mutex> lock(profilingMutex);
        timerMap[name] = steady_clock::now();
    }

    double stopTimer(const string& name) {
        lock_guard<mutex> lock(profilingMutex);
        auto it = timerMap.find(name);
        if (it == timerMap.end()) {
            return 0.0;
        }
        
        auto start = it->second;
        auto end = steady_clock::now();
        auto duration = duration_cast<microseconds>(end - start).count();
        
        timerMap.erase(it);
        return duration / 1000000.0; // convert to seconds
    }

    void setMemoryUsage(double memoryMB) {
        currentMemoryUsage = memoryMB;
        if (memoryMB > peakMemoryUsage) {
            peakMemoryUsage = memoryMB;
        }
    }

    void setCPUUsage(double usage) {
        cpuUsage = usage;
    }

    double getPeakMemoryUsage() const { return peakMemoryUsage; }
    double getCurrentMemoryUsage() const { return currentMemoryUsage; }
    double getCPUUsage() const { return cpuUsage; }
    double getSessionDuration() const {
        return duration_cast<seconds>(steady_clock::now() - sessionStart).count();
    }

    map<string, double> getAllMeasurements() const {
        map<string, double> measurements;
        for (const auto& timer : timerMap) {
            measurements[timer.first] = stopTimer(timer.first);
        }
        measurements["memory_peak"] = peakMemoryUsage;
        measurements["memory_current"] = currentMemoryUsage;
        measurements["cpu_usage"] = cpuUsage;
        measurements["session_duration"] = getSessionDuration();
        return measurements;
    }

    void reset() {
        lock_guard<mutex> lock(profilingMutex);
        timerMap.clear();
        sessionStart = steady_clock::now();
        currentMemoryUsage = 0.0;
        peakMemoryUsage = 0.0;
        cpuUsage = 0.0;
    }
};

// Benchmark runner
class BenchmarkRunner {
private:
    BenchmarkConfig config;
    PerformanceProfiler profiler;
    vector<BenchmarkResult> results;
    mutex resultsMutex;
    function<void(const BenchmarkResult&)> resultCallback;

public:
    BenchmarkRunner(const BenchmarkConfig& cfg) : config(cfg) {}

    void setResultCallback(function<void(const BenchmarkResult&)> callback) {
        resultCallback = callback;
    }

    BenchmarkResult runBenchmark() {
        BenchmarkResult result;
        result.benchmarkName = config.name;
        result.benchmarkDescription = config.description;
        
        profiler.startTimer("total");
        
        try {
            if (config.verbose) {
                cout << "Starting benchmark: " << config.name << endl;
                cout << "Description: " << config.description << endl;
                cout << "Duration: " << config.duration << " seconds" << endl;
                cout << "Warmup steps: " << config.warmupSteps << endl;
                cout << "Min steps: " << config.minSteps << endl;
                cout << "Max steps: " << config.maxSteps << endl;
            }
            
            // Run warm-up phase
            runWarmupPhase(result);
            
            // Run main benchmark phase
            runBenchmarkPhase(result);
            
            // Calculate results
            calculateResults(result);
            
        } catch (const exception& e) {
            cerr << "Benchmark failed with error: " << e.what() << endl;
            result.metadata["error"] = e.what();
        }
        
        profiler.stopTimer("total");
        
        // Store result
        {
            lock_guard<mutex> lock(resultsMutex);
            results.push_back(result);
        }
        
        // Call callback if set
        if (resultCallback) {
            resultCallback(result);
        }
        
        // Save results
        saveResults(result);
        
        return result;
    }

private:
    void runWarmupPhase(BenchmarkResult& result) {
        if (config.verbose) {
            cout << "Running warm-up phase..." << endl;
        }
        
        profiler.startTimer("warmup");
        
        for (int step = 0; step < config.warmupSteps; ++step) {
            runBenchmarkStep(step, result, true);
            
            // Check if we should stop early
            if (config.measurementMode == "time" && 
                profiler.getSessionDuration() > config.duration) {
                break;
            }
            
            if (config.measurementMode == "event" && 
                shouldStopByEvent(result)) {
                break;
            }
        }
        
        double warmupTime = profiler.stopTimer("warmup");
        if (config.verbose) {
            cout << "Warm-up completed in " << warmupTime << " seconds" << endl;
        }
    }

    void runBenchmarkPhase(BenchmarkResult& result) {
        if (config.verbose) {
            cout << "Running benchmark phase..." << endl;
        }
        
        int stepsToRun = config.maxSteps;
        if (config.measurementMode == "time") {
            double remainingTime = config.duration - profiler.getSessionDuration();
            if (remainingTime > 0) {
                stepsToRun = min(config.maxSteps, static_cast<int>(remainingTime * config.targetFps));
            }
        }
        
        stepsToRun = max(config.minSteps, min(stepsToRun, config.maxSteps));
        
        profiler.startTimer("benchmark");
        
        for (int step = 0; step < stepsToRun; ++step) {
            runBenchmarkStep(step, result, false);
            
            // Check stopping conditions
            if (config.measurementMode == "time" && 
                profiler.getSessionDuration() >= config.duration) {
                break;
            }
            
            if (config.measurementMode == "event" && 
                shouldStopByEvent(result)) {
                break;
            }
            
            if (config.maxSteps > 0 && step >= config.maxSteps) {
                break;
            }
        }
        
        double benchmarkTime = profiler.stopTimer("benchmark");
        if (config.verbose) {
            cout << "Benchmark completed in " << benchmarkTime << " seconds" << endl;
            cout << "Steps executed: " << result.stepsExecuted << endl;
        }
    }

    void runBenchmarkStep(int step, BenchmarkResult& result, bool isWarmup) {
        profiler.startTimer("step");
        
        // Simulate benchmark work
        // In a real implementation, this would run actual NLM computations
        simulateComputation(step, result);
        
        double stepTime = profiler.stopTimer("step");
        
        result.stepsExecuted++;
        
        // Record step-specific metrics
        map<string, double> stepMetrics;
        stepMetrics["step_time"] = stepTime;
        stepMetrics["step_number"] = static_cast<double>(step);
        stepMetrics["cumulative_steps"] = static_cast<double>(result.stepsExecuted);
        stepMetrics["steps_per_second"] = 1.0 / stepTime;
        
        result.stepResults.push_back(make_pair(stepTime, stepMetrics));
        
        if (!isWarmup) {
            result.metricHistory.push_back(measureCustomMetric("general"));
        }
        
        // Save intermediate results if requested
        if (config.saveIntermediate && step % 100 == 0) {
            saveIntermediateResults(result);
        }
    }

    void simulateComputation(int step, BenchmarkResult& result) {
        // Simulate various types of NLM computations
        // This is a simplified simulation - real implementation would use actual NLM components
        
        double computationLoad = 1.0 + (step % 10) * 0.1; // Varying load
        double memoryUsage = 10.0 + computationLoad * 5.0; // Memory scales with load
        
        this_thread::sleep_for(milliseconds(static_cast<int>(computationLoad * 10)));
        
        profiler.setMemoryUsage(memoryUsage);
        profiler.setCPUUsage(computationLoad * 0.8);
        
        // Update metrics
        result.metrics["spike_count"] += step % 50;
        result.metrics["neuron_count"] = 1000;
        result.metrics["synapse_count"] = 50000;
        result.metrics["firing_rate"] = result.metrics["spike_count"] / max(1.0, result.stepsExecuted);
    }

    double measureCustomMetric(const string& metricName) {
        // Simulate measurement of custom metrics
        static mt19937 rng(42);
        static uniform_real_distribution<double> dist(0.0, 100.0);
        
        if (metricName == "general") {
            return dist(rng);
        } else if (metricName == "stability") {
            return 1.0 - dist(rng) * 0.1; // Stability between 0.9 and 1.0
        } else if (metricName == "learning") {
            return dist(rng) * 0.5; // Learning rate between 0 and 0.5
        }
        
        return dist(rng);
    }

    bool shouldStopByEvent(const BenchmarkResult& result) {
        // Check if benchmark should stop based on certain events
        // For example, if the network has converged or reached a plateau
        
        if (result.stepsExecuted < 100) return false;
        
        // Check for convergence (metric history stabilized)
        if (result.metricHistory.size() >= 20) {
            auto recentBegin = result.metricHistory.end() - 20;
            auto recentEnd = result.metricHistory.end();
            
            double avgRecent = accumulate(recentBegin, recentEnd, 0.0) / 20.0;
            
            double variance = 0.0;
            for (double val : vector<double>(recentBegin, recentEnd)) {
                variance += (val - avgRecent) * (val - avgRecent);
            }
            variance /= 20.0;
            
            if (sqrt(variance) < 0.01) { // Low variance indicates convergence
                return true;
            }
        }
        
        return false;
    }

    void calculateResults(BenchmarkResult& result) {
        result.executionTime = profiler.stopTimer("total");
        
        if (result.executionTime > 0) {
            result.stepsPerSecond = static_cast<double>(result.stepsExecuted) / result.executionTime;
        }
        
        result.wallClockTime = profiler.getSessionDuration();
        result.memoryUsageMB = profiler.getPeakMemoryUsage();
        result.cpuUtilization = profiler.getCPUUsage();
        
        // Calculate additional metrics
        if (!result.stepResults.empty()) {
            double totalStepTime = 0.0;
            for (const auto& stepResult : result.stepResults) {
                totalStepTime += stepResult.first;
            }
            
            result.cpuTime = totalStepTime;
        }
    }

    void saveResults(const BenchmarkResult& result) {
        if (config.outputFile.empty()) return;
        
        if (config.outputFormat == "json") {
            saveAsJson(result, config.outputFile);
        } else if (config.outputFormat == "csv") {
            saveAsCsv(result, config.outputFile);
        } else if (config.outputFormat == "text") {
            saveAsText(result, config.outputFile);
        }
        
        if (config.verbose) {
            cout << "Results saved to: " << config.outputFile << endl;
        }
    }

    void saveAsJson(const BenchmarkResult& result, const string& filepath) {
        ofstream file(filepath);
        if (!file.is_open()) {
            cerr << "Failed to open output file: " << filepath << endl;
            return;
        }
        
        // Simplified JSON output
        file << "{\n";
        file << "  \"benchmark_name\": \"" << result.benchmarkName << "\",\n";
        file << "  \"execution_time\": " << result.executionTime << ",\n";
        file << "  \"steps_executed\": " << result.stepsExecuted << ",\n";
        file << "  \"steps_per_second\": " << result.stepsPerSecond << ",\n";
        file << "  \"memory_usage_mb\": " << result.memoryUsageMB << ",\n";
        file << "  \"cpu_utilization\": " << result.cpuUtilization << ",\n";
        file << "  \"metrics\": {\n";
        
        bool firstMetric = true;
        for (const auto& metric : result.metrics) {
            if (!first) file << ",\n";
            file << "    \"" << metric.first << "\": " << metric.second;
            first = false;
        }
        
        file << "\n  }\n";
        file << "}\n";
    }

    void saveAsCsv(const BenchmarkResult& result, const string& filepath) {
        ofstream file(filepath);
        if (!file.is_open()) {
            cerr << "Failed to open output file: " << filepath << endl;
            return;
        }
        
        file << "metric,value" << endl;
        file << "benchmark_name," << result.benchmarkName << endl;
        file << "execution_time," << result.executionTime << endl;
        file << "steps_executed," << result.stepsExecuted << endl;
        file << "steps_per_second," << result.stepsPerSecond << endl;
        file << "memory_usage_mb," << result.memoryUsageMB << endl;
        file << "cpu_utilization," << result.cpuUtilization << endl;
        
        for (const auto& metric : result.metrics) {
            file << metric.first << "," << metric.second << endl;
        }
    }

    void saveAsText(const BenchmarkResult& result, const string& filepath) {
        ofstream file(filepath);
        if (!file.is_open()) {
            cerr << "Failed to open output file: " << filepath << endl;
            return;
        }
        
        file << "Benchmark Results:" << endl;
        file << "===================" << endl;
        file << "Benchmark: " << result.benchmarkName << endl;
        file << "Description: " << result.benchmarkDescription << endl;
        file << "" << endl;
        
        file << "Performance Metrics:" << endl;
        file << "  Execution time: " << result.executionTime << " seconds" << endl;
        file << "  Steps executed: " << result.stepsExecuted << endl;
        file << "  Steps per second: " << result.stepsPerSecond << endl;
        file << "  Memory usage: " << result.memoryUsageMB << " MB" << endl;
        file << "  CPU utilization: " << (result.cpuUtilization * 100) << "%" << endl;
        file << "" << endl;
        
        file << "Detailed Metrics:" << endl;
        for (const auto& metric : result.metrics) {
            file << "  " << metric.first << ": " << metric.second << endl;
        }
    }

    void saveIntermediateResults(const BenchmarkResult& result) {
        // Implement intermediate results saving
        // This could save results to a temporary file or database
    }

public:
    const vector<BenchmarkResult>& getResults() const { return results; }
    const BenchmarkResult& getLastResult() const { return results.back(); }

    void printResults(const BenchmarkResult& result) const {
        cout << "=== Benchmark Results ===" << endl;
        cout << "Benchmark: " << result.benchmarkName << endl;
        cout << "Description: " << result.benchmarkDescription << endl;
        cout << "" << endl;
        
        cout << "Performance Summary:" << endl;
        cout << "  Execution time: " << fixed << setprecision(3) << result.executionTime << " seconds" << endl;
        cout << "  Steps executed: " << result.stepsExecuted << endl;
        cout << "  Steps per second: " << fixed << setprecision(2) << result.stepsPerSecond << endl;
        cout << "  Memory usage: " << fixed << setprecision(2) << result.memoryUsageMB << " MB" << endl;
        cout << "  CPU utilization: " << fixed << setprecision(2) << (result.cpuUtilization * 100) << "%" << endl;
        cout << "" << endl;
        
        if (!result.metrics.empty()) {
            cout << "Detailed Metrics:" << endl;
            for (const auto& metric : result.metrics) {
                cout << "  " << metric.first << ": " << fixed << setprecision(6) << metric.second << endl;
            }
        }
        
        if (!result.metricHistory.empty()) {
            double avgMetric = accumulate(result.metricHistory.begin(), result.metricHistory.end(), 0.0) / result.metricHistory.size();
            double minMetric = *min_element(result.metricHistory.begin(), result.metricHistory.end());
            double maxMetric = *max_element(result.metricHistory.begin(), result.metricHistory.end());
            
            cout << "" << endl;
            cout << "Metric Statistics:" << endl;
            cout << "  Average: " << fixed << setprecision(6) << avgMetric << endl;
            cout << "  Minimum: " << fixed << setprecision(6) << minMetric << endl;
            cout << "  Maximum: " << fixed << setprecision(6) << maxMetric << endl;
        }
    }
};

// Benchmark suite
class BenchmarkSuite {
private:
    vector<BenchmarkConfig> benchmarks;
    map<string, BenchmarkRunner*> runners;
    mutex suiteMutex;

public:
    BenchmarkSuite() {}

    void addBenchmark(const BenchmarkConfig& config) {
        lock_guard<mutex> lock(suiteMutex);
        benchmarks.push_back(config);
        
        // Create benchmark runner
        BenchmarkRunner* runner = new BenchmarkRunner(config);
        runners[config.name] = runner;
    }

    void removeBenchmark(const string& name) {
        lock_guard<mutex> lock(suiteMutex);
        auto it = runners.find(name);
        if (it != runners.end()) {
            delete it->second;
            runners.erase(it);
        }
        
        // Also remove from benchmarks list
        benchmarks.erase(remove_if(benchmarks.begin(), benchmarks.end(),
                                 [name](const BenchmarkConfig& cfg) { return cfg.name == name; }),
                        benchmarks.end());
    }

    BenchmarkResult runBenchmark(const string& name) {
        lock_guard<mutex> lock(suiteMutex);
        
        auto it = runners.find(name);
        if (it == runners.end()) {
            throw runtime_error("Benchmark not found: " + name);
        }
        
        return it->second->runBenchmark();
    }

    void runAllBenchmarks() {
        for (const auto& benchmark : benchmarks) {
            if (config.verbose) {
                cout << "\nRunning benchmark: " << benchmark.name << endl;
                cout << "Description: " << benchmark.description << endl;
            }
            
            runBenchmark(benchmark.name);
            
            if (config.verbose) {
                cout << "Benchmark completed: " << benchmark.name << endl;
            }
        }
    }

    vector<BenchmarkResult> getAllResults() const {
        vector<BenchmarkResult> allResults;
        lock_guard<mutex> lock(suiteMutex);
        
        for (const auto& runner : runners) {
            const vector<BenchmarkResult>& results = runner.second->getResults();
            allResults.insert(allResults.end(), results.begin(), results.end());
        }
        
        return allResults;
    }

    void printSummary() const {
        vector<BenchmarkResult> allResults = getAllResults();n
        if (allResults.empty()) {
            cout << "No benchmark results available." << endl;
            return;
        }
        
        cout << "=== Benchmark Suite Summary ===" << endl;
        cout << "Total benchmarks run: " << allResults.size() << endl;
        cout << "" << endl;
        
        double totalTime = 0.0;
        double totalSteps = 0.0;
        double totalMemory = 0.0;
        
        for (const auto& result : allResults) {
            cout << result.benchmarkName << ": " << fixed << setprecision(3) << result.executionTime 
                 << " seconds, " << result.stepsExecuted << " steps" << endl;
            
            totalTime += result.executionTime;
            totalSteps += result.stepsExecuted;
            totalMemory += result.memoryUsageMB;
        }
        
        cout << "" << endl;
        cout << "Overall Performance:" << endl;
        cout << "  Total execution time: " << fixed << setprecision(3) << totalTime << " seconds" << endl;
        cout << "  Total steps executed: " << totalSteps << endl;
        cout << "  Average steps per second: " << fixed << setprecision(2) << (totalSteps / totalTime) << endl;
        cout << "  Average memory usage: " << fixed << setprecision(2) << (totalMemory / allResults.size()) << " MB" << endl;
    }

    void clearResults() {
        lock_guard<mutex> lock(suiteMutex);
        
        for (auto& runner : runners) {
            // Clear results in each runner
            // This would require adding a clearResults method to BenchmarkRunner
        }
        
        results.clear();
    }

    void saveResults(const string& filepath) const {
        vector<BenchmarkResult> allResults = getAllResults();
        
        if (allResults.empty()) {
            cout << "No results to save." << endl;
            return;
        }
        
        ofstream file(filepath);
        if (!file.is_open()) {
            cerr << "Failed to open output file: " << filepath << endl;
            return;
        }
        
        file << "NLM Benchmark Suite Results" << endl;
        file << "=============================" << endl;
        file << "Generated: " << getCurrentTimestamp() << endl;
        file << "" << endl;
        
        for (const auto& result : allResults) {
            file << "Benchmark: " << result.benchmarkName << endl;
            file << "Description: " << result.benchmarkDescription << endl;
            file << "Execution time: " << fixed << setprecision(3) << result.executionTime << " seconds" << endl;
            file << "Steps executed: " << result.stepsExecuted << endl;
            file << "Steps per second: " << fixed << setprecision(2) << result.stepsPerSecond << endl;
            file << "Memory usage: " << fixed << setprecision(2) << result.memoryUsageMB << " MB" << endl;
            file << "CPU utilization: " << fixed << setprecision(2) << (result.cpuUtilization * 100) << "%" << endl;
            file << "" << endl;
        }
    }

    static string getCurrentTimestamp() {
        auto now = system_clock::now();
        auto in_time_t = system_clock::to_time_t(now);
        
        stringstream ss;
        ss << put_time(localtime(&in_time_t), "%Y-%m-%d %H:%M:%S");
        return ss.str();
    }
};

// Convenience functions for common benchmark configurations
inline BenchmarkConfig createQuickBenchmark(const string& name, const string& description = "Quick benchmark") {
    BenchmarkConfig config;
    config.name = name;
    config.description = description;
    config.duration = 30.0;
    config.warmupSteps = 50;
    config.minSteps = 500;
    config.maxSteps = 5000;
    config.verbose = true;
    config.outputFormat = "text";
    return config;
}

inline BenchmarkConfig createThoroughBenchmark(const string& name, const string& description = "Thorough benchmark") {
    BenchmarkConfig config;
    config.name = name;
    config.description = description;
    config.duration = 300.0;
    config.warmupSteps = 500;
    config.minSteps = 5000;
    config.maxSteps = 50000;
    config.verbose = true;
    config.outputFormat = "json";
    return config;
}

inline BenchmarkConfig createLearningBenchmark(const string& name, const string& description = "Learning benchmark") {
    BenchmarkConfig config;
    config.name = name;
    config.description = description;
    config.duration = 120.0;
    config.warmupSteps = 200;
    config.minSteps = 2000;
    config.maxSteps = 20000;
    config.verbose = true;
    config.outputFormat = "csv";
    return config;
}

inline BenchmarkConfig createStabilityBenchmark(const string& name, const string& description = "Stability benchmark") {
    BenchmarkConfig config;
    config.name = name;
    config.description = description;
    config.duration = 180.0;
    config.warmupSteps = 300;
    config.minSteps = 3000;
    config.maxSteps = 30000;
    config.verbose = true;
    config.outputFormat = "text";
    return config;
}

// Benchmark utilities
namespace BenchmarkUtils {
    
    // Parse benchmark configuration from command line
    inline BenchmarkConfig parseBenchmarkConfig(int argc, char* argv[]) {
        BenchmarkConfig config;
        
        for (int i = 1; i < argc; ++i) {
            string arg = argv[i];
            
            if (arg == "--quick") {
                config = createQuickBenchmark("Quick Benchmark");
            } else if (arg == "--thorough") {
                config = createThoroughBenchmark("Thorough Benchmark");
            } else if (arg == "--learning") {
                config = createLearningBenchmark("Learning Benchmark");
            } else if (arg == "--stability") {
                config = createStabilityBenchmark("Stability Benchmark");
            } else if (arg.find("--name=") == 0) {
                config.name = arg.substr(7);
            } else if (arg.find("--duration=") == 0) {
                config.duration = stod(arg.substr(10));
            } else if (arg.find("--steps=") == 0) {
                config.maxSteps = stoi(arg.substr(8));
            } else if (arg.find("--output=") == 0) {
                config.outputFile = arg.substr(9);
            } else if (arg.find("--output-format=") == 0) {
                config.outputFormat = arg.substr(17);
            } else if (arg == "--help" || arg == "-h") {
                printBenchmarkHelp();
                exit(0);
            }
        }
        
        return config;
    }
    
    // Print benchmark help
    inline void printBenchmarkHelp() {
        cout << "=== NLM Benchmark Suite ===" << endl;
        cout << "" << endl;
        cout << "Usage: nlmbenchmark [OPTIONS]" << endl;
        cout << "" << endl;
        cout << "OPTIONS:" << endl;
        cout << "  --quick           Run quick benchmark (default)" << endl;
        cout << "  --thorough        Run thorough benchmark" << endl;
        cout << "  --learning        Run learning benchmark" << endl;
        cout << "  --stability       Run stability benchmark" << endl;
        cout << "  --name=NAME      Set benchmark name" << endl;
        cout << "  --duration=SECONDS  Set benchmark duration" << endl;
        cout << "  --steps=NUMBER   Set maximum number of steps" << endl;
        cout << "  --output=FILE    Set output file" << endl;
        cout << "  --output-format=FORMAT  Set output format (json, csv, text)" << endl;
        cout << "  --help, -h       Show this help message" << endl;
        cout << "" << endl;
        cout << "Examples:" << endl;
        cout << "  nlmbenchmark --quick" << endl;
        cout << "  nlmbenchmark --thorough --output=benchmark_results.json" << endl;
        cout << "  nlmbenchmark --learning --name=plasticity_test" << endl;
        cout << "" << endl;
    }
}

} // namespace nlm

// Example usage
#ifdef NLM_BENCHMARK_EXAMPLES
void benchmarkDemonstration() {
    // Example 1: Run quick benchmark
    BenchmarkConfig quickBenchmark = createQuickBenchmark("Quick Performance Test");
    BenchmarkRunner quickRunner(quickBenchmark);
    BenchmarkResult quickResult = quickRunner.runBenchmark();
    
    // Example 2: Create thorough benchmark
    BenchmarkConfig thoroughBenchmark = createThoroughBenchmark("Comprehensive Analysis");
    BenchmarkRunner thoroughRunner(thoroughBenchmark);
    BenchmarkResult thoroughResult = thoroughRunner.runBenchmark();
    
    // Example 3: Create benchmark suite
    BenchmarkSuite suite;
    suite.addBenchmark(createQuickBenchmark("Quick Test"));
    suite.addBenchmark(createThoroughBenchmark("Thorough Test"));
    suite.addBenchmark(createLearningBenchmark("Learning Test"));
    suite.addBenchmark(createStabilityBenchmark("Stability Test"));
    
    // Run all benchmarks
    suite.runAllBenchmarks();
    
    // Print summary
    suite.printSummary();
    
    // Save results
    suite.saveResults("benchmark_results.txt");
}
#endif // NLM_BENCHMARK_EXAMPLES

```