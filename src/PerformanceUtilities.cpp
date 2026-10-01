// PerformanceUtilities.cpp
// Implementation of performance measurement and optimization tools for NLM

#include "PerformanceUtilities.hpp"
#include <fstream>
#include <algorithm>
#include <numeric>
#include <cmath>
#include <sstream>

using namespace nlm;

namespace {
    // Helper functions
    size_t getSystemMemoryUsage();
    size_t getPeakMemoryUsage();
    std::string formatBytes(size_t bytes);
    std::string getCurrentTimestamp();
}

// Profiler Implementation
Profiler::Profiler() {
    startSession();
}

Profiler::~Profiler() {
    endSession();
}

void Profiler::startProfile(const std::string& operation) {
    currentProfile = operation;
    profileHistory.push_back(ProfileData(operation));
    profileHistory.back().start = std::chrono::high_resolution_clock::now();
}

void Profiler::endProfile() {
    if (!currentProfile.empty() && !profileHistory.empty()) {
        auto& lastProfile = profileHistory.back();
        lastProfile.end = std::chrono::high_resolution_clock::now();
        lastProfile.duration = std::chrono::duration<double>(lastProfile.end - lastProfile.start);
        currentProfile.clear();
    }
}

void Profiler::incrementIteration() {
    if (!profileHistory.empty()) {
        profileHistory.back().iterationCount++;
    }
}

void Profiler::startSession() {
    sessionStart = std::chrono::high_resolution_clock::now();
}

void Profiler::endSession() {
    // Session end is handled by destructor
}

const std::vector<Profiler::ProfileData>& Profiler::getProfileHistory() const {
    return profileHistory;
}

void Profiler::clearHistory() {
    profileHistory.clear();
}

double Profiler::getTotalTime() const {
    double total = 0.0;
    for (const auto& profile : profileHistory) {
        total += profile.duration.count();
    }
    return total;
}

double Profiler::getAverageTime(const std::string& operation) const {
    std::vector<double> times;
    for (const auto& profile : profileHistory) {
        if (profile.operationName == operation) {
            times.push_back(profile.duration.count());
        }
    }
    if (times.empty()) return 0.0;
    
    return std::accumulate(times.begin(), times.end(), 0.0) / times.size();
}

double Profiler::getMinTime(const std::string& operation) const {
    std::vector<double> times;
    for (const auto& profile : profileHistory) {
        if (profile.operationName == operation) {
            times.push_back(profile.duration.count());
        }
    }
    if (times.empty()) return 0.0;
    
    return *std::min_element(times.begin(), times.end());
}

double Profiler::getMaxTime(const std::string& operation) const {
    std::vector<double> times;
    for (const auto& profile : profileHistory) {
        if (profile.operationName == operation) {
            times.push_back(profile.duration.count());
        }
    }
    if (times.empty()) return 0.0;
    
    return *std::max_element(times.begin(), times.end());
}

void Profiler::printReport() const {
    std::cout << "=== Performance Profile Report ===" << std::endl;
    std::cout << "Total operations: " << profileHistory.size() << std::endl;
    std::cout << "Total time: " << getTotalTime() << "s" << std::endl;
    
    // Group by operation
    std::map<std::string, std::vector<double>> operationTimes;
    for (const auto& profile : profileHistory) {
        operationTimes[profile.operationName].push_back(profile.duration.count());
    }
    
    std::cout << std::endl;
    std::cout << "Operations by type:" << std::endl;
    for (const auto& pair : operationTimes) {
        const auto& operation = pair.first;
        const auto& times = pair.second;
        
        double avg = std::accumulate(times.begin(), times.end(), 0.0) / times.size();
        double min = *std::min_element(times.begin(), times.end());
        double max = *std::max_element(times.begin(), times.end());
        
        std::cout << "  " << operation << ": " << times.size() << " calls, "
                  << "avg=" << avg << "s, min=" << min << "s, max=" << max << "s" << std::endl;
    }
}

