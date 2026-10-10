#pragma once

#include "core/Types/Types.hpp"
#include <string>
#include <vector>
#include <map>
#include <mutex>
#include <chrono>
#include <unordered_map>
#include <iomanip>
#include <sstream>
#include <algorithm>
#include <numeric>
#include <cmath>

namespace nlm {

// Bottleneck analysis result
struct BottleneckResult {
    std::string bottleneckType; // "cpu", "memory", "io", "network", "algorithm"
    std::string componentName; // Name of the component causing the bottleneck
    std::string location; // File and line where bottleneck occurs
    double severity; // 0.0 to 1.0, where 1.0 is critical
    double impact; // Performance impact (0.0 to 1.0)
    std::vector<std::string> symptoms; // Observed symptoms
    std::vector<std::string> causes; // Potential causes
    std::string recommendation; // Recommended fix
    Timestamp timestamp;
    double detectionConfidence; // How confident we are in the detection
};

// Resource usage profile
struct ResourceProfile {
    Timestamp timestamp;
    double cpuUsage; // 0.0 to 1.0
    double memoryUsage; // In MB
    double diskIO; // In MB/s
    double networkIO; // In MB/s
    std::map<std::string, double> customMetrics;
};

// Execution trace entry
struct ExecutionTrace {
    Timestamp timestamp;
    std::string threadId;
    std::string functionName;
    std::string fileName;
    int lineNumber;
    std::string moduleName;
    double executionTime;
    double memoryUsage;
    std::string callStack;
    std::map<std::string, double> metrics;
};

// Bottleneck analyzer - Bottleneck analysis
class BottleneckAnalyzer {
public:
    BottleneckAnalyzer();
    ~BottleneckAnalyzer();
    
    // Analysis control
    void startAnalysis();
    void stopAnalysis();
    bool isAnalyzing() const;
    void resetAnalysis();
    
    // Data collection
    void recordResourceUsage(const ResourceProfile& profile);
    void recordExecutionTrace(const ExecutionTrace& trace);
    void recordSystemEvent(const std::string& eventType, const std::string& message,
                           Timestamp timestamp, double impact = 0.0);
    
    // Analysis
    std::vector<BottleneckResult> analyzeBottlenecks() const;
    std::vector<BottleneckResult> analyzeBottlenecksByType(const std::string& type) const;
    bool hasBottlenecks() const;
    double getOverallBottleneckSeverity() const;
    
    // Resource analysis
    double analyzeCpuUsagePatterns() const;
    double analyzeMemoryUsagePatterns() const;
    double analyzeIoPatterns() const;
    double analyzeNetworkPatterns() const;
    
    // Performance analysis
    std::vector<std::string> identifySlowFunctions(double thresholdTime) const;
    std::vector<std::string> identifyMemoryHogs(size_t thresholdMemory) const;
    std::vector<std::string> identifyCpuHogs(double thresholdCpu) const;
    std::vector<std::string> identifyIoHogs(double thresholdIo) const;
    
    // Time-based analysis
    std::vector<BottleneckResult> analyzeTimeBasedTrends(Timestamp startTime,
                                                          Timestamp endTime) const;
    std::vector<ResourceProfile> getResourceProfiles(Timestamp startTime,
                                                      Timestamp endTime) const;
    std::vector<ExecutionTrace> getExecutionTraces(Timestamp startTime,
                                                   Timestamp endTime) const;
    
    // System analysis
    double analyzeSystemLoad() const;
    double analyzeResponsiveness() const;
    double analyzeResourceUtilization() const;
    
    // Bottleneck detection
    BottleneckResult detectCpuBottleneck() const;
    BottleneckResult detectMemoryBottleneck() const;
    BottleneckResult detectIoBottleneck() const;
    BottleneckResult detectNetworkBottleneck() const;
    BottleneckResult detectAlgorithmBottleneck() const;
    
    // Analysis reports
    std::string generateBottleneckReport() const;
    std::string generateResourceReport() const;
    std::string generateTimeSeriesReport() const;
    std::string generateOptimizationReport() const;
    
    // Bottleneck export/import
    bool exportBottleneckData(const std::string& filepath,
                             const std::string& format = "json") const;
    bool importBottleneckData(const std::string& filepath,
                             const std::string& format = "json");
    
    // Control
    void enable(bool enable);
    bool isEnabled() const;
    void setAnalysisThreshold(double threshold);
    void setDetectionSensitivity(double sensitivity);
    void setTimeBasedAnalysis(bool enable, Timestamp maxAge = 3600.0);
    void setCorrelationAnalysis(bool enable);
    
    // Advanced analysis
    std::map<std::string, double> correlateMetrics(const std::string& metric1,
                                                   const std::string& metric2) const;
    std::vector<std::string> predictPerformanceTrends(Timestamp horizon) const;
    double calculateBottleneckScore(const BottleneckResult& bottleneck) const;
    std::vector<BottleneckResult> prioritizeBottlenecks() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm