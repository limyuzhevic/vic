#include "PerformanceAnalysis.hpp"
#include "core/Config/Config.hpp"
#include "brain/Brain.hpp"
#include "experiments/Experiment.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <chrono>
#include <thread>
#include <map>
#include <string>
#include <algorithm>
#include <cmath>
#include <iomanip>
#include <ctime>
#include <sstream>

namespace nlm {

struct PerformanceAnalyzer::Impl {
    std::map<std::string, BenchmarkResult> benchmarks;
    std::map<std::string, ProfileInfo> profiles;
    std::vector<MemoryAllocation> memoryAllocations;
    std::vector<MemoryAllocation> currentAllocations;
    bool profilingEnabled;
    bool memoryTrackingEnabled;
    bool cpuTimingEnabled;
    int threadCount;
    CPUBenchmark cpuBenchmark;
    NetworkProfile networkProfile;
    GPUBenchmark gpuBenchmark;
    std::shared_ptr<Brain> currentBrain;
    std::shared_ptr<Experiment> currentExperiment;
    std::chrono::high_resolution_clock::time_point sessionStart;
    std::map<std::string, std::chrono::high_resolution_clock::time_point> benchmarkStartTimes;
    
    Impl() : profilingEnabled(true), memoryTrackingEnabled(true), cpuTimingEnabled(true),
             threadCount(std::thread::hardware_concurrency()), sessionStart(std::chrono::high_resolution_clock::now()) {}
};

PerformanceAnalyzer::PerformanceAnalyzer() : pImpl(std::make_unique<Impl>()) {}
PerformanceAnalyzer::~PerformanceAnalyzer() = default;

void PerformanceAnalyzer::startBenchmark(const std::string& name) {
    if (!pImpl->profilingEnabled) return;
    pImpl->benchmarkStartTimes[name] = std::chrono::high_resolution_clock::now();
    NLM_LOG_INFO("Starting benchmark: " + name);
}

void PerformanceAnalyzer::stopBenchmark(const std::string& name) {
    if (!pImpl->profilingEnabled) return;
    
    auto start = pImpl->benchmarkStartTimes.find(name);
    if (start == pImpl->benchmarkStartTimes.end()) {
        NLM_LOG_WARN("Benchmark not started: " + name);
        return;
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start->second).count();
    
    BenchmarkResult result;
    result.benchmarkName = name;
    result.wallClockTime = duration / 1000000.0; // Convert to seconds
    
    // Estimate CPU time (simplified)
    result.cpuTime = duration / 1000000.0 * 0.8; // Assume 80% CPU utilization
    result.userTime = result.cpuTime * 0.6;
    result.systemTime = result.cpuTime * 0.4;
    
    // Calculate performance metrics
    result.instructionsPerSecond = 1000000000.0 / result.wallClockTime; // Estimate
    result.flopsPerSecond = 500000000.0 / result.wallClockTime; // Estimate for neural ops
    
    // Estimate memory usage
    result.memoryPeak = 100 * 1024 * 1024; // 100MB estimate
    result.memoryAllocated = 50 * 1024 * 1024; // 50MB estimate
    
    result.threadCount = pImpl->threadCount;
    result.efficiency = 1.0 / result.threadCount;
    
    pImpl->benchmarks[name] = result;
    pImpl->benchmarkStartTimes.erase(start);
    
    NLM_LOG_INFO("Benchmark completed: " + name + " - Time: " + 
                 std::to_string(result.wallClockTime) + "s");
}

void PerformanceAnalyzer::startProfiling(const std::string& functionName) {
    if (!pImpl->profilingEnabled) return;
    
    ProfileInfo info;
    info.functionName = functionName;
    info.times.clear();
    
    pImpl->profiles[functionName] = info;
}

void PerformanceAnalyzer::stopProfiling(const std::string& functionName) {
    if (!pImpl->profilingEnabled) return;
    
    auto it = pImpl->profiles.find(functionName);
    if (it == pImpl->profiles.end()) return;
    
    double totalTime = 0.0;
    for (double time : it->second.times) {
        totalTime += time;
    }
    
    it->second.timeSpent = totalTime;
    it->second.cpuTimeSpent = totalTime * 0.9;
    it->second.callCount = it->second.times.size();
    
    NLM_LOG_INFO("Profiling stopped: " + functionName + " - Time: " + 
                 std::to_string(totalTime) + "s, Calls: " + 
                 std::to_string(it->second.callCount));
}

void PerformanceAnalyzer::trackMemoryAllocation(void* address, size_t size, const std::string& site) {
    if (!pImpl->memoryTrackingEnabled) return;
    
    MemoryAllocation alloc;
    alloc.address = address;
    alloc.size = size;
    alloc.allocationSite = site;
    alloc.timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::high_resolution_clock::now() - pImpl->sessionStart).count();
    alloc.stackDepth = 10; // Simplified
    
