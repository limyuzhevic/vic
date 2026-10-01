#include "PerformanceAnalyzer.hpp"
#include "PerformanceProfiler.hpp"
#include "../../core/Logger/Logger.hpp"
#include <numeric>
#include <algorithm>
#include <cmath>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <map>
#include <unordered_map>
#include <queue>
#include <stack>

namespace nlm {

struct PerformanceAnalyzer::Impl {
    bool initialized;
    size_t analysisWindowSize;
    bool trendAnalysisEnabled;
    bool correlationAnalysisEnabled;
    bool optimizationSuggestionsEnabled;
    size_t minimumSampleSize;
    double confidenceLevel;
    
    // Time series storage
    std::map<std::string, std::vector<TimeSeriesPoint>> timeSeriesData;
    std::map<std::string, std::vector<double>> metricValues;
    std::map<std::string, std::vector<std::chrono::time_point<std::chrono::system_clock>>> timestamps;
    mutable std::mutex dataMutex;
    
    Impl() : initialized(false), analysisWindowSize(100), trendAnalysisEnabled(true),
             correlationAnalysisEnabled(true), optimizationSuggestionsEnabled(true),
             minimumSampleSize(10), confidenceLevel(0.95) {}
};

PerformanceAnalyzer::PerformanceAnalyzer() : pImpl(std::make_unique<Impl>()) {}

PerformanceAnalyzer::~PerformanceAnalyzer() {
    shutdown();
}

bool PerformanceAnalyzer::initialize() {
    if (pImpl->initialized) return false;
    
    pImpl->initialized = true;
    NLM_LOG_INFO("PerformanceAnalyzer: Initialized successfully");
    return true;
}

void PerformanceAnalyzer::shutdown() {
    if (!pImpl->initialized) return;
    
    std::lock_guard<std::mutex> lock(pImpl->dataMutex);
    pImpl->timeSeriesData.clear();
    pImpl->metricValues.clear();
    pImpl->timestamps.clear();
    
    pImpl->initialized = false;
    NLM_LOG_INFO("PerformanceAnalyzer: Shutdown completed");
}

void PerformanceAnalyzer::addBenchmarkResult(const std::string& benchmarkName, 
                                            const std::string& metricName,
                                            double value,
                                            const std::chrono::time_point<std::chrono::system_clock>& timestamp) {
    if (!pImpl->initialized) return;
    
    std::lock_guard<std::mutex> lock(pImpl->dataMutex);
    
    pImpl->metricValues[metricName].push_back(value);
    pImpl->timestamps[metricName].push_back(timestamp);
    
    // Keep only recent data for performance
    if (pImpl->metricValues[metricName].size() > pImpl->analysisWindowSize) {
        pImpl->metricValues[metricName].erase(pImpl->metricValues[metricName].begin());
        pImpl->timestamps[metricName].erase(pImpl->timestamps[metricName].begin());
    }
    
    NLM_LOG_DEBUG("PerformanceAnalyzer: Added result - " + benchmarkName + "." + metricName + " = " + std::to_string(value));
}

void PerformanceAnalyzer::addTimeSeriesPoint(const TimeSeriesPoint& point) {
    if (!pImpl->initialized) return;
    
    std::lock_guard<std::mutex> lock(pImpl->dataMutex);
    pImpl->timeSeriesData[point.metricName].push_back(point);
}

void PerformanceAnalyzer::addBenchmarkRun(const std::string& benchmarkName, 
                                         const std::map<std::string, double>& metrics,
                                         const std::chrono::time_point<std::chrono::system_clock>& timestamp) {
    for (const auto& pair : metrics) {
        std::string metricKey = benchmarkName + "." + pair.first;
        addBenchmarkResult(benchmarkName, metricKey, pair.second, timestamp);
    }
}

PerformanceAnalyzer::StatisticalSummary PerformanceAnalyzer::calculateStatistics(const std::string& metricName, 
                                                                               size_t windowSize) const {
    StatisticalSummary summary(metricName);
    
    std::lock_guard<std::mutex> lock(pImpl->dataMutex);
    
    auto it = pImpl->metricValues.find(metricName);
    if (it == pImpl->metricValues.end() || it->second.size() < 2) return summary;
    
    const std::vector<double>& values = it->second;
    size_t dataSize = std::min(values.size(), windowSize);
    
    // Get recent data
    std::vector<double> recentValues;
    if (dataSize == values.size()) {
        recentValues = values;
    } else {
        recentValues.assign(values.end() - dataSize, values.end());
    }
    
    if (recentValues.empty()) return summary;
    
    // Calculate statistics
    summary.sampleCount = recentValues.size();
    summary.mean = calculateMean(recentValues);
    summary.median = calculateMedian(recentValues);
    summary.standardDeviation = calculateStandardDeviation(recentValues, summary.mean);
    summary.variance = summary.standardDeviation * summary.standardDeviation;
    
    summary.minValue = *std::min_element(recentValues.begin(), recentValues.end());
    summary.maxValue = *std::max_element(recentValues.begin(), recentValues.end());
    summary.range = summary.maxValue - summary.minValue;
    
    if (summary.mean != 0.0) {
        summary.coefficientOfVariation = (summary.standardDeviation / summary.mean) * 100.0;
    }
    
    // Calculate percentiles
    std::vector<double> percentiles = {25.0, 50.0, 75.0, 90.0, 95.0, 99.0};
    summary.percentiles = calculatePercentiles(recentValues, percentiles);
    
    return summary;
}

std::vector<PerformanceAnalyzer::StatisticalSummary> PerformanceAnalyzer::calculateStatisticsForAllMetrics(size_t windowSize) const {
    std::vector<StatisticalSummary> results;
    
    std::lock_guard<std::mutex> lock(pImpl->dataMutex);
    
    for (const auto& pair : pImpl->metricValues) {
        if (pair.second.size() >= pImpl->minimumSampleSize) {
            results.push_back(calculateStatistics(pair.first, windowSize));
        }
    }
    
    return results;
}

double PerformanceAnalyzer::calculateMean(const std::vector<double>& values) const {
    if (values.empty()) return 0.0;
    return std::accumulate(values.begin(), values.end(), 0.0) / values.size();
}

double PerformanceAnalyzer::calculateMedian(const std::vector<double>& values) const {
    if (values.empty()) return 0.0;
    
    std::vector<double> sorted = values;
    std::sort(sorted.begin(), sorted.end());
    
    size_t size = sorted.size();
    if (size % 2 == 0) {
        return (sorted[size/2 - 1] + sorted[size/2]) / 2.0;
    } else {
        return sorted[size/2];
    }
}

double PerformanceAnalyzer::calculateStandardDeviation(const std::vector<double>& values, double mean) const {
    if (values.size() < 2) return 0.0;
    
    double sumSquaredDiff = 0.0;
    for (double value : values) {
        double diff = value - mean;
        sumSquaredDiff += diff * diff;
    }
    
    return std::sqrt(sumSquaredDiff / (values.size() - 1));
}

std::vector<double> PerformanceAnalyzer::calculatePercentiles(const std::vector<double>& values,
                                                              const std::vector<double>& percentiles) const {
    std::vector<double> sorted = values;
    std::sort(sorted.begin(), sorted.end());
    
    std::vector<double> result;
    result.reserve(percentiles.size());
    
    for (double percentile : percentiles) {
        double position = (percentile / 100.0) * (sorted.size() - 1);
        size_t lowerIndex = static_cast<size_t>(std::floor(position));
        size_t upperIndex = static_cast<size_t>(std::ceil(position));
        
        if (lowerIndex == upperIndex) {
            result.push_back(sorted[lowerIndex]);
        } else {
            double weight = position - lowerIndex;
            result.push_back(sorted[lowerIndex] * (1.0 - weight) + sorted[upperIndex] * weight);
        }
    }
    
    return result;
}

PerformanceAnalyzer::TrendAnalysis PerformanceAnalyzer::analyzeTrend(const std::string& metricName, 
                                                                     size_t windowSize) const {
    TrendAnalysis analysis(metricName);
    
    std::lock_guard<std::mutex> lock(pImpl->dataMutex);
    
    auto it = pImpl->metricValues.find(metricName);
    if (it == pImpl->metricValues.end() || it->second.size() < 2) return analysis;
    
    const std::vector<double>& values = it->second;
    size_t dataSize = std::min(values.size(), windowSize);
    
    // Get recent data
    std::vector<double> recentValues;
    if (dataSize == values.size()) {
        recentValues = values;
    } else {
        recentValues.assign(values.end() - dataSize, values.end());
    }
    
    if (recentValues.size() < 2) return analysis;
    
    analysis.values = recentValues;
    analysis.timestamps = (it->second.size() == recentValues.size()) ? 
                         std::vector<std::chrono::time_point<std::chrono::system_clock>>(it->second.begin(), it->second.end()) :
                         std::vector<std::chrono::time_point<std::chrono::system_clock>>(values.end() - dataSize, values.end());
    
    if (analysis.timestamps.empty()) return analysis;
    
    // Calculate trend
    std::vector<double> xValues;
    for (size_t i = 0; i < analysis.values.size(); ++i) {
        xValues.push_back(static_cast<double>(i));
    }
    
    analysis.slope = linearRegressionSlope(xValues, analysis.values);
    double meanY = calculateMean(analysis.values);
    analysis.rSquared = calculateRSquared(xValues, analysis.values, analysis.slope, meanY);
    
    // Determine trend direction
    if (std::abs(analysis.slope) < 0.01) {
        analysis.trend = "STABLE";
    } else if (analysis.slope > 0.01) {
        analysis.trend = "INCREASING";
    } else {
        analysis.trend = "DECREASING";
    }
    
    // Calculate volatility
    std::vector<double> firstDifferences;
    for (size_t i = 1; i < analysis.values.size(); ++i) {
        firstDifferences.push_back(analysis.values[i] - analysis.values[i-1]);
    }
    
    if (!firstDifferences.empty()) {
        double meanDiff = calculateMean(firstDifferences);
        double varDiff = 0.0;
        for (double diff : firstDifferences) {
            varDiff += (diff - meanDiff) * (diff - meanDiff);
        }
        analysis.volatility = std::sqrt(varDiff / (firstDifferences.size() - 1));
        
        if (analysis.volatility > meanDiff * 2.0) {
            analysis.trend = "VOLATILE";
        }
    }
    
    analysis.trendStart = analysis.timestamps.front();
    analysis.trendEnd = analysis.timestamps.back();
    
    return analysis;
}

std::vector<PerformanceAnalyzer::TrendAnalysis> PerformanceAnalyzer::analyzeTrendsForAllMetrics(size_t windowSize) const {
    std::vector<TrendAnalysis> results;
    
    std::lock_guard<std::mutex> lock(pImpl->dataMutex);
    
    for (const auto& pair : pImpl->metricValues) {
        if (pair.second.size() >= pImpl->minimumSampleSize) {
            results.push_back(analyzeTrend(pair.first, windowSize));
        }
    }
    
    return results;
}

double PerformanceAnalyzer::linearRegressionSlope(const std::vector<double>& xValues,
                                                 const std::vector<double>& yValues) const {
    if (xValues.size() != yValues.size() || xValues.size() < 2) return 0.0;
    
    double sumX = std::accumulate(xValues.begin(), xValues.end(), 0.0);
    double sumY = std::accumulate(yValues.begin(), yValues.end(), 0.0);
    double sumXY = 0.0;
    double sumX2 = 0.0;
    
    for (size_t i = 0; i < xValues.size(); ++i) {
        sumXY += xValues[i] * yValues[i];
        sumX2 += xValues[i] * xValues[i];
    }
    
    double n = static_cast<double>(xValues.size());
    return (n * sumXY - sumX * sumY) / (n * sumX2 - sumX * sumX);
}

double PerformanceAnalyzer::calculateRSquared(const std::vector<double>& xValues,
                                             const std::vector<double>& yValues,
                                             double slope, double meanY) const {
    if (xValues.size() != yValues.size()) return 0.0;
    
    double totalSS = 0.0;
    double residualSS = 0.0;
    
    for (size_t i = 0; i < xValues.size(); ++i) {
        double predictedY = slope * xValues[i] + (meanY - slope * std::accumulate(xValues.begin(), xValues.end(), 0.0) / xValues.size());
        totalSS += (yValues[i] - meanY) * (yValues[i] - meanY);
        residualSS += (yValues[i] - predictedY) * (yValues[i] - predictedY);
    }
    
    if (totalSS == 0.0) return 1.0;
    return 1.0 - (residualSS / totalSS);
}

PerformanceAnalyzer::CorrelationAnalysis PerformanceAnalyzer::calculateCorrelation(const std::string& metric1,
                                                                                 const std::string& metric2,
                                                                                 size_t windowSize) const {
    CorrelationAnalysis correlation(metric1, metric2);
    
    std::lock_guard<std::mutex> lock(pImpl->dataMutex);
    
    auto it1 = pImpl->metricValues.find(metric1);
    auto it2 = pImpl->metricValues.find(metric2);
    
    if (it1 == pImpl->metricValues.end() || it2 == pImpl->metricValues.end()) return correlation;
    
    const std::vector<double>& values1 = it1->second;
    const std::vector<double>& values2 = it2->second;
    
    size_t dataSize = std::min({values1.size(), values2.size(), windowSize});
    
    if (dataSize < 2) return correlation;
    
    // Get recent data
    std::vector<double> recent1, recent2;
    if (dataSize == values1.size()) {
        recent1 = values1;
        recent2 = values2;
    } else {
        recent1.assign(values1.end() - dataSize, values1.end());
        recent2.assign(values2.end() - dataSize, values2.end());
    }
    
    correlation.sampleSize = recent1.size();
    correlation.dataPoints.reserve(recent1.size());
    for (size_t i = 0; i < recent1.size(); ++i) {
        correlation.dataPoints.emplace_back(recent1[i], recent2[i]);
    }
    
    correlation.correlationCoefficient = pearsonCorrelation(recent1, recent2);
    
    // Simple p-value approximation (for large samples)
    if (correlation.sampleSize > 3) {
        double t = correlation.correlationCoefficient * std::sqrt((correlation.sampleSize - 2) / (1 - correlation.correlationCoefficient * correlation.correlationCoefficient));
        // Approximate p-value using t-distribution
        correlation.pValue = 2.0 * (1.0 - 0.5 * (1.0 + std::erf(std::abs(t) / std::sqrt(2.0))));
    }
    
    // Determine relationship
    double absCorr = std::abs(correlation.correlationCoefficient);
    if (absCorr > 0.7) {
        correlation.relationship = (correlation.correlationCoefficient > 0) ? "STRONG_POSITIVE" : "STRONG_NEGATIVE";
    } else if (absCorr > 0.3) {
        correlation.relationship = (correlation.correlationCoefficient > 0) ? "WEAK_POSITIVE" : "WEAK_NEGATIVE";
    } else {
        correlation.relationship = "NONE";
    }
    
    return correlation;
}

std::vector<PerformanceAnalyzer::CorrelationAnalysis> PerformanceAnalyzer::calculateCorrelations(const std::vector<std::string>& metricNames,
                                                                                                 size_t windowSize) const {
    std::vector<CorrelationAnalysis> results;
    
    for (size_t i = 0; i < metricNames.size(); ++i) {
        for (size_t j = i + 1; j < metricNames.size(); ++j) {
            CorrelationAnalysis correlation = calculateCorrelation(metricNames[i], metricNames[j], windowSize);
            if (correlation.sampleSize >= pImpl->minimumSampleSize) {
                results.push_back(correlation);
            }
        }
    }
    
    return results;
}

double PerformanceAnalyzer::pearsonCorrelation(const std::vector<double>& xValues,
                                              const std::vector<double>& yValues) const {
    if (xValues.size() != yValues.size() || xValues.size() < 2) return 0.0;
    
    double xMean = calculateMean(xValues);
    double yMean = calculateMean(yValues);
    
    double numerator = 0.0;
    double xDenominator = 0.0;
    double yDenominator = 0.0;
    
    for (size_t i = 0; i < xValues.size(); ++i) {
        double xDiff = xValues[i] - xMean;
        double yDiff = yValues[i] - yMean;
        numerator += xDiff * yDiff;
        xDenominator += xDiff * xDiff;
        yDenominator += yDiff * yDiff;
    }
    
    if (xDenominator == 0.0 || yDenominator == 0.0) return 0.0;
    
    return numerator / std::sqrt(xDenominator * yDenominator);
}

double PerformanceAnalyzer::spearmanCorrelation(const std::vector<double>& xValues,
                                                const std::vector<double>& yValues) const {
    if (xValues.size() != yValues.size() || xValues.size() < 2) return 0.0;
    
    // Create rank vectors
    std::vector<double> xRanks = xValues;
    std::vector<double> yRanks = yValues;
    
    std::sort(xRanks.begin(), xRanks.end());
    std::sort(yRanks.begin(), yRanks.end());
    
    for (size_t i = 0; i < xRanks.size(); ++i) {
        // Assign ranks (handling ties)
        size_t rankCount = std::count(xRanks.begin(), xRanks.end(), xRanks[i]);
        xRanks[i] = i + 1;
    }
    
    for (size_t i = 0; i < yRanks.size(); ++i) {
        size_t rankCount = std::count(yRanks.begin(), yRanks.end(), yRanks[i]);
        yRanks[i] = i + 1;
    }
    
    return pearsonCorrelation(xRanks, yRanks);
}

std::vector<PerformanceAnalyzer::OptimizationSuggestion> PerformanceAnalyzer::generateOptimizationSuggestions() const {
    std::vector<OptimizationSuggestion> suggestions;
    
    std::lock_guard<std::mutex> lock(pImpl->dataMutex);
    
    for (const auto& pair : pImpl->metricValues) {
        StatisticalSummary stats = calculateStatistics(pair.first);
        
        if (stats.sampleCount < pImpl->minimumSampleSize) continue;
        
        OptimizationSuggestion suggestion(pair.first);
        suggestion.currentValue = stats.mean;
        
        // Generate suggestions based on statistical analysis
        if (stats.sampleCount > 10 && stats.standardDeviation > stats.mean * 0.5) {
            suggestion.targetValue = stats.mean * 0.8; // Reduce by 20%
            suggestion.suggestedAction = "Reduce variance through normalization";
            suggestion.priority = "MEDIUM";
            suggestion.impact = "PERFORMANCE";
            suggestion.estimatedGain = (stats.mean - suggestion.targetValue) / stats.mean * 100.0;
            suggestion.implementationDifficulty = "MEDIUM";
        } else if (stats.sampleCount > 50 && stats.standardDeviation < stats.mean * 0.1) {
            suggestion.targetValue = stats.mean * 1.2; // Increase by 20%
            suggestion.suggestedAction = "Increase target throughput";
            suggestion.priority = "HIGH";
            suggestion.impact = "PERFORMANCE";
            suggestion.estimatedGain = (suggestion.targetValue - stats.mean) / stats.mean * 100.0;
            suggestion.implementationDifficulty = "EASY";
        } else {
            suggestion.targetValue = stats.mean;
            suggestion.suggestedAction = "Maintain current configuration";
            suggestion.priority = "LOW";
            suggestion.impact = "STABILITY";
            suggestion.estimatedGain = 0.0;
            suggestion.implementationDifficulty = "N/A";
        }
        
        if (suggestion.targetValue > 0) {
            suggestion.improvementPercentage = (suggestion.targetValue - suggestion.currentValue) / suggestion.currentValue * 100.0;
            suggestions.push_back(suggestion);
        }
    }
    
    return suggestions;
}

PerformanceAnalyzer::OptimizationSuggestion PerformanceAnalyzer::analyzeBottleneck(const std::string& metricName) const {
    OptimizationSuggestion suggestion(metricName);
    
    StatisticalSummary stats = calculateStatistics(metricName);
    suggestion.currentValue = stats.mean;
    
    if (stats.sampleCount < pImpl->minimumSampleSize) return suggestion;
    
    // Check for bottlenecks based on high coefficient of variation
    if (stats.coefficientOfVariation > 50.0) {
        suggestion.targetValue = stats.mean * 0.7;
        suggestion.suggestedAction = "Identify and fix resource contention";
        suggestion.priority = "HIGH";
        suggestion.impact = "PERFORMANCE";
        suggestion.estimatedGain = (stats.mean - suggestion.targetValue) / stats.mean * 100.0;
        suggestion.implementationDifficulty = "HARD";
    } else if (stats.sampleCount > 100 && stats.standardDeviation > 10.0 * stats.mean) {
        suggestion.targetValue = stats.mean * 0.9;
        suggestion.suggestedAction = "Optimize memory access patterns";
        suggestion.priority = "MEDIUM";
        suggestion.impact = "MEMORY";
        suggestion.estimatedGain = (stats.mean - suggestion.targetValue) / stats.mean * 100.0;
        suggestion.implementationDifficulty = "MEDIUM";
    } else {
        suggestion.targetValue = stats.mean;
        suggestion.suggestedAction = "No bottleneck detected";
        suggestion.priority = "LOW";
        suggestion.impact = "NONE";
        suggestion.estimatedGain = 0.0;
        suggestion.implementationDifficulty = "N/A";
    }
    
    return suggestion;
}

std::vector<PerformanceAnalyzer::OptimizationSuggestion> PerformanceAnalyzer::analyzeResourceConstraints() const {
    // Analyze for resource constraints across all metrics
    return generateOptimizationSuggestions();
}

bool PerformanceAnalyzer::trainModel(const std::string& modelName,
                                    const std::string& targetMetric,
                                    const std::map<std::string, std::vector<double>>& featureData,
                                    const std::string& modelType) {
    // Simplified model training
    NLM_LOG_INFO("PerformanceAnalyzer: Training model " + modelName + " for metric " + targetMetric);
    
    // In real implementation, this would perform actual machine learning
    // For now, we'll just store a simple linear model
    return true;
}

double PerformanceAnalyzer::predictPerformance(const std::string& modelName,
                                              const std::map<std::string, double>& inputFeatures) const {
    // Simplified prediction
    return 0.0;
}

PerformanceAnalyzer::PerformancePrediction PerformanceAnalyzer::predictFuturePerformance(const std::string& metricName,
                                                                                        const std::string& scenario,
                                                                                        const std::chrono::duration<double>& horizon) const {
    PerformancePrediction prediction("pred_" + metricName + "_" + std::to_string(std::chrono::system_clock::now().time_since_epoch().count()),
                                    metricName);
    
    // Simple prediction based on trend
    TrendAnalysis trend = analyzeTrend(metricName);
    
    if (trend.trend == "INCREASING") {
        prediction.predictedValue = trend.values.back() * 1.1;
    } else if (trend.trend == "DECREASING") {
        prediction.predictedValue = trend.values.back() * 0.9;
    } else {
        prediction.predictedValue = trend.values.back();
    }
    
    prediction.predictionTime = std::chrono::system_clock::now();
    prediction.predictionHorizon = prediction.predictionTime + horizon;
    prediction.confidenceInterval = std::max(0.1, 1.0 - std::abs(trend.slope));
    prediction.uncertainty = std::abs(trend.volatility);
    prediction.scenario = scenario;
    
    return prediction;
}

bool PerformanceAnalyzer::detectRegression(const std::string& metricName,
                                         double threshold,
                                         size_t regressionWindow) const {
    // Simple regression detection
    if (regressionWindow == 0) regressionWindow = 10;
    
    std::lock_guard<std::mutex> lock(pImpl->dataMutex);
    auto it = pImpl->metricValues.find(metricName);
    if (it == pImpl->metricValues.end() || it->second.size() < regressionWindow * 2) return false;
    
    const std::vector<double>& values = it->second;
    
    // Compare recent window with previous window
    size_t recentStart = values.size() - regressionWindow;
    size_t previousStart = recentStart - regressionWindow;
    
    double recentMean = calculateMean(std::vector<double>(values.begin() + recentStart, values.end()));
    double previousMean = calculateMean(std::vector<double>(values.begin() + previousStart, values.begin() + recentStart));
    
    if (previousMean == 0.0) return false;
    
    double regression = std::abs((previousMean - recentMean) / previousMean);
    return regression > threshold;
}

std::vector<std::string> PerformanceAnalyzer::detectRegressions(const std::vector<std::string>& metricNames,
                                                                double threshold) const {
    std::vector<std::string> regressions;
    
    for (const std::string& metricName : metricNames) {
        if (detectRegression(metricName, threshold)) {
            regressions.push_back(metricName);
        }
    }
    
    return regressions;
}

bool PerformanceAnalyzer::validatePerformanceRequirements(const std::map<std::string, double>& requirements) const {
    for (const auto& pair : requirements) {
        StatisticalSummary stats = calculateStatistics(pair.first);
        if (stats.sampleCount < pImpl->minimumSampleSize) return false;
        if (stats.mean < pair.second) return false;
    }
    return true;
}

double PerformanceAnalyzer::calculatePerformanceScore(const std::map<std::string, double>& metrics) const {
    if (metrics.empty()) return 0.0;
    
    double sum = 0.0;
    for (const auto& pair : metrics) {
        sum += pair.second;
    }
    return sum / metrics.size();
}

std::string PerformanceAnalyzer::getPerformanceGrade(double score) const {
    if (score >= 90.0) return "A+";
    if (score >= 85.0) return "A";
    if (score >= 80.0) return "B+";
    if (score >= 75.0) return "B";
    if (score >= 70.0) return "C+";
    if (score >= 60.0) return "C";
    return "F";
}

std::string PerformanceAnalyzer::generatePerformanceReport() const {
    std::stringstream ss;
    ss << "Performance Analysis Report" << std::endl;
    ss << "===========================" << std::endl;
    ss << std::endl;
    
    std::vector<StatisticalSummary> stats = calculateStatisticsForAllMetrics();
    
    for (const auto& summary : stats) {
        ss << "Metric: " << summary.metricName << std::endl;
        ss << "  Samples: " << summary.sampleCount << std::endl;
        ss << "  Mean: " << summary.mean << std::endl;
        ss << "  Median: " << summary.median << std std::endl;
        ss << "  Std Dev: " << summary.standardDeviation << std::endl;
        ss << "  Min: " << summary.minValue << std::endl;
        ss << "  Max: " << summary.maxValue << std::endl;
        ss << "  Coefficient of Variation: " << summary.coefficientOfVariation << "%" << std::endl;
        ss << std::endl;
    }
    
    return ss.str();
}

std::string PerformanceAnalyzer::generateTrendReport() const {
    std::stringstream ss;
    ss << "Trend Analysis Report" << std::endl;
    ss << "=======================" << std::endl;
    ss << std::endl;
    
    std::vector<TrendAnalysis> trends = analyzeTrendsForAllMetrics();
    
    for (const auto& trend : trends) {
        ss << "Metric: " << trend.metricName << std::endl;
        ss << "  Trend: " << trend.trend << std::endl;
        ss << "  Slope: " << trend.slope << std::endl;
        ss << "  R-squared: " << trend.rSquared << std::endl;
        ss << "  Volatility: " << trend.volatility << std::endl;
        ss << std::endl;
    }
    
    return ss.str();
}

std::string PerformanceAnalyzer::generateCorrelationReport() const {
    std::stringstream ss;
    ss << "Correlation Analysis Report" << std::endl;
    ss << "===========================" << std::endl;
    ss << std::endl;
    
    std::vector<std::string> metricNames;
    std::lock_guard<std::mutex> lock(pImpl->dataMutex);
    for (const auto& pair : pImpl->metricValues) {
        if (pair.second.size() >= pImpl->minimumSampleSize) {
            metricNames.push_back(pair.first);
        }
    }
    
    std::vector<CorrelationAnalysis> correlations = calculateCorrelations(metricNames);
    
    for (const auto& correlation : correlations) {
        ss << "Correlation: " << correlation.metric1 << " vs " << correlation.metric2 << std::endl;
        ss << "  Coefficient: " << correlation.correlationCoefficient << std::endl;
        ss << "  Relationship: " << correlation.relationship << std::endl;
        ss << "  Samples: " << correlation.sampleSize << std::endl;
        ss << std::endl;
    }
    
    return ss.str();
}

std::string PerformanceAnalyzer::generateOptimizationReport() const {
    std::stringstream ss;
    ss << "Optimization Recommendations" << std::endl;
    ss << "===========================" << std::endl;
    ss << std::endl;
    
    std::vector<OptimizationSuggestion> suggestions = generateOptimizationSuggestions();
    
    for (const auto& suggestion : suggestions) {
        if (suggestion.priority == "HIGH") {
            ss << "*** HIGH PRIORITY *** " << std::endl;
        }
        
        ss << suggestion.metricName << std::endl;
        ss << "  Current: " << suggestion.currentValue << std::endl;
        ss << "  Target: " << suggestion.targetValue << std::endl;
        ss << "  Improvement: " << suggestion.improvementPercentage << "%" << std::endl;
        ss << "  Action: " << suggestion.suggestedAction << std::endl;
        ss << "  Impact: " << suggestion.impact << std::endl;
        ss << "  Priority: " << suggestion.priority << std::endl;
        ss << std::endl;
    }
    
    return ss.str();
}

void PerformanceAnalyzer::exportAnalysisResults(const std::string& filename, const std::string& format) const {
    if (format == "json") {
        std::ofstream file(filename);
        if (!file.is_open()) return;
        
        file << "{\n";
        file << "  \"performance_analysis\": {\n";
        
        std::vector<StatisticalSummary> stats = calculateStatisticsForAllMetrics();
        
        file << "    \"statistics\": [\n";
        for (size_t i = 0; i < stats.size(); ++i) {
            const auto& s = stats[i];
            file << "      {\"metric\": \"" << s.metricName << "\", \"mean\": " << s.mean << ", \"std_dev\": " << s.standardDeviation << "\"";
            if (i < stats.size() - 1) file << ",\n";
        }
        file << "\n    ]\n";
        
        file << "  }\n";
        file << "}\n";
        file.close();
    }
    
    NLM_LOG_INFO("PerformanceAnalyzer: Analysis results exported to " + filename);
}

void PerformanceAnalyzer::setAnalysisWindow(size_t windowSize) {
    std::lock_guard<std::mutex> lock(pImpl->dataMutex);
    pImpl->analysisWindowSize = windowSize;
}

void PerformanceAnalyzer::enableTrendAnalysis(bool enable) {
    std::lock_guard<std::mutex> lock(pImpl->dataMutex);
    pImpl->trendAnalysisEnabled = enable;
}

void PerformanceAnalyzer::enableCorrelationAnalysis(bool enable) {
    std::lock_guard<std::mutex> lock(pImpl->dataMutex);
    pImpl->correlationAnalysisEnabled = enable;
}

void PerformanceAnalyzer::enableOptimizationSuggestions(bool enable) {
    std::lock_guard<std::mutex> lock(pImpl->dataMutex);
    pImpl->optimizationSuggestionsEnabled = enable;
}

void PerformanceAnalyzer::setMinimumSampleSize(size_t size) {
    std::lock_guard<std::mutex> lock(pImpl->dataMutex);
    pImpl->minimumSampleSize = size;
}

void PerformanceAnalyzer::setConfidenceLevel(double level) {
    if (level >= 0.0 && level <= 1.0) {
        std::lock_guard<std::mutex> lock(pImpl->dataMutex);
        pImpl->confidenceLevel = level;
    }
}

std::vector<PerformanceAnalyzer::TimeSeriesPoint> PerformanceAnalyzer::getTimeSeriesData(const std::string& metricName,
                                                                                     const std::chrono::time_point<std::chrono::system_clock>& startTime,
                                                                                     const std::chrono::time_point<std::chrono::system_clock>& endTime) const {
    std::vector<TimeSeriesPoint> result;
    
    std::lock_guard<std::mutex> lock(pImpl->dataMutex);
    
    auto it = pImpl->timeSeriesData.find(metricName);
    if (it == pImpl->timeSeriesData.end()) return result;
    
    for (const auto& point : it->second) {
        if (point.timestamp >= startTime && point.timestamp <= endTime) {
            result.push_back(point);
        }
    }
    
    return result;
}

std::map<std::string, std::vector<double>> PerformanceAnalyzer::getTimeSeriesMatrix(const std::vector<std::string>& metricNames,
                                                                                   const std::chrono::time_point<std::chrono::system_clock>& startTime,
                                                                                   const std::chrono::time_point<std::chrono::system_clock>& endTime) const {
    std::map<std::string, std::vector<double>> result;
    
    std::lock_guard<std::mutex> lock(pImpl->dataMutex);
    
    for (const std::string& metricName : metricNames) {
        auto it = pImpl->timeSeriesData.find(metricName);
        if (it != pImpl->timeSeriesData.end()) {
            std::vector<double> values;
            for (const auto& point : it->second) {
                if (point.timestamp >= startTime && point.timestamp <= endTime) {
                    // Extract values from measurements map (simplified)
                    for (const auto& measure : point.measurements) {
                        values.push_back(measure.second);
                        break; // Only use first metric for simplicity
                    }
                }
            }
            result[metricName] = values;
        }
    }
    
    return result;
}

void PerformanceAnalyzer::cleanupOldData() {
    auto now = std::chrono::system_clock::now();
    auto cutoff = now - std::chrono::hours(24);
    
    std::lock_guard<std::mutex> lock(pImpl->dataMutex);
    
    // Remove old time series points
    for (auto& pair : pImpl->timeSeriesData) {
        auto& points = pair.second;
        points.erase(std::remove_if(points.begin(), points.end(),
                                   [cutoff](const TimeSeriesPoint& point) {
                                       return point.timestamp < cutoff;
                                   }),
                    points.end());
    }
}

std::vector<double> PerformanceAnalyzer::getRecentValues(const std::string& metricName, size_t windowSize) const {
    std::lock_guard<std::mutex> lock(pImpl->dataMutex);
    
    auto it = pImpl->metricValues.find(metricName);
    if (it == pImpl->metricValues.end()) return {};
    
    const std::vector<double>& values = it->second;
    size_t dataSize = std::min(values.size(), windowSize);
    
    std::vector<double> recent;
    if (dataSize == values.size()) {
        recent = values;
    } else {
        recent.assign(values.end() - dataSize, values.end());
    }
    
    return recent;
}

std::chrono::time_point<std::chrono::system_clock> PerformanceAnalyzer::getTimestampFromIndex(size_t index) const {
    std::lock_guard<std::mutex> lock(pImpl->dataMutex);
    
    // Simplified - return current time in real implementation
    return std::chrono::system_clock::now();
}

bool PerformanceAnalyzer::validateDataQuality(const std::vector<double>& values) const {
    if (values.size() < pImpl->minimumSampleSize) return false;
    
    // Check for outliers using IQR method
    if (values.size() >= 4) {
        std::vector<double> sorted = values;
        std::sort(sorted.begin(), sorted.end());
        
        size_t q1 = sorted.size() / 4;
        size_t q3 = 3 * sorted.size() / 4;
        
        double q1Value = sorted[q1];
        double q3Value = sorted[q3];
        double iqr = q3Value - q1Value;
        
        double lowerBound = q1Value - 1.5 * iqr;
        double upperBound = q3Value + 1.5 * iqr;
        
        for (double value : values) {
            if (value < lowerBound || value > upperBound) {
                return false; // Contains outliers
            }
        }
    }
    
    return true;
}

} // namespace nlm
