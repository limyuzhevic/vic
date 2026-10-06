#include "PerformanceAnalyzer.hpp"
#include <nlohmann/json.hpp>
#include <algorithm>
#include <numeric>
#include <cmath>
#include <sstream>
#include <iomanip>

using json = nlohmann::json;

namespace nlm {

PerformanceAnalyzer::PerformanceAnalyzer(const AnalysisConfig& config) : config_(config) {}

std::vector<AnalysisResult> PerformanceAnalyzer::analyze(const std::vector<MetricData>& metrics) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    std::vector<AnalysisResult> results;
    
    // Group metrics by name
    std::map<std::string, std::vector<double>> metricValues;
    for (const auto& metric : metrics) {
        metricValues[metric.name].push_back(metric.value);
    }
    
    // Analyze each metric
    for (const auto& pair : metricValues) {
        const auto& name = pair.first;
        const auto& values = pair.second;
        
        if (values.empty()) {
            continue;
        }
        
        AnalysisResult result(name);
        
        // Calculate basic statistics
        result.mean = calculateMean(values);
        result.median = calculateMedian(values);
        result.variance = calculateVariance(values, result.mean);
        result.standardDeviation = std::sqrt(result.variance);
        result.min = *std::min_element(values.begin(), values.end());
        result.max = *std::max_element(values.begin(), values.end());
        result.range = result.max - result.min;
        
        // Calculate higher-order statistics
        if (result.standardDeviation > 0.0) {
            result.skew = calculateSkew(values, result.mean, result.standardDeviation);
            result.kurtosis = calculateKurtosis(values, result.mean, result.standardDeviation);
        }
        
        result.analysisTime = std::chrono::system_clock::now();
        
        results.push_back(result);
        
        // Cache the result
        analysisCache_[name] = result;
    }
    
    return results;
}

std::vector<PerformanceTrend> PerformanceAnalyzer::analyzeTrends(const std::vector<MetricData>& metrics) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    std::vector<PerformanceTrend> trends;
    
    // Group metrics by name and sort by time
    std::map<std::string, std::vector<std::pair<std::chrono::system_clock::time_point, double>>> timeSeries;
    for (const auto& metric : metrics) {
        timeSeries[metric.name].emplace_back(metric.timestamp, metric.value);
    }
    
    // Analyze trends for each metric
    for (auto& pair : timeSeries) {
        auto& name = pair.first;
        auto& dataPoints = pair.second;
        
        // Sort by time
        std::sort(dataPoints.begin(), dataPoints.end(),
                  [](const auto& a, const auto& b) { return a.first < b.second; });
        
        // Extract values for trend analysis
        std::vector<double> values;
        std::vector<double> timePoints;
        
        for (size_t i = std::max(static_cast<size_t>(config_.trendAnalysisWindow), dataPoints.size()) - config_.trendAnalysisWindow;
             i < dataPoints.size(); ++i) {
            values.push_back(dataPoints[i].second);
            auto timeSinceStart = std::chrono::duration<double>(dataPoints[i].first - dataPoints.front().first).count();
            timePoints.push_back(timeSinceStart);
        }
        
        if (values.size() < 2) {
            continue; // Not enough data for trend analysis
        }
        
        // Calculate trend
        double trend = calculateTrend(values);
        std::string direction = determineTrendDirection(trend);
        double rSquared = calculateRSquared(timePoints, values);
        
        PerformanceTrend trendResult(name);
        trendResult.trendValue = trend;
        trendResult.trendDirection = direction;
        trendResult.confidence = std::min(rSquared, 1.0); // Clamp confidence to [0, 1]
        trendResult.rSquared = rSquared;
        trendResult.trendTime = std::chrono::system_clock::now();
        
        trends.push_back(trendResult);
        
        // Cache the trend
        trendCache_[name] = trendResult;
    }
    
    return trends;
}

std::vector<AnomalyResult> PerformanceAnalyzer::detectAnomalies(const std::vector<MetricData>& metrics) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    std::vector<AnomalyResult> anomalies;
    
    // Group metrics by name
    std::map<std::string, std::vector<double>> metricValues;
    for (const auto& metric : metrics) {
        metricValues[metric.name].push_back(metric.value);
    }
    
    // Detect anomalies for each metric
    for (const auto& pair : metricValues) {
        const auto& name = pair.first;
        const auto& values = pair.second;
        
        if (values.size() < 3) {
            continue; // Not enough data for anomaly detection
        }
        
        double mean = calculateMean(values);
        double stdDev = calculateStandardDeviation(values, mean);
        
        if (stdDev <= 0.0) {
            continue; // No variation
        }
        
        // Check for outliers
        for (size_t i = 0; i < values.size(); ++i) {
            if (isOutlier(values[i], mean, stdDev, config_.anomalyThreshold)) {
                AnomalyResult anomaly(name);
                anomaly.anomalousValue = values[i];
                anomaly.expectedValue = mean;
                anomaly.deviation = std::abs(values[i] - mean) / stdDev;
                
                // Determine anomaly type based on deviation
                if (anomaly.deviation > 3.0) {
                    anomaly.anomalyType = "spike";
                } else if (anomaly.deviation > 2.0) {
                    anomaly.anomalyType = "drop";
                } else {
                    anomaly.anomalyType = "outlier";
                }
                
                anomaly.severity = std::min(anomaly.deviation / 5.0, 1.0); // Normalize to [0, 1]
                anomaly.detectionTime = std::chrono::system_clock::now();
                
                anomalies.push_back(anomaly);
            }
        }
    }
    
    return anomalies;
}

std::string PerformanceAnalyzer::generateReport(const std::vector<MetricData>& metrics,
                                                 const std::string& format) const {
    if (format == "json" || format == "auto") {
        json report;
        
        // Add metadata
        report["timestamp"] = std::chrono::system_clock::now();
        report["metricCount"] = metrics.size();
        
        // Add analysis results
        auto analysis = const_cast<PerformanceAnalyzer*>(this)->analyze(metrics);
        
        json analysisArray = json::array();
        for (const auto& result : analysis) {
            json resultJson;
            resultJson["metricName"] = result.metricName;
            resultJson["mean"] = result.mean;
            resultJson["median"] = result.median;
            resultJson["standardDeviation"] = result.standardDeviation;
            resultJson["variance"] = result.variance;
            resultJson["min"] = result.min;
            resultJson["max"] = result.max;
            resultJson["range"] = result.range;
            
            analysisArray.push_back(resultJson);
        }
        
        report["analysis"] = analysisArray;
        
        // Add trend analysis
        auto trends = const_cast<PerformanceAnalyzer*>(this)->analyzeTrends(metrics);
        
        json trendsArray = json::array();
        for (const auto& trend : trends) {
            json trendJson;
            trendJson["metricName"] = trend.metricName;
            trendJson["trendValue"] = trend.trendValue;
            trendJson["trendDirection"] = trend.trendDirection;
            trendJson["confidence"] = trend.confidence;
            trendJson["rSquared"] = trend.rSquared;
            
            trendsArray.push_back(trendJson);
        }
        
        report["trends"] = trendsArray;
        
        // Add anomalies
        auto anomalies = const_cast<PerformanceAnalyzer*>(this)->detectAnomalies(metrics);
        
        json anomaliesArray = json::array();
        for (const auto& anomaly : anomalies) {
            json anomalyJson;
            anomalyJson["metricName"] = anomaly.metricName;
            anomalyJson["anomalousValue"] = anomaly.anomalousValue;
            anomalyJson["expectedValue"] = anomaly.expectedValue;
            anomalyJson["deviation"] = anomaly.deviation;
            anomalyJson["anomalyType"] = anomaly.anomalyType;
            anomalyJson["severity"] = anomaly.severity;
            
            anomaliesArray.push_back(anomalyJson);
        }
        
        report["anomalies"] = anomaliesArray;
        
        return report.dump(2);
    } else if (format == "text") {
        std::stringstream report;
        
        report << "=== Performance Analysis Report ===\n";
        report << "Generated at: " << std::chrono::system_clock::now() << "\n\n";
        
        // Add summary
        report << "Summary Statistics:\n";
        auto analysis = const_cast<PerformanceAnalyzer*>(this)->analyze(metrics);
        
        for (const auto& result : analysis) {
            report << "  " << result.metricName << ":\n";
            report << "    Mean: " << result.mean << "\n";
            report << "    Median: " << result.median << "\n";
            report << "    Std Dev: " << result.standardDeviation << "\n";
            report << "    Range: [" << result.min << ", " << result.max << "]\n";
            report << "    Skew: " << result.skew << "\n";
            report << "    Kurtosis: " << result.kurtosis << "\n\n";
        }
        
        // Add trends
        auto trends = const_cast<PerformanceAnalyzer*>(this)->analyzeTrends(metrics);
        
        report << "Performance Trends:\n";
        for (const auto& trend : trends) {
            report << "  " << trend.metricName << ": " << trend.trendDirection
                   << " (value: " << trend.trendValue << ", confidence: " << trend.confidence << ")\n";
        }
        report << "\n";
        
        // Add anomalies
        auto anomalies = const_cast<PerformanceAnalyzer*>(this)->detectAnomalies(metrics);
        
        if (!anomalies.empty()) {
            report << "Anomalies Detected: " << anomalies.size() << "\n";
            for (const auto& anomaly : anomalies) {
                report << "  " << anomaly.metricName << ": " << anomaly.anomalyType
                       << " (value: " << anomaly.anomalousValue << ", severity: " << anomaly.severity << ")\n";
            }
            report << "\n";
        }
        
        return report.str();
    }
    
    return "Unsupported format: " + format;
}

AnalysisResult PerformanceAnalyzer::getMetricAnalysis(const std::string& metricName,
                                                      const std::vector<MetricData>& metrics) const {
    std::lock_guard<std::mutex> lock(mutex_);
    
    // Check cache first
    auto cacheIt = analysisCache_.find(metricName);
    if (cacheIt != analysisCache_.end()) {
        return cacheIt->second;
    }
    
    // Calculate if not in cache
    AnalysisResult result(metricName);
    
    std::vector<double> values;
    for (const auto& metric : metrics) {
        if (metric.name == metricName) {
            values.push_back(metric.value);
        }
    }
    
    if (values.empty()) {
        return result; // Return empty result
    }
    
    result.mean = calculateMean(values);
    result.median = calculateMedian(values);
    result.variance = calculateVariance(values, result.mean);
    result.standardDeviation = std::sqrt(result.variance);
    result.min = *std::min_element(values.begin(), values.end());
    result.max = *std::max_element(values.begin(), values.end());
    result.range = result.max - result.min;
    
    if (result.standardDeviation > 0.0) {
        result.skew = calculateSkew(values, result.mean, result.standardDeviation);
        result.kurtosis = calculateKurtosis(values, result.mean, result.standardDeviation);
    }
    
    result.analysisTime = std::chrono::system_clock::now();
    
    return result;
}

std::string PerformanceAnalyzer::getPerformanceSummary(const std::vector<MetricData>& metrics) const {
    std::stringstream summary;
    
    summary << "Performance Summary\n";
    summary << "===================\n\n";
    
    summary << "Total Metrics Analyzed: " << metrics.size() << "\n";
    
    if (metrics.empty()) {
        return summary.str();
    }
    
    // Calculate overall statistics
    std::vector<double> allValues;
    for (const auto& metric : metrics) {
        allValues.push_back(metric.value);
    }
    
    double overallMean = calculateMean(allValues);
    double overallStdDev = calculateStandardDeviation(allValues, overallMean);
    
    summary << "Overall Mean: " << overallMean << "\n";
    summary << "Overall Standard Deviation: " << overallStdDev << "\n";
    
    // Count metrics by category (simplified)
    std::map<std::string, size_t> categoryCounts;
    for (const auto& metric : metrics) {
        // This is a simplification - in a real implementation, you'd parse the category string
        categoryCounts["ALL"]++; // For now, just count all
    }
    
    summary << "\nMetrics by Category:\n";
    for (const auto& pair : categoryCounts) {
        summary << "  " << pair.first << ": " << pair.second << "\n";
    }
    
    return summary.str();
}

// Statistical calculation implementations

double PerformanceAnalyzer::calculateMean(const std::vector<double>& values) const {
    if (values.empty()) return 0.0;
    
    double sum = std::accumulate(values.begin(), values.end(), 0.0);
    return sum / values.size();
}

double PerformanceAnalyzer::calculateMedian(std::vector<double> values) const {
    if (values.empty()) return 0.0;
    
    size_t n = values.size();
    std::sort(values.begin(), values.end());
    
    if (n % 2 == 0) {
        return (values[n/2 - 1] + values[n/2]) / 2.0;
    } else {
        return values[n/2];
    }
}

