// NLM Performance Integration
// Main integration of benchmarking and monitoring tools with existing NLM infrastructure

#pragma once

#include "../benchmarking/BenchmarkSuite.hpp"
#include "../monitoring/PerformanceMonitor.hpp"
#include "../core/Config/Config.hpp"
#include "../experiments/ExperimentRunner.hpp"
#include <memory>
#include <string>
#include <vector>
#include <map>
#include <chrono>

namespace nlm {

// Performance integration result
struct IntegrationResult {
    // Benchmark results
    BenchmarkSuiteResult benchmarkResult;
    
    // Monitoring results
    PerformanceMetrics monitoringMetrics;
    
    // Experiment integration
    std::map<std::string, float> experimentMetrics;
    
    // System analysis
    std::map<std::string, double> systemAnalysis;
    
    // Overall assessment
    double integrationScore;        // Overall integration quality (0-1)
    std::string status;             // "PASS", "WARNING", "FAIL"
    std::vector<std::string> issues; // Detected issues
    
    // Timestamp
    uint64_t timestamp;
    std::chrono::steady_clock::time_point completedTime;
};

// Performance integration manager
class PerformanceIntegration {
public:
    PerformanceIntegration();
    ~PerformanceIntegration();
    
    void initialize(std::shared_ptr<Config> config);
    
    // Integration with main.cpp functionality
    IntegrationResult integrateWithMainDemo(const std::string& demoType = "phase6");
    IntegrationResult integrateWithPhase6Demo();
    IntegrationResult integrateWithPhase2Demo();
    
    // Integration with existing experiment infrastructure
    IntegrationResult integrateWithExperimentRunner(std::shared_ptr<ExperimentRunner> experimentRunner);
    IntegrationResult integrateWithPhase6Experiment();
    
    // Integration with visualization
    IntegrationResult integrateWithVisualization(const std::string& vizType = "basic");
    
    // Integration with agent system
    IntegrationResult integrateWithAgentSystem(std::shared_ptr<AgentBrain> agent);
    
    // Integration with Python API
    IntegrationResult integrateWithPythonAPI();
    
    // Configuration and setup
    void enableBenchmarkSuite(bool enable) { benchmarkSuiteEnabled_ = enable; }
    void enablePerformanceMonitoring(bool enable) { performanceMonitoringEnabled_ = enable; }
    void enableRegressionTesting(bool enable) { regressionTestingEnabled_ = enable; }
    void enableRealTimeMonitoring(bool enable) { realTimeMonitoringEnabled_ = enable; }
    
    // Configuration files
    void loadPerformanceConfig(const std::string& configFile);
    void savePerformanceConfig(const std::string& configFile);
    
    // Status and control
    bool isInitialized() const { return initialized_; }
    bool isRunning() const { return running_; }
    double getIntegrationProgress() const { return integrationProgress_; }
    std::string getCurrentStatus() const { return currentStatus_; }
    
    // Results management
    IntegrationResult getLastIntegrationResult() const { return lastResult_; }
    std::vector<IntegrationResult> getIntegrationHistory() const { return integrationHistory_; }
    
    // Reporting
    void generateIntegrationReport(const std::string& outputDirectory = "integration_reports");
    void generatePerformanceReport(const std::string& outputDirectory = "performance_reports");
    
    // Performance tools
    BenchmarkManager& getBenchmarkManager() { return benchmarkManager_; }
    PerformanceMonitor& getPerformanceMonitor() { return performanceMonitor_; }
    
private:
    std::shared_ptr<Config> config_;
    
    bool initialized_;
    bool running_;
    bool benchmarkSuiteEnabled_;
    bool performanceMonitoringEnabled_;
    bool regressionTestingEnabled_;
    bool realTimeMonitoringEnabled_;
    
    double integrationProgress_;
    std::string currentStatus_;
    
    IntegrationResult lastResult_;
    std::vector<IntegrationResult> integrationHistory_;
    
    std::chrono::steady_clock::time_point startTime_;
    
    // Core components
    BenchmarkManager benchmarkManager_;
    PerformanceMonitor performanceMonitor_;
    
    // Integration components
    std::unique_ptr<PerformanceRegressionTester> regressionTester_;
    std::unique_ptr<ComparativeBenchmark> comparativeBenchmark_;
    std::unique_ptr<StressTestSuite> stressTestSuite_;
    
    // Integration helpers
    IntegrationResult runBenchmarkIntegration();
    IntegrationResult runMonitoringIntegration();
    IntegrationResult runExperimentIntegration();
    IntegrationResult runVisualizationIntegration();
    IntegrationResult runAgentIntegration();
    IntegrationResult runPythonAPIIntegration();
    
    void updateIntegrationProgress(double progress, const std::string& status);
    void updatePerformanceMetrics();
    void detectIntegrationIssues(IntegrationResult& result);
    double calculateIntegrationScore(const IntegrationResult& result) const;
    
    std::string getDemoIntegrationStatus(const std::string& demoType) const;
    IntegrationResult runDemoBenchmark(const std::string& demoType);
};

// Convenience functions for integration
inline IntegrationResult performFullIntegration(std::shared_ptr<Config> config) {
    PerformanceIntegration integrator;
    integrator.initialize(config);
    integrator.enableBenchmarkSuite(true);
    integrator.enablePerformanceMonitoring(true);
    integrator.enableRegressionTesting(true);
    integrator.enableRealTimeMonitoring(true);
    
    return integrator.integrateWithMainDemo("phase6");
}

inline IntegrationResult performBenchmarkIntegration(std::shared_ptr<Config> config) {
    PerformanceIntegration integrator;
    integrator.initialize(config);
    integrator.enableBenchmarkSuite(true);
    integrator.enableRealTimeMonitoring(false);
    
    return integrator.runBenchmarkIntegration();
}

inline IntegrationResult performMonitoringIntegration(std::shared_ptr<Config> config) {
    PerformanceIntegration integrator;
    integrator.initialize(config);
    integrator.enableBenchmarkSuite(false);
    integrator.enablePerformanceMonitoring(true);
    integrator.enableRealTimeMonitoring(true);
    
    return integrator.runMonitoringIntegration();
}

inline IntegrationResult performRegressionIntegration(std::shared_ptr<Config> config) {
    PerformanceIntegration integrator;
    integrator.initialize(config);
    integrator.enableBenchmarkSuite(true);
    integrator.enableRegressionTesting(true);
    
    return integrator.integrateWithMainDemo("phase6");
}

} // namespace nlm
