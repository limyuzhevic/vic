#include "PerformanceMonitor.hpp"

#include <chrono>
#include <thread>
#include <sstream>
#include <iomanip>
#include <iostream>
#include <atomic>
#include <algorithm>
#include <numeric>

namespace nlm {

struct PerformanceMonitor::Impl {
    // Timing
    std::chrono::steady_clock::time_point startTime;
    std::chrono::steady_clock::time_point lastStepTime;
    std::chrono::steady_clock::time_point lastUpdateTime;
    double updateInterval;
    bool monitoringCPU;
    bool monitoringMemory;
    bool monitoringNetwork;
    bool monitoringPlasticity;
    
    // Metrics
    PerformanceMetrics currentMetrics;
    std::vector<PerformanceMetrics> metricsHistory;
    size_t maxHistorySize;
    
    // Timing accumulators
    double totalStepTime;
    double stepCount;
    
    // Memory tracking
    size_t peakMemoryUsed;
    size_t totalMemoryAllocated;
    size_t totalMemoryDeallocated;
    
    // Network tracking
    size_t totalSpikes;
    size_t totalSTDPUpdates;
    size_t totalHebbianUpdates;
    size_t totalStructuralPlasticityEvents;
    
    // Benchmark tracking
    struct BenchmarkResult {
        std::string operation;
        double timePerOperation;
        double operationsPerSecond;
        size_t operationCount;
    };
    
    std::vector<BenchmarkResult> benchmarks;
    
    // Threading
    std::atomic<bool> monitoringActive;
    std::thread monitorThread;
    mutable std::mutex mutex;
    
    Impl() 
        : updateInterval(1.0) // 1 second updates
        , monitoringCPU(true)
        , monitoringMemory(true)
        , monitoringNetwork(true)
        , monitoringPlasticity(true)
        , maxHistorySize(1000)
        , totalStepTime(0.0)
        , stepCount(0.0)
        , peakMemoryUsed(0)
        , totalMemoryAllocated(0)
        , totalMemoryDeallocated(0)
        , totalSpikes(0)
        , totalSTDPUpdates(0)
        , totalHebbianUpdates(0)
        , totalStructuralPlasticityEvents(0)
        , monitoringActive(false) {}
};

PerformanceMonitor::PerformanceMonitor() : pImpl(std::make_unique<Impl>()) {}

PerformanceMonitor::~PerformanceMonitor() {
    stop();
}

void PerformanceMonitor::start() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    if (pImpl->monitoringActive) {
        return;
    }
    
    pImpl->startTime = std::chrono::steady_clock::now();
    pImpl->lastStepTime = pImpl->startTime;
    pImpl->lastUpdateTime = pImpl->startTime;
    pImpl->currentMetrics.wallClockTime = 0.0;
    pImpl->monitoringActive = true;
    
    // Start monitoring thread
    pImpl->monitorThread = std::thread([this]() {
        while (pImpl->monitoringActive) {
            std::this_thread::sleep_for(std::chrono::milliseconds(static_cast<int>(pImpl->updateInterval * 1000)));
            updateMetrics();
        }
    });
}

void PerformanceMonitor::stop() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    if (!pImpl->monitoringActive) {
        return;
    }
    
    pImpl->monitoringActive = false;
    
    if (pImpl->monitorThread.joinable()) {
        pImpl->monitorThread.join();
    }
    
    // Final metrics update
    updateMetrics();
}

bool PerformanceMonitor::isMonitoring() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    return pImpl->monitoringActive;
}

void PerformanceMonitor::setUpdateInterval(double interval) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->updateInterval = std::max(0.1, interval); // Minimum 100ms
}

void PerformanceMonitor::setMonitoringCPU(bool enable) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->monitoringCPU = enable;
}

void PerformanceMonitor::setMonitoringMemory(bool enable) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->monitoringMemory = enable;
}

void PerformanceMonitor::setMonitoringNetwork(bool enable) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->monitoringNetwork = enable;
}

void PerformanceMonitor::setMonitoringPlasticity(bool enable) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->monitoringPlasticity = enable;
}

PerformanceMetrics PerformanceMonitor::getCurrentMetrics() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    return pImpl->currentMetrics;
}

std::vector<PerformanceMetrics> PerformanceMonitor::getMetricsHistory(size_t count) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    std::vector<PerformanceMetrics> result;
    size_t startIndex = std::max(size_t(0), pImpl->metricsHistory.size() - count);
    
    for (size_t i = startIndex; i < pImpl->metricsHistory.size(); ++i) {
        result.push_back(pImpl->metricsHistory[i]);
    }
    
    return result;
}

void PerformanceMonitor::benchmarkMemoryAllocation(size_t size) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    auto start = std::chrono::steady_clock::now();
    
    // Allocate memory
    std::vector<char> buffer(size);
    std::fill(buffer.begin(), buffer.end(), 0);
    
    auto end = std::chrono::steady_clock::now();
    
    double time = std::chrono::duration<double>(end - start).count();
    double opsPerSecond = (size / 1024.0 / 1024.0) / time * 1024 * 1024; // MB/s
    
    pImpl->benchmarks.push_back({
        "Memory Allocation",
        time,
        opsPerSecond,
        1
    });
}

void PerformanceMonitor::benchmarkSpikeProcessing(size_t spikeCount) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    auto start = std::chrono::steady_clock::now();
    
    // Simulate spike processing
    for (size_t i = 0; i < spikeCount; ++i) {
        // Dummy spike processing
        double dummy = std::sin(i * 0.01);
    }
    
    auto end = std::chrono::steady_clock::now();
    
    double time = std::chrono::duration<double>(end - start).count();
    double opsPerSecond = spikeCount / time;
    
    pImpl->benchmarks.push_back({
        "Spike Processing",
        time,
        opsPerSecond,
        spikeCount
    });
}

void PerformanceMonitor::benchmarkSynapseProcessing(size_t synapseCount) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    auto start = std::chrono::steady_clock::now();
    
    // Simulate synapse processing
    for (size_t i = 0; i < synapseCount; ++i) {
        // Dummy synapse processing
        double dummy = std::cos(i * 0.01);
    }
    
    auto end = std::chrono::steady_clock::now();
    
    double time = std::chrono::duration<double>(end - start).count();
    double opsPerSecond = synapseCount / time;
    
    pImpl->benchmarks.push_back({
        "Synapse Processing",
        time,
        opsPerSecond,
        synapseCount
    });
}

void PerformanceMonitor::reset() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    pImpl->currentMetrics = PerformanceMetrics();
    pImpl->metricsHistory.clear();
    pImpl->totalStepTime = 0.0;
    pImpl->stepCount = 0.0;
    pImpl->peakMemoryUsed = 0;
    pImpl->totalMemoryAllocated = 0;
    pImpl->totalMemoryDeallocated = 0;
    pImpl->totalSpikes = 0;
    pImpl->totalSTDPUpdates = 0;
    pImpl->totalHebbianUpdates = 0;
    pImpl->totalStructuralPlasticityEvents = 0;
    pImpl->benchmarks.clear();
}

bool PerformanceMonitor::isActive() const {
    return isMonitoring();
}

std::string PerformanceMonitor::getSummary() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    std::stringstream ss;
    ss << "=== Performance Monitor Summary ===" << std::endl;
    ss << "Monitoring Active: " << (pImpl->monitoringActive ? "Yes" : "No") << std::endl;
    ss << "Update Interval: " << pImpl->updateInterval << "s" << std::endl;
    ss << "History Size: " << pImpl->metricsHistory.size() << "/" << pImpl->maxHistorySize << std::endl;
    ss << std::endl;
    ss << "Current Metrics:" << std::endl;
    ss << pImpl->currentMetrics.toString() << std::endl;
    ss << "Benchmarks:" << std::endl;
    
    for (const auto& benchmark : pImpl->benchmarks) {
        ss << "  " << benchmark.operation << ": "
           << std::fixed << std::setprecision(6) << benchmark.timePerOperation << "s per operation, "
           << std::fixed << std::setprecision(1) << benchmark.operationsPerSecond << " ops/sec" << std::endl;
    }
    
    return ss.str();
}

void PerformanceMonitor::updateMetrics() {
    auto now = std::chrono::steady_clock::now();
    double elapsed = std::chrono::duration<double>(now - pImpl->lastUpdateTime).count();
    
    if (elapsed < pImpl->updateInterval) {
        return;
    }
    
    // Calculate wall clock time
    pImpl->currentMetrics.wallClockTime += elapsed;
    
    // Calculate performance metrics
    if (pImpl->stepCount > 0) {
        pImpl->currentMetrics.stepTime = pImpl->totalStepTime / pImpl->stepCount;
    }
    
    if (pImpl->currentMetrics.wallClockTime > 0) {
        pImpl->currentMetrics.stepsPerSecond = pImpl->stepCount / pImpl->currentMetrics.wallClockTime;
    }
    
    // Update history
    pImpl->metricsHistory.push_back(pImpl->currentMetrics);
    
    if (pImpl->metricsHistory.size() > pImpl->maxHistorySize) {
        pImpl->metricsHistory.erase(pImpl->metricsHistory.begin());
    }
    
    pImpl->lastUpdateTime = now;
}

void PerformanceMonitor::recordStepTime(double time) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    pImpl->totalStepTime += time;
    pImpl->stepCount++;
    
    if (pImpl->monitoringCPU) {
        pImpl->currentMetrics.cpuTime += time;
    }
}

void PerformanceMonitor::recordMemoryUsage() {
    if (!pImpl->monitoringMemory) {
        return;
    }
    
    // Simple memory tracking - could be enhanced with platform-specific functions
    // For now, we'll use a dummy implementation
    static size_t dummyCounter = 0;
    pImpl->currentMetrics.memoryUsed = ++dummyCounter * 1024; // Fake memory usage
    pImpl->peakMemoryUsed = std::max(pImpl->peakMemoryUsed, pImpl->currentMetrics.memoryUsed);
}

void PerformanceMonitor::recordNetworkMetrics() {
    if (!pImpl->monitoringNetwork) {
        return;
    }
    
    // Update network metrics
    pImpl->currentMetrics.totalNeuronCount = 1000; // Placeholder
    pImpl->currentMetrics.totalSynapseCount = 100000; // Placeholder
    pImpl->currentMetrics.activeNeuronCount = static_cast<size_t>(pImpl->currentMetrics.totalNeuronCount * 0.1); // Placeholder
    pImpl->currentMetrics.firingNeuronCount = static_cast<size_t>(pImpl->currentMetrics.activeNeuronCount * 0.05); // Placeholder
    pImpl->currentMetrics.spikeCount = pImpl->totalSpikes;
    
    if (pImpl->currentMetrics.activeNeuronCount > 0) {
        pImpl->currentMetrics.neuronsPerSecond = 
            pImpl->currentMetrics.firingNeuronCount / pImpl->currentMetrics.wallClockTime;
        pImpl->currentMetrics.synapsesPerSecond = 
            pImpl->currentMetrics.totalSynapseCount / pImpl->currentMetrics.wallClockTime;
    }
}

void PerformanceMonitor::recordPlasticityMetrics() {
    if (!pImpl->monitoringPlasticity) {
        return;
    }
    
    // Update plasticity metrics
    pImpl->currentMetrics.stdpUpdates = pImpl->totalSTDPUpdates;
    pImpl->currentMetrics.hebbianUpdates = pImpl->totalHebbianUpdates;
    pImpl->currentMetrics.structuralPlasticityEvents = pImpl->totalStructuralPlasticityEvents;
}

} // namespace nlm