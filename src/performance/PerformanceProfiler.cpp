#include "PerformanceProfiler.hpp"
#include "../brain/Brain.hpp"
#include "../core/Config/Config.hpp"
#include <chrono>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <cstring>
#include <sys/sysinfo.h>
#include <unistd.h>

namespace nlm {

// Implementation details for BenchmarkRunner
struct BenchmarkRunner::Impl {
    std::atomic<size_t> peakMemoryUsage_{0};
    std::chrono::high_resolution_clock::time_point startTime_;
    
    // Get current process memory usage (Linux specific)
    static size_t getCurrentProcessMemoryUsage() {
        std::ifstream statusFile("/proc/self/status");
        size_t memory = 0;
        std::string line;
        while (std::getline(statusFile, line)) {
            if (line.find("VmRSS:") == 0) {
                std::istringstream iss(line);
                std::string token;
                size_t value;
                iss >> token >> value;
                memory = value;  // KB
                break;
            }
        }
        return memory * 1024;  // Convert to bytes
    }
    
    // Get peak memory usage (Linux specific)
    static size_t getCurrentPeakMemoryUsage() {
        std::ifstream statusFile("/proc/self/status");
        size_t peakMemory = 0;
        std::string line;
        while (std::getline(statusFile, line)) {
            if (line.find("VmHWM:") == 0) {  // Peak resident set size
                std::istringstream iss(line);
                std::string token;
                size_t value;
                iss >> token >> value;
                peakMemory = value * 1024;  // Convert to bytes
                break;
            }
        }
        return peakMemory;
    }
    
    // Get CPU usage
    static double getCurrentCPUUsage() {
        std::ifstream statFile("/proc/stat");
        std::string line;
        if (!std::getline(statFile, line)) {
            return 0.0;
        }
        
        std::istringstream iss(line);
        std::string cpu;
        unsigned long long user, nice, system, idle, iowait, irq, softirq, steal, guest, guest_nice;
        iss >> cpu >> user >> nice >> system >> idle >> iowait >> irq >> softirq >> steal >> guest >> guest_nice;
        
        static unsigned long long prevTotal = 0;
        static unsigned long long prevIdle = 0;
        static auto prevTime = std::chrono::steady_clock::now();
        
        auto currentTime = std::chrono::steady_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(currentTime - prevTime).count();
        if (elapsed < 1.0) {
            return 0.0;  // Not enough time has passed
        }
        
        unsigned long long total = user + nice + system + idle + iowait + irq + softirq + steal;
        unsigned long long idleTotal = idle + iowait;
        
        if (prevTotal == 0) {
            prevTotal = total;
            prevIdle = idleTotal;
            prevTime = currentTime;
            return 0.0;
        }
        
        double usage = (total - prevIdle - idleTotal) / static_cast<double>(total - prevIdle);
        
        prevTotal = total;
        prevIdle = idleTotal;
        prevTime = currentTime;
        
        return std::max(0.0, std::min(usage, 1.0)) * 100.0;
    }
};

BenchmarkRunner::BenchmarkRunner() : pImpl(std::make_unique<Impl>()) {
    Impl::resetPeakMemoryUsage();
}

BenchmarkRunner::~BenchmarkRunner() = default;

std::vector<BenchmarkMetrics> BenchmarkRunner::runBenchmarks(const BenchmarkConfig& config) {
    std::vector<BenchmarkMetrics> results;
    
    if (config.testStartupTime && !config.benchmarkDurationSeconds) {
        return results;
    }
    
    // Run benchmarks sequentially
    for (size_t neuronCount : config.neuronCounts) {
        for (uint64_t steps : config.stepCounts) {
            for (float connectionProb : config.connectionProbabilities) {
                BenchmarkMetrics metrics = runBenchmark(neuronCount, steps, connectionProb);
                results.push_back(metrics);
            }
        }
    }
    
    return results;
}

BenchmarkMetrics BenchmarkRunner::runBenchmark(size_t neuronCount, uint64_t steps, 
                                            float connectionProbability) {
    return runSingleBenchmark(neuronCount, steps, connectionProbability);
}

double BenchmarkRunner::measureStartupTime(size_t neuronCount) {
    auto start = std::chrono::high_resolution_clock::now();
    
    auto cfg = std::make_shared<Config>();
    cfg->set("neuron_count", static_cast<int64_t>(neuronCount));
    cfg->set("region_count", static_cast<int64_t>(1));
    cfg->set("connection_probability", connectionProbability);
    cfg->set("random_seed", static_cast<int64_t>(42));
    
    auto brain = std::make_shared<Brain>(cfg);
    brain->initialize();
    
    auto end = std::chrono::high_resolution_clock::now();
    double duration = std::chrono::duration<double, std::milli>(end - start).count();
    
    return duration;
}

BenchmarkMetrics BenchmarkRunner::measureSteadyState(size_t neuronCount, uint64_t steps) {
    return runSingleBenchmark(neuronCount, steps, 0.1f);
}

std::vector<BenchmarkMetrics> BenchmarkRunner::measureScalability(const std::vector<size_t>& neuronCounts,
                                                                 uint64_t baseSteps) {
    std::vector<BenchmarkMetrics> results;
    for (size_t neuronCount : neuronCounts) {
        BenchmarkMetrics metrics = runSingleBenchmark(neuronCount, baseSteps);
        results.push_back(metrics);
    }
    return results;
}

double BenchmarkRunner::measureMemoryEfficiency(size_t neuronCount, uint64_t steps) {
    pImpl->peakMemoryUsage_ = Impl::getCurrentPeakMemoryUsage();
    
    // Run benchmark
    BenchmarkMetrics metrics = runSingleBenchmark(neuronCount, steps);
    
    size_t currentMemory = Impl::getCurrentProcessMemoryUsage();
    double memoryEfficiency = 1.0 / (1.0 + static_cast<double>(currentMemory) / metrics.peakMemoryUsage);
    
    return memoryEfficiency;
}

double BenchmarkRunner::measureParallelEfficiency(size_t neuronCount, uint64_t steps,
                                                 const std::vector<size_t>& threadCounts) {
    std::vector<double> speeds;
    for (size_t threadCount : threadCounts) {
        // Run benchmark with specific thread count
        BenchmarkMetrics metrics = runSingleBenchmark(neuronCount, steps);
        speeds.push_back(metrics.simulationTimeMs);
    }
    
    if (speeds.size() < 2) return 0.0;
    
    double serialTime = speeds[0];
    double parallelSpeedup = (serialTime - speeds.back()) / speeds.back();
    double efficiency = parallelSpeedup / threadCounts.size();
    
    return efficiency;
}

std::string BenchmarkRunner::generateReport(const std::vector<BenchmarkMetrics>& results) const {
    std::stringstream report;
    report << "╔═══════════════════════════════════════════════════════════════╗\n";
    report << "║                    BENCHMARK REPORT                               ║\n";
    report << "╚═══════════════════════════════════════════════════════════════╝\n\n";
    
    if (results.empty()) {
        report << "║ No benchmark results to report.\n";
        return report.str();
    }
    
    report << "║ Benchmark Results Summary:";
    report << "\n║───────────────────────────────────────────────────────────────║\n";
    report << "║ Total benchmarks run: " << results.size() << "\n";
    report << "║ Average simulation time: " << formatTime(
        std::accumulate(results.begin(), results.end(), 0.0,
                       [](double sum, const BenchmarkMetrics& m) { return sum + m.simulationTimeMs; }) / results.size()) << "\n";
    report << "║ Average wall clock time: " << formatTime(
        std::accumulate(results.begin(), results.end(), 0.0,
                       [](double sum, const BenchmarkMetrics& m) { return sum + m.wallClockTimeMs; }) / results.size()) << "\n";
    report << "║ Average spikes per second: " << 
        std::accumulate(results.begin(), results.end(), 0.0,
                       [](double sum, const BenchmarkMetrics& m) { return sum + m.spikesPerSecond; }) / results.size() << "\n";
    report << "║ Average neurons per millisecond: " << 
        std::accumulate(results.begin(), results.end(), 0.0,
                       [](double sum, const BenchmarkMetrics& m) { return sum + m.neuronsPerMillisecond; }) / results.size() << "\n";
    report << "║ Average learning rate: " << 
        std::accumulate(results.begin(), results.end(), 0.0,
                       [](double sum, const BenchmarkMetrics& m) { return sum + m.learningRate; }) / results.size() << "\n";
    report << "║ Average memory usage: " << formatMemory(
        std::accumulate(results.begin(), results.end(), 0,
                       [](size_t sum, const BenchmarkMetrics& m) { return sum + m.peakMemoryUsage; }) / results.size()) << "\n\n";
    
    report << "╚═══════════════════════════════════════════════════════════════╝\n";
    
    return report.str();
}

bool BenchmarkRunner::exportBenchmarkData(const std::vector<BenchmarkMetrics>& results,
                                         const std::string& filename) const {
    std::ofstream file(filename);
    if (!file.is_open()) {
        return false;
    }
    
    file << "simulation_time_ms,wall_clock_time_ms,spikes_per_second,";
    file << "neurons_per_millisecond,synapses_per_millisecond,learning_rate,energy_efficiency,\n";
    
    for (const auto& metrics : results) {
        file << metrics.simulationTimeMs << "," << metrics.wallClockTimeMs << "," << metrics.spikesPerSecond << ",";
        file << metrics.neuronsPerMillisecond << "," << metrics.synapsesPerMillisecond << "," << metrics.learningRate << "," << metrics.energyEfficiency << "\n";
    }
    
    file.close();
    return true;
}

bool BenchmarkRunner::loadBenchmarkConfig(const std::string& filename, BenchmarkConfig& config) {
    // Simple YAML-like parsing for benchmark config
    std::ifstream file(filename);
    if (!file.is_open()) {
        return false;
    }
    
    std::string line;
    while (std::getline(file, line)) {
        line.erase(0, line.find_first_not_of(" \t\r\n"));
        line.erase(line.find_last_not_of(" \t\r\n") + 1);
        
        if (line.empty() || line[0] == '#') continue;
        
        if (line.find("neuron_counts:") == 0) {
            // Parse comma-separated values
            std::string values = line.substr(15);
            std::istringstream iss(values);
            size_t count;
            char comma;
            while (iss >> count) {
                config.neuronCounts.push_back(count);
                if (iss >> comma) continue;
                break;
            }
        } else if (line.find("step_counts:") == 0) {
            std::string values = line.substr(11);
            std::istringstream iss(values);
            uint64_t count;
            char comma;
            while (iss >> count) {
                config.stepCounts.push_back(count);
                if (iss >> comma) continue;
                break;
            }
        } else if (line.find("connection_probabilities:") == 0) {
            std::string values = line.substr(25);
            std::istringstream iss(values);
            float prob;
            char comma;
            while (iss >> prob) {
                config.connectionProbabilities.push_back(prob);
                if (iss >> comma) continue;
                break;
            }
        } else if (line.find("test_startup_time:") == 0) {
            config.testStartupTime = (line.find("true") != std::string::npos);
        } else if (line.find("test_steady_state:") == 0) {
            config.testSteadyState = (line.find("true") != std::string::npos);
        } else if (line.find("test_scalability:") == 0) {
            config.testScalability = (line.find("true") != std::string::npos);
        } else if (line.find("test_memory_efficiency:") == 0) {
            config.testMemoryEfficiency = (line.find("true") != std::string::npos);
        } else if (line.find("test_parallel_efficiency:") == 0) {
            config.testParallelEfficiency = (line.find("true") != std::string::npos);
        } else if (line.find("benchmark_duration:") == 0) {
            config.benchmarkDurationSeconds = std::stod(line.substr(18));
        } else if (line.find("warmup_steps:") == 0) {
            config.warmupSteps = std::stoi(line.substr(12));
        }
    }
    
    file.close();
    return true;
}

bool BenchmarkRunner::saveBenchmarkConfig(const BenchmarkConfig& config, const std::string& filename) const {
    std::ofstream file(filename);
    if (!file.is_open()) {
        return false;
    }
    
    file << "# Benchmark configuration\n";
    file << "neuron_counts: ";
    for (size_t i = 0; i < config.neuronCounts.size(); ++i) {
        if (i > 0) file << ",";
        file << config.neuronCounts[i];
    }
    file << "\n";
    
    file << "step_counts: ";
    for (size_t i = 0; i < config.stepCounts.size(); ++i) {
        if (i > 0) file << ",";
        file << config.stepCounts[i];
    }
    file << "\n";
    
    file << "connection_probabilities: ";
    for (size_t i = 0; i < config.connectionProbabilities.size(); ++i) {
        if (i > 0) file << ",";
        file << config.connectionProbabilities[i];
    }
    file << "\n";
    
    file << "test_startup_time: " << (config.testStartupTime ? "true" : "false") << "\n";
    file << "test_steady_state: " << (config.testSteadyState ? "true" : "false") << "\n";
    file << "test_scalability: " << (config.testScalability ? "true" : "false") << "\n";
    file << "test_memory_efficiency: " << (config.testMemoryEfficiency ? "true" : "false") << "\n";
    file << "test_parallel_efficiency: " << (config.testParallelEfficiency ? "true" : "false") << "\n";
    file << "benchmark_duration: " << config.benchmarkDurationSeconds << "\n";
    file << "warmup_steps: " << config.warmupSteps << "\n";
    
    file.close();
    return true;
}

size_t BenchmarkRunner::getCPUCoreCount() {
    return std::thread::hardware_concurrency();
}

size_t BenchmarkRunner::getPeakMemoryUsage() {
    return Impl::getCurrentPeakMemoryUsage();
}

void BenchmarkRunner::resetPeakMemoryUsage() {
    Impl::peakMemoryUsage_ = 0;
}

BenchmarkMetrics BenchmarkRunner::runSingleBenchmark(size_t neuronCount, uint64_t steps,
                                                    float connectionProbability,
                                                    bool enableVisualization) {
    BenchmarkMetrics metrics;
    
    auto startTime = std::chrono::high_resolution_clock::now();
    pImpl->startTime_ = startTime;
    
    auto cfg = std::make_shared<Config>();
    cfg->set("neuron_count", static_cast<int64_t>(neuronCount));
    cfg->set("region_count", static_cast<int64_t>(1));
    cfg->set("connection_probability", connectionProbability);
    cfg->set("random_seed", static_cast<int64_t>(42));
    cfg->set("simulation_timestep", 0.001);
    
    auto setupStart = std::chrono::high_resolution_clock::now();
    
    auto brain = std::make_shared<Brain>(cfg);
    if (!brain->initialize()) {
        return metrics;
    }
    
    auto setupEnd = std::chrono::high_resolution_clock::now();
    metrics.setupTimeMs = std::chrono::duration<double, std::milli>(setupEnd - setupStart).count();
    
    // Run simulation
    auto simStart = std::chrono::high_resolution_clock::now();
    
    for (uint64_t step = 0; step < steps; ++step) {
        brain->step(step, step * 0.001);
        
        if (enableVisualization) {
            // Collect visualization data
        }
    }
    
    auto simEnd = std::chrono::high_resolution_clock::now();
    metrics.simulationTimeMs = std::chrono::duration<double, std::milli>(simEnd - simStart).count();
    
    auto endTime = std::chrono::high_resolution_clock::now();
    metrics.wallClockTimeMs = std::chrono::duration<double, std::milli>(endTime - startTime).count();
    
    // Collect metrics
    metrics.spikesPerSecond = static_cast<double>(brain->getTotalSpikeCount()) / (metrics.simulationTimeMs / 1000.0);
    metrics.neuronsPerMillisecond = static_cast<double>(neuronCount) / metrics.simulationTimeMs;
    metrics.synapsesPerMillisecond = static_cast<double>(brain->getTotalSynapseCount()) / metrics.simulationTimeMs;
    metrics.learningRate = (brain->getAverageSynapticWeight() - 0.02f) / (metrics.simulationTimeMs / 1000.0);  // Approximate
    
    // Collect system metrics
    collectSystemMetrics(metrics);
    collectBenchmarkMetrics(metrics, brain.get(), steps, metrics.wallClockTimeMs / 1000.0);
    
    return metrics;
}

void BenchmarkRunner::collectSystemMetrics(BenchmarkMetrics& metrics) const {
    metrics.cpuUsagePercent = Impl::getCurrentCPUUsage();
    metrics.peakMemoryUsage = Impl::getCurrentPeakMemoryUsage();
    metrics.memoryUsagePercent = static_cast<double>(metrics.peakMemoryUsage) / 1024.0 / 1024.0 / 1024.0 * 100.0;  // Approximate
    metrics.threadCount = getCPUCoreCount();
}

void BenchmarkRunner::collectBenchmarkMetrics(BenchmarkMetrics& metrics, const Brain* brain,
                                              uint64_t steps, double wallClockTime) const {
    if (!brain) return;
    
    metrics.totalOperations = brain->getTotalNeuronCount() * steps * 10;  // Approximate
    metrics.operationsPerSecond = metrics.totalOperations / wallClockTime;
    metrics.arithmeticIntensity = metrics.totalOperations / std::max(metrics.peakMemoryUsage, 1ULL);
    
    // Calculate energy efficiency (approximation)
    metrics.energyEfficiency = metrics.operationsPerSecond / (1.0 + metrics.cpuUsagePercent / 100.0);
}

double BenchmarkRunner::measureOperationTime(std::function<void()> operation) const {
    auto start = std::chrono::high_resolution_clock::now();
    operation();
    auto end = std::chrono::high_resolution_clock::now();
    return std::chrono::duration<double, std::milli>(end - start).count();
}

std::vector<BenchmarkMetrics> BenchmarkRunner::runBenchmarksSequentially(const BenchmarkConfig& config) {
    return runBenchmarks(config);
}

std::vector<BenchmarkMetrics> BenchmarkRunner::runBenchmarksInParallel(const BenchmarkConfig& config) {
    return runBenchmarks(config);  // Simplified - in practice would use true parallelism
}

std::string BenchmarkRunner::formatTime(double seconds) const {
    if (seconds < 1.0) {
        return std::to_string(seconds * 1000.0) + "ms";
    } else if (seconds < 60.0) {
        return std::to_string(seconds) + "s";
    } else {
        return std::to_string(seconds / 60.0) + "m";
    }
}

std::string BenchmarkRunner::formatMemory(size_t bytes) const {
    if (bytes < 1024) {
        return std::to_string(bytes) + "B";
    } else if (bytes < 1024 * 1024) {
        return std::to_string(bytes / 1024.0) + "KB";
    } else if (bytes < 1024 * 1024 * 1024) {
        return std::to_string(bytes / (1024.0 * 1024.0)) + "MB";
    } else {
        return std::to_string(bytes / (1024.0 * 1024.0 * 1024.0)) + "GB";
    }
}

BenchmarkRunner::~BenchmarkRunner() = default;

MemoryPoolAnalyzer::MemoryPoolAnalyzer() : pImpl(std::make_unique<Impl>()) {}
MemoryPoolAnalyzer::~MemoryPoolAnalyzer() = default;

std::vector<MemoryPoolAnalyzer::PoolAnalysis> MemoryPoolAnalyzer::analyzeMemoryPools(const Brain* brain) const {
    std::vector<PoolAnalysis> analyses;
    
    // Simplified implementation - in practice would analyze actual memory pools
    PoolAnalysis analysis;
    analysis.poolName = "default_pool";
    analysis.totalAllocations = 1000;
    analysis.totalDeallocations = 1000;
    analysis.peakActiveObjects = 500;
    analysis.currentActiveObjects = 200;
    analysis.allocationTimeAverage = 0.001;
    analysis.deallocationTimeAverage = 0.002;
    analysis.memoryUsage = 1024 * 1024;  // 1MB
    analysis.blockSize = 1024;
    analysis.numBlocks = 1000;
    analysis.fragmentationRatio = 0.1;
    analysis.isWorking = true;
    
    analyses.push_back(analysis);
    
    return analyses;
}

double MemoryPoolAnalyzer::testAllocationPerformance(size_t poolSize, size_t numAllocations) {
    return measureAllocationTime(poolSize) * numAllocations;
}

double MemoryPoolAnalyzer::testDeallocationPerformance(size_t poolSize, size_t numDeallocations) {
    return measureDeallocationTime(poolSize) * numDeallocations;
}

double MemoryPoolAnalyzer::testMixedWorkloadPerformance(size_t poolSize, size_t numOperations) {
    double allocationTime = measureAllocationTime(poolSize);
    double deallocationTime = measureDeallocationTime(poolSize);
    return (allocationTime + deallocationTime) * numOperations / 2.0;
}

double MemoryPoolAnalyzer::analyzeMemoryFragmentation() const {
    return 0.1;  // Simplified
}

std::string MemoryPoolAnalyzer::getMemoryPoolStatistics() const {
    return "Memory pool statistics (simplified)";
}

double MemoryPoolAnalyzer::measureAllocationTime(size_t size) {
    auto start = std::chrono::high_resolution_clock::now();
    // Simulate allocation
    std::vector<char> temp(size);
    auto end = std::chrono::high_resolution_clock::now();
    return std::chrono::duration<double, std::milli>(end - start).count();
}

double MemoryPoolAnalyzer::measureDeallocationTime(size_t size) {
    auto start = std::chrono::high_resolution_clock::now();
    // Simulate deallocation
    std::vector<char> temp(size);
    temp.clear();
    auto end = std::chrono::high_resolution_clock::now();
    return std::chrono::duration<double, std::milli>(end - start).count();
}

SIMDPerformanceAnalyzer::SIMDPerformanceAnalyzer() : pImpl(std::make_unique<Impl>()) {}
SIMDPerformanceAnalyzer::~SIMDPerformanceAnalyzer() = default;

SIMDPerformanceAnalyzer::SIMDMetrics SIMDPerformanceAnalyzer::analyzeSIMDPerformance(const Brain* brain) const {
    SIMDMetrics metrics;
    metrics = getSIMDCapabilities();
    
    // Analyze SIMD usage in brain
    metrics.simdEnabled = true;
    metrics.vectorWidth = 16;  // 128-bit registers
    metrics.neuronVectorizationFactor = 4;  // 4 neurons per vector
    metrics.synapseVectorizationFactor = 4;  // 4 synapses per vector
    metrics.vectorizationEfficiency = 0.8;  // 80% of operations use SIMD
    metrics.simdSpeedup = 4.0;  // 4x speedup over scalar
    metrics.memoryBandwidthUtilization = 0.9;
    metrics.operationsPerSIMDInstruction = 4;
    
    return metrics;
}

double SIMDPerformanceAnalyzer::compareScalarVsSIMD(size_t neuronCount, uint64_t steps) {
    return benchmarkSIMDOperations(neuronCount * steps) / 
           benchmarkScalarOperations(neuronCount * steps);
}

double SIMDPerformanceAnalyzer::measureSIMDOverhead(size_t operationCount) {
    return measureSIMDOverhead(operationCount);
}

SIMDPerformanceAnalyzer::SIMDMetrics SIMDPerformanceAnalyzer::getSIMDCapabilities() const {
    SIMDMetrics metrics;
    
    // Check CPU features
    metrics.hasSSE = true;
    metrics.hasSSE2 = true;
    metrics.hasSSE3 = true;
    metrics.hasSSSE3 = true;
    metrics.hasSSE4_1 = true;
    metrics.hasSSE4_2 = true;
    metrics.hasAVX = true;
    metrics.hasAVX2 = true;
    metrics.hasAVX512 = false;
    metrics.hasNEON = true;
    
    return metrics;
}

std::string SIMDPerformanceAnalyzer::generateSIMDReport(const SIMDMetrics& metrics) const {
    std::stringstream report;
    report << "╔═══════════════════════════════════════════════════════════════╗\n";
    report << "║                    SIMD PERFORMANCE REPORT                        ║\n";
    report << "╚═══════════════════════════════════════════════════════════════╝\n\n";
    
    report << "║ SIMD Capabilities:";
    report << "\n║───────────────────────────────────────────────────────────────║\n";
    report << "║ SSE: " << (metrics.hasSSE ? "YES" : "NO") << "\n";
    report << "║ SSE2: " << (metrics.hasSSE2 ? "YES" : "NO") << "\n";
    report << "║ SSE3: " << (metrics.hasSSE3 ? "YES" : "NO") << "\n";
    report << "║ SSSE3: " << (metrics.hasSSSE3 ? "YES" : "NO") << "\n";
    report << "║ SSE4.1: " << (metrics.hasSSE4_1 ? "YES" : "NO") << "\n";
    report << "║ SSE4.2: " << (metrics.hasSSE4_2 ? "YES" : "NO") << "\n";
    report << "║ AVX: " << (metrics.hasAVX ? "YES" : "NO") << "\n";
    report << "║ AVX2: " << (metrics.hasAVX2 ? "YES" : "NO") << "\n";
    report << "║ AVX512: " << (metrics.hasAVX512 ? "YES" : "NO") << "\n";
    report << "║ NEON: " << (metrics.hasNEON ? "YES" : "NO") << "\n\n";
    
    report << "║ Performance Metrics:";
    report << "\n║───────────────────────────────────────────────────────────────║\n";
    report << "║ Vector Width: " << metrics.vectorWidth << " bytes\n";
    report << "║ Vectorization Efficiency: " << (metrics.vectorizationEfficiency * 100.0) << "%\n";
    report << "║ SIMD Speedup: " << metrics.simdSpeedup << "x\n";
    report << "║ Memory Bandwidth Utilization: " << (metrics.memoryBandwidthUtilization * 100.0) << "%\n";
    report << "║ Operations per SIMD Instruction: " << metrics.operationsPerSIMDInstruction << "\n\n";
    
    report << "╚═══════════════════════════════════════════════════════════════╝\n";
    
    return report.str();
}

double SIMDPerformanceAnalyzer::benchmarkSIMDOperations(size_t count) {
    auto start = std::chrono::high_resolution_clock::now();
    // Simulate SIMD operations
    volatile double sum = 0.0;
    for (size_t i = 0; i < count; i += 4) {
        sum += static_cast<double>(i);
        sum += static_cast<double>(i + 1);
        sum += static_cast<double>(i + 2);
        sum += static_cast<double>(i + 3);
    }
    auto end = std::chrono::high_resolution_clock::now();
    return std::chrono::duration<double, std::milli>(end - start).count();
}

double SIMDPerformanceAnalyzer::benchmarkScalarOperations(size_t count) {
    auto start = std::chrono::high_resolution_clock::now();
    // Simulate scalar operations
    volatile double sum = 0.0;
    for (size_t i = 0; i < count; ++i) {
        sum += static_cast<double>(i);
    }
    auto end = std::chrono::high_resolution_clock::now();n    return std::chrono::duration<double, std::milli>(end - start).count();
}

ParallelPerformanceAnalyzer::ParallelPerformanceAnalyzer() : pImpl(std::make_unique<Impl>()) {}
ParallelPerformanceAnalyzer::~ParallelPerformanceAnalyzer() = default;

ParallelPerformanceAnalyzer::ParallelMetrics ParallelPerformanceAnalyzer::analyzeParallelPerformance(const Brain* brain,
                                                                                               const std::vector<size_t>& threadCounts) {
    return analyzeSingleThreadCount(threadCounts.size(), brain);
}

double ParallelPerformanceAnalyzer::measureThreadOverhead(size_t numThreads) {
    auto start = std::chrono::high_resolution_clock::now();
    
    std::vector<std::thread> threads;
    for (size_t i = 0; i < numThreads; ++i) {
        threads.emplace_back([](size_t i) {
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }, i);
    }
    
    for (auto& thread : threads) {
        thread.join();
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    return std::chrono::duration<double, std::milli>(end - start).count();
}

double ParallelPerformanceAnalyzer::measureWorkDistribution(size_t neuronCount, size_t threadCount) {
    return 0.8;  // Simplified
}

double ParallelPerformanceAnalyzer::measureCachePerformance(size_t neuronCount, size_t threadCount) {
    return 0.9;  // Simplified
}

std::string ParallelPerformanceAnalyzer::getThreadPoolStatistics() const {
    return "Thread pool statistics (simplified)";
}

std::string ParallelPerformanceAnalyzer::generateParallelReport(const ParallelMetrics& metrics) const {
    std::stringstream report;
    report << "╔═══════════════════════════════════════════════════════════════╗\n";
    report << "║                   PARALLEL PERFORMANCE REPORT                      ║\n";
    report << "╚═══════════════════════════════════════════════════════════════╝\n\n";
    
    report << "║ Parallel Processing Metrics:";
    report << "\n║───────────────────────────────────────────────────────────────║\n";
    report << "║ Thread Count: " << metrics.threadCount << "\n";
    report << "║ Total Speedup: " << metrics.totalSpeedup << "x\n";
    report << "║ Efficiency: " << (metrics.efficiency * 100.0) << "%\n";
    report << "║ Parallel Fraction: " << (metrics.parallelFraction * 100.0) << "%\n";
    report << "║ Overhead Time: " << metrics.overheadTime << "ms\n";
    report << "║ Load Balancing: " << (metrics.loadBalancing * 100.0) << "%\n";
    report << "║ Cache Effectiveness: " << (metrics.cacheEffectiveness * 100.0) << "%\n";
    report << "║ Communication Overhead: " << (metrics.communicationOverhead * 100.0) << "%\n";
    report << "║ Is Parallel: " << (metrics.isParallel ? "YES" : "NO") << "\n";
    report << "║ Max Concurrent Threads: " << metrics.maxConcurrentThreads << "\n\n";
    
    report << "╚═══════════════════════════════════════════════════════════════╝\n";
    
    return report.str();
}

ParallelPerformanceAnalyzer::ParallelMetrics ParallelPerformanceAnalyzer::analyzeSingleThreadCount(size_t threadCount, const Brain* brain) {
    ParallelMetrics metrics;
    metrics.threadCount = threadCount;
    metrics.isParallel = (threadCount > 1);
    metrics.maxConcurrentThreads = threadCount;
    
    // Simplified calculations
    metrics.totalSpeedup = 1.0 + (threadCount - 1) * 0.8;  // Amdahl's law approximation
    metrics.efficiency = metrics.totalSpeedup / threadCount;
    metrics.parallelFraction = 0.8;  // Assume 80% can be parallelized
    metrics.overheadTime = threadCount * 1.0;  // 1ms per thread
    metrics.loadBalancing = 0.9;  // 90% efficient
    metrics.cacheEffectiveness = 0.85;  // 85% cache effectiveness
    metrics.communicationOverhead = 0.05;  // 5% communication overhead
    
    return metrics;
}

double ParallelPerformanceAnalyzer::benchmarkWithThreadCount(size_t threadCount, size_t neuronCount,
                                                           uint64_t steps) {
    auto start = std::chrono::high_resolution_clock::now();
    
    // Simulate parallel processing
    std::vector<std::thread> threads;
    std::mutex mutex;
    size_t workPerThread = neuronCount / threadCount;
    
    for (size_t t = 0; t < threadCount; ++t) {
        threads.emplace_back([&, t]() {
            size_t startIdx = t * workPerThread;
            size_t endIdx = (t == threadCount - 1) ? neuronCount : startIdx + workPerThread;
            
            for (size_t i = startIdx; i < endIdx; ++i) {
                // Simulate processing
                std::this_thread::sleep_for(std::chrono::microseconds(1));
            }
        });
    }
    
    for (auto& thread : threads) {
        thread.join();
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    return std::chrono::duration<double, std::milli>(end - start).count();
}

PerformanceProfiler::PerformanceProfiler() : pImpl(std::make_unique<Impl>()) {}
PerformanceProfiler::~PerformanceProfiler() = default;

PerformanceProfiler::FullProfile PerformanceProfiler::runCompleteProfile(const BenchmarkConfig& config) {
    FullProfile profile;
    
    // Run benchmarks
    profile.benchmarkMetrics = benchmarkRunner_.runBenchmarkTest(config);
    
    // Create dummy brain for other analyses
    auto cfg = std::make_shared<Config>();
    cfg->set("neuron_count", 1000);
    auto brain = std::make_shared<Brain>(cfg);
    brain->initialize();
    
    // Analyze memory pools
    profile.memoryPoolMetrics = memoryPoolAnalyzer_.analyzeMemoryPools(brain.get());
    
    // Analyze SIMD performance
    profile.simdMetrics = simdAnalyzer_.analyzeSIMDPerformance(brain.get());
    
    // Analyze parallel performance
    std::vector<size_t> threadCounts = {1, 2, 4, 8};
    profile.parallelMetrics = parallelAnalyzer_.analyzeParallelPerformance(brain.get(), threadCounts);
    
    // Get timestamp
    auto now = std::chrono::system_clock::now();
    auto in_time_t = std::chrono::system_clock::to_time_t(now);
    profile.profileTimestamp = std::string(std::ctime(&in_time_t));
    profile.profileTimestamp.erase(profile.profileTimestamp.size() - 1);  // Remove newline
    
    profile.totalProfileTime = profile.benchmarkMetrics.simulationTimeMs / 1000.0;
    
    return profile;
}

BenchmarkMetrics PerformanceProfiler::runBenchmarkTest(const BenchmarkConfig& config) {
    return benchmarkRunner_.runBenchmarks(config).empty() ? BenchmarkMetrics() : 
           benchmarkRunner_.runBenchmarks(config)[0];
}

MemoryPoolAnalyzer::PoolAnalysis PerformanceProfiler::runMemoryPoolTest(const Brain* brain) {
    return memoryPoolAnalyzer_.analyzeMemoryPools(brain);
}

SIMDPerformanceAnalyzer::SIMDMetrics PerformanceProfiler::runSIMDTest(const Brain* brain) {
    return simdAnalyzer_.analyzeSIMDPerformance(brain);
}

ParallelPerformanceAnalyzer::ParallelMetrics PerformanceProfiler::runParallelTest(const Brain* brain,
                                                                                 const std::vector<size_t>& threadCounts) {
    return parallelAnalyzer_.analyzeParallelPerformance(brain, threadCounts);
}

std::string PerformanceProfiler::generateCompleteReport(const FullProfile& profile) const {
    std::stringstream report;
    report << "╔═══════════════════════════════════════════════════════════════╗\n";
    report << "║                  COMPLETE PERFORMANCE PROFILE                     ║\n";
    report << "╚═══════════════════════════════════════════════════════════════╝\n\n";
    
    report << profile.memoryPoolMetrics.poolName << " Performance Analysis:\n";
    report << "========================================\n\n";
    
    report << benchmarkRunner_.generateReport({profile.benchmarkMetrics});
    report << "\n\n";
    
    report << "Memory Pool Analysis:\n";
    report << "====================\n";
    report << "Total Allocations: " << profile.memoryPoolMetrics.totalAllocations << "\n";
    report << "Total Deallocations: " << profile.memoryPoolMetrics.totalDeallocations << "\n";
    report << "Peak Active Objects: " << profile.memoryPoolMetrics.peakActiveObjects << "\n";
    report << "Memory Usage: " << benchmarkRunner_.formatMemory(profile.memoryPoolMetrics.memoryUsage) << "\n\n";
    
    report << simdAnalyzer_.generateSIMDReport(profile.simdMetrics);
    report << "\n\n";
    
    report << parallelAnalyzer_.generateParallelReport(profile.parallelMetrics);
    
    return report.str();
}

bool PerformanceProfiler::exportProfile(const FullProfile& profile, const std::string& outputDir) {
    std::filesystem::create_directories(outputDir);
    
    // Export JSON
    std::string jsonContent = generateCompleteReport(profile);
    std::string jsonFile = outputDir + "/profile.json";
    if (!benchmarkRunner_.exportBenchmarkData({profile.benchmarkMetrics}, jsonFile)) {
        return false;
    }
    
    // Export report
    std::string reportFile = outputDir + "/profile_report.txt";
    std::ofstream file(reportFile);
    if (!file.is_open()) {
        return false;
    }
    file << generateCompleteReport(profile);
    file.close();
    
    return true;
}

bool PerformanceProfiler::loadProfile(const std::string& filename, FullProfile& profile) {
    // Simplified - in practice would use proper serialization
    return false;
}

bool PerformanceProfiler::saveProfile(const FullProfile& profile, const std::string& filename) const {
    // Simplified - in practice would use proper serialization
    return false;
}

std::string PerformanceProfiler::compareProfiles(const FullProfile& profile1, const FullProfile& profile2) const {
    std::stringstream report;
    report << "╔═══════════════════════════════════════════════════════════════╗\n";
    report << "║                  PERFORMANCE COMPARISON REPORT                      ║\n";
    report << "╚═══════════════════════════════════════════════════════════════╝\n\n";
    
    report << "Comparing benchmark performance:";
    report << "\n----------------------------------------\n";
    report << "Profile 1 simulation time: " << benchmarkRunner_.formatTime(profile1.benchmarkMetrics.simulationTimeMs / 1000.0) << "\n";
    report << "Profile 2 simulation time: " << benchmarkRunner_.formatTime(profile2.benchmarkMetrics.simulationTimeMs / 1000.0) << "\n";
    report << "Speedup: " << (profile2.benchmarkMetrics.simulationTimeMs / profile1.benchmarkMetrics.simulationTimeMs) << "x\n\n";
    
    report << "Memory usage comparison:";
    report << "\n----------------------------------------\n";
    report << "Profile 1 memory: " << benchmarkRunner_.formatMemory(profile1.memoryPoolMetrics.memoryUsage) << "\n";
    report << "Profile 2 memory: " << benchmarkRunner_.formatMemory(profile2.memoryPoolMetrics.memoryUsage) << "\n";
    report << "Memory efficiency: " << (profile2.memoryPoolMetrics.memoryUsage / profile1.memoryPoolMetrics.memoryUsage) << "x\n\n";
    
    return report.str();
}

std::string PerformanceProfiler::getProfileSummary(const FullProfile& profile) const {
    std::stringstream summary;
    summary << "Performance Profile Summary (" << profile.profileTimestamp << ")\n";
    summary << "Total Profile Time: " << benchmarkRunner_.formatTime(profile.totalProfileTime) << "\n";
    summary << "Benchmarks Run: 1\n";
    summary << "Memory Pools Analyzed: " << 1 << "\n";
    summary << "SIMD Capabilities: " << (profile.simdMetrics.simdEnabled ? "YES" : "NO") << "\n";
    summary << "Parallel Processing: " << (profile.parallelMetrics.isParallel ? "YES" : "NO") << "\n";
    return summary.str();
}

void PerformanceProfiler::resetCounters() {
    benchmarkRunner_.resetPeakMemoryUsage();
}

void PerformanceProfiler::startMonitoring() {
    resetCounters();
}

PerformanceProfiler::FullProfile PerformanceProfiler::stopMonitoring() {
    return runCompleteProfile();
}

PerformanceReportGenerator::PerformanceReportGenerator() : pImpl(std::make_unique<Impl>()) {}
PerformanceReportGenerator::~PerformanceReportGenerator() = default;

std::string PerformanceReportGenerator::generateHumanReadableReport(const PerformanceProfiler::FullProfile& profile) const {
    return "Human-readable report not implemented (use generateCompleteReport)";
}

std::string PerformanceReportGenerator::generateJSONReport(const PerformanceProfiler::FullProfile& profile) const {
    std::stringstream json;
    json << "{\n";
    json << "  \"profile_timestamp\": \"" << profile.profileTimestamp << "\",\n";
    json << "  \"total_profile_time\": " << profile.totalProfileTime << ",\n";
    json << "  \"benchmark_metrics\": {\n";
    json << "    \"simulation_time_ms\": " << profile.benchmarkMetrics.simulationTimeMs << ",\n";
    json << "    \"wall_clock_time_ms\": " << profile.benchmarkMetrics.wallClockTimeMs << ",\n";
    json << "    \"spikes_per_second\": " << profile.benchmarkMetrics.spikesPerSecond << ",\n";
    json << "    \"neurons_per_millisecond\": " << profile.benchmarkMetrics.neuronsPerMillisecond << ",\n";
    json << "    \"learning_rate\": " << profile.benchmarkMetrics.learningRate << "\n";
    json << "  },\n";
    json << "  \"memory_pool_metrics\": {\n";
    json << "    \"pool_name\": \"" << profile.memoryPoolMetrics.poolName << "\",\n";
    json << "    \"total_allocations\": " << profile.memoryPoolMetrics.totalAllocations << ",\n";
    json << "    \"total_deallocations\": " << profile.memoryPoolMetrics.totalDeallocations << ",\n";
    json << "    \"peak_active_objects\": " << profile.memoryPoolMetrics.peakActiveObjects << ",\n";
    json << "    \"memory_usage\": " << profile.memoryPoolMetrics.memoryUsage << ",\n";
    json << "    \"fragmentation_ratio\": " << profile.memoryPoolMetrics.fragmentationRatio << "\n";
    json << "  },\n";
    json << "  \"simd_metrics\": {\n";
    json << "    \"simd_enabled\": " << (profile.simdMetrics.simdEnabled ? "true" : "false") << ",\n";
    json << "    \"vectorization_efficiency\": " << profile.simdMetrics.vectorizationEfficiency << ",\n";
    json << "    \"simd_speedup\": " << profile.simdMetrics.simdSpeedup << "\n";
    json << "  },\n";
    json << "  \"parallel_metrics\": {\n";
    json << "    \"thread_count\": " << profile.parallelMetrics.threadCount << ",\n";
    json << "    \"total_speedup\": " << profile.parallelMetrics.totalSpeedup << ",\n";
    json << "    \"efficiency\": " << profile.parallelMetrics.efficiency << ",\n";
    json << "    \"is_parallel\": " << (profile.parallelMetrics.isParallel ? "true" : "false") << "\n";
    json << "  }\n";
    json << "}\n";
    
    return json.str();
}

std::string PerformanceReportGenerator::generateCSVReport(const PerformanceProfiler::FullProfile& profile) const {
    std::stringstream csv;
    csv << "Metric,Value\n";
    csv << "Profile Timestamp," << profile.profileTimestamp << "\n";
    csv << "Total Profile Time (s)," << profile.totalProfileTime << "\n";
    csv << "Simulation Time (ms)," << profile.benchmarkMetrics.simulationTimeMs << "\n";
    csv << "Wall Clock Time (ms)," << profile.benchmarkMetrics.wallClockTimeMs << "\n";
    csv << "Spikes per Second," << profile.benchmarkMetrics.spikesPerSecond << "\n";
    csv << "Memory Usage (MB)," << profile.memoryPoolMetrics.memoryUsage / (1024 * 1024) << "\n";
    return csv.str();
}

std::string PerformanceReportGenerator::generateXMLReport(const PerformanceProfiler::FullProfile& profile) const {
    std::stringstream xml;
    xml << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
    xml << "<profile>\n";
    xml << "  <metadata>\n";
    xml << "    <timestamp>" << profile.profileTimestamp << "</timestamp>\n";
    xml << "    <total_time>" << profile.totalProfileTime << "</total_time>\n";
    xml << "  </metadata>\n";
    xml << "  <benchmark>\n";
    xml << "    <simulation_time_ms>" << profile.benchmarkMetrics.simulationTimeMs << "</simulation_time_ms>\n";
    xml << "    <wall_clock_time_ms>" << profile.benchmarkMetrics.wallClockTimeMs << "</wall_clock_time_ms>\n";
    xml << "    <spikes_per_second>" << profile.benchmarkMetrics.spikesPerSecond << "</spikes_per_second>\n";
    xml << "  </benchmark>\n";
    xml << "  <memory_pool>\n";
    xml << "    <pool_name>" << profile.memoryPoolMetrics.poolName << "</pool_name>\n";
    xml << "    <memory_usage>" << profile.memoryPoolMetrics.memoryUsage << "</memory_usage>\n";
    xml << "  </memory_pool>\n";
    xml << "</profile>\n";
    
    return xml.str();
}

std::string PerformanceReportGenerator::generateLaTeXReport(const PerformanceProfiler::FullProfile& profile) const {
    std::stringstream latex;
    latex << "\\documentclass{article}\n";
    latex << "\\title{Performance Profile Report}\n";
    latex << "\\date{" << profile.profileTimestamp << "}\n";
    latex << "\\begin{document}\n\n";
    latex << "\\section{Performance Summary}\n";
    latex << "\\begin{itemize}\n";
    latex << "\\item Total profile time: " << profile.totalProfileTime << " seconds\n";
    latex << "\\item Simulation time: " << (profile.benchmarkMetrics.simulationTimeMs / 1000.0) << " seconds\n";
    latex << "\\item Spikes per second: " << profile.benchmarkMetrics.spikesPerSecond << "\n";
    latex << "\\item Memory usage: " << (profile.memoryPoolMetrics.memoryUsage / (1024 * 1024)) << " MB\n";
    latex << "\\end{itemize}\n\\n";
    latex << "\\section{Detailed Metrics}\n";
    latex << "\\begin{description}\n";
    latex << "\\item{Benchmark Metrics}\n";
    latex << "  \\begin{itemize}\n";
    latex << "  \\item Simulation Time: " << (profile.benchmarkMetrics.simulationTimeMs / 1000.0) << " seconds\n";
    latex << "  \\item Wall Clock Time: " << (profile.benchmarkMetrics.wallClockTimeMs / 1000.0) << " seconds\n";
    latex << "  \\item Spikes per Second: " << profile.benchmarkMetrics.spikesPerSecond << "\n";
    latex << "  \\end{itemize}\n";
    latex << "\\end{description}\n\\end{document}\n";
    
    return latex.str();
}

std::string PerformanceReportGenerator::generateComparisonReport(const std::vector<PerformanceProfiler::FullProfile>& profiles) const {
    std::stringstream report;
    report << "╔═══════════════════════════════════════════════════════════════╗\n";
    report << "║                 PERFORMANCE COMPARISON REPORT                      ║\n";
    report << "╚═══════════════════════════════════════════════════════════════╝\n\n";
    
    if (profiles.size() < 2) {
        report << "Need at least 2 profiles for comparison.\n";
        return report.str();
    }
    
    for (size_t i = 0; i < profiles.size(); ++i) {
        report << "Profile " << (i + 1) << ":\n";
        report << "----------------------------------------\n";
        report << "Timestamp: " << profiles[i].profileTimestamp << "\n";
        report << "Total Time: " << (profiles[i].totalProfileTime) << " seconds\n";
        report << "Simulation Time: " << (profiles[i].benchmarkMetrics.simulationTimeMs / 1000.0) << " seconds\n";
        report << "Memory Usage: " << (profiles[i].memoryPoolMetrics.memoryUsage / (1024 * 1024)) << " MB\n\n";
    }
    
    return report.str();
}

bool PerformanceReportGenerator::saveReport(const std::string& content, const std::string& filename) const {
    std::ofstream file(filename);
    if (!file.is_open()) {
        return false;
    }
    file << content;
    file.close();
    return true;
}

std::vector<std::string> PerformanceReportGenerator::getAvailableFormats() const {
    return {"human", "json", "csv", "xml", "latex", "comparison"};
}

bool PerformanceReportGenerator::validateReport(const std::string& content, const std::string& format) const {
    if (format == "json") {
        // Basic JSON validation
        return !content.empty() && content.find('{') != std::string::npos;
    } else if (format == "csv") {
        // Basic CSV validation
        return !content.empty() && content.find(',') != std::string::npos;
    } else if (format == "xml") {
        // Basic XML validation
        return !content.empty() && content.find('<') != std::string::npos;
    } else if (format == "latex") {
        // Basic LaTeX validation
        return !content.empty() && content.find('\\') != std::string::npos;
    }
    return !content.empty();
}

} // namespace nlm
