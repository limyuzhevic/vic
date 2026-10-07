// AnalysisReporter.hpp - Analysis and Reporting System
// Identifies performance bottlenecks, provides optimization recommendations,
// performs comparative analysis and trend analysis

#pragma once

/**
 * AnalysisReporter - Comprehensive analysis and reporting for NLM performance
 * 
 * This module provides advanced analysis capabilities for NLM performance data,
 * including:
 * 
 * 1. Performance Bottleneck Identification
 *    - Automated bottleneck detection
 *    - Component-level performance analysis
 *    - Resource contention analysis
 *    - Performance degradation tracking
 * 
 * 2. Optimization Recommendations
 *    - Data-driven optimization suggestions
 *    - Performance tuning recommendations
 *    - Resource allocation optimization
 *    - Algorithm optimization guidance
 * 
 * 3. Comparative Analysis Across Runs
 *    - Multiple run comparison
 *    - Performance trend analysis
 *    - Configuration comparison
 *    - Benchmark analysis
 * 
 * 4. Performance Trend Analysis
 *    - Time-series analysis
 *    - Performance drift detection
 *    - Seasonal performance patterns
 *    - Predictive performance modeling
 * 
 * 5. Automated Performance Testing
 *    - Performance regression detection
 *    - Baseline comparison
 *    - Performance test automation
 *    - Continuous performance monitoring
 *    
 * Features:
 * - Statistical analysis and modeling
 * - Machine learning for pattern detection
 * - Interactive reporting and visualization
 * - Export capabilities for multiple formats
 * - Integration with existing NLM systems
 * - Real-time and post-mortem analysis
 */

#include <chrono>
#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <memory>
#include <atomic>
#include <functional>
#include <algorithm>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <mutex>
#include <thread>
#include <future>
#include <map>
#include <variant>
#include <optional>
#include <numeric>
#include <cmath>
#include <type_traits>
#include <set>
#include <queue>
#include <list>
#include <regex>
#include <numeric>

namespace nlm {

/**
 * Bottleneck analysis result
 */
struct BottleneckAnalysis {
    std::string bottleneckComponent; // Component that is the bottleneck
    double bottleneckSeverity;        // Severity (0.0 - 1.0)
    double performanceImpact;         // Impact on overall performance
    std::string rootCause;            // Root cause of the bottleneck
    std::string suggestedFix;         // Suggested fix
    double estimatedImprovement;      // Estimated performance improvement (percent)
    double fixComplexity;             // Complexity of implementing the fix (0.0 - 1.0)
    
    BottleneckAnalysis() : bottleneckSeverity(0.0), performanceImpact(0.0),
                          estimatedImprovement(0.0), fixComplexity(0.0) {}
};

/**
 * Optimization recommendation
 */
struct OptimizationRecommendation {
    std::string recommendationId;
    std::string title;
    std::string description;
    std::string category; // "memory", "cpu", "io", "algorithm", "configuration"
    double priority;       // Priority (1.0 - highest, 0.1 - lowest)
    double impact;         // Expected impact on performance (percent improvement)
    double effort;         // Implementation effort (person-hours)
    double risk;           // Risk level (0.0 - 1.0)
    std::string implementation; // Implementation steps
    std::string expectedResults; // Expected results after implementation
    bool implemented;      // Whether the recommendation has been implemented
    uint64_t implementationTimestamp; // When implemented (if implemented)
    
    OptimizationRecommendation() : priority(1.0), impact(0.0), effort(0.0), risk(0.0), implemented(false) {}
};

/**
 * Performance comparison between two runs
 */
struct PerformanceComparison {
    std::string run1Id;
    std::string run2Id;
    std::string run1Description;
    std::string run2Description;
    
    // Overall performance metrics
    double performanceRatio;       // run2 / run1
    double performanceImprovement; // (run2 - run1) / run1 * 100%
    
