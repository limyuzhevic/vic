#include "PerformanceProfiler.hpp"

namespace nlm {

OperationProfiler::OperationProfiler(size_t maxHistoryEntries)
    : active_(true), maxHistoryEntries_(maxHistoryEntries) {
}

void OperationProfiler::startProfiling(OperationType operationType, const std::string& operationName) {
    if (!active_) return;
    
    std::lock_guard<std::mutex> lock(mutex_);
    recentOperations_.emplace_back(operationType, 0);  // Will be updated when stopped
}

void OperationProfiler::stopProfiling() {
    if (!active_) return;
    
    std::lock_guard<std::mutex> lock(mutex_);
    if (!recentOperations_.empty()) {
        auto& lastOp = recentOperations_.back();
        PerformanceTimer timer;
        uint64_t elapsedMicroseconds = timer.elapsedMicroseconds();
        lastOp.second = elapsedMicroseconds;
        
        // Record the sample
        recordSample(lastOp.first, "", elapsedMicroseconds);
    }
}

void OperationProfiler::recordSample(OperationType operationType, const std::string& operationName, uint64_t timeMicroseconds) {
    if (!active_) return;
    
    std::lock_guard<std::mutex> lock(mutex_);
    
    std::string name = operationName.empty() ? getOperationTypeName(operationType) : operationName;
    
    if (dataMap_.find(operationType) == dataMap_.end()) {
        profilingData_.push_back(std::make_unique<ProfilingData>(operationType, name));
        dataMap_[operationType] = profilingData_.back().get();
    }
    
    ProfilingData* data = dataMap_[operationType];
    data->callCount++;
    data->totalTimeMicroseconds += timeMicroseconds;
    data->minTimeMicroseconds = std::min(data->minTimeMicroseconds, timeMicroseconds);
    data->maxTimeMicroseconds = std::max(data->maxTimeMicroseconds, timeMicroseconds);
    data->avgTimeMicroseconds = static_cast<double>(data->totalTimeMicroseconds) / data->callCount;
    
    // Maintain history size
    recentOperations_.push_back({operationType, timeMicroseconds});
    if (recentOperations_.size() > maxHistoryEntries_) {
        recentOperations_.pop_front();
    }
}

const OperationProfiler::ProfilingData* OperationProfiler::getProfilingData(OperationType operationType) const {
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = dataMap_.find(operationType);
    if (it != dataMap_.end()) {
        return it->second;
    }
    return nullptr;
}

std::vector<const OperationProfiler::ProfilingData*> OperationProfiler::getAllProfilingData() const {
    std::lock_guard<std::mutex> lock(mutex_);
    std::vector<const ProfilingData*> result;
    result.reserve(dataMap_.size());
    for (const auto& pair : dataMap_) {
        result.push_back(pair.second);
    }
    return result;
}

std::vector<const OperationProfiler::ProfilingData*> OperationProfiler::getHotOperations(size_t count) const {
    std::lock_guard<std::mutex> lock(mutex_);
    std::vector<const ProfilingData*> result;
    result.reserve(dataMap_.size());
    
    for (const auto& pair : dataMap_) {
        result.push_back(pair.second);
    }
    
    // Sort by average time (descending)
    std::sort(result.begin(), result.end(),
              [](const ProfilingData* a, const ProfilingData* b) {
                  return a->avgTimeMicroseconds > b->avgTimeMicroseconds;
              });
    
    // Return top N
    size_t limit = std::min(count, result.size());
    return std::vector<const ProfilingData*>(result.begin(), result.begin() + limit);
}

std::vector<const OperationProfiler::ProfilingData*> OperationProfiler::getFrequentOperations(size_t count) const {
    std::lock_guard<std::mutex> lock(mutex_);
    std::vector<const ProfilingData*> result;
    result.reserve(dataMap_.size());
    
    for (const auto& pair : dataMap_) {
        result.push_back(pair.second);
    }
    
    // Sort by call count (descending)
    std::sort(result.begin(), result.end(),
              [](const ProfilingData* a, const ProfilingData* b) {
                  return a->callCount > b->callCount;
              });
    
    // Return top N
    size_t limit = std::min(count, result.size());
    return std::vector<const ProfilingData*>(result.begin(), result.begin() + limit);
}

void OperationProfiler::clearProfilingData() {
    std::lock_guard<std::mutex> lock(mutex_);
    profilingData_.clear();
    dataMap_.clear();
    recentOperations_.clear();
}

std::string OperationProfiler::generateReport() const {
    std::lock_guard<std::mutex> lock(mutex_);
    std::stringstream ss;
    ss << "=== Operation Profiling Report ===\n";
    ss << "Total operations: " << dataMap_.size() << "\n";
    ss << "Recent operations tracked: " << recentOperations_.size() << "\n\n";
    
    ss << "=== Operation Breakdown ===\n";
    for (const auto& pair : dataMap_) {
        const ProfilingData* data = pair.second;
        ss << getOperationTypeName(data->operationType) << ": "
           << data->callCount << " calls, "
           << data->avgTimeMicroseconds << " avg μs, "
           << (data->avgTimeMicroseconds * data->callCount / 1000000.0) << " total μs\n";
    }
    
    ss << "\n=== Hot Operations ===\n";
    auto hotOps = getHotOperations(5);
    for (size_t i = 0; i < hotOps.size(); ++i) {
        const ProfilingData* data = hotOps[i];
        ss << i + 1 << ". " << getOperationTypeName(data->operationType)
           << ": " << data->avgTimeMicroseconds << " avg μs\n";
    }
    
    ss << "\n=== Frequent Operations ===\n";
    auto freqOps = getFrequentOperations(5);
    for (size_t i = 0; i < freqOps.size(); ++i) {
        const ProfilingData* data = freqOps[i];
        ss << i + 1 << ". " << getOperationTypeName(data->operationType)
           << ": " << data->callCount << " calls\n";
    }
    
    return ss.str();
}

std::string OperationProfiler::getOperationTypeName(OperationType type) {
    switch (type) {
        case OperationType::NEURON_UPDATE: return "Neuron Update";
        case OperationType::SYNAPSE_UPDATE: return "Synapse Update";
        case OperationType::PLASTICITY_APPLY: return "Plasticity Application";
        case OperationType::SPIKE_PROCESSING: return "Spike Processing";
        case OperationType::MEMORY_ACCESS: return "Memory Access";
        case OperationType::PREDICTION_COMPUTATION: return "Prediction Computation";
        case OperationType::COGNITIVE_PROCESSING: return "Cognitive Processing";
        case OperationType::NEUROMODULATION: return "Neuromodulation";
        case OperationType::SENSOR_INPUT: return "Sensor Input";
        case OperationType::MOTOR_OUTPUT: return "Motor Output";
        case OperationType::MEMORY_ALLOCATION: return "Memory Allocation";
        case OperationType::MEMORY_DEALLOCATION: return "Memory Deallocation";
        case OperationType::UNKNOWN: return "Unknown";
        default: return "Unknown";
    }
}

MemoryTracker::MemoryTracker(size_t maxTrackedAllocations)
    : allocationTrackingEnabled_(true), maxTrackedAllocations_(maxTrackedAllocations),
      stackDepth_(10), peakMemoryUsage_(0) {
}

void MemoryTracker::trackAllocation(AllocationType type, void* pointer, size_t size,
                                   const std::string& allocationSite) {
    if (!allocationTrackingEnabled_) return;
    
    std::lock_guard<std::mutex> lock(mutex_);
    
    // Get current call stack
    std::vector<std::string> callStack;
    // In a real implementation, we would capture the actual call stack here
    // For now, we'll simulate it
    callStack.push_back(allocationSite);
    
    uint64_t currentTime = std::chrono::duration_cast<std::chrono::microseconds>(
        std::chrono::high_resolution_clock::now().time_since_epoch()).count();
    
    auto info = std::make_unique<AllocationInfo>(type, pointer, size, allocationSite, currentTime, callStack);
    
    // Update statistics
    size_t currentMemory = getTotalMemoryUsage();
    peakMemoryUsage_ = std::max(peakMemoryUsage_.load(), currentMemory + size);
    
    // Store allocation
    allocationMap_[pointer] = std::move(info);
    AllocationInfo* infoPtr = allocationMap_[pointer].get();
    
    // Update indexes
    allocationsBySite_[allocationSite].push_back(infoPtr);
    
    // Limit allocations by tracked count
    if (allocationMap_.size() > maxTrackedAllocations_) {
        // Remove oldest allocation
        auto oldestIt = allocationMap_.begin();
        allocationsBySite_[oldestIt->second->allocationSite].erase(
            std::remove(allocationsBySite_[oldestIt->second->allocationSite].begin(),
                       allocationsBySite_[oldestIt->second->allocationSite].end(),
                       oldestIt->second.get()),
            allocationsBySite_[oldestIt->second->allocationSite].end());
        allocationMap_.erase(oldestIt);
    }
}

void MemoryTracker::trackDeallocation(AllocationType type, void* pointer) {
    if (!allocationTrackingEnabled_) return;
    
    std::lock_guard<std::mutex> lock(mutex_);
    
    auto it = allocationMap_.find(pointer);
    if (it != allocationMap_.end()) {
        AllocationInfo* info = it->second.get();
        info->isFreed = true;
        
        // Update indexes
        allocationsBySite_[info->allocationSite].erase(
            std::remove(allocationsBySite_[info->allocationSite].begin(),
                       allocationsBySite_[info->allocationSite].end(),
                       info),
            allocationsBySite_[info->allocationSite].end());
    }
}

const MemoryTracker::AllocationInfo* MemoryTracker::getAllocationInfo(void* pointer) const {
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = allocationMap_.find(pointer);
    if (it != allocationMap_.end()) {
        return it->second.get();
    }
    return nullptr;
}

std::vector<const MemoryTracker::AllocationInfo*> MemoryTracker::getCurrentAllocations() const {
    std::lock_guard<std::mutex> lock(mutex_);
    std::vector<const AllocationInfo*> result;
    for (const auto& pair : allocationMap_) {
        if (!pair.second->isFreed) {
            result.push_back(pair.second.get());
        }
    }
    return result;
}

std::vector<const MemoryTracker::AllocationInfo*> MemoryTracker::getAllocationsByThread(size_t threadId) const {
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = allocationsByThread_.find(threadId);
    if (it != allocationsByThread_.end()) {
        return it->second;
    }
    return {};
}

std::vector<const MemoryTracker::AllocationInfo*> MemoryTracker::getAllocationsBySite(const std::string& site) const {
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = allocationsBySite_.find(site);
    if (it != allocationsBySite_.end()) {
        return it->second;
    }
    return {};
}

std::string MemoryTracker::generateMemoryReport() const {
    std::lock_guard<std::mutex> lock(mutex_);
    std::stringstream ss;
    ss << "=== Memory Tracking Report ===\n";
    ss << "Total allocations tracked: " << allocationMap_.size() << "\n";
    ss << "Current allocations: " << getCurrentAllocations().size() << "\n";
    ss << "Memory leaks: " << getMemoryLeaks().size() << "\n";
    ss << "Total memory usage: " << getTotalMemoryUsage() << " bytes\n";
    ss << "Peak memory usage: " << peakMemoryUsage_.load() << " bytes\n\n";
    
    ss << "=== Allocation by Type ===\n";
    std::unordered_map<AllocationType, size_t> typeCounts;
    for (const auto& pair : allocationMap_) {
        typeCounts[pair.second->allocType]++;
    }
    
    for (const auto& pair : typeCounts) {
        ss << getAllocationTypeName(pair.first) << ": " << pair.second << "\n";
    }
    
    ss << "\n=== Top Allocation Sites ===\n";
    std::vector<std::pair<std::string, size_t>> siteCounts;
    for (const auto& pair : allocationsBySite_) {
        siteCounts.emplace_back(pair.first, pair.second.size());
    }
    
    std::sort(siteCounts.begin(), siteCounts.end(),
              [](const auto& a, const auto& b) { return a.second > b.second; });
    
    size_t limit = std::min(size_t(10), siteCounts.size());
    for (size_t i = 0; i < limit; ++i) {
        ss << i + 1 << ". " << siteCounts[i].first << ": " << siteCounts[i].second << " allocations\n";
    }
    
    return ss.str();
}

MemoryTracker::MemoryStats MemoryTracker::getMemoryStats() const {
    std::lock_guard<std::mutex> lock(mutex_);
    MemoryStats stats;
    stats.totalAllocations = allocationMap_.size();
    stats.totalDeallocations = getDeallocationCount();
    stats.currentAllocations = getCurrentAllocations().size();
    stats.totalMemoryBytes = getTotalMemoryUsage();
    stats.peakMemoryBytes = peakMemoryUsage_.load();
    
    if (stats.totalAllocations > 0) {
        stats.fragmentationRatio = static_cast<double>(stats.currentAllocations) / stats.totalAllocations;
    } else {
        stats.fragmentationRatio = 0.0;
    }
    
    return stats;
}

std::string MemoryTracker::getAllocationTypeName(AllocationType type) {
    switch (type) {
        case AllocationType::NEW: return "new";
        case AllocationType::NEW_ARRAY: return "new[]";
        case AllocationType::MALLOC: return "malloc";
        case AllocationType::MEMORY_POOL: return "memory_pool";
        case AllocationType::ALLOCATE_UNIQUE: return "allocate_unique";
        case AllocationType::ALLOCATE_SHARED: return "allocate_shared";
        case AllocationType::UNKNOWN: return "unknown";
        default: return "unknown";
    }
}

PerformanceMetricsCollector::PerformanceMetricsCollector(size_t metricsHistorySize)
    : metricsCollectionEnabled_(true), maxHistorySize_(metricsHistorySize) {
}

void PerformanceMetricsCollector::recordMetrics(const BrainMetrics& metrics) {
    if (!metricsCollectionEnabled_) return;
    
    std::lock_guard<std::mutex> lock(mutex_);
    metrics_.push_back(metrics);
    
    if (metrics_.size() > maxHistorySize_) {
        metrics_.pop_front();
    }
}

std::vector<PerformanceMetricsCollector::BrainMetrics> PerformanceMetricsCollector::getRecentMetrics(size_t count) const {
    std::lock_guard<std::mutex> lock(mutex_);
    if (count > metrics_.size()) {
        count = metrics_.size();
    }
    
    auto startIt = metrics_.rbegin();
    auto endIt = metrics_.rbegin() + count;
    std::vector<BrainMetrics> result(startIt, endIt);
    std::reverse(result.begin(), result.end());
    return result;
}

std::vector<PerformanceMetricsCollector::BrainMetrics> PerformanceMetricsCollector::getMetricsInRange(
    uint64_t startStep, uint64_t endStep) const {
    std::lock_guard<std::mutex> lock(mutex_);
    std::vector<BrainMetrics> result;
    for (const auto& metrics : metrics_) {
        if (metrics.simulationStep >= startStep && metrics.simulationStep <= endStep) {
            result.push_back(metrics);
        }
    }
    return result;
}

PerformanceMetricsCollector::PerformanceSummary PerformanceMetricsCollector::getPerformanceSummary() const {
    std::lock_guard<std::mutex> lock(mutex_);
    PerformanceSummary summary;
    
    if (metrics_.empty()) return summary;
    
    double totalStepTime = 0.0;
    double minStepTime = std::numeric_limits<double>::max();
    double maxStepTime = 0.0;
    double totalNeuronsPerSecond = 0.0;
    double totalSpikesPerSecond = 0.0;
    size_t peakNeuronCount = 0;
    
    for (const auto& metrics : metrics_) {
        totalStepTime += metrics.stepTime;
        minStepTime = std::min(minStepTime, metrics.stepTime);
        maxStepTime = std::max(maxStepTime, metrics.stepTime);
        totalNeuronsPerSecond += metrics.neuronsPerSecond;
        totalSpikesPerSecond += metrics.spikesPerSecond;
        peakNeuronCount = std::max(peakNeuronCount, metrics.neuronCount);
    }
    
    size_t count = metrics_.size();
    if (count > 0) {
        summary.averageStepTime = totalStepTime / count;
        summary.minStepTime = minStepTime;
        summary.maxStepTime = maxStepTime;
        summary.averageNeuronsPerSecond = totalNeuronsPerSecond / count;
        summary.averageSpikesPerSecond = totalSpikesPerSecond / count;
    }
    
    return summary;
}

PerformanceMetricsCollector::BottleneckAnalysis PerformanceMetricsCollector::analyzeBottlenecks() const {
    PerformanceMetricsCollector::BottleneckAnalysis analysis;
    analysis.bottleneckType = "unknown";
    analysis.severity = 0.0;
    analysis.description = "No performance data available";
    
    if (metrics_.empty()) return analysis;
    
    // Analyze step times
    double avgStepTime = getPerformanceSummary().averageStepTime;
    if (avgStepTime > 0.1) {  // High step time
        analysis.bottleneckType = "cpu";
        analysis.severity = std::min(1.0, avgStepTime / 1.0);
        analysis.description = "High simulation step times indicate CPU bottlenecks";
        analysis.recommendations = {
            "Increase multithreading",
            "Optimize neural computations",
            "Consider hardware acceleration"
        };
    }
    
    // Analyze memory usage
    size_t peakMemoryUsage = getPerformanceSummary().peakMemoryUsage;
    if (peakMemoryUsage > 1024 * 1024 * 1024) {  // 1GB
        analysis.bottleneckType = "memory";
        analysis.severity = std::min(1.0, peakMemoryUsage / (2 * 1024 * 1024 * 1024));
        analysis.description = "High memory usage indicates memory bottlenecks";
        analysis.recommendations = {
            "Use memory pools",
            "Implement sparse connectivity",
            "Optimize data structures"
        };
    }
    
    // Analyze thread utilization
    size_t threadCount = 0;
    for (const auto& metrics : metrics_) {
        threadCount = std::max(threadCount, metrics.threadCount);
    }
    
    if (threadCount > 0 && metrics_.back().activeThreads < threadCount * 0.5) {
        analysis.bottleneckType = "thread";
        analysis.severity = 0.5;
        analysis.description = "Poor thread utilization indicates synchronization bottlenecks";
        analysis.recommendations = {
            "Reduce lock contention",
            "Improve load balancing",
            "Use lock-free data structures"
        };
    }
    
    return analysis;
}

std::string PerformanceMetricsCollector::generateMetricsReport() const {
    std::lock_guard<std::mutex> lock(mutex_);
    std::stringstream ss;
    ss << "=== Performance Metrics Report ===\n";
    ss << "Total metrics collected: " << metrics_.size() << "\n";
    ss << "Simulation steps: " << (metrics_.empty() ? 0 : metrics_.back().simulationStep) << "\n\n";
    
    PerformanceSummary summary = getPerformanceSummary();
    ss << "=== Performance Summary ===\n";
    ss << "Average step time: " << summary.averageStepTime * 1000 << " ms\n";
    ss << "Min step time: " << summary.minStepTime * 1000 << " ms\n";
    ss << "Max step time: " << summary.maxStepTime * 1000 << " ms\n";
    ss << "Average neurons/sec: " << summary.averageNeuronsPerSecond << "\n";
    ss << "Average spikes/sec: " << summary.averageSpikesPerSecond << "\n";
    ss << "Peak neuron count: " << summary.peakNeuronCount << "\n";
    ss << "Peak memory usage: " << summary.peakMemoryUsage << " bytes\n\n";
    
    // Recent performance trend
    if (metrics_.size() >= 10) {
        ss << "=== Recent Performance Trend ===\n";
        auto recentMetrics = getRecentMetrics(10);
        for (size_t i = 0; i < recentMetrics.size(); ++i) {
            ss << "Step " << recentMetrics[i].simulationStep
               << ": " << recentMetrics[i].stepTime * 1000 << " ms, "
               << recentMetrics[i].neuronsPerSecond << " neurons/sec\n";
        }
    }
    
    return ss.str();
}

BrainPerformanceProfiler::BrainPerformanceProfiler(std::shared_ptr<Config> config)
    : brain_(nullptr), profilingActive_(false), operationProfilingEnabled_(true),
      memoryTrackingEnabled_(true), metricsCollectionEnabled_(true) {
}

void BrainPerformanceProfiler::initialize(Brain* brain) {
    brain_ = brain;
}

void BrainPerformanceProfiler::startProfiling() {
    if (brain_ == nullptr) return;
    
    std::lock_guard<std::mutex> lock(mutex_);
    profilingActive_ = true;
    stepTimer_.reset();
    
    // Record initial state
    recordPerformanceSnapshot();
}

void BrainPerformanceProfiler::stopProfiling() {
    std::lock_guard<std::mutex> lock(mutex_);
    profilingActive_ = false;
    
    // Record final state
    recordPerformanceSnapshot();
}

void BrainPerformanceProfiler::recordPerformanceSnapshot() {
    if (brain_ == nullptr || !profilingActive_) return;
    
    BrainMetrics metrics;
    metrics.simulationStep = brain_->getCurrentStep();
    metrics.simulationTime = stepTimer_.elapsedMilliseconds() / 1000.0;
    
    // Get system state
    metrics.neuronCount = brain_->getTotalNeuronCount();
    metrics.activeNeurons = brain_->getActiveNeuronCount();
    metrics.firingNeurons = brain_->getFiringNeuronCount();
    metrics.pendingSpikes = brain_->getPendingSpikeCount();
    metrics.totalSpikes = brain_->getTotalSpikeCount();
    
    // Calculate derived metrics
    if (metrics.simulationTime > 0) {
        metrics.averageFiringRate = static_cast<double>(metrics.firingNeurons) / metrics.simulationTime;
        metrics.neuronsPerSecond = static_cast<double>(metrics.neuronCount) / metrics.simulationTime;
        metrics.spikesPerSecond = static_cast<double>(metrics.totalSpikes) / metrics.simulationTime;
    }
    
    // Get memory usage
    metrics.memoryUsageBytes = getCurrentMemoryUsage();
    
    // Record metrics
    metricsCollector_.recordMetrics(metrics);
}

BrainPerformanceProfiler::PerformanceStatistics BrainPerformanceProfiler::getPerformanceStatistics() const {
    PerformanceStatistics stats;
    
    if (brain_ == nullptr) return stats;
    
    stats.totalSteps = brain_->getCurrentStep();
    stats.totalSimulationTime = metricsCollector_.getPerformanceSummary().averageStepTime * stats.totalSteps;
    stats.averageStepTime = stats.totalSimulationTime / stats.totalSteps;
    
    // Get peak metrics from recent data
    auto recentMetrics = metricsCollector_.getRecentMetrics(100);
    for (const auto& metrics : recentMetrics) {
        stats.peakNeuronCount = std::max(stats.peakNeuronCount, metrics.neuronCount);
        stats.peakFiringNeurons = std::max(stats.peakFiringNeurons, metrics.firingNeurons);
        stats.peakMemoryUsage = std::max(stats.peakMemoryUsage, metrics.memoryUsageBytes);
        stats.totalCPUUsage += metrics.cpuUsagePercentage;
        stats.totalMemoryBandwidthUsage += metrics.memoryBandwidthUsage;
    }
    
    if (!recentMetrics.empty()) {
        stats.totalCPUUsage /= recentMetrics.size();
        stats.totalMemoryBandwidthUsage /= recentMetrics.size();
    }
    
    return stats;
}

std::string BrainPerformanceProfiler::generatePerformanceReport() const {
    std::stringstream ss;
    ss << "=== Brain Performance Report ===\n";
    ss << "Profiling active: " << (profilingActive_ ? "yes" : "no") << "\n";
    ss << "Operation profiling: " << (operationProfilingEnabled_ ? "yes" : "no") << "\n";
    ss << "Memory tracking: " << (memoryTrackingEnabled_ ? "yes" : "no") << "\n";
    ss << "Metrics collection: " << (metricsCollectionEnabled_ ? "yes" : "no") << "\n\n";
    
    // Operation profiling report
    if (operationProfilingEnabled_) {
        ss << "=== Operation Profiling ===\n";
        ss << operationProfiler_.generateReport() << "\n";
    }
    
    // Memory tracking report
    if (memoryTrackingEnabled_) {
        ss << "=== Memory Tracking ===\n";
        ss << memoryTracker_.generateMemoryReport() << "\n";
    }
    
    // Metrics collection report
    if (metricsCollectionEnabled_) {
        ss << "=== Performance Metrics ===\n";
        ss << metricsCollector_.generateMetricsReport() << "\n";
    }
    
    return ss.str();
}

} // namespace nlm

#endif // NLM_PERFORMANCE_PROFILING_HPP