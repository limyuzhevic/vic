#pragma once

#include <string>
#include <vector>
#include <memory>
#include "../benchmarks/Benchmark.hpp"

namespace nlm {
namespace benchmark {
namespace scalability {

// Scalability tester implementation
class ScalabilityTester {
public:
    struct ScalabilityResult {
        std::string testName;
        double neurons;
        double regions;
        double connectionDensity;
        double timePerStep;
        double memoryPerNeuron;
        double scalabilityFactor;
        double confidenceInterval;
    };
    
    std::vector<ScalabilityResult> testNeuronScalability(const std::vector<size_t>& neuronCounts);
    std::vector<ScalabilityResult> testRegionScalability(const std::vector<size_t>& regionCounts);
    std::vector<ScalabilityResult> testConnectionDensityScalability(const std::vector<double>& densities);
    double calculateTimeComplexity(const std::vector<size_t>& neuronCounts, const std::vector<double>& times);
    double calculateSpaceComplexity(const std::vector<size_t>& neuronCounts, const std::vector<double>& memoryUsage);
    std::string analyzeScalabilityResults(const std::vector<ScalabilityResult>& results);
    void generateScalabilityReport(const std::vector<ScalabilityResult>& results, const std::string& outputPath);
};

// Comparative benchmarking implementation
class ComparativeBenchmarker {
public:
    struct ComparisonResult {
        std::string testName;
        std::string referenceImplementation;
        std::string currentImplementation;
        double speedup;
        double memoryImprovement;
        double qualityScore;
        double regressionRisk;
        double confidence;
        std::string severity;
        std::string recommendation;
    };
    
    ComparativeBenchmarker(const std::string& referenceDirectory);
    void setReferenceImplementation(const std::string& path);
    std::vector<ComparisonResult> compareWithReference(const std::string& testName, const BenchmarkMetrics& currentMetrics);
    double calculateRegressionRisk(const std::vector<BenchmarkMetrics>& metrics);
    double calculateQualityScore(const BenchmarkMetrics& metrics);
    std::string generateComparisonReport(const std::vector<ComparisonResult>& results);
    void saveComparisonResults(const std::vector<ComparisonResult>& results, const std::string& outputPath);
    
private:
    std::string referenceDirectory;
    std::map<std::string, std::vector<BenchmarkMetrics>> referenceMetrics;
};

// Performance regression testing implementation
class PerformanceRegressionTester {
public:
    struct RegressionTestResult {
        std::string testName;
        bool passed;
        double threshold;
        double actualValue;
        double difference;
        double percentChange;
        double pValue;
        bool isStatisticallySignificant;
        std::string recommendation;
        std::string severity;
    };
    
    PerformanceRegressionTester(double alpha = 0.05, double performanceThreshold = 0.95);
    void setBaseline(const std::vector<BenchmarkMetrics>& baseline);
    std::vector<RegressionTestResult> runRegressionTests(const std::vector<BenchmarkMetrics>& currentMetrics);
    bool isAcceptableChange(double baseline, double current, double threshold);
    double calculateEffectSize(const std::vector<double>& baseline, const std::vector<double>& current);
    double calculatePValue(const std::vector<double>& baseline, const std::vector<double>& current);
    std::string generateRegressionReport(const std::vector<RegressionTestResult>& results);
    void saveRegressionResults(const std::vector<RegressionTestResult>& results, const std::string& outputPath);
    
private:
    double significanceLevel;
    double performanceThreshold;
    std::vector<BenchmarkMetrics> baselineMetrics;
};

// Cross-architecture benchmarking implementation
class CrossArchitectureBenchmarker {
public:
    struct ArchitectureResult {
        std::string architecture;
        std::string cpuInfo;
        std::string gpuInfo;
        std::string memoryInfo;
        BenchmarkMetrics metrics;
        double relativePerformance;
        double efficiencyScore;
    };
    
    void discoverArchitectures();
    std::vector<ArchitectureResult> benchmarkArchitecture(const std::string& architectureName);
    double calculateRelativePerformance(const BenchmarkMetrics& baseline, const BenchmarkMetrics& current);
    double calculateEfficiencyScore(const BenchmarkMetrics& metrics);
    std::string getCPUInfo();
    std::string getGPUInfo();
    std::string getMemoryInfo();
    std::string generateCrossArchitectureReport(const std::vector<ArchitectureResult>& results);
};

// Hardware-specific optimizations validation implementation
class HardwareOptimizationValidator {
public:
    struct OptimizationValidationResult {
        std::string optimizationName;
        std::string hardwareFeature;
        bool isEnabled;
        BenchmarkMetrics metricsWithOptimization;
        BenchmarkMetrics metricsWithoutOptimization;
        double improvementFactor;
        bool passesValidation;
        std::string validationCriteria;
        std::string recommendation;
    };
    
    HardwareOptimizationValidator();
    void discoverHardwareFeatures();
    std::vector<OptimizationValidationResult> validateSIMDOptimizations();
    std::vector<OptimizationValidationResult> validateMemoryPoolOptimizations();
    std::vector<OptimizationValidationResult> validateThreadingOptimizations();
    std::vector<OptimizationValidationResult> validateCacheAwareOptimizations();
    std::vector<OptimizationValidationResult> validateVectorizationOptimizations();
    bool testHardwareFeature(const std::string& featureName);
    BenchmarkMetrics benchmarkWithFeature(const std::string& featureName, const BenchmarkMetrics& baseline);
    std::string generateOptimizationValidationReport(const std::vector<OptimizationValidationResult>& results);
    
private:
    std::vector<std::string> availableFeatures;
};

// Automated benchmarking orchestration implementation
class AutomatedBenchmarkOrchestrator {
public:
    struct BenchmarkSchedule {
        std::string name;
        std::string cronExpression; // for periodic runs
        std::vector<std::string> benchmarks; // which benchmarks to run
        std::string outputPath;
        bool enabled;
        int maxHistory; // number of historical results to keep
    };
    
    AutomatedBenchmarkOrchestrator(const std::string& configPath);
    void loadConfiguration(const std::string& configPath);
    void saveConfiguration(const std::string& configPath);
    void addSchedule(const BenchmarkSchedule& schedule);
    void removeSchedule(const std::string& name);
    void runScheduledBenchmarks();
    void runAllBenchmarks();
    void scheduleBenchmark(const std::string& name, const std::string& cronExpression);
    void stopScheduling();
    std::vector<BenchmarkMetrics> getHistoricalResults(const std::string& benchmarkName, size_t limit = 100);
    std::vector<BenchmarkMetrics> getRecentResults(const std::string& benchmarkName, double timeWindowHours = 24.0);
    void exportHistoricalData(const std::string& outputPath);
    std::string generateTrendReport(const std::string& benchmarkName, size_t dataPoints = 50);
    
private:
    std::vector<BenchmarkSchedule> schedules;
    std::map<std::string, std::vector<BenchmarkMetrics>> historicalResults;
    bool isRunning;
    std::thread schedulerThread;
    
    void schedulerWorker();
};

// CI/CD integration for performance regression detection
class CIPerformanceIntegration {
public:
    struct CIStatus {
        bool passed;
        std::string status; // "passed", "failed", "warning", "skipped"
        std::string message;
        std::vector<std::string> regressionTests;
        std::vector<std::string> performanceTests;
        double overallScore;
        std::chrono::high_resolution_clock::time_point timestamp;
    };
    
    CIPerformanceIntegration(const std::string& repositoryPath, const std::string& resultsPath);
    void initializeCI();
    CIStatus runPerformanceCI();
    bool detectPerformanceRegression();
    std::string generateCIStatusReport(const CIStatus& status);
    void updateGitHubStatus(const CIStatus& status);
    void createPullRequestComment(const CIStatus& status);
    void setEnvironmentVariables();
    std::string getEnvironmentVariable(const std::string& name);
    void setPerformanceThresholds();
    std::vector<std::string> getRegressionTests();
    std::vector<std::string> getPerformanceTests();
    std::string generateFailureMessage(const CIStatus& status);
    std::string generateSuccessMessage(const CIStatus& status);
    
private:
    std::string repositoryPath;
    std::string resultsPath;
    std::map<std::string, double> performanceThresholds;
};

} // namespace scalability
} // namespace benchmark
} // namespace nlm

#include "ScalabilityBenchmarks.inl"