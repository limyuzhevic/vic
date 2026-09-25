#include "Monitor.hpp"
#include "../src/brain/NeuronPool.hpp"
#include "../src/brain/SynapsePool.hpp"
#include "../src/performance/EventQueue.hpp"
#include "../src/performance/ParallelProcessing.hpp"
#include "../src/performance/SIMDVectorization.hpp"
#include "../src/brain/Brain.hpp"
#include <numeric>
#include <algorithm>
#include <fstream>
#include <iomanip>
#include <cmath>
#include <thread>

namespace nlm {

PerformanceMonitor::PerformanceMonitor()
    : enabled_(true)
    , monitoringInterval_(0.1)  // 100 ms
    , snapshotInterval_(1.0)    // 1 second
    , timelineEnabled_(true)
    , brain_(nullptr)
    , neuronPool_(nullptr)
    , synapsePool_(nullptr)
    , eventQueue_(nullptr)
    , parallelProc_(nullptr)
    , simd_(nullptr)
    , currentStep_(0)
    , currentTime_(0.0)
    , totalMemoryAllocated_(0)
    , totalMemoryDeallocated_(0)
    , neuronPoolAllocated_(0)
    , neuronPoolAvailable_(0)
    , synapsePoolAllocated_(0)
    , synapsePoolAvailable_(0)
    , eventQueueSize_(0)
    , eventQueueFillRate_(0.0)
    , totalEventProcessingTime_(0.0)
    , totalEventsProcessed_(0)
    , simdVectorizedOps_(0)
    , simdTotalOps_(0)
    , simdUtilization_(0.0)
    , activeThreads_(0)
    , parallelEfficiency_(0.0) {
    
    globalStartTime_ = std::chrono::high_resolution_clock::now();
}

PerformanceMonitor::~PerformanceMonitor() {
    flush();
}

PerformanceMonitor& PerformanceMonitor::getInstance() {
    static PerformanceMonitor instance;
    return instance;
}

void PerformanceMonitor::enable(bool enable) {
    enabled_ = enable;
    if (enabled_) {
        globalStartTime_ = std::chrono::high_resolution_clock::now();
    }
}

void PerformanceMonitor::setMonitoringInterval(double interval) {
    monitoringInterval_ = std::max(0.01, interval);
}

void PerformanceMonitor::setSnapshotInterval(double interval) {
    snapshotInterval_ = std::max(0.1, interval);
}

void PerformanceMonitor::setTimelineEnabled(bool enable) {
    timelineEnabled_ = enable;
}

void PerformanceMonitor::integrateWithBrain(nlm::Brain* brain) {
    brain_ = brain;
}

void PerformanceMonitor::integrateWithMemoryPool(nlm::NeuronPool* neuronPool) {
    neuronPool_ = neuronPool;
}

void PerformanceMonitor::integrateWithMemoryPool(nlm::SynapsePool* synapsePool) {
    synapsePool_ = synapsePool;
}

void PerformanceMonitor::integrateWithEventQueue(nlm::EventQueue* eventQueue) {
    eventQueue_ = eventQueue;
}

void PerformanceMonitor::integrateWithParallelProcessing(nlm::ParallelProcessing* parallelProc) {
    parallelProc_ = parallelProc;
}

void PerformanceMonitor::integrateWithSIMD(nlm::SIMDVectorization* simd) {
    simd_ = simd;
}

void PerformanceMonitor::startBenchmark(const std::string& name) {
    if (!enabled_) return;
    
    std::lock_guard<std::mutex> lock(dataMutex_);
    
    auto it = benchmarks_.find(name);
    if (it != benchmarks_.end() && it->second.active) {
        return; // Already running
    }
    
    BenchmarkInfo info;
    info.name = name;
    info.startTime = std::chrono::high_resolution_clock::now();
    info.cpuStart = std::chrono::high_resolution_clock::now();
    info.active = true;
    
    benchmarks_[name] = info;
}

BenchmarkResult PerformanceMonitor::stopBenchmark(const std::string& name) {
    BenchmarkResult result;
    result.benchmarkName = name;
    result.duration = 0.0;
    result.cpuTime = 0.0;
    
    if (!enabled_ || benchmarks_.find(name) == benchmarks_.end()) {
        return result;
    }
    
    std::lock_guard<std::mutex> lock(dataMutex_);
    
    auto& info = benchmarks_[name];
    if (!info.active) {
        return result;
    }
    
    auto endTime = std::chrono::high_resolution_clock::now();
    auto cpuEndTime = std::chrono::high_resolution_clock::now();
    
    auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(endTime - info.startTime);
    auto cpuDuration = std::chrono::duration_cast<std::chrono::nanoseconds>(cpuEndTime - info.cpuStart);
    
    result.duration = duration.count() * 1e-9; // Convert to seconds
    result.cpuTime = cpuDuration.count() * 1e-9;
    
    // Estimate operations and SIMD level
    result.opsPerSecond = 0.0; // Will be calculated based on specific benchmark
    result.simdLevel = getSIMDLevel();
    result.numThreads = activeThreads_.load();
    result.efficiency = parallelEfficiency_.load();
    
    info.active = false;
    benchmarkHistory_.push_back(result);
    
    return result;
}

void PerformanceMonitor::startTimer(const std::string& name) {
    if (!enabled_) return;
    
    std::lock_guard<std::mutex> lock(dataMutex_);
    
    TimerInfo info;
    info.startTime = std::chrono::high_resolution_clock::now();
    info.totalDuration = 0.0;
    info.count = 0;
    
    timers_[name] = info;
}

void PerformanceMonitor::stopTimer(const std::string& name) {
    if (!enabled_) return;
    
    std::lock_guard<std::mutex> lock(dataMutex_);
    
    auto it = timers_.find(name);
    if (it == timers_.end()) return;
    
    auto& info = it->second;
    auto endTime = std::chrono::high_resolution_clock::now();
    
    auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(endTime - info.startTime);
    info.totalDuration += duration.count() * 1e-9; // Convert to seconds
    info.count++;
}

void PerformanceMonitor::incrementCounter(const std::string& name, uint64_t count) {
    if (!enabled_) return;
    
    std::lock_guard<std::mutex> lock(dataMutex_);
    counters_[name] += count;
}

void PerformanceMonitor::updateMetric(const std::string& name, double value) {
    if (!enabled_) return;
    
    std::lock_guard<std::mutex> lock(dataMutex_);
    metrics_[name] = value;
}

void PerformanceMonitor::trackMemoryAllocation(size_t bytes) {
    if (!enabled_) return;
    totalMemoryAllocated_ += bytes;
}

void PerformanceMonitor::trackMemoryDeallocation(size_t bytes) {
    if (!enabled_) return;
    totalMemoryDeallocated_ += bytes;
}

void PerformanceMonitor::trackMemoryPoolUsage(size_t allocated, size_t available) {
    if (!enabled_) return;
    neuronPoolAllocated_ = allocated;
    neuronPoolAvailable_ = available;
}

void PerformanceMonitor::trackEventQueueStats(size_t size, double fillRate) {
    if (!enabled_) return;
    
    std::lock_guard<std::mutex> lock(dataMutex_);
    eventQueueSize_ = size;
    eventQueueFillRate_ = std::min(1.0, fillRate);
}

void PerformanceMonitor::trackEventProcessingTime(double duration, uint64_t eventCount) {
    if (!enabled_) return;
    
    totalEventProcessingTime_ += duration;
    totalEventsProcessed_ += eventCount;
}

void PerformanceMonitor::trackSIMDU utilization(double utilization, const std::string& simdLevel) {
    if (!enabled_) return;
    simdUtilization_ = utilization;
}

void PerformanceMonitor::trackSIMDVectorization(size_t vectorizedOps, size_t totalOps) {
    if (!enabled_) return;
    
    simdVectorizedOps_ += vectorizedOps;
    simdTotalOps_ += totalOps;
}

void PerformanceMonitor::trackThreadStats(size_t activeThreads, double efficiency) {
    if (!enabled_) return;
    
    activeThreads_ = activeThreads;
    parallelEfficiency_ = efficiency;
}

void PerformanceMonitor::trackParallelEfficiency(double efficiency, size_t threads, size_t work) {
    if (!enabled_) return;
    parallelEfficiency_ = efficiency;
    activeThreads_ = threads;
}

PerformanceSnapshot PerformanceMonitor::takeSnapshot() {
    std::lock_guard<std::mutex> lock(dataMutex_);
    return createSnapshot();
}

std::vector<PerformanceSnapshot> PerformanceMonitor::getSnapshots() const {
    std::lock_guard<std::mutex> lock(dataMutex_);
    return snapshots_;
}

void PerformanceMonitor::recordTimelineEvent(const TimelineEvent& event) {
    if (!enabled_ || !timelineEnabled_) return;
    
    std::lock_guard<std::mutex> lock(dataMutex_);
    timelineEvents_.push_back(event);
}

std::vector<TimelineEvent> PerformanceMonitor::getTimelineEvents(const std::string& eventType) const {
    std::lock_guard<std::mutex> lock(dataMutex_);
    
    if (eventType.empty()) {
        return timelineEvents_;
    }
    
    std::vector<TimelineEvent> filtered;
    filtered.reserve(timelineEvents_.size());
    
    for (const auto& event : timelineEvents_) {
        if (event.eventType == eventType) {
            filtered.push_back(event);
        }
    }
    
    return filtered;
}

std::vector<BottleneckAnalysis> PerformanceMonitor::analyzeBottlenecks() {
    std::lock_guard<std::mutex> lock(dataMutex_);
    
    std::vector<BottleneckAnalysis> bottlenecks;
    
    // Analyze different types of bottlenecks
    auto memoryBottlenecks = analyzeMemoryBottlenecks();
    bottlenecks.insert(bottlenecks.end(), memoryBottlenecks.begin(), memoryBottlenecks.end());
    
    auto cpuBottlenecks = analyzeCPUbottlenecks();
    bottlenecks.insert(bottlenecks.end(), cpuBottlenecks.begin(), cpuBottlenecks.end());
    
    auto eventQueueBottlenecks = analyzeEventQueueBottlenecks();
    bottlenecks.insert(bottlenecks.end(), eventQueueBottlenecks.begin(), eventQueueBottlenecks.end());
    
    auto parallelBottlenecks = analyzeParallelBottlenecks();
    bottlenecks.insert(bottlenecks.end(), parallelBottlenecks.begin(), parallelBottlenecks.end());
    
    // Sort by severity
    std::sort(bottlenecks.begin(), bottlenecks.end(),
        [](const BottleneckAnalysis& a, const BottleneckAnalysis& b) {
            return a.severity > b.severity;
        });
    
    return bottlenecks;
}

std::vector<BenchmarkResult> PerformanceMonitor::getBenchmarkResults() const {
    std::lock_guard<std::mutex> lock(dataMutex_);
    return benchmarkHistory_;
}

std::string PerformanceMonitor::generateReport() const {
    std::lock_guard<std::mutex> lock(dataMutex_);
    
    std::stringstream report;
    
    report << "=== NLM Performance Monitoring Report ===" << std::endl;
    report << "Generated at: " << getWallClockTime() << " seconds" << std::endl;
    report << "Total runtime: " << (getWallClockTime() - getWallClockTime()) << " seconds" << std::endl;
    report << std::endl;
    
    // Summary statistics
    report << "=== Summary Statistics ===" << std::endl;
    report << "Total benchmarks: " << benchmarkHistory_.size() << std::endl;
    report << "Memory allocated: " << totalMemoryAllocated_.load() << " bytes" << std::endl;
    report << "Memory deallocated: " << totalMemoryDeallocated_.load() << " bytes" << std::endl;
    report << "Neuron pool allocated: " << neuronPoolAllocated_.load() << " objects" << std::endl;
    report << "Neuron pool available: " << neuronPoolAvailable_.load() << " objects" << std::endl;
    report << "Event queue size: " << eventQueueSize_.load() << std::endl;
    report << "Event queue fill rate: " << eventQueueFillRate_.load() << std::endl;
    report << "SIMD utilization: " << simdUtilization_.load() * 100.0 << "%" << std::endl;
    report << "Active threads: " << activeThreads_.load() << std::endl;
    report << "Parallel efficiency: " << parallelEfficiency_.load() * 100.0 << "%" << std::endl;
    report << std::endl;
    
    // Benchmark results
    if (!benchmarkHistory_.empty()) {
        report << "=== Benchmark Results ===" << std::endl;
        for (const auto& result : benchmarkHistory_) {
            report << formatBenchmarkResult(result) << std::endl;
        }
        report << std::endl;
    }
    
    // Bottleneck analysis
    const_cast<PerformanceMonitor*>(this)->flush();
    auto bottlenecks = const_cast<PerformanceMonitor*>(this)->analyzeBottlenecks();
    
    if (!bottlenecks.empty()) {
        report << "=== Bottleneck Analysis ===" << std::endl;
        for (const auto& bottleneck : bottlenecks) {
            report << formatBottleneckAnalysis(bottleneck) << std::endl;
        }
        report << std::endl;
    }
    
    // Performance snapshots
    if (!snapshots_.empty()) {
        report << "=== Recent Performance Snapshots ===" << std::endl;
        for (size_t i = 0; i < std::min(size_t(10), snapshots_.size()); ++i) {
            report << formatPerformanceSnapshot(snapshots_[snapshots_.size() - 1 - i]) << std::endl;
        }
        report << std::endl;
    }
    
    return report.str();
}

void PerformanceMonitor::saveReport(const std::string& filename) const {
    std::ofstream file(filename);
    if (file.is_open()) {
        file << generateReport();
        file.close();
    }
}

std::vector<std::string> PerformanceMonitor::getOptimizationRecommendations() const {
    std::lock_guard<std::mutex> lock(dataMutex_);
    
    std::vector<std::string> recommendations;
    
    // Check for memory issues
    if (neuronPoolAvailable_.load() < neuronPoolAllocated_.load() * 0.1) {
        recommendations.push_back("Memory pool running low on available objects - consider increasing pool size");
    }
    
    // Check for event queue issues
    if (eventQueueFillRate_.load() > 0.9) {
        recommendations.push_back("Event queue nearly full - consider increasing capacity or optimizing event processing");
    }
    
    // Check for SIMD issues
    if (simdUtilization_.load() < 0.5) {
        recommendations.push_back("SIMD utilization below 50% - consider tuning vectorization or using wider SIMD");
    }
    
    // Check for parallel issues
    if (parallelEfficiency_.load() < 0.5) {
        recommendations.push_back("Parallel efficiency below 50% - check for synchronization bottlenecks or load imbalance");
    }
    
    return recommendations;
}

bool PerformanceMonitor::applyAutoOptimizations() {
    // Apply auto optimizations based on performance analysis
    // This is a placeholder for actual optimization logic
    return false;
}

void PerformanceMonitor::startStep(uint64_t step, double time) {
    std::lock_guard<std::mutex> lock(dataMutex_);
    currentStep_ = step;
    currentTime_ = time;
}

void PerformanceMonitor::endStep(uint64_t step, double time) {
    // Record performance data for this step
    if (step % 100 == 0) { // Take snapshot every 100 steps
        PerformanceSnapshot snapshot = createSnapshot();
        
        std::lock_guard<std::mutex> lock(dataMutex_);
        snapshots_.push_back(snapshot);
        
        // Keep only recent snapshots (last 1000)
        if (snapshots_.size() > 1000) {
            snapshots_.erase(snapshots_.begin(), snapshots_.begin() + 100);
        }
    }
}

void PerformanceMonitor::recordSpikeProcessingTime(double duration, size_t neuronCount) {
    startTimer("spike_processing");
    // Timer will be stopped by caller or at step end
}

void PerformanceMonitor::recordNeuronUpdateTime(double duration, size_t neuronCount) {
    startTimer("neuron_update");
}

void PerformanceMonitor::recordPlasticityUpdateTime(double duration, size_t synapseCount) {
    startTimer("plasticity_update");
}

void PerformanceMonitor::flush() {
    std::lock_guard<std::mutex> lock(dataMutex_);
    
    // Force a final snapshot
    if (enabled_) {
        snapshots_.push_back(createSnapshot());
    }
    
    // Clear benchmarks that are no longer active
    for (auto& pair : benchmarks_) {
        if (pair.second.active) {
            BenchmarkResult result;
            result.benchmarkName = pair.first;
            result.duration = 0.0;
            result.cpuTime = 0.0;
            benchmarkHistory_.push_back(result);
            pair.second.active = false;
        }
    }
}

void PerformanceMonitor::reset() {
    std::lock_guard<std::mutex> lock(dataMutex_);
    
    benchmarks_.clear();
    benchmarkHistory_.clear();
    timers_.clear();
    counters_.clear();
    metrics_.clear();
    
    totalMemoryAllocated_ = 0;
    totalMemoryDeallocated_ = 0;
    neuronPoolAllocated_ = 0;
    neuronPoolAvailable_ = 0;
    synapsePoolAllocated_ = 0;
    synapsePoolAvailable_ = 0;
    
    eventQueueSize_ = 0;
    eventQueueFillRate_ = 0.0;
    totalEventProcessingTime_ = 0.0;
    totalEventsProcessed_ = 0;
    
    simdVectorizedOps_ = 0;
    simdTotalOps_ = 0;
    simdUtilization_ = 0.0;
    
    activeThreads_ = 0;
    parallelEfficiency_ = 0.0;
    
    snapshots_.clear();
    timelineEvents_.clear();
}

// Utility functions

double PerformanceMonitor::getWallClockTime() const {
    auto now = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(now - globalStartTime_);
    return duration.count() * 1e-9;
}

double PerformanceMonitor::getCPUTime() const {
    // Simple implementation - in practice, would use platform-specific functions
    return getWallClockTime() * 0.5; // Approximate 50% CPU usage
}

size_t PerformanceMonitor::getCurrentMemoryUsage() const {
    return totalMemoryAllocated_.load() - totalMemoryDeallocated_.load();
}

std::string PerformanceMonitor::getSIMDLevel() const {
    return simd_.get() ? simd_->getSIMDType() : "Scalar";
}

PerformanceSnapshot PerformanceMonitor::createSnapshot() const {
    PerformanceSnapshot snapshot;
    snapshot.timestamp = getWallClockTime();
    snapshot.step = currentStep_;
    snapshot.totalSpikes = 0; // Will be updated by integration
    snapshot.activeNeurons = 0; // Will be updated by integration
    snapshot.memoryUsage = getCurrentMemoryUsage();
    snapshot.cpuUsage = getCPUTime();
    snapshot.eventQueueSize = eventQueueSize_.load();
    snapshot.eventQueueFill = eventQueueFillRate_.load();
    snapshot.memoryPoolAllocations = neuronPoolAllocated_.load();
    snapshot.memoryPoolDeallocations = neuronPoolAvailable_.load();
    snapshot.simdUtilization = simdUtilization_.load();
    snapshot.parallelThreads = activeThreads_.load();
    
    return snapshot;
}

std::vector<BottleneckAnalysis> PerformanceMonitor::analyzeMemoryBottlenecks() const {
    std::vector<BottleneckAnalysis> bottlenecks;
    
    size_t totalPoolUsage = neuronPoolAllocated_.load() + synapsePoolAllocated_.load();
    if (totalPoolUsage > neuronPoolAvailable_.load()) {
        BottleneckAnalysis bottleneck;
        bottleneck.bottleneckType = "memory_pool_ exhaustion";
        bottleneck.severity = std::min(1.0, static_cast<double>(totalPoolUsage) / (totalPoolUsage + 1000));
        bottleneck.affectedOperations = totalPoolUsage;
        bottleneck.recommendation = "Increase memory pool size or optimize object reuse";
        bottleneck.potentialGain = 5.0; // 5% potential gain
        bottleneck.autoFixable = true;
        bottlenecks.push_back(bottleneck);
    }
    
    if (getCurrentMemoryUsage() > 100 * 1024 * 1024) { // 100 MB
        BottleneckAnalysis bottleneck;
        bottleneck.bottleneckType = "high_memory_usage";
        bottleneck.severity = std::min(1.0, static_cast<double>(getCurrentMemoryUsage()) / (200 * 1024 * 1024));
        bottleneck.affectedOperations = getCurrentMemoryUsage() / 1024; // KB affected
        bottleneck.recommendation = "Consider memory pooling or reducing object overhead";
        bottleneck.potentialGain = 10.0; // 10% potential gain
        bottleneck.autoFixable = true;
        bottlenecks.push_back(bottleneck);
    }
    
    return bottlenecks;
}

std::vector<BottleneckAnalysis> PerformanceMonitor::analyzeCPUbottlenecks() const {
    std::vector<BottleneckAnalysis> bottlenecks;
    
    // Check for high event queue processing time
    if (totalEventsProcessed_.load() > 0) {
        double avgEventProcessingTime = totalEventProcessingTime_.load() / totalEventsProcessed_.load();
        if (avgEventProcessingTime > 0.001) { // 1 ms per event
            BottleneckAnalysis bottleneck;
            bottleneck.bottleneckType = "event_queue_processing";
            bottleneck.severity = std::min(1.0, avgEventProcessingTime / 0.01); // 10 ms threshold
            bottleneck.affectedOperations = totalEventsProcessed_.load();
            bottleneck.recommendation = "Optimize event queue processing or increase event batch size";
            bottleneck.potentialGain = 15.0; // 15% potential gain
            bottleneck.autoFixable = true;
            bottlenecks.push_back(bottleneck);
        }
    }
    
    // Check for low SIMD utilization
    if (simdUtilization_.load() < 0.5) {
        BottleneckAnalysis bottleneck;
        bottleneck.bottleneckType = "low_simd_utilization";
        bottleneck.severity = 1.0 - simdUtilization_.load();
        bottleneck.affectedOperations = simdTotalOps_.load();
        bottleneck.recommendation = "Tune SIMD usage or increase vectorized operations";
        bottleneck.potentialGain = 30.0; // 30% potential gain (SIMD can give huge gains)
        bottleneck.autoFixable = true;
        bottlenecks.push_back(bottleneck);
    }
    
    return bottlenecks;
}

std::vector<BottleneckAnalysis> PerformanceMonitor::analyzeEventQueueBottlenecks() const {
    std::vector<BottleneckAnalysis> bottlenecks;
    
    if (eventQueueFillRate_.load() > 0.95) {
        BottleneckAnalysis bottleneck;
        bottleneck.bottleneckType = "event_queue_full";
        bottleneck.severity = eventQueueFillRate_.load();
        bottleneck.affectedOperations = eventQueueSize_.load();
        bottleneck.recommendation = "Increase event queue capacity or optimize event generation";
        bottleneck.potentialGain = 8.0; // 8% potential gain
        bottleneck.autoFixable = true;
        bottlenecks.push_back(bottleneck);
    }
    
    return bottlenecks;
}

std::vector<BottleneckAnalysis> PerformanceMonitor::analyzeParallelBottlenecks() const {
    std::vector<BottleneckAnalysis> bottlenecks;
    
    if (parallelEfficiency_.load() < 0.5) {
        BottleneckAnalysis bottleneck;
        bottleneck.bottleneckType = "parallel_inefficiency";
        bottleneck.severity = 1.0 - parallelEfficiency_.load();
        bottleneck.affectedOperations = activeThreads_.load() * 100; // Estimate
        bottleneck.recommendation = "Check for synchronization bottlenecks or load imbalance";
        bottleneck.potentialGain = 20.0; // 20% potential gain
        bottleneck.autoFixable = true;
        bottlenecks.push_back(bottleneck);
    }
    
    return bottlenecks;
}

std::string PerformanceMonitor::formatBenchmarkResult(const BenchmarkResult& result) const {
    std::stringstream ss;
    ss << "  Benchmark: " << result.benchmarkName << std::endl;
    ss << "    Duration: " << std::fixed << std::setprecision(3) << result.duration << "s" << std::endl;
    ss << "    Operations/sec: " << std::fixed << std::setprecision(0) << result.opsPerSecond" << std::endl;
    ss << "    SIMD Level: " << result.simdLevel << std::endl;
    ss << "    Threads: " << result.numThreads << " (Efficiency: " << std::fixed << std::setprecision(1) << result.efficiency * 100.0 << "%)" << std::endl;
    
    return ss.str();
}

std::string PerformanceMonitor::formatBottleneckAnalysis(const BottleneckAnalysis& bottleneck) const {
    std::stringstream ss;
    ss << "  " << bottleneck.bottleneckType << std::endl;
    ss << "    Severity: " << std::fixed << std::setprecision(1) << bottleneck.severity * 100.0 << "%" << std::endl;
    ss << "    Affected: " << bottleneck.affectedOperations << " operations" << std::endl;
    ss << "    Recommendation: " << bottleneck.recommendation << std::endl;
    ss << "    Potential Gain: " << std::fixed << std::setprecision(1) << bottleneck.potentialGain << "%" << std::endl;
    
    return ss.str();
}

std::string PerformanceMonitor::formatPerformanceSnapshot(const PerformanceSnapshot& snapshot) const {
    std::stringstream ss;
    ss << "Step " << snapshot.step << " (t=" << std::fixed << std::setprecision(3) << snapshot.timestamp << "s): "
       << snapshot.activeNeurons << " neurons, "
       << snapshot.totalSpikes << " spikes, "
       << snapshot.memoryUsage / 1024 << " KB memory, "
       << snapshot.eventQueueFill * 100.0 << "% queue fill, "
       << snapshot.simdUtilization * 100.0 << "% SIMD utilization";
    
    return ss.str();
}

} // namespace nlm