    pImpl->currentAllocations.push_back(alloc);
}

void PerformanceAnalyzer::trackMemoryDeallocation(void* address) {
    // Remove corresponding allocation
    auto it = std::remove_if(pImpl->currentAllocations.begin(), 
                           pImpl->currentAllocations.end(),
                           [address](const MemoryAllocation& alloc) {
                               return alloc.address == address;
                           });
    pImpl->currentAllocations.erase(it, pImpl->currentAllocations.end());
}

bool PerformanceAnalyzer::detectMemoryLeaks() {
    pImpl->memoryAllocations = pImpl->currentAllocations;
    return !pImpl->currentAllocations.empty();
}

std::vector<MemoryAllocation> PerformanceAnalyzer::getMemoryLeaks() {
    return pImpl->memoryAllocations;
}

CPUBenchmark PerformanceAnalyzer::getCPUBenchmark() const {
    return pImpl->cpuBenchmark;
}

std::map<std::string, ProfileInfo> PerformanceAnalyzer::getProfileData() const {
    return pImpl->profiles;
}

void PerformanceAnalyzer::trackNetworkTraffic(double bytesSent, double bytesReceived, double latency) {
    pImpl->networkProfile.bytesSent += bytesSent;
    pImpl->networkProfile.bytesReceived += bytesReceived;
    pImpl->networkProfile.packetsSent++;
    pImpl->networkProfile.packetsReceived++;
    pImpl->networkProfile.networkLatency = latency;
    pImpl->networkProfile.bandwidth += (bytesSent + bytesReceived) / latency;
    pImpl->networkProfile.messageSize = (bytesSent + bytesReceived) / 2;
}

NetworkProfile PerformanceAnalyzer::getNetworkProfile() const {
    return pImpl->networkProfile;
}

void PerformanceAnalyzer::trackGPUMetrics(const GPUBenchmark& metrics) {
    pImpl->gpuBenchmark = metrics;
}

GPUBenchmark PerformanceAnalyzer::getGPUBenchmark() const {
    return pImpl->gpuBenchmark;
}

BenchmarkResult PerformanceAnalyzer::getBenchmarkResult(const std::string& name) const {
    auto it = pImpl->benchmarks.find(name);
    if (it != pImpl->benchmarks.end()) {
        return it->second;
    }
    return BenchmarkResult(); // Return empty result
}

std::vector<BenchmarkResult> PerformanceAnalyzer::getAllBenchmarkResults() const {
    std::vector<BenchmarkResult> results;
    for (const auto& pair : pImpl->benchmarks) {
        results.push_back(pair.second);
    }
    return results;
}

void PerformanceAnalyzer::configure(int threadCount, bool enableProfiling, 
                                   bool enableMemoryTracking, bool enableCPUTiming) {
    pImpl->threadCount = threadCount > 0 ? threadCount : std::thread::hardware_concurrency();
    pImpl->profilingEnabled = enableProfiling;
    pImpl->memoryTrackingEnabled = enableMemoryTracking;
    pImpl->cpuTimingEnabled = enableCPUTiming;
}

