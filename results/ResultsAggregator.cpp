// ResultsAggregator.cpp - Result aggregation and analysis implementation

#include "results/ResultsAggregator.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <cmath>
#include <random>
#include <numeric>

namespace nlm {

ResultsAggregator::ResultsAggregator()
    : confidenceLevel_(0.95) {
    NLM_LOG_INFO("Initializing Results Aggregator...");
    
    // Set default metrics to compute
    metricsToCompute_ = {"mean", "variance", "stddev", "median", "mode", "skewness", "kurtosis"};
    
    NLM_LOG_INFO("Results Aggregator initialized successfully");
}

ResultsAggregator::~ResultsAggregator() = default;

std::map<std::string, double> ResultsAggregator::aggregateResults(const std::map<std::string, std::string>& batchResults) {
    NLM_LOG_INFO("Aggregating " + std::to_string(batchResults.size()) + " batch results...");
    
    aggregatedResults_.clear();
    
    // Parse string results to extract numeric values
    for (const auto& result : batchResults) {
        // Extract numeric value from result string
        double value = parseResultToDouble(result.second);
        aggregatedResults_[result.first] = value;
    }
    
    NLM_LOG_INFO("Results aggregation completed");
    return aggregatedResults_;
}

std::map<std::string, double> ResultsAggregator::statisticalAnalysis(const std::vector<double>& results,
                                                              const std::vector<std::string>& metricsToCompute) {
    NLM_LOG_INFO("Performing statistical analysis on " + std::to_string(results.size()) + " results...");
    
    statisticalResults_.clear();n    
    std::vector<std::string> metrics = metricsToCompute.empty() ? metricsToCompute_ : metricsToCompute;
    
    for (const auto& metric : metrics) {
        if (metric == "mean") {
            statisticalResults_["mean"] = calculateMean(results);
        } else if (metric == "variance") {
            double mean = calculateMean(results);
            statisticalResults_["variance"] = calculateVariance(results, mean);
        } else if (metric == "stddev") {
            double mean = calculateMean(results);
            statisticalResults_["stddev"] = calculateStandardDeviation(results, mean);
        } else if (metric == "median") {
            statisticalResults_["median"] = calculateMedian(results);
        } else if (metric == "mode") {
            statisticalResults_["mode"] = calculateMode(results);
        } else if (metric == "skewness") {
            double mean = calculateMean(results);
            double stddev = calculateStandardDeviation(results, mean);
            if (stddev > 0) {
                statisticalResults_["skewness"] = calculateSkewness(results, mean, stddev);
            }
        } else if (metric == "kurtosis") {
            double mean = calculateMean(results);
            double stddev = calculateStandardDeviation(results, mean);
            if (stddev > 0) {
                statisticalResults_["kurtosis"] = calculateKurtosis(results, mean, stddev);
            }
        } else if (metric == "min") {
            statisticalResults_["min"] = *std::min_element(results.begin(), results.end());
        } else if (metric == "max") {
            statisticalResults_["max"] = *std::max_element(results.begin(), results.end());
        } else if (metric == "range") {
            if (!results.empty()) {
                double min = *std::min_element(results.begin(), results.end());
                double max = *std::max_element(results.begin(), results.end());
                statisticalResults_["range"] = max - min;
            }
        } else if (metric == "interquartile_range") {
            statisticalResults_["interquartile_range"] = calculateInterquartileRange(results);
        }
    }
    
    NLM_LOG_INFO("Statistical analysis completed");
    return statisticalResults_;
}

std::map<std::string, std::string> ResultsAggregator::classifyResults(const std::map<std::string, double>& results,
                                                             const std::map<std::string, double>& thresholds) {
    NLM_LOG_INFO("Classifying " + std::to_string(results.size()) + " results...");
    
    classificationResults_.clear();
    
    for (const auto& result : results) {
        std::string classification = classifyValue(result.second, result.first);
        classificationResults_[result.first] = classification;
    }
    
    NLM_LOG_INFO("Result classification completed");
    return classificationResults_;
}

std::map<std::string, double> ResultsAggregator::filterResults(const std::map<std::string, double>& results,
                                             const std::map<std::string, std::string>& filterCriteria) {
    NLM_LOG_INFO("Filtering results based on criteria...");
    
    std::map<std::string, double> filteredResults;
    
    for (const auto& result : results) {
        if (passesFilterCriteria(result, filterCriteria)) {
            filteredResults[result.first] = result.second;
        }
    }
    
    NLM_LOG_INFO("Result filtering completed, " + std::to_string(filteredResults.size()) + " results remaining");
    return filteredResults;
}

std::map<std::string, double> ResultsAggregator::correlationAnalysis(const std::vector<double>& metric1,
                                                     const std::vector<double>& metric2) {
    NLM_LOG_INFO("Performing correlation analysis...");
    
    correlationResults_.clear();
    
    if (metric1.size() != metric2.size() || metric1.empty()) {
        NLM_LOG_WARNING("Invalid data for correlation analysis");
        return correlationResults_;
    }
    
    correlationResults_["pearson"] = pearsonCorrelation(metric1, metric2);
    correlationResults_["spearman"] = spearmanCorrelation(metric1, metric2);
    
    // Calculate p-value using permutation test (simplified)
    double pValue = calculateCorrelationPValue(metric1, metric2);
    correlationResults_["p_value"] = pValue;
    
    // Determine statistical significance
    correlationResults_["significant"] = (pValue < 0.05) ? 1.0 : 0.0;
    
    NLM_LOG_INFO("Correlation analysis completed");
    return correlationResults_;
}

std::map<std::string, double> ResultsAggregator::compareToBaseline(const std::map<std::string, double>& results,
                                                    const std::map<std::string, double>& baseline,
                                                    const std::vector<std::string>& comparisonMetrics) {
    NLM_LOG_INFO("Comparing results to baseline...");
    
    std::map<std::string, double> comparisonResults;
    
    for (const auto& result : results) {
        std::string key = result.first;
        
        if (baseline.find(key) != baseline.end()) {
            double current = result.second;
            double baselineValue = baseline.at(key);
            
            // Calculate absolute difference
            double absDiff = std::abs(current - baselineValue);
            comparisonResults[key + "_abs_diff"] = absDiff;
            
            // Calculate relative difference
            double relDiff = (baselineValue != 0) ? (current - baselineValue) / baselineValue : 0.0;
            comparisonResults[key + "_rel_diff"] = relDiff;
            
            // Determine if change is significant (> 5%)
            comparisonResults[key + "_significant"] = (std::abs(relDiff) > 0.05) ? 1.0 : 0.0;
        }
    }
    
    NLM_LOG_INFO("Baseline comparison completed");
    return comparisonResults;
}

std::map<std::string, double> ResultsAggregator::confidenceIntervals(const std::vector<double>& results,
                                                  double confidenceLevel) {
    NLM_LOG_INFO("Calculating confidence intervals...");
    
    std::map<std::string, double> intervalResults;
    
    if (results.empty()) {
        return intervalResults;
    }
    
    double alpha = 1.0 - confidenceLevel;
    
    // Calculate mean and standard deviation
    double mean = calculateMean(results);
    double stddev = calculateStandardDeviation(results, mean);
    double n = static_cast<double>(results.size());
    
    if (n > 1 && stddev > 0) {
        double tValue = calculateTValue(confidenceLevel, n - 1);
        
        // Mean confidence interval
        double margin = tValue * stddev / std::sqrt(n);
        intervalResults["mean_ci_lower"] = mean - margin;
        intervalResults["mean_ci_upper"] = mean + margin;
        
        // Percentile intervals
        double lowerPercentile = (alpha / 2.0) * 100.0;
        double upperPercentile = (1.0 - alpha / 2.0) * 100.0;
        
        std::vector<double> sortedResults = results;
        std::sort(sortedResults.begin(), sortedResults.end());
        
        intervalResults["percentile_ci_lower"] = sortedResults[static_cast<size_t>(lowerPercentile / 100.0 * n)];
        intervalResults["percentile_ci_upper"] = sortedResults[static_cast<size_t>(upperPercentile / 100.0 * n)];
    }
    
    NLM_LOG_INFO("Confidence interval calculation completed");
    return intervalResults;
}

std::string ResultsAggregator::prepareVisualizationData(const std::map<std::string, double>& results,
                                        const std::string& format) {
    NLM_LOG_INFO("Preparing visualization data in " + format + " format...");
    
    if (format == "csv") {
        return formatAsCSV(results);
    } else if (format == "json") {
        return formatAsJSON(results);
    } else {
        NLM_LOG_WARNING("Unsupported format: " + format + ", using CSV");
        return formatAsCSV(results);
    }
}

std::map<std::string, double> ResultsAggregator::calculatePerformanceMetrics(const std::vector<double>& executionTimes,
                                                              const std::vector<double>& memoryUsageVector) {
    NLM_LOG_INFO("Calculating performance metrics...");
    
    std::map<std::string, double> performanceMetrics;
    
    if (executionTimes.empty()) {
        return performanceMetrics;
    }
    
    // Throughput (operations per second)
    double totalTime = std::accumulate(executionTimes.begin(), executionTimes.end(), 0.0);
    if (totalTime > 0) {
        performanceMetrics["throughput"] = executionTimes.size() / totalTime;
    }
    
    // Latency (average execution time)
    performanceMetrics["latency_mean"] = calculateMean(executionTimes);
    performanceMetrics["latency_stddev"] = calculateStandardDeviation(executionTimes, performanceMetrics["latency_mean"]);
    
    // Resource efficiency
    if (!memoryUsageVector.empty() && !executionTimes.empty()) {
        double totalMemory = std::accumulate(memoryUsageVector.begin(), memoryUsageVector.end(), 0.0);
        performanceMetrics["memory_efficiency"] = memoryUsageVector.size() / totalMemory;
    }
    
    // Scalability metrics (time per operation)
    double timePerOp = totalTime / executionTimes.size();
    performanceMetrics["time_per_operation"] = timePerOp;
    
    NLM_LOG_INFO("Performance metrics calculation completed");
    return performanceMetrics;
}

std::map<std::string, double> ResultsAggregator::analyzeTrends(const std::vector<std::pair<std::chrono::steady_clock::time_point, double>>& timeSeriesResults) {
    NLM_LOG_INFO("Analyzing trends...");
    
    std::map<std::string, double> trendResults;
    
    if (timeSeriesResults.size() < 2) {
        return trendResults;
    }
    
    // Calculate time differences and value changes
    std::vector<double> timeDiffs;
    std::vector<double> valueDiffs;
    
    auto prevTime = timeSeriesResults[0].first;
    double prevValue = timeSeriesResults[0].second;
    
    for (size_t i = 1; i < timeSeriesResults.size(); ++i) {
        auto currentTime = timeSeriesResults[i].first;
        double currentValue = timeSeriesResults[i].second;
        
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - prevTime);
        timeDiffs.push_back(duration.count() / 1000.0); // Convert to seconds
        
        valueDiffs.push_back(currentValue - prevValue);
        
        prevTime = currentTime;
        prevValue = currentValue;
    }
    