void Profiler::exportReport(const std::string& filename) const {
    std::ofstream outFile(filename);
    if (!outFile.is_open()) {
        std::cerr << "ERROR: Cannot open output file: " << filename << std::endl;
        return;
    }
    
    outFile << "NLM Performance Profile Export" << std::endl;
    outFile << "===============================" << std::endl;
    outFile << "Generated: " << getCurrentTimestamp() << std::endl;
    outFile << "Total operations: " << profileHistory.size() << std::endl;
    outFile << "Total time: " << getTotalTime() << "s" << std::endl;
    outFile << std::endl;
    
    // Group by operation
    std::map<std::string, std::vector<double>> operationTimes;
    for (const auto& profile : profileHistory) {
        operationTimes[profile.operationName].push_back(profile.duration.count());
    }
    
    outFile << "Operations:" << std::endl;
    for (const auto& pair : operationTimes) {
        const auto& operation = pair.first;
        const auto& times = pair.second;
        
        double avg = std::accumulate(times.begin(), times.end(), 0.0) / times.size();
        double min = *std::min_element(times.begin(), times.end());
        double max = *std::max_element(times.begin(), times.end());
        
        outFile << "  " << operation << ": " << times.size() << " calls, "
                << "avg=" << avg << "s, min=" << min << "s, max=" << max << "s" << std::endl;
    }
    
    outFile.close();
    std::cout << "Performance profile exported to: " << filename << std::endl;
}

// Memory Profiler Implementation
void MemoryProfiler::takeSnapshot() {
    MemorySnapshot snapshot;
    snapshot.timestamp = std::chrono::high_resolution_clock::now();
    snapshot.allocatedMemory = getSystemMemoryUsage();
    snapshot.totalMemory = getSystemMemoryUsage();
    snapshot.peakMemory = getPeakMemoryUsage();
    snapshot.objectCount = 0; // Placeholder
    
    memoryHistory.push_back(snapshot);
}

const std::vector<MemoryProfiler::MemorySnapshot>& MemoryProfiler::getHistory() const {
    return memoryHistory;
}

void MemoryProfiler::printReport() const {
    std::cout << "=== Memory Profile Report ===" << std::endl;
    std::cout << "Snapshots taken: " << memoryHistory.size() << std::endl;
    
    if (memoryHistory.empty()) return;
    
    size_t minMemory = memoryHistory[0].totalMemory;
    size_t maxMemory = memoryHistory[0].totalMemory;
    double avgMemory = 0.0;
    
    for (const auto& snapshot : memoryHistory) {
        minMemory = std::min(minMemory, snapshot.totalMemory);
        maxMemory = std::max(maxMemory, snapshot.totalMemory);
        avgMemory += snapshot.totalMemory;
    }
    
    avgMemory /= memoryHistory.size();
    
    std::cout << "Average memory usage: " << formatBytes(avgMemory) << std::endl;
    std::cout << "Peak memory usage: " << formatBytes(maxMemory) << std::endl;
    std::cout << "Minimum memory usage: " << formatBytes(minMemory) << std::endl;
    
    // Show memory trend
    std::cout << std::endl;
    std::cout << "Memory usage over time:" << std::endl;
    for (size_t i = 0; i < memoryHistory.size(); ++i) {
        const auto& snapshot = memoryHistory[i];
        auto time = std::chrono::duration<double>(snapshot.timestamp - memoryHistory[0].timestamp).count();
        std::cout << "  " << time << "s: " << formatBytes(snapshot.totalMemory) << std::endl;
    }
}