    // Component-wise comparisons
    struct ComponentComparison {
        std::string component;
        double oldValue;
        double newValue;
        double ratio;
        double improvement;
        std::string trend; // "improving", "degrading", "stable"
    };
    
    std::vector<ComponentComparison> componentComparisons;
    
    // Statistical significance
    double statisticalSignificance; // p-value
    bool statisticallySignificant;  // Whether difference is statistically significant
    
    // Confidence intervals
    double ciLowerBound;  // 95% confidence interval lower bound
    double ciUpperBound;  // 95% confidence interval upper bound
    
    PerformanceComparison() : performanceRatio(1.0), performanceImprovement(0.0),
                              statisticalSignificance(1.0), statisticallySignificant(false),
                              ciLowerBound(0.0), ciUpperBound(0.0) {}
};

/**
 * Performance trend analysis
 */
struct PerformanceTrend {
    std::string trendId;
    std::string metricName;
    std::vector<double> dataPoints; // Time series data
    std::vector<uint64_t> timestamps; // Corresponding timestamps
    
    // Trend analysis results
    double slope;           // Linear regression slope
    double rSquared;         // Coefficient of determination
    std::string trendType;   // "increasing", "decreasing", "stable", "oscillating"
    double trendStrength;    // Strength of the trend (0.0 - 1.0)
    
    // Statistical metrics
    double mean;            // Mean value
    double median;          // Median value
    double stdDev;           // Standard deviation
    double min;             // Minimum value
    double max;             // Maximum value
    double percentile95;    // 95th percentile
    
    // Change detection
    double volatility;      // Volatility measure
    double anomalyScore;     // Anomaly score (higher = more anomalous)
    std::vector<size_t> anomalyIndices; // Indices of anomalous data points
    
    PerformanceTrend() : slope(0.0), rSquared(0.0), trendStrength(0.0),
                        mean(0.0), median(0.0), stdDev(0.0), min(0.0), max(0.0),
                        percentile95(0.0), volatility(0.0), anomalyScore(0.0) {}
};

/**
 * Performance benchmark configuration
 */
struct BenchmarkConfig {
    std::string name;
    std::string description;
    
    // Test parameters
    std::vector<size_t> neuronCounts; // Different neuron counts to test
    std::vector<size_t> timestepCounts; // Different timestep counts to test
    std::vector<std::unordered_map<std::string, std::string>> configurations; // Different configurations
    
    // Performance targets
    double minStepsPerSecond;    // Minimum steps per second
    size_t maxMemoryUsageMB;    // Maximum memory usage in MB
    double maxTimestepTimeMs;   // Maximum timestep time in milliseconds
    
    // Baseline configuration
    std::unordered_map<std::string, std::string> baselineConfig;
    std::string baselineId;     // ID of the baseline run
    
    // Analysis settings
    bool detectRegressions;     // Detect performance regressions
    bool detectImprovements;    // Detect performance improvements
    double regressionThreshold; // Threshold for regression detection (percent)
    double improvementThreshold; // Threshold for improvement detection (percent)
    bool enableTrendAnalysis;   // Enable trend analysis
    bool enableAnomalyDetection; // Enable anomaly detection
    
    BenchmarkConfig() : minStepsPerSecond(1000.0), maxMemoryUsageMB(1024),
                       maxTimestepTimeMs(100.0), regressionThreshold(-5.0),
                       improvementThreshold(5.0), enableTrendAnalysis(true),
                       enableAnomalyDetection(true) {}
};

/**
 * Automated test result
 */
struct AutomatedTestResult {
    std::string testId;
    std::string testName;
    uint64_t timestamp; // Unix timestamp
    std::string status; // "passed", "failed", "skipped", "error"
    std::string message; // Test message or error description
    
    // Performance metrics
    double executionTimeMs;   // Execution time in milliseconds
    size_t memoryUsageMB;     // Memory usage in MB
    double cpuUsagePercent;  // CPU usage percentage
    