    // Linear regression trend
    if (!timeDiffs.empty()) {
        double slope = calculateLinearRegressionSlope(timeDiffs, valueDiffs);
        trendResults["trend_slope"] = slope;
        
        // Trend strength (R-squared)
        double rSquared = calculateRSquared(timeDiffs, valueDiffs, slope);
        trendResults["trend_strength"] = rSquared;
        
        // Trend direction
        if (std::abs(slope) < 0.01) {
            trendResults["trend_direction"] = 0.0; // Flat
        } else if (slope > 0) {
            trendResults["trend_direction"] = 1.0; // Increasing
        } else {
            trendResults["trend_direction"] = -1.0; // Decreasing
        }
    }
    
    NLM_LOG_INFO("Trend analysis completed");
    return trendResults;
}

std::string ResultsAggregator::generateReport(const std::map<std::string, double>& aggregatedResults) {
    std::ostringstream oss;
    oss << "=== NLM Results Aggregator Report ===\n\n";
    
    oss << "Aggregated Results:\n";
    for (const auto& result : aggregatedResults) {
        oss << "  " << result.first << ": " << std::fixed << std::setprecision(6) << result.second << "\n";
    }
    oss << "\n";
    
    oss << "Statistical Summary:\n";
    for (const auto& stat : statisticalResults_) {
        oss << "  " << stat.first << ": " << std::fixed << std::setprecision(6) << stat.second << "\n";
    }
    oss << "\n";
    
    oss << "Classification Summary:\n";
    std::unordered_map<std::string, int> classificationCounts;
    for (const auto& classification : classificationResults_) {
        classificationCounts[classification.second]++;
    }
    
    for (const auto& pair : classificationCounts) {
        oss << "  " << pair.first << ": " << pair.second << "\n";
    }
    oss << "\n";
    
    oss << "Correlation Analysis:\n";
    for (const auto& correlation : correlationResults_) {
        oss << "  " << correlation.first << ": " << std::fixed << std::setprecision(6) << correlation.second << "\n";
    }
    
    return oss.str();
}

