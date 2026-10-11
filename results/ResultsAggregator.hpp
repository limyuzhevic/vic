// ResultsAggregator.hpp - Result aggregation and analysis system
#pragma once

#include <string>
#include <vector>
#include <map>
#include <unordered_map>
#include <algorithm>
#include <numeric>
#include <cmath>
#include <chrono>
#include <functional>
#include <memory>

namespace nlm {

/**
 * ResultsAggregator - Result aggregation and analysis system
 * 
 * Provides comprehensive result aggregation and analysis capabilities:
 * - Cross-batch result aggregation
 * - Statistical analysis of batch results
 * - Success/failure classification
 * - Result filtering and visualization preparation
 * - Correlation analysis
 * - Performance metrics calculation
 * - Confidence interval computation
 * - Result comparison and difference analysis
 * 
 * Features:
 * - Multi-dimensional aggregation
 * - Statistical metrics calculation (mean, variance, stddev, etc.)
 * - Classification of results (success, warning, error)
 * - Correlation analysis between different metrics
 * - Visualization preparation (CSV, JSON, etc.)
 * - Performance benchmarks and comparisons
 * - Confidence interval computation
 * - Result filtering based on criteria
 * 
 * @note Integrates with batch processing results
 * @see BatchProcessor for batch processing integration
 * @see ParallelProcessor for parallel execution
 */
class ResultsAggregator {
public:
    /**
     * @brief ResultsAggregator constructor
     * @details Initializes results aggregator with default settings
     * @post Aggregator initialized and ready for result processing
     */
    ResultsAggregator();
    
    /**
     * @brief ResultsAggregator destructor
     * @post All resources released
     */
    ~ResultsAggregator();
    
    /**
     * @brief Aggregate results from batch processing
     * @param batchResults Map of configuration files to results
     * @return Aggregated results
     * @post Results aggregated and processed
     * @details Performs comprehensive aggregation including:
     * - Success/failure classification
     * - Statistical analysis
     * - Cross-batch comparisons
     * - Metrics computation
     */
    std::map<std::string, double> aggregateResults(const std::map<std::string, std::string>& batchResults);
    
    /**
     * @brief Perform statistical analysis on results
     * @param results Vector of result values
     * @param metricsToCompute Metrics to compute (mean, variance, etc.)
     * @return Statistical analysis results
     * @post Analysis completed
     * @details Calculates various statistical metrics including:
     * - Basic statistics (mean, median, mode)
     * - Dispersion (variance, stddev, range)
     * - Shape (skewness, kurtosis)
     * - Distribution characteristics
     */
    std::map<std::string, double> statisticalAnalysis(const std::vector<double>& results,
                                                       const std::vector<std::string>& metricsToCompute = {});
    
    /**
     * @brief Classify results into success/failure categories
     * @param results Map of configuration files to result values
     * @param thresholds Thresholds for classification
     * @return Classification results
     * @post Results classified
     * @details Classifies results based on thresholds and metrics:
     * - Success: results within acceptable range
     * - Warning: results approaching limits
     * - Error: results outside acceptable range
     * - Critical: results severely outside limits
     */
    std::map<std::string, std::string> classifyResults(const std::map<std::string, double>& results,
                                                       const std::map<std::string, double>& thresholds = {});
    
    /**
     * @brief Filter results based on criteria
     * @param results Map of configuration files to result values
     * @param filterCriteria Filter criteria (key-value pairs)
     * @return Filtered results
     * @post Results filtered
     * @details Filters results based on specified criteria:
     * - Numeric thresholds (greater than, less than, range)
     * - String pattern matching
     * - Date/time filtering
     * - Custom filtering functions
     */
    std::map<std::string, double> filterResults(const std::map<std::string, double>& results,
                                                const std::map<std::string, std::string>& filterCriteria);
    
    /**
     * @brief Perform correlation analysis between metrics
     * @param metric1 First metric data
     * @param metric2 Second metric data
     * @return Correlation analysis results
     * @post Correlation computed
     * @details Computes Pearson and Spearman correlations:
     * - Correlation coefficient
     * - P-value
     * - Statistical significance
     * - Effect size
     */
    std::map<std::string, double> correlationAnalysis(const std::vector<double>& metric1,
                                                      const std::vector<double>& metric2);
    
    /**
     * @brief Compare results against baseline
     * @param results Current results
     * @param baseline Baseline results
     * @param comparisonMetrics Metrics to compare
     * @return Comparison results
     * @post Comparison completed
     * @details Compares results against baseline:
     * - Absolute and relative differences
     * - Performance improvements/regressions
     * - Statistical significance
     * - Confidence intervals
     */
    std::map<std::string, double> compareToBaseline(const std::map<std::string, double>& results,
                                                    const std::map<std::string, double>& baseline,
                                                    const std::vector<std::string>& comparisonMetrics = {});
    
    /**
     * @brief Generate confidence intervals for results
     * @param results Result values
     * @param confidenceLevel Confidence level (0.0 to 1.0)
     * @return Confidence interval results
     * @post Intervals computed
     * @details Computes confidence intervals:
     * - Mean confidence interval
     * - Percentile intervals
     * - Bootstrap confidence intervals
     */
    std::map<std::string, double> confidenceIntervals(const std::vector<double>& results,
                                                      double confidenceLevel = 0.95);
    
