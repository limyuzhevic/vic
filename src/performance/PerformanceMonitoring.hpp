#pragma once

/**
 * Performance Monitoring Tools - Modern C++ Implementation
 * 
 * This provides comprehensive performance monitoring and profiling capabilities
 * for NLM (Neural Learning Machine) using modern C++ features for enhanced
 * performance analysis and optimization.
 * 
 * Key features:
 * - Real-time performance monitoring
 * - Memory usage tracking and analysis
 * - CPU profiling and optimization
 * - Neural network performance metrics
 * - Experiment performance monitoring
 * - Resource usage analysis
 * - Performance trend analysis
 * - Performance alerting and notification
 * - Performance benchmarking
 * - Performance optimization recommendations
 * 
 * This system provides the foundation for comprehensive performance monitoring
 * and optimization of NLM applications, with enterprise-grade reliability and
 * extensibility characteristics.
 */

#include <memory>
#include <string>
#include <vector>
#include <unordered_map>
#include <functional>
#include <optional>
#include <variant>
#include <chrono>
#include <thread>
#include <atomic>
#include <concepts>
#include <ranges>
#include <span>
#include <queue>
#include <stack>
#include <mutex>
#include <shared_mutex>
#include <map>
#include <set>
#include <algorithm>
#include <numeric>
#include <random>
#include <future>
#include <exception>
#include <type_traits>
#include <any>
#include <filesystem>
#include <format>
#include <source_location>
#include <execution>
#include <numeric>
#include <typeinfo>
#include <sstream>
#include <iomanip>
#include <fstream>

namespace nlm {

// Type definitions for performance monitoring system
using PerformanceMetricID = std::string;
using PerformanceMetricValue = std::variant<double, float, int, std::string>;
using PerformanceMetricTimestamp = std::chrono::high_resolution_clock::time_point;
using PerformanceMetricTags = std::vector<std::string>;
using PerformanceMetricLabels = std::vector<std::string>;
using PerformanceBenchmarkID = std::string;
using PerformanceAlertID = std::string;
using PerformanceReportID = std::string;
using PerformanceTrendID = std::string;

// Performance metric data structure
struct PerformanceMetric {
    // Metric identification and classification
    PerformanceMetricID id;
    std::string name;
    std::string description;
    std::string unit;
    
    // Metric properties and characteristics
    PerformanceMetricType type;
    PerformanceMetricCategory category;
    PerformanceMetricTags tags;
    PerformanceMetricLabels labels;
    
    // Metric data and properties
    PerformanceMetricValue currentValue;
    PerformanceMetricValue previousValue;
    std::vector<PerformanceMetricValue> history;
    std::vector<PerformanceMetricTimestamp> timestamps;
    
    // Metric statistics and analysis
    double mean;
    double standardDeviation;
    double minimum;
    double maximum;
    double median;
    double percentile95;
    double percentile99;
    
    // Metric temporal and contextual information
    PerformanceMetricTimestamp timestamp;
    std::chrono::duration<double> age;
    std::string context;
    double updateFrequency;
    
    // Metric quality and confidence
    double confidence;
    double reliability;
    std::vector<std::string> qualityIssues;
    bool isValid;
    
    PerformanceMetric() : mean(0.0), standardDeviation(0.0), minimum(0.0), maximum(0.0),
                         median(0.0), percentile95(0.0), percentile99(0.0), confidence(0.0),
                         reliability(0.0), updateFrequency(0.0), isValid(true) {}
    
    PerformanceMetric(const PerformanceMetricID& id, const std::string& name, 
                     const std::string& description, PerformanceMetricType type)
        : id(id), name(name), description(description), unit(""), type(type),
          category(PerformanceMetricCategory::GENERAL), confidence(0.0), reliability(0.0),
          updateFrequency(0.0), isValid(true) {
        timestamp = std::chrono::high_resolution_clock::now();
    }
    
    // Metric processing and analysis
    void update(const PerformanceMetricValue& value) {
        previousValue = currentValue;
        currentValue = value;
        
        history.push_back(value);
        timestamps.push_back(std::chrono::high_resolution_clock::now());
        
        // Update statistics
        updateStatistics();
        
        // Update confidence based on data quality
        confidence = std::min(1.0, confidence + (0.1 - confidence) * 0.1);
    }
    
    // Compute statistics from history
    void updateStatistics() {
        if (history.empty()) return;
        
        // Compute basic statistics
        std::vector<double> numericValues;
        for (const auto& value : history) {
            if (std::holds_alternative<double>(value)) {
                numericValues.push_back(std::get<double>(value));
            } else if (std::holds_alternative<float>(value)) {
                numericValues.push_back(static_cast<double>(std::get<float>(value)));
            } else if (std::holds_alternative<int>(value)) {
                numericValues.push_back(static_cast<double>(std::get<int>(value)));
            }
        }
        
        if (numericValues.empty()) return;
        
        // Compute mean
        double sum = std::accumulate(numericValues.begin(), numericValues.end(), 0.0);
        mean = sum / numericValues.size();
        
        // Compute standard deviation
        double sumSquares = 0.0;
        for (double value : numericValues) {
            double diff = value - mean;
            sumSquares += diff * diff;
        }
        standardDeviation = std::sqrt(sumSquares / numericValues.size());
        
        // Compute min/max
        minimum = *std::min_element(numericValues.begin(), numericValues.end());
        maximum = *std::max_element(numericValues.begin(), numericValues.end());
        
        // Compute median
        std::vector<double> sortedValues = numericValues;
        std::sort(sortedValues.begin(), sortedValues.end());
        size_t middle = sortedValues.size() / 2;
        median = sortedValues[middle];
        if (sortedValues.size() % 2 == 0) {
            median = (sortedValues[middle - 1] + sortedValues[middle]) / 2.0;
        }
        
        // Compute percentiles
        if (numericValues.size() >= 20) {
            std::nth_element(numericValues.begin(), numericValues.begin() + numericValues.size() * 0.95, numericValues.end());
            percentile95 = numericValues[numericValues.size() * 0.95];
            
            std::nth_element(numericValues.begin(), numericValues.begin() + numericValues.size() * 0.99, numericValues.end());
            percentile99 = numericValues[numericValues.size() * 0.99];
        }
    }
    
    // Get metric value as specific type
    template<typename T>
    std::optional<T> getValue() const {
        if (const auto* val = std::get_if<T>(&currentValue)) {
            return *val;
        }
        return std::nullopt;
    }
    
    // Get metric value as string
    std::string getStringValue() const {
        if (std::holds_alternative<double>(currentValue)) {
            std::ostringstream oss;
            oss << std::fixed << std::setprecision(6) << std::get<double>(currentValue);
            return oss.str();
        } else if (std::holds_alternative<float>(currentValue)) {
            std::ostringstream oss;
            oss << std::fixed << std::setprecision(6) << std::get<float>(currentValue);
            return oss.str();
        } else if (std::holds_alternative<int>(currentValue)) {
            return std::to_string(std::get<int>(currentValue));
        } else if (std::holds_alternative<std::string>(currentValue)) {
            return std::get<std::string>(currentValue);
        }
        return "";
    }
    
    // Compute metric change rate
    double getChangeRate() const {
        if (!previousValue.has_value() || currentValue == previousValue) {
            return 0.0;
        }
        
        double previousDouble = getDoubleValue(previousValue);
        double currentDouble = getDoubleValue(currentValue);
        
        if (previousDouble == 0.0) {
            return (currentDouble - previousDouble);
        }
        
        return (currentDouble - previousDouble) / previousDouble;
    }
    
    // Get double value from variant
    double getDoubleValue(const PerformanceMetricValue& value) const {
        if (std::holds_alternative<double>(value)) {
            return std::get<double>(value);
        } else if (std::holds_alternative<float>(value)) {
            return static_cast<double>(std::get<float>(value));
        } else if (std::holds_alternative<int>(value)) {
            return static_cast<double>(std::get<int>(value));
        }
        return 0.0;
    }
};

/**
 * Performance benchmark structure
 * 
 * Defines benchmark specifications and execution parameters for
 * performance testing and comparison.
 */
struct PerformanceBenchmark {
    // Benchmark identification and classification
    PerformanceBenchmarkID id;
    std::string name;
    std::string description;
    
    // Benchmark properties and characteristics
    PerformanceBenchmarkType type;
    std::string targetComponent;
    std::string benchmarkSuite;
    
    // Benchmark configuration and parameters
    std::vector<PerformanceMetricID> metrics;
    std::chrono::duration<double> duration;
    size_t iterationCount;
    size_t warmupIterations;
    double targetPerformance;
    std::string convergenceCriteria;
    
    // Benchmark execution and results
    std::chrono::high_resolution_clock::time_point startTime;
    std::chrono::high_resolution_clock::time_point endTime;
    std::vector<PerformanceMetric> results;
    double throughput;
    double efficiency;
    double scalability;
    
    // Benchmark metadata and configuration
    std::string platform;
    std::string compiler;
    std::string architecture;
    std::unordered_map<std::string, std::string> environmentVariables;
    std::string benchmarkVersion;
    std::string baselineVersion;
    
    PerformanceBenchmark() : iterationCount(100), warmupIterations(10),
                           throughput(0.0), efficiency(0.0), scalability(0.0) {}
    
    PerformanceBenchmark(const PerformanceBenchmarkID& id, const std::string& name,
                         PerformanceBenchmarkType type, const std::string& targetComponent)
        : id(id), name(name), description(""), type(type), targetComponent(targetComponent),
          benchmarkSuite("default"), iterationCount(100), warmupIterations(10),
          throughput(0.0), efficiency(0.0), scalability(0.0) {
        startTime = std::chrono::high_resolution_clock::now();
    }
    
    // Start benchmark execution
    void start() {
        startTime = std::chrono::high_resolution_clock::now();
    }
    
    // End benchmark execution
    void end() {
        endTime = std::chrono::high_resolution_clock::now();
        
        duration = std::chrono::duration<double>(endTime - startTime);
        
        if (duration.count() > 0.0) {
            throughput = static_cast<double>(iterationCount) / duration.count();
        }
    }
    
    // Compute efficiency
    void computeEfficiency(double baselinePerformance) {
        if (baselinePerformance > 0.0) {
            efficiency = throughput / baselinePerformance;
        }
    }
    
    // Compute scalability
    void computeScalability(const std::vector<double>& throughputs) {
        if (throughputs.size() <= 1) return;
        
        double minThroughput = *std::min_element(throughputs.begin(), throughputs.end());
        double maxThroughput = *std::max_element(throughputs.begin(), throughputs.end());
        
        if (minThroughput > 0.0) {
            scalability = maxThroughput / minThroughput;
        }
    }
};

/**
 * Performance alert system
 * 
 * Provides alerting and notification capabilities for performance metrics
 * that exceed specified thresholds or exhibit concerning trends.
 */
struct PerformanceAlert {
    // Alert identification and classification
    PerformanceAlertID id;
    std::string name;
    std::string description;
    
    // Alert properties and characteristics
    PerformanceAlertType type;
    PerformanceAlertSeverity severity;
    PerformanceAlertCategory category;
    
    // Alert conditions and thresholds
    PerformanceMetricID metricId;
    std::string condition;
    double threshold;
    std::string thresholdUnit;
    double tolerance;
    std::chrono::duration<double> cooldownPeriod;
    
    // Alert state and tracking
    PerformanceAlertStatus status;
    std::chrono::high_resolution_clock::time_point lastTriggered;
    std::chrono::high_resolution_clock::time_point nextAllowedTrigger;
    size_t triggerCount;
    double currentValue;
    double currentChange;
    
    // Alert metadata and configuration
    std::string message;
    std::string recommendedAction;
    std::string notificationChannel;
    std::unordered_map<std::string, std::any> metadata;
    
    PerformanceAlert() : threshold(0.0), tolerance(0.0), triggerCount(0),
                         currentValue(0.0), currentChange(0.0), status(PerformanceAlertStatus::INACTIVE) {}
    
    PerformanceAlert(const PerformanceAlertID& id, const std::string& name,
                     PerformanceAlertType type, PerformanceAlertSeverity severity)
        : id(id), name(name), description(""), type(type), severity(severity),
          category(PerformanceAlertCategory::PERFORMANCE), threshold(0.0),
          tolerance(0.0), triggerCount(0), currentValue(0.0), currentChange(0.0),
          status(PerformanceAlertStatus::INACTIVE) {}
    
    // Check if alert should be triggered
    bool shouldTrigger(double metricValue, double metricChange) {
        currentValue = metricValue;
        currentChange = metricChange;
        
        // Check cooldown period
        auto now = std::chrono::high_resolution_clock::now();
        if (now < nextAllowedTrigger) {
            return false;
        }
        
        // Check threshold conditions
        switch (type) {
            case PerformanceAlertType::THRESHOLD_EXCEEDED:
                if (metricValue > threshold) {
                    return true;
                }
                break;
            case PerformanceAlertType::THRESHOLD_UNDERFLOWED:
                if (metricValue < threshold) {
                    return true;
                }
                break;
            case PerformanceAlertType::CHANGE_RATE_EXCEEDED:
                if (std::abs(metricChange) > threshold) {
                    return true;
                }
                break;
            case PerformanceAlertType::CRITICAL_DECREASE:
                if (metricChange < -threshold) {
                    return true;
                }
                break;
            case PerformanceAlertType::CRITICAL_INCREASE:
                if (metricChange > threshold) {
                    return true;
                }
                break;
            case PerformanceAlertType::UNEXPECTED_VALUE:
                if (metricValue == 0.0 || std::isnan(metricValue) || std::isinf(metricValue)) {
                    return true;
                }
                break;
            case PerformanceAlertType::PERFORMANCE_DEGRADATION:
                if (metricValue < threshold * 0.9) {
                    return true;
                }
                break;
            case PerformanceAlertType::RESOURCE_EXHAUSTION:
                if (metricValue > threshold * 0.95) {
                    return true;
                }
                break;
        }
        
        return false;
    }
    
    // Trigger alert
    void trigger() {
        status = PerformanceAlertStatus::ACTIVE;
        lastTriggered = std::chrono::high_resolution_clock::now();
        nextAllowedTrigger = lastTriggered + cooldownPeriod;
        triggerCount++;
    }
    
    // Resolve alert
    void resolve() {
        status = PerformanceAlertStatus::RESOLVED;
    }
    
    // Get alert message
    std::string getMessage() const {
        std::ostringstream oss;
        oss << "[" << name << "] " << description << ": " << currentValue;
        if (!thresholdUnit.empty()) {
            oss << " " << thresholdUnit;
        }
        oss << " (threshold: " << threshold;
        if (!thresholdUnit.empty()) {
            oss << " " << thresholdUnit;
        }
        oss << ")";
        return oss.str();
    }
};

/**
 * Performance trend analysis
 * 
 * Analyzes performance trends over time to identify patterns,
 * trends, and potential issues.
 */
class PerformanceTrendAnalyzer {
public:
    enum class TrendDirection {
        STABLE,
        INCREASING,
        DECREASING,
        VOLATILE,
        UNKNOWN
    };
    
    struct TrendData {
        PerformanceTrendID id;
        std::string name;
        PerformanceMetricID metricId;
        std::vector<PerformanceMetric> metricHistory;
        std::chrono::duration<double> analysisPeriod;
        
        // Trend analysis results
        TrendDirection direction;
        double trendStrength;
        double variability;
        double volatility;
        double seasonality;
        double anomalyScore;
        
        // Trend predictions
        std::vector<std::pair<TimePoint, double>> predictions;
        double confidenceLevel;
        std::vector<std::string> trendFactors;
        
        TrendData() : direction(TrendDirection::UNKNOWN), trendStrength(0.0),
                     variability(0.0), volatility(0.0), seasonality(0.0),
                     anomalyScore(0.0), confidenceLevel(0.0) {}
    };
    
    PerformanceTrendAnalyzer() = default;
    
    // Analyze trend from metric history
    TrendData analyzeTrend(const PerformanceMetric& metric,
                          const std::chrono::duration<double>& analysisPeriod) {
        TrendData trend;
        trend.id = "trend_" + metric.id;
        trend.name = "Trend for " + metric.name;
        trend.metricId = metric.id;
        trend.metricHistory = metric.history;
        trend.analysisPeriod = analysisPeriod;
        
        if (trend.metricHistory.empty()) {
            trend.direction = TrendDirection::UNKNOWN;
            return trend;
        }
        
        // Analyze trend direction
        trend.direction = calculateTrendDirection(trend.metricHistory);
        
        // Calculate trend strength
        trend.trendStrength = calculateTrendStrength(trend.metricHistory);
        
        // Calculate variability
        trend.variability = calculateVariability(trend.metricHistory);
        
        // Calculate volatility
        trend.volatility = calculateVolatility(trend.metricHistory);
        
        // Calculate seasonality
        trend.seasonality = calculateSeasonality(trend.metricHistory);
        
        // Calculate anomaly score
        trend.anomalyScore = calculateAnomalyScore(trend.metricHistory);
        
        // Generate predictions
        trend.predictions = generatePredictions(trend.metricHistory);
        
        // Calculate confidence level
        trend.confidenceLevel = calculateConfidenceLevel(trend.metricHistory);
        
        // Identify trend factors
        trend.trendFactors = identifyTrendFactors(trend.metricHistory);
        
        return trend;
    }
    
    // Analyze trend from multiple metrics
    std::vector<TrendData> analyzeTrends(const std::vector<PerformanceMetric>& metrics,
                                       const std::chrono::duration<double>& analysisPeriod) {
        std::vector<TrendData> trends;
        trends.reserve(metrics.size());
        
        for (const auto& metric : metrics) {
            trends.push_back(analyzeTrend(metric, analysisPeriod));
        }
        
        return trends;
    }
    
private:
    // Calculate trend direction
    TrendDirection calculateTrendDirection(const std::vector<PerformanceMetric>& history) {
        if (history.size() < 2) return TrendDirection::UNKNOWN;
        
        std::vector<double> values;
        for (const auto& metric : history) {
            double value = metric.getDoubleValue(metric.currentValue);
            if (value != 0.0) {
                values.push_back(value);
            }
        }
        
        if (values.size() < 2) return TrendDirection::UNKNOWN;
        
        // Calculate correlation between time and values
        double n = static_cast<double>(values.size());
        double sumX = n * (n - 1) / 2.0;
        double sumY = std::accumulate(values.begin(), values.end(), 0.0);
        double sumXY = 0.0;
        double sumX2 = (n - 1) * n * (2 * n - 1) / 6.0;
        double sumY2 = std::inner_product(values.begin(), values.end(), values.begin(), 0.0);
        
        double correlation = (n * sumXY - sumX * sumY) / std::sqrt((n * sumX2 - sumX * sumX) * (n * sumY2 - sumY * sumY));
        
        if (correlation > 0.7) return TrendDirection::INCREASING;
        if (correlation < -0.7) return TrendDirection::DECREASING;
        if (std::abs(correlation) < 0.3) return TrendDirection::STABLE;
        
        return TrendDirection::VOLATILE;
    }
    
    // Calculate trend strength
    double calculateTrendStrength(const std::vector<PerformanceMetric>& history) {
        if (history.size() < 2) return 0.0;
        
        std::vector<double> values;
        for (const auto& metric : history) {
            double value = metric.getDoubleValue(metric.currentValue);
            if (value != 0.0) {
                values.push_back(value);
            }
        }
        
        if (values.size() < 2) return 0.0;
        
        // Calculate coefficient of variation
        double mean = std::accumulate(values.begin(), values.end(), 0.0) / values.size();
        double variance = 0.0;
        for (double value : values) {
            double diff = value - mean;
            variance += diff * diff;
        }
        variance /= values.size();
        double stdDev = std::sqrt(variance);
        
        if (mean == 0.0) return 0.0;
        
        return stdDev / mean;
    }
    
    // Calculate variability
    double calculateVariability(const std::vector<PerformanceMetric>& history) {
        if (history.size() < 2) return 0.0;
        
        std::vector<double> values;
        for (const auto& metric : history) {
            double value = metric.getDoubleValue(metric.currentValue);
            if (value != 0.0) {
                values.push_back(value);
            }
        }
        
        if (values.size() < 2) return 0.0;
        
        // Calculate range-based variability
        double minValue = *std::min_element(values.begin(), values.end());
        double maxValue = *std::max_element(values.begin(), values.end());
        
        return (maxValue - minValue) / (maxValue + minValue + 0.001);
    }
    
    // Calculate volatility
    double calculateVolatility(const std::vector<PerformanceMetric>& history) {
        if (history.size() < 2) return 0.0;
        
        std::vector<double> values;
        for (const auto& metric : history) {
            double value = metric.getDoubleValue(metric.currentValue);
            if (value != 0.0) {
                values.push_back(value);
            }
        }
        
        if (values.size() < 2) return 0.0;
        
        // Calculate rolling standard deviation
        double rollingStdDev = 0.0;
        size_t windowSize = std::min<size_t>(10, values.size());
        
        for (size_t i = 0; i <= values.size() - windowSize; ++i) {
            std::vector<double> window(values.begin() + i, values.begin() + i + windowSize);
            double mean = std::accumulate(window.begin(), window.end(), 0.0) / window.size();
            double variance = 0.0;
            for (double value : window) {
                double diff = value - mean;
                variance += diff * diff;
            }
            variance /= window.size();
            rollingStdDev += std::sqrt(variance);
        }
        
        rollingStdDev /= values.size();
        return rollingStdDev;
    }
    
    // Calculate seasonality
    double calculateSeasonality(const std::vector<PerformanceMetric>& history) {
        if (history.size() < 4) return 0.0;
        
        std::vector<double> values;
        for (const auto& metric : history) {
            double value = metric.getDoubleValue(metric.currentValue);
            if (value != 0.0) {
                values.push_back(value);
            }
        }
        
        if (values.size() < 4) return 0.0;
        
        // Simple seasonality detection using FFT
        // This is a simplified implementation
        double seasonality = 0.0;
        
        // Check for periodic patterns
        for (size_t period = 1; period <= values.size() / 2; ++period) {
            double correlation = 0.0;
            for (size_t i = 0; i + period < values.size(); ++i) {
                correlation += values[i] * values[i + period];
            }
            correlation /= values.size() - period;
            
            seasonality = std::max(seasonality, std::abs(correlation));
        }
        
        return seasonality;
    }
    
