// Performance Benchmark and Metrics System for NLM
// Phase 2: Real Neural Computation
//
// This system provides:
// - Performance benchmarking for simulation components
// - Metrics collection and analysis
// - Performance profiling and optimization support
// - Benchmarking of plasticity rules
// - Benchmarking of neural dynamics
// - Benchmarking of learning algorithms
// - Performance comparison across configurations

#include <chrono>
#include <map>
#include <vector>
#include <string>
#include <memory>
#include <algorithm>
#include <iomanip>
#include <sstream>
#include <fstream>
#include <numeric>
#include <cmath>
#include <thread>
#include <atomic>
#include <mutex>

using namespace std;

namespace nlm {

// Performance measurement types
enum class PerformanceMetricType {
    Throughput,        // Operations per second
    Latency,           // Time per operation
    MemoryUsage,       // Memory consumption
    CPUUsage,          // CPU utilization
    Accuracy,          // Success rate or accuracy
    EnergyEfficiency,  // Performance per watt
    Scalability,       // Performance scaling with load
    Quality           // Quality of results
};

// Benchmark configuration
struct BenchmarkConfig {
    string benchmarkName;
    PerformanceMetricType primaryMetric;
    double targetValue;
    double tolerance;  // Acceptable deviation from target
    int numIterations; // Number of iterations to run
    int warmupIterations; // Warmup iterations to discard
    bool enableProfiling; // Enable detailed profiling
    bool enableAveraging; // Enable averaging of results
    chrono::duration<double> timeout; // Maximum benchmark duration
    map<string, string> parameters; // Additional parameters
    
    BenchmarkConfig() : targetValue(0.0), tolerance(0.05), numIterations(100),
                       warmupIterations(10), enableProfiling(false),
                       enableAveraging(true), timeout(300.0) {} // 5 minutes timeout
};

// Benchmark result
struct BenchmarkResult {
    string benchmarkName;
    PerformanceMetricType metricType;
    double value;              // Measured value
    double unitValue;          // Value in primary units
    string unit;               // Unit (e.g., "spikes/sec", "ms", "MB")
    chrono::duration<double> duration; // Actual benchmark duration
    int iterationsCompleted;   // Number of iterations completed
    bool successful;           // Whether benchmark completed successfully
    string errorMessage;       // Error message if failed
    chrono::system_clock::time_point startTime;
    chrono::system_clock::time_point endTime;
    
    // Detailed measurements for each iteration
    vector<double> iterationValues;
    vector<chrono::duration<double>> iterationDurations;
    
    // Statistical analysis
    double mean;               // Mean value
    double median;             // Median value
    double stddev;             // Standard deviation
    double min;                // Minimum value
    double max;                // Maximum value
    double percentile95;       // 95th percentile
    double percentile99;       // 99th percentile
    
    // Performance metrics
    double opsPerSecond;       // Operations per second
    double efficiencyScore;    // 0-1 efficiency score
    
    BenchmarkResult() : value(0.0), unitValue(0.0), iterationsCompleted(0),
                       successful(false), mean(0.0), median(0.0), stddev(0.0),
                       min(0.0), max(0.0), percentile95(0.0), percentile99(0.0),
                       opsPerSecond(0.0), efficiencyScore(0.0) {}
};

// Performance benchmark suite
class PerformanceBenchmark {
public:
    PerformanceBenchmark() {
        enableAggregation = true;
        enableReporting = true;
        enableAutoSave = true;
        reportDirectory = "benchmark_reports";
        csvFile = "benchmark_results.csv";
        jsonFile = "benchmark_results.json";
        maxResults = 1000;
        clearOnStart = true;
    }
    
    // Register a benchmark
    void registerBenchmark(const string& name, 
                          function<double()> benchmarkFunc,
                          const BenchmarkConfig& config) {
        unique_ptr<Benchmark> benchmark = make_unique<Benchmark>();
        benchmark->name = name;
        benchmark->func = benchmarkFunc;
        benchmark->config = config;
        
        benchmarks[name] = move(benchmark);
    }
    
    // Run a single benchmark
    BenchmarkResult runBenchmark(const string& name) {
        auto it = benchmarks.find(name);
        if (it == benchmarks.end()) {
            BenchmarkResult result;
            result.benchmarkName = name;
            result.successful = false;
            result.errorMessage = "Benchmark not found: " + name;
            return result;
        }
        
        return it->second->run();
    }
    
