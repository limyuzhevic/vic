#pragma once

#include <string>
#include <vector>
#include <memory>
#include <map>
#include <set>
#include <chrono>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <mutex>
#include <unordered_map>
#include <algorithm>
#include <thread>
#include <atomic>

namespace nlm {

// Performance monitoring for neural networks
class PerformanceMonitor {
public:
    struct PerformanceMetrics {
        double cpuUsage;
        double memoryUsage;
        double diskUsage;
        std::chrono::system_clock::time_point timestamp;
        size_t activeThreads;
        size_t totalThreads;
        double networkUsage;
        size_t allocatedMemory;
        size_t peakMemory;
        double frameTime;
        double simulationSpeed;
    };
    
    PerformanceMonitor();
    ~PerformanceMonitor();
    
    // Start/stop monitoring
    bool startMonitoring(size_t intervalMs = 1000);
    void stopMonitoring();
    bool isMonitoring() const;
    
    // Get current metrics
    PerformanceMetrics getCurrentMetrics() const;
    
    // Historical data
    std::vector<PerformanceMetrics> getHistory(size_t maxEntries = 1000) const;
    std::vector<PerformanceMetrics> getTimeRange(
        const std::chrono::system_clock::time_point& start,
        const std::chrono::system_clock::time_point& end) const;
    
    // Performance analysis
    struct PerformanceAnalysis {
        double averageCpuUsage;
        double averageMemoryUsage;
        double averageDiskUsage;
        double cpuStddev;
        double memoryStddev;
        double peakMemory;
        double averageFrameTime;
        double simulationEfficiency;
        std::chrono::duration<double> totalMonitoringTime;
    };
    
    PerformanceAnalysis analyzePerformance() const;
    
    // Resource profiling
    struct ResourceProfile {
        size_t totalMemoryAllocated;
        size_t currentMemoryUsage;
        size_t peakMemoryUsage;
        size_t totalThreads;
        std::map<std::string, size_t> allocationsByType;
        std::vector<std::string> memoryLeaks;
    };
    
    ResourceProfile getResourceProfile() const;
    
    // Performance alerts
    struct PerformanceAlert {
        std::chrono::system_clock::time_point timestamp;
        std::string type; // "high_cpu", "high_memory", "low_cpu", "disk_full"
        double value;
        double threshold;
        std::string severity;
        std::string message;
    };
    
    std::vector<PerformanceAlert> getAlerts() const;
    
    // Performance tuning recommendations
    struct TuningRecommendation {
        std::string category;
        std::string description;
        double expectedImprovement;
        std::string implementationDifficulty;
        std::string estimatedTime;
    };
    
    std::vector<TuningRecommendation> getTuningRecommendations() const;
    
    // Visualization
    bool exportMetrics(const std::string& filepath) const;
    bool exportAnalysis(const std::string& filepath) const;
    
    // Real-time monitoring
    void enableRealTimeMonitoring(size_t updateIntervalMs = 500);
    bool isRealTimeMonitoringEnabled() const;
    
    // Custom metrics
    void addCustomMetric(const std::string& name, double value);
    std::map<std::string, std::vector<double>> getCustomMetrics() const;
    
    // Monitoring control
    void pauseMonitoring();
    void resumeMonitoring();
    void resetMetrics();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Neural network profiler
class NeuralProfiler {
public:
    NeuralProfiler();
    ~NeuralProfiler();
    
    // Profiling modes
    enum class ProfilingMode {
        None,
        Basic,
        Detailed,
        EventBased,
        Sampling
    };
    
    // Start/stop profiling
    void startProfiling(ProfilingMode mode = ProfilingMode::Detailed);
    void stopProfiling();
    bool isProfiling() const;
    
    // Profile neural operations
    void profileNeuron(const std::shared_ptr<Neuron>& neuron);
    void profileSynapse(const std::shared_ptr<Synapse>& synapse);
    void profileNetwork(const std::shared_ptr<Brain>& brain);
    
    // Profile simulation
    void profileSimulationStep(const std::shared_ptr<Brain>& brain, SimulationStep step);
    void profileTimeStep(const std::shared_ptr<Brain>& brain, TimestepDuration dt);
    
    // Profile plasticity
    void profilePlasticity(const std::string& neuronId, const std::string& synapseId, double weightChange);
    
    // Profile neuromodulation
    void profileNeuromodulation(const std::string& neuromodulatorId, double concentration);
    
    // Get profiling data
    struct NeuronProfile {
        std::string neuronId;
        std::chrono::duration<double> totalTime;
        size_t stepCount;
        double averageMembranePotential;
        double averageFiringRate;
        std::vector<double> membranePotentialHistory;
        std::vector<double> firingRateHistory;
    };
    
    std::vector<NeuronProfile> getNeuronProfiles() const;
    
    struct SynapseProfile {
        std::string synapseId;
        std::string preNeuronId;
        std::string postNeuronId;
        std::chrono::duration<double> totalTime;
        size_t transmissionCount;
        double averageWeight;
        double weightChangeTotal;
    };
    
    std::vector<SynapseProfile> getSynapseProfiles() const;
    
    struct SimulationProfile {
        std::chrono::duration<double> totalTime;
        size_t totalSteps;
        double averageStepTime;
        double stepsPerSecond;
        std::vector<double> stepTimes;
        std::vector<double> neuronCounts;
        std::vector<size_t> spikeCounts;
    };
    
    SimulationProfile getSimulationProfile() const;
    
    // Profile analysis
    struct ProfilingAnalysis {
        double totalProfilingTime;
        size_t totalNeuronsProfiled;
        size_t totalSynapsesProfiled;
        double averageNeuronProfileTime;
        double averageSynapseProfileTime;
        std::vector<std::string> bottlenecks;
        std::vector<std::string> optimizationOpportunities;
    };
    
    ProfilingAnalysis analyzeProfiling() const;
    
    // Profile visualization
    bool exportProfiles(const std::string& directory) const;
    bool generateProfileReport(const std::string& filepath) const;
    
    // Profile comparison
    struct ProfileComparison {
        std::string profile1Id;
        std::string profile2Id;
        double similarity;
        std::map<std::string, double> performanceDifferences;
        std::string recommendation;
    };
    
    ProfileComparison compareProfiles(const NeuralProfiler& other) const;
    
    // Profile filtering
    void filterProfiles(double minTime, double maxTime);
    void filterNeurons(const std::set<std::string>& neuronIds);
    void filterSynapses(const std::set<std::string>& synapseIds);
    
    // Profile reset
    void resetProfiles();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Memory profiler for neural networks
class MemoryProfiler {
public:
    MemoryProfiler();
    ~MemoryProfiler();
    
    // Memory tracking
    struct MemoryAllocation {
        std::string id;
        size_t size;
        std::string type;
        std::string allocator;
        std::chrono::system_clock::time_point timestamp;
        std::string context;
    };
    
    struct MemoryDeallocation {
        std::string id;
        std::chrono::system_clock::time_point timestamp;
        std::string context;
    };
    
    void trackAllocation(const std::string& id, size_t size, const std::string& type, 
                         const std::string& allocator, const std::string& context = "");
    void trackDeallocation(const std::string& id, const std::string& context = "");
    
    // Memory snapshot
    struct MemorySnapshot {
        std::chrono::system_clock::time_point timestamp;
        size_t totalAllocated;
        size_t currentAllocated;
        size_t peakAllocated;
        std::map<std::string, size_t> allocationsByType;
        std::vector<MemoryAllocation> liveAllocations;
        std::vector<MemoryDeallocation> deallocations;
    };
    
    MemorySnapshot takeSnapshot();
    std::vector<MemorySnapshot> getSnapshots() const;
    
    // Memory analysis
    struct MemoryAnalysis {
        size_t totalAllocations;
        size_t totalDeallocations;
        size_t currentLiveObjects;
        size_t peakLiveObjects;
        double memoryLeakProbability;
        std::vector<std::string> potentialMemoryLeaks;
        std::map<std::string, double> memoryUsageByType;
        std::chrono::duration<double> peakMemoryTime;
    };
    
    MemoryAnalysis analyzeMemory() const;
    
    // Memory validation
    struct MemoryValidationResult {
        bool isValid;
        std::vector<std::string> errors;
        std::vector<std::string> warnings;
        std::vector<std::string> suggestions;
    };
    
    MemoryValidationResult validateMemory() const;
    