    // Test details
    std::unordered_map<std::string, std::string> parameters; // Test parameters
    std::unordered_map<std::string, std::string> expectedResults; // Expected results
    std::unordered_map<std::string, std::string> actualResults; // Actual results
    
    // Comparison with baseline
    bool comparedToBaseline;  // Whether compared to baseline
    double regressionPercent; // Regression percentage (positive = regression)
    bool regressionDetected;  // Whether regression was detected
    
    AutomatedTestResult() : timestamp(0), executionTimeMs(0.0), memoryUsageMB(0),
                           cpuUsagePercent(0.0), comparedToBaseline(false),
                           regressionPercent(0.0), regressionDetected(false) {}
};

/**
 * Analysis report configuration
 */
struct AnalysisReportConfig {
    std::string name;
    std::string outputFormat; // "text", "json", "html", "pdf"
    std::string outputPath;   // Output file path or directory
    bool generateSummary;    // Generate summary report
    bool generateDetailed;   // Generate detailed report
    bool generateVisualizations; // Generate visualizations (requires external tools)
    std::vector<std::string> includeSections; // Sections to include
    std::vector<std::string> excludeSections; // Sections to exclude
    
    AnalysisReportConfig() : generateSummary(true), generateDetailed(true),
                            generateVisualizations(false) {
        includeSections = {"executive_summary", "bottleneck_analysis", 
                          "optimization_recommendations", "performance_trends",
                          "comparative_analysis", "automated_tests"};
    }
};

/**
 * Analysis report
 */
struct AnalysisReport {
    std::string reportId;
    std::string title;
    std::string description;
    uint64_t timestamp; // Generation time
    
    // Report sections
    std::string executiveSummary;
    std::string bottleneckAnalysis;
    std::string optimizationRecommendations;
    std::string performanceTrends;
    std::string comparativeAnalysis;
    std::string automatedTests;
    std::string technicalDetails;
    std::string conclusions;
    
    // Metadata
    std::string generatedBy; // Tool that generated the report
    std::string generatedFrom; // Source of data
    double dataCollectionDuration; // Duration of data collection
    
    AnalysisReport() : timestamp(0), dataCollectionDuration(0.0) {}
    
    std::string toText() const;
    std::string toJSON() const;
    std::string toHTML() const;
    std::string toMarkdown() const;
};

/**
 * Performance analyzer
 */
class PerformanceAnalyzer {
public:
    virtual ~PerformanceAnalyzer() = default;
    
    virtual std::string getAnalyzerName() const = 0;
    virtual std::string getVersion() const = 0;
    
    virtual BottleneckAnalysis analyzeBottlenecks(const std::vector<TimestepPerformance>& timestepData) = 0;
    virtual std::vector<OptimizationRecommendation> generateRecommendations(const BottleneckAnalysis& bottleneck) = 0;
    virtual std::vector<PerformanceComparison> compareRuns(const std::vector<ProfileSession>& sessions) = 0;
    virtual std::vector<PerformanceTrend> analyzeTrends(const std::vector<ProfileSession>& sessions, const std::string& metricName) = 0;
    virtual std::vector<AutomatedTestResult> runAutomatedTests(const BenchmarkConfig& benchmark) = 0;
    