void PerformanceAnalyzer::saveResults(const std::string& filename) const {
    std::ofstream file(filename);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Could not open file for saving: " + filename);
        return;
    }
    
    file << "NLM Performance Analysis Results\n";
    file << "Generated: " << std::ctime(&pImpl->sessionStart) << "\n\n";
    
    file << "Benchmark Results:\n";
    for (const auto& pair : pImpl->benchmarks) {
        file << "  " << pair.first << ": " << pair.second.wallClockTime << "s\n";
    }
    
    file << "\nProfile Data:\n";
    for (const auto& pair : pImpl->profiles) {
        file << "  " << pair.first << ": " << pair.second.timeSpent << "s (" 
             << pair.second.callCount << " calls)\n";
    }
    
    file << "\nMemory Leaks: " << pImpl->currentAllocations.size() << "\n";
    file.close();
    NLM_LOG_INFO("Performance results saved to: " + filename);
}

void PerformanceAnalyzer::loadResults(const std::string& filename) {
    // Implementation for loading results from file
    NLM_LOG_INFO("Loading performance results from: " + filename);
}

void PerformanceAnalyzer::clearResults() {
    pImpl->benchmarks.clear();
    pImpl->profiles.clear();
    pImpl->memoryAllocations.clear();
    pImpl->currentAllocations.clear();
    NLM_LOG_INFO("Performance results cleared");
}

void PerformanceAnalyzer::attachToBrainSimulation(std::shared_ptr<Brain> brain) {
    pImpl->currentBrain = brain;
}

void PerformanceAnalyzer::attachToExperiment(std::shared_ptr<Experiment> experiment) {
    pImpl->currentExperiment = experiment;
}

BenchmarkResult benchmarkSimulation(const std::string& name, 
                                    std::function<void()> simulationFunction,
                                    const Config& config) {
    BenchmarkResult result;
    result.benchmarkName = name;
    
    auto start = std::chrono::high_resolution_clock::now();
    
    // Warm-up run
    simulationFunction();
    
    // Actual benchmark run
    auto warmStart = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < 10; ++i) { // Multiple iterations for accuracy
        simulationFunction();
    }
    auto warmEnd = std::chrono::high_resolution_clock::now();
    
    auto end = std::chrono::high_resolution_clock::now();
    
    auto totalDuration = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
    auto warmDuration = std::chrono::duration_cast<std::chrono::microseconds>(warmEnd - warmStart).count();
    
    result.wallClockTime = totalDuration / 1000000.0;
    result.cpuTime = warmDuration / 1000000.0;
    
    // Estimate based on configuration
    if (config.has("neuron_count")) {
        size_t neuronCount = std::stoi(config.getValue("neuron_count"));
        result.memoryPeak = neuronCount * 1024; // Estimate: 1KB per neuron
    }
    
    return result;
}

