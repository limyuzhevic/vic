#include "PerformanceProfiler.hpp"

namespace nlm {

// Static member definitions
PerformanceProfiler PerformanceProfiler::instance_;
thread_local PerformanceData* PerformanceProfiler::currentProfile_ = nullptr;
PerformanceProfiler* PerformanceProfiler::active_profiler_ = nullptr;

PerformanceProfiler::PerformanceProfiler() : enabled_(false) {
    config_.enableCPUProfiling = true;
    config_.enableMemoryTracking = true;
    config_.enableThreadProfiling = true;
    config_.enableIOPerformance = false;
    config_.enableGPUProfiling = false;
    config_.enableBottleneckDetection = true;
    config_.enableRealTimeMonitoring = false;
    config_.profileDepth = 5;
    config_.samplingIntervalMs = 100.0;
    config_.maxSamples = 10000;
}

PerformanceProfiler::~PerformanceProfiler() {
    // Cleanup any resources
}

void PerformanceProfiler::initialize(const ProfilingConfig& config) {
    std::lock_guard<std::mutex> lock(dataMutex_);
    config_ = config;
    enabled_ = true;
    performanceData_.clear();
    threadPerformance_.clear();
    ioPerformance_.clear();
}

void PerformanceProfiler::startProfiling(const std::string& name, const std::string& category) {
    if (!enabled_) return;
    
    std::lock_guard<std::mutex> lock(dataMutex_);
    PerformanceData data(name, category);
    data.startTime = std::chrono::high_resolution_clock::now();
    performanceData_[name] = data;
    currentProfile_ = &performanceData_[name];
}

void PerformanceProfiler::stopProfiling(const std::string& name) {
    if (!enabled_) return;
    
    std::lock_guard<std::mutex> lock(dataMutex_);
    auto it = performanceData_.find(name);
    if (it != performanceData_.end()) {
        it->second.endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration<double, std::milli>(
            it->second.endTime - it->second.startTime).count();
        it->second.wallTimeMs = duration;
        
        // Calculate average if we have multiple calls
        if (it->second.callCount > 1) {
            it->second.avgTimeMs = it->second.cpuTimeMs / it->second.callCount;
        }
    }
}

void PerformanceProfiler::recordMeasurement(const std::string& name, double timeMs, size_t memoryBytes) {
    if (!enabled_) return;
    
    std::lock_guard<std::mutex> lock(dataMutex_);
    auto it = performanceData_.find(name);
    if (it != performanceData_.end()) {
        it->second.callCount++;
        it->second.cpuTimeMs += timeMs;
        
        if (timeMs < it->second.minTimeMs) {
            it->second.minTimeMs = timeMs;
        }
        if (timeMs > it->second.maxTimeMs) {
            it->second.maxTimeMs = timeMs;
        }
        
        it->second.memoryBytes = std::max(it->second.memoryBytes, memoryBytes);
        it->second.peakMemoryBytes = std::max(it->second.peakMemoryBytes, memoryBytes);
    }
}

void PerformanceProfiler::recordThreadPerformance(const ThreadPerformance& threadPerf) {
    if (!enabled_) return;
    
    std::lock_guard<std::mutex> lock(dataMutex_);
    threadPerformance_.push_back(threadPerf);
}

void PerformanceProfiler::recordIOPerformance(const IOPerformance& ioPerf) {
    if (!enabled_) return;
    
    std::lock_guard<std::mutex> lock(dataMutex_);
    ioPerformance_.push_back(ioPerf);
}

PerformanceData PerformanceProfiler::getPerformanceData(const std::string& name) const {
    std::lock_guard<std::mutex> lock(dataMutex_);
    auto it = performanceData_.find(name);
    if (it != performanceData_.end()) {
        return it->second;
    }
    return PerformanceData(name, "");
}

std::vector<PerformanceData> PerformanceProfiler::getPerformanceDataByCategory(const std::string& category) const {
    std::lock_guard<std::mutex> lock(dataMutex_);
    std::vector<PerformanceData> result;
    for (const auto& pair : performanceData_) {
        if (pair.second.category == category) {
            result.push_back(pair.second);
        }
    }
    return result;
}

std::vector<BottleneckAnalysis> PerformanceProfiler::analyzeBottlenecks() const {
    std::vector<BottleneckAnalysis> bottlenecks;
    
    std::lock_guard<std::mutex> lock(dataMutex_);
    
    // Analyze based on performance data
    for (const auto& pair : performanceData_) {
        const auto& data = pair.second;
        
        if (data.cpuTimeMs > 100.0) { // High CPU time
            BottleneckAnalysis bottleneck;
            bottleneck.category = data.category;
            bottleneck.name = "High CPU Usage";  // The actual component name is stored in PerformanceData.name
            bottleneck.impactScore = data.cpuTimeMs / 1000.0; // Normalize
            bottleneck.severity = std::min(data.cpuTimeMs / 100.0, 10.0); // Cap at 10
            bottleneck.description = "Component " + data.name + " has high CPU time: " + 
                                    std::to_string(data.cpuTimeMs) + "ms";
            bottleneck.recommendation = "Consider optimizing the algorithm or using parallelization";
            bottleneck.affectedComponents = 1;
            bottleneck.timeLostMs = data.cpuTimeMs - data.avgTimeMs;
            bottlenecks.push_back(bottleneck);
        }
        
        if (data.memoryBytes > 100 * 1024 * 1024) { // > 100 MB
            BottleneckAnalysis bottleneck;
            bottleneck.category = data.category;
            bottleneck.name = "High Memory Usage";
            bottleneck.impactScore = static_cast<double>(data.memoryBytes) / (1024 * 1024 * 1024); // GB
            bottleneck.severity = std::min(data.memoryBytes / (10 * 1024 * 1024), 10.0); // MB to severity
            bottleneck.description = "Component " + data.name + " has high memory usage: " + 
                                    std::to_string(data.memoryBytes) + " bytes";
            bottleneck.recommendation = "Consider reducing memory footprint or using streaming";
            bottleneck.affectedComponents = 1;
            bottleneck.timeLostMs = data.cpuTimeMs;
            bottlenecks.push_back(bottleneck);
        }
        
        if (data.callCount == 0) { // No calls recorded
            BottleneckAnalysis bottleneck;
            bottleneck.category = data.category;
            bottleneck.name = "No Activity";
            bottleneck.impactScore = 1.0;
            bottleneck.severity = 5.0;
            bottleneck.description = "Component " + data.name + " has not been called";
            bottleneck.recommendation = "Check if the component is being used";
            bottleneck.affectedComponents = 1;
            bottleneck.timeLostMs = 0.0;
            bottlenecks.push_back(bottleneck);
        }
    }
    
    return bottlenecks;
}

std::vector<OptimizationRecommendation> PerformanceProfiler::getOptimizationRecommendations() const {
    std::vector<OptimizationRecommendation> recommendations;
    
    std::lock_guard<std::mutex> lock(dataMutex_);
    
    for (const auto& pair : performanceData_) {
        const auto& data = pair.second;
        
        // High execution time
        if (data.avgTimeMs > 50.0) {
            OptimizationRecommendation rec;
            rec.category = data.category;
            rec.issue = "High execution time";
            rec.potentialGain = (data.maxTimeMs - data.minTimeMs) * 0.1; // 10% potential gain
            rec.estimatedEffort = 3.0; // Medium effort
            rec.description = "Component " + data.name + " has high average execution time of " + 
                             std::to_string(data.avgTimeMs) + "ms";
            rec.implementationHint = "Consider using more efficient algorithms or caching";
            rec.expectedPerformanceGain = "Reduce average execution time by 20-50%";
            recommendations.push_back(rec);
        }
        
        // High memory usage
        if (data.peakMemoryBytes > 50 * 1024 * 1024) {
            OptimizationRecommendation rec;
            rec.category = data.category;
            rec.issue = "High memory usage";
            rec.potentialGain = static_cast<double>(data.peakMemoryBytes) / (1024 * 1024 * 1024); // GB
            rec.estimatedEffort = 4.0; // High effort
            rec.description = "Component " + data.name + " has peak memory usage of " + 
                             std::to_string(data.peakMemoryBytes / (1024 * 1024)) + " MB";
            rec.implementationHint = "Consider using object pooling, value semantics, or streaming";
            rec.expectedPerformanceGain = "Reduce memory usage by 30-70%";
            recommendations.push_back(rec);
        }
        
        // Low call count (potential dead code)
        if (data.callCount < 10 && !data.name.empty()) {
            OptimizationRecommendation rec;
            rec.category = data.category;
            rec.issue = "Low call count";
            rec.potentialGain = 0.0; // No direct gain
            rec.estimatedEffort = 1.0; // Low effort
            rec.description = "Component " + data.name + " has low call count: " + 
                             std::to_string(data.callCount);
            rec.implementationHint = "Consider if this component is still needed";
            rec.expectedPerformanceGain = "Remove unused code";
            recommendations.push_back(rec);
        }
        
        // High variability
        if (data.callCount > 1 && data.stdDevTimeMs > data.avgTimeMs * 2.0) {
            OptimizationRecommendation rec;
            rec.category = data.category;
            rec.issue = "High execution time variability";
            rec.potentialGain = data.stdDevTimeMs * 0.1; // 10% potential reduction
            rec.estimatedEffort = 2.0; // Low-medium effort
            rec.description = "Component " + data.name + " has high variability in execution time (std dev: " + 
                             std::to_string(data.stdDevTimeMs) + "ms)";
            rec.implementationHint = "Ensure consistent input conditions and check for side effects";
            rec.expectedPerformanceGain = "Reduce execution time variability";
            recommendations.push_back(rec);
        }
    }
    
    return recommendations;
}

std::string PerformanceProfiler::generateReport() const {
    std::ostringstream report;
    
    report << "=== Performance Profiler Report ===\n";
    report << "Generated: " << std::chrono::system_clock::now() << "\n\n";
    
    std::lock_guard<std::mutex> lock(dataMutex_);
    
    report << "Performance Data:\n";
    for (const auto& pair : performanceData_) {
        const auto& data = pair.second;
        report << "  " << data.category << ": " << data.name << "\n";
        report << "    CPU Time: " << data.cpuTimeMs << " ms\n";
        report << "    Wall Time: " << data.wallTimeMs << " ms\n";
        report << "    Memory: " << data.memoryBytes << " bytes\n";
        report << "    Peak Memory: " << data.peakMemoryBytes << " bytes\n";
        report << "    Call Count: " << data.callCount << "\n";
        report << "    Avg Time: " << data.avgTimeMs << " ms\n";
        report << "    Min Time: " << data.minTimeMs << " ms\n";
        report << "    Max Time: " << data.maxTimeMs << " ms\n\n";
    }
    
    // Generate bottlenecks
    auto bottlenecks = analyzeBottlenecks();
    if (!bottlenecks.empty()) {
        report << "Bottleneck Analysis:\n";
        for (const auto& bottleneck : bottlenecks) {
            report << "  " << bottleneck.name << " (Severity: " << bottleneck.severity << ")\n";
            report << "    Impact: " << bottleneck.impactScore << "\n";
            report << "    Description: " << bottleneck.description << "\n";
            report << "    Recommendation: " << bottleneck.recommendation << "\n\n";
        }
    }
    
    // Generate recommendations
    auto recommendations = getOptimizationRecommendations();
    if (!recommendations.empty()) {
        report << "Optimization Recommendations:\n";
        for (const auto& rec : recommendations) {
            report << "  " << rec.issue << "\n";
            report << "    Potential Gain: " << rec.potentialGain << "\n";
            report << "    Estimated Effort: " << rec.estimatedEffort << "\n";
            report << "    Description: " << rec.description << "\n";
            report << "    Implementation Hint: " << rec.implementationHint << "\n";
            report << "    Expected Performance Gain: " << rec.expectedPerformanceGain << "\n\n";
        }
    }
    
    // Generate thread performance summary
    if (!threadPerformance_.empty()) {
        report << "Thread Performance Summary:\n";
        for (const auto& thread : threadPerformance_) {
            report << "  Thread " << thread.threadId << ":\n";
            report << "    CPU Time: " << thread.cpuTimeMs << " ms\n";
            report << "    CPU Utilization: " << thread.cpuUtilizationPercent << "%\n\n";
        }
    }
    
    // Generate I/O performance summary
    if (!ioPerformance_.empty()) {
        report << "I/O Performance Summary:\n";
        for (const auto& io : ioPerformance_) {
            report << "  " << io.operation << ":\n";
            report << "    Read Speed: " << io.readSpeedMBps << " MB/s\n";
            report << "    Write Speed: " << io.writeSpeedMBps << " MB/s\n\n";
        }
    }
    
    report << "=== End of Report ===\n";
    return report.str();
}

bool PerformanceProfiler::exportToJSON(const std::string& filepath) const {
    // TODO: Implement JSON export
    return false;
}

bool PerformanceProfiler::exportToCSV(const std::string& filepath) const {
    // TODO: Implement CSV export
    return false;
}

void PerformanceProfiler::reset() {
    std::lock_guard<std::mutex> lock(dataMutex_);
    performanceData_.clear();
    threadPerformance_.clear();
    ioPerformance_.clear();
}

void PerformanceProfiler::updateConfig(const ProfilingConfig& config) {
    std::lock_guard<std::mutex> lock(dataMutex_);
    config_ = config;
}

} // namespace nlm
