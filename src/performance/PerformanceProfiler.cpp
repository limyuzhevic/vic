#include "PerformanceProfiler.hpp"
#include <algorithm>
#include <iomanip>

namespace nlm {

// PerformanceProfiler implementation

void PerformanceProfiler::startStep(SimulationStep step) {
    std::lock_guard<std::mutex> lock(mutex_);
    currentStep_ = step;
    stepStartTime_ = std::chrono::high_resolution_clock::now();
    timers_.push_back(PerformanceTimer());
}

PerformanceProfiler::CompleteMetrics PerformanceProfiler::endStep() {
    std::lock_guard<std::mutex> lock(mutex_);
    auto endTime = std::chrono::high_resolution_clock::now();
    double wallClockTime = std::chrono::duration_cast<std::chrono::duration<double>>(endTime - stepStartTime_).count();
    
    CompleteMetrics metrics;
    metrics.simulationStep = currentStep_;
    metrics.wallClockTime = wallClockTime;
    
    // Collect metrics from subsystems
    metrics.memoryMetrics = memoryTracker_.getMetrics();
    metrics.cacheMetrics = cacheMonitor_.getMetrics();
    metrics.simdMetrics = simdTracker_.getMetrics();
    metrics.parallelMetrics = parallelProfiler_.getMetrics();
    
    // Calculate derived metrics
    if (metrics.memoryMetrics.allocatedBytes > 0) {
        metrics.memoryEfficiency = static_cast<double>(metrics.memoryMetrics.usedBytes) / 
                                  metrics.memoryMetrics.allocatedBytes;
    }
    metrics.cacheHitRate = metrics.cacheMetrics.hitRate;
    metrics.simdUtilization = metrics.simdMetrics.simdUtilizationRatio;
    metrics.parallelEfficiency = metrics.parallelMetrics.parallelEfficiency;
    
    // Update task count for parallel metrics
    parallelProfiler_.updateParallelEfficiency();
    
    // Reset timers for next step
    timers_.clear();
    
    return metrics;
}

void PerformanceProfiler::recordMemoryAllocation(uint64_t bytes) {
    std::lock_guard<std::mutex> lock(mutex_);
    memoryTracker_.recordAllocation(bytes);
}

void PerformanceProfiler::recordMemoryDeallocation(uint64_t bytes) {
    std::lock_guard<std::mutex> lock(mutex_);
    memoryTracker_.recordDeallocation(bytes);
}

void PerformanceProfiler::recordCacheAccess(bool hit) {
    std::lock_guard<std::mutex> lock(mutex_);
    cacheMonitor_.recordAccess(hit);
}

void PerformanceProfiler::recordCacheFetch() {
    std::lock_guard<std::mutex> lock(mutex_);
    cacheMonitor_.recordFetch();
}

void PerformanceProfiler::recordCacheEviction() {
    std::lock_guard<std::mutex> lock(mutex_);
    cacheMonitor_.recordEviction();
}

void PerformanceProfiler::recordSIMDOperation(const std::string& operationType, uint64_t count) {
    std::lock_guard<std::mutex> lock(mutex_);
    if (operationType == "LIF") {
        simdTracker_.recordLIFSIMD(count);
    } else if (operationType == "Synapse") {
        simdTracker_.recordSynapseSIMD(count);
    } else if (operationType == "Memory") {
        simdTracker_.recordMemorySIMD(count);
    } else if (operationType == "Similarity") {
        simdTracker_.recordSimilaritySIMD(count);
    }
}

void PerformanceProfiler::recordParallelTask(uint64_t duration) {
    std::lock_guard<std::mutex> lock(mutex_);
    parallelProfiler_.recordParallelTask(duration);
}

void PerformanceProfiler::recordSerialTask(uint64_t duration) {
    std::lock_guard<std::mutex> lock(mutex_);
    parallelProfiler_.recordSerialTask(duration);
}

void PerformanceProfiler::updateSIMDUtilization(uint64_t totalOperations) {
    std::lock_guard<std::mutex> lock(mutex_);
    simdTracker_.updateUtilizationRatio(totalOperations);
}

PerformanceProfiler::CompleteMetrics PerformanceProfiler::getCompleteMetrics() const {
    std::lock_guard<std::mutex> lock(mutex_);
    CompleteMetrics metrics;
    metrics.memoryMetrics = memoryTracker_.getMetrics();
    metrics.cacheMetrics = cacheMonitor_.getMetrics();
    metrics.simdMetrics = simdTracker_.getMetrics();
    metrics.parallelMetrics = parallelProfiler_.getMetrics();
    
    if (metrics.memoryMetrics.allocatedBytes > 0) {
        metrics.memoryEfficiency = static_cast<double>(metrics.memoryMetrics.usedBytes) / 
                                  metrics.memoryMetrics.allocatedBytes;
    }
    metrics.cacheHitRate = metrics.cacheMetrics.hitRate;
    metrics.simdUtilization = metrics.simdMetrics.simdUtilizationRatio;
    metrics.parallelEfficiency = metrics.parallelMetrics.parallelEfficiency;
    
    return metrics;
}

void PerformanceProfiler::printPerformanceReport(const CompleteMetrics& metrics) const {
    std::cout << "=== Performance Report ===" << std::endl;
    std::cout << "Simulation Step: " << metrics.simulationStep << std::endl;
    std::cout << "Wall Clock Time: " << std::fixed << std::setprecision(4) 
              << metrics.wallClockTime << " seconds" << std::endl;
    
    std::cout << "\nMemory Usage:" << std::endl;
    std::cout << "  Allocated: " << metrics.memoryMetrics.allocatedBytes << " bytes" << std::endl;
    std::cout << "  Used:      " << metrics.memoryMetrics.usedBytes << " bytes" << std::endl;
    std::cout << "  Peak:      " << metrics.memoryMetrics.peakAllocatedBytes << " bytes" << std::endl;
    std::cout << "  Efficiency: " << std::fixed << std::setprecision(2) 
              << metrics.memoryEfficiency * 100 << "%" << std::endl;
    
    std::cout << "\nCache Performance:" << std::endl;
    std::cout << "  Hits:       " << metrics.cacheMetrics.cacheHits << std::endl;
    std::cout << "  Misses:     " << metrics.cacheMetrics.cacheMisses << std::endl;
    std::cout << "  Hit Rate:   " << std::fixed << std::setprecision(2) 
              << metrics.cacheHitRate * 100 << "%" << std::endl;
    std::cout << "  Total Accesses: " << metrics.cacheMetrics.totalAccesses << std::endl;
    
    std::cout << "\nSIMD Utilization:" << std::endl;
    std::cout << "  LIF Operations:     " << metrics.simdMetrics.lifSimdOperations << std::endl;
    std::cout << "  Synapse Operations: " << metrics.simdMetrics.synapseSimdOperations << std::endl;
    std::cout << "  Memory Operations:  " << metrics.simdMetrics.memorySimdOperations << std::endl;
    std::cout << "  Similarity Ops:    " << metrics.simdMetrics.similaritySimdOperations << std::endl;
    std::cout << "  Total SIMD Ops:    " << metrics.simdMetrics.totalSimdOperations << std::endl;
    std::cout << "  Utilization Ratio: " << std::fixed << std::setprecision(2) 
              << metrics.simdUtilization * 100 << "%" << std::endl;
    
    std::cout << "\nParallel Execution:" << std::endl;
    std::cout << "  Parallel Tasks:  " << metrics.parallelMetrics.parallelTasks << std::endl;
    std::cout << "  Serial Tasks:    " << metrics.parallelMetrics.serialTasks << std::endl;
    std::cout << "  Total Tasks:     " << metrics.parallelMetrics.totalTasks << std::endl;
    std::cout << "  Efficiency:      " << std::fixed << std::setprecision(2) 
              << metrics.parallelEfficiency * 100 << "%" << std::endl;
    std::cout << "  Thread Pool Size:" << metrics.parallelMetrics.threadPoolSize << std::endl;
    std::cout << "  Avg Task Duration:" << metrics.parallelMetrics.averageTaskDuration << " μs" << std::endl;
    
    std::cout << "\n=== End Performance Report ===" << std::endl;
}

void PerformanceProfiler::reset() {
    std::lock_guard<std::mutex> lock(mutex_);
    currentStep_ = 0;
    simulationStartTime_ = std::chrono::high_resolution_clock::now();
    memoryTracker_.reset();
    cacheMonitor_.reset();
    simdTracker_.reset();
    parallelProfiler_.reset();
}

} // namespace nlm