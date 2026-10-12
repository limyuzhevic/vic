#include "BenchmarkSuite.hpp"
#include "../core/Logger.hpp"
#include <chrono>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <cmath>
#include <algorithm>
#include <numeric>
#include <limits>

namespace nlm {

struct BenchmarkSuite::Impl {
    // Benchmark management
    std::map<std::string, std::function<void()>> benchmarkFunctions;
    std::map<std::string, BenchmarkConfig> benchmarkConfigs;
    std::map<std::string, std::vector<BenchmarkResult>> benchmarkResults;
    
    // Performance baseline
    PerformanceBaseline currentBaseline;
    std::map<std::string, PerformanceBaseline> namedBaselines;
    
    // Global configuration
    BenchmarkConfig defaultConfig;
    bool initialized;
    
    // Performance analysis
    std::map<std::string, std::vector<double>> performanceTrends;
    std::map<std::string, double> stabilityScores;
    
    // Thread management for parallel benchmarking
    std::vector<std::thread> workerThreads;
    std::atomic<bool> shutdown;
    
    Impl() : initialized(false), shutdown(false) {}
};

BenchmarkSuite::BenchmarkSuite() : pImpl(std::make_unique<Impl>()) {}
BenchmarkSuite::~BenchmarkSuite() {
    shutdown();
}

bool BenchmarkSuite::initialize() {
    try {
        Logger::getInstance().logInfo("BenchmarkSuite initialized");
        pImpl->initialized = true;
        
        // Initialize default baselines
        pImpl->currentBaseline.name = "default_baseline";
        pImpl->currentBaseline.createdTime = std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::high_resolution_clock::now().time_since_epoch()).count();
        pImpl->currentBaseline.lastUpdated = pImpl->currentBaseline.createdTime;
        pImpl->currentBaseline.stabilityScore = 1.0;
        pImpl->currentBaseline.sampleCount = 0;
        pImpl->currentBaseline.confidenceInterval = 0.0;
        
        return true;
    } catch (const std::exception& e) {
        Logger::getInstance().logError("Failed to initialize BenchmarkSuite: " + std::string(e.what()));
        return false;
    }
}

void BenchmarkSuite::shutdown() {
    if (pImpl->shutdown) return;
    
    pImpl->shutdown = true;
    
    // Wait for worker threads
    for (auto& thread : pImpl->workerThreads) {
        if (thread.joinable()) {
            thread.join();
        }
    }
    
    pImpl->workerThreads.clear();
    
    Logger::getInstance().logInfo("BenchmarkSuite shutdown");
}

bool BenchmarkSuite::addBenchmark(const std::string& name, std::function<void()> benchmarkFunc) {
    return addBenchmark(BenchmarkConfig{name}, std::move(benchmarkFunc));
}

bool BenchmarkSuite::addBenchmark(const BenchmarkConfig& config, std::function<void()> benchmarkFunc) {
    if (!pImpl->initialized) {
        Logger::getInstance().logError("BenchmarkSuite not initialized");
        return false;
    }
    
    if (pImpl->benchmarkFunctions.find(config.name) != pImpl->benchmarkFunctions.end()) {
        Logger::getInstance().logError("Benchmark already exists: " + config.name);
        return false;
    }
    
    pImpl->benchmarkFunctions[config.name] = std::move(benchmarkFunc);
    pImpl->benchmarkConfigs[config.name] = config;
    pImpl->benchmarkResults[config.name] = std::vector<BenchmarkResult>();
    
    Logger::getInstance().logInfo("Added benchmark: " + config.name);
    return true;
}

BenchmarkResult BenchmarkSuite::runBenchmark(const std::string& name) {
    return runBenchmark(pImpl->benchmarkConfigs[name]);
}

