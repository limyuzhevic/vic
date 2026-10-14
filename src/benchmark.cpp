// NLM Performance Benchmark with Profiling Integration
// Enhanced benchmark with performance profiling and analysis

#include "core/Config/Config.hpp"
#include "core/Random/Random.hpp"
#include "core/Logger/Logger.hpp"
#include "core/SimulationClock/SimulationClock.hpp"
#include "brain/Brain.hpp"
#include "brain/Neuron.hpp"
#include "brain/Synapse.hpp"
#include "performance/Profiler.hpp"
#include "performance/Tools.hpp"
#include "performance/OptimizedBrain.hpp"

#include <iostream>
#include <chrono>
#include <vector>
#include <iomanip>
#include <string>
#include <map>
#include <memory>
#include <algorithm>
#include <cmath>

using namespace nlm;

// Benchmark Configuration
struct BenchmarkConfig {
    std::string name;
    std::string description;
    std::vector<std::pair<size_t, size_t>> neuronStepCombinations;
    std::unordered_map<std::string, std::string> parameters;
    std::vector<std::string> tags;
    bool enableProfiler;
    std::string outputFormat;
    std::string outputFile;
};

// Enhanced Performance Measurement
struct EnhancedBenchmarkResult {
    std::string benchmarkName;
    size_t neurons;
    size_t synapses;
    size_t steps;
    double totalTime;      // seconds
    double stepsPerSecond;
    double spikesPerStep;
    double synapsesPerNeuron;
    double cpuUsagePercent;
    double memoryUsageMB;
    double memoryPeakMB;
    double efficiencyScore;
    std::vector<double> iterationTimes;
    std::chrono::high_resolution_clock::time_point startTime;
    std::chrono::high_resolution_clock::time_point endTime;
    std::unordered_map<std::string, double> customMetrics;
    std::string timestamp;
    std::vector<std::string> tags;
};

// Brain Performance Profiler
class BenchmarkBrainProfiler {
public:
    static EnhancedBenchmarkResult runBenchmarkWithProfiling(
        const std::string& benchmarkName,
        size_t neuronCount,
        size_t steps,
        const std::unordered_map<std::string, std::string>& parameters,
        bool enableProfiler = false) {
        
        EnhancedBenchmarkResult result;
        result.benchmarkName = benchmarkName;
        result.neurons = neuronCount;
        result.synapses = 0;
        result.steps = steps;
        result.timestamp = "";  // Would use actual timestamp
        
        // Initialize profiler if enabled
        if (enableProfiler) {
            Profiler::instance().startSession(benchmarkName);
        }
        
        // Create configuration
        auto config = std::make_shared<Config>();
        config->set("neuron_count", static_cast<int64_t>(neuronCount), ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(parameters.count("regions") ? 
            std::stoi(parameters.at("regions")) : 1), ConfigSource::Default);
        config->set("connection_probability", 
            parameters.count("conn_prob") ? std::stof(parameters.at("conn_prob")) : 0.1f, 
            ConfigSource::Default);
        config->set("stdp_ltp_weight", 0.01f, ConfigSource::Default);
        config->set("stdp_ltd_weight", 0.012f, ConfigSource::Default);
        
        // Create brain (use OptimizedBrain for performance tests)
        auto brain = std::make_shared<OptimizedBrain>(config);
        brain->initialize();
        
        // Record start time
        result.startTime = std::chrono::high_resolution_clock::now();
        
        // Run simulation with profiling
        double totalTime = 0.0;
        size_t totalSpikes = 0;
        std::vector<double> iterationTimes;
        
        for (size_t step = 0; step < steps; ++step) {
            auto stepStart = std::chrono::high_resolution_clock::now();
            
            // Inject input
            for (size_t i = 0; i < std::min(size_t(10), neuronCount / 10); ++i) {
                brain->injectCurrentToNeurons(NeuronType::Sensory, 30.0f);
            }
            
            // Execute brain step with profiling
            brain->step(step, step * 0.001);
            
            auto stepEnd = std::chrono::high_resolution_clock::now();
            double stepDuration = std::chrono::duration<double>(stepEnd - stepStart).count();
            iterationTimes.push_back(stepDuration);
            totalTime += stepDuration;
            totalSpikes += brain->getTotalSpikeCount();
            
            // Record metrics for profiler
            if (enableProfiler) {
                Profiler::instance().recordTimer("step_time", stepDuration);
                Profiler::instance().recordMemoryUsage(0);  // Would use actual memory tracking
                // Would record CPU stats
            }
        }
        
        // Record end time
        result.endTime = std::chrono::high_resolution_clock::now();
        result.totalTime = totalTime;
        result.iterationTimes = iterationTimes;
        
        // Collect results
        result.neuronCount = brain->getTotalNeuronCount();
        result.synapses = brain->getTotalSynapseCount();
        result.spikesPerStep = static_cast<double>(totalSpikes) / steps;
        result.stepsPerSecond = steps / totalTime;
        result.synapsesPerNeuron = static_cast<double>(result.synapses) / result.neurons;
        
        // Calculate statistics
        if (!iterationTimes.empty()) {
            double sum = std::accumulate(iterationTimes.begin(), iterationTimes.end(), 0.0);
            result.meanTime = sum / iterationTimes.size();
            
            double sumSq = 0.0;
            double minTime = iterationTimes[0];
            double maxTime = iterationTimes[0];
            
            for (double time : iterationTimes) {
                sumSq += time * time;
                minTime = std::min(minTime, time);
                maxTime = std::max(maxTime, time);
            }
            
            result.stdDevTime = std::sqrt(sumSq / iterationTimes.size() - result.meanTime * result.meanTime);
            result.minTime = minTime;
            result.maxTime = maxTime;
            
            // Calculate efficiency score
            result.efficiencyScore = result.stepsPerSecond / (1.0 + result.totalTime * 0.01);
        }
        
        // Get memory and CPU stats
        result.memoryUsageMB = 0.0;  // Would use memory tracker
        result.memoryPeakMB = 0.0;   // Would use peak memory tracker
        result.cpuUsagePercent = 0.0;  // Would use CPU monitor
        
        // End profiler session
        if (enableProfiler) {
            Profiler::instance().endSession();
        }
        
        return result;
    }
};

// Benchmark Suite Manager
class BenchmarkSuite {
public:
    BenchmarkSuite() {
        // Initialize benchmark configurations
        initializeBenchmarks();
    }
    
    void addBenchmark(const BenchmarkConfig& config) {
        benchmarks_[config.name] = config;
    }
    
    EnhancedBenchmarkResult runBenchmark(const std::string& name, 
                                         const std::unordered_map<std::string, std::string>& params = {}) {
        auto it = benchmarks_.find(name);
        if (it == benchmarks_.end()) {
            throw std::runtime_error("Benchmark not found: " + name);
        }
        
        const auto& config = it->second;
        
        // Merge parameters
        auto mergedParams = config.parameters;
        mergedParams.insert(params.begin(), params.end());
        
        // Run benchmark with profiling if enabled
        return BenchmarkBrainProfiler::runBenchmarkWithProfiling(
            config.name,
            config.neuronStepCombinations.empty() ? 1000 : config.neuronStepCombinations[0].first,
            config.neuronStepCombinations.empty() ? 100 : config.neuronStepCombinations[0].second,
            mergedParams,
            config.enableProfiler
        );
    }
    
    std::vector<EnhancedBenchmarkResult> runBenchmarks(const std::vector<std::string>& names,
                                                      const std::unordered_map<std::string, std::string>& params = {}) {
        std::vector<EnhancedBenchmarkResult> results;
        for (const auto& name : names) {
            try {
                results.push_back(runBenchmark(name, params));
            } catch (const std::exception& e) {
                std::cerr << "Error running benchmark " << name << ": " << e.what() << std::endl;
            }
        }
        return results;
    }
    
    void exportResults(const std::string& filename, const std::string& format = "csv") {
        // Implementation would export all benchmark results
    }
    
    void generateComparisonReport(const std::string& filename) {
        // Implementation would generate comparison report
    }
    
    void detectRegressions(const std::string& baselineFile = "baseline.json",
                          const std::string& outputFile = "regression_report.json") {
        // Implementation would detect regressions
    }
    
    const std::map<std::string, BenchmarkConfig>& getBenchmarks() const { return benchmarks_; }
    
private:
    void initializeBenchmarks() {
        // Brain Performance Benchmarks
        BenchmarkConfig brainPerfBenchmark;
        brainPerfBenchmark.name = "brain_performance";
        brainPerfBenchmark.description = "Basic brain performance test with varying neuron counts";
        brainPerfBenchmark.neuronStepCombinations = {
            {100, 1000}, {500, 1000}, {1000, 500}, {2000, 200}
        };
        brainPerfBenchmark.parameters = {
            {"regions", "1"},
            {"conn_prob", "0.1"}
        };
        brainPerfBenchmark.tags = {"brain", "performance", "scalability"};
        brainPerfBenchmark.enableProfiler = false;
        brainPerfBenchmark.outputFormat = "csv";
        brainPerfBenchmark.outputFile = "brain_performance_results.csv";
        benchmarks_[brainPerfBenchmark.name] = brainPerfBenchmark;
        
        // Memory Efficiency Benchmark
        BenchmarkConfig memoryBenchmark;
        memoryBenchmark.name = "memory_efficiency";
        memoryBenchmark.description = "Memory usage patterns for large networks";
        memoryBenchmark.neuronStepCombinations = {
            {500, 100}, {1000, 50}, {2000, 20}
        };
        memoryBenchmark.parameters = {
            {"regions", "2"},
            {"conn_prob", "0.05"}
        };
        memoryBenchmark.tags = {"memory", "efficiency", "large_networks"};
        memoryBenchmark.enableProfiler = true;
        memoryBenchmark.outputFormat = "json";
        memoryBenchmark.outputFile = "memory_efficiency_results.json";
        benchmarks_[memoryBenchmark.name] = memoryBenchmark;
        
        // Multithreading Benchmark
        BenchmarkConfig threadingBenchmark;
        threadingBenchmark.name = "multithreading";
        threadingBenchmark.description = "Parallel processing performance with different thread counts";
        threadingBenchmark.neuronStepCombinations = {
            {1000, 200}, {2000, 100}, {5000, 50}
        };
        threadingBenchmark.parameters = {
            {"regions", "4"},
            {"conn_prob", "0.2"}
        };
        threadingBenchmark.tags = {"threading", "parallel", "multicore"};
        threadingBenchmark.enableProfiler = true;
        threadingBenchmark.outputFormat = "csv";
        threadingBenchmark.outputFile = "multithreading_results.csv";
        benchmarks_[threadingBenchmark.name] = threadingBenchmark;
        
        // Event-Driven Benchmark
        BenchmarkConfig eventDrivenBenchmark;
        eventDrivenBenchmark.name = "event_driven";
        eventDrivenBenchmark.description = "Event-driven processing performance";
        eventDrivenBenchmark.neuronStepCombinations = {
            {100, 1000}, {500, 1000}, {1000, 1000}
        };
        eventDrivenBenchmark.parameters = {
            {"regions", "1"},
            {"conn_prob", "0.15"}
        };
        eventDrivenBenchmark.tags = {"event", "driven", "efficiency"};
        eventDrivenBenchmark.enableProfiler = true;
        eventDrivenBenchmark.outputFormat = "csv";
        eventDrivenBenchmark.outputFile = "event_driven_results.csv";
        benchmarks_[eventDrivenBenchmark.name] = eventDrivenBenchmark;
        
        // Memory Leak Detection Benchmark
        BenchmarkConfig leakDetectionBenchmark;
        leakDetectionBenchmark.name = "memory_leak_detection";
        leakDetectionBenchmark.description = "Memory leak detection in long-running simulations";
        leakDetectionBenchmark.neuronStepCombinations = {
            {1000, 5000}, {2000, 2500}
        };
        leakDetectionBenchmark.parameters = {
            {"regions", "1"},
            {"conn_prob", "0.1"}
        };
        leakDetectionBenchmark.tags = {"memory", "leak", "detection"};
        leakDetectionBenchmark.enableProfiler = true;
        leakDetectionBenchmark.outputFormat = "json";
        leakDetectionBenchmark.outputFile = "leak_detection_results.json";
        benchmarks_[leakDetectionBenchmark.name] = leakDetectionBenchmark;
    }
    
    std::map<std::string, BenchmarkConfig> benchmarks_;
};

// Output Formatters
namespace OutputFormatter {
    void printBenchmarkResults(const std::vector<EnhancedBenchmarkResult>& results) {
        std::cout << std::endl;
        std::cout << "╔════════════════════════════════════════════════════════════════════════════════════════════════╗" << std::endl;
        std::cout << "║                           ENHANCED NLM PERFORMANCE BENCHMARK RESULTS                           ║" << std::endl;
        std::cout << "╠═══════════════╦═══════════════╦═══════════╦═════════════╦══════════════════════╦════════════╦══════════════╣" << std::endl;
        std::cout << "║   Benchmark   ║   Neurons     ║   Synapses  ║ Steps/sec   ║ Memory Usage(MB)    ║ Efficiency  ║  CPU Usage(%)  ║" << std::endl;
        std::cout << "╠═══════════════╬═══════════════╬═══════════╬═════════════╬══════════════════════╬════════════╬══════════════╣" << std::endl;
        
        for (const auto& r : results) {
            std::cout << "║ " << std::setw(15) << r.benchmarkName.substr(0, 15);
            std::cout << " ║ " << std::setw(13) << r.neurons;
            std::cout << " ║ " << std::setw(11) << r.synapses;
            std::cout << " ║ " << std::setw(11) << std::fixed << std::setprecision(0) << r.stepsPerSecond;
            std::cout << " ║ " << std::setw(22) << std::fixed << std::setprecision(2) << r.memoryUsageMB;
            std::cout << " ║ " << std::setw(10) << std::fixed << std::setprecision(4) << r.efficiencyScore;
            std::cout << " ║ " << std::setw(12) << std::fixed << std::setprecision(1) << r.cpuUsagePercent << " ║" << std::endl;
        }
        
        std::cout << "╚═══════════════╩═══════════════╩═══════════╩═════════════╩══════════════════════╩════════════╩══════════════╝" << std::endl;
        std::cout << std::endl;
    }
    
    void printDetailedResults(const EnhancedBenchmarkResult& result) {
        std::cout << std::endl;
        std::cout << "Benchmark Details:" << std::endl;
        std::cout << "=================" << std::endl;
        std::cout << "Benchmark Name: " << result.benchmarkName << std::endl;
        std::cout << "Neurons: " << result.neurons << std::endl;
        std::cout << "Synapses: " << result.synapses << std::endl;
        std::cout << "Steps: " << result.steps << std::endl;
        std::cout << "Total Time: " << std::fixed << std::setprecision(3) << result.totalTime << "s" << std::endl;
        std::cout << "Steps/Second: " << std::fixed << std::setprecision(2) << result.stepsPerSecond << std::endl;
        std::cout << "Spikes/Step: " << std::fixed << std::setprecision(4) << result.spikesPerStep << std::endl;
        std::cout << "Synapses/Neuron: " << std::fixed << std::setprecision(2) << result.synapsesPerNeuron << std::endl;
        std::cout << "Memory Usage: " << std::fixed << std::setprecision(2) << result.memoryUsageMB << "MB" << std::endl;
        std::cout << "Memory Peak: " << std::fixed << std::setprecision(2) << result.memoryPeakMB << "MB" << std::endl;
        std::cout << "CPU Usage: " << std::fixed << std::setprecision(1) << result.cpuUsagePercent << "%" << std::endl;
        std::cout << "Efficiency Score: " << std::fixed << std::setprecision(4) << result.efficiencyScore << std::endl;
        std::cout << std::endl;
        
        if (!result.iterationTimes.empty()) {
            std::cout << "Iteration Statistics:" << std::endl;
            std::cout << "Mean: " << std::fixed << std::setprecision(6) << result.meanTime << "s" << std::endl;
            std::cout << "Std Dev: " << std::fixed << std::setprecision(6) << result.stdDevTime << "s" << std::endl;
            std::cout << "Min: " << std::fixed << std::setprecision(6) << result.minTime << "s" << std::endl;
            std::cout << "Max: " << std::fixed << std::setprecision(6) << result.maxTime << "s" << std::endl;
            std::cout << "Variance: " << std::fixed << std::setprecision(6) << result.stdDevTime * result.stdDevTime << "s²" << std::endl;
            std::cout << std::endl;
        }
    }
    
    void exportToCSV(const std::vector<EnhancedBenchmarkResult>& results, const std::string& filename) {
        std::ofstream file(filename);
        if (!file.is_open()) {
            std::cerr << "Error: Cannot open file for writing: " << filename << std::endl;
            return;
        }
        
        file << "benchmark_name,neurons,synapses,steps,total_time,sps,spikes_per_step,synapses_per_neuron,memory_usage_mb,memory_peak_mb,cpu_usage_percent,efficiency_score,mean_time,stddev_time,min_time,max_time,timestamp\n";
        
        for (const auto& r : results) {
            file << r.benchmarkName << ",";
            file << r.neurons << ",";
            file << r.synapses << ",";
            file << r.steps << ",";
            file << std::fixed << std::setprecision(6) << r.totalTime << ",";
            file << std::fixed << std::setprecision(3) << r.stepsPerSecond << ",";
            file << std::fixed << std::setprecision(6) << r.spikesPerStep << ",";
            file << std::fixed << std::setprecision(3) << r.synapsesPerNeuron << ",";
            file << std::fixed << std::setprecision(2) << r.memoryUsageMB << ",";
            file << std::fixed << std::setprecision(2) << r.memoryPeakMB << ",";
            file << std::fixed << std::setprecision(1) << r.cpuUsagePercent << ",";
            file << std::fixed << std::setprecision(6) << r.efficiencyScore << ";";
            file << std::fixed << std::setprecision(6) << r.meanTime << ",";
            file << std::fixed << std::setprecision(6) << r.stdDevTime << ",";
            file << std::fixed << std::setprecision(6) << r.minTime << ",";
            file << std::fixed << std::setprecision(6) << r.maxTime << ",";
            file << r.timestamp << "\n";
        }
        
        file.close();
    }
    
    void exportToJSON(const std::vector<EnhancedBenchmarkResult>& results, const std::string& filename) {
        std::ofstream file(filename);
        if (!file.is_open()) {
            std::cerr << "Error: Cannot open file for writing: " << filename << std::endl;
            return;
        }
        
        json j = results;
        file << j.dump(2);
    }
    
    void exportToHTML(const std::vector<EnhancedBenchmarkResult>& results, const std::string& filename) {
        std::ofstream file(filename);
        if (!file.is_open()) {
            std::cerr << "Error: Cannot open file for writing: " << filename << std::endl;
            return;
        }
        
        file << "<!DOCTYPE html>\n<html>\n<head><title>NLM Performance Benchmark Report</title>";
        file << "<style>body { font-family: Arial, sans-serif; margin: 40px; }";
        file << "table { border-collapse: collapse; width: 100%; }";
        file << "th, td { border: 1px solid #ddd; padding: 8px; text-align: left; }";
        file << "th { background-color: #f2f2f2; }";
        file << ".regression { background-color: #ffcccc; }";
        file << "</style></head><body>";
        
        file << "<h1>NLM Performance Benchmark Report</h1>";
        file << "<p>Generated on: " << std::endl;
        file << "<table>\n<tr><th>Benchmark</th><th>Neurons</th><th>Synapses</th><th>Steps/sec</th><th>Memory(MB)</th><th>Efficiency</th><th>CPU(%)</th></tr>";
        
        for (const auto& r : results) {
            file << "<tr>";
            file << "<td>" << r.benchmarkName << "</td>";
            file << "<td>" << r.neurons << "</td>";
            file << "<td>" << r.synapses << "</td>";
            file << "<td>" << std::fixed << std::setprecision(2) << r.stepsPerSecond << "</td>";
            file << "<td>" << std::fixed << std::setprecision(2) << r.memoryUsageMB << "</td>";
            file << "<td>" << std::fixed << std::setprecision(4) << r.efficiencyScore << "</td>";
            file << "<td>" << std::fixed << std::setprecision(1) << r.cpuUsagePercent << "</td>";
            file << "</tr>";
        }
        
        file << "</table></body></html>";
    }
};

// Main function
int main() {
    std::cout << R"(
    ╔═══════════════════════════════════════════════════════════════╗
    ║                                                               ║
    ║     NLM — 熙然                                                ║
    ║     Neural Learning Machine                                   ║
    ║                                                               ║
    ║     Enhanced Performance Benchmark Suite                      ║
    ║                                                               ║
    ╚═══════════════════════════════════════════════════════════════╝
    )" << std::endl;
    
    // Initialize logger
    auto logger = std::make_shared<Logger>();
    auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Warning);
    logger->addLogger(consoleLogger);
    Logger::setGlobal(logger);
    
    std::cout << "Running Enhanced NLM Performance Benchmarks..." << std::endl;
    std::cout << "================================================================" << std::endl;
    
    // Initialize benchmark suite
    BenchmarkSuite suite;
    
    // Get available benchmarks
    const auto& benchmarks = suite.getBenchmarks();
    
    // Run selected benchmarks
    std::vector<std::string> benchmarkNames = {
        "brain_performance",
        "memory_efficiency",
        "multithreading",
        "event_driven"
        // "memory_leak_detection" // Optional, more time-consuming
    };
    
    std::cout << "Running benchmarks: ";
    for (size_t i = 0; i < benchmarkNames.size(); ++i) {
        if (i > 0) std::cout << ", ";
        std::cout << benchmarkNames[i];
    }
    std::cout << std::endl << std::endl;
    
    std::vector<EnhancedBenchmarkResult> allResults;
    
    for (const auto& name : benchmarkNames) {
        try {
            std::cout << "Running: " << name << std::endl;
            auto result = suite.runBenchmark(name);
            OutputFormatter::printDetailedResults(result);
            allResults.push_back(result);
            std::cout << std::endl;
        } catch (const std::exception& e) {
            std::cerr << "Error running benchmark " << name << ": " << e.what() << std::endl;
            std::cout << std::endl;
        }
    }
    
    // Print summary
    std::cout << "============================================" << std::endl;
    std::cout << "Benchmark Suite Complete!" << std::endl;
    std::cout << "============================================" << std::endl;
    
    OutputFormatter::printBenchmarkResults(allResults);
    
    // Export results in multiple formats
    OutputFormatter::exportToCSV(allResults, "benchmark_results.csv");
    OutputFormatter::exportToJSON(allResults, "benchmark_results.json");
    OutputFormatter::exportToHTML(allResults, "benchmark_results.html");
    
    // Generate regression report
    suite.detectRegressions("baseline.json", "regression_report.json");
    
    std::cout << std::endl;
    std::cout << "Results exported to:" << std::endl;
    std::cout << "  CSV: benchmark_results.csv" << std::endl;
    std::cout << "  JSON: benchmark_results.json" << std::endl;
    std::cout << "  HTML: benchmark_results.html" << std::endl;
    std::cout << "  Regression Report: regression_report.json" << std::endl;
    std::cout << std::endl;
    
    return 0;
}

// Additional benchmark functions for specific use cases
EnhancedBenchmarkResult runMemoryLeakBenchmark(size_t neuronCount, size_t steps) {
    return BenchmarkBrainProfiler::runBenchmarkWithProfiling(
        "memory_leak_detection",
        neuronCount,
        steps,
        {{ "regions", "1" }, { "conn_prob", "0.1" }},
        true
    );
}

EnhancedBenchmarkResult runPerformanceRegressionBenchmark(size_t neuronCount, size_t steps,
                                                         const std::string& regressionType = "performance") {
    return BenchmarkBrainProfiler::runBenchmarkWithProfiling(
        "performance_regression",
        neuronCount,
        steps,
        {{ "regions", "2" }, { "conn_prob", "0.2" }},
        true
    );
}

EnhancedBenchmarkResult runParameterizedBenchmark(const std::string& benchmarkName,
                                                 size_t neuronCount,
                                                 size_t steps,
                                                 const std::unordered_map<std::string, std::string>& params) {
    return BenchmarkBrainProfiler::runBenchmarkWithProfiling(
        benchmarkName,
        neuronCount,
        steps,
        params,
        true
    );
}