    virtual std::string analyzeRegressions(const std::vector<ProfileSession>& sessions) = 0;
    virtual std::string analyzeImprovements(const std::vector<ProfileSession>& sessions) = 0;
    virtual AnalysisReport generateReport(const AnalysisReportConfig& config, 
                                         const std::vector<ProfileSession>& sessions) = 0;
};

/**
 * Statistical analyzer
 */
class StatisticalAnalyzer {
public:
    static double calculateMean(const std::vector<double>& values);
    static double calculateStdDev(const std::vector<double>& values, double mean = 0.0);
    static double calculateMedian(const std::vector<double>& values);
    static double calculatePercentile(const std::vector<double>& values, double percentile);
    static size_t calculateMode(const std::vector<size_t>& values);
    static std::string describeDistribution(const std::vector<double>& values);
    static std::pair<double, double> linearRegression(const std::vector<double>& x, const std::vector<double>& y);
    static double calculateCorrelation(const std::vector<double>& x, const std::vector<double>& y);
    static double calculateCovariance(const std::vector<double>& x, const std::vector<double>& y);
    static double calculateSpearmanRankCorrelation(const std::vector<double>& x, const std::vector<double>& y);
    static bool performTTest(const std::vector<double>& sample1, const std::vector<double>& sample2, double alpha = 0.05);
    static std::string identifyTrendType(const std::vector<double>& values, double threshold = 0.1);
    static double calculateTrendStrength(const std::vector<double>& values);
    static std::vector<size_t> detectAnomalies(const std::vector<double>& values, double threshold = 2.0);
    static std::vector<std::vector<double>> normalizeData(const std::vector<std::vector<double>>& data);
    static std::vector<std::vector<double>> principalComponentAnalysis(const std::vector<std::vector<double>>& data, size_t components = 2);
    static std::string formatStatisticalResult(double value, const std::string& format = "%.4f");
};

/**
 * Performance analyzer for NLM
 */
class NLPerformanceAnalyzer : public PerformanceAnalyzer {
private:
    std::string analyzerName_;
    std::string version_;
    
    // Configuration
    BenchmarkConfig benchmarkConfig_;
    AnalysisReportConfig reportConfig_;
    
    // State
    std::unordered_map<std::string, std::vector<ProfileSession>> sessionCache_;
    std::unordered_map<std::string, std::vector<PerformanceTrend>> trendCache_;
    
    mutable std::mutex mutex_;
    
public:
    NLPerformanceAnalyzer(const std::string& name = "NLM Performance Analyzer");
    
    std::string getAnalyzerName() const override { return analyzerName_; }
    std::string getVersion() const override { return version_; }
    
    BottleneckAnalysis analyzeBottlenecks(const std::vector<TimestepPerformance>& timestepData) override;
    std::vector<OptimizationRecommendation> generateRecommendations(const BottleneckAnalysis& bottleneck) override;
    std::vector<PerformanceComparison> compareRuns(const std::vector<ProfileSession>& sessions) override;
    std::vector<PerformanceTrend> analyzeTrends(const std::vector<ProfileSession>& sessions, const std::string& metricName) override;
    std::vector<AutomatedTestResult> runAutomatedTests(const BenchmarkConfig& benchmark) override;
    
    std::string analyzeRegressions(const std::vector<ProfileSession>& sessions) override;
    std::string analyzeImprovements(const std::vector<ProfileSession>& sessions) override;
    AnalysisReport generateReport(const AnalysisReportConfig& config, 
                                 const std::vector<ProfileSession>& sessions) override;
    
    // Configuration methods
    void setBenchmarkConfig(const BenchmarkConfig& config) { benchmarkConfig_ = config; }
    const BenchmarkConfig& getBenchmarkConfig() const { return benchmarkConfig_; }
    
    void setReportConfig(const AnalysisReportConfig& config) { reportConfig_ = config; }
    const AnalysisReportConfig& getReportConfig() const { return reportConfig_; }
    
    // Cache management
    void cacheSessions(const std::vector<ProfileSession>& sessions);
    std::vector<ProfileSession> getCachedSessions(const std::string& cacheKey) const;
    
    // Helper methods
    std::vector<double> extractMetricFromSessions(const std::vector<ProfileSession>& sessions, 
                                                 const std::string& metricName) const;
    std::vector<TimestepPerformance> extractTimestepDataFromSessions(const std::vector<ProfileSession>& sessions) const;
    