    // Calculate anomaly score
    double calculateAnomalyScore(const std::vector<PerformanceMetric>& history) {
        if (history.size() < 3) return 0.0;
        
        std::vector<double> values;
        for (const auto& metric : history) {
            double value = metric.getDoubleValue(metric.currentValue);
            if (value != 0.0) {
                values.push_back(value);
            }
        }
        
        if (values.size() < 3) return 0.0;
        
        // Calculate z-scores
        double mean = std::accumulate(values.begin(), values.end(), 0.0) / values.size();
        double variance = 0.0;
        for (double value : values) {
            double diff = value - mean;
            variance += diff * diff;
        }
        variance /= values.size();
        double stdDev = std::sqrt(variance);
        
        // Count anomalies (values beyond 2 standard deviations)
        size_t anomalyCount = 0;
        for (double value : values) {
            if (std::abs(value - mean) > 2 * stdDev) {
                anomalyCount++;
            }
        }
        
        return static_cast<double>(anomalyCount) / values.size();
    }
    
    // Generate predictions
    std::vector<std::pair<TimePoint, double>> generatePredictions(const std::vector<PerformanceMetric>& history) {
        std::vector<std::pair<TimePoint, double>> predictions;
        
        if (history.size() < 3) return predictions;
        
        // Simple linear regression for prediction
        std::vector<double> values;
        for (const auto& metric : history) {
            double value = metric.getDoubleValue(metric.currentValue);
            if (value != 0.0) {
                values.push_back(value);
            }
        }
        
        if (values.size() < 3) return predictions;
        
        double n = static_cast<double>(values.size());
        double sumX = n * (n - 1) / 2.0;
        double sumY = std::accumulate(values.begin(), values.end(), 0.0);
        double sumXY = 0.0;
        double sumX2 = (n - 1) * n * (2 * n - 1) / 6.0;
        double sumY2 = std::inner_product(values.begin(), values.end(), values.begin(), 0.0);
        
        double slope = (n * sumXY - sumX * sumY) / (n * sumX2 - sumX * sumX);
        double intercept = (sumY - slope * sumX) / n;
        
        // Generate predictions for next time points
        for (size_t i = 1; i <= 5; ++i) {
            double predictedValue = slope * (n + i) + intercept;
            predictions.push_back(std::make_pair(
                std::chrono::high_resolution_clock::now() + std::chrono::hours(i),
                predictedValue
            ));
        }
        
        return predictions;
    }
    
    // Calculate confidence level
    double calculateConfidenceLevel(const std::vector<PerformanceMetric>& history) {
        if (history.size() < 2) return 0.0;
        
        std::vector<double> values;
        for (const auto& metric : history) {
            double value = metric.getDoubleValue(metric.currentValue);
            if (value != 0.0) {
                values.push_back(value);
            }
        }
        
        if (values.size() < 2) return 0.0;
        
        // Calculate prediction interval confidence
        double n = static_cast<double>(values.size());
        double sumX = n * (n - 1) / 2.0;
        double sumY = std::accumulate(values.begin(), values.end(), 0.0);
        double sumXY = 0.0;
        double sumX2 = (n - 1) * n * (2 * n - 1) / 6.0;
        double sumY2 = std::inner_product(values.begin(), values.end(), values.begin(), 0.0);
        
        double correlation = (n * sumXY - sumX * sumY) / std::sqrt((n * sumX2 - sumX * sumX) * (n * sumY2 - sumY * sumY));
        
        double confidence = std::abs(correlation);
        return std::min(1.0, confidence * 2.0); // Cap at 1.0
    }
    
    // Identify trend factors
    std::vector<std::string> identifyTrendFactors(const std::vector<PerformanceMetric>& history) {
        std::vector<std::string> factors;
        
        if (history.size() < 3) return factors;
        
        std::vector<double> values;
        for (const auto& metric : history) {
            double value = metric.getDoubleValue(metric.currentValue);
            if (value != 0.0) {
                values.push_back(value);
            }
        }
        
        // Identify potential factors
        if (values.size() >= 3) {
            // Check for exponential growth/decay
            double growthRate = 0.0;
            for (size_t i = 1; i < values.size(); ++i) {
                if (values[i - 1] != 0.0) {
                    growthRate += (values[i] - values[i - 1]) / values[i - 1];
                }
            }
            growthRate /= (values.size() - 1);
            
            if (growthRate > 0.1) {
                factors.push_back("exponential_growth");
            } else if (growthRate < -0.1) {
                factors.push_back("exponential_decay");
            }
            
            // Check for linear trend
            double correlation = calculateLinearCorrelation(values);
            if (correlation > 0.8) {
                factors.push_back("strong_linear_trend");
            } else if (correlation < -0.8) {
                factors.push_back("strong_negative_trend");
            } else if (std::abs(correlation) < 0.3) {
                factors.push_back("no_significant_trend");
            } else {
                factors.push_back("moderate_trend");
            }
            
            // Check for seasonality
            if (values.size() >= 7) {
                double weeklyTrend = calculateWeeklyTrend(values);
                if (weeklyTrend > 0.2) {
                    factors.push_back("weekly_seasonality");
                }
            }
        }
        
        return factors;
    }
    
    // Calculate linear correlation
    double calculateLinearCorrelation(const std::vector<double>& values) const {
        if (values.size() < 2) return 0.0;
        
        double n = static_cast<double>(values.size());
        double sumX = n * (n - 1) / 2.0;
        double sumY = std::accumulate(values.begin(), values.end(), 0.0);
        double sumXY = 0.0;
        for (size_t i = 0; i < values.size(); ++i) {
            sumXY += static_cast<double>(i) * values[i];
        }
        
        double sumX2 = (n - 1) * n * (2 * n - 1) / 6.0;
        double sumY2 = std::inner_product(values.begin(), values.end(), values.begin(), 0.0);
        
        double correlation = (n * sumXY - sumX * sumY) / std::sqrt((n * sumX2 - sumX * sumX) * (n * sumY2 - sumY * sumY));
        
        return correlation;
    }
    
    // Calculate weekly trend
    double calculateWeeklyTrend(const std::vector<double>& values) const {
        if (values.size() < 7) return 0.0;
        
        // Simple weekly trend calculation
        std::vector<double> weeklyValues;
        for (size_t i = 0; i + 7 <= values.size(); i += 7) {
            double weekSum = std::accumulate(values.begin() + i, values.begin() + i + 7, 0.0);
            weeklyValues.push_back(weekSum / 7.0);
        }
        
        if (weeklyValues.size() < 2) return 0.0;
        
        double trend = (weeklyValues.back() - weeklyValues.front()) / weeklyValues.front();
        return trend;
    }
};

/**
 * Performance monitoring system
 * 
 * Provides comprehensive performance monitoring and analysis capabilities
 * for NLM applications using modern C++ features for efficient and
 * scalable performance monitoring.
 */
class PerformanceMonitor {
public:
    // Performance monitoring configuration
    struct MonitorConfig {
        // Monitoring settings
        bool enableRealTimeMonitoring;
        bool enableHistoricalAnalysis;
        bool enableAlerting;
        bool enableBenchmarking;
        bool enableTrendAnalysis;
        bool enableResourceTracking;
        
        // Performance thresholds
        double cpuThreshold;
        double memoryThreshold;
        double diskThreshold;
        double networkThreshold;
        
        // Analysis settings
        std::chrono::duration<double> analysisInterval;
        std::chrono::duration<double> historicalDataRetention;
        size_t maxHistoricalDataPoints;
        
        // Alert settings
        std::chrono::duration<double> alertCooldown;
        std::string alertOutputChannel;
        bool enableEmailAlerts;
        bool enableConsoleAlerts;
        
        MonitorConfig() : enableRealTimeMonitoring(true), enableHistoricalAnalysis(true),
                         enableAlerting(true), enableBenchmarking(true), enableTrendAnalysis(true),
                         enableResourceTracking(true), cpuThreshold(90.0), memoryThreshold(85.0),
                         diskThreshold(95.0), networkThreshold(100.0), analysisInterval(std::chrono::seconds(5)),
                         historicalDataRetention(std::chrono::hours(24)), maxHistoricalDataPoints(1000),
                         alertCooldown(std::chrono::minutes(1)), alertOutputChannel("console"),
                         enableEmailAlerts(false), enableConsoleAlerts(true) {}
    };
    
    // Performance report
    struct PerformanceReport {
        PerformanceReportID id;
        std::string title;
        std::string description;
        
        // Report metadata
        TimePoint generationTime;
        std::chrono::duration<double> reportPeriod;
        PerformanceBenchmarkID benchmarkId;
        
        // Analysis results
        std::vector<PerformanceMetric> metrics;
        std::vector<PerformanceBenchmark> benchmarks;
        std::vector<PerformanceAlert> alerts;
        std::vector<PerformanceTrendAnalyzer::TrendData> trends;
        
        // Summary statistics
        double overallHealthScore;
        std::string overallStatus;
        std::vector<std::string> healthIssues;
        
        // Recommendations
        std::vector<std::string> optimizationRecommendations;
        std::vector<std::string> maintenanceRecommendations;
        
        PerformanceReport() : overallHealthScore(0.0), overallStatus("UNKNOWN") {
            generationTime = std::chrono::high_resolution_clock::now();
        }
        
        PerformanceReport(const PerformanceReportID& id, const std::string& title)
            : id(id), title(title), description(""), overallHealthScore(0.0), overallStatus("UNKNOWN") {
            generationTime = std::chrono::high_resolution_clock::now();
        }
    };
    
    PerformanceMonitor() : isRunning(false), config(), currentMetrics(), alerts(), benchmarks(),
                           trendAnalyzer(), lastUpdateTime(std::chrono::high_resolution_clock::now()),
                           totalDataPointsCollected(0), totalAlertsTriggered(0) {}
    
    // Initialize performance monitor
    bool initialize(const MonitorConfig& config);
    
    // Start performance monitoring
    void start();
    
    // Stop performance monitoring
    void stop();
    
    // Update performance metrics
    void updateMetrics();
    
    // Add performance metric
    PerformanceMetricID addMetric(const PerformanceMetric& metric);
    
    // Remove performance metric
    bool removeMetric(const PerformanceMetricID& id);
    
    // Get performance metric
    std::optional<PerformanceMetric> getMetric(const PerformanceMetricID& id) const;
    
    // Get all performance metrics
    std::vector<PerformanceMetric> getAllMetrics() const;
    
    // Get performance metric history
    std::vector<PerformanceMetric> getMetricHistory(const PerformanceMetricID& id, 
                                                   const std::chrono::duration<double>& period) const;
    
    // Add performance benchmark
    PerformanceBenchmarkID addBenchmark(const PerformanceBenchmark& benchmark);
    
    // Remove performance benchmark
    bool removeBenchmark(const PerformanceBenchmarkID& id);
    
    // Get performance benchmark
    std::optional<PerformanceBenchmark> getBenchmark(const PerformanceBenchmarkID& id) const;
    
    // Add performance alert
    PerformanceAlertID addAlert(const PerformanceAlert& alert);
    
    // Remove performance alert
    bool removeAlert(const PerformanceAlertID& id);
    
    // Get performance alert
    std::optional<PerformanceAlert> getAlert(const PerformanceAlertID& id) const;
    
