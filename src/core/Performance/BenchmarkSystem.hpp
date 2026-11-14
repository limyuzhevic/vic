#pragma once

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
    ExecutionTime = 0,
    MemoryUsage = 1,
    CPUUtilization = 2,
    SpikeCount = 3,
    NeuronCount = 4,
    SynapseCount = 5,
    FiringRate = 6,
    LearningSpeed = 7,
    ConvergenceRate = 8,
    EnergyConsumption = 9,
    NetworkDiameter = 10,
    ClusteringCoefficient = 11,
    BetweennessCentrality = 12,
    CustomMetric = 13
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

// Benchmark runner interface
class BenchmarkRunnerInterface {
public:
    virtual ~BenchmarkRunnerInterface() = default;
    virtual BenchmarkResult runBenchmark() = 0;
    virtual void setVerbose(bool verbose) = 0;
    virtual const vector<BenchmarkResult>& getResults() const = 0;
    virtual void clearResults() = 0;
    virtual string getName() const = 0;
    virtual string getDescription() const = 0;
};

// Benchmark runner implementation
class BenchmarkRunner : public BenchmarkRunnerInterface {
private:
    BenchmarkConfig config;
    PerformanceProfiler profiler;
    vector<BenchmarkResult> results;
    mutex resultsMutex;
    function<void(const BenchmarkResult&)> resultCallback;

public:
    BenchmarkRunner(const BenchmarkConfig& cfg) : config(cfg) {}

    void setResultCallback(function<void(const BenchmarkResult&)> callback) override {
        resultCallback = callback;
    }

    void setVerbose(bool verbose) override {
        config.verbose = verbose;
    }

    const vector<BenchmarkResult>& getResults() const override {
        return results;
    }

    void clearResults() override {
        results.clear();
    }

    string getName() const override {
        return config.name;
    }

    string getDescription() const override {
        return config.description;
    }

    BenchmarkResult runBenchmark() override;

private:
    // Internal helper methods (implementation in BenchmarkSystem.cpp)
    void runWarmupPhase(BenchmarkResult& result);
    void runBenchmarkPhase(BenchmarkResult& result);
    void runBenchmarkStep(int step, BenchmarkResult& result, bool isWarmup);
    void simulateComputation(int step, BenchmarkResult& result);
    double measureCustomMetric(const string& metricName);
    bool shouldStopByEvent(const BenchmarkResult& result);
    void calculateResults(BenchmarkResult& result);
    void saveResults(const BenchmarkResult& result);
    void saveAsJson(const BenchmarkResult& result, const string& filepath);
    void saveAsCsv(const BenchmarkResult& result, const string& filepath);
    void saveAsText(const BenchmarkResult& result, const string& filepath);
    void saveIntermediateResults(const BenchmarkResult& result);

public:
    void printResults(const BenchmarkResult& result) const;
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

// Benchmark suite for managing multiple benchmarks
class BenchmarkSuite {
private:
    vector<BenchmarkConfig> benchmarks;
    vector<unique_ptr<BenchmarkRunnerInterface>> runners;
    mutex suiteMutex;

public:
    BenchmarkSuite() {}

    void addBenchmark(const BenchmarkConfig& config);
    void removeBenchmark(const string& name);
    BenchmarkResult runBenchmark(const string& name);
    void runAllBenchmarks();
    vector<BenchmarkResult> getAllResults() const;
    void printSummary() const;
    void saveResults(const string& filepath) const;
    void clearResults();

private:
    BenchmarkRunner* getRunner(const string& name);
    static string getCurrentTimestamp();
};

// Convenience functions for creating common benchmark configurations
namespace BenchmarkConfigFactory {
    BenchmarkConfig createQuickBenchmark(const string& name, const string& description = "Quick benchmark");
    BenchmarkConfig createThoroughBenchmark(const string& name, const string& description = "Thorough benchmark");
    BenchmarkConfig createLearningBenchmark(const string& name, const string& description = "Learning benchmark");
    BenchmarkConfig createStabilityBenchmark(const string& name, const string& description = "Stability benchmark");
}

// Command-line argument parsing
namespace BenchmarkCLI {
    BenchmarkConfig parseBenchmarkConfig(int argc, char* argv[]);
    void printBenchmarkHelp();
}

// Benchmarking utilities
namespace BenchmarkUtils {
    // Parse benchmark configuration from command line
    inline BenchmarkConfig parseBenchmarkConfig(int argc, char* argv[]) {
        BenchmarkConfig config;
        
        for (int i = 1; i < argc; ++i) {
            string arg = argv[i];
            
            if (arg == "--quick") {
                config = BenchmarkConfigFactory::createQuickBenchmark("Quick Benchmark");
            } else if (arg == "--thorough") {
                config = BenchmarkConfigFactory::createThoroughBenchmark("Thorough Benchmark");
            } else if (arg == "--learning") {
                config = BenchmarkConfigFactory::createLearningBenchmark("Learning Benchmark");
            } else if (arg == "--stability") {
                config = BenchmarkConfigFactory::createStabilityBenchmark("Stability Benchmark");
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

// Benchmarking exceptions
class BenchmarkException : public runtime_error {
public:
    BenchmarkException(const string& message) : runtime_error(message) {}
};

class BenchmarkConfigurationException : public BenchmarkException {
public:
    BenchmarkConfigurationException(const string& message) : BenchmarkException(message) {}
};

class BenchmarkExecutionException : public BenchmarkException {
public:
    BenchmarkExecutionException(const string& message) : BenchmarkException(message) {}
};

// Benchmarking result validation
namespace BenchmarkValidation {
    bool isValid(const BenchmarkConfig& config);
    vector<string> getValidationErrors(const BenchmarkConfig& config);
}

// Benchmarking statistics
namespace BenchmarkStats {
    struct Statistics {
        double mean;
        double median;
        double mode;
        double variance;
        double standardDeviation;
        double min;
        double max;
        double range;
        double coefficientOfVariation;
    };
    
    Statistics calculateStatistics(const vector<double>& values);
    string statisticsToString(const Statistics& stats);
}

// Benchmarking report generation
namespace BenchmarkReport {
    string generateTextReport(const BenchmarkResult& result);
    string generateJsonReport(const BenchmarkResult& result);
    string generateCsvReport(const BenchmarkResult& result);
    string generateHtmlReport(const BenchmarkResult& result);
}

// Benchmarking comparison utilities
namespace BenchmarkComparison {
    vector<pair<string, double>> compareResults(const vector<BenchmarkResult>& results, const string& metric);
    string generateComparisonReport(const map<string, BenchmarkResult>& results);
}

// Benchmarking visualization (basic)
namespace BenchmarkVisualization {
    void generateTimeSeriesPlot(const BenchmarkResult& result, const string& filepath);
    void generateHistogram(const vector<double>& values, const string& filepath);
    void generateScatterPlot(const map<string, BenchmarkResult>& results, const string& filepath);
}