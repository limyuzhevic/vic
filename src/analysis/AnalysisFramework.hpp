#pragma once

/**
 * Performance Analysis Infrastructure
 * 
 * This module provides comprehensive performance analysis capabilities for NLM,
 * including performance analysis framework, performance data analysis, performance trend analysis,
 * performance correlation analysis, performance prediction, and performance optimization recommendation.
 * 
 * Key Features:
 * - Performance analysis framework with configurable analysis strategies
 * - Performance data analysis for raw performance data
 * - Performance trend analysis for time-series data
 * - Performance correlation analysis for relationship analysis
 * - Performance prediction for forecasting future performance
 * - Performance optimization recommendation for actionable insights
 * - Automated analysis pipeline
 * - Analysis validation framework
 * - Analysis documentation framework
 * 
 * Architecture:
 * - AnalysisBase: Abstract base class for all analysis strategies
 * - AnalysisFramework: Main framework for managing analyses
 * - DataAnalysis: Analysis of raw performance data
 * - TrendAnalysis: Analysis of performance trends
 * - CorrelationAnalysis: Analysis of performance correlations
 * - PredictionAnalysis: Analysis for performance prediction
 * - RecommendationGenerator: Generation of optimization recommendations
 * - AnalysisResult: Container for analysis results
 * - AnalysisValidator: Validation of analysis results
 * - AnalysisExecutor: Execution of analyses
 */

#include <chrono>
#include <fstream>
#include <iostream>
#include <map>
#include <memory>
#include <mutex>
#include <sstream>
#include <string>
#include <thread>
#include <unordered_map>
#include <vector>
#include <algorithm>
#include <numeric>
#include <iomanip>
#include <functional>
#include <cmath>
#include <set>
#include <random>
#include <Eigen/Dense>

namespace nlm {
namespace analysis {

// Performance analysis data structures
struct PerformanceDataset {
    std::string dataset_name;
    std::string dataset_type;  // metrics, events, trends, predictions
    std::vector<double> values;
    std::vector<uint64_t> timestamps;
    std::map<std::string, std::vector<double>> multidimensional_data;
    std::map<std::string, std::string> metadata;
    std::chrono::nanoseconds sampling_interval;
    uint64_t start_timestamp_ns;
    uint64_t end_timestamp_ns;
};

struct TrendAnalysisResult {
    std::string metric_name;
    std::string trend_type;  // linear, exponential, seasonal, cyclical, irregular
    double slope;
    double r_squared;
    double adjusted_r_squared;
    double p_value;
    std::vector<double> trend_coefficients;
    std::vector<uint64_t> trend_timestamps;
    std::vector<double> trend_values;
    std::map<std::string, std::string> trend_details;
    std::chrono::nanoseconds confidence_interval;
};

struct CorrelationAnalysisResult {
    std::string metric_a_name;
    std::string metric_b_name;
    double correlation_coefficient;
    double p_value;
    double spearman_correlation;
    std::vector<std::pair<double, double>> correlation_points;
    std::map<std::string, std::string> relationship_details;
    std::string correlation_type;  // positive, negative, no_correlation
    std::string confidence_level;
};

struct PredictionAnalysisResult {
    std::string metric_name;
    std::string prediction_type;  // linear, exponential, moving_average, arima, rnn
    std::vector<uint64_t> prediction_timestamps;
    std::vector<double> predicted_values;
    std::vector<double> confidence_intervals;
    std::vector<double> prediction_errors;
    double mae;  // Mean Absolute Error
    double rmse; // Root Mean Squared Error
    double mape; // Mean Absolute Percentage Error
    std::chrono::nanoseconds prediction_horizon;
    std::map<std::string, std::string> model_parameters;
    std::string model_accuracy;
};

struct AnalysisResult {
    std::string analysis_name;
    std::string analysis_type;
    uint64_t timestamp_ns;
    double overall_score;
    std::vector<std::string> findings;
    std::vector<std::string> recommendations;
    std::map<std::string, double> analysis_metrics;
    std::map<std::string, std::string> analysis_details;
    bool is_valid;
    std::string confidence_level;
    std::chrono::nanoseconds analysis_duration;
    std::string comments;
};

// Abstract base class for analysis strategies
class AnalysisBase {
protected:
    PerformanceDataset dataset_;
    bool enabled_;
    bool running_;
    std::mutex analysis_mutex_;
    
public:
    AnalysisBase(const PerformanceDataset& dataset) : dataset_(dataset), enabled_(true), running_(false) {}
    virtual ~AnalysisBase() = default;
    
    virtual AnalysisResult execute() = 0;
    virtual bool validate_preconditions() = 0;
    virtual bool validate_postconditions(const AnalysisResult& result) = 0;
    
    const PerformanceDataset& get_dataset() const { return dataset_; }
    bool is_enabled() const { return enabled_; }
    bool is_running() const { return running_; }
    
    void start() {
        std::lock_guard<std::mutex> lock(analysis_mutex_);
        if (!validate_preconditions()) {
            throw std::runtime_error("Analysis preconditions not met");
        }
        running_ = true;
    }
    
    void stop() {
        std::lock_guard<std::mutex> lock(analysis_mutex_);
        running_ = false;
    }
    
protected:
    static uint64_t getCurrentTimestamp() {
        auto now = std::chrono::high_resolution_clock::now();
        auto duration = now.time_since_epoch();
        return std::chrono::duration_cast<std::chrono::nanoseconds>(duration).count();
    }
};

// Data Analysis implementation
class DataAnalysis : public AnalysisBase {
public:
    DataAnalysis(const PerformanceDataset& dataset) : AnalysisBase(dataset) {}
    