void MemoryProfiler::exportReport(const std::string& filename) const {
    std::ofstream outFile(filename);
    if (!outFile.is_open()) {
        std::cerr << "ERROR: Cannot open output file: " << filename << std::endl;
        return;
    }
    
    outFile << "NLM Memory Profile Export" << std::endl;
    outFile << "===========================" << std::endl;
    outFile << "Generated: " << getCurrentTimestamp() << std::endl;
    outFile << "Snapshots taken: " << memoryHistory.size() << std::endl;
    outFile << std::endl;
    
    for (size_t i = 0; i < memoryHistory.size(); ++i) {
        const auto& snapshot = memoryHistory[i];
        auto time = std::chrono::duration<double>(snapshot.timestamp - memoryHistory[0].timestamp).count();
        
        outFile << "Snapshot " << i << ": " << time << "s" << std::endl;
        outFile << "  Total memory: " << formatBytes(snapshot.totalMemory) << std::endl;
        outFile << "  Allocated memory: " << formatBytes(snapshot.allocatedMemory) << std::endl;
        outFile << "  Peak memory: " << formatBytes(snapshot.peakMemory) << std::endl;
        outFile << std::endl;
    }
    
    outFile.close();
    std::cout << "Memory profile exported to: " << filename << std::endl;
}

double MemoryProfiler::getAverageMemoryUsage() const {
    if (memoryHistory.empty()) return 0.0;
    
    double total = 0.0;
    for (const auto& snapshot : memoryHistory) {
        total += snapshot.totalMemory;
    }
    
    return total / memoryHistory.size();
}

double MemoryProfiler::getPeakMemoryUsage() const {
    if (memoryHistory.empty()) return 0.0;
    
    double peak = 0.0;
    for (const auto& snapshot : memoryHistory) {
        peak = std::max(peak, static_cast<double>(snapshot.peakMemory));
    }
    
    return peak;
}

size_t MemoryProfiler::getCurrentMemoryUsage() const {
    if (memoryHistory.empty()) return 0;
    return memoryHistory.back().totalMemory;
}

// Benchmark Suite Implementation
BenchmarkSuite::BenchmarkSuite(const std::string& suiteName) : name(suiteName) {}

void BenchmarkSuite::addConfiguration(size_t neurons, size_t steps) {
    configurations.push_back(std::make_pair(neurons, steps));
}

void BenchmarkSuite::addConfiguration(const std::pair<size_t, size_t>& config) {
    configurations.push_back(config);
}

const std::vector<BenchmarkResult>& BenchmarkSuite::getResults() const {
    return results;
}

void BenchmarkSuite::runBenchmarks() {
    results.clear();
    
    for (const auto& config : configurations) {
        runBenchmark(config.first, config.second);
    }
}

void BenchmarkSuite::runBenchmark(size_t neurons, size_t steps) {
    std::cout << "Running benchmark: " << neurons << " neurons, " << steps << " steps..." << std::endl;
    
    BenchmarkResult result(name, neurons, steps);
    
    // Simulate benchmark execution
    auto startTime = std::chrono::high_resolution_clock::now();
    
    // Simulate some work
    for (size_t i = 0; i < steps; ++i) {
        // Simulate neuron processing
        for (size_t j = 0; j < neurons; ++j) {
            // Simulate some computation
            double temp = static_cast<double>(j) * 0.001;
            temp = temp * temp + 0.5;
        }
        
        // Simulate spike generation
        for (size_t j = 0; j < neurons / 10; ++j) {
            double spikeProb = 0.01 + static_cast<double>(j) / neurons * 0.1;
            if ((rand() % 1000) / 1000.0 < spikeProb) {
                result.spikesPerStep += 1.0;
            }
        }
    }
    
    auto endTime = std::chrono::high_resolution_clock::now();
    result.totalTime = std::chrono::duration<double>(endTime - startTime).count();
    
    if (result.totalTime > 0.0) {
        result.stepsPerSecond = steps / result.totalTime;
        result.spikesPerStep /= steps;
    }
    
    result.synapsesPerNeuron = static_cast<double>(neurons) * 0.1; // Estimate
    result.memoryUsage = neurons * 100; // Estimate in KB
    
    // Add some metrics
    PerformanceMetrics timeMetric("Execution Time", result.totalTime, "seconds");
    PerformanceMetrics throughputMetric("Throughput", result.stepsPerSecond, "steps/second");
    PerformanceMetrics activityMetric("Activity", result.spikesPerStep, "spikes/step");
    
    result.metrics.push_back(timeMetric);
    result.metrics.push_back(throughputMetric);
    result.metrics.push_back(activityMetric);
    
    results.push_back(result);
    
    std::cout << "  Completed in " << result.totalTime << "s" << std::endl;
    std::cout << "  Throughput: " << result.stepsPerSecond << " steps/s" << std::endl;
    std::cout << "  Activity: " << result.spikesPerStep << " spikes/step" << std::endl;
    std::cout << std::endl;
}

