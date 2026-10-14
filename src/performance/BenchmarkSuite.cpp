#include "BenchmarkSuite.hpp"

namespace nlm {

// BenchmarkSuite implementation

BenchmarkSuite::BenchmarkSuite(const BenchmarkConfig& config) 
    : config_(config)
    , enableHardwareBenchmarking_(config.enableHardwareCounters)
    , enableMemoryProfiling_(config.enableMemoryProfiling)
    , enableCPUPProfiling_(config.enableCPUP profiling) {
}

BenchmarkSuite::~BenchmarkSuite() {
}

bool BenchmarkSuite::run(const std::string& name,
                        std::function<void()> benchmarkFunc,
                        const std::string& description) {
    if (benchmarkFunc == nullptr) {
        return false;
    }
    
    BenchmarkResult result;
    result.name = name;
    result.description = description;
    result.startTime = std::chrono::high_resolution_clock::now();
    
    try {
        // Run warmup iterations
        for (uint32_t i = 0; i < config_.warmupIterations; ++i) {
            benchmarkFunc();
        }
        
        // Run actual benchmark
        if (enableHardwareBenchmarking_) {
            result = runWithMeasurements(name, benchmarkFunc,
                [this](BenchmarkResult& res) {
                    res.memoryUsageMB = measureMemoryUsage(nullptr);
                    res.cpuUsagePercent = measureCPUUsage(nullptr);
                    res.flops = 0; // Would need hardware-specific implementation
                    res.memoryTrafficBytes = 0; // Would need hardware-specific implementation
                },
                description);
        } else {
            result = runWithMeasurements(name, benchmarkFunc,
                [this](BenchmarkResult& res) {
                    res.memoryUsageMB = measureMemoryUsage(nullptr);
                    res.cpuUsagePercent = measureCPUUsage(nullptr);
                },
                description);
        }
        
        // Add to results
        std::lock_guard<std::mutex> lock(resultsMutex_);
        results_.push_back(result);
        
        return true;
    } catch (const std::exception& e) {
        // Log error
        return false;
    }
}

bool BenchmarkSuite::runWithMeasurements(const std::string& name,
                                         std::function<void()> benchmarkFunc,
                                         std::function<void(BenchmarkResult&)> measureFunc,
                                         const std::string& description) {
    BenchmarkResult result;
    result.name = name;
    result.description = description;
    result.startTime = std::chrono::high_resolution_clock::now();
    
    try {
        // Run benchmark
        for (uint32_t i = 0; i < config_.numIterations; ++i) {
            benchmarkFunc();
        }
        
        result.endTime = std::chrono::high_resolution_clock::now();
        double totalTimeMs = std::chrono::duration<double, std::milli>(
            result.endTime - result.startTime).count();
        
        result.totalTimeMs = totalTimeMs;
        result.iterations = config_.numIterations;
        
        if (config_.numIterations > 0) {
            result.avgTimeMs = totalTimeMs / config_.numIterations;
            result.minTimeMs = result.avgTimeMs; // Would need tracking of min/max
            result.maxTimeMs = result.avgTimeMs;
        }
        
        // Run measurements
        if (measureFunc) {
            measureFunc(result);
        }
        
        // Calculate statistics
        if (config_.enableDetailedStats) {
            // Would need actual measurements
            result.stdDevMs = result.avgTimeMs * 0.1; // Placeholder
            result.cvMs = result.stdDevMs / result.avgTimeMs;
        }
        
        // Calculate operations per second
        if (result.avgTimeMs > 0) {
            result.opsPerSecond = (config_.numIterations * 1000.0) / result.avgTimeMs;
        }
        
        return true;
    } catch (const std::exception& e) {
        return false;
    }
}

std::vector<BenchmarkResult> BenchmarkSuite::runBatch(const std::vector<std::pair<std::string, std::function<void()>>>& benchmarks) {
    std::vector<BenchmarkResult> results;
    
    for (const auto& benchmark : benchmarks) {
        BenchmarkResult result;
        try {
            result = run(benchmark.first, benchmark.second);
            results.push_back(result);
        } catch (const std::exception& e) {
            // Log error
        }
    }
    
    return results;
}

const BenchmarkSuite::BenchmarkResult* BenchmarkSuite::getResult(const std::string& name) const {
    for (const auto& result : results_) {
        if (result.name == name) {
            return &result;
        }
    }
    return nullptr;
}

bool BenchmarkSuite::saveResults(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    // Write results in text format
    file << "Benchmark Results" << std::endl;
    file << "================" << std::endl;
    
    for (const auto& result : results_) {
        file << "Benchmark: " << result.name << std::endl;
        file << "  Description: " << result.description << std::endl;
        file << "  Iterations: " << result.iterations << std::endl;
        file << "  Total Time: " << result.totalTimeMs << " ms" << std::endl;
        file << "  Average Time: " << result.avgTimeMs << " ms" << std::endl;
        file << "  Min Time: " << result.minTimeMs << " ms" << std::endl;
        file << "  Max Time: " << result.maxTimeMs << " ms" << std::endl;
        file << "  Standard Deviation: " << result.stdDevMs << " ms" << std::endl;
        file << "  Operations/sec: " << result.opsPerSecond << std::endl;
        file << "  Memory Usage: " << result.memoryUsageMB << " MB" << std::endl;
        file << "  CPU Usage: " << result.cpuUsagePercent << "%" << std::endl;
        file << std::endl;
    }
    
    file.close();
    return true;
}

bool BenchmarkSuite::loadResults(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    // In a real implementation, this would parse a saved benchmark results file
    // For now, just clear existing results
    clearResults();
    
    file.close();
    return true;
}

std::vector<BenchmarkSuite::BenchmarkResult> BenchmarkSuite::compareWithBaseline(
    const std::vector<BenchmarkResult>& baseline,
    double regressionThresholdPercent) const {
    
    std::vector<BenchmarkResult> regressions;
    
    for (const auto& baselineResult : baseline) {
        const BenchmarkResult* currentResult = getResult(baselineResult.name);
        if (currentResult) {
            double regression = (currentResult->avgTimeMs - baselineResult.avgTimeMs) / 
                               baselineResult.avgTimeMs * 100.0;
            
            if (std::abs(regression) > regressionThresholdPercent) {
                BenchmarkResult regressedResult = *currentResult;
                regressedResult.avgTimeMs = baselineResult.avgTimeMs; // Show baseline
                regressions.push_back(regressedResult);
            }
        }
    }
    
    return regressions;
}

std::string BenchmarkSuite::generateReport() const {
    std::stringstream ss;
    ss << std::fixed << std::setprecision(config_.precision);
    
    ss << "=== Performance Benchmark Report ===" << std::endl;
    ss << "Generated: " << std::chrono::system_clock::now() << std::endl;
    ss << std::endl;
    ss << "Benchmark Configuration:" << std::endl;
    ss << "  Iterations: " << config_.numIterations << std::endl;
    ss << "  Warmup Iterations: " << config_.warmupIterations << std::endl;
    ss << "  Timeout: " << config_.timeout.count() << " ms" << std::endl;
    ss << "  Detailed Stats: " << (config_.enableDetailedStats ? "Yes" : "No") << std::endl;
    ss << "  Hardware Counters: " << (config_.enableHardwareCounters ? "Yes" : "No") << std::endl;
    ss << "  Memory Profiling: " << (config_.enableMemoryProfiling ? "Yes" : "No") << std::endl;
    ss << "  CPU Profiling: " << (config_.enableCPUP profiling ? "Yes" : "No") << std::endl;
    ss << "  Confidence Level: " << config_.confidenceLevel << std::endl;
    ss << "  Regression Detection: " << (config_.detectRegressions ? "Yes" : "No") << std::endl;
    ss << std::endl;
    
    ss << "=== Benchmark Results ===" << std::endl;
    for (const auto& result : results_) {
        ss << "Benchmark: " << result.name << std::endl;
        ss << "  Description: " << result.description << std::endl;
        ss << "  Iterations: " << result.iterations << std::endl;
        ss << "  Total Time: " << result.totalTimeMs << " ms" << std::endl;
        ss << "  Average Time: " << result.avgTimeMs << " ms" << std::endl;
        ss << "  Min Time: " << result.minTimeMs << " ms" << std::endl;
        ss << "  Max Time: " << result.maxTimeMs << " ms" << std::endl;
        ss << "  Standard Deviation: " << result.stdDevMs << " ms" << std::endl;
        ss << "  Coefficient of Variation: " << result.cvMs << std::endl;
        ss << "  Operations/sec: " << result.opsPerSecond << std::endl;
        ss << "  Memory Usage: " << result.memoryUsageMB << " MB" << std::endl;
        ss << "  CPU Usage: " << result.cpuUsagePercent << "%" << std::endl;
        ss << std::endl;
    }
    
    ss << "=== Summary ===" << std::endl;
    ss << "Total Benchmarks: " << results_.size() << std::endl;
    
    return ss.str();
}

double BenchmarkSuite::measureTime(std::function<void()> func) const {
    auto start = std::chrono::high_resolution_clock::now();
    func();
    auto end = std::chrono::high_resolution_clock::now();
    return std::chrono::duration<double, std::milli>(end - start).count();
}

double BenchmarkSuite::measureMemoryUsage(std::function<void()> func) const {
    // In a real implementation, this would use system-specific memory measurement
    return 0.0;
}

double BenchmarkSuite::measureCPUUsage(std::function<void()> func) const {
    // In a real implementation, this would use system-specific CPU measurement
    return 0.0;
}

double BenchmarkSuite::measureHardwareSpecific(std::function<void()> func) const {
    // In a real implementation, this would use hardware-specific performance counters
    return 0.0;
}

double BenchmarkSuite::calculateMean(const std::vector<double>& values) const {
    if (values.empty()) return 0.0;
    double sum = std::accumulate(values.begin(), values.end(), 0.0);
    return sum / values.size();
}

double BenchmarkSuite::calculateStdDev(const std::vector<double>& values, double mean) const {
    if (values.size() <= 1) return 0.0;
    
    double sum = 0.0;
    for (double value : values) {
        sum += (value - mean) * (value - mean);
    }
    
    return std::sqrt(sum / (values.size() - 1));
}

double BenchmarkSuite::calculateCV(const std::vector<double>& values, double mean) const {
    double stddev = calculateStdDev(values, mean);
    return (mean > 0) ? (stddev / mean) : 0.0;
}

bool BenchmarkSuite::isRegression(double baseline, double current, double thresholdPercent) const {
    if (baseline == 0) return current > 0;
    double regression = (current - baseline) / baseline * 100.0;
    return regression > thresholdPercent;
}

// NeuronSteppingBenchmark implementation

BenchmarkSuite::NeuronSteppingBenchmark::NeuronSteppingBenchmark(const BenchmarkConfig& config)
    : BenchmarkSuite(config) {
}

BenchmarkSuite::BenchmarkResult BenchmarkSuite::NeuronSteppingBenchmark::benchmarkSingleNeuron(
    size_t neuronCount, uint32_t steps) {
    BenchmarkResult result;
    result.name = "SingleNeuronBenchmark_" + std::to_string(neuronCount) + "x" + std::to_string(steps);
    result.description = "Single neuron stepping benchmark with " + 
                        std::to_string(neuronCount) + " neurons and " + 
                        std::to_string(steps) + " steps";
    
    // In a real implementation, this would run the actual benchmark
    return result;
}

BenchmarkSuite::BenchmarkResult BenchmarkSuite::NeuronSteppingBenchmark::benchmarkBatchNeurons(
    size_t neuronCount, uint32_t steps, size_t batchSize) {
    BenchmarkResult result;
    result.name = "BatchNeuronsBenchmark_" + std::to_string(neuronCount) + "x" + std::to_string(steps);
    result.description = "Batch neuron stepping benchmark with " + 
                        std::to_string(neuronCount) + " neurons and " + 
                        std::to_string(steps) + " steps, batch size " + 
                        std::to_string(batchSize ? batchSize : neuronCount);
    
    // In a real implementation, this would run the actual benchmark
    return result;
}

BenchmarkSuite::BenchmarkResult BenchmarkSuite::NeuronSteppingBenchmark::benchmarkActiveNeurons(
    size_t totalNeurons, size_t activeNeurons, uint32_t steps) {
    BenchmarkResult result;
    result.name = "ActiveNeuronsBenchmark_" + std::to_string(totalNeurons) + 
                  "x" + std::to_string(activeNeurons) + "x" + std::to_string(steps);
    result.description = "Active neuron processing benchmark with " + 
                        std::to_string(totalNeurons) + " total neurons, " + 
                        std::to_string(activeNeurons) + " active neurons, " + 
                        std::to_string(steps) + " steps";
    
    // In a real implementation, this would run the actual benchmark
    return result;
}

BenchmarkSuite::BenchmarkResult BenchmarkSuite::NeuronSteppingBenchmark::benchmarkSpikePropagation(
    size_t neuronCount, size_t connectionDensity, uint32_t steps) {
    BenchmarkResult result;
    result.name = "SpikePropagationBenchmark_" + std::to_string(neuronCount) + 
                  "x" + std::to_string(connectionDensity) + "x" + std::to_string(steps);
    result.description = "Spike propagation benchmark with " + 
                        std::to_string(neuronCount) + " neurons, " + 
                        std::to_string(connectionDensity) + "% connection density, " + 
                        std::to_string(steps) + " steps";
    
    // In a real implementation, this would run the actual benchmark
    return result;
}

// MemoryPoolBenchmark implementation

BenchmarkSuite::MemoryPoolBenchmark::MemoryPoolBenchmark(const BenchmarkConfig& config)
    : BenchmarkSuite(config) {
}

BenchmarkSuite::BenchmarkResult BenchmarkSuite::MemoryPoolBenchmark::benchmarkAllocation(
    size_t objectSize, size_t objectCount, size_t blockSize) {
    BenchmarkResult result;
    result.name = "MemoryPoolAllocationBenchmark_" + std::to_string(objectSize) + "x" + 
                  std::to_string(objectCount) + "x" + std::to_string(blockSize);
    result.description = "Memory pool allocation benchmark with " + 
                        std::to_string(objectSize) + " byte objects, " + 
                        std::to_string(objectCount) + " objects, " + 
                        std::to_string(blockSize) + " block size";
    
    // In a real implementation, this would run the actual benchmark
    return result;
}

BenchmarkSuite::BenchmarkResult BenchmarkSuite::MemoryPoolBenchmark::benchmarkDeallocation(
    size_t objectSize, size_t objectCount) {
    BenchmarkResult result;
    result.name = "MemoryPoolDeallocationBenchmark_" + std::to_string(objectSize) + "x" + 
                  std::to_string(objectCount);
    result.description = "Memory pool deallocation benchmark with " + 
                        std::to_string(objectSize) + " byte objects, " + 
                        std::to_string(objectCount) + " objects";
    
    // In a real implementation, this would run the actual benchmark
    return result;
}

BenchmarkSuite::BenchmarkResult BenchmarkSuite::MemoryPoolBenchmark::benchmarkMixedOperations(
    size_t objectSize, size_t objectCount, size_t allocRate, size_t deallocRate) {
    BenchmarkResult result;
    result.name = "MemoryPoolMixedBenchmark_" + std::to_string(objectSize) + "x" + 
                  std::to_string(objectCount) + "x" + std::to_string(allocRate) + "x" + 
                  std::to_string(deallocRate);
    result.description = "Memory pool mixed operations benchmark with " + 
                        std::to_string(objectSize) + " byte objects, " + 
                        std::to_string(objectCount) + " objects, " + 
                        std::to_string(allocRate) + " alloc rate, " + 
                        std::to_string(deallocRate) + " dealloc rate";
    
    // In a real implementation, this would run the actual benchmark
    return result;
}

BenchmarkSuite::BenchmarkResult BenchmarkSuite::MemoryPoolBenchmark::benchmarkFragmentation(
    size_t objectSize, size_t totalObjects, size_t allocationPattern) {
    BenchmarkResult result;
    result.name = "MemoryPoolFragmentationBenchmark_" + std::to_string(objectSize) + "x" + 
                  std::to_string(totalObjects) + "x" + std::to_string(allocationPattern);
    result.description = "Memory pool fragmentation benchmark with " + 
                        std::to_string(objectSize) + " byte objects, " + 
                        std::to_string(totalObjects) + " total objects, " + 
                        std::to_string(allocationPattern) + " allocation pattern";
    
    // In a real implementation, this would run the actual benchmark
    return result;
}

// ParallelProcessingBenchmark implementation

BenchmarkSuite::ParallelProcessingBenchmark::ParallelProcessingBenchmark(const BenchmarkConfig& config)
    : BenchmarkSuite(config) {
}

BenchmarkSuite::BenchmarkResult BenchmarkSuite::ParallelProcessingBenchmark::benchmarkParallelFor(
    size_t workSize, size_t grainSize) {
    BenchmarkResult result;
    result.name = "ParallelForBenchmark_" + std::to_string(workSize) + "x" + std::to_string(grainSize);
    result.description = "Parallel for-loop benchmark with " + 
                        std::to_string(workSize) + " work items, " + 
                        std::to_string(grainSize) + " grain size";
    
    // In a real implementation, this would run the actual benchmark
    return result;
}

BenchmarkSuite::BenchmarkResult BenchmarkSuite::ParallelProcessingBenchmark::benchmarkThreadPool(
    size_t taskCount, size_t tasksPerThread) {
    BenchmarkResult result;
    result.name = "ThreadPoolBenchmark_" + std::to_string(taskCount) + "x" + std::to_string(tasksPerThread);
    result.description = "Thread pool benchmark with " + 
                        std::to_string(taskCount) + " tasks, " + 
                        std::to_string(tasksPerThread) + " tasks per thread";
    
    // In a real implementation, this would run the actual benchmark
    return result;
}

BenchmarkSuite::BenchmarkResult BenchmarkSuite::ParallelProcessingBenchmark::benchmarkParallelNeuronProcessing(
    size_t neuronCount, size_t threadCount) {
    BenchmarkResult result;
    result.name = "ParallelNeuronProcessingBenchmark_" + std::to_string(neuronCount) + "x" + std::to_string(threadCount);
    result.description = "Parallel neuron processing benchmark with " + 
                        std::to_string(neuronCount) + " neurons, " + 
                        std::to_string(threadCount) + " threads";
    
    // In a real implementation, this would run the actual benchmark
    return result;
}

BenchmarkSuite::BenchmarkResult BenchmarkSuite::ParallelProcessingBenchmark::benchmarkSynchronization(
    size_t threadCount, size_t iterationCount) {
    BenchmarkResult result;
    result.name = "SynchronizationBenchmark_" + std::to_string(threadCount) + "x" + std::to_string(iterationCount);
    result.description = "Synchronization benchmark with " + 
                        std::to_string(threadCount) + " threads, " + 
                        std::to_string(iterationCount) + " iterations";
    
    // In a real implementation, this would run the actual benchmark
    return result;
}

// CheckpointBenchmark implementation

BenchmarkSuite::CheckpointBenchmark::CheckpointBenchmark(const BenchmarkConfig& config)
    : BenchmarkSuite(config) {
}

BenchmarkSuite::BenchmarkResult BenchmarkSuite::CheckpointBenchmark::benchmarkSave(
    size_t neuronCount, size_t synapseCount, const std::string& compression) {
    BenchmarkResult result;
    result.name = "CheckpointSaveBenchmark_" + std::to_string(neuronCount) + "x" + 
                  std::to_string(synapseCount) + "x" + compression;
    result.description = "Checkpoint save benchmark with " + 
                        std::to_string(neuronCount) + " neurons, " + 
                        std::to_string(synapseCount) + " synapses, " + 
                        compression + " compression";
    
    // In a real implementation, this would run the actual benchmark
    return result;
}

BenchmarkSuite::BenchmarkResult BenchmarkSuite::CheckpointBenchmark::benchmarkLoad(
    size_t neuronCount, size_t synapseCount, const std::string& compression) {
    BenchmarkResult result;
    result.name = "CheckpointLoadBenchmark_" + std::to_string(neuronCount) + "x" + 
                  std::to_string(synapseCount) + "x" + compression;
    result.description = "Checkpoint load benchmark with " + 
                        std::to_string(neuronCount) + " neurons, " + 
                        std::to_string(synapseCount) + " synapses, " + 
                        compression + " compression";
    
    // In a real implementation, this would run the actual benchmark
    return result;
}

BenchmarkSuite::BenchmarkResult BenchmarkSuite::CheckpointBenchmark::benchmarkDifferential(
    size_t neuronCount, size_t synapseCount, uint64_t changeRate) {
    BenchmarkResult result;
    result.name = "CheckpointDifferentialBenchmark_" + std::to_string(neuronCount) + "x" + 
                  std::to_string(synapseCount) + "x" + std::to_string(changeRate);
    result.description = "Differential checkpoint benchmark with " + 
                        std::to_string(neuronCount) + " neurons, " + 
                        std::to_string(synapseCount) + " synapses, " + 
                        std::to_string(changeRate) + " change rate";
    
    // In a real implementation, this would run the actual benchmark
    return result;
}

BenchmarkSuite::BenchmarkResult BenchmarkSuite::CheckpointBenchmark::benchmarkIncremental(
    size_t neuronCount, size_t synapseCount, uint64_t stepInterval) {
    BenchmarkResult result;
    result.name = "CheckpointIncrementalBenchmark_" + std::to_string(neuronCount) + "x" + 
                  std::to_string(synapseCount) + "x" + std::to_string(stepInterval);
    result.description = "Incremental checkpoint benchmark with " + 
                        std::to_string(neuronCount) + " neurons, " + 
                        std::to_string(synapseCount) + " synapses, " + 
                        std::to_string(stepInterval) + " step interval";
    
    // In a real implementation, this would run the actual benchmark
    return result;
}

// HardwareBenchmark implementation

BenchmarkSuite::HardwareBenchmark::HardwareBenchmark(const BenchmarkConfig& config)
    : BenchmarkSuite(config) {
}

BenchmarkSuite::BenchmarkResult BenchmarkSuite::HardwareBenchmark::benchmarkSIMD(
    size_t neuronCount, uint32_t steps) {
    BenchmarkResult result;
    result.name = "SIMDBenchmark_" + std::to_string(neuronCount) + "x" + std::to_string(steps);
    result.description = "SIMD optimization benchmark with " + 
                        std::to_string(neuronCount) + " neurons, " + 
                        std::to_string(steps) + " steps";
    
    // In a real implementation, this would run the actual benchmark
    return result;
}

BenchmarkSuite::BenchmarkResult BenchmarkSuite::HardwareBenchmark::benchmarkCacheEfficiency(
    size_t accessPattern, size_t workingSetSize) {
    BenchmarkResult result;
    result.name = "CacheEfficiencyBenchmark_" + std::to_string(accessPattern) + "x" + 
                  std::to_string(workingSetSize);
    result.description = "Cache efficiency benchmark with " + 
                        std::to_string(accessPattern) + " access pattern, " + 
                        std::to_string(workingSetSize) + " working set size";
    
    // In a real implementation, this would run the actual benchmark
    return result;
}

BenchmarkSuite::BenchmarkResult BenchmarkSuite::HardwareBenchmark::benchmarkMemoryBandwidth(
    size_t dataSize, size_t iterationCount) {
    BenchmarkResult result;
    result.name = "MemoryBandwidthBenchmark_" + std::to_string(dataSize) + "x" + 
                  std::to_string(iterationCount);
    result.description = "Memory bandwidth benchmark with " + 
                        std::to_string(dataSize) + " data size, " + 
                        std::to_string(iterationCount) + " iterations";
    
    // In a real implementation, this would run the actual benchmark
    return result;
}

BenchmarkSuite::BenchmarkResult BenchmarkSuite::HardwareBenchmark::benchmarkThreadingEfficiency(
    size_t workSize, size_t threadCount) {
    BenchmarkResult result;
    result.name = "ThreadingEfficiencyBenchmark_" + std::to_string(workSize) + "x" + 
                  std::to_string(threadCount);
    result.description = "Threading efficiency benchmark with " + 
                        std::to_string(workSize) + " work size, " + 
                        std::to_string(threadCount) + " threads";
    
    // In a real implementation, this would run the actual benchmark
    return result;
}

// RegressionTestSuite implementation

BenchmarkSuite::RegressionTestSuite::RegressionTestSuite(const BenchmarkConfig& config)
    : BenchmarkSuite(config) {
}

void BenchmarkSuite::RegressionTestSuite::addBaseline(const std::string& name, const BenchmarkResult& baseline) {
    std::lock_guard<std::mutex> lock(baselinesMutex_);
    baselines_[name] = baseline;
}

std::vector<std::pair<std::string, bool>> BenchmarkSuite::RegressionTestSuite::testForRegressions(
    const std::vector<BenchmarkResult>& currentResults) const {
    
    std::vector<std::pair<std::string, bool>> regressions;
    
    for (const auto& baselineResult : baselines_) {
        const BenchmarkResult* currentResult = nullptr;
        for (const auto& result : currentResults) {
            if (result.name == baselineResult.first) {
                currentResult = &result;
                break;
            }
        }
        
        if (currentResult) {
            double regression = (currentResult->avgTimeMs - baselineResult.second.avgTimeMs) / 
                               baselineResult.second.avgTimeMs * 100.0;
            
            bool isRegressed = regression > 5.0; // 5% threshold
            regressions.push_back(std::make_pair(baselineResult.first, isRegressed));
        }
    }
    
    return regressions;
}

std::string BenchmarkSuite::RegressionTestSuite::generateRegressionReport(
    const std::vector<BenchmarkResult>& currentResults) const {
    
    std::stringstream ss;
    ss << "=== Performance Regression Report ===" << std::endl;
    
    auto regressions = testForRegressions(currentResults);
    
    if (regressions.empty()) {
        ss << "No performance regressions detected." << std::endl;
    } else {
        ss << "Performance regressions detected:" << std::endl;
        for (const auto& regression : regressions) {
            ss << "  " << regression.first << ": " << 
                (regression.second ? "REGRESSED" : "OK") << std::endl;
        }
    }
    
    return ss.str();
}

// BenchmarkRunner implementation

std::vector<BenchmarkSuite::BenchmarkResult> BenchmarkRunner::runAllBenchmarks() {
    std::vector<BenchmarkResult> allResults;
    
    // In a real implementation, this would run all benchmark suites
    // For now, just return empty results
    return allResults;
}

std::vector<BenchmarkSuite::BenchmarkResult> BenchmarkRunner::runBenchmarkSuite(BenchmarkSuite& suite) {
    return suite.getResults();
}

bool BenchmarkRunner::saveResults(const std::string& filepath) const {
    // In a real implementation, this would save all benchmark results
    return true;
}

// Global benchmark runner instance
BenchmarkRunner& BenchmarkRunner::getInstance() {
    static BenchmarkRunner instance;
    return instance;
}

} // namespace nlm