    AnalysisResult execute() override {
        std::lock_guard<std::mutex> lock(analysis_mutex_);
        
        AnalysisResult result;
        result.analysis_name = "data_analysis_" + std::to_string(getCurrentTimestamp());
        result.analysis_type = "data_analysis";
        result.timestamp_ns = getCurrentTimestamp();
        
        // Perform data analysis
        result.analysis_metrics = calculate_basic_statistics();
        result.analysis_details = calculate_data_quality_metrics();
        result.findings = generate_data_findings();
        result.recommendations = generate_data_recommendations();
        
        // Calculate overall score
        result.overall_score = calculate_overall_score(result);
        result.is_valid = validate_postconditions(result);
        result.confidence_level = assess_confidence_level(result);
        result.comments = generate_data_comments(result);
        
        return result;
    }
    
    bool validate_preconditions() override {
        return !dataset_.values.empty() &&
               dataset_.timestamps.size() == dataset_.values.size();
    }
    
    bool validate_postconditions(const AnalysisResult& result) override {
        return result.is_valid &&
               result.overall_score >= 0.0 &&
               !result.findings.empty();
    }
    
private:
    std::map<std::string, double> calculate_basic_statistics() {
        std::map<std::string, double> stats;
        
        if (dataset_.values.empty()) return stats;
        
        double sum = std::accumulate(dataset_.values.begin(), dataset_.values.end(), 0.0);
        double mean = sum / dataset_.values.size();
        
        double sq_sum = 0.0;
        for (double value : dataset_.values) {
            double diff = value - mean;
            sq_sum += diff * diff;
        }
        double std_dev = std::sqrt(sq_sum / dataset_.values.size());
        
        double min_val = *std::min_element(dataset_.values.begin(), dataset_.values.end());
        double max_val = *std::max_element(dataset_.values.begin(), dataset_.values.end());
        
        stats["mean"] = mean;
        stats["stddev"] = std_dev;
        stats["min"] = min_val;
        stats["max"] = max_val;
        stats["range"] = max_val - min_val;
        stats["median"] = calculate_median(dataset_.values);
        
        return stats;
    }
    
    std::map<std::string, std::string> calculate_data_quality_metrics() {
        std::map<std::string, std::string> quality_metrics;
        
        if (dataset_.values.empty()) return quality_metrics;
        
        double completeness = 1.0;  // Simplified completeness check
        quality_metrics["completeness"] = std::to_string(completeness * 100.0) + "%";
        
        // Check for missing values
        size_t missing_count = 0;
        for (double value : dataset_.values) {
            if (std::isnan(value) || std::isinf(value)) missing_count++;
        }
        double missing_percentage = (missing_count / dataset_.values.size()) * 100.0;
        quality_metrics["missing_percentage"] = std::to_string(missing_percentage) + "%";
        
        // Check for outliers
        double mean = calculate_mean(dataset_.values);
        double std_dev = calculate_standard_deviation(dataset_.values, mean);
        size_t outlier_count = 0;
        for (double value : dataset_.values) {
            if (std::abs(value - mean) > 3.0 * std_dev) outlier_count++;
        }
        double outlier_percentage = (outlier_count / dataset_.values.size()) * 100.0;
        quality_metrics["outlier_percentage"] = std::to_string(outlier_percentage) + "%";
        
        // Calculate data consistency
        double consistency = calculate_consistency(dataset_.values);
        quality_metrics["consistency"] = std::to_string(consistency * 100.0) + "%";
        
        return quality_metrics;
    }
    
    std::vector<std::string> generate_data_findings() {
        std::vector<std::string> findings;
        
        if (dataset_.values.empty()) return findings;
        
        double mean = calculate_mean(dataset_.values);
        double std_dev = calculate_standard_deviation(dataset_.values, mean);
        
        if (std_dev > 0.0) {
            findings.push_back("Data shows moderate variability (stddev: " + std::to_string(std_dev) + ")");
        }
        
        // Check for trends
        if (dataset_.values.size() >= 3) {
            double correlation = calculate_linear_correlation(dataset_.values);
            if (std::abs(correlation) > 0.7) {
                findings.push_back("Data shows strong " + std::string(correlation > 0 ? "positive" : "negative") + " trend");
            }
        }
        
        // Check for seasonality
        if (dataset_.timestamps.size() >= 3) {
            double seasonality = calculate_seasonality_score(dataset_.values, dataset_.timestamps);
            if (seasonality > 0.5) {
                findings.push_back("Data shows seasonal patterns with strength: " + std::to_string(seasonality));
            }
        }
        
        return findings;
    }
    
    std::vector<std::string> generate_data_recommendations() {
        std::vector<std::string> recommendations;
        
        if (dataset_.values.empty()) return recommendations;
        
        double mean = calculate_mean(dataset_.values);
        double std_dev = calculate_standard_deviation(dataset_.values, mean);
        
        if (std_dev > mean * 0.5) {
            recommendations.push_back("Consider normalizing data to reduce variability");
        }
        
        if (dataset_.values.size() < 100) {
            recommendations.push_back("Increase dataset size for more reliable analysis");
        }
        
        recommendations.push_back("Validate data sources for accuracy and completeness");
        recommendations.push_back("Consider time-series specific analysis techniques");
        
        return recommendations;
    }
    
    double calculate_overall_score(const AnalysisResult& result) {
        double score = 0.0;
        
        // Base score for having data
        score += 30.0;
        
        // Add score for basic statistics
        if (result.analysis_metrics.find("stddev") != result.analysis_metrics.end()) {
            score += 20.0;
        }
        
        // Add score for findings
        score += result.findings.size() * 10.0;
        
        // Add score for recommendations
        score += result.recommendations.size() * 5.0;
        
        return std::min(100.0, score);
    }
    
    std::string assess_confidence_level(const AnalysisResult& result) {
        if (result.analysis_metrics.find("stddev") == result.analysis_metrics.end()) {
            return "low";
        }
        
        double stddev = result.analysis_metrics.at("stddev");
        if (stddev < 0.1) {
            return "high";
        } else if (stddev < 0.5) {
            return "medium";
        } else {
            return "low";
        }
    }
    
    std::string generate_data_comments(const AnalysisResult& result) {
        std::stringstream ss;
        ss << "Data analysis completed with overall score: " << result.overall_score << "/100";
        ss << ". Found " << result.findings.size() << " key findings and " << result.recommendations.size() << " recommendations.";
        return ss.str();
    }
    
    double calculate_mean(const std::vector<double>& values) {
        if (values.empty()) return 0.0;
        double sum = std::accumulate(values.begin(), values.end(), 0.0);
        return sum / values.size();
    }
    
    double calculate_standard_deviation(const std::vector<double>& values, double mean) {
        if (values.empty()) return 0.0;
        double sq_sum = 0.0;
        for (double value : values) {
            double diff = value - mean;
            sq_sum += diff * diff;
        }
        return std::sqrt(sq_sum / values.size());
    }
    
    double calculate_median(std::vector<double> values) {
        if (values.empty()) return 0.0;
        size_t n = values.size();
        std::sort(values.begin(), values.end());
        if (n % 2 == 0) {
            return (values[n/2 - 1] + values[n/2]) / 2.0;
        } else {
            return values[n/2];
        }
    }
    
    double calculate_consistency(const std::vector<double>& values) {
        if (values.size() < 2) return 1.0;
        
        double mean = calculate_mean(values);
        double sum_abs_diff = 0.0;
        for (double value : values) {
            sum_abs_diff += std::abs(value - mean);
        }
        
        return 1.0 - (sum_abs_diff / (values.size() * mean));
    }
    
    double calculate_linear_correlation(const std::vector<double>& values) {
        if (values.size() < 2) return 0.0;
        
        double mean = calculate_mean(values);
        double numerator = 0.0;
        double denominator_x = 0.0;
        double denominator_y = 0.0;
        
        for (size_t i = 0; i < values.size(); ++i) {
            double x = static_cast<double>(i);
            double y = values[i];
            numerator += (x - i) * (y - mean);
            denominator_x += (x - i) * (x - i);
            denominator_y += (y - mean) * (y - mean);
        }
        
        if (denominator_x == 0.0 || denominator_y == 0.0) return 0.0;
        
        return numerator / std::sqrt(denominator_x * denominator_y);
    }
    
    double calculate_seasonality_score(const std::vector<double>& values, const std::vector<uint64_t>& timestamps) {
        if (values.size() < 3) return 0.0;
        
        // Simplified seasonality calculation
        double sum = std::accumulate(values.begin(), values.end(), 0.0);
        double mean = sum / values.size();
        
        double sq_sum = 0.0;
        for (double value : values) {
            double diff = value - mean;
            sq_sum += diff * diff;
        }
        
        return std::sqrt(sq_sum / values.size());
    }
};

// Trend Analysis implementation
class TrendAnalysis : public AnalysisBase {
public:
    TrendAnalysis(const PerformanceDataset& dataset) : AnalysisBase(dataset) {}
    
    AnalysisResult execute() override {
        std::lock_guard<std::mutex> lock(analysis_mutex_);
        
        AnalysisResult result;
        result.analysis_name = "trend_analysis_" + std::to_string(getCurrentTimestamp());
        result.analysis_type = "trend_analysis";
        result.timestamp_ns = getCurrentTimestamp();
        
        // Perform trend analysis for each metric
        result.analysis_metrics = analyze_all_metrics();
        result.analysis_details = get_trend_details();
        result.findings = generate_trend_findings();
        result.recommendations = generate_trend_recommendations();
        
        // Calculate overall score
        result.overall_score = calculate_overall_score(result);
        result.is_valid = validate_postconditions(result);
        result.confidence_level = assess_confidence_level(result);
        result.comments = generate_trend_comments(result);
        
        return result;
    }
    
    bool validate_preconditions() override {
        return !dataset_.values.empty() &&
               !dataset_.multidimensional_data.empty();
    }
    
    bool validate_postconditions(const AnalysisResult& result) override {
        return result.is_valid &&
               result.overall_score >= 0.0 &&
               !result.findings.empty();
    }
    
    std::vector<TrendAnalysisResult> analyze_trends() {
        std::vector<TrendAnalysisResult> trends;
        
        if (dataset_.values.empty()) return trends;
        
        // Analyze the main time series
        TrendAnalysisResult trend;
        trend.metric_name = dataset_.dataset_name;
        trend.trend_type = determine_trend_type(dataset_.values);
        trend.slope = calculate_linear_regression_slope(dataset_.timestamps, dataset_.values);
        trend.r_squared = calculate_r_squared(dataset_.timestamps, dataset_.values, trend.slope);
        trend.adjusted_r_squared = calculate_adjusted_r_squared(trend.r_squared, dataset_.timestamps.size(), 1);
        trend.p_value = calculate_p_value(trend.slope, trend.r_squared, dataset_.timestamps.size(), 1);
        trend.trend_coefficients = calculate_regression_coefficients(dataset_.timestamps, dataset_.values);
        trend.trend_timestamps = dataset_.timestamps;
        trend.trend_values = dataset_.values;
        trend.confidence_interval = std::chrono::nanoseconds(static_cast<long>(3.0 * std::sqrt(calculate_variance(dataset_.values))));
        
        trends.push_back(trend);
        
        // Analyze multidimensional data
        for (const auto& pair : dataset_.multidimensional_data) {
            const std::string& metric_name = pair.first;
            const std::vector<double>& values = pair.second;
            
            if (values.size() >= dataset_.timestamps.size()) {
                TrendAnalysisResult dim_trend;
                dim_trend.metric_name = metric_name;
                dim_trend.trend_type = determine_trend_type(values);
                dim_trend.slope = calculate_linear_regression_slope(dataset_.timestamps, values);
                dim_trend.r_squared = calculate_r_squared(dataset_.timestamps, values, dim_trend.slope);
                dim_trend.adjusted_r_squared = calculate_adjusted_r_squared(dim_trend.r_squared, dataset_.timestamps.size(), 1);
                dim_trend.p_value = calculate_p_value(dim_trend.slope, dim_trend.r_squared, dataset_.timestamps.size(), 1);
                dim_trend.trend_coefficients = calculate_regression_coefficients(dataset_.timestamps, values);
                dim_trend.trend_timestamps = dataset_.timestamps;
                dim_trend.trend_values = values;
                dim_trend.confidence_interval = std::chrono::nanoseconds(static_cast<long>(3.0 * std::sqrt(calculate_variance(values))));
                
                trends.push_back(dim_trend);
            }
        }
        
        return trends;
    }
    
private:
    std::map<std::string, double> analyze_all_metrics() {
        std::map<std::string, double> metrics;
        
        // Analyze main metric
        if (!dataset_.values.empty()) {
            metrics["main_metric_slope"] = calculate_linear_regression_slope(dataset_.timestamps, dataset_.values);
            metrics["main_metric_r_squared"] = calculate_r_squared(dataset_.timestamps, dataset_.values, metrics["main_metric_slope"]);
            metrics["main_metric_trend_strength"] = calculate_trend_strength(dataset_.values);
            metrics["main_metric_volatility"] = calculate_volatility(dataset_.values);
        }
        
        // Analyze multidimensional metrics
        for (const auto& pair : dataset_.multidimensional_data) {
            const std::string& metric_name = pair.first;
            const std::vector<double>& values = pair.second;
            
            if (!values.empty() && values.size() == dataset_.timestamps.size()) {
                metrics[metric_name + "_slope"] = calculate_linear_regression_slope(dataset_.timestamps, values);
                metrics[metric_name + "_r_squared"] = calculate_r_squared(dataset_.timestamps, values, metrics[metric_name + "_slope"]);
                metrics[metric_name + "_trend_strength"] = calculate_trend_strength(values);
                metrics[metric_name + "_volatility"] = calculate_volatility(values);
            }
        }
        
        return metrics;
    }
    
    std::map<std::string, std::string> get_trend_details() {
        std::map<std::string, std::string> details;
        
        std::vector<TrendAnalysisResult> trends = analyze_trends();
        
        for (const auto& trend : trends) {
            details[trend.metric_name + "_type"] = trend.trend_type;
            details[trend.metric_name + "_slope"] = std::to_string(trend.slope);
            details[trend.metric_name + "_r_squared"] = std::to_string(trend.r_squared);
            details[trend.metric_name + "_p_value"] = std::to_string(trend.p_value);
            details[trend.metric_name + "_confidence"] = trend_confidence_to_string(trend.confidence_interval);
        }
        
        return details;
    }
    
    std::vector<std::string> generate_trend_findings() {
        std::vector<std::string> findings;
        
        std::vector<TrendAnalysisResult> trends = analyze_trends();
        
        for (const auto& trend : trends) {
            if (std::abs(trend.slope) > 0.01) {
                findings.push_back(std::string(trend.metric_name) + " shows " + 
                                 (trend.slope > 0 ? "upward" : "downward") + " trend with slope: " + 
                                 std::to_string(trend.slope));
            }
            
            if (trend.r_squared > 0.8) {
                findings.push_back(std::string(trend.metric_name) + " has strong correlation with time (R²: " + 
                                 std::to_string(trend.r_squared) + ")");
            }
            
            if (trend.p_value < 0.05) {
                findings.push_back(std::string(trend.metric_name) + " shows statistically significant trend (p-value: " + 
                                 std::to_string(trend.p_value) + ")");
            }
        }
        
        return findings;
    }
    
    std::vector<std::string> generate_trend_recommendations() {
        std::vector<std::string> recommendations;
        
        std::vector<TrendAnalysisResult> trends = analyze_trends();
        
        for (const auto& trend : trends) {
            if (trend.trend_type == "increasing") {
                recommendations.push_back(std::string(trend.metric_name) + ": prepare for continued growth");
            } else if (trend.trend_type == "decreasing") {
                recommendations.push_back(std::string(trend.metric_name) + ": investigate causes of decline");
            } else if (trend.trend_type == "volatile") {
                recommendations.push_back(std::string(trend.metric_name) + ": implement smoothing techniques to reduce noise");
            } else if (trend.trend_type == "seasonal") {
                recommendations.push_back(std::string(trend.metric_name) + ": leverage seasonal patterns for planning");
            }
        }
        
        recommendations.push_back("Monitor trend stability over longer periods");
        recommendations.push_back("Consider external factors that may influence trends");
        recommendations.push_back("Update forecasting models regularly");
        
        return recommendations;
    }
    
    double calculate_overall_score(const AnalysisResult& result) {
        double score = 0.0;
        
        // Base score for having trend analysis
        score += 30.0;
        
        // Add score for trend findings
        score += result.findings.size() * 10.0;
        
        // Add score for recommendations
        score += result.recommendations.size() * 5.0;
        
        // Add score for trend strength
        if (result.analysis_metrics.find("main_metric_trend_strength") != result.analysis_metrics.end()) {
            score += result.analysis_metrics.at("main_metric_trend_strength") * 20.0;
        }
        
        return std::min(100.0, score);
    }
    
    std::string assess_confidence_level(const AnalysisResult& result) {
        if (result.analysis_metrics.find("main_metric_r_squared") == result.analysis_metrics.end()) {
            return "low";
        }
        
        double r_squared = result.analysis_metrics.at("main_metric_r_squared");
        if (r_squared > 0.8) {
            return "high";
        } else if (r_squared > 0.5) {
            return "medium";
        } else {
            return "low";
        }
    }
    
    std::string generate_trend_comments(const AnalysisResult& result) {
        std::stringstream ss;
        ss << "Trend analysis completed with overall score: " << result.overall_score << "/100";
        ss << ". Found " << result.findings.size() << " key findings and " << result.recommendations.size() << " recommendations.";
        
        if (result.analysis_metrics.find("main_metric_trend_strength") != result.analysis_metrics.end()) {
            double strength = result.analysis_metrics.at("main_metric_trend_strength");
            ss << " Trend strength: " << strength;
        }
        
        return ss.str();
    }
    
    std::string determine_trend_type(const std::vector<double>& values) {
        if (values.size() < 3) return "insufficient_data";
        
        double slope = calculate_linear_regression_slope(std::vector<uint64_t>(values.size(), 0), values);
        
        if (std::abs(slope) < 0.001) {
            return "stable";
        } else if (slope > 0.01) {
            return "increasing";
        } else if (slope < -0.01) {
            return "decreasing";
        } else {
            // Check for seasonality
            if (has_seasonality(values)) {
                return "seasonal";
            } else {
                return "volatile";
            }
        }
    }
    
    bool has_seasonality(const std::vector<double>& values) {
        if (values.size() < 6) return false;
        
        // Simple seasonality check: look for repeating patterns
        size_t period = values.size() / 3;  // Assume 3 cycles
        if (period < 2) return false;
        
        double sum_diff = 0.0;
        for (size_t i = 0; i < values.size() - period; ++i) {
            sum_diff += std::abs(values[i] - values[i + period]);
        }
        
        double avg_diff = sum_diff / (values.size() - period);
        return avg_diff < 0.1 * calculate_mean(values);
    }
    
    double calculate_linear_regression_slope(const std::vector<uint64_t>& x,
                                               const std::vector<double>& y) {
        if (x.size() != y.size() || x.size() < 2) return 0.0;
        
        double sum_x = std::accumulate(x.begin(), x.end(), 0.0);
        double sum_y = std::accumulate(y.begin(), y.end(), 0.0);
        double sum_xy = 0.0;
        double sum_xx = 0.0;
        
        for (size_t i = 0; i < x.size(); ++i) {
            sum_xy += x[i] * y[i];
            sum_xx += x[i] * x[i];
        }
        
        double n = static_cast<double>(x.size());
        return (n * sum_xy - sum_x * sum_y) / (n * sum_xx - sum_x * sum_x);
    }
    
    double calculate_r_squared(const std::vector<uint64_t>& x,
                                const std::vector<double>& y,
                                double slope) {
        if (x.size() != y.size() || x.size() < 2) return 0.0;
        
        double mean_y = std::accumulate(y.begin(), y.end(), 0.0) / y.size();
        double ss_total = 0.0;
        double ss_residual = 0.0;
        
        for (size_t i = 0; i < x.size(); ++i) {
            double predicted_y = mean_y + slope * (x[i] - std::accumulate(x.begin(), x.end(), 0.0) / x.size());
            ss_total += (y[i] - mean_y) * (y[i] - mean_y);
            ss_residual += (y[i] - predicted_y) * (y[i] - predicted_y);
        }
        
        return ss_total - ss_residual / ss_total;
    }
    
    double calculate_adjusted_r_squared(double r_squared, double n, double p) {
        if (n <= p + 1) return 0.0;
        return 1.0 - (1.0 - r_squared) * (n - 1) / (n - p - 1);
    }
    
    double calculate_p_value(double slope, double r_squared, double n, double p) {
        if (r_squared <= 0.0) return 1.0;
        
        double f_stat = (r_squared / p) / ((1.0 - r_squared) / (n - p - 1));
        
        // Approximate p-value from F-distribution
        // In a real implementation, this would use proper statistical libraries
        if (f_stat > 10.0) return 0.001;
        else if (f_stat > 5.0) return 0.01;
        else if (f_stat > 2.0) return 0.05;
        else return 0.1;
    }
    
    std::vector<double> calculate_regression_coefficients(const std::vector<uint64_t>& x,
                                                           const std::vector<double>& y) {
        std::vector<double> coefficients(2, 0.0);  // intercept, slope
        
        if (x.size() != y.size() || x.size() < 2) return coefficients;
        
        double sum_x = std::accumulate(x.begin(), x.end(), 0.0);
        double sum_y = std::accumulate(y.begin(), y.end(), 0.0);
        double sum_xy = 0.0;
        double sum_xx = 0.0;
        double sum_yy = 0.0;
        
        for (size_t i = 0; i < x.size(); ++i) {
            sum_xy += x[i] * y[i];
            sum_xx += x[i] * x[i];
            sum_yy += y[i] * y[i];
        }
        
        double n = static_cast<double>(x.size());
        double denominator = n * sum_xx - sum_x * sum_x;
        
        if (denominator != 0.0) {
            coefficients[1] = (n * sum_xy - sum_x * sum_y) / denominator;  // slope
            coefficients[0] = (sum_y - coefficients[1] * sum_x) / n;  // intercept
        }
        
        return coefficients;
    }
    
    double calculate_variance(const std::vector<double>& values) {
        if (values.size() < 2) return 0.0;
        
        double mean = calculate_mean(values);
        double sum_sq = 0.0;
        for (double value : values) {
            double diff = value - mean;
            sum_sq += diff * diff;
        }
        
        return sum_sq / (values.size() - 1);
    }
    
    double calculate_trend_strength(const std::vector<double>& values) {
        if (values.size() < 3) return 0.0;
        
        double slope = calculate_linear_regression_slope(std::vector<uint64_t>(values.size(), 0), values);
        double mean = calculate_mean(values);
        
        return std::min(1.0, std::abs(slope) / (mean * 0.1));  // Normalize
    }
    
    double calculate_volatility(const std::vector<double>& values) {
        if (values.size() < 2) return 0.0;
        
        double mean = calculate_mean(values);
        double sum_sq = 0.0;
        for (double value : values) {
            double diff = value - mean;
            sum_sq += diff * diff;
        }
        
        double variance = sum_sq / (values.size() - 1);
        return std::sqrt(variance);
    }
    
    std::string trend_confidence_to_string(const std::chrono::nanoseconds& interval) {
        double seconds = std::chrono::duration<double>(interval).count();
        return std::to_string(seconds) + " seconds";
    }
};

// Main Analysis Framework Class
class AnalysisFramework {
private:
    std::vector<std::unique_ptr<AnalysisBase>> analyses_;
    std::vector<AnalysisResult> analysis_history_;
    std::mutex framework_mutex_;
    bool enabled_;
    
public:
    AnalysisFramework() : enabled_(false) {}
    
    void add_analysis(std::unique_ptr<AnalysisBase> analysis) {
        std::lock_guard<std::mutex> lock(framework_mutex_);
        analyses_.push_back(std::move(analysis));
    }
    
    AnalysisResult execute_analysis(const std::string& analysis_name) {
        std::lock_guard<std::mutex> lock(framework_mutex_);
        
        // Find the analysis
        auto it = std::find_if(analyses_.begin(), analyses_.end(),
            [&analysis_name](const std::unique_ptr<AnalysisBase>& analysis) {
                return analysis->get_dataset().dataset_name == analysis_name;
            });
        
        if (it == analyses_.end()) {
            throw std::runtime_error("Analysis not found: " + analysis_name);
        }
        
        AnalysisResult result = (*it)->execute();
        
        // Add to history
        analysis_history_.push_back(result);
        
        return result;
    }
    
    std::vector<AnalysisResult> execute_all_analyses() {
        std::lock_guard<std::mutex> lock(framework_mutex_);
        
        std::vector<AnalysisResult> results;
        
        for (auto& analysis : analyses_) {
            if (analysis->is_enabled()) {
                try {
                    AnalysisResult result = analysis->execute();
                    results.push_back(result);
                } catch (const std::exception& e) {
                    std::cerr << "Error executing analysis " << analysis->get_dataset().dataset_name 
                              << ": " << e.what() << std::endl;
                }
            }
        }
        
        return results;
    }
    
    std::vector<AnalysisResult> get_analysis_history() const {
        std::lock_guard<std::mutex> lock(framework_mutex_);
        return analysis_history_;
    }
    
    void print_analysis_summary() {
        std::cout << "=== NLM Analysis Summary ===" << std::endl;
        std::cout << "Total analyses: " << analyses_.size() << std::endl;
        std::cout << "Successful analyses: " << 
            std::count_if(analysis_history_.begin(), analysis_history_.end(),
                         [](const AnalysisResult& r) { return r.is_valid; }) << std::endl;
        std::cout << "Failed analyses: " << 
            std::count_if(analysis_history_.begin(), analysis_history_.end(),
                         [](const AnalysisResult& r) { return !r.is_valid; }) << std::endl;
        std::cout << std::endl;
        
        for (const auto& analysis : analyses_) {
            const auto& dataset = analysis->get_dataset();
            std::cout << "Analysis: " << dataset.dataset_name << std::endl;
            std::cout << "  Type: " << dataset.dataset_type << std::endl;
            std::cout << "  Dataset size: " << dataset.values.size() << " points" << std::endl;
            std::cout << "  Time range: " << dataset.start_timestamp_ns << " - " << dataset.end_timestamp_ns << " ns" << std::endl;
            std::cout << std::endl;
        }
    }
    
    void print_analysis_history() {
        std::cout << "=== Analysis History ===" << std::endl;
        
        for (const auto& result : analysis_history_) {
            std::cout << "Analysis: " << result.analysis_name << std::endl;
            std::cout << "  Type: " << result.analysis_type << std::endl;
            std::cout << "  Timestamp: " << result.timestamp_ns << std::endl;
            std::cout << "  Overall score: " << result.overall_score << "/100" << std::endl;
            std::cout << "  Valid: " << (result.is_valid ? "Yes" : "No") << std::endl;
            std::cout << "  Confidence: " << result.confidence_level << std::endl;
            std::cout << "  Duration: " << result.analysis_duration.count() << " ns" << std::endl;
            std::cout << "  Findings: " << result.findings.size() << std::endl;
            std::cout << "  Recommendations: " << result.recommendations.size() << std::endl;
            std::cout << "  Comments: " << result.comments << std::endl;
            std::cout << std::endl;
        }
    }
    
    std::vector<OptimizationRecommendation> generate_optimization_recommendations_from_analysis(
        const std::vector<AnalysisResult>& analysis_results) {
        std::vector<OptimizationRecommendation> recommendations;
        
        for (const auto& result : analysis_results) {
            if (result.is_valid && !result.findings.empty()) {
                OptimizationRecommendation recommendation;
                recommendation.recommendation_id = "rec_" + result.analysis_name + "_" + std::to_string(getCurrentTimestamp());
                recommendation.issue_type = "analysis_based_optimization";
                recommendation.component = result.analysis_name;
                recommendation.impact_score = result.overall_score / 100.0;
                recommendation.recommendation = generate_optimization_recommendation_from_analysis(result);
                recommendation.estimated_improvement = estimate_optimization_improvement(result);
                recommendation.implementation_difficulty = assess_implementation_difficulty(result);
                recommendation.required_steps = generate_required_steps(result);
                recommendation.estimated_time_ns = estimate_implementation_time(result);
                recommendation.prerequisites = generate_prerequisites(result);
                
                recommendations.push_back(recommendation);
            }
        }
        
        return recommendations;
    }
    
    void print_optimization_recommendations(const std::vector<OptimizationRecommendation>& recommendations) {
        std::cout << "=== Analysis-Based Optimization Recommendations ===" << std::endl;
        std::cout << "Total recommendations: " << recommendations.size() << std::endl;
        std::cout << std::endl;
        
        for (const auto& recommendation : recommendations) {
            std::cout << "Recommendation: " << recommendation.recommendation_id << std::endl;
            std::cout << "  Issue: " << recommendation.issue_type << std::endl;
            std::cout << "  Component: " << recommendation.component << std::endl;
            std::cout << "  Impact score: " << recommendation.impact_score << std::endl;
            std::cout << "  Recommendation: " << recommendation.recommendation << std::endl;
            std::cout << "  Estimated improvement: " << recommendation.estimated_improvement << "%" << std::endl;
            std::cout << "  Difficulty: " << recommendation.implementation_difficulty << std::endl;
            std::cout << "  Estimated time: " << recommendation.estimated_time_ns << " ns" << std::endl;
            std::cout << "  Prerequisites: " << recommendation.prerequisites << std::endl;
            std::cout << std::endl;
        }
    }
    
private:
    std::string generate_optimization_recommendation_from_analysis(const AnalysisResult& result) {
        if (result.analysis_type == "data_analysis") {
            return "Use data quality metrics to improve data collection and processing";
        } else if (result.analysis_type == "trend_analysis") {
            return "Leverage trend analysis to predict future performance and plan optimizations";
        } else if (result.analysis_type == "correlation_analysis") {
            return "Use correlation analysis to identify relationships and optimize system parameters";
        } else if (result.analysis_type == "prediction_analysis") {
            return "Use prediction analysis to forecast performance and prevent degradation";
        } else {
            return "Apply analysis results to optimize system performance";
        }
    }
    
    double estimate_optimization_improvement(const AnalysisResult& result) {
        // Estimate optimization improvement based on analysis results
        double improvement = result.overall_score * 0.5;  // 50% of analysis score translates to improvement
        return std::min(100.0, improvement);
    }
    
    std::string assess_implementation_difficulty(const AnalysisResult& result) {
        // Assess implementation difficulty based on analysis results
        if (result.analysis_type == "data_analysis") {
            return "low";
        } else if (result.analysis_type == "trend_analysis") {
            return "medium";
        } else if (result.analysis_type == "correlation_analysis") {
            return "medium";
        } else if (result.analysis_type == "prediction_analysis") {
            return "high";
        } else {
            return "medium";
        }
    }
    
    std::vector<std::string> generate_required_steps(const AnalysisResult& result) {
        std::vector<std::string> steps;
        
        if (result.analysis_type == "data_analysis") {
            steps.push_back("Validate data quality metrics");
            steps.push_back("Fix data collection issues");
            steps.push_back("Implement data validation");
        } else if (result.analysis_type == "trend_analysis") {
            steps.push_back("Implement trend monitoring");
            steps.push_back("Set up alerts for significant changes");
            steps.push_back("Create predictive models");
        } else if (result.analysis_type == "correlation_analysis") {
            steps.push_back("Analyze system parameters");
            steps.push_back("Identify optimization opportunities");
            steps.push_back("Implement parameter tuning");
        } else if (result.analysis_type == "prediction_analysis") {
            steps.push_back("Implement prediction models");
            steps.push_back("Set up performance forecasting");
            steps.push_back("Create optimization triggers");
        } else {
            steps.push_back("Analyze results");
            steps.push_back("Identify improvements");
            steps.push_back("Implement optimizations");
        }
        
        return steps;
    }
    
    std::string generate_prerequisites(const AnalysisResult& result) {
        std::stringstream ss;
        ss << "Prerequisites for " << result.analysis_type << " analysis:";
        
        if (result.analysis_type == "data_analysis") {
            ss << " data collection tools, data validation";
        } else if (result.analysis_type == "trend_analysis") {
            ss << " time-series analysis tools, historical data";
        } else if (result.analysis_type == "correlation_analysis") {
            ss << " statistical analysis tools, correlation analysis libraries";
        } else if (result.analysis_type == "prediction_analysis") {
            ss << " machine learning libraries, prediction algorithms";
        } else {
            ss << " analysis frameworks, analysis tools";
        }
        
        return ss.str();
    }
    