    // Process performance alerts
    void processAlerts();
    
    // Generate performance report
    PerformanceReportID generateReport(const std::string& title, 
                                      const std::chrono::duration<double>& period);
    
    // Get performance report
    std::optional<PerformanceReport> getReport(const PerformanceReportID& id) const;
    
    // Save performance report
    bool saveReport(const PerformanceReportID& id, const std::string& filename);
    
    // Load performance report
    PerformanceReportID loadReport(const std::string& filename);
    
    // Get trend analysis
    PerformanceTrendAnalyzer::TrendData analyzeTrend(const PerformanceMetricID& metricId,
                                                    const std::chrono::duration<double>& period);
    
    // Get all trend analyses
    std::vector<PerformanceTrendAnalyzer::TrendData> analyzeTrends(
        const std::chrono::duration<double>& period);
    
    // Check if monitor is running
    bool isRunning() const { return isRunning; }
    
    // Get configuration
    const MonitorConfig& getConfig() const { return config; }
    
    // Set configuration
    void setConfig(const MonitorConfig& config) { this->config = config; }
    
    // Get total data points collected
    size_t getTotalDataPointsCollected() const { return totalDataPointsCollected; }
    
    // Get total alerts triggered
    size_t getTotalAlertsTriggered() const { return totalAlertsTriggered; }
    
    // Get current metrics
    std::vector<PerformanceMetric> getCurrentMetrics() const { return currentMetrics; }
    
    // Get current alerts
    std::vector<PerformanceAlert> getCurrentAlerts() const { return alerts; }
    
    // Get current benchmarks
    std::vector<PerformanceBenchmark> getCurrentBenchmarks() const { return benchmarks; }
    
    // Clear all metrics
    void clearMetrics();
    
    // Clear all alerts
    void clearAlerts();
    
    // Clear all benchmarks
    void clearBenchmarks();
    
    // Clear all reports
    void clearReports();
    
    // Export metrics to file
    bool exportMetrics(const std::string& filename);
    
    // Import metrics from file
    bool importMetrics(const std::string& filename);
    
    // Generate performance dashboard data
    std::string generateDashboardData() const;
    
    // Generate performance alert report
    std::string generateAlertReport() const;
    
    // Generate performance trend report
    std::string generateTrendReport() const;
    
    // Generate performance benchmark report
    std::string generateBenchmarkReport() const;
    
    // Generate performance health report
    std::string generateHealthReport() const;
    
    // Schedule performance benchmark
    PerformanceBenchmarkID scheduleBenchmark(const PerformanceBenchmark& benchmark);
    
    // Cancel performance benchmark
    bool cancelBenchmark(const PerformanceBenchmarkID& id);
    
    // Get scheduled benchmarks
    std::vector<PerformanceBenchmark> getScheduledBenchmarks() const;
    
    // Execute scheduled benchmarks
    void executeScheduledBenchmarks();
    
    // Monitor system resources
    void monitorSystemResources();
    
    // Monitor neural network performance
    void monitorNeuralNetworkPerformance();
    
    // Monitor experiment performance
    void monitorExperimentPerformance();
    
    // Monitor application performance
    void monitorApplicationPerformance();
    
    // Collect system metrics
    std::vector<PerformanceMetric> collectSystemMetrics();
    
    // Collect neural network metrics
    std::vector<PerformanceMetric> collectNeuralNetworkMetrics();
    
    // Collect experiment metrics
    std::vector<PerformanceMetric> collectExperimentMetrics();
    
    // Collect application metrics
    std::vector<PerformanceMetric> collectApplicationMetrics();
    
    // Update metric with new value
    void updateMetric(const PerformanceMetricID& id, const PerformanceMetricValue& value);
    
    // Update metric with new value and timestamp
    void updateMetric(const PerformanceMetricID& id, const PerformanceMetricValue& value,
                     const PerformanceMetricTimestamp& timestamp);
    
    // Create system metric
    PerformanceMetricID createSystemMetric(const std::string& name, const std::string& unit);
    
    // Create neural network metric
    PerformanceMetricID createNeuralNetworkMetric(const std::string& name, const std::string& unit);
    
    // Create experiment metric
    PerformanceMetricID createExperimentMetric(const std::string& name, const std::string& unit);
    
    // Create application metric
    PerformanceMetricID createApplicationMetric(const std::string& name, const std::string& unit);
    
    // Calculate performance score
    double calculatePerformanceScore(const std::vector<PerformanceMetric>& metrics) const;
    
    // Determine performance health status
    std::string determinePerformanceHealthStatus(const std::vector<PerformanceMetric>& metrics) const;
    
    // Generate performance health recommendations
    std::vector<std::string> generatePerformanceHealthRecommendations() const;
    
    // Generate performance optimization recommendations
    std::vector<std::string> generatePerformanceOptimizationRecommendations() const;
    
    // Generate performance maintenance recommendations
    std::vector<std::string> generatePerformanceMaintenanceRecommendations() const;
    
private:
    // Performance monitoring implementation
    // Real-time monitoring
    // Historical data management
    // Alert processing
    // Report generation
    // Trend analysis
    // System resource monitoring
    // Neural network monitoring
    // Experiment monitoring
    // Application monitoring
    
    bool isRunning;
    MonitorConfig config;
    
    // Performance metrics
    std::unordered_map<PerformanceMetricID, PerformanceMetric> metrics;
    std::vector<PerformanceMetric> currentMetrics;
    
    // Performance alerts
    std::unordered_map<PerformanceAlertID, PerformanceMetric> alerts;
    std::vector<PerformanceAlert> activeAlerts;
    