double PerformanceAnalyzer::calculateStandardDeviation(const std::vector<double>& values, double mean) const {
    if (values.size() <= 1) return 0.0;
    
    double sumSquares = 0.0;
    for (double value : values) {
        double diff = value - mean;
        sumSquares += diff * diff;
    }
    
    return std::sqrt(sumSquares / (values.size() - 1));
}

double PerformanceAnalyzer::calculateVariance(const std::vector<double>& values, double mean) const {
    if (values.size() <= 1) return 0.0;
    
    double sumSquares = 0.0;
    for (double value : values) {
        double diff = value - mean;
        sumSquares += diff * diff;
    }
    
    return sumSquares / (values.size() - 1);
}

double PerformanceAnalyzer::calculateSkew(const std::vector<double>& values, double mean, double stdDev) const {
    if (values.size() <= 2 || stdDev == 0.0) return 0.0;
    
    double sumCubed = 0.0;
    for (double value : values) {
        double diff = value - mean;
        sumCubed += (diff * diff * diff) / values.size();
    }
    
    return sumCubed / (stdDev * stdDev * stdDev);
}

double PerformanceAnalyzer::calculateKurtosis(const std::vector<double>& values, double mean, double stdDev) const {
    if (values.size() <= 3 || stdDev == 0.0) return 0.0;
    
    double sumFourth = 0.0;
    for (double value : values) {
        double diff = value - mean;
        sumFourth += (diff * diff * diff * diff) / values.size();
    }
    
    double kurt = (sumFourth / (stdDev * stdDev * stdDev * stdDev)) - 3.0;
    return kurt;
}

double PerformanceAnalyzer::calculateTrend(const std::vector<double>& values) const {
    if (values.size() < 2) return 0.0;
    
    // Simple linear trend calculation
    size_t n = values.size();
    double sumX = 0.0, sumY = 0.0, sumXY = 0.0, sumX2 = 0.0;
    
    for (size_t i = 0; i < n; ++i) {
        double x = static_cast<double>(i);
        double y = values[i];
        
        sumX += x;
        sumY += y;
        sumXY += x * y;
        sumX2 += x * x;
    }
    
    double denominator = n * sumX2 - sumX * sumX;
    if (denominator == 0.0) return 0.0;
    
    return (n * sumXY - sumX * sumY) / denominator;
}

std::string PerformanceAnalyzer::determineTrendDirection(double trendValue) const {
    if (trendValue > 0.01) {
        return "increasing";
    } else if (trendValue < -0.01) {
        return "decreasing";
    } else {
        return "stable";
    }
}

double PerformanceAnalyzer::calculateRSquared(const std::vector<double>& x, const std::vector<double>& y) const {
    if (x.size() != y.size() || x.size() < 2) return 0.0;
    
    double meanX = calculateMean(x);
    double meanY = calculateMean(y);
    
    double numerator = 0.0;
    double denominator = 0.0;
    
    for (size_t i = 0; i < x.size(); ++i) {
        double diffX = x[i] - meanX;
        double diffY = y[i] - meanY;
        
        numerator += diffX * diffY;
        denominator += diffX * diffX;
    }
    
    if (denominator == 0.0) return 0.0;
    
    return (numerator / denominator) * (numerator / denominator);
}

bool PerformanceAnalyzer::isOutlier(double value, double mean, double stdDev, double threshold) const {
    if (stdDev == 0.0) return false;
    
    double zScore = std::abs((value - mean) / stdDev);
    return zScore > threshold;
}

std::vector<double> PerformanceAnalyzer::calculateMovingAverage(const std::vector<double>& values, size_t window) const {
    if (values.empty() || window == 0) return std::vector<double>();
    
    std::vector<double> movingAverage;
    movingAverage.reserve(values.size());
    
    for (size_t i = 0; i < values.size(); ++i) {
        size_t start = std::max(static_cast<size_t>(0), i - static_cast<size_t>(window) + 1);
        size_t end = i + 1;
        
        double sum = std::accumulate(values.begin() + start, values.begin() + end, 0.0);
        movingAverage.push_back(sum / (end - start));
    }
    
    return movingAverage;
}

} // namespace nlm
