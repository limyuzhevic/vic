#include "PerformanceProfiler.hpp"
#include <numeric>

namespace nlm {

// PerformanceSampler implementation
void PerformanceSampler::samplingLoop() {
    while (isSampling) {
        // Update metrics
        auto now = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(now - samplingStartTime);
        metrics.totalTime = duration.count();
        
        // Sleep for sampling interval (100ms)
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}

// TimelineProfiler implementation
std::vector<TimelineProfiler::ProfileEntry> TimelineProfiler::getProfilesForOperation(const std::string& operation) const {
    std::vector<ProfileEntry> matchingProfiles;
    
    for (const auto& profile : completedProfiles) {
        if (profile.operation == operation) {
            matchingProfiles.push_back(profile);
        }
    }
    
    return matchingProfiles;
}

// PerformanceAnalyzer implementation
PerformanceAnalyzer::AnalysisResult PerformanceAnalyzer::analyzeForBottlenecks(const TimelineProfiler& timelineProfiler) {
    AnalysisResult result;
    result.bottleneck = "None identified";
    result.bottleneckPercentage = 0.0;
    result.recommendation = "Performance is acceptable";
    
    auto operationCounts = timelineProfiler.getOperationCounts();
    if (operationCounts.empty()) {
        return result;
    }
    
    // Find most frequent operation
    std::string mostFrequentOp = "";
    size_t maxCount = 0;
    for (const auto& pair : operationCounts) {
        if (pair.second > maxCount) {
            mostFrequentOp = pair.first;
            maxCount = pair.second;
        }
    }
    
    // Get duration statistics for this operation
    auto operationProfiles = timelineProfiler.getProfilesForOperation(mostFrequentOp);
    if (operationProfiles.empty()) {
        return result;
    }
    
    // Calculate total duration for this operation
    uint64_t totalOpDuration = 0;
    for (const auto& profile : operationProfiles) {
        totalOpDuration += profile.duration;
    }
    
    // Get total duration across all operations
    uint64_t totalDuration = 0;
    for (const auto& profile : timelineProfiler.getProfilesForOperation("")) {
        totalDuration += profile.duration;
    }
    
    if (totalDuration > 0) {
        result.bottleneckPercentage = static_cast<double>(totalOpDuration) / totalDuration * 100.0;
        
        if (result.bottleneckPercentage > 50.0) {
            result.bottleneck = mostFrequentOp;
            result.recommendation = "Consider optimizing this operation or reducing its frequency";
            result.affectedOperations = getAffectedOperations(mostFrequentOp);
        } else if (result.bottleneckPercentage > 25.0) {
            result.bottleneck = mostFrequentOp;
            result.recommendation = "Monitor this operation for potential optimization opportunities";
            result.affectedOperations = getAffectedOperations(mostFrequentOp);
        }
    }
    
    return result;
}

std::vector<std::string> PerformanceAnalyzer::generateOptimizationRecommendations(const TimelineProfiler& timelineProfiler) {
    std::vector<std::string> recommendations;
    
    auto analysis = analyzeForBottlenecks(timelineProfiler);
    
    if (analysis.bottleneck != "None identified") {
        recommendations.push_back("Primary bottleneck: " + analysis.bottleneck + " (" + 
                                   std::to_string(analysis.bottleneckPercentage) + "% of total time)");
        recommendations.push_back("Recommendation: " + analysis.recommendation);
    }
    
    // Check for frequent short operations (potential overhead)
    auto operationCounts = timelineProfiler.getOperationCounts();
    for (const auto& pair : operationCounts) {
        if (pair.second > 100 && pair.first.find("Allocation") != std::string::npos) {
            recommendations.push_back("High allocation overhead detected for: " + pair.first + ". Consider using memory pools.");
        }
        
        if (pair.second > 1000 && pair.first.find("Copy") != std::string::npos) {
            recommendations.push_back("High copy overhead detected for: " + pair.first + ". Consider using references or views.");
        }
    }
    
    // Check for long-running operations
    auto longOperations = getLongOperations(timelineProfiler, 10000);  // 10ms threshold
    if (!longOperations.empty()) {
        recommendations.push_back("Found " + std::to_string(longOperations.size()) + 
                               " operations exceeding 10ms. Consider optimizing:");
        for (const auto& op : longOperations) {
            recommendations.push_back("  - " + op);
        }
    }
    
    if (recommendations.empty()) {
        recommendations.push_back("Performance is within acceptable ranges. No immediate optimizations needed.");
    }
    
    return recommendations;
}

std::vector<std::string> PerformanceAnalyzer::getAffectedOperations(const std::string& operation) const {
    // Return operations that depend on or are affected by the given operation
    return { operation + "_dependency1", operation + "_dependency2", operation + "_initialization" };
}

std::vector<std::string> PerformanceAnalyzer::getLongOperations(const TimelineProfiler& timelineProfiler, uint64_t threshold) const {
    std::vector<std::string> longOps;
    
    for (const auto& profile : timelineProfiler.getProfilesForOperation("")) {
        if (profile.duration > threshold) {
            longOps.push_back(profile.operation);
        }
    }
    
    return longOps;
}

// RealTimePerformanceMonitor implementation
void RealTimePerformanceMonitor::checkForAlerts(const PerformanceMetrics& metrics) {
    // Check for performance degradation
    if (metrics.wallClockTime > 1000000) {  // > 1 second
        alerts.push_back("High wall clock time detected: " + std::to_string(metrics.wallClockTime) + " us");
    }
    
    if (metrics.averageFiringRate > 500.0f) {  // > 500 Hz (unrealistic for biological neurons)
        alerts.push_back("Unrealistically high firing rate detected: " + std::to_string(metrics.averageFiringRate) + " Hz");
    }
    
    if (metrics.spikeEventsProcessed > 1000000) {  // > 1M spikes (possible high activity)
        alerts.push_back("Very high spike count detected: " + std::to_string(metrics.spikeEventsProcessed) + " events");
    }
    
    // Check for memory issues
    if (metrics.peakMemory > 1024 * 1024 * 1024) {  // > 1GB
        alerts.push_back("High memory usage detected: " + std::to_string(metrics.peakMemory / (1024 * 1024)) + " MB");
    }
}

} // namespace nlm