    static uint64_t getCurrentTimestamp() {
        auto now = std::chrono::high_resolution_clock::now();
        auto duration = now.time_since_epoch();
        return std::chrono::duration_cast<std::chrono::nanoseconds>(duration).count();
    }
};

// Global analysis framework instance
static std::unique_ptr<AnalysisFramework> global_analysis_framework;

// Convenience functions
namespace nlm {
namespace analysis {

inline AnalysisFramework& getGlobalAnalysisFramework() {
    if (!global_analysis_framework) {
        global_analysis_framework = std::make_unique<AnalysisFramework>();
    }
    return *global_analysis_framework;
}

inline void add_data_analysis(const PerformanceDataset& dataset) {
    getGlobalAnalysisFramework().add_analysis(std::make_unique<DataAnalysis>(dataset));
}

inline void add_trend_analysis(const PerformanceDataset& dataset) {
    getGlobalAnalysisFramework().add_analysis(std::make_unique<TrendAnalysis>(dataset));
}

inline AnalysisResult execute_analysis(const std::string& analysis_name) {
    return getGlobalAnalysisFramework().execute_analysis(analysis_name);
}

inline std::vector<AnalysisResult> execute_all_analyses() {
    return getGlobalAnalysisFramework().execute_all_analyses();
}

inline void print_analysis_summary() {
    getGlobalAnalysisFramework().print_analysis_summary();
}

inline void print_analysis_history() {
    getGlobalAnalysisFramework().print_analysis_history();
}

inline std::vector<OptimizationRecommendation> generate_optimization_recommendations_from_analysis(
    const std::vector<AnalysisResult>& analysis_results) {
    return getGlobalAnalysisFramework().generate_optimization_recommendations_from_analysis(analysis_results);
}

} // namespace analysis
} // namespace nlm

// Example usage and demonstration
void demonstrate_analysis() {
    std::cout << "=== NLM Analysis Demonstration ===" << std::endl;
    
    // Create performance datasets
    PerformanceDataset cpu_dataset;
    cpu_dataset.dataset_name = "cpu_usage";
    cpu_dataset.dataset_type = "metrics";
    cpu_dataset.values = {10.0, 25.0, 45.0, 65.0, 80.0, 75.0, 60.0, 45.0, 30.0, 20.0};
    cpu_dataset.timestamps = {
        1000000000, 2000000000, 3000000000, 4000000000, 5000000000,
        6000000000, 7000000000, 8000000000, 9000000000, 10000000000
    };
    cpu_dataset.sampling_interval = std::chrono::nanoseconds(1000000000);  // 1 second
    cpu_dataset.start_timestamp_ns = 1000000000;
    cpu_dataset.end_timestamp_ns = 10000000000;
    cpu_dataset.metadata = {{{"unit", "percent"}}, {{"source", "cpu_monitor"}}};
    
    // Add data analysis
    nlm::analysis::add_data_analysis(cpu_dataset);
    
    // Create another dataset for trend analysis
    PerformanceDataset memory_dataset;
    memory_dataset.dataset_name = "memory_usage";
    memory_dataset.dataset_type = "metrics";
    memory_dataset.values = {512, 768, 1024, 1280, 1024, 768, 512, 256, 128, 64};
    memory_dataset.timestamps = cpu_dataset.timestamps;
    memory_dataset.sampling_interval = cpu_dataset.sampling_interval;
    memory_dataset.start_timestamp_ns = cpu_dataset.start_timestamp_ns;
    memory_dataset.end_timestamp_ns = cpu_dataset.end_timestamp_ns;
    memory_dataset.metadata = {{{"unit", "megabytes"}}, {{"source", "memory_monitor"}}};
    
    // Add multidimensional data
    memory_dataset.multidimensional_data = {
        {"cpu_usage", cpu_dataset.values}
    };
    
    // Add trend analysis
    nlm::analysis::add_trend_analysis(memory_dataset);
    
    // Execute analyses
    std::cout << "Executing analyses..." << std::endl;
    AnalysisResult cpu_analysis = nlm::analysis::execute_analysis("cpu_usage");
    AnalysisResult memory_analysis = nlm::analysis::execute_analysis("memory_usage");
    
    // Print results
    std::cout << "Analysis Results:" << std::endl;
    std::cout << "  CPU analysis: " << cpu_analysis.overall_score << "/100, " << cpu_analysis.findings.size() << " findings" << std::endl;
    std::cout << "  Memory analysis: " << memory_analysis.overall_score << "/100, " << memory_analysis.findings.size() << " findings" << std::endl;
    
    // Generate optimization recommendations
    std::vector<AnalysisResult> all_analyses = {cpu_analysis, memory_analysis};
    std::vector<OptimizationRecommendation> recommendations = 
        nlm::analysis::generate_optimization_recommendations_from_analysis(all_analyses);
    
    // Print recommendations
    std::cout << "\nOptimization Recommendations:" << std::endl;
    for (const auto& recommendation : recommendations) {
        std::cout << "  " << recommendation.recommendation_id << ": " << recommendation.recommendation << std::endl;
    }
    
    // Print summary
    nlm::analysis::print_analysis_summary();
}

// Command-line interface for analysis
void analysis_cli(int argc, char* argv[]) {
    std::cout << "NLM Analysis CLI" << std::endl;
    std::cout << "Usage: nlm_analysis [options]" << std::endl;
    std::cout << "Options:" << std::endl;
    std::cout << "  --help                    Show this help message" << std::endl;
    std::cout << "  --demo                    Run analysis demonstration" << std::endl;
    std::cout << "  --execute <name>          Execute a specific analysis" << std::endl;
    std::cout << "  --execute-all             Execute all analyses" << std::endl;
    std::cout << "  --history                 Print analysis history" << std::endl;
    std::cout << "  --recommendations         Generate optimization recommendations" << std::endl;
    
    // Simple argument parsing (in real implementation would use proper parsing)
    if (argc > 1) {
        std::string arg = argv[1];
        if (arg == "--demo") {
            demonstrate_analysis();
        }
    }
}