    /**
     * @brief Prepare results for visualization
     * @param results Results to prepare
     * @param format Output format (csv, json, etc.)
     * @return Formatted results string
     * @post Results formatted for visualization
     * @details Formats results for various visualization formats:
     * - CSV: Comma-separated values
     * - JSON: JavaScript Object Notation
     * - Parquet: Columnar format
     * - Custom formats
     */
    std::string prepareVisualizationData(const std::map<std::string, double>& results,
                                          const std::string& format = "csv");
    
    /**
     * @brief Calculate performance metrics
     * @param executionTimes Vector of execution times
     * @param memoryUsageVector Vector of memory usage values
     * @return Performance metrics
     * @post Metrics calculated
     * @details Calculates various performance metrics:
     * - Throughput (operations per second)
     * - Latency (average execution time)
     * - Resource efficiency
     * - Scalability metrics
     */
    std::map<std::string, double> calculatePerformanceMetrics(const std::vector<double>& executionTimes,
                                                                const std::vector<double>& memoryUsageVector);
    
    /**
     * @brief Analyze result trends over time
     * @param timeSeriesResults Time-series results (timestamp, value)
     * @return Trend analysis results
     * @post Trend analysis completed
     * @details Analyzes trends in time-series data:
     * - Linear regression trends
     * - Seasonal patterns
     * - Anomalies detection
     * - Trend strength
     */
    std::map<std::string, double> analyzeTrends(const std::vector<std::pair<std::chrono::steady_clock::time_point, double>>& timeSeriesResults);
    
    /**
     * @brief Get comprehensive report
     * @param aggregatedResults Aggregated results
     * @return Report string
     * @post Report generated
     * @details Generates comprehensive report including:
     * - Summary statistics
     * - Classification results
     * - Performance metrics
     * - Recommendations
     */
    std::string generateReport(const std::map<std::string, double>& aggregatedResults);
    
    /**
     * @brief Set result aggregation options
     * @param options Aggregation options
     * @post Options set
     */
    void setAggregationOptions(const std::map<std::string, std::string>& options) { aggregationOptions_ = options; }
    
    /**
     * @brief Set statistical metrics to compute
     * @param metrics Metrics to compute
     * @post Metrics set
     */
    void setMetricsToCompute(const std::vector<std::string>& metrics) { metricsToCompute_ = metrics; }
    
    /**
     * @brief Set confidence level for intervals
     * @param level Confidence level
     * @post Level set
     */
    void setConfidenceLevel(double level) { confidenceLevel_ = level; }
    
    /**
     * @brief Set result filtering criteria
     * @param criteria Filter criteria
     * @post Criteria set
     */
    void setFilterCriteria(const std::map<std::string, std::string>& criteria) { filterCriteria_ = criteria; }
    
    /**
     * @brief Get aggregated results
     * @return Aggregated results
     * @post Results accessible
     */
    const std::map<std::string, double>& getAggregatedResults() const { return aggregatedResults_; }
    
    /**
     * @brief Get classification results
     * @return Classification results
     * @post Classifications accessible
     */
    const std::map<std::string, std::string>& getClassificationResults() const { return classificationResults_; }
    
    /**
     * @brief Get statistical results
     * @return Statistical results
     * @post Statistics accessible
     */
    const std::map<std::string, double>& getStatisticalResults() const { return statisticalResults_; }
    
    /**
     * @brief Get correlation results
     * @return Correlation results
     * @post Correlations accessible
     */
    const std::map<std::string, double>& getCorrelationResults() const { return correlationResults_; }

private:
    // Core aggregation state
    std::map<std::string, double> aggregatedResults_;
    std::map<std::string, std::string> classificationResults_;
    std::map<std::string, double> statisticalResults_;
    std::map<std::string, double> correlationResults_;
    
    // Configuration options
    std::map<std::string, std::string> aggregationOptions_;
    std::vector<std::string> metricsToCompute_;
    double confidenceLevel_;
    std::map<std::string, std::string> filterCriteria_;
    
    // Statistical calculations
    double calculateMean(const std::vector<double>& values) const;
    double calculateVariance(const std::vector<double>& values, double mean) const;
    double calculateStandardDeviation(const std::vector<double>& values, double mean) const;
    double calculateMedian(const std::vector<double>& values) const;
    double calculateMode(const std::vector<double>& values) const;
    double calculateSkewness(const std::vector<double>& values, double mean, double stddev) const;
    double calculateKurtosis(const std::vector<double>& values, double mean, double stddev) const;
    
    // Classification logic
    std::string classifyValue(double value, const std::string& metricName) const;
    
    // Correlation calculations
    double pearsonCorrelation(const std::vector<double>& x, const std::vector<double>& y) const;
    double spearmanCorrelation(const std::vector<double>& x, const std::vector<double>& y) const;
    
    // Confidence interval calculations
    std::pair<double, double> meanConfidenceInterval(const std::vector<double>& values, double confidenceLevel) const;
    std::pair<double, double> percentileInterval(const std::vector<double>& values, double confidenceLevel) const;
    
    // Data formatting
    std::string formatAsCSV(const std::map<std::string, double>& data) const;
    std::string formatAsJSON(const std::map<std::string, double>& data) const;
    
    // Suppress warnings about unused private members
    ResultsAggregator(const ResultsAggregator&) = delete;
    ResultsAggregator& operator=(const ResultsAggregator&) = delete;
};

} // namespace nlm