BenchmarkResult benchmarkNeuronOperations(size_t neuronCount, size_t steps) {
    BenchmarkResult result;
    result.benchmarkName = "Neuron Operations Benchmark";
    
    auto start = std::chrono::high_resolution_clock::now();
    
    // Simulate neuron operations
    for (size_t i = 0; i < steps; ++i) {
        // Simulate neuron activation
        double voltage = 0.0;
        for (size_t j = 0; j < neuronCount; ++j) {
            voltage += 0.01 * sin(j * 0.1); // Simple simulation
        }
        // Simple thresholding
        if (voltage > 1.0) {}
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
    
    result.wallClockTime = duration / 1000000.0;
    result.cpuTime = duration / 1000000.0 * 0.9;
    result.stepsPerSecond = steps / result.wallClockTime;
    
    return result;
}

BenchmarkResult benchmarkSynapseOperations(size_t synapseCount, size_t steps) {
    BenchmarkResult result;
    result.benchmarkName = "Synapse Operations Benchmark";
    
    auto start = std::chrono::high_resolution_clock::now();
    
    // Simulate synapse operations
    std::vector<double> weights(synapseCount, 0.5);
    for (size_t i = 0; i < steps; ++i) {
        for (size_t j = 0; j < synapseCount; ++j) {
            // Simulate synaptic plasticity
            double delta = 0.01 * (rand() / (RAND_MAX + 1.0) - 0.5);
            weights[j] += delta;
        }
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
    
    result.wallClockTime = duration / 1000000.0;
    result.cpuTime = duration / 1000000.0 * 0.85;
    result.stepsPerSecond = steps / result.wallClockTime;
    
    return result;
}

BenchmarkResult benchmarkMemoryAllocation(size_t size, size_t iterations) {
    BenchmarkResult result;
    result.benchmarkName = "Memory Allocation Benchmark";
    
    auto start = std::chrono::high_resolution_clock::now();
    
    std::vector<std::vector<char>> memoryBlocks;
    for (size_t i = 0; i < iterations; ++i) {
        memoryBlocks.push_back(std::vector<char>(size, 0));
    }
    
    // Force some deallocations
    for (size_t i = 0; i < iterations / 2; ++i) {
        memoryBlocks.erase(memoryBlocks.begin());
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
    
    result.wallClockTime = duration / 1000000.0;
    result.cpuTime = duration / 1000000.0 * 0.8;
    result.memoryPeak = size * iterations / 2;
    
    return result;
}

BenchmarkResult benchmarkThreading(int threadCount, size_t workload) {
    BenchmarkResult result;
    result.benchmarkName = "Threading Benchmark";
    result.threadCount = threadCount;
    
    auto start = std::chrono::high_resolution_clock::now();
    
    std::vector<std::thread> threads;
    std::atomic<size_t> completed(0);
    
    for (int i = 0; i < threadCount; ++i) {
        threads.emplace_back([&, i]() {
            size_t localCompleted = 0;
            for (size_t j = 0; j < workload / threadCount; ++j) {
                // Simulate work
                double value = sin(j * 0.1) * cos(i * 0.2);
                localCompleted++;
            }
            completed += localCompleted;
        });
    }
    
    for (auto& thread : threads) {
        thread.join();
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
    
    result.wallClockTime = duration / 1000000.0;
    result.cpuTime = duration / 1000000.0 * threadCount;
    result.stepsPerSecond = completed / result.wallClockTime;
    result.efficiency = 1.0 / (threadCount * result.wallClockTime / workload);
    
    return result;
}

PerformanceComparator::PerformanceComparator() : pImpl(std::make_unique<Impl>()) {}
PerformanceComparator::~PerformanceComparator() = default;

void PerformanceComparator::addBenchmark(const std::string& name, const BenchmarkResult& result) {
    pImpl->benchmarks[name] = result;
}

void PerformanceComparator::compareAll() {
    NLM_LOG_INFO("Performance comparison results:");
    for (const auto& pair : pImpl->benchmarks) {
        NLM_LOG_INFO("  " + pair.first + ": " + std::to_string(pair.second.wallClockTime) + "s, " +
                     std::to_string(pair.second.stepsPerSecond) + " steps/sec");
    }
}

void PerformanceComparator::generateComparisonReport(const std::string& outputFile) {
    std::ofstream file(outputFile);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Could not open file for comparison report: " + outputFile);
        return;
    }
    
    file << "NLM Performance Comparison Report\n";
    file << "Generated: " << std::ctime(&std::time(nullptr)) << "\n\n";
    
    for (const auto& pair : pImpl->benchmarks) {
        file << "Benchmark: " << pair.first << "\n";
        file << "  Wall Time: " << pair.second.wallClockTime << "s\n";
        file << "  CPU Time: " << pair.second.cpuTime << "s\n";
        file << "  Memory Peak: " << pair.second.memoryPeak << " bytes\n";
        file << "  Steps/Second: " << pair.second.stepsPerSecond << "\n\n";
    }
    
    file.close();
    NLM_LOG_INFO("Comparison report saved to: " + outputFile);
}

double PerformanceComparator::calculateMean(const std::vector<double>& values) const {
    if (values.empty()) return 0.0;
    double sum = std::accumulate(values.begin(), values.end(), 0.0);
    return sum / values.size();
}

double PerformanceComparator::calculateStdDev(const std::vector<double>& values, double mean) const {
    if (values.size() <= 1) return 0.0;
    double sumSquares = 0.0;
    for (double v : values) {
        double diff = v - mean;
        sumSquares += diff * diff;
    }
    return std::sqrt(sumSquares / (values.size() - 1));
}

double PerformanceComparator::calculateConfidenceInterval(const std::vector<double>& values, double confidence) const {
    if (values.size() <= 1) return 0.0;
    // Simplified implementation
    double mean = calculateMean(values);
    double stdDev = calculateStdDev(values, mean);
    double zScore = 1.96; // For 95% confidence
    return zScore * stdDev / std::sqrt(values.size());
}

} // namespace nlm