// Helper methods

// Parse string result to double
double ResultsAggregator::parseResultToDouble(const std::string& result) const {
    try {
        // Try to extract numeric value from result string
        std::istringstream iss(result);
        double value;
        if (iss >> value) {
            return value;
        }
        
        // Look for numbers in the string
        for (char c : result) {
            if (std::isdigit(c) || c == '.' || c == '-') {
                // Found a number, parse it
                size_t pos = result.find(c);
                std::string numberStr = result.substr(pos);
                std::istringstream niss(numberStr);
                if (niss >> value) {
                    return value;
                }
            }
        }
        
        // Default value
        return 0.0;
    } catch (...) {
        return 0.0;
    }
}

// Calculate mean
double ResultsAggregator::calculateMean(const std::vector<double>& values) const {
    if (values.empty()) return 0.0;
    return std::accumulate(values.begin(), values.end(), 0.0) / values.size();
}

// Calculate variance
double ResultsAggregator::calculateVariance(const std::vector<double>& values, double mean) const {
    if (values.empty()) return 0.0;
    
    double sum = 0.0;
    for (double value : values) {
        double diff = value - mean;
        sum += diff * diff;
    }
    return sum / values.size();
}

// Calculate standard deviation
double ResultsAggregator::calculateStandardDeviation(const std::vector<double>& values, double mean) const {
    return std::sqrt(calculateVariance(values, mean));
}

// Calculate median
double ResultsAggregator::calculateMedian(const std::vector<double>& values) const {
    if (values.empty()) return 0.0;
    
    std::vector<double> sortedValues = values;
    std::sort(sortedValues.begin(), sortedValues.end());
    
    size_t n = sortedValues.size();
    if (n % 2 == 0) {
        return (sortedValues[n/2 - 1] + sortedValues[n/2]) / 2.0;
    } else {
        return sortedValues[n/2];
    }
}

// Calculate mode
double ResultsAggregator::calculateMode(const std::vector<double>& values) const {
    if (values.empty()) return 0.0;
    
    std::unordered_map<double, int> frequencyMap;
    for (double value : values) {
        frequencyMap[value]++;
    }
    
    double mode = values[0];
    int maxCount = 0;
    
    for (const auto& pair : frequencyMap) {
        if (pair.second > maxCount) {
            mode = pair.first;
            maxCount = pair.second;
        }
    }
    
    return mode;
}

// Calculate skewness
double ResultsAggregator::calculateSkewness(const std::vector<double>& values, double mean, double stddev) const {
    if (values.empty() || stddev == 0) return 0.0;
    
    double n = static_cast<double>(values.size());
    double sum = 0.0;
    
    for (double value : values) {
        double diff = value - mean;
        sum += (diff * diff * diff);
    }
    
    double skewness = (n / ((n - 1) * (n - 2))) * (sum / stddev * stddev * stddev);
    return skewness;
}

// Calculate kurtosis
double ResultsAggregator::calculateKurtosis(const std::vector<double>& values, double mean, double stddev) const {
    if (values.empty() || stddev == 0) return 0.0;
    
    double n = static_cast<double>(values.size());
    double sum = 0.0;
    
    for (double value : values) {
        double diff = value - mean;
        sum += (diff * diff * diff * diff);
    }
    
    double kurtosis = (n * (n + 1)) / ((n - 1) * (n - 2) * (n - 3)) * (sum / (stddev * stddev * stddev * stddev)) - 
                      (3 * (n - 1) * (n - 1)) / ((n - 2) * (n - 3));
    return kurtosis;
}

// Calculate interquartile range
double ResultsAggregator::calculateInterquartileRange(const std::vector<double>& values) const {
    if (values.empty()) return 0.0;
    
    std::vector<double> sortedValues = values;
    std::sort(sortedValues.begin(), sortedValues.end());
    
    size_t n = sortedValues.size();
    size_t q1Index = n / 4;
    size_t q3Index = (3 * n) / 4;
    
    double q1 = sortedValues[q1Index];
    double q3 = sortedValues[q3Index];
    
    return q3 - q1;
}

// Classify value
double ResultsAggregator::calculateLinearRegressionSlope(const std::vector<double>& x, const std::vector<double>& y) const {
    if (x.size() != y.size() || x.size() < 2) return 0.0;
    
    double n = static_cast<double>(x.size());
    double sumX = std::accumulate(x.begin(), x.end(), 0.0);
    double sumY = std::accumulate(y.begin(), y.end(), 0.0);
    double sumXY = 0.0;
    double sumX2 = 0.0;
    
    for (size_t i = 0; i < x.size(); ++i) {
        sumXY += x[i] * y[i];
        sumX2 += x[i] * x[i];
    }
    
    double denominator = n * sumX2 - sumX * sumX;
    if (denominator == 0) return 0.0;
    
    return (n * sumXY - sumX * sumY) / denominator;
}

double ResultsAggregator::calculateRSquared(const std::vector<double>& x, const std::vector<double>& y,
                                          double slope) const {
    if (x.size() != y.size() || x.size() < 2) return 0.0;
    
    double meanY = calculateMean(y);
    double sumSquaredErrors = 0.0;
    double sumTotal = 0.0;
    
    for (size_t i = 0; i < y.size(); ++i) {
        double predictedY = slope * x[i] + (meanY - slope * calculateMean(x));
        sumSquaredErrors += (y[i] - predictedY) * (y[i] - predictedY);
        sumTotal += (y[i] - meanY) * (y[i] - meanY);
    }
    
    if (sumTotal == 0) return 0.0;
    
    return 1.0 - (sumSquaredErrors / sumTotal);
}

std::string ResultsAggregator::formatAsCSV(const std::map<std::string, double>& data) const {
    std::ostringstream oss;
    oss << "Key,Value\n";
    
    for (const auto& pair : data) {
        oss << pair.first << "," << std::fixed << std::setprecision(6) << pair.second << "\n";
    }
    
    return oss.str();
}

std::string ResultsAggregator::formatAsJSON(const std::map<std::string, double>& data) const {
    std::ostringstream oss;
    oss << "{\n";
    
    bool first = true;
    for (const auto& pair : data) {
        if (!first) oss << ",\n";
        oss << "  \"" << pair.first << "\": " << std::fixed << std::setprecision(6) << pair.second;
        first = false;
    }
    oss << "\n}\n";
    
    return oss.str();
}

double ResultsAggregator::pearsonCorrelation(const std::vector<double>& x, const std::vector<double>& y) const {
    if (x.size() != y.size() || x.size() < 2) return 0.0;
    
    double meanX = calculateMean(x);
    double meanY = calculateMean(y);
    double stddevX = calculateStandardDeviation(x, meanX);
    double stddevY = calculateStandardDeviation(y, meanY);
    
    if (stddevX == 0 || stddevY == 0) return 0.0;
    
    double covariance = 0.0;
    for (size_t i = 0; i < x.size(); ++i) {
        covariance += (x[i] - meanX) * (y[i] - meanY);
    }
    covariance /= x.size();
    
    return covariance / (stddevX * stddevY);
}

double ResultsAggregator::spearmanCorrelation(const std::vector<double>& x, const std::vector<double>& y) const {
    if (x.size() != y.size() || x.size() < 2) return 0.0;
    
    // Rank the data
    std::vector<size_t> rankX = rankData(x);
    std::vector<size_t> rankY = rankData(y);
    
    return pearsonCorrelation(
        convertToDoubleVector(rankX),
        convertToDoubleVector(rankY)
    );
}

double ResultsAggregator::calculateCorrelationPValue(const std::vector<double>& x, const std::vector<double>& y) const {
    if (x.size() != y.size() || x.size() < 2) return 1.0;
    
    double correlation = pearsonCorrelation(x, y);
    size_t n = x.size();
    
    // Calculate t-statistic
    double t = correlation * std::sqrt((n - 2) / (1 - correlation * correlation));
    
    // Calculate p-value using approximation
    double pValue = 2 * (1.0 - 0.5 * (1.0 + std::erf(std::abs(t) / std::sqrt(2.0))));
    
    return pValue;
}

double ResultsAggregator::calculateTValue(double confidenceLevel, size_t degreesOfFreedom) const {
    // Simplified t-value calculation (for degrees of freedom > 30)
    if (degreesOfFreedom <= 0) return 1.96; // Normal approximation
    
    // Use normal approximation for simplicity
    return 1.96; // 95% confidence level
}
double ResultsAggregator::calculateLinearRegressionSlope(const std::vector<double>& x, const std::vector<double>& y) const {
    if (x.size() != y.size() || x.size() < 2) return 0.0;
    
    double n = static_cast<double>(x.size());
    double sumX = std::accumulate(x.begin(), x.end(), 0.0);
    double sumY = std::accumulate(y.begin(), y.end(), 0.0);
    double sumXY = 0.0;
    double sumX2 = 0.0;
    
    for (size_t i = 0; i < x.size(); ++i) {
        sumXY += x[i] * y[i];
        sumX2 += x[i] * x[i];
    }
    
    double denominator = n * sumX2 - sumX * sumX;
    if (denominator == 0) return 0.0;
    
    return (n * sumXY - sumX * sumY) / denominator;
}
double ResultsAggregator::calculateRSquared(const std::vector<double>& x, const std::vector<double>& y,
                                          double slope) const {
    if (x.size() != y.size() || x.size() < 2) return 0.0;
    
    double meanY = calculateMean(y);
    double sumSquaredErrors = 0.0;
    double sumTotal = 0.0;
    
    for (size_t i = 0; i < y.size(); ++i) {
        double predictedY = slope * x[i] + (meanY - slope * calculateMean(x));
        sumSquaredErrors += (y[i] - predictedY) * (y[i] - predictedY);
        sumTotal += (y[i] - meanY) * (y[i] - meanY);
    }
    
    if (sumTotal == 0) return 0.0;
    
    return 1.0 - (sumSquaredErrors / sumTotal);
}

bool ResultsAggregator::passesFilterCriteria(const std::pair<std::string, double>& result,
                                            const std::map<std::string, std::string>& filterCriteria) const {
    for (const auto& criterion : filterCriteria) {
        std::string key = criterion.first;
        std::string value = criterion.second;
        
        if (key == "greater_than") {
            double threshold = std::stod(value);
            if (result.second <= threshold) return false;
        } else if (key == "less_than") {
            double threshold = std::stod(value);
            if (result.second >= threshold) return false;
        } else if (key == "range") {
            // Parse range (e.g., "10-20")
            size_t dashPos = value.find('-');
            if (dashPos != std::string::npos) {
                double minVal = std::stod(value.substr(0, dashPos));
                double maxVal = std::stod(value.substr(dashPos + 1));
                if (result.second < minVal || result.second > maxVal) return false;
            }
        } else if (key == "contains") {
            // String matching (not applicable for double values)
            return false;
        }
    }
    
    return true;
}

std::string ResultsAggregator::classifyValue(double value, const std::string& metricName) const {
    if (metricName.find("time") != std::string::npos || metricName.find("duration") != std::string::npos) {
        // Time-related metrics: faster is better
        if (value < 100) return "fast";
        else if (value < 500) return "normal";
        else if (value < 1000) return "slow";
        else return "very_slow";
    } else if (metricName.find("accuracy") != std::string::npos || metricName.find("precision") != std::string::npos) {
        // Accuracy metrics: higher is better
        if (value > 0.95) return "excellent";
        else if (value > 0.90) return "good";
        else if (value > 0.80) return "acceptable";
        else if (value > 0.70) return "poor";
        else return "very_poor";
    } else {
        // General metrics: use absolute thresholds
        if (value < 1) return "low";
        else if (value < 10) return "medium";
        else if (value < 100) return "high";
        else return "very_high";
    }
}

std::vector<size_t> ResultsAggregator::rankData(const std::vector<double>& data) const {
    if (data.empty()) return {};
    
    std::vector<std::pair<double, size_t>> indexedData;
    for (size_t i = 0; i < data.size(); ++i) {
        indexedData.emplace_back(data[i], i);
    }
    
    std::sort(indexedData.begin(), indexedData.end(),
              [](const auto& a, const auto& b) { return a.first < b.first; });
    
    std::vector<size_t> ranks(data.size(), 0);
    size_t currentRank = 0;
    
    for (size_t i = 0; i < indexedData.size(); ++i) {
        if (i > 0 && indexedData[i].first != indexedData[i-1].first) {
            currentRank = i;
        }
        ranks[indexedData[i].second] = currentRank;
    }
    
    return ranks;
}

std::vector<double> ResultsAggregator::convertToDoubleVector(const std::vector<size_t>& data) const {
    std::vector<double> result;
    result.reserve(data.size());
    for (size_t value : data) {
        result.push_back(static_cast<double>(value));
    }
    return result;
}

} // namespace nlm