#pragma once

#include <string>
#include <vector>
#include <memory>
#include "../benchmarks/Benchmark.hpp"

namespace nlm {
namespace benchmark {
namespace infrastructure {

// Flame graph utilities
namespace flamegraph {
    struct FlameGraphNode {
        std::string name;
        double selfTime;
        double totalTime;
        double percentage;
        std::vector<FlameGraphNode> children;
        std::string threadId;
        std::string fileName;
        int lineNumber;
    };
    
    FlameGraphNode buildFlameGraph(const std::vector<ProfileEvent>& events);
    void exportFlameGraph(const FlameGraphNode& root, const std::string& outputPath);
    void generateSVG(const FlameGraphNode& root, const std::string& outputPath);
    std::string generateHTMLFlameGraph(const FlameGraphNode& root);
}

// Memory usage tracking and analysis
class MemoryUsageTracker {
public:
    struct MemorySample {
        std::chrono::high_resolution_clock::time_point timestamp;
        size_t residentSet;      // Resident set size (RSS)
        size_t virtualMemory;    // Virtual memory size
        size_t sharedMemory;     // Shared memory
        size_t textSegment;      // Text segment (code)
        size_t dataSegment;      // Data segment
        size_t stack;            // Stack memory
        double cpuUtilization;   // CPU utilization during sample
    };
    
    void startTracking();
    void stopTracking();
    void recordSample();
    std::vector<MemorySample> getMemoryProfile() const;
    double calculatePeakMemoryUsage() const;
    size_t getCurrentMemoryUsage() const;
    double analyzeMemoryLeak(const std::vector<MemorySample>& samples) const;
    void generateMemoryReport(const std::string& outputPath);
    
private:
    std::vector<MemorySample> samples;
    bool tracking;
    std::thread trackingThread;
};

// CPU cache efficiency measurement
class CacheEfficiencyAnalyzer {
public:
    struct CacheMetrics {
        uint64_t L1CacheHits;
        uint64_t L1CacheMisses;
        uint64_t L2CacheHits;
        uint64_t L2CacheMisses;
        uint64_t L3CacheHits;
        uint64_t L3CacheMisses;
        uint64_t L1CacheLines;
        uint64_t L2CacheLines;
        uint64_t L3CacheLines;
        uint64_t totalInstructions;
        uint64_t totalBranches;
        uint64_t branchMisses;
        uint64_t memoryOperations;
        double IPC; // Instructions per cycle
        double L1MissRate;
        double L2MissRate;
        double L3MissRate;
        double branchMissRate;
        double cacheHitRatio;
        double cacheEfficiency;
    };
    
    CacheMetrics measureCacheEfficiency(const std::string& benchmarkName = "default");
    double calculateCacheEfficiency(const CacheMetrics& metrics);
    std::string analyzeCacheBottlenecks(const CacheMetrics& metrics);
    void generateCacheReport(const CacheMetrics& metrics, const std::string& outputPath);
    
    // Specific cache analysis functions
    double analyzeSpatialLocality(const std::vector<uint64_t>& addresses);
    double analyzeTemporalLocality(const std::vector<uint64_t>& accessTimestamps);
    double analyzeWorkingSetSize(const std::vector<uint64_t>& addresses);
    double analyzeMemoryLevelDistribution(const std::vector<uint64_t>& addresses);
};

// Statistical analysis utilities (enhanced)
class AdvancedStatistics {
public:
    struct StatisticalSummary {
        double mean;
        double median;
        double mode;
        double variance;
        double standardDeviation;
        double skewness;
        double kurtosis;
        double coefficientOfVariation;
        double confidenceInterval95;
        std::vector<double> percentiles; // 25th, 50th, 75th, 90th, 95th, 99th
        size_t sampleSize;
        double minValue;
        double maxValue;
        double range;
        double iqr; // Interquartile range
    };
    
    static StatisticalSummary computeSummary(const std::vector<double>& data);
    static std::vector<double> bootstrap(const std::vector<double>& data, size_t iterations = 10000);
    static double bootstrapConfidenceInterval(const std::vector<double>& data, double confidence = 0.95);
    static double coefficientOfVariation(const std::vector<double>& data);
    static double skewness(const std::vector<double>& data);
    static double kurtosis(const std::vector<double>& data);
    static std::vector<double> calculatePercentiles(const std::vector<double>& data, const std::vector<double>& percentiles);
    static std::pair<double, double> linearRegression(const std::vector<double>& x, const std::vector<double>& y);
    static double rSquared(const std::vector<double>& x, const std::vector<double>& y, const std::pair<double, double>& regression);
    static std::vector<double> movingAverage(const std::vector<double>& data, size_t windowSize);
    static std::vector<double> exponentialMovingAverage(const std::vector<double>& data, double alpha);
    static double analyzeTimeSeriesTrend(const std::vector<double>& data);
    static bool isOutlier(double value, const std::vector<double>& data, double zThreshold = 3.0);
    static std::vector<size_t> findOutliers(const std::vector<double>& data, double zThreshold = 3.0);
    static double entropy(const std::vector<size_t>& frequencies);
    static double mutualInformation(const std::vector<size_t>& x, const std::vector<size_t>& y);
    static std::vector<double> normalize(const std::vector<double>& data);
    static std::vector<double> standardize(const std::vector<double>& data);
    static double mahalanobisDistance(const std::vector<double>& point, const std::vector<std::vector<double>>& data);
};

// Performance profiling with flame graphs
class FlameGraphProfiler {
public:
    struct ProfileEvent {
        std::string functionName;
        std::string sourceFile;
        int lineNumber;
        std::chrono::high_resolution_clock::time_point startTime;
        std::chrono::high_resolution_clock::time_point endTime;
        std::string threadId;
        uint64_t cpuInstructions;
        uint64_t cacheMisses;
        uint64_t branchMispredictions;
        uint64_t memoryOperations;
        std::string eventType; // "enter", "exit", "sample"
    };
    
    void startProfile(const std::string& functionName, const std::string& sourceFile = "", int lineNumber = 0);
    void endProfile(const std::string& functionName);
    void sampleProfile(const std::string& threadId = "main");
    void setSamplingInterval(double intervalSeconds);
    std::vector<ProfileEvent> getProfileEvents() const;
    flamegraph::FlameGraphNode buildFlameGraph() const;
    void exportFlameGraph(const std::string& outputPath);
    void generateSVGFlameGraph(const std::string& outputPath);
    void generateHTMLReport(const std::string& outputPath);
    void clearProfile();
    
private:
    std::vector<ProfileEvent> events;
    std::mutex mutex;
    double samplingInterval;
    std::thread samplingThread;
    bool isSampling;
    
    void samplingWorker(const std::string& threadId);
};

// Performance regression detection
class RegressionDetector {
public:
    struct RegressionResult {
        std::string metricName;
        double baselineMean;
        double baselineStddev;
        double currentMean;
        double currentStddev;
        double relativeChange;
        double pValue;
        bool isRegression;
        double confidence;
        std::string severity; // "critical", "warning", "info"
        std::string recommendation;
    };
    
    void setBaseline(const std::vector<BenchmarkMetrics>& baselineMetrics);
    std::vector<RegressionResult> detectRegression(const std::vector<BenchmarkMetrics>& currentMetrics);
    bool isSignificantChange(double baselineMean, double baselineStddev, double currentMean, double currentStddev, double alpha = 0.05);
    double calculatePValue(const std::vector<double>& baseline, const std::vector<double>& current);
    std::string severityFromPValue(double pValue);
    std::string generateRegressionReport(const std::vector<RegressionResult>& results);
    void saveRegressionResults(const std::vector<RegressionResult>& results, const std::string& outputPath);
    
private:
    std::vector<BenchmarkMetrics> baselineMetrics;
    std::mutex mutex;
};

// Automated benchmarking orchestration
class BenchmarkOrchestrator {
public:
    struct BenchmarkConfig {
        std::string name;
        std::string description;
        std::vector<std::string> dependencies; // Other benchmark names that must run first
        std::map<std::string, std::string> parameters;
        bool enabled;
        double timeout; // seconds
        int iterations; // Number of times to run
        double warmUpTime; // seconds
        bool collectFlameGraph;
        bool collectMemoryProfile;
        bool collectCacheMetrics;
        bool generateReports;
        bool generateCharts;
        std::string outputFormat;
        std::string outputDirectory;
    };
    
    BenchmarkOrchestrator(const std::string& outputDir, const std::string& resultsFile);
    
    void addBenchmark(const BenchmarkConfig& config);
    void removeBenchmark(const std::string& name);
    void runBenchmark(const std::string& name);
    void runAllBenchmarks();
    void setBaseline(const std::string& name);
    void updateBaseline(const std::string& name);
    std::vector<BenchmarkMetrics> getResults() const;
    std::vector<BenchmarkMetrics> getResults(const std::string& benchmarkName) const;
    void saveResults(const std::string& format = "json");
    void generateComprehensiveReport();
    void exportAllResults();
    void exportForCI();
    
    // CI/CD integration
    bool runCIIntegration();
    bool detectPerformanceRegression();
    std::string generateCIStatusReport();
    void setCITimeout(double timeout);
    
private:
    std::vector<BenchmarkConfig> benchmarkConfigs;
    std::vector<BenchmarkMetrics> allResults;
    std::string outputDirectory;
    std::string resultsFile;
    std::map<std::string, std::vector<BenchmarkMetrics>> baselineResults;
    bool ciMode;
    double ciTimeout;
    
    void runBenchmarkSequence();
    void executeBenchmark(const BenchmarkConfig& config);
    void validateDependencies(const BenchmarkConfig& config);
    std::vector<BenchmarkConfig> getBenchmarksToRun() const;
    bool isBenchmarkEnabled(const BenchmarkConfig& config) const;
};

// Performance metrics exporter
class MetricsExporter {
public:
    enum ExportFormat {
        JSON,
        CSV,
        Markdown,
        HTML,
        YAML,
        XML
    };
    
    static void exportMetrics(const std::vector<BenchmarkMetrics>& metrics, const std::string& outputPath, ExportFormat format = JSON);
    static void exportComparison(const std::vector<BenchmarkMetrics>& baseline, const std::vector<BenchmarkMetrics>& current, const std::string& outputPath, ExportFormat format = JSON);
    static void exportScalabilityResults(const std::vector<ScalabilityTester::ScalabilityResult>& results, const std::string& outputPath);
    static void exportRegressionResults(const std::vector<RegressionDetector::RegressionResult>& results, const std::string& outputPath);
    static void exportCIResults(const std::vector<BenchmarkMetrics>& metrics, const std::string& outputPath);
    static std::string metricsToJSON(const std::vector<BenchmarkMetrics>& metrics);
    static std::string metricsToCSV(const std::vector<BenchmarkMetrics>& metrics);
    static std::string metricsToMarkdown(const std::vector<BenchmarkMetrics>& metrics);
    static std::string metricsToHTML(const std::vector<BenchmarkMetrics>& metrics);
    static std::string metricsToYAML(const std::vector<BenchmarkMetrics>& metrics);
    static std::string metricsToXML(const std::vector<BenchmarkMetrics>& metrics);
    
    static ExportFormat parseFormat(const std::string& formatString);
    static std::string formatToString(ExportFormat format);
    static std::string getDefaultOutputPath(const std::string& benchmarkName, ExportFormat format);
};

// Advanced metrics calculator
class AdvancedMetricsCalculator {
public:
    struct AdvancedMetrics {
        double opsPerSecond;           // Operations per second
        double flopsPerSecond;         // Floating-point operations per second
        double memoryBandwidth;        // Memory bandwidth (GB/s)
        double cacheHitMissRatio;       // Cache hit/miss ratio
        double threadSynchronizationOverhead; // Thread synchronization overhead (seconds)
        double instructionPerCycle;    // Instructions per cycle
        double branchPredictionRate;   // Branch prediction success rate
        double SIMDUtilization;        // SIMD utilization efficiency
        double memoryLevelEfficiency;  // Memory hierarchy efficiency
        double computationalIntensity; // Computational intensity (FLOPs/byte)
        double arithmeticIntensity;    // Arithmetic intensity (FLOPs/operation)
        double energyEfficiency;       // Energy efficiency (operations/Joule)
        double scalabilityScore;       // Overall scalability score
        double performanceScore;       // Overall performance score
    };
    
    static AdvancedMetrics calculateAdvancedMetrics(const BenchmarkMetrics& metrics);
    static double calculateOPS(const BenchmarkMetrics& metrics);
    static double calculateFLOPsPerSecond(const BenchmarkMetrics& metrics);
    static double calculateMemoryBandwidth(const BenchmarkMetrics& metrics);
    static double calculateCacheHitMissRatio(const BenchmarkMetrics& metrics);
    static double calculateThreadSynchronizationOverhead(const BenchmarkMetrics& metrics);
    static double calculateInstructionPerCycle(const BenchmarkMetrics& metrics);
    static double calculateBranchPredictionRate(const BenchmarkMetrics& metrics);
    static double calculateSIMDUtilization(const BenchmarkMetrics& metrics);
    static double calculateMemoryLevelEfficiency(const BenchmarkMetrics& metrics);
    static double calculateComputationalIntensity(const BenchmarkMetrics& metrics);
    static double calculateArithmeticIntensity(const BenchmarkMetrics& metrics);
    static double calculateEnergyEfficiency(const BenchmarkMetrics& metrics, double energyConsumption);
    static double calculateScalabilityScore(const std::vector<BenchmarkMetrics>& metrics);
    static double calculatePerformanceScore(const BenchmarkMetrics& metrics);
    static std::string rankPerformance(const std::vector<BenchmarkMetrics>& metrics);
    static void generateAdvancedMetricsReport(const AdvancedMetrics& metrics, const std::string& outputPath);
};

} // namespace infrastructure
} // namespace benchmark
} // namespace nlm

#include "InfrastructureBenchmarks.inl"