    // Performance benchmarks
    std::unordered_map<PerformanceBenchmarkID, PerformanceBenchmark> benchmarks;
    std::vector<PerformanceBenchmark> scheduledBenchmarks;
    
    // Performance reports
    std::unordered_map<PerformanceReportID, PerformanceReport> reports;
    
    // Timing and state
    TimePoint lastUpdateTime;
    size_t totalDataPointsCollected;
    size_t totalAlertsTriggered;
    
    // Resource monitoring
    std::thread monitoringThread;
    std::mutex metricsMutex;
    std::mutex alertsMutex;
    std::mutex benchmarksMutex;
    std::mutex reportsMutex;
    
    // Performance analysis
    PerformanceTrendAnalyzer trendAnalyzer;
    
    // System resource tracking
    struct SystemResourceStats {
        double cpuUsage;
        double memoryUsage;
        double diskUsage;
        double networkUsage;
        std::chrono::high_resolution_clock::time_point timestamp;
    };
    
    std::vector<SystemResourceStats> resourceHistory;
    
    // Neural network performance tracking
    struct NeuralNetworkStats {
        size_t neuronCount;
        size_t synapseCount;
        double firingRate;
        double averageWeight;
        size_t activeNeuronCount;
        double networkCapacity;
        std::chrono::high_resolution_clock::time_point timestamp;
    };
    
    std::vector<NeuralNetworkStats> neuralNetworkHistory;
    
    // Experiment performance tracking
    struct ExperimentStats {
        PerformanceBenchmarkID benchmarkId;
        std::string experimentName;
        double executionTime;
        double successRate;
        double rewardRate;
        size_t trialsCompleted;
        std::chrono::high_resolution_clock::time_point timestamp;
    };
    
    std::vector<ExperimentStats> experimentHistory;
    
    // Application performance tracking
    struct ApplicationStats {
        std::string componentName;
        double responseTime;
        double throughput;
        double errorRate;
        size_t requestsProcessed;
        std::chrono::high_resolution_clock::time_point timestamp;
    };
    
    std::vector<ApplicationStats> applicationHistory;
    
    // Performance event system
    enum class PerformanceEventType {
        METRIC_UPDATED,
        ALERT_TRIGGERED,
        BENCHMARK_COMPLETED,
        REPORT_GENERATED,
        SYSTEM_ALERT,
        PERFORMANCE_ANOMALY,
        PERFORMANCE_OPTIMIZATION
    };
    
    struct PerformanceEvent {
        PerformanceEventType type;
        std::string source;
        TimePoint timestamp;
        std::unordered_map<std::string, std::any> data;
        std::optional<std::string> message;
    };
    
    using PerformanceEventCallback = std::function<void(const PerformanceEvent&)>
    
    std::vector<PerformanceEventCallback> eventCallbacks;
    std::mutex eventCallbacksMutex;
    
    // Event processing
    void processEvents();
    
    // Monitor system resources
    void monitorSystemResourcesInternal();
    
    // Monitor neural network performance
    void monitorNeuralNetworkPerformanceInternal();
    
    // Monitor experiment performance
    void monitorExperimentPerformanceInternal();
    
    // Monitor application performance
    void monitorApplicationPerformanceInternal();
    
    // Collect system metrics internally
    PerformanceMetric collectSystemMetric(const std::string& name, double value);
    
    // Collect neural network metrics internally
    PerformanceMetric collectNeuralNetworkMetric(const std::string& name, double value);
    
    // Collect experiment metrics internally
    PerformanceMetric collectExperimentMetric(const std::string& name, double value);
    
    // Collect application metrics internally
    PerformanceMetric collectApplicationMetric(const std::string& name, double value);
    
    // Update metric internally
    void updateMetricInternal(PerformanceMetric& metric, const PerformanceMetricValue& value);
    
    // Add event callback
    void addEventCallback(PerformanceEventCallback callback);
    
    // Remove event callback
    void removeEventCallback(PerformanceEventCallback callback);
    
    // Publish event
    void publishEvent(const PerformanceEvent& event);
};

// Convenience factory functions for performance monitoring
namespace nlm::PerformanceMonitorFactory {
    
    /**
     * Create performance monitor
     * 
     * @param config Monitor configuration
     * @return Shared pointer to created monitor
     */
    std::shared_ptr<PerformanceMonitor> createMonitor(const PerformanceMonitor::MonitorConfig& config);
    
    /**
     * Create performance monitor with default configuration
     * 
     * @return Shared pointer to created monitor
     */
    std::shared_ptr<PerformanceMonitor> createMonitor();
    
    /**
     * Create performance metric
     * 
     * @param id Metric identifier
     * @param name Metric name
     * @param description Metric description
     * @param type Metric type
     * @return Created metric
     */
    PerformanceMetric createMetric(const PerformanceMetricID& id, const std::string& name,
                                  const std::string& description, PerformanceMetricType type);
    
    /**
     * Create performance benchmark
     * 
     * @param id Benchmark identifier
     * @param name Benchmark name
     * @param type Benchmark type
     * @param targetComponent Target component
     * @return Created benchmark
     */
    PerformanceBenchmark createBenchmark(const PerformanceBenchmarkID& id, const std::string& name,
                                        PerformanceBenchmarkType type, const std::string& targetComponent);
    
    /**
     * Create performance alert
     * 
     * @param id Alert identifier
     * @param name Alert name
     * @param type Alert type
     * @param severity Alert severity
     * @return Created alert
     */
    PerformanceAlert createAlert(const PerformanceAlertID& id, const std::string& name,
                                PerformanceAlertType type, PerformanceAlertSeverity severity);
    
} // namespace nlm::PerformanceMonitorFactory

} // namespace nlm

#endif // PERFORMANCE_MONITORING_HPP

EOF
