#pragma once

#include <chrono>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <map>
#include <memory>
#include <sstream>
#include <string>
#include <thread>
#include <vector>
#include <unordered_map>
#include <algorithm>

namespace nlm {

// Performance measurement types
struct PerformanceMetrics {
    // Time measurements (in microseconds)
    uint64_t totalTime;
    uint64_t cpuTime;
    uint64_t wallClockTime;
    
    // Memory measurements (in bytes)
    size_t peakMemory;
    size_t currentMemory;
    
    // Operation counts
    uint64_t spikeEventsProcessed;
    uint64_t neuronSteps;
    uint64_t synapticOperations;
    uint64_t plasticityUpdates;
    
    // Performance counters
    float averageFiringRate;
    float synapticEfficiency;
    float plasticityRate;
    
    PerformanceMetrics()
        : totalTime(0), cpuTime(0), wallClockTime(0)
        , peakMemory(0), currentMemory(0)
        , spikeEventsProcessed(0), neuronSteps(0)
        , synapticOperations(0), plasticityUpdates(0)
        , averageFiringRate(0.0f), synapticEfficiency(0.0f)
        , plasticityRate(0.0f) {}
};

// Performance sampling for real-time monitoring
class PerformanceSampler {
public:
    PerformanceSampler() = default;
    ~PerformanceSampler() = default;
    
    // Start sampling
    void startSampling() {
        samplingStartTime = std::chrono::high_resolution_clock::now();
        isSampling = true;
        
        // Start background thread for continuous sampling
        samplingThread = std::thread(&PerformanceSampler::samplingLoop, this);
    }
    
    // Stop sampling
    void stopSampling() {
        isSampling = false;
        if (samplingThread.joinable()) {
            samplingThread.join();
        }
        
        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(endTime - samplingStartTime);
        
        metrics.wallClockTime = duration.count();
    }
    
    // Get current metrics
    PerformanceMetrics getCurrentMetrics() const {
        PerformanceMetrics current = metrics;
        
        // Update wall clock time
        if (isSampling) {
            auto now = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::microseconds>(now - samplingStartTime);
            current.wallClockTime = duration.count();
        }
        
        return current;
    }
    
    // Increment performance counters
    void incrementSpikeEvents() {
        ++metrics.spikeEventsProcessed;
    }
    
    void incrementNeuronSteps() {
        ++metrics.neuronSteps;
    }
    
    void incrementSynapticOperations() {
        ++metrics.synapticOperations;
    }
    
    void incrementPlasticityUpdates() {
        ++metrics.plasticityUpdates;
    }
    
    void setAverageFiringRate(float rate) {
        metrics.averageFiringRate = rate;
    }
    
    // Log metrics to file
    void logToFile(const std::string& filename) const {
        std::ofstream file(filename, std::ios::app);
        if (!file.is_open()) {
            return;
        }
        
        auto metrics = getCurrentMetrics();
        auto now = std::chrono::high_resolution_clock::now();
        auto timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count();
        
        file << "[" << timestamp << "] ";
        file << "Neurons: " << metrics.neuronSteps << ", ";
        file << "Spikes: " << metrics.spikeEventsProcessed << ", ";
        file << "Synapses: " << metrics.synapticOperations << ", ";
        file << "Plasticity: " << metrics.plasticityUpdates << ", ";
        file << "AvgFiring: " << metrics.averageFiringRate << ", ";
        file << "WallTime: " << metrics.wallClockTime << "us\n";
        
        file.close();
    }
    
    // Generate performance report
    std::string generateReport() const {
        std::ostringstream oss;
        
        oss << "=== NLM Performance Report ===\n";
        oss << "Wall Clock Time: " << metrics.wallClockTime << " microseconds\n";
        oss << "CPU Time: " << metrics.cpuTime << " microseconds\n";
        oss << "Total Time: " << metrics.totalTime << " microseconds\n";
        
        oss << "\nOperation Counts:\n";
        oss << "  Spike Events Processed: " << metrics.spikeEventsProcessed << "\n";
        oss << "  Neuron Steps: " << metrics.neuronSteps << "\n";
        oss << "  Synaptic Operations: " << metrics.synapticOperations << "\n";
        oss << "  Plasticity Updates: " << metrics.plasticityUpdates << "\n";
        
        oss << "\nPerformance Metrics:\n";
        oss << "  Average Firing Rate: " << metrics.averageFiringRate << " Hz\n";
        oss << "  Synaptic Efficiency: " << metrics.synapticEfficiency << "\n";
        oss << "  Plasticity Rate: " << metrics.plasticityRate << "\n";
        
        oss << "\nMemory Usage:\n";
        oss << "  Peak Memory: " << metrics.peakMemory << " bytes\n";
        oss << "  Current Memory: " << metrics.currentMemory << " bytes\n";
        
        return oss.str();
    }
    
private:
    void samplingLoop() {
        while (isSampling) {
            // Update metrics
            auto now = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::microseconds>(now - samplingStartTime);
            metrics.totalTime = duration.count();
            
            // Sleep for sampling interval (100ms)
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
    }
    
    std::chrono::high_resolution_clock::time_point samplingStartTime;
    std::thread samplingThread;
    bool isSampling = false;
    mutable PerformanceMetrics metrics;
};

// Timeline profiling for detailed performance analysis
class TimelineProfiler {
public:
    TimelineProfiler() = default;
    ~TimelineProfiler() = default;
    
    // Profile entry
    struct ProfileEntry {
        std::string operation;
        std::chrono::high_resolution_clock::time_point startTime;
        std::chrono::high_resolution_clock::time_point endTime;
        uint64_t duration;  // microseconds
        size_t threadId;
        
        ProfileEntry(const std::string& op, size_t thread)
            : operation(op), threadId(thread) {}
    };
    
    // Start profiling an operation
    void startProfile(const std::string& operation) {
        std::lock_guard<std::mutex> lock(profileMutex);
        
        ProfileEntry entry(operation, std::hash<std::thread::id>{}(std::this_thread::get_id()));
        entry.startTime = std::chrono::high_resolution_clock::now();
        
        currentProfiles.push_back(std::move(entry));
    }
    
    // End profiling the current operation
    void endProfile() {
        std::lock_guard<std::mutex> lock(profileMutex);
        
        if (currentProfiles.empty()) {
            return;
        }
        
        ProfileEntry& entry = currentProfiles.back();
        entry.endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(entry.endTime - entry.startTime);
        entry.duration = duration.count();
        
        // Move to completed profiles
        completedProfiles.push_back(std::move(entry));
        currentProfiles.pop_back();
    }
    
    // Get current profile (if any)
    bool hasCurrentProfile() const {
        return !currentProfiles.empty();
    }
    
    // Get total profile count
    size_t getProfileCount() const {
        return completedProfiles.size();
    }
    
    // Get profile for specific operation
    std::vector<ProfileEntry> getProfilesForOperation(const std::string& operation) const {
        std::vector<ProfileEntry> matchingProfiles;
        
        for (const auto& profile : completedProfiles) {
            if (profile.operation == operation) {
                matchingProfiles.push_back(profile);
            }
        }
        
        return matchingProfiles;
    }
    
    // Generate timeline report
    std::string generateTimelineReport() const {
        std::ostringstream oss;
        
        oss << "=== Performance Timeline Report ===\n";
        oss << "Total Operations Profiled: " << completedProfiles.size() << "\n";
        oss << "Average Duration: " << getAverageDuration() << " microseconds\n";
        oss << "Max Duration: " << getMaxDuration() << " microseconds\n";
        oss << "Min Duration: " << getMinDuration() << " microseconds\n";
        
        oss << "\nOperation Breakdown:\n";
        auto operationCounts = getOperationCounts();
        for (const auto& pair : operationCounts) {
            oss << "  " << pair.first << ": " << pair.second << " calls\n";
        }
        
        oss << "\nDetailed Timeline:\n";
        for (size_t i = 0; i < std::min<size_t>(completedProfiles.size(), 50); ++i) {
            const auto& profile = completedProfiles[i];
            oss << "  " << std::setw(6) << profile.duration << " us: " 
                << std::left << std::setw(30) << profile.operation << " (thread " 
                << profile.threadId << ")\n";
        }
        
        if (completedProfiles.size() > 50) {
            oss << "  ... and " << (completedProfiles.size() - 50) << " more operations\n";
        }
        
        return oss.str();
    }
    
    // Clear all profiles
    void clear() {
        std::lock_guard<std::mutex> lock(profileMutex);
        completedProfiles.clear();
        currentProfiles.clear();
    }
    
private:
    uint64_t getAverageDuration() const {
        if (completedProfiles.empty()) return 0;
        
        uint64_t total = 0;
        for (const auto& profile : completedProfiles) {
            total += profile.duration;
        }
        
        return total / completedProfiles.size();
    }
    
    uint64_t getMaxDuration() const {
        if (completedProfiles.empty()) return 0;
        
        uint64_t max = 0;
        for (const auto& profile : completedProfiles) {
            if (profile.duration > max) max = profile.duration;
        }
        
        return max;
    }
    
    uint64_t getMinDuration() const {
        if (completedProfiles.empty()) return 0;
        
        uint64_t min = UINT64_MAX;
        for (const auto& profile : completedProfiles) {
            if (profile.duration < min) min = profile.duration;
        }
        
        return min;
    }
    
    std::unordered_map<std::string, size_t> getOperationCounts() const {
        std::unordered_map<std::string, size_t> counts;
        
        for (const auto& profile : completedProfiles) {
            counts[profile.operation]++;
        }
        
        return counts;
    }
    
    mutable std::mutex profileMutex;
    std::vector<ProfileEntry> currentProfiles;
    std::vector<ProfileEntry> completedProfiles;
};

// Performance analyzer for bottleneck detection
class PerformanceAnalyzer {
public:
    PerformanceAnalyzer() = default;
    ~PerformanceAnalyzer() = default;
    
    // Analyze performance for specific time period
    struct AnalysisResult {
        std::string bottleneck;
        double bottleneckPercentage;
        std::string recommendation;
        std::vector<std::string> affectedOperations;
    };
    
    // Analyze timeline profiles for bottlenecks
    AnalysisResult analyzeForBottlenecks(const TimelineProfiler& timelineProfiler) {
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
    
    // Generate performance optimization recommendations
    std::vector<std::string> generateOptimizationRecommendations(const TimelineProfiler& timelineProfiler) {
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
    
private:
    std::vector<std::string> getAffectedOperations(const std::string& operation) const {
        // Return operations that depend on or are affected by the given operation
        return { operation + "_dependency1", operation + "_dependency2", operation + "_initialization" };
    }
    
    std::vector<std::string> getLongOperations(const TimelineProfiler& timelineProfiler, uint64_t threshold) const {
        std::vector<std::string> longOps;
        
        for (const auto& profile : timelineProfiler.getProfilesForOperation("")) {
            if (profile.duration > threshold) {
                longOps.push_back(profile.operation);
            }
        }
        
        return longOps;
    }
};

// Real-time performance monitor
class RealTimePerformanceMonitor {
public:
    RealTimePerformanceMonitor() = default;
    ~RealTimePerformanceMonitor() = default;
    
    // Update monitor with current performance data
    void update(const PerformanceMetrics& metrics) {
        std::lock_guard<std::mutex> lock(dataMutex);
        
        // Update historical data (keep last 100 samples)
        history.push_back(metrics);
        if (history.size() > 100) {
            history.erase(history.begin());
        }
        
        // Calculate current statistics
        currentMetrics = metrics;
        
        // Update real-time alerts
        checkForAlerts(metrics);
    }
    
    // Get current performance metrics
    PerformanceMetrics getCurrentMetrics() const {
        std::lock_guard<std::mutex> lock(dataMutex);
        return currentMetrics;
    }
    
    // Get historical average performance
    PerformanceMetrics getAverageMetrics() const {
        std::lock_guard<std::mutex> lock(dataMutex);
        
        if (history.empty()) {
            return PerformanceMetrics();
        }
        
        PerformanceMetrics avg;
        size_t count = history.size();
        
        for (const auto& metrics : history) {
            avg.totalTime += metrics.totalTime;
            avg.cpuTime += metrics.cpuTime;
            avg.wallClockTime += metrics.wallClockTime;
            avg.spikeEventsProcessed += metrics.spikeEventsProcessed;
            avg.neuronSteps += metrics.neuronSteps;
            avg.synapticOperations += metrics.synapticOperations;
            avg.plasticityUpdates += metrics.plasticityUpdates;
            avg.averageFiringRate += metrics.averageFiringRate;
        }
        
        avg.totalTime /= count;
        avg.cpuTime /= count;
        avg.wallClockTime /= count;
        avg.averageFiringRate /= count;
        
        return avg;
    }
    
    // Check for performance alerts
    std::vector<std::string> getAlerts() const {
        std::lock_guard<std::mutex> lock(dataMutex);
        return alerts;
    }
    
    // Clear all alerts
    void clearAlerts() {
        std::lock_guard<std::mutex> lock(dataMutex);
        alerts.clear();
    }
    
    // Generate performance dashboard
    std::string generateDashboard() const {
        std::ostringstream oss;
        
        oss << "=== Performance Dashboard ===\n";
        oss << "Current Metrics:\n";
        oss << "  Total Time: " << currentMetrics.totalTime << " us\n";
        oss << "  Wall Clock: " << currentMetrics.wallClockTime << " us\n";
        oss << "  CPU Time: " << currentMetrics.cpuTime << " us\n";
        oss << "  Neuron Steps: " << currentMetrics.neuronSteps << "\n";
        oss << "  Spike Events: " << currentMetrics.spikeEventsProcessed << "\n";
        oss << "  Average Firing Rate: " << currentMetrics.averageFiringRate << " Hz\n";
        
        oss << "\nAlerts: " << alerts.size() << "\n";
        for (const auto& alert : alerts) {
            oss << "  - " << alert << "\n";
        }
        
        if (alerts.empty()) {
            oss << "  No active alerts\n";
        }
        
        return oss.str();
    }
    
private:
    void checkForAlerts(const PerformanceMetrics& metrics) {
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
    
    mutable std::mutex dataMutex;
    PerformanceMetrics currentMetrics;
    std::vector<PerformanceMetrics> history;
    std::vector<std::string> alerts;
};

// Convenience functions for performance monitoring
namespace Performance {
    // Global performance sampler instance
    static PerformanceSampler globalSampler;
    
    // Global timeline profiler instance
    static TimelineProfiler globalProfiler;
    
    // Global performance analyzer instance
    static PerformanceAnalyzer globalAnalyzer;
    
    // Global performance monitor instance
    static RealTimePerformanceMonitor globalMonitor;
    
    // Start global performance monitoring
    inline void startMonitoring() {
        globalSampler.startSampling();
    }
    
    // Stop global performance monitoring
    inline void stopMonitoring() {
        globalSampler.stopSampling();
    }
    
    // Start timeline profiling
    inline void startProfiling(const std::string& operation) {
        globalProfiler.startProfile(operation);
    }
    
    // End timeline profiling
    inline void endProfiling() {
        globalProfiler.endProfile();
    }
    
    // Update global monitor
    inline void updateMonitor(const PerformanceMetrics& metrics) {
        globalMonitor.update(metrics);
    }
    
    // Get global performance report
    inline std::string getPerformanceReport() {
        std::ostringstream oss;
        
        oss << "=== Global Performance Report ===\n";
        oss << globalSampler.generateReport() << "\n";
        oss << globalProfiler.generateTimelineReport() << "\n";
        
        auto recommendations = globalAnalyzer.generateOptimizationRecommendations(globalProfiler);
        oss << "Optimization Recommendations:\n";
        for (const auto& rec : recommendations) {
            oss << "  " << rec << "\n";
        }
        
        oss << "\n" << globalMonitor.generateDashboard();
        
        return oss.str();
    }
    
    // Log performance to file
    inline void logPerformance(const std::string& filename) {
        globalSampler.logToFile(filename);
    }
    
    // Get current alerts
    inline std::vector<std::string> getAlerts() {
        return globalMonitor.getAlerts();
    }
}

} // namespace nlm