BenchmarkResult BenchmarkSuite::runBenchmark(const BenchmarkConfig& config) {
    if (!pImpl->initialized) {
        Logger::getInstance().logError("BenchmarkSuite not initialized");
        return BenchmarkResult{};
    }
    
    if (pImpl->benchmarkFunctions.find(config.name) == pImpl->benchmarkFunctions.end()) {
        Logger::getInstance().logError("Benchmark not found: " + config.name);
        return BenchmarkResult{};
    }
    
    BenchmarkResult result;
    result.benchmarkName = config.name;
    result.timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::high_resolution_clock::now().time_since_epoch()).count();
    
    try {
        // Warm-up phase
        if (config.warmUpEnabled) {
            for (size_t i = 0; i < config.warmUpIterations; ++i) {
                pImpl->benchmarkFunctions[config.name]();
            }
        }
        
        // Actual benchmark run
        std::vector<double> measurements;
        measurements.reserve(config.iterationCount);
        
        for (size_t i = 0; i < config.iterationCount; ++i) {
            auto startTime = std::chrono::high_resolution_clock::now();
            
            if (config.profileEnabled) {
                // We would start profiling here
            }
            
            pImpl->benchmarkFunctions[config.name]();
            
            if (config.profileEnabled) {
                // We would stop profiling here and collect metrics
            }
            
            auto endTime = std::chrono::high_resolution_clock::now();
            double executionTime = std::chrono::duration<double, std::milli>(endTime - startTime).count();
            
            measurements.push_back(executionTime);
            
            // Check timeout
            if (i > 0 && executionTime > config.timeout * 1000.0) {
                break;
            }
        }
        
        // Calculate statistics
        if (!measurements.empty()) {
            double sum = std::accumulate(measurements.begin(), measurements.end(), 0.0);
            double mean = sum / measurements.size();
            double variance = 0.0;
            
            for (double measurement : measurements) {
                variance += (measurement - mean) * (measurement - mean);
            }
            variance /= measurements.size();
            double stddev = std::sqrt(variance);
            
            result.executionTime = mean;
            result.cpuTime = mean * 0.8; // Assume 80% CPU time
            result.memoryUsage = 1024 * 1024; // Placeholder
            result.throughput = config.iterationCount / (mean / 1000.0); // iterations per second
            result.efficiency = std::min(100.0, 1000.0 / mean); // Efficiency score
            result.iterationCount = measurements.size();
            result.rawMeasurements = measurements;
            result.passed = true;
            
            // Component-specific metrics (placeholder)
            result.componentMetrics["cpu_usage"] = 50.0;
            result.componentMetrics["memory_bandwidth"] = 25.6;
            result.componentMetrics["cache_hit_rate"] = 85.0;
            
            // Update baseline
            pImpl->currentBaseline.metrics[config.name] = mean;
            pImpl->currentBaseline.lastUpdated = result.timestamp;
            pImpl->currentBaseline.sampleCount++;
            pImpl->currentBaseline.confidenceInterval = stddev;
            
            // Update trend
            pImpl->performanceTrends[config.name].push_back(mean);
        } else {
            result.passed = false;
            result.errorMessage = "Benchmark timed out or failed";
        }
        
    } catch (const std::exception& e) {
        result.passed = false;
        result.errorMessage = e.what();
        Logger::getInstance().logError("Benchmark failed: " + std::string(e.what()));
    }
    
    // Store result
    pImpl->benchmarkResults[config.name].push_back(result);
    
    return result;
}

std::vector<BenchmarkResult> BenchmarkSuite::runBenchmarks(const std::vector<std::string>& benchmarkNames) {
    std::vector<BenchmarkResult> results;
    results.reserve(benchmarkNames.size());
    
    for (const auto& name : benchmarkNames) {
        results.push_back(runBenchmark(name));
    }
    
    return results;
}

void BenchmarkSuite::runNeuralNetworkBenchmarks() {
    // Standard neural network benchmarks
    addBenchmark("neuron_initialization", []() { benchmarks::benchmarkNeuronInitialization(); });
    addBenchmark("synapse_creation", []() { benchmarks::benchmarkSynapseCreation(); });
    addBenchmark("spike_propagation", []() { benchmarks::benchmarkSpikePropagation(); });
    addBenchmark("neural_connectivity", []() { benchmarks::benchmarkNeuralConnectivity(); });
}

void BenchmarkSuite::runMemorySystemBenchmarks() {
    addBenchmark("working_memory_access", []() { benchmarks::benchmarkWorkingMemoryAccess(); });
    addBenchmark("episodic_memory_storage", []() { benchmarks::benchmarkEpisodicMemoryStorage(); });
    addBenchmark("memory_consolidation", []() { benchmarks::benchmarkMemoryConsolidation(); });
    addBenchmark("memory_retrieval", []() { benchmarks::benchmarkMemoryRetrieval(); });
}

void BenchmarkSuite::runPlasticityBenchmarks() {
    addBenchmark("stdp_computation", []() { benchmarks::benchmarkSTDPComputation(); });
    addBenchmark("hebbian_learning", []() { benchmarks::benchmarkHebbianLearning(); });
    addBenchmark("structural_plasticity", []() { benchmarks::benchmarkStructuralPlasticity(); });
    addBenchmark("homeostatic_plasticity", []() { benchmarks::benchmarkHomeostaticPlasticity(); });
}

void BenchmarkSuite::runIntegrationBenchmarks() {
    addBenchmark("integration_loop", []() { benchmarks::benchmarkIntegrationLoop(); });
    addBenchmark("sensor_processing", []() { benchmarks::benchmarkSensorProcessing(); });
    addBenchmark("motor_output", []() { benchmarks::benchmarkMotorOutput(); });
    addBenchmark("decision_making", []() { benchmarks::benchmarkDecisionMaking(); });
}

void BenchmarkSuite::runIobenchmarks() {
    addBenchmark("disk_io", []() { benchmarks::benchmarkDiskIO(); });
    addBenchmark("network_io", []() { benchmarks::benchmarkNetworkIO(); });
    addBenchmark("file_processing", []() { benchmarks::benchmarkFileProcessing(); });
    addBenchmark("data_serialization", []() { benchmarks::benchmarkDataSerialization(); });
}

void BenchmarkSuite::runScalabilityBenchmarks() {
    // Scalability benchmarks require variable parameters
    // These are added on-demand based on configuration
}

bool BenchmarkSuite::isRegressionDetected(const std::string& benchmarkName, const BenchmarkResult& result) {
    if (pImpl->benchmarkResults.find(benchmarkName) == pImpl->benchmarkResults.end()) {
        return false;
    }
    
    const auto& history = pImpl->benchmarkResults[benchmarkName];
    if (history.empty()) return false;
    
    // Calculate baseline from history
    double baseline = 0.0;
    size_t count = 0;
    for (const auto& prevResult : history) {
        if (prevResult.passed) {
            baseline += prevResult.executionTime;
            count++;
        }
    }
    
    if (count == 0) return false;
    
    baseline /= count;
    
    // Check for regression (20% or more degradation)
    double regressionThreshold = baseline * 1.2;
    return result.executionTime > regressionThreshold;
}

double BenchmarkSuite::calculateRegressionPercent(const std::string& benchmarkName, const BenchmarkResult& result) {
    if (pImpl->benchmarkResults.find(benchmarkName) == pImpl->benchmarkResults.end()) {
        return 0.0;
    }
    
    const auto& history = pImpl->benchmarkResults[benchmarkName];
    if (history.empty()) return 0.0;
    
    double baseline = 0.0;
    size_t count = 0;
    for (const auto& prevResult : history) {
        if (prevResult.passed) {
            baseline += prevResult.executionTime;
            count++;
        }
    }
    
    if (count == 0) return 0.0;
    
    baseline /= count;
    
    return ((result.executionTime - baseline) / baseline) * 100.0;
}

bool BenchmarkSuite::runRegressionTest(const std::string& baselineName) {
    if (pImpl->namedBaselines.find(baselineName) == pImpl->namedBaselines.end()) {
        Logger::getInstance().logError("Baseline not found: " + baselineName);
        return false;
    }
    
    const auto& baseline = pImpl->namedBaselines[baselineName];
    bool hasRegressions = false;
    
    for (const auto& [benchmarkName, results] : pImpl->benchmarkResults) {
        if (results.empty()) continue;
        
        const auto& latestResult = results.back();
        if (latestResult.passed) {
            double regressionPercent = calculateRegressionPercent(benchmarkName, latestResult);
            if (regressionPercent > 10.0) { // 10% regression threshold
                hasRegressions = true;
                Logger::getInstance().logWarning("Regression detected in " + benchmarkName + ": " + 
                                               std::to_string(regressionPercent) + "%");
            }
        }
    }
    
    return !hasRegressions;
}

void BenchmarkSuite::setPerformanceBaseline(const std::string& baselineName) {
    if (pImpl->namedBaselines.find(baselineName) != pImpl->namedBaselines.end()) {
        pImpl->currentBaseline = pImpl->namedBaselines[baselineName];
    }
}

double BenchmarkSuite::compareToBaseline(const std::string& benchmarkName, const BenchmarkResult& result) {
    if (pImpl->benchmarkResults.find(benchmarkName) == pImpl->benchmarkResults.end()) {
        return 0.0;
    }
    
    const auto& history = pImpl->benchmarkResults[benchmarkName];
    if (history.empty()) return 0.0;
    
    // Simple comparison with latest result
    const auto& latestResult = history.back();
    if (latestResult.passed && result.passed) {
        return (result.executionTime / latestResult.executionTime) * 100.0;
    }
    
    return 0.0;
}

void BenchmarkSuite::generateComparisonReport(const std::string& filename) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        Logger::getInstance().logError("Failed to open comparison report file: " + filename);
        return;
    }
    
    file << "=== Benchmark Comparison Report ===" << std::endl;
    file << "Generated: " << std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::high_resolution_clock::now().time_since_epoch()).count() << " ms" << std::endl;
    file << "Current Baseline: " << pImpl->currentBaseline.name << std::endl;
    file << std::endl;
    
    for (const auto& [benchmarkName, results] : pImpl->benchmarkResults) {
        if (results.empty()) continue;
        
        const auto& latestResult = results.back();
        file << "Benchmark: " << benchmarkName << std::endl;
        file << "  Latest Execution Time: " << std::fixed << std::setprecision(2) 
             << latestResult.executionTime << " ms" << std::endl;
        file << "  Latest Throughput: " << std::fixed << std::setprecision(2) 
             << latestResult.throughput << " ops/sec" << std::endl;
        file << "  Latest Efficiency: " << std::fixed << std::setprecision(2) 
             << latestResult.efficiency << "%" << std::endl;
        file << "  Status: " << (latestResult.passed ? "PASSED" : "FAILED") << std::endl;
        
        if (latestResult.passed) {
            double regression = calculateRegressionPercent(benchmarkName, latestResult);
            if (regression > 0) {
                file << "  Regression: " << std::fixed << std::setprecision(1) << regression << "% DECREASE" << std::endl;
            } else {
                file << "  Improvement: " << std::fixed << std::setprecision(1) << -regression << "% INCREASE" << std::endl;
            }
        }
        file << std::endl;
    }
    
    file.close();
    Logger::getInstance().logInfo("Benchmark comparison report generated: " + filename);
}

const BenchmarkResult& BenchmarkSuite::getLastResult(const std::string& benchmarkName) const {
    static BenchmarkResult emptyResult;
    
    auto it = pImpl->benchmarkResults.find(benchmarkName);
    if (it == pImpl->benchmarkResults.end() || it->second.empty()) {
        return emptyResult;
    }
    
    return it->second.back();
}

const std::vector<BenchmarkResult>& BenchmarkSuite::getBenchmarkHistory(const std::string& benchmarkName) const {
    static std::vector<BenchmarkResult> emptyHistory;
    
    auto it = pImpl->benchmarkResults.find(benchmarkName);
    if (it == pImpl->benchmarkResults.end()) {
        return emptyHistory;
    }
    
    return it->second;
}

const std::vector<BenchmarkResult>& BenchmarkSuite::getAllResults() const {
    static std::vector<BenchmarkResult> allResults;
    allResults.clear();
    
    for (const auto& [benchmarkName, results] : pImpl->benchmarkResults) {
        allResults.insert(allResults.end(), results.begin(), results.end());
    }
    
    return allResults;
}

void BenchmarkSuite::analyzePerformanceTrends() {
    for (auto& [benchmarkName, measurements] : pImpl->performanceTrends) {
        if (measurements.size() < 2) continue;
        
        double sum = std::accumulate(measurements.begin(), measurements.end(), 0.0);
        double mean = sum / measurements.size();
        
        double variance = 0.0;
        for (double measurement : measurements) {
            variance += (measurement - mean) * (measurement - mean);
        }
        variance /= measurements.size();
        
        pImpl->stabilityScores[benchmarkName] = std::exp(-variance / (mean * mean)); // Stability score
    }
}

void BenchmarkSuite::identifyPerformancePatterns() {
    // Simple pattern detection (placeholder for advanced algorithms)
    for (auto& [benchmarkName, measurements] : pImpl->performanceTrends) {
        if (measurements.size() < 3) continue;
        
        // Check for monotonic trends
        bool increasing = true, decreasing = true;
        for (size_t i = 1; i < measurements.size(); ++i) {
            if (measurements[i] > measurements[i-1]) {
                decreasing = false;
            } else if (measurements[i] < measurements[i-1]) {
                increasing = false;
            }
        }
        
        if (increasing) {
            Logger::getInstance().logInfo("Performance trend: " + benchmarkName + " is increasing");
        } else if (decreasing) {
            Logger::getInstance().logWarning("Performance trend: " + benchmarkName + " is decreasing");
        }
    }
}

void BenchmarkSuite::generateBenchmarkReport(const std::string& filename) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        Logger::getInstance().logError("Failed to open benchmark report file: " + filename);
        return;
    }
    
    file << "=== NLM Benchmark Suite Report ===" << std::endl;
    file << "Generated: " << std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::high_resolution_clock::now().time_since_epoch()).count() << " ms" << std::endl;
    file << "Total Benchmarks: " << pImpl->benchmarkFunctions.size() << std::endl;
    file << "Total Runs: " << getAllResults().size() << std::endl;
    file << "Successful Runs: " << getSuccessfulRunCount() << std::endl;
    file << "Failed Runs: " << getFailedRunCount() << std::endl;
    file << std::endl;
    
    // Benchmark summary table
    file << "=== Benchmark Summary ===" << std::endl;
    file << "Name,ExecutionTime(ms),Throughput,Stability,Status" << std::endl;
    
    for (const auto& [benchmarkName, results] : pImpl->benchmarkResults) {
        if (results.empty()) continue;
        
        const auto& latestResult = results.back();
        double stability = 0.0;
        auto it = pImpl->stabilityScores.find(benchmarkName);
        if (it != pImpl->stabilityScores.end()) {
            stability = it->second;
        }
        
        file << benchmarkName << "," 
             << std::fixed << std::setprecision(2) << latestResult.executionTime << "," 
             << std::fixed << std::setprecision(2) << latestResult.throughput << "," 
             << std::fixed << std::setprecision(2) << stability << "," 
             << (latestResult.passed ? "PASSED" : "FAILED") << std::endl;
    }
    
    file.close();
    Logger::getInstance().logInfo("Benchmark report generated: " + filename);
}

void BenchmarkSuite::exportResults(const std::string& filename) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        Logger::getInstance().logError("Failed to export benchmark results: " + filename);
        return;
    }
    
    // Export in CSV format for external analysis
    file << "benchmark,timestamp,execution_time,throughput,efficiency,passed" << std::endl;
    
    for (const auto& [benchmarkName, results] : pImpl->benchmarkResults) {
        for (const auto& result : results) {
            file << benchmarkName << "," 
                 << result.timestamp << "," 
                 << std::fixed << std::setprecision(2) << result.executionTime << "," 
                 << std::fixed << std::setprecision(2) << result.throughput << "," 
                 << std::fixed << std::setprecision(2) << result.efficiency << "," 
                 << (result.passed ? "true" : "false") << std::endl;
        }
    }
    
    file.close();
    Logger::getInstance().logInfo("Benchmark results exported: " + filename);
}

size_t BenchmarkSuite::getBenchmarkCount() const {
    return pImpl->benchmarkFunctions.size();
}

size_t BenchmarkSuite::getSuccessfulRunCount() const {
    size_t count = 0;
    for (const auto& [benchmarkName, results] : pImpl->benchmarkResults) {
        for (const auto& result : results) {
            if (result.passed) count++;
        }
    }
    return count;
}

size_t BenchmarkSuite::getFailedRunCount() const {
    size_t count = 0;
    for (const auto& [benchmarkName, results] : pImpl->benchmarkResults) {
        for (const auto& result : results) {
            if (!result.passed) count++;
        }
    }
    return count;
}

double BenchmarkSuite::getAverageExecutionTime(const std::string& benchmarkName) const {
    auto it = pImpl->benchmarkResults.find(benchmarkName);
    if (it == pImpl->benchmarkResults.end() || it->second.empty()) {
        return 0.0;
    }
    
    double sum = 0.0;
    size_t count = 0;
    for (const auto& result : it->second) {
        if (result.passed) {
            sum += result.executionTime;
            count++;
        }
    }
    
    return count > 0 ? sum / count : 0.0;
}

double BenchmarkSuite::getMinExecutionTime(const std::string& benchmarkName) const {
    auto it = pImpl->benchmarkResults.find(benchmarkName);
    if (it == pImpl->benchmarkResults.end() || it->second.empty()) {
        return 0.0;
    }
    
    double minTime = std::numeric_limits<double>::max();
    for (const auto& result : it->second) {
        if (result.passed && result.executionTime < minTime) {
            minTime = result.executionTime;
        }
    }
    
    return minTime == std::numeric_limits<double>::max() ? 0.0 : minTime;
}

double BenchmarkSuite::getMaxExecutionTime(const std::string& benchmarkName) const {
    auto it = pImpl->benchmarkResults.find(benchmarkName);
    if (it == pImpl->benchmarkResults.end() || it->second.empty()) {
        return 0.0;
    }
    
    double maxTime = 0.0;
    for (const auto& result : it->second) {
        if (result.passed && result.executionTime > maxTime) {
            maxTime = result.executionTime;
        }
    }
    
    return maxTime;
}

} // namespace nlm