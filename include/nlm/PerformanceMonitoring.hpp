#pragma once

#include <string>
#include <map>
#include <memory>
#include <functional>
#include <mutex>
#include <chrono>
#include <unordered_map>
#include <iomanip>
#include <sstream>
#include <vector>
#include <algorithm>
#include <numeric>
#include <cmath>
#include <fstream>

namespace nlm {

// Performance monitoring and debugging tools
class PerformanceMonitor {
public:
    struct PerformanceSnapshot {
        std::chrono::system_clock::time_point timestamp;
        std::string operation;
        std::string component;
        double duration;  // in seconds
        double cpuUsage;  // percentage
        double memoryUsage;  // in MB
        size_t neuronCount;
        size_t synapseCount;
        float firingRate;
        std::map<std::string, double> customMetrics;
        
        PerformanceSnapshot(const std::string& op, const std::string& comp)
            : operation(op), component(comp), duration(0.0), cpuUsage(0.0), 
              memoryUsage(0.0), neuronCount(0), synapseCount(0), firingRate(0.0) {}
    };
    
    PerformanceMonitor() : enableRealTimeTracking_(false), 
                           maxHistorySize_(10000),
                           enableDetailedMetrics_(true),
                           enableAlertThresholds_(true) {}
    
    void enableRealTimeTracking(bool enable) { enableRealTimeTracking_ = enable; }
    void setMaxHistorySize(size_t size) { maxHistorySize_ = size; }
    void enableDetailedMetrics(bool enable) { enableDetailedMetrics_ = enable; }
    void enableAlertThresholds(bool enable) { enableAlertThresholds_ = enable; }
    
    void startMeasurement(const std::string& operation, const std::string& component = "");
    PerformanceSnapshot stopMeasurement(const std::string& operation);
    
    void recordMetric(const std::string& operation, const std::string& component,
                     double duration, bool success = true,
                     const std::map<std::string, double>& customMetrics = {});
    
    PerformanceSnapshot getCurrentMetrics(const std::string& operation = "",
                                         const std::string& component = "") const;
    
    std::vector<PerformanceSnapshot> getMetrics(const std::string& operation = "",
                                                const std::string& component = "",
                                                std::chrono::system_clock::time_point since = {}) const;
    
    double getAverageDuration(const std::string& operation = "",
                             const std::string& component = "",
                             std::chrono::system_clock::time_point since = {}) const;
    
    double getSuccessRate(const std::string& operation = "",
                         const std::string& component = "",
                         std::chrono::system_clock::time_point since = {}) const;
    
    std::string getPerformanceReport(const std::string& operation = "",
                                    const std::string& component = "",
                                    std::chrono::system_clock::time_point since = {}) const;
    
    // Advanced analytics
    double calculateMovingAverage(const std::string& operation,
                                  const std::string& component,
                                  size_t windowSize = 100,
                                  std::chrono::system_clock::time_point since = {}) const;
    
    std::vector<double> getMovingAverageSeries(const std::string& operation,
                                              const std::string& component = "",
                                              size_t windowSize = 100,
                                              std::chrono::system_clock::time_point since = {}) const;
    
    // Anomaly detection
    std::vector<PerformanceSnapshot> detectAnomalies(const std::string& operation,
                                                    const std::string& component = "",
                                                    double threshold = 2.0,
                                                    std::chrono::system_clock::time_point since = {}) const;
    
    // Memory and resource tracking
    struct MemoryInfo {
        size_t totalMemory;
        size_t usedMemory;
        size_t availableMemory;
        double memoryUtilization;
        size_t peakMemory;
        std::chrono::system_clock::time_point lastMeasured;
    };
    
    MemoryInfo getMemoryInfo() const;
    
    // System-level metrics
    struct SystemInfo {
        double cpuUsage;
        size_t totalCores;
        std::chrono::system_clock::time_point uptime;
        std::string osInfo;
    };
    
    SystemInfo getSystemInfo() const;
    
    // Configuration and alerts
    struct AlertThreshold {
        std::string metric;
        double warningThreshold;
        double criticalThreshold;
        std::string message;
        std::string severity;
    };
    
    void addAlertThreshold(const AlertThreshold& threshold);
    void clearAlertThresholds();
    
    std::vector<std::string> checkAlerts(const PerformanceSnapshot& metrics) const;
    
    // Data export and import
    bool exportMetrics(const std::string& filepath) const;
    bool importMetrics(const std::string& filepath);
    
    std::string exportMetricsJSON() const;
    bool importMetricsJSON(const std::string& jsonData);
    
    // Performance comparison
    std::string comparePerformance(const std::vector<PerformanceSnapshot>& snapshots) const;
    
private:
    mutable std::mutex metricsMutex_;
    std::unordered_map<std::string, std::vector<PerformanceSnapshot>> metrics_;
    
    // Current measurements
    std::map<std::pair<std::string, std::string>, PerformanceSnapshot> currentMetrics_;
    
    // Configuration
    bool enableRealTimeTracking_;
    size_t maxHistorySize_;
    bool enableDetailedMetrics_;
    bool enableAlertThresholds_;
    
    // Alert thresholds
    std::vector<AlertThreshold> alertThresholds_;
    
    double calculateStandardDeviation(const std::vector<double>& values) const;
    double calculateMean(const std::vector<double>& values) const;
};

// Advanced debugging tools
class Debugger {
public:
    enum class DebugLevel {
        None,
        Error,
        Warning,
        Info,
        Debug,
        Trace
    };
    
    struct DebugInfo {
        std::chrono::system_clock::time_point timestamp;
        DebugLevel level;
        std::string source;
        std::string message;
        std::map<std::string, std::string> context;
        std::map<std::string, std::string> metadata;
        std::string stackTrace;
    };
    
    Debugger() : currentLevel_(DebugLevel::Info), enableStackTrace_(true),
                 enableContextTracking_(true), maxLogSize_(10000) {}
    
    void setLevel(DebugLevel level) { currentLevel_ = level; }
    DebugLevel getLevel() const { return currentLevel_; }
    
    void enableStackTrace(bool enable) { enableStackTrace_ = enable; }
    void enableContextTracking(bool enable) { enableContextTracking_ = enable; }
    
    // Core debugging functions
    void log(const std::string& message, DebugLevel level = DebugLevel::Info,
             const std::string& source = "", 
             const std::map<std::string, std::string>& context = {});
    
    void logError(const std::string& message, const std::string& source = "",
                  const std::map<std::string, std::string>& context = {});
    
    void logWarning(const std::string& message, const std::string& source = "",
                    const std::map<std::string, std::string>& context = {});
    
    void logInfo(const std::string& message, const std::string& source = "",
                 const std::map<std::string, std::string>& context = {});
    
    void logDebug(const std::string& message, const std::string& source = "",
                  const std::map<std::string, std::string>& context = {});
    
    // Advanced debugging features
    void startDebugSession(const std::string& sessionName);
    void endDebugSession(const std::string& sessionName);
    
    std::vector<DebugInfo> getDebugLogs(DebugLevel minLevel = DebugLevel::Debug) const;
    std::vector<DebugInfo> getDebugLogs(const std::string& source,
                                       DebugLevel minLevel = DebugLevel::Debug) const;
    
    void clearDebugLogs();
    
    // Memory debugging
    void logMemoryInfo(const std::string& context = "");
    void checkMemoryLeaks();
    
    // Neural network debugging
    void logNeuralActivity(const std::string& context = "");
    void logPlasticityChanges(const std::string& context = "");
    void logDevelopmentProgress(const std::string& context = "");
    
    // Performance debugging
    void logPerformanceMetrics(const std::string& context = "");
    void logBottleneckAnalysis(const std::string& context = "");
    
    // State inspection
    std::string inspectBrainState() const;
    std::string inspectAgentState() const;
    std::string inspectWorldState() const;
    
    // File output
    bool exportDebugLogs(const std::string& filepath) const;
    bool importDebugLogs(const std::string& filepath);
    
    std::string exportDebugLogsJSON() const;
    bool importDebugLogsJSON(const std::string& jsonData);
    
    // Profiling
    void startProfiling(const std::string& profileName);
    void stopProfiling(const std::string& profileName);
    
    std::string getProfilingResults(const std::string& profileName = "") const;
    
private:
    mutable std::mutex debugMutex_;
    std::vector<DebugInfo> debugLogs_;
    
    DebugLevel currentLevel_;
    bool enableStackTrace_;
    bool enableContextTracking_;
    size_t maxLogSize_;
    
    // Profiling data
    std::unordered_map<std::string, std::vector<std::chrono::duration<double>>> profiles_;
    
    std::string generateStackTrace() const;
    std::string getSystemInfo() const;
    
    void cleanupOldLogs();
};

// Advanced validation and testing tools
class TestingTools {
public:
    struct TestResult {
        std::chrono::system_clock::time_point timestamp;
        std::string testName;
        std::string testCategory;
        bool passed;
        double executionTime;  // in seconds
        std::string errorMessage;
        std::map<std::string, std::string> metadata;
        std::map<std::string, double> metrics;
    };
    
    TestingTools() : enableAutoTesting_(false), autoTestInterval_(std::chrono::seconds(60)) {}
    
    void enableAutoTesting(bool enable, std::chrono::seconds interval = std::chrono::seconds(60));
    
    // Neural network tests
    TestResult testNeuronDynamics(const std::string& testName = "Neuron Dynamics Test");
    TestResult testSynapticTransmission(const std::string& testName = "Synaptic Transmission Test");
    TestResult testPlasticityRules(const std::string& testName = "Plasticity Rules Test");
    TestResult testMemorySystems(const std::string& testName = "Memory Systems Test");
    TestResult testNeuromodulation(const std::string& testName = "Neuromodulation Test");
    
    // Agent tests
    TestResult testAgentBehavior(const std::string& testName = "Agent Behavior Test");
    TestResult testSensoryProcessing(const std::string& testName = "Sensory Processing Test");
    TestResult testMotorControl(const std::string& testName = "Motor Control Test");
    TestResult testLearningCapability(const std::string& testName = "Learning Capability Test");
    
    // World integration tests
    TestResult testWorldInteraction(const std::string& testName = "World Interaction Test");
    TestResult testEnvironmentModeling(const std::string& testName = "Environment Modeling Test");
    TestResult testRewardSystem(const std::string& testName = "Reward System Test");
    
    // Performance tests
    TestResult testPerformance(const std::string& testName = "Performance Test");
    TestResult testScalability(const std::string& testName = "Scalability Test");
    TestResult testMemoryUsage(const std::string& testName = "Memory Usage Test");
    
    // Comprehensive testing
    TestResult runComprehensiveTestSuite(const std::string& testSuiteName = "Comprehensive Test Suite");
    TestResult runRegressionTest(const std::string& testName = "Regression Test");
    
    std::vector<TestResult> getTestResults(const std::string& testName = "",
                                          const std::string& testCategory = "") const;
    
    double getTestSuccessRate(const std::string& testName = "",
                             const std::string& testCategory = "") const;
    
    std::string getTestReport(const std::string& testName = "",
                             const std::string& testCategory = "") const;
    
    // Test configuration
    struct TestConfiguration {
        std::string testName;
        std::string testCategory;
        std::chrono::duration<double> timeout;
        std::map<std::string, std::string> parameters;
        bool enabled;
        std::string description;
    };
    
    void addTestConfiguration(const TestConfiguration& config);
    void removeTestConfiguration(const std::string& testName);
    
    std::vector<TestConfiguration> getTestConfigurations(const std::string& testCategory = "") const;
    
    // Stress testing
    TestResult runStressTest(const std::string& testName = "Stress Test",
                            const std::map<std::string, std::string>& parameters = {});
    
    TestResult runLoadTest(const std::string& testName = "Load Test",
                          const std::map<std::string, std::string>& parameters = {});
    
    // Validation testing
    TestResult runValidationTest(const std::string& testName = "Validation Test");
    TestResult runConsistencyTest(const std::string& testName = "Consistency Test");
    
private:
    mutable std::mutex testMutex_;
    std::vector<TestResult> testResults_;
    
    bool enableAutoTesting_;
    std::chrono::seconds autoTestInterval_;
    
    std::unordered_map<std::string, TestConfiguration> testConfigurations_;
    
    TestResult executeTest(const TestConfiguration& config);
    TestResult createTestResult(const std::string& testName,
                               const std::string& testCategory,
                               bool passed,
                               double executionTime,
                               const std::string& errorMessage = "",
                               const std::map<std::string, std::string>& metadata = {});
};

// Advanced configuration and optimization tools
class OptimizationTools {
public:
    struct PerformanceProfile {
        std::string profileName;
        std::map<std::string, double> parameters;
        double expectedPerformance;
        std::chrono::duration<double> estimatedTime;
        std::string description;
    };
    
    OptimizationTools() : enableProfileOptimization_(true), enableAutoOptimization_(false) {}
    
    void enableProfileOptimization(bool enable) { enableProfileOptimization_ = enable; }
    void enableAutoOptimization(bool enable) { enableAutoOptimization_ = enable; }
    
    // Profile management
    void addPerformanceProfile(const PerformanceProfile& profile);
    bool removePerformanceProfile(const std::string& profileName);
    
    std::vector<PerformanceProfile> getPerformanceProfiles() const;
    
    // Optimization algorithms
    PerformanceProfile findOptimalProfile(const std::map<std::string, double>& targetPerformance,
                                         const std::vector<PerformanceProfile>& availableProfiles);
    
    std::map<std::string, double> optimizeParameters(const std::string& profileName,
                                                     const std::map<std::string, double>& currentParameters,
                                                     const std::map<std::string, double>& targetValues);
    
    // Performance benchmarking
    double benchmarkOperation(const std::string& operation,
                             const std::map<std::string, std::string>& parameters = {});
    
    std::map<std::string, double> benchmarkSystem(const std::map<std::string, std::string>& scenarios = {});
    
    // Resource optimization
    struct ResourceUsage {
        double cpuUsage;
        double memoryUsage;
        double networkUsage;
        std::chrono::duration<double> timeUsage;
    };
    
    ResourceUsage analyzeResourceUsage(const std::string& operation,
                                       const std::map<std::string, std::string>& parameters = {});
    
    std::vector<std::string> getOptimizationRecommendations(
        const std::map<std::string, double>& currentMetrics,
        const std::map<std::string, double>& targetMetrics);
    
    // Advanced optimization techniques
    std::map<std::string, double> applyParticleSwarmOptimization(
        const std::map<std::string, double>& currentParameters,
        const std::function<double(const std::map<std::string, double>&)>& objectiveFunction,
        size_t iterations = 100);
    
    std::map<std::string, double> applyGeneticAlgorithm(
        const std::map<std::string, double>& currentParameters,
        const std::function<double(const std::map<std::string, double>&)>& objectiveFunction,
        size_t populationSize = 50,
        size_t generations = 100);
    
    // Configuration optimization
    std::map<std::string, double> optimizeNeuralNetworkConfiguration(
        const std::map<std::string, double>& currentConfig,
        const std::map<std::string, double>& performanceTargets);
    
    std::map<std::string, double> optimizeMemoryConfiguration(
        const std::map<std::string, double>& currentConfig,
        const std::map<std::string, double>& performanceTargets);
    
private:
    std::unordered_map<std::string, PerformanceProfile> performanceProfiles_;
    mutable std::mutex optimizationMutex_;
    
    bool enableProfileOptimization_;
    bool enableAutoOptimization_;
    
    double evaluateObjectiveFunction(const std::map<std::string, double>& parameters,
                                    const std::function<double(const std::map<std::string, double>&)>& objectiveFunction);
};

} // namespace nlm