void BenchmarkSuite::printResults() const {
    std::cout << "=== Benchmark Results ===" << std::endl;
    std::cout << "Suite: " << name << std::endl;
    std::cout << "Configurations: " << configurations.size() << std::endl;
    std::cout << std::endl;
    
    std::cout << "Benchmark Summary:" << std::endl;
    std::cout << "  Neurons     | Steps      | Total Time | Steps/sec | Spikes/step | Synapses/neuron | Mem usage" << std::endl;
    std::cout << "  ----------- | ---------- | ---------- | --------- | ----------- | --------------- | --------" << std::endl;
    
    for (const auto& result : results) {
        std::cout << "  " << std::setw(11) << result.neurons 
                  << " | " << std::setw(10) << result.steps
                  << " | " << std::setw(10) << std::fixed << std::setprecision(3) << result.totalTime
                  << " | " << std::setw(9) << std::fixed << std::setprecision(0) << result.stepsPerSecond
                  << " | " << std::setw(12) << std::fixed << std::setprecision(3) << result.spikesPerStep
                  << " | " << std::setw(16) << std::fixed << std::setprecision(1) << result.synapsesPerNeuron
                  << " | " << std::setw(9) << std::fixed << std::setprecision(1) << (result.memoryUsage / 1024.0) << "KB" << std::endl;
    }
}

void BenchmarkSuite::exportResults(const std::string& filename) const {
    std::ofstream outFile(filename);
    if (!outFile.is_open()) {
        std::cerr << "ERROR: Cannot open output file: " << filename << std::endl;
        return;
    }
    
    outFile << "NLM Benchmark Export" << std::endl;
    outFile << "=====================" << std::endl;
    outFile << "Generated: " << getCurrentTimestamp() << std::endl;
    outFile << "Suite: " << name << std::endl;
    outFile << "Configurations: " << configurations.size() << std::endl;
    outFile << std::endl;
    
    for (const auto& result : results) {
        outFile << "Test: " << result.testName << std::endl;
        outFile << "  Neurons: " << result.neurons << std::endl;
        outFile << "  Steps: " << result.steps << std::endl;
        outFile << "  Total Time: " << result.totalTime << "s" << std::endl;
        outFile << "  Steps per second: " << result.stepsPerSecond << std::endl;
        outFile << "  Spikes per step: " << result.spikesPerStep << std::endl;
        outFile << "  Synapses per neuron: " << result.synapsesPerNeuron << std::endl;
        outFile << "  Memory usage: " << (result.memoryUsage / 1024.0) << "KB" << std::endl;
        outFile << std::endl;
    }
    
    outFile.close();
    std::cout << "Benchmark results exported to: " << filename << std::endl;
}

void BenchmarkSuite::analyzeTrends() const {
    std::cout << "=== Benchmark Trend Analysis ===" << std::endl;
    
    if (results.size() < 2) {
        std::cout << "Insufficient data for trend analysis (need at least 2 results)" << std::endl;
        return;
    }
    
    // Analyze scaling with neuron count
    std::cout << "Scaling analysis:" << std::endl;
    std::cout << "  Neurons | Steps/sec | Efficiency" << std::endl;
    std::cout << "  ------- | -------- | ---------" << std::endl;
    
    for (size_t i = 1; i < results.size(); ++i) {
        const auto& prev = results[i - 1];
        const auto& curr = results[i];
        
        double neuronRatio = static_cast<double>(curr.neurons) / prev.neurons;
        double stepsRatio = static_cast<double>(curr.stepsPerSecond) / prev.stepsPerSecond;
        double efficiency = stepsRatio / neuronRatio;
        
        std::cout << "  " << std::setw(7) << curr.neurons 
                  << " | " << std::setw(9) << std::fixed << std::setprecision(0) << curr.stepsPerSecond
                  << " | " << std::setw(11) << std::fixed << std::setprecision(3) << efficiency << std::endl;
    }
}

void BenchmarkSuite::generateReport() const {
    printResults();
    analyzeTrends();
    
    std::cout << std::endl;
    std::cout << "=== Performance Recommendations ===" << std::endl;
    std::cout << "1. Consider scaling optimizations for larger networks" << std::endl;
    std::cout << "2. Monitor memory usage for neuron counts above 1000" << std::endl;
    std::cout << "3. Implement SIMD optimizations for mathematical operations" << std::endl;
    std::cout << "4. Consider sparse connectivity for very large networks" << std::endl;
    std::cout << "5. Use checkpointing for long-running simulations" << std::endl;
}

// SIMD Utils Implementation
void SIMDUtils::initialize() {
    // Initialize SIMD extensions if available
    // This is a placeholder for actual SIMD initialization
}

bool SIMDUtils::isAvailable() {
    // Check if SIMD instructions are available
    return true; // Placeholder
}

bool SIMDUtils::isAVX2Available() {
    // Check for AVX2 support
    return true; // Placeholder
}

bool SIMDUtils::isAVX512Available() {
    // Check for AVX512 support
    return true; // Placeholder
}

void SIMDUtils::vectorAdd(const float* a, const float* b, float* result, size_t size) {
    for (size_t i = 0; i < size; ++i) {
        result[i] = a[i] + b[i];
    }
}

void SIMDUtils::vectorMultiply(const float* a, const float* b, float* result, size_t size) {
    for (size_t i = 0; i < size; ++i) {
        result[i] = a[i] * b[i];
    }
}

void SIMDUtils::vectorScale(float* data, float scale, size_t size) {
    for (size_t i = 0; i < size; ++i) {
        data[i] = data[i] * scale;
    }
}

float SIMDUtils::vectorSum(const float* data, size_t size) {
    float sum = 0.0f;
    for (size_t i = 0; i < size; ++i) {
        sum += data[i];
    }
    return sum;
}

float SIMDUtils::vectorDotProduct(const float* a, const float* b, size_t size) {
    float dot = 0.0f;
    for (size_t i = 0; i < size; ++i) {
        dot += a[i] * b[i];
    }
    return dot;
}

void SIMDUtils::simulateNeuronsOptimized(float* potentials, const float* currents,
                                         float threshold, float leak, size_t size, size_t steps) {
    for (size_t step = 0; step < steps; ++step) {
        for (size_t i = 0; i < size; ++i) {
            // LIF dynamics: V = V + dt * (-g_L * (V - E_L) + I_total)
            potentials[i] += (-leak * (potentials[i] - (-70.0f)) + currents[i]) * 0.001f;
            
            if (potentials[i] >= threshold) {
                potentials[i] = -70.0f; // Reset
            }
        }
    }
}

void SIMDUtils::applySynapticInputsOptimized(float* potentials, const float* weights,
                                            const int* targets, float* inputs, size_t count) {
    for (size_t i = 0; i < count; ++i) {
        int target = targets[i];
        if (target >= 0 && target < static_cast<int>(count)) {
            potentials[target] += weights[i];
            inputs[i] = weights[i]; // Store input for recording
        }
    }
}

void SIMDUtils::copyAligned(float* dest, const float* src, size_t size) {
    std::copy(src, src + size, dest);
}

void SIMDUtils::zeroAligned(float* data, size_t size) {
    std::fill(data, data + size, 0.0f);
}

// Performance Monitor Implementation
PerformanceMonitor::PerformanceMonitor() {
    startTimer();
    reset();
}

void PerformanceMonitor::startTimer() {
    startTime = std::chrono::high_resolution_clock::now();
}

void PerformanceMonitor::stopTimer() {
    auto endTime = std::chrono::high_resolution_clock::now();
    double duration = std::chrono::duration<double>(endTime - startTime).count();
    
    totalTime += duration;
    averageTimePerOperation = (totalOperations > 0) ? (totalTime / totalOperations) : 0.0;
    operationsPerSecond = (totalTime > 0.0) ? (totalOperations / totalTime) : 0.0;
}

void PerformanceMonitor::incrementOperations(size_t count) {
    totalOperations += count;
}

void PerformanceMonitor::reset() {
    totalOperations = 0;
    totalTime = 0.0;
    operationsPerSecond = 0.0;
    averageTimePerOperation = 0.0;
}

double PerformanceMonitor::getTotalTime() const {
    return totalTime;
}

double PerformanceMonitor::getAverageTime() const {
    return averageTimePerOperation;
}

size_t PerformanceMonitor::getTotalOperations() const {
    return totalOperations;
}

double PerformanceMonitor::getOperationsPerSecond() const {
    return operationsPerSecond;
}

void PerformanceMonitor::printStats() const {
    std::cout << "=== Performance Monitor Statistics ===" << std::endl;
    std::cout << "Total operations: " << totalOperations << std::endl;
    std::cout << "Total time: " << totalTime << "s" << std::endl;
    std::cout << "Average time per operation: " << averageTimePerOperation << "s" << std::endl;
    std::cout << "Operations per second: " << operationsPerSecond << std::endl;
}

void PerformanceMonitor::exportStats(const std::string& filename) const {
    std::ofstream outFile(filename);
    if (!outFile.is_open()) {
        std::cerr << "ERROR: Cannot open output file: " << filename << std::endl;
        return;
    }
    
    outFile << "NLM Performance Monitor Export" << std::endl;
    outFile << "===============================" << std::endl;
    outFile << "Generated: " << getCurrentTimestamp() << std::endl;
    outFile << "Total operations: " << totalOperations << std::endl;
    outFile << "Total time: " << totalTime << "s" << std::endl;
    outFile << "Average time per operation: " << averageTimePerOperation << "s" << std::endl;
    outFile << "Operations per second: " << operationsPerSecond << std::endl;
    
    outFile.close();
    std::cout << "Performance monitor statistics exported to: " << filename << std::endl;
}

// Global convenience functions
void startProfiling(const std::string& operation) {
    getProfiler().startProfile(operation);
}

void endProfiling() {
    getProfiler().endProfile();
}

void incrementIterationCount() {
    getProfiler().incrementIteration();
}

void takeMemorySnapshot() {
    getMemoryProfiler().takeSnapshot();
}

double getCurrentFPS() {
    // Placeholder implementation
    return 60.0;
}

double getAverageFPS() {
    // Placeholder implementation
    return 59.0;
}

size_t getTotalFrames() {
    // Placeholder implementation
    return 1000;
}

void printPerformanceReport() {
    getProfiler().printReport();
    getMemoryProfiler().printReport();
    getBenchmarkSuite().printResults();
}

void exportPerformanceReport(const std::string& filename) {
    getProfiler().exportReport(filename + "_profile.txt");
    getMemoryProfiler().exportReport(filename + "_memory.txt");
    getBenchmarkSuite().exportResults(filename + "_benchmark.csv");
}

namespace {
    size_t getSystemMemoryUsage() {
        // Placeholder implementation - return 0 for now
        return 0;
    }
    
    size_t getPeakMemoryUsage() {
        // Placeholder implementation - return 0 for now
        return 0;
    }
    
    std::string formatBytes(size_t bytes) {
        std::stringstream ss;
        if (bytes < 1024) {
            ss << bytes << " B";
        } else if (bytes < 1024 * 1024) {
            ss << bytes / 1024.0 << " KB";
        } else if (bytes < 1024 * 1024 * 1024) {
            ss << bytes / (1024.0 * 1024.0) << " MB";
        } else {
            ss << bytes / (1024.0 * 1024.0 * 1024.0) << " GB";
        }
        return ss.str();
    }
    
    std::string getCurrentTimestamp() {
        auto now = std::chrono::system_clock::now();
        auto time = std::chrono::system_clock::to_time_t(now);
        
        std::stringstream ss;
        ss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
        return ss.str();
    }
}