    // Performance analysis helpers
    std::string classifyPerformanceLevel(double metricValue, const std::string& metricName) const;
    double calculatePerformanceScore(const PerformanceMetrics& metrics) const;
    std::string analyzePerformanceIssues(const PerformanceMetrics& metrics) const;
};

/**
 * Performance benchmark runner
 */
class PerformanceBenchmark {
private:
    std::string benchmarkName_;
    bool running_;
    std::chrono::high_resolution_clock::time_point startTime_;
    std::vector<AutomatedTestResult> testResults_;
    
    mutable std::mutex mutex_;
    
public:
    PerformanceBenchmark(const std::string& name);
    
    void startBenchmark();
    void stopBenchmark();
    
    bool isRunning() const { return running_.load(); }
    
    // Test execution
    AutomatedTestResult runTest(const std::string& testName, 
                               const std::function<bool()>& testFunction,
                               const std::unordered_map<std::string, std::string>& parameters = {});
    
    void runBenchmarkSuite(const BenchmarkConfig& config);
    
    // Configuration
    void setBenchmarkConfig(const BenchmarkConfig& config) { config_ = config; }
    const BenchmarkConfig& getBenchmarkConfig() const { return config_; }
    
    // Results
    std::vector<AutomatedTestResult> getTestResults() const;
    std::string getBenchmarkSummary() const;
    
    // Export
    std::string exportResultsToJSON() const;
    std::string exportResultsToCSV() const;
    
private:
    BenchmarkConfig config_;
};

/**
 * Performance reporter
 */
class PerformanceReporter {
public:
    virtual ~PerformanceReporter() = default;
    
    virtual std::string getReporterName() const = 0;
    virtual std::string getVersion() const = 0;
    
    virtual std::string generateReport(const AnalysisReport& report) = 0;
    virtual bool exportReport(const AnalysisReport& report, const std::string& outputPath) = 0;
    virtual bool generateVisualizations(const AnalysisReport& report, const std::string& outputPath) = 0;
};

/**
 * Console performance reporter
 */
class ConsolePerformanceReporter : public PerformanceReporter {
public:
    std::string getReporterName() const override { return "Console Reporter"; }
    std::string getVersion() const override { return "1.0.0"; }
    
    std::string generateReport(const AnalysisReport& report) override;
    bool exportReport(const AnalysisReport& report, const std::string& outputPath) override;
    bool generateVisualizations(const AnalysisReport& report, const std::string& outputPath) override;
};

/**
 * JSON performance reporter
 */
class JSONPerformanceReporter : public PerformanceReporter {
public:
    std::string getReporterName() const override { return "JSON Reporter"; }
    std::string getVersion() const override { return "1.0.0"; }
    
    std::string generateReport(const AnalysisReport& report) override;
    bool exportReport(const AnalysisReport& report, const std::string& outputPath) override;
    bool generateVisualizations(const AnalysisReport& report, const std::string& outputPath) override;
};

/**
 * HTML performance reporter
 */
class HTMLPerformanceReporter : public PerformanceReporter {
public:
    std::string getReporterName() const override { return "HTML Reporter"; }
    std::string getVersion() const override { return "1.0.0"; }
    
    std::string generateReport(const AnalysisReport& report) override;
    bool exportReport(const AnalysisReport& report, const std::string& outputPath) override;
    bool generateVisualizations(const AnalysisReport& report, const std::string& outputPath) override;
};

/**
 * Reporter factory
 */
class ReporterFactory {
public:
    static std::shared_ptr<PerformanceReporter> createReporter(const std::string& name);
    
    static std::vector<std::string> getAvailableReporters();
};

/**
 * Analysis and reporting factory
 */
class AnalysisFactory {
public:
    static std::shared_ptr<PerformanceAnalyzer> createAnalyzer(const std::string& name);
    static std::shared_ptr<PerformanceReporter> createReporter(const std::string& name);
    static std::shared_ptr<PerformanceBenchmark> createBenchmark(const std::string& name);
    
    static std::vector<std::string> getAvailableAnalyzers();
    static std::vector<std::string> getAvailableReporters();
    static std::vector<std::string> getAvailableBenchmarks();
};

} // namespace nlm