    // Memory visualization
    bool exportSnapshots(const std::string& directory) const;
    bool generateMemoryReport(const std::string& filepath) const;
    
    // Memory comparison
    struct MemoryComparison {
        std::string snapshot1Id;
        std::string snapshot2Id;
        double memoryDifference;
        std::map<std::string, size_t> allocationDifferences;
        std::string recommendation;
    };
    
    MemoryComparison compareSnapshots(const MemorySnapshot& snapshot1, const MemorySnapshot& snapshot2) const;
    
    // Memory optimization
    void optimizeMemoryUsage() const;
    void compactMemory() const;
    
    // Memory control
    void enableMemoryTracking(bool enabled);
    bool isMemoryTrackingEnabled() const;
    
    void setMemoryLimit(size_t limit);
    size_t getMemoryLimit() const;
    
    void pauseMemoryTracking();
    void resumeMemoryTracking();
    
    void resetMemoryTracking();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// CPU profiler for neural networks
class CPUProfiler {
public:
    CPUProfiler();
    ~CPUProfiler();
    
    // CPU profiling
    struct CPUProfile {
        std::chrono::system_clock::time_point timestamp;
        double cpuUsage;
        size_t threadId;
        std::string threadName;
        std::chrono::duration<double> duration;
        double frequency;
        std::string context;
        std::vector<std::string> functionStack;
    };
    
    void startProfile(const std::string& context = "");
    void stopProfile(const std::string& context = "");
    void profileThread(const std::string& context = "");
    
    std::vector<CPUProfile> getProfiles() const;
    
    // CPU analysis
    struct CPUAnalysis {
        double averageCpuUsage;
        double peakCpuUsage;
        double totalCpuTime;
        size_t totalProfiles;
        std::map<size_t, double> cpuByThread;
        std::vector<std::string> bottlenecks;
        double cpuEfficiency;
    };
    
    CPUAnalysis analyzeCPU() const;
    
    // CPU validation
    struct CPUValidationResult {
        bool isValid;
        std::vector<std::string> warnings;
        std::vector<std::string> recommendations;
    };
    
    CPUValidationResult validateCPU() const;
    
    // CPU visualization
    bool exportProfiles(const std::string& directory) const;
    bool generateCPUReport(const std::string& filepath) const;
    
    // CPU comparison
    struct CPUComparison {
        std::string profile1Id;
        std::string profile2Id;
        double cpuDifference;
        std::string recommendation;
    };
    
    CPUComparison compareProfiles(const CPUProfile& profile1, const CPUProfile& profile2) const;
    
    // CPU optimization
    void optimizeCPUUsage() const;
    void balanceCPU() const;
    
    // CPU control
    void enableCPUProfiling(bool enabled);
    bool isCPUProfilingEnabled() const;
    
    void setCPULimit(double limit);
    double getCPULimit() const;
    
    void pauseCPUProfiling();
    void resumeCPUProfiling();
    
    void resetCPUProfiling();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Network profiler for neural networks
class NetworkProfiler {
public:
    NetworkProfiler();
    ~NetworkProfiler();
    
    // Network profiling
    struct NetworkProfile {
        std::chrono::system_clock::time_point timestamp;
        double uploadSpeed;
        double downloadSpeed;
        size_t packetsSent;
        size_t packetsReceived;
        size_t bytesSent;
        size_t bytesReceived;
        std::map<std::string, size_t> packetsByProtocol;
        std::string context;
    };
    
    void profileNetwork(const std::string& context = "");
    
    std::vector<NetworkProfile> getProfiles() const;
    
    // Network analysis
    struct NetworkAnalysis {
        double averageUploadSpeed;
        double averageDownloadSpeed;
        size_t totalPacketsSent;
        size_t totalPacketsReceived;
        size_t totalBytesSent;
        size_t totalBytesReceived;
        std::chrono::duration<double> totalNetworkTime;
        double networkEfficiency;
        std::vector<std::string> networkIssues;
    };
    
    NetworkAnalysis analyzeNetwork() const;
    
    // Network validation
    struct NetworkValidationResult {
        bool isValid;
        std::vector<std::string> issues;
        std::vector<std::string> recommendations;
    };
    
    NetworkValidationResult validateNetwork() const;
    
    // Network visualization
    bool exportProfiles(const std::string& directory) const;
    bool generateNetworkReport(const std::string& filepath) const;
    
    // Network comparison
    struct NetworkComparison {
        std::string profile1Id;
        std::string profile2Id;
        double speedDifference;
        double dataDifference;
        std::string recommendation;
    };
    
    NetworkComparison compareProfiles(const NetworkProfile& profile1, const NetworkProfile& profile2) const;
    
    // Network optimization
    void optimizeNetwork() const;
    void balanceNetwork() const;
    
    // Network control
    void enableNetworkProfiling(bool enabled);
    bool isNetworkProfilingEnabled() const;
    
    void setNetworkLimit(size_t limit);
    size_t getNetworkLimit() const;
    
    void pauseNetworkProfiling();
    void resumeNetworkProfiling();
    
    void resetNetworkProfiling();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Comprehensive profiler for neural networks
class ComprehensiveProfiler {
public:
    ComprehensiveProfiler();
    ~ComprehensiveProfiler();
    
    // Start/stop comprehensive profiling
    void startProfiling(const std::map<std::string, std::string>& config = {});
    void stopProfiling();
    bool isProfiling() const;
    
    // Profile all components
    void profileNetwork(const std::shared_ptr<Brain>& brain);
    void profileMemory();
    void profileCPU();
    void profileNetworkTraffic();
    void profileSystem();
    
    // Get comprehensive results
    struct ComprehensiveResults {
        PerformanceMonitor::PerformanceMetrics systemMetrics;
        NeuralProfiler::SimulationProfile neuralProfile;
        MemoryProfiler::MemoryAnalysis memoryAnalysis;
        CPUProfiler::CPUAnalysis cpuAnalysis;
        NetworkProfiler::NetworkAnalysis networkAnalysis;
        
        double overallPerformanceScore;
        std::vector<std::string> bottlenecks;
        std::vector<std::string> recommendations;
        std::chrono::duration<double> totalProfilingTime;
    };
    
    ComprehensiveResults getComprehensiveResults() const;
    
    // Results analysis
    struct ComprehensiveAnalysis {
        double averagePerformanceScore;
        size_t totalComponentsProfiled;
        std::vector<std::string> criticalIssues;
        std::vector<std::string> optimizationOpportunities;
        std::chrono::duration<double> totalAnalysisTime;
    };
    
    ComprehensiveAnalysis analyzeResults() const;
    
    // Results export
    bool exportResults(const std::string& directory) const;
    bool generateReport(const std::string& filepath) const;
    
    // Results comparison
    struct ResultsComparison {
        std::string results1Id;
        std::string results2Id;
        double performanceDifference;
        std::map<std::string, double> componentDifferences;
        std::string recommendation;
    };
    
    ResultsComparison compareResults(const ComprehensiveProfiler& other) const;
    
    // Results validation
    struct ResultsValidationResult {
        bool isValid;
        std::vector<std::string> errors;
        std::vector<std::string> warnings;
    };
    
    ResultsValidationResult validateResults() const;
    
    // Results optimization
    void optimizePerformance() const;
    void balanceResources() const;
    
    // Results control
    void pauseProfiling();
    void resumeProfiling();
    void resetResults();
    
    // Real-time monitoring
    void enableRealTimeAnalysis(bool enabled);
    bool isRealTimeAnalysisEnabled() const;
    
    // Configuration
    void setProfilingInterval(size_t intervalMs);
    void setSamplingRate(double rate);
    void enableDetailedProfiling(bool enabled);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Performance testing framework
class PerformanceTestFramework {
public:
    PerformanceTestFramework();
    ~PerformanceTestFramework();
    
    // Test definitions
    struct TestCase {
        std::string name;
        std::function<bool()> testFunction;
        std::chrono::duration<double> timeout;
        std::vector<std::string> requiredComponents;
        std::map<std::string, std::string> configuration;
        bool enabled;
    };
    
    struct TestResult {
        std::string testName;
        bool passed;
        std::chrono::duration<double> executionTime;
        std::string output;
        std::vector<std::string> errorMessages;
        std::chrono::system_clock::time_point timestamp;
        PerformanceMonitor::PerformanceMetrics metrics;
    };
    
    // Test management
    void addTestCase(const TestCase& testCase);
    void removeTestCase(const std::string& testName);
    void enableTestCase(const std::string& testName);
    void disableTestCase(const std::string& testName);
    
    // Test execution
    TestResult runTestCase(const std::string& testName);
    std::vector<TestResult> runAllTests();
    std::vector<TestResult> runTestsByCategory(const std::string& category);
    
    // Test suites
    struct TestSuite {
        std::string name;
        std::vector<TestCase> testCases;
        double targetPerformanceScore;
        std::chrono::duration<double> targetExecutionTime;
    };
    
    TestSuite createTestSuite(const std::string& name);
    void addTestCaseToSuite(const std::string& suiteName, const TestCase& testCase);
    TestResult runTestSuite(const std::string& suiteName);
    
    // Test analysis
    struct TestAnalysis {
        size_t totalTests;
        size_t passedTests;
        size_t failedTests;
        double successRate;
        std::chrono::duration<double> totalExecutionTime;
        std::chrono::duration<double> averageExecutionTime;
        PerformanceMonitor::PerformanceMetrics averageMetrics;
        std::vector<std::string> bottlenecks;
        std::string overallStatus;
    };
    
    TestAnalysis analyzeTestResults(const std::vector<TestResult>& results) const;
    
    // Test export
    bool exportTestResults(const std::string& filepath) const;
    bool importTestResults(const std::string& filepath);
    
    // Test comparison
    struct TestComparison {
        std::string test1Id;
        std::string test2Id;
        double performanceDifference;
        std::chrono::duration<double> timeDifference;
        std::string recommendation;
    };
    
    TestComparison compareTestResults(const TestResult& result1, const TestResult& result2) const;
    
    // Test validation
    struct TestValidationResult {
        bool isValid;
        std::vector<std::string> errors;
        std::vector<std::string> warnings;
    };
    
    TestValidationResult validateTestResults(const std::vector<TestResult>& results) const;
    
    // Test optimization
    void optimizeTestCases();
    void improveTestPerformance();
    
    // Test control
    void pauseTesting();
    void resumeTesting();
    void resetTestFramework();
    
    // Real-time monitoring
    void enableRealTimeTestMonitoring(bool enabled);
    bool isRealTimeTestMonitoringEnabled() const;
    
    // Configuration
    void setTestTimeout(const std::chrono::duration<double>& timeout);
    void setTestRetryCount(size_t retryCount);
    void enableTestScreenshots(bool enabled);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Performance regression detector
class PerformanceRegressionDetector {
public:
    PerformanceRegressionDetector();
    ~PerformanceRegressionDetector();
    
    // Baseline establishment
    void establishBaseline(const std::string& baselineName);
    bool hasBaseline(const std::string& baselineName) const;
    
    // Baseline comparison
    struct BaselineComparison {
        std::string currentRunId;
        std::string baselineId;
        double performanceRegression;
        std::chrono::duration<double> timeRegression;
        std::map<std::string, double> metricDifferences;
        std::string severity;
        std::string recommendation;
    };
    
    BaselineComparison compareWithBaseline(const std::string& baselineId);
    
    // Regression detection
    struct RegressionResult {
        std::chrono::system_clock::time_point timestamp;
        std::string runId;
        double performanceScore;
        std::chrono::duration<double> executionTime;
        std::map<std::string, double> metrics;
        bool isRegression;
        double regressionMagnitude;
        std::string regressionType;
    };
    
    RegressionResult detectRegression(const std::string& runId);
    std::vector<RegressionResult> getRegressionHistory() const;
    
    // Regression analysis
    struct RegressionAnalysis {
        size_t totalRuns;
        size_t regressionCount;
        double regressionRate;
        std::map<std::string, double> regressionByType;
        std::vector<std::string> regressionTrends;
        std::chrono::system_clock::time_point lastRegression;
    };
    
    RegressionAnalysis analyzeRegressions() const;
    
    // Regression prevention
    void setRegressionThreshold(double threshold);
    void enableRegressionDetection(bool enabled);
    void configureRegressionDetection(const std::map<std::string, std::string>& config);
    
    // Regression reporting
    bool exportRegressionReport(const std::string& filepath) const;
    bool generateRegressionAnalysis(const std::string& filepath) const;
    
    // Regression control
    void pauseRegressionDetection();
    void resumeRegressionDetection();
    void resetRegressionHistory();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm