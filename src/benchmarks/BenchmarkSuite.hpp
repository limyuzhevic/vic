#pragma once

#include <string>
#include <vector>
#include <memory>
#include <map>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <chrono>
#include <thread>
#include <mutex>
#include <algorithm>
#include <numeric>
#include <cmath>
#include <random>
#include <filesystem>

// Core benchmarking infrastructure
#include "benchmarks/Benchmark.hpp"
#include "benchmarks/core/CoreBenchmarks.hpp"
#include "benchmarks/infrastructure/InfrastructureBenchmarks.hpp"
#include "benchmarks/scalability/ScalabilityBenchmarks.hpp"

namespace nlm {
namespace benchmark {

// Comprehensive benchmarking suite
class ComprehensiveBenchmarkSuite {
public:
    struct BenchmarkConfiguration {
        std::string name;
        std::string description;
        std::map<std::string, std::string> parameters;
        bool enabled;
        double timeout; // seconds
        int iterations;
        bool generateDetailedReport;
        bool generateCharts;
        bool collectProfiles;
        std::string outputDirectory;
    };
    
    ComprehensiveBenchmarkSuite(const std::string& outputDirectory = "benchmark_results");
    ~ComprehensiveBenchmarkSuite();
    
    // Benchmark suite management
    void addBenchmarkSuite(const BenchmarkConfiguration& config);
    void removeBenchmarkSuite(const std::string& name);
    void enableBenchmarkSuite(const std::string& name);
    void disableBenchmarkSuite(const std::string& name);
    void runAllBenchmarkSuites();
    void runBenchmarkSuite(const std::string& name);
    
    // Result management
    std::vector<BenchmarkMetrics> getResults() const;
    std::vector<BenchmarkMetrics> getResults(const std::string& benchmarkName) const;
    void saveAllResults(const std::string& format = "json");
    void generateAllReports();
    void exportForCI();
    
    // Core benchmarking suites
    void runCoreBenchmarks();
    void runScalabilityBenchmarks();
    void runComparativeBenchmarks();
    void runAdvancedMetricsBenchmarks();
    void runIntegrationBenchmarks();
    
    // Specialized benchmark suites
    void runBrainBenchmarks(const std::vector<size_t>& neuronCounts);
    void runMemoryBenchmarks(const std::vector<size_t>& objectCounts);
    void runSIMDBenchmarks(const std::vector<float>& testData);
    void runIntegrationBenchmarks(const std::vector<size_t>& networkSizes);
    
    // Performance analysis
    std::string analyzePerformance(const std::vector<BenchmarkMetrics>& metrics);
    std::string identifyBottlenecks(const std::vector<BenchmarkMetrics>& metrics);
    std::string generateRecommendations(const std::vector<BenchmarkMetrics>& metrics);
    
    // Configuration
    void setOutputDirectory(const std::string& directory);
    void setPerformanceThresholds(const std::map<std::string, double>& thresholds);
    void setCIEnabled(bool enabled);
    void setCIBaseline(const std::string& baselineFile);
    void updateCIBaseline();
    
    // Automation
    void scheduleBenchmark(const std::string& suiteName, const std::string& cronExpression);
    void stopScheduling();
    std::vector<std::string> getScheduledBenchmarks() const;
    
    // Results export
    void exportResultsToCSV(const std::string& path);
    void exportResultsToJSON(const std::string& path);
    void exportResultsToMarkdown(const std::string& path);
    void exportResultsToHTML(const std::string& path);
    void exportResultsToYAML(const std::string& path);
    void exportResultsToXML(const std::string& path);
    
    // CI/CD integration
    bool runCIIntegration();
    bool detectPerformanceRegression();
    std::string generateCIStatusReport();
    void updateGitHubStatus(const std::string& status, const std::string& description);
    
    // Advanced metrics
    BenchmarkMetrics calculateAdvancedMetrics(const std::vector<BenchmarkMetrics>& metrics);
    std::string rankBenchmarks(const std::vector<BenchmarkMetrics>& metrics);
    std::string generatePerformanceScorecard(const std::vector<BenchmarkMetrics>& metrics);
    
    // Utility functions
    static std::string generateBenchmarkID(const std::string& suiteName, const std::map<std::string, std::string>& parameters);
    static std::string formatDuration(double seconds);
    static std::string formatBytes(size_t bytes);
    static std::string formatPercentage(double value);
    static std::string formatOPS(double opsPerSecond);
    static std::string formatFLOPsPerSecond(double flopsPerSecond);
    static std::string generateTimestamp();
    static std::string sanitizeFilename(const std::string& filename);
    
    // Flame graph and profiling
    void generateFlameGraphs(const std::string& outputPath);
    void generateMemoryProfiles(const std::string& outputPath);
    void generateCacheAnalysis(const std::string& outputPath);
    void generateScalabilityAnalysis(const std::string& outputPath);
    
    // Statistics and analysis
    static std::string calculateStatistics(const std::vector<BenchmarkMetrics>& metrics);
    static std::string calculateTrends(const std::vector<BenchmarkMetrics>& metrics, size_t dataPoints = 10);
    static std::string identifyOutliers(const std::vector<BenchmarkMetrics>& metrics);
    static std::vector<BenchmarkMetrics> filterResults(const std::vector<BenchmarkMetrics>& metrics, const std::string& filter);
    
    // Configuration file management
    bool loadConfiguration(const std::string& configPath);
    bool saveConfiguration(const std::string& configPath);
    bool validateConfiguration();
    
    // Performance regression detection
    bool isPerformanceRegression(const BenchmarkMetrics& baseline, const BenchmarkMetrics& current);
    double calculatePerformanceDifference(const BenchmarkMetrics& baseline, const BenchmarkMetrics& current);
    std::string getRegressionSeverity(double difference);
    std::string getRegressionRecommendation(const BenchmarkMetrics& baseline, const BenchmarkMetrics& current);
    
    // Benchmarking suites
    std::vector<BenchmarkConfiguration> getBenchmarkConfigurations() const;
    bool isBenchmarkSuiteEnabled(const std::string& name) const;
    
private:
    std::string outputDirectory;
    std::vector<BenchmarkConfiguration> benchmarkSuites;
    std::vector<BenchmarkMetrics> allResults;
    std::map<std::string, std::vector<BenchmarkMetrics>> suiteResults;
    std::map<std::string, double> performanceThresholds;
    bool ciEnabled;
    std::string ciBaselineFile;
    bool isScheduling;
    std::thread schedulerThread;
    std::mutex mutex;
    
    void initializeDefaultSuites();
    void validateAndRunSuite(const BenchmarkConfiguration& config);
    void executeBenchmarkSuite(const BenchmarkConfiguration& config);
    void generateSuiteReport(const BenchmarkConfiguration& config, const std::vector<BenchmarkMetrics>& results);
    void scheduleWorker();
    void cleanup();
};

// Benchmark configuration management
class BenchmarkConfigManager {
public:
    struct BenchmarkConfig {
        std::string name;
        std::string description;
        std::map<std::string, std::string> parameters;
        bool enabled;
        double timeout;
        int iterations;
        bool generateReports;
        bool generateCharts;
        bool collectProfiles;
        std::string outputFormat;
    };
    
    BenchmarkConfigManager(const std::string& configFile = "benchmark_config.json");
    bool loadConfig(const std::string& configPath);
    bool saveConfig(const std::string& configPath);
    bool addBenchmarkConfig(const BenchmarkConfig& config);
    bool removeBenchmarkConfig(const std::string& name);
    bool updateBenchmarkConfig(const BenchmarkConfig& config);
    std::vector<BenchmarkConfig> getAllBenchmarkConfigs() const;
    BenchmarkConfig getBenchmarkConfig(const std::string& name) const;
    bool isBenchmarkConfigEnabled(const std::string& name) const;
    void enableBenchmarkConfig(const std::string& name);
    void disableBenchmarkConfig(const std::string& name);
    std::vector<BenchmarkConfig> getEnabledBenchmarkConfigs() const;
    
    // Configuration validation
    bool validateBenchmarkConfig(const BenchmarkConfig& config);
    std::string getValidationErrors(const BenchmarkConfig& config);
    
    // Default configurations
    static BenchmarkConfig getCoreBenchmarkConfig();
    static BenchmarkConfig getScalabilityBenchmarkConfig();
    static BenchmarkConfig getComparativeBenchmarkConfig();
    static BenchmarkConfig getAdvancedMetricsConfig();
    static BenchmarkConfig getIntegrationBenchmarkConfig();
    
private:
    std::string configFile;
    std::vector<BenchmarkConfig> benchmarkConfigs;
    std::mutex mutex;
    
    static std::string generateConfigFilename(const std::string& baseName);
    bool loadFromJSON(const std::string& configPath);
    bool saveToJSON(const std::string& configPath);
    bool loadFromYAML(const std::string& configPath);
    bool saveToYAML(const std::string& configPath);
    bool loadFromXML(const std::string& configPath);
    bool saveToXML(const std::string& configPath);
};

// Result aggregation and analysis
class BenchmarkResultAggregator {
public:
    struct AggregatedResults {
        std::string benchmarkName;
        size_t totalRuns;
        double totalTime;
        double averageTime;
        double minTime;
        double maxTime;
        double timeStandardDeviation;
        double timeCoefficientOfVariation;
        
        double totalOperations;
        double averageOperationsPerSecond;
        double operationsPerSecondStandardDeviation;
        
        size_t totalMemoryUsage;
        size_t peakMemoryUsage;
        double averageMemoryUsage;
        
        double averageCacheHitRatio;
        double cacheHitRatioStandardDeviation;
        
        double averageSIMDUtilization;
        double simdUtilizationStandardDeviation;
        
        double averageBranchPredictionRate;
        double branchPredictionRateStandardDeviation;
        
        std::vector<double> timeSeries;
        std::vector<double> opsPerSecondSeries;
        std::vector<size_t> memoryUsageSeries;
        
        double calculateTrend() const;
        double calculateVolatility() const;
        bool isImproving() const;
        std::string getPerformanceGrade() const;
    };
    
    BenchmarkResultAggregator();
    void addResults(const std::vector<BenchmarkMetrics>& metrics);
    void addResult(const BenchmarkMetrics& metric);
    AggregatedResults aggregate(const std::string& benchmarkName) const;
    std::vector<AggregatedResults> aggregateAll() const;
    std::vector<AggregatedResults> getTopBenchmarks(size_t count = 10) const;
    std::vector<AggregatedResults> getBenchmarksByPerformance(const std::string& metric = "opsPerSecond", size_t count = 10) const;
    std::vector<BenchmarkMetrics> filterResults(const std::string& benchmarkName, const std::string& filter) const;
    std::string generateDetailedReport(const std::vector<AggregatedResults>& results) const;
    void saveAggregatedResults(const std::vector<AggregatedResults>& results, const std::string& outputPath);
    void generateVisualizations(const std::vector<AggregatedResults>& results, const std::string& outputPath);
    
private:
    std::vector<BenchmarkMetrics> allResults;
    mutable std::mutex mutex;
    
    double calculateMovingAverage(const std::vector<double>& values, size_t windowSize) const;
    double calculateLinearTrend(const std::vector<double>& values) const;
    double calculateVolatility(const std::vector<double>& values) const;
    std::string getGradeFromScore(double score) const;
};

// Performance regression detector
class PerformanceRegressionDetector {
public:
    struct RegressionDetection {
        std::string testName;
        bool hasRegression;
        double regressionRisk;
        double performanceImpact;
        double confidenceLevel;
        std::string severity;
        std::vector<std::string> affectedComponents;
        std::string recommendation;
        std::chrono::high_resolution_clock::time_point detectionTime;
    };
    
    PerformanceRegressionDetector(double alpha = 0.05, double performanceThreshold = 0.95);
    void setBaseline(const std::vector<BenchmarkMetrics>& baseline);
    std::vector<RegressionDetection> detectRegression(const std::vector<BenchmarkMetrics>& currentMetrics);
    std::vector<RegressionDetection> detectRegression(const std::string& benchmarkName, const std::vector<BenchmarkMetrics>& currentMetrics);
    bool isRegression(const BenchmarkMetrics& baseline, const BenchmarkMetrics& current);
    double calculateRegressionRisk(const BenchmarkMetrics& baseline, const BenchmarkMetrics& current);
    double calculatePerformanceImpact(const BenchmarkMetrics& baseline, const BenchmarkMetrics& current);
    std::string getSeverityFromRisk(double risk);
    std::string getRecommendation(const BenchmarkMetrics& baseline, const BenchmarkMetrics& current);
    std::string generateRegressionReport(const std::vector<RegressionDetection>& regressions);
    void saveRegressionDetections(const std::vector<RegressionDetection>& regressions, const std::string& outputPath);
    void generateCIStatus(const std::vector<RegressionDetection>& regressions);
    
private:
    double significanceLevel;
    double performanceThreshold;
    std::vector<BenchmarkMetrics> baselineMetrics;
    std::mutex mutex;
    
    double calculateEffectSize(const BenchmarkMetrics& baseline, const BenchmarkMetrics& current);
    bool isStatisticallySignificant(double pValue);
};

// Benchmark report generator
class BenchmarkReportGenerator {
public:
    struct ReportOptions {
        std::string title;
        std::string description;
        std::string generationTime;
        bool includeRawData;
        bool includeDetailedMetrics;
        bool includeStatisticalAnalysis;
        bool includeTrendAnalysis;
        bool includeBenchmarkComparisons;
        bool includeScalabilityAnalysis;
        bool includeRegressionAnalysis;
        bool includeRecommendations;
        std::vector<std::string> customSections;
        std::map<std::string, std::string> customColors;
    };
    
    BenchmarkReportGenerator();
    std::string generateComprehensiveReport(const std::vector<BenchmarkMetrics>& metrics);
    std::string generateExecutiveSummary(const std::vector<BenchmarkMetrics>& metrics);
    std::string generateDetailedAnalysis(const std::vector<BenchmarkMetrics>& metrics);
    std::string generateScalabilityReport(const std::vector<BenchmarkMetrics>& metrics);
    std::string generateComparisonReport(const std::vector<BenchmarkMetrics>& metrics);
    std::string generateRegressionReport(const std::vector<BenchmarkMetrics>& metrics);
    std::string generateRecommendations(const std::vector<BenchmarkMetrics>& metrics);
    std::string generateHTMLReport(const std::vector<BenchmarkMetrics>& metrics);
    std::string generateMarkdownReport(const std::vector<BenchmarkMetrics>& metrics);
    std::string generateJSONReport(const std::vector<BenchmarkMetrics>& metrics);
    std::string generateYAMLReport(const std::vector<BenchmarkMetrics>& metrics);
    void saveReport(const std::string& content, const std::string& path);
    
private:
    ReportOptions defaultOptions;
    
    std::string generateReportHeader(const ReportOptions& options) const;
    std::string generateReportFooter(const ReportOptions& options) const;
    std::string generateTable(const std::vector<BenchmarkMetrics>& metrics, const std::string& title) const;
    std::string generateMetricChart(const std::vector<BenchmarkMetrics>& metrics, const std::string& metricName) const;
    std::string generateComparisonChart(const std::vector<BenchmarkMetrics>& metrics) const;
    std::string generateTrendChart(const std::vector<BenchmarkMetrics>& metrics) const;
    std::string generatePerformanceRadar(const BenchmarkMetrics& metric) const;
};

} // namespace benchmark
} // namespace nlm

#include "BenchmarkSuite.inl"