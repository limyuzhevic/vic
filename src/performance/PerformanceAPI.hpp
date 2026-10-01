#pragma once

#include "PerformanceProfiler.hpp"
#include "BenchmarkSuite.hpp"
#include "PerformanceAnalyzer.hpp"
#include "../../brain/Brain.hpp"
#include <string>
#include <memory>
#include <map>

namespace nlm {

class PerformanceAPI {
public:
    // Singleton access
    static PerformanceAPI& getInstance();
    static void shutdown();
    
    // Initialize performance system
    bool initialize(std::shared_ptr<Brain> brain = nullptr);
    void shutdownAPI();

    // Core profiling interface
    bool startProfiling(const std::string& sectionName = "default");
    void stopProfiling(const std::string& sectionName = "default");
    void pauseProfiling(const std::string& sectionName = "default");
    void resumeProfiling(const std::string& sectionName = "default");
    
    // Performance measurement shortcuts
    void profileBrainStep();
    void profileMemoryOperation(size_t bytes, bool isAllocation);
    void profileCPUOperation(size_t operations = 1);
    void profileIOOperation(size_t bytes, bool isRead);
    
    // Real-time monitoring
    void startRealTimeMonitoring(std::chrono::milliseconds interval = std::chrono::milliseconds(100));
    void stopRealTimeMonitoring();
    bool isMonitoring() const;
    
    // Benchmark execution
    std::vector<BenchmarkSuite::BenchmarkResult> runPerformanceBenchmarks(const std::string& benchmarkType = "all");
    void scheduleBenchmark(const std::string& benchmarkName, 
                          std::chrono::system_clock::time_point when);
    
    // Analysis and reporting
    std::string generatePerformanceReport() const;
    std::string getPerformanceSummary() const;
    std::vector<std::string> getBottleneckSections(double threshold = 0.2) const;
    std::map<std::string, double> getPerformanceMetrics() const;
    
    // Integration with Brain
    void enableBrainIntegration(std::shared_ptr<Brain> brain);
    void disableBrainIntegration();
    bool isBrainIntegrated() const;
    void profileBrainStep(const std::string& operation = "general");
    
    // Configuration
    void setProfilingInterval(std::chrono::milliseconds interval);
    void setMemoryTracking(bool enable);
    void setCPUTracking(bool enable);
    void setIOTracking(bool enable);
    void setGPUTracking(bool enable);
    void setDetailedTiming(bool enable);
    void setMaxBenchmarkConcurrency(size_t count);
    
    // Data export
    bool exportPerformanceData(const std::string& filename, const std::string& format = "json") const;
    bool exportBenchmarkResults(const std::string& filename, const std::string& format = "csv") const;
    bool exportAnalysisResults(const std::string& filename, const std::string& format = "json") const;
    
    // Automation
    void enablePerformanceValidation(const std::map<std::string, double>& requirements);
    void disablePerformanceValidation();
    bool isPerformanceValidationEnabled() const;
    std::vector<std::string> validatePerformanceRequirements() const;
    
    // Control
    void resetPerformanceData();
    void clearAllData();
    void pauseAllProfiling();
    void resumeAllProfiling();
    void stopAllProfiling();

private:
    PerformanceAPI();
    ~PerformanceAPI();
    PerformanceAPI(const PerformanceAPI&) = delete;
    PerformanceAPI& operator=(const PerformanceAPI&) = delete;
    
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    static PerformanceAPI* instance;
};

} // namespace nlm