    // Run all benchmarks
    vector<BenchmarkResult> runAllBenchmarks() {
        vector<BenchmarkResult> results;
        
        for (auto& pair : benchmarks) {
            BenchmarkResult result = pair.second->run();
            results.push_back(result);
            
            // Store results
            storeResult(result);
            
            // Report progress
            if (enableReporting) {
                reportBenchmarkResult(result);
            }
        }
        
        // Aggregate results
        if (enableAggregation) {
            aggregateResults();
        }
        
        return results;
    }
    
    // Get results for a specific benchmark
    vector<BenchmarkResult> getResults(const string& name) const {
        vector<BenchmarkResult> results;
        for (const auto& result : storedResults) {
            if (result.benchmarkName == name) {
                results.push_back(result);
            }
        }
        return results;
    }
    
    // Get all results
    vector<BenchmarkResult> getAllResults() const {
        return storedResults;
    }
    
    // Clear results
    void clearResults() {
        storedResults.clear();
        aggregatedResults.clear();
    }
    
    // Generate report
    string generateReport(const vector<BenchmarkResult>& results) {
        stringstream report;
        
        report << "NLM Performance Benchmark Report" << endl;
        report << "=================================" << endl;
        report << "Generated: " << getCurrentTimestamp() << endl;
        report << "Total benchmarks: " << results.size() << endl;
        report << "Successful: " << countSuccessful(results) << endl;
        report << "Failed: " << countFailed(results) << endl;
        report << endl;
        
        // Summary table
        report << "Benchmark Summary:" << endl;
        report << "------------------" << endl;
        report << left << setw(30) << "Benchmark" 
               << setw(15) << "Value" 
               << setw(15) << "Unit" 
               << setw(15) << "Iterations" 
               << setw(15) << "Success" 
               << setw(15) << "Duration" << endl;
        report << string(90, '-') << endl;
        
        for (const auto& result : results) {
            report << left << setw(30) << result.benchmarkName
                   << setw(15) << fixed << setprecision(4) << result.value
                   << setw(15) << result.unit
                   << setw(15) << result.iterationsCompleted
                   << setw(15) << (result.successful ? "Yes" : "No")
                   << setw(15) << fixed << setprecision(2) << result.duration.count() << "s" << endl;
        }
        
        report << endl;
        
        // Detailed results
        report << "Detailed Results:" << endl;
        report << "-----------------" << endl;
        
        for (const auto& result : results) {
            if (!result.successful) {
                report << result.benchmarkName << ": " << result.errorMessage << endl;
                continue;
            }
            
            report << result.benchmarkName << ":" << endl;
            report << "  Metric: " << performanceMetricTypeToString(result.metricType) << endl;
            report << "  Value: " << fixed << setprecision(6) << result.value << " " << result.unit << endl;
            report << "  Mean: " << fixed << setprecision(6) << result.mean << " " << result.unit << endl;
            report << "  Stddev: " << fixed << setprecision(6) << result.stddev << " " << result.unit << endl;
            report << "  Min: " << fixed << setprecision(6) << result.min << " " << result.unit << endl;
            report << "  Max: " << fixed << setprecision(6) << result.max << " " << result.unit << endl;
            report << "  Iterations: " << result.iterationsCompleted << endl;
            report << "  Duration: " << fixed << setprecision(2) << result.duration.count() << "s" << endl;
            report << "  Ops/sec: " << fixed << setprecision(2) << result.opsPerSecond << endl;
            report << "  Efficiency: " << fixed << setprecision(4) << result.efficiencyScore << endl;
            report << endl;
        }
        
        return report.str();
    }
    
    // Export results to CSV
    bool exportToCSV(const string& filename) {
        try {
            ofstream file(filename);
            if (!file.is_open()) {
                return false;
            }
            
            // Header
            file << "Benchmark,MetricType,Value,Unit,Mean,Stddev,Min,Max,Iterations,Success,Duration(s),OpsPerSec,Efficiency" << endl;
            
            for (const auto& result : storedResults) {
                file << escapeCSV(result.benchmarkName) << ","
                     << escapeCSV(performanceMetricTypeToString(result.metricType)) << ","
                     << fixed << setprecision(6) << result.value << ","
                     << escapeCSV(result.unit) << ","
                     << fixed << setprecision(6) << result.mean << ","
                     << fixed << setprecision(6) << result.stddev << ","
                     << fixed << setprecision(6) << result.min << ","
                     << fixed << setprecision(6) << result.max << ","
                     << result.iterationsCompleted << ","
                     << (result.successful ? "true" : "false") << ","
                     << fixed << setprecision(4) << result.duration.count() << ","
                     << fixed << setprecision(2) << result.opsPerSecond << ","
                     << fixed << setprecision(6) << result.efficiencyScore << endl;
            }
            
            file.close();
            return true;
            
        } catch (const exception& e) {
            return false;
        }
    }
    
    // Export results to JSON
    bool exportToJSON(const string& filename) {
        try {
            ofstream file(filename);
            if (!file.is_open()) {
                return false;
            }
            
            json report;
            report["benchmark_report"] = {
                {"generated", getCurrentTimestamp()},
                {"total_benchmarks", storedResults.size()},
                {"successful_benchmarks", countSuccessful(storedResults)},
                {"failed_benchmarks", countFailed(storedResults)}
            };
            
            report["benchmarks"] = json::array();
            for (const auto& result : storedResults) {
                json benchmark;
                benchmark["name"] = result.benchmarkName;
                benchmark["metric_type"] = performanceMetricTypeToString(result.metricType);
                benchmark["value"] = result.value;
                benchmark["unit"] = result.unit;
                benchmark["mean"] = result.mean;
                benchmark["stddev"] = result.stddev;
                benchmark["min"] = result.min;
                benchmark["max"] = result.max;
                benchmark["iterations"] = result.iterationsCompleted;
                benchmark["success"] = result.successful;
                benchmark["duration_seconds"] = result.duration.count();
                benchmark["ops_per_second"] = result.opsPerSecond;
                benchmark["efficiency_score"] = result.efficiencyScore;
                
                if (!result.errorMessage.empty()) {
                    benchmark["error"] = result.errorMessage;
                }
                
                report["benchmarks"].push_back(benchmark);
            }
            
            file << report.dump(2) << endl;
            file.close();
            return true;
            
        } catch (const exception& e) {
            return false;
        }
    }
    
    // Set up benchmark directory
    void setupBenchmarkDirectory(const string& directory) {
        reportDirectory = directory;
        if (!directory.empty() && directory != "auto") {
            createDirectory(directory);
        } else if (directory == "auto") {
            reportDirectory = "benchmark_reports_" + getCurrentTimestampString();
            createDirectory(reportDirectory);
        }
        
        csvFile = reportDirectory + "/benchmark_results.csv";
        jsonFile = reportDirectory + "/benchmark_results.json";
    }
    
    // Enable/disable features
    void enableAggregation(bool enable) { enableAggregation = enable; }
    void enableReporting(bool enable) { enableReporting = enable; }
    void enableAutoSave(bool enable) { enableAutoSave = enable; }
    void clearOnStart(bool enable) { clearOnStart = enable; }
    
    // Configuration
    struct Config {
        string benchmarkName;
        PerformanceMetricType metricType;
        double targetValue;
        double tolerance;
        int iterations;
        int warmupIterations;
        bool enableProfiling;
        bool enableAveraging;
        chrono::duration<double> timeout;
        map<string, string> parameters;
    };
    
private:
    struct Benchmark {
        string name;
        function<double()> func;
        BenchmarkConfig config;
        
        BenchmarkResult run() {
            BenchmarkResult result;
            result.benchmarkName = name;
            result.metricType = config.primaryMetric;
            result.startTime = chrono::system_clock::now();
            
            try {
                // Warmup
                for (int i = 0; i < config.warmupIterations; ++i) {
                    func(); // Discard warmup results
                }
                
                // Actual benchmark run
                vector<double> values;
                values.reserve(config.numIterations);
                
                for (int i = 0; i < config.numIterations; ++i) {
                    if (chrono::system_clock::now() - result.startTime > config.timeout) {
                        result.errorMessage = "Benchmark timeout exceeded";
                        break;
                    }
                    
                    auto iterationStart = chrono::system_clock::now();
                    double value = func();
                    auto iterationEnd = chrono::system_clock::now();
                    
                    values.push_back(value);
                    result.iterationValues.push_back(value);
                    result.iterationDurations.push_back(
                        chrono::duration<double>(iterationEnd - iterationStart)
                    );
                }
                
                if (values.empty()) {
                    result.errorMessage = "No values collected";
                } else {
                    result.value = values.back(); // Last value as primary result
                    calculateStatistics(result, values);
                    result.successful = true;
                }
                
            } catch (const exception& e) {
                result.errorMessage = string("Exception: ") + e.what();
            }
            
            result.endTime = chrono::system_clock::now();
            result.duration = chrono::duration<double>(result.endTime - result.startTime);
            
            return result;
        }
        
    private:
        void calculateStatistics(BenchmarkResult& result, const vector<double>& values) {
            if (values.empty()) return;
            
            // Basic statistics
            result.mean = accumulate(values.begin(), values.end(), 0.0) / values.size();
            result.min = *min_element(values.begin(), values.end());
            result.max = *max_element(values.begin(), values.end());
            
            // Standard deviation
            double sumSquares = 0.0;
            for (double val : values) {
                double diff = val - result.mean;
                sumSquares += diff * diff;
            }
            result.stddev = sqrt(sumSquares / values.size());
            
            // Percentiles
            vector<double> sortedValues = values;
            sort(sortedValues.begin(), sortedValues.end());
            
            size_t idx95 = static_cast<size_t>(0.95 * values.size());
            idx95 = min(idx95, values.size() - 1);
            result.percentile95 = sortedValues[idx95];
            
            size_t idx99 = static_cast<size_t>(0.99 * values.size());
            idx99 = min(idx99, values.size() - 1);
            result.percentile99 = sortedValues[idx99];
            
            // Median
            size_t mid = values.size() / 2;
            if (values.size() % 2 == 0) {
                result.median = (sortedValues[mid - 1] + sortedValues[mid]) / 2.0;
            } else {
                result.median = sortedValues[mid];
            }
            
            // Operations per second
            double totalTime = 0.0;
            for (const auto& duration : result.iterationDurations) {
                totalTime += duration.count();
            }
            if (totalTime > 0.0) {
                result.opsPerSecond = values.size() / totalTime;
            }
            
            // Efficiency score (0-1, higher is better)
            double target = result.config.targetValue;
            if (target > 0.0) {
                double relativePerformance = result.value / target;
                if (relativePerformance > 1.0) {
                    result.efficiencyScore = min(1.0, relativePerformance);
                } else {
                    result.efficiencyScore = relativePerformance * (1.0 - config.tolerance);
                }
            } else {
                result.efficiencyScore = 1.0 - (result.stddev / result.mean);
            }
        }
    };
    
    void storeResult(const BenchmarkResult& result) {
        if (enableAutoSave) {
            storedResults.push_back(result);
            if (storedResults.size() > maxResults) {
                storedResults.erase(storedResults.begin());
            }
        }
    }
    
    void aggregateResults() {
        aggregatedResults.clear();
        
        // Simple aggregation: group by benchmark name
        map<string, vector<BenchmarkResult>> groupedResults;
        for (const auto& result : storedResults) {
            groupedResults[result.benchmarkName].push_back(result);
        }
        
        for (const auto& pair : groupedResults) {
            BenchmarkResult aggregated;
            aggregated.benchmarkName = pair.first;
            
            // Average values
            vector<double> values;
            for (const auto& result : pair.second) {
                if (result.successful) {
                    values.push_back(result.value);
                }
            }
            
            if (!values.empty()) {
                aggregated.value = accumulate(values.begin(), values.end(), 0.0) / values.size();
                aggregated.mean = aggregated.value; // For aggregated results, mean = value
                aggregated.iterationsCompleted = values.size();
                aggregated.successful = true;
            }
            
            aggregatedResults.push_back(aggregated);
        }
    }
    
    void reportBenchmarkResult(const BenchmarkResult& result) {
        if (result.successful) {
            infoEnhanced(string("Benchmark completed: ") + result.benchmarkName +
                        " - " + to_string(result.value) + " " + result.unit +
                        " in " + to_string(result.duration.count()) + "s",
                        "", 0, "PerformanceBenchmark",
                        "benchmark");
        } else {
            warningEnhanced(string("Benchmark failed: ") + result.benchmarkName +
                          " - " + result.errorMessage,
                          "", 0, "PerformanceBenchmark",
                          "benchmark");
        }
    }
    
    size_t countSuccessful(const vector<BenchmarkResult>& results) const {
        size_t count = 0;
        for (const auto& result : results) {
            if (result.successful) count++;
        }
        return count;
    }
    
    size_t countFailed(const vector<BenchmarkResult>& results) const {
        size_t count = 0;
        for (const auto& result : results) {
            if (!result.successful) count++;
        }
        return count;
    }
    
    void createDirectory(const string& path) {
        // In a real implementation, this would use filesystem operations
        // For now, just assume the directory exists
    }
    
    string escapeCSV(const string& value) {
        string escaped = value;
        // Escape quotes and wrap in quotes if contains comma or quote
        if (value.find(',') != string::npos || value.find('"') != string::npos) {
            replace(escaped.begin(), escaped.end(), '"', '""');
            escaped = '"' + escaped + '"';
        }
        return escaped;
    }
    
    string performanceMetricTypeToString(PerformanceMetricType type) {
        switch (type) {
            case PerformanceMetricType::Throughput: return "throughput";
            case PerformanceMetricType::Latency: return "latency";
            case PerformanceMetricType::MemoryUsage: return "memory_usage";
            case PerformanceMetricType::CPUUsage: return "cpu_usage";
            case PerformanceMetricType::Accuracy: return "accuracy";
            case PerformanceMetricType::EnergyEfficiency: return "energy_efficiency";
            case PerformanceMetricType::Scalability: return "scalability";
            case PerformanceMetricType::Quality: return "quality";
            default: return "unknown";
        }
    }
    
    string getCurrentTimestamp() {
        auto now = chrono::system_clock::now();
        auto time = chrono::system_clock::to_time_t(now);
        stringstream ss;
        ss << put_time(localtime(&time), "%Y-%m-%d %H:%M:%S");
        return ss.str();
    }
    
    string getCurrentTimestampString() {
        auto now = chrono::system_clock::now();
        auto time = chrono::system_clock::to_time_t(now);
        stringstream ss;
        ss << put_time(localtime(&time), "%Y%m%d_%H%M%S");
        return ss.str();
    }
    
    map<string, unique_ptr<Benchmark>> benchmarks;
    vector<BenchmarkResult> storedResults;
    vector<BenchmarkResult> aggregatedResults;
    
    bool enableAggregation;
    bool enableReporting;
    bool enableAutoSave;
    bool clearOnStart;
    string reportDirectory;
    string csvFile;
    string jsonFile;
    size_t maxResults;
};

// Global benchmark instance
PerformanceBenchmark globalBenchmark;

// Convenience functions for benchmarking
void registerBenchmark(const string& name, 
                      function<double()> benchmarkFunc,
                      const BenchmarkConfig& config = BenchmarkConfig()) {
    globalBenchmark.registerBenchmark(name, benchmarkFunc, config);
}

BenchmarkResult runBenchmark(const string& name) {
    return globalBenchmark.runBenchmark(name);
}

vector<BenchmarkResult> runAllBenchmarks() {
    return globalBenchmark.runAllBenchmarks();
}

void setupBenchmarkDirectory(const string& directory = "auto") {
    globalBenchmark.setupBenchmarkDirectory(directory);
}

// Specialized benchmark functions for NLM components
struct NLMBenchmarks {
    static BenchmarkConfig createSpikeBenchmark(int numNeurons = 100, int numSteps = 1000) {
        BenchmarkConfig config;
        config.benchmarkName = "spike_generation";
        config.primaryMetric = PerformanceMetricType::Throughput;
        config.targetValue = 1000.0; // target spikes per second
        config.tolerance = 0.1;
        config.numIterations = 10;
        config.warmupIterations = 2;
        config.enableProfiling = true;
        
        // Store parameters
        config.parameters["neurons"] = to_string(numNeurons);
        config.parameters["steps"] = to_string(numSteps);
        
        return config;
    }
    
    static BenchmarkConfig createPlasticityBenchmark(int numSynapses = 500, int learningIterations = 100) {
        BenchmarkConfig config;
        config.benchmarkName = "plasticity_learning";
        config.primaryMetric = PerformanceMetricType::Accuracy;
        config.targetValue = 0.95; // target learning accuracy
        config.tolerance = 0.05;
        config.numIterations = 5;
        config.warmupIterations = 1;
        config.enableProfiling = true;
        
        config.parameters["synapses"] = to_string(numSynapses);
        config.parameters["iterations"] = to_string(learningIterations);
        
        return config;
    }
    
    static BenchmarkConfig createMemoryBenchmark(int memorySize = 1000, int operations = 5000) {
        BenchmarkConfig config;
        config.benchmarkName = "memory_operations";
        config.primaryMetric = PerformanceMetricType::Throughput;
        config.targetValue = 10000.0; // target operations per second
        config.tolerance = 0.1;
        config.numIterations = 8;
        config.warmupIterations = 2;
        config.enableProfiling = true;
        
        config.parameters["memory_size"] = to_string(memorySize);
        config.parameters["operations"] = to_string(operations);
        
        return config;
    }
};

// Performance monitoring helper
class PerformanceMonitor {
public:
    PerformanceMonitor() {
        enabled = false;
        samplingInterval = 100; // milliseconds
        maxSamples = 1000;
    }
    
    void startMonitoring() {
        enabled = true;
        samples.clear();
        startTime = chrono::high_resolution_clock::now();
        monitorThread = thread(&PerformanceMonitor::monitorWorker, this);
    }
    
    void stopMonitoring() {
        if (monitorThread.joinable()) {
            enabled = false;
            monitorCondition.notify_one();
            monitorThread.join();
        }
    }
    
    vector<PerformanceSample> getSamples() const {
        return samples;
    }
    
    void setSamplingInterval(int intervalMs) {
        samplingInterval = intervalMs;
    }
    
    void setMaxSamples(size_t max) {
        maxSamples = max;
    }
    
private:
    struct PerformanceSample {
        chrono::system_clock::time_point timestamp;
        double cpuUsage;
        double memoryUsage;
        double diskUsage;
        uint64_t availableMemory;
        double networkUsage;
    };
    
    void monitorWorker() {
        while (enabled) {
            collectSample();
            this_thread::sleep_for(chrono::milliseconds(samplingInterval));
        }
    }
    
    void collectSample() {
        PerformanceSample sample;
        sample.timestamp = chrono::system_clock::now();
        sample.cpuUsage = getCpuUsage();
        auto memInfo = getMemoryUsage();
        sample.memoryUsage = get<0>(memInfo);
        sample.availableMemory = get<1>(memInfo);
        sample.diskUsage = getDiskUsage();
        sample.networkUsage = getNetworkUsage();
        
        lock_guard<mutex> lock(mutex);
        samples.push_back(sample);
        
        if (samples.size() > maxSamples) {
            samples.erase(samples.begin());
        }
    }
    
    tuple<double, uint64_t, uint64_t> getMemoryUsage() {
        // Simplified memory usage calculation
        // In a real implementation, this would read from system APIs
        return {50.0, 8589934592, 4294967296}; // 8GB total, 4GB available (placeholder)
    }
    
    double getCpuUsage() {
        // Simplified CPU usage calculation
        // In a real implementation, this would read from /proc/stat or use platform-specific APIs
        return 50.0; // Placeholder
    }
    
    double getDiskUsage() {
        // Simplified disk usage calculation
        // In a real implementation, this would read from /proc/mounts or use platform-specific APIs
        return 30.0; // Placeholder
    }
    
    double getNetworkUsage() {
        // Simplified network usage calculation
        // In a real implementation, this would read from network interfaces
        return 0.0; // Placeholder
    }
    
    atomic<bool> enabled;
    int samplingInterval;
    size_t maxSamples;
    vector<PerformanceSample> samples;
    chrono::system_clock::time_point startTime;
    thread monitorThread;
    mutex mutex;
    condition_variable monitorCondition;
};

// Global performance monitor instance
PerformanceMonitor globalMonitor;

// Convenience functions for performance monitoring
void startPerformanceMonitoring() {
    globalMonitor.startMonitoring();
}

void stopPerformanceMonitoring() {
    globalMonitor.stopMonitoring();
}

vector<PerformanceMonitor::PerformanceSample> getPerformanceSamples() {
    return globalMonitor.getSamples();
}

// Performance profiling helper
class PerformanceProfiler {
public:
    PerformanceProfiler() {
        enabled = false;
        profileDepth = 2;
    }
    
    void startProfiling(const string& functionName) {
        if (!enabled) return;
        
        lock_guard<mutex> lock(mutex);
        
        ProfileData data;
        data.functionName = functionName;
        data.startTime = chrono::high_resolution_clock::now();
        data.threadId = this_thread::get_id();
        
        profileStack.push_back(data);
    }
    
    void stopProfiling() {
        if (!enabled) return;
        
        lock_guard<mutex> lock(mutex);
        
        if (profileStack.empty()) return;
        
        ProfileData data = profileStack.back();
        profileStack.pop_back();
        
        auto endTime = chrono::high_resolution_clock::now();
        data.duration = chrono::duration<double>(endTime - data.startTime);
        
        // Store profile data
        profileData.push_back(data);
        
        // Log performance
        logPerformance(data.functionName, data.duration.count(), data.threadId);
    }
    
    vector<ProfileData> getProfileData() const {
        return profileData;
    }
    
    void enableProfiling(bool enable) {
        enabled = enable;
    }
    
    void setProfileDepth(int depth) {
        profileDepth = depth;
    }
    
    // Generate profile report
    string generateProfileReport() {
        stringstream report;
        
        report << "NLM Performance Profile Report" << endl;
        report << "================================" << endl;
        report << "Generated: " << getCurrentTimestamp() << endl;
        report << "Total functions profiled: " << profileData.size() << endl;
        report << "Max stack depth: " << profileDepth << endl;
        report << endl;
        
        // Sort by duration
        vector<ProfileData> sortedData = profileData;
        sort(sortedData.begin(), sortedData.end(), 
             [](const ProfileData& a, const ProfileData& b) {
                 return a.duration.count() > b.duration.count();
             });
        
        // Top functions by duration
        report << "Top Functions by Duration:" << endl;
        report << "----------------------------" << endl;
        
        size_t limit = min(profileDepth, sortedData.size());
        for (size_t i = 0; i < limit; ++i) {
            const ProfileData& data = sortedData[i];
            report << left << setw(40) << data.functionName
                   << setw(15) << fixed << setprecision(6) << data.duration.count() << "s"
                   << setw(15) << (data.duration.count() * 1000.0) << "ms" << endl;
        }
        
        report << endl;
        
        // Summary statistics
        double totalDuration = 0.0;
        for (const auto& data : profileData) {
            totalDuration += data.duration.count();
        }
        
        double avgDuration = profileData.empty() ? 0.0 : totalDuration / profileData.size();
        double maxDuration = profileData.empty() ? 0.0 : 
            max_element(profileData.begin(), profileData.end(),
                       [](const ProfileData& a, const ProfileData& b) {
                           return a.duration.count() < b.duration.count();
                       })->duration.count();
        
        report << "Summary Statistics:" << endl;
        report << "-------------------" << endl;
        report << "Total duration: " << fixed << setprecision(4) << totalDuration << "s" << endl;
        report << "Average duration: " << fixed << setprecision(6) << avgDuration << "s" << endl;
        report << "Maximum duration: " << fixed << setprecision(6) << maxDuration << "s" << endl;
        
        if (!profileData.empty()) {
            report << "Number of samples: " << profileData.size() << endl;
        }
        
        return report.str();
    }
    
private:
    struct ProfileData {
        string functionName;
        chrono::high_resolution_clock::time_point startTime;
        chrono::duration<double> duration;
        thread::id threadId;
    };
    
    atomic<bool> enabled;
    int profileDepth;
    vector<ProfileData> profileStack;
    vector<ProfileData> profileData;
    mutex mutex;
    
    void logPerformance(const string& functionName, double duration, thread::id threadId) {
        // Log performance data
        // This could be extended to use the enhanced logging system
    }
    
    string getCurrentTimestamp() {
        auto now = chrono::system_clock::now();
        auto time = chrono::system_clock::to_time_t(now);
        stringstream ss;
        ss << put_time(localtime(&time), "%Y-%m-%d %H:%M:%S");
        return ss.str();
    }
};

// Global performance profiler instance
PerformanceProfiler globalProfiler;

// Convenience functions for performance profiling
void startPerformanceProfiler(const string& functionName) {
    globalProfiler.startProfiling(functionName);
}

void stopPerformanceProfiler() {
    globalProfiler.stopProfiling();
}

vector<PerformanceProfiler::ProfileData> getPerformanceProfileData() {
    return globalProfiler.getProfileData();
}

void enablePerformanceProfiler(bool enable) {
    globalProfiler.enableProfiling(enable);
}

string generatePerformanceProfileReport() {
    return globalProfiler.generateProfileReport();
}

// Macro for easy performance profiling
#define NLM_PERFORMANCE_PROFILER_START(name) nlm::startPerformanceProfiler(name)
#define NLM_PERFORMANCE_PROFILER_STOP() nlm::stopPerformanceProfiler()

// Macro for easy benchmarking
#define NLM_BENCHMARK(name, func, config) nlm::registerBenchmark(name, func, config)

} // namespace nlm
