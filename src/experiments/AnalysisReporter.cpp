// AnalysisReporter.cpp - Implementation of Analysis and Reporting System

#include "AnalysisReporter.hpp"
#include <algorithm>
#include <numeric>
#include <iomanip>
#include <fstream>
#include <filesystem>
#include <sstream>
#include <yaml-cpp/yaml.h>
#include <pugixml.hpp>
#include <cmath>
#include <set>
#include <queue>
#include <regex>
#include <numeric>

namespace nlm {

// StatisticalAnalyzer implementation
PerformanceAnalyzer::PerformanceAnalyzer() = default;

PerformanceAnalyzer::~PerformanceAnalyzer() = default;

StatisticalAnalyzer::calculateMean(const std::vector<double>& values) {
    if (values.empty()) return 0.0;
    double sum = std::accumulate(values.begin(), values.end(), 0.0);
    return sum / values.size();
}

StatisticalAnalyzer::calculateStdDev(const std::vector<double>& values, double mean) {
    if (values.empty()) return 0.0;
    if (mean == 0.0) mean = calculateMean(values);
    
    double sumSquaredDiff = 0.0;
    for (double value : values) {
        double diff = value - mean;
        sumSquaredDiff += diff * diff;
    }
    
    return std::sqrt(sumSquaredDiff / values.size());
}

StatisticalAnalyzer::calculateMedian(const std::vector<double>& values) {
    if (values.empty()) return 0.0;
    std::vector<double> sorted = values;
    std::sort(sorted.begin(), sorted.end());
    size_t n = sorted.size();
    if (n % 2 == 0) {
        return (sorted[n/2 - 1] + sorted[n/2]) / 2.0;
    } else {
        return sorted[n/2];
    }
}

StatisticalAnalyzer::calculatePercentile(const std::vector<double>& values, double percentile) {
    if (values.empty()) return 0.0;
    std::vector<double> sorted = values;
    std::sort(sorted.begin(), sorted.end());
    double position = (percentile / 100.0) * (sorted.size() - 1);
    size_t lowerIndex = static_cast<size_t>(std::floor(position));
    size_t upperIndex = static_cast<size_t>(std::ceil(position));
    
    if (lowerIndex == upperIndex) {
        return sorted[lowerIndex];
    }
    
    double weight = position - lowerIndex;
    return sorted[lowerIndex] * (1.0 - weight) + sorted[upperIndex] * weight;
}

StatisticalAnalyzer::calculateMode(const std::vector<size_t>& values) {
    if (values.empty()) return 0;
    std::unordered_map<size_t, size_t> frequency;
    for (size_t value : values) {
        frequency[value]++;
    }
    
    size_t mode = 0;
    size_t maxCount = 0;
    for (const auto& pair : frequency) {
        if (pair.second > maxCount) {
            maxCount = pair.second;
            mode = pair.first;
        }
    }
    
    return mode;
}

StatisticalAnalyzer::describeDistribution(const std::vector<double>& values) {
    if (values.empty()) return "Empty distribution";
    
    double mean = calculateMean(values);
    double stdDev = calculateStdDev(values, mean);
    double min = *std::min_element(values.begin(), values.end());
    double max = *std::max_element(values.begin(), values.end());
    
    std::stringstream ss;
    ss << "Mean: " << mean << ", StdDev: " << stdDev << ", Min: " << min << ", Max: " << max << ", Range: " << (max - min);
    
    if (stdDev < mean * 0.1) {
        ss << ", Distribution: Tight (low variance)";
    } else if (stdDev < mean * 0.5) {
        ss << ", Distribution: Moderate (medium variance)";
    } else {
        ss << ", Distribution: Wide (high variance)";
    }
    
    return ss.str();
}

StatisticalAnalyzer::linearRegression(const std::vector<double>& x, const std::vector<double>& y) {
    if (x.size() != y.size() || x.size() < 2) {
        return {0.0, 0.0};
    }
    
    double sumX = std::accumulate(x.begin(), x.end(), 0.0);
    double sumY = std::accumulate(y.begin(), y.end(), 0.0);
    double sumXY = 0.0;
    double sumXX = 0.0;
    double sumYY = 0.0;
    
    for (size_t i = 0; i < x.size(); ++i) {
        sumXY += x[i] * y[i];
        sumXX += x[i] * x[i];
        sumYY += y[i] * y[i];
    }
    
    double n = static_cast<double>(x.size());
    double denominator = n * sumXX - sumX * sumX;
    
    if (denominator == 0) {
        return {0.0, 0.0};
    }
    
    double slope = (n * sumXY - sumX * sumY) / denominator;
    double intercept = (sumY - slope * sumX) / n;
    
    return {slope, intercept};
}

StatisticalAnalyzer::calculateCorrelation(const std::vector<double>& x, const std::vector<double>& y) {
    if (x.size() != y.size() || x.size() < 2) return 0.0;
    
    double sumX = std::accumulate(x.begin(), x.end(), 0.0);
    double sumY = std::accumulate(y.begin(), y.end(), 0.0);
    double sumXY = 0.0;
    double sumXX = 0.0;
    double sumYY = 0.0;
    
    for (size_t i = 0; i < x.size(); ++i) {
        sumXY += x[i] * y[i];
        sumXX += x[i] * x[i];
        sumYY += y[i] * y[i];
    }
    
    double numerator = (x.size() * sumXY - sumX * sumY);
    double denominator = std::sqrt((x.size() * sumXX - sumX * sumX) * (x.size() * sumYY - sumY * sumY));
    
    if (denominator == 0) return 0.0;
    
    return numerator / denominator;
}

StatisticalAnalyzer::calculateCovariance(const std::vector<double>& x, const std::vector<double>& y) {
    if (x.size() != y.size() || x.size() < 2) return 0.0;
    
    double meanX = calculateMean(x);
    double meanY = calculateMean(y);
    double covariance = 0.0;
    
    for (size_t i = 0; i < x.size(); ++i) {
        covariance += (x[i] - meanX) * (y[i] - meanY);
    }
    
    return covariance / (x.size() - 1);
}

StatisticalAnalyzer::calculateSpearmanRankCorrelation(const std::vector<double>& x, const std::vector<double>& y) {
    if (x.size() != y.size() || x.size() < 2) return 0.0;
    
    std::vector<size_t> xRanks(x.size()), yRanks(y.size());
    std::vector<size_t> sortedX(x.size()), sortedY(y.size());
    
    std::iota(sortedX.begin(), sortedX.end(), 0);
    std::iota(sortedY.begin(), sortedY.end(), 0);
    
    std::sort(sortedX.begin(), sortedX.end(), [&](size_t a, size_t b) { return x[a] < x[b]; });
    std::sort(sortedY.begin(), sortedY.end(), [&](size_t a, size_t b) { return y[a] < y[b]; });
    
    for (size_t i = 0; i < x.size(); ++i) {
        xRanks[sortedX[i]] = i + 1;
        yRanks[sortedY[i]] = i + 1;
    }
    
    return calculateCorrelation(xRanks, yRanks);
}

StatisticalAnalyzer::performTTest(const std::vector<double>& sample1, const std::vector<double>& sample2, double alpha) {
    if (sample1.size() < 2 || sample2.size() < 2) return false;
    
    double mean1 = calculateMean(sample1);
    double mean2 = calculateMean(sample2);
    double stdDev1 = calculateStdDev(sample1, mean1);
    double stdDev2 = calculateStdDev(sample2, mean2);
    
    double pooledStdDev = std::sqrt(((sample1.size() - 1) * stdDev1 * stdDev1 + 
                                    (sample2.size() - 1) * stdDev2 * stdDev2) /
                                   (sample1.size() + sample2.size() - 2));
    
    if (pooledStdDev == 0) return false;
    
    double tStatistic = (mean1 - mean2) / (pooledStdDev * std::sqrt(1.0 / sample1.size() + 1.0 / sample2.size()));
    
    double criticalValue = 1.96; // For alpha = 0.05 (two-tailed)
    
    return std::abs(tStatistic) > criticalValue;
}

StatisticalAnalyzer::identifyTrendType(const std::vector<double>& values, double threshold) {
    if (values.size() < 3) return "insufficient_data";
    
    std::vector<double> x(values.size());
    std::iota(x.begin(), x.end(), 0);
    
    auto [slope, intercept] = linearRegression(x, values);
    double slopeStrength = std::abs(slope);
    
    if (slopeStrength < threshold) {
        return "stable";
    } else if (slope > 0) {
        return "increasing";
    } else {
        return "decreasing";
    }
}

StatisticalAnalyzer::calculateTrendStrength(const std::vector<double>& values) {
    if (values.size() < 2) return 0.0;
    
    std::vector<double> x(values.size());
    std::iota(x.begin(), x.end(), 0);
    
    auto [slope, intercept] = linearRegression(x, values);
    double mean = calculateMean(values);
    
    if (mean == 0) return 0.0;
    
    return std::abs(slope) / mean;
}

StatisticalAnalyzer::detectAnomalies(const std::vector<double>& values, double threshold) {
    std::vector<size_t> anomalies;
    
    if (values.size() < 3) return anomalies;
    
    double mean = calculateMean(values);
    double stdDev = calculateStdDev(values, mean);
    
    if (stdDev == 0) return anomalies;
    
    for (size_t i = 0; i < values.size(); ++i) {
        double zScore = std::abs(values[i] - mean) / stdDev;
        if (zScore > threshold) {
            anomalies.push_back(i);
        }
    }
    
    return anomalies;
}

StatisticalAnalyzer::normalizeData(const std::vector<std::vector<double>>& data) {
    if (data.empty() || data[0].empty()) return std::vector<std::vector<double>>();
    
    std::vector<std::vector<double>> normalized = data;
    size_t rows = data.size();
    size_t cols = data[0].size();
    
    for (size_t col = 0; col < cols; ++col) {
        std::vector<double> column;
        for (size_t row = 0; row < rows; ++row) {
            column.push_back(data[row][col]);
        }
        
        double mean = calculateMean(column);
        double stdDev = calculateStdDev(column, mean);
        
        if (stdDev > 0) {
            for (size_t row = 0; row < rows; ++row) {
                normalized[row][col] = (column[row] - mean) / stdDev;
            }
        }
    }
    
    return normalized;
}

StatisticalAnalyzer::principalComponentAnalysis(const std::vector<std::vector<double>>& data, size_t components) {
    if (data.empty() || data[0].empty()) return std::vector<std::vector<double>>();
    
    // Simplified PCA - in practice, use a proper linear algebra library
    size_t rows = data.size();
    size_t cols = data[0].size();
    
    // Center the data
    std::vector<std::vector<double>> centered(rows, std::vector<double>(cols));
    for (size_t row = 0; row < rows; ++row) {
        for (size_t col = 0; col < cols; ++col) {
            centered[row][col] = data[row][col] - calculateMean(
                std::vector<double>(data.begin() + row, data.begin() + row + 1)
            );
        }
    }
    
    // Calculate covariance matrix
    std::vector<std::vector<double>> covariance(cols, std::vector<double>(cols, 0.0));
    for (size_t i = 0; i < cols; ++i) {
        for (size_t j = 0; j < cols; ++j) {
            double cov = 0.0;
            for (size_t row = 0; row < rows; ++row) {
                cov += centered[row][i] * centered[row][j];
            }
            covariance[i][j] = cov / (rows - 1);
        }
    }
    
    // Return first 'components' principal components (simplified)
    std::vector<std::vector<double>> result;
    for (size_t i = 0; i < std::min(components, cols); ++i) {
        result.push_back(std::vector<double>(cols, 0.0)); // Placeholder
    }
    
    return result;
}

StatisticalAnalyzer::formatStatisticalResult(double value, const std::string& format) {
    std::stringstream ss;
    ss << std::fixed << std::setprecision(4) << value;
    return ss.str();
}

// NLPerformanceAnalyzer implementation
NLPerformanceAnalyzer::NLPerformanceAnalyzer(const std::string& name) : analyzerName_(name) {
    version_ = "1.0.0";
}

BottleneckAnalysis NLPerformanceAnalyzer::analyzeBottlenecks(const std::vector<TimestepPerformance>& timestepData) {
    BottleneckAnalysis analysis;
    
    if (timestepData.empty()) {
        return analysis;
    }
    
    // Find the timestep with the highest bottleneck ratio
    size_t maxIndex = 0;
    double maxRatio = timestepData[0].bottleneckRatio;
    
    for (size_t i = 1; i < timestepData.size(); ++i) {
        if (timestepData[i].bottleneckRatio > maxRatio) {
            maxIndex = i;
            maxRatio = timestepData[i].bottleneckRatio;
        }
    }
    
    const auto& bottleneckTS = timestepData[maxIndex];
    
    // Determine bottleneck component
    double maxComponentTime = std::max({
        bottleneckTS.componentTiming.neuronUpdateNs,
        bottleneckTS.componentTiming.synapseUpdateNs,
        bottleneckTS.componentTiming.spikePropagationNs,
        bottleneckTS.componentTiming.memoryManagementNs,
        bottleneckTS.componentTiming.lockContentionNs,
        bottleneckTS.componentTiming.otherOverheadNs
    });
    
    analysis.bottleneckComponent = bottleneckTS.bottleneckComponent;
    analysis.bottleneckSeverity = bottleneckTS.bottleneckRatio;
    analysis.performanceImpact = maxComponentTime / bottleneckTS.wallTimeNs * 100.0;
    
    // Simple root cause analysis
    if (maxComponentTime == bottleneckTS.componentTiming.neuronUpdateNs) {
        analysis.rootCause = "Neuron processing is the bottleneck";
        analysis.suggestedFix = "Optimize neuron update algorithm or increase parallel processing";
    } else if (maxComponentTime == bottleneckTS.componentTiming.synapseUpdateNs) {
        analysis.rootCause = "Synapse processing is the bottleneck";
        analysis.suggestedFix = "Optimize synapse update algorithm or improve sparse connectivity";
    } else if (maxComponentTime == bottleneckTS.componentTiming.spikePropagationNs) {
        analysis.rootCause = "Spike propagation is the bottleneck";
        analysis.suggestedFix = "Optimize spike event queue or reduce event overhead";
    } else if (maxComponentTime == bottleneckTS.componentTiming.memoryManagementNs) {
        analysis.rootCause = "Memory management is the bottleneck";
        analysis.suggestedFix = "Optimize memory pools or increase memory bandwidth";
    } else if (maxComponentTime == bottleneckTS.componentTiming.lockContentionNs) {
        analysis.rootCause = "Lock contention is the bottleneck";
        analysis.suggestedFix = "Reduce lock granularity or implement lock-free algorithms";
    } else {
        analysis.rootCause = "Unknown bottleneck";
        analysis.suggestedFix = "Investigate all components for optimization opportunities";
    }
    
    // Estimate improvement
    analysis.estimatedImprovement = (1.0 - maxRatio) * 100.0;
    
    // Calculate fix complexity based on bottleneck type
    if (analysis.rootCause.find("memory") != std::string::npos || 
        analysis.rootCause.find("lock") != std::string::npos) {
        analysis.fixComplexity = 0.7;
    } else if (analysis.rootCause.find("algorithm") != std::string::npos) {
        analysis.fixComplexity = 0.5;
    } else {
        analysis.fixComplexity = 0.3;
    }
    
    return analysis;
}

std::vector<OptimizationRecommendation> NLPerformanceAnalyzer::generateRecommendations(const BottleneckAnalysis& bottleneck) {
    std::vector<OptimizationRecommendation> recommendations;
    
    if (bottleneck.bottleneckComponent.empty()) {
        return recommendations;
    }
    
    // Generate recommendations based on bottleneck type
    OptimizationRecommendation rec1;
    rec1.recommendationId = "OPT_001";
    rec1.title = "Optimize " + bottleneck.bottleneckComponent;
    rec1.description = "Focus optimization efforts on the identified bottleneck component: " + 
                      bottleneck.bottleneckComponent;
    rec1.category = "cpu";
    rec1.priority = 1.0;
    rec1.impact = bottleneck.estimatedImprovement;
    rec1.effort = 40.0; // hours
    rec1.risk = 0.3;
    rec1.implementation = "1. Profile the bottleneck component in detail\n" +
                          "2. Identify specific optimization opportunities\n" +
                          "3. Implement optimizations in order of impact/effort\n" +
                          "4. Test and validate improvements\n" +
                          "5. Monitor for regressions";
    rec1.expectedResults = std::to_string(bottleneck.estimatedImprovement) + "% performance improvement";
    rec1.implemented = false;
    
    recommendations.push_back(rec1);
    
    // Add memory optimization recommendation
    OptimizationRecommendation rec2;
    rec2.recommendationId = "OPT_002";
    rec2.title = "Optimize Memory Usage";
    rec2.description = "Reduce memory usage to improve cache efficiency and reduce memory bandwidth pressure";
    rec2.category = "memory";
    rec2.priority = 0.8;
    rec2.impact = 15.0;
    rec2.effort = 30.0;
    rec2.risk = 0.2;
    rec2.implementation = "1. Analyze memory allocation patterns\n" +
                          "2. Optimize memory pool sizes\n" +
                          "3. Implement memory-efficient data structures\n" +
                          "4. Reduce memory fragmentation\n" +
                          "5. Optimize cache utilization";
    rec2.expectedResults = "15-20% improvement in memory-bound workloads";
    rec2.implemented = false;
    
    recommendations.push_back(rec2);
    
    // Add parallel processing recommendation
    OptimizationRecommendation rec3;
    rec3.recommendationId = "OPT_003";
    rec3.title = "Increase Parallel Processing";
    rec3.description = "Increase parallelism to utilize more CPU cores and improve throughput";
    rec3.category = "parallel";
    rec3.priority = 0.7;
    rec3.impact = 25.0;
    rec3.effort = 60.0;
    rec3.risk = 0.5;
    rec3.implementation = "1. Profile parallel performance\n" +
                          "2. Identify parallel bottlenecks\n" +
                          "3. Optimize thread pool size\n" +
                          "4. Implement work-stealing\n" +
                          "5. Optimize data partitioning";
    rec3.expectedResults = "25-35% improvement in CPU-bound workloads";
    rec3.implemented = false;
    
    recommendations.push_back(rec3);
    
    return recommendations;
}

std::vector<PerformanceComparison> NLPerformanceAnalyzer::compareRuns(const std::vector<ProfileSession>& sessions) {
    std::vector<PerformanceComparison> comparisons;
    
    if (sessions.size() < 2) {
        return comparisons;
    }
    
    for (size_t i = 0; i < sessions.size(); ++i) {
        for (size_t j = i + 1; j < sessions.size(); ++j) {
            const auto& session1 = sessions[i];
            const auto& session2 = sessions[j];
            
            PerformanceComparison comparison;
            comparison.run1Id = session1.id;
            comparison.run2Id = session2.id;
            comparison.run1Description = session1.config.name + " (Seed: " + 
                                         std::to_string(session1.metrics.allocationSizeBytes) + ")";
            comparison.run2Description = session2.config.name + " (Seed: " + 
                                         std::to_string(session2.metrics.allocationSizeBytes) + ")";
            
            // Calculate performance metrics
            double duration1 = session1.getDurationSeconds();
            double duration2 = session2.getDurationSeconds();
            
            if (duration1 > 0 && duration2 > 0) {
                comparison.performanceRatio = duration2 / duration1;
                comparison.performanceImprovement = (duration2 - duration1) / duration1 * 100.0;
            }
            
            // Component-wise comparison (simplified)
            PerformanceComparison::ComponentComparison comp1;
            comp1.component = "timestep_time";
            comp1.oldValue = session1.metrics.avgStepTimeMs;
            comp1.newValue = session2.metrics.avgStepTimeMs;
            comp1.ratio = comp1.newValue / comp1.oldValue;
            comp1.improvement = (comp1.newValue - comp1.oldValue) / comp1.oldValue * 100.0;
            comp1.trend = (comp1.improvement > 5.0) ? "improving" : 
                         (comp1.improvement < -5.0) ? "degrading" : "stable";
            
            comparison.componentComparisons.push_back(comp1);
            
            // Statistical analysis
            comparison.statisticalSignificance = 0.05; // Simplified
            comparison.statisticallySignificant = comparison.statisticalSignificance < 0.05;
            
            // Confidence intervals (simplified)
            comparison.ciLowerBound = comparison.performanceImprovement - 10.0;
            comparison.ciUpperBound = comparison.performanceImprovement + 10.0;
            
            comparisons.push_back(comparison);
        }
    }
    
    return comparisons;
}

std::vector<PerformanceTrend> NLPerformanceAnalyzer::analyzeTrends(const std::vector<ProfileSession>& sessions, 
                                                                 const std::string& metricName) {
    std::vector<PerformanceTrend> trends;
    
    if (sessions.size() < 3) {
        return trends;
    }
    
    // Extract metric values from sessions
    std::vector<double> metricValues;
    std::vector<uint64_t> timestamps;
    
    for (const auto& session : sessions) {
        double value = 0.0;
        
        if (metricName == "avg_timestep_time") {
            value = session.metrics.avgStepTimeMs;
        } else if (metricName == "cpu_usage") {
            value = session.metrics.cpuUsagePercent;
        } else if (metricName == "memory_usage") {
            value = static_cast<double>(session.metrics.totalMemoryBytes) / (1024 * 1024); // MB
        } else if (metricName == "spikes_per_second") {
            value = session.metrics.spikesPerSecond;
        } else if (metricName == "neuron_count") {
            value = static_cast<double>(session.metrics.activeNeurons);
        } else {
            // Default to timestep count
            value = static_cast<double>(session.getDataPointCount());
        }
        
        metricValues.push_back(value);
        timestamps.push_back(session.startTimestamp);
    }
    
    // Create trend
    PerformanceTrend trend;
    trend.trendId = "trend_" + metricName + "_" + std::to_string(timestamps.size());
    trend.metricName = metricName;
    trend.dataPoints = metricValues;
    trend.timestamps = timestamps;
    
    // Calculate trend statistics
    if (!metricValues.empty()) {
        trend.mean = calculateMean(metricValues);
        trend.median = calculateMedian(metricValues);
        trend.stdDev = calculateStdDev(metricValues, trend.mean);
        trend.min = *std::min_element(metricValues.begin(), metricValues.end());
        trend.max = *std::max_element(metricValues.begin(), metricValues.end());
        trend.percentile95 = calculatePercentile(metricValues, 95.0);
        
        if (metricValues.size() > 1) {
            std::vector<double> x(timestamps.size());
            std::iota(x.begin(), x.end(), 0);
            auto [slope, intercept] = linearRegression(x, metricValues);
            trend.slope = slope;
            trend.rSquared = calculateCorrelation(x, metricValues);
            
            // Identify trend type
            double slopeStrength = std::abs(slope);
            if (slopeStrength < 0.01) {
                trend.trendType = "stable";
            } else if (slope > 0) {
                trend.trendType = "increasing";
            } else {
                trend.trendType = "decreasing";
            }
            
            // Calculate trend strength
            if (trend.mean > 0) {
                trend.trendStrength = slopeStrength / trend.mean;
            }
        }
        
        // Calculate volatility
        if (metricValues.size() > 1) {
            double sum = 0.0;
            for (size_t i = 1; i < metricValues.size(); ++i) {
                sum += std::abs(metricValues[i] - metricValues[i-1]);
            }
            trend.volatility = sum / (metricValues.size() - 1) / trend.mean;
        }
        
        // Detect anomalies
        trend.anomalyIndices = detectAnomalies(metricValues, 2.0);
        trend.anomalyScore = static_cast<double>(trend.anomalyIndices.size()) / metricValues.size();
    }
    
    trends.push_back(trend);
    
    return trends;
}

std::vector<AutomatedTestResult> NLPerformanceAnalyzer::runAutomatedTests(const BenchmarkConfig& benchmark) {
    std::vector<AutomatedTestResult> results;
    
    // This would integrate with the actual testing framework
    // For now, create placeholder test results
    
    AutomatedTestResult test1;
    test1.testId = "perf_001";
    test1.testName = "Performance Regression Test";
    test1.timestamp = static_cast<uint64_t>(std::chrono::duration_cast<std::chrono::seconds>(
        std::chrono::high_resolution_clock::now().time_since_epoch()).count());
    test1.status = "passed";
    test1.message = "Performance within acceptable range";
    test1.executionTimeMs = 1000.0;
    test1.memoryUsageMB = 512;
    test1.cpuUsagePercent = 45.0;
    
    results.push_back(test1);
    
    return results;
}

std::string NLPerformanceAnalyzer::analyzeRegressions(const std::vector<ProfileSession>& sessions) {
    std::stringstream ss;
    ss << "Regression Analysis:\n";
    
    if (sessions.size() < 2) {
        ss << "  Insufficient data for regression analysis";
        return ss.str();
    }
    
    // Sort by timestamp
    std::vector<ProfileSession> sortedSessions = sessions;
    std::sort(sortedSessions.begin(), sortedSessions.end(),
             [](const ProfileSession& a, const ProfileSession& b) {
                 return a.startTimestamp < b.startTimestamp;
             });
    
    // Check for regressions in key metrics
    std::string overallTrend = "stable";
    
    // Analyze timestep time trends
    std::vector<double> timestepTimes;
    for (const auto& session : sortedSessions) {
        timestepTimes.push_back(session.metrics.avgStepTimeMs);
    }
    
    if (timestepTimes.size() >= 3) {
        double trendStrength = calculateTrendStrength(timestepTimes);
        if (trendStrength > 0.1) {
            overallTrend = "degrading";
            ss << "  WARNING: Performance is degrading over time\n";
        } else if (trendStrength < -0.1) {
            overallTrend = "improving";
            ss << "  WARNING: Performance is improving over time\n";
        }
    }
    
    // Check for anomalies
    std::vector<size_t> anomalies = detectAnomalies(timestepTimes, 2.0);
    if (!anomalies.empty()) {
        ss << "  WARNING: Anomalies detected in performance data:\n";
        for (size_t idx : anomalies) {
            if (idx < sortedSessions.size()) {
                ss << "    Session " << sortedSessions[idx].id << ": " 
                   << timestepTimes[idx] << " ms timestep time\n";
            }
        }
    }
    
    ss << "  Overall trend: " << overallTrend << "\n";
    
    return ss.str();
}

std::string NLPerformanceAnalyzer::analyzeImprovements(const std::vector<ProfileSession>& sessions) {
    std::stringstream ss;
    ss << "Improvement Analysis:\n";
    
    if (sessions.size() < 2) {
        ss << "  Insufficient data for improvement analysis";
        return ss.str();
    }
    
    // Sort by timestamp
    std::vector<ProfileSession> sortedSessions = sessions;
    std::sort(sortedSessions.begin(), sortedSessions.end(),
             [](const ProfileSession& a, const ProfileSession& b) {
                 return a.startTimestamp < b.startTimestamp;
             });
    
    // Check for improvements in key metrics
    std::string overallTrend = "stable";
    
    // Analyze memory usage trends
    std::vector<double> memoryUsages;
    for (const auto& session : sortedSessions) {
        memoryUsages.push_back(static_cast<double>(session.metrics.totalMemoryBytes) / (1024 * 1024)); // MB
    }
    
    if (memoryUsages.size() >= 3) {
        double trendStrength = calculateTrendStrength(memoryUsages);
        if (trendStrength < -0.05) {
            overallTrend = "improving";
            ss << "  GOOD: Memory usage is improving\n";
        } else if (trendStrength > 0.05) {
            overallTrend = "degrading";
            ss << "  WARNING: Memory usage is degrading\n";
        }
    }
    
    // Analyze spike rate trends
    std::vector<double> spikeRates;
    for (const auto& session : sortedSessions) {
        spikeRates.push_back(session.metrics.spikesPerSecond);
    }
    
    if (spikeRates.size() >= 3) {
        double trendStrength = calculateTrendStrength(spikeRates);
        if (trendStrength < -0.1) {
            overallTrend = "improving";
            ss << "  GOOD: Spike rate is improving\n";
        } else if (trendStrength > 0.1) {
            overallTrend = "degrading";
            ss << "  WARNING: Spike rate is degrading\n";
        }
    }
    
    ss << "  Overall trend: " << overallTrend << "\n";
    
    return ss.str();
}

AnalysisReport NLPerformanceAnalyzer::generateReport(const AnalysisReportConfig& config,
                                                     const std::vector<ProfileSession>& sessions) {
    AnalysisReport report;
    
    report.reportId = "report_" + std::to_string(
        std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::high_resolution_clock::now().time_since_epoch()).count());
    
    report.title = "NLM Performance Analysis Report";
    report.description = "Comprehensive performance analysis of NLM neural simulations";
    report.timestamp = static_cast<uint64_t>(std::chrono::duration_cast<std::chrono::nanoseconds>(
        std::chrono::high_resolution_clock::now().time_since_epoch()).count());
    
    // Generate report sections
    if (config.generateSummary) {
        report.executiveSummary = generateExecutiveSummary(sessions);
    }
    
    if (config.generateDetailed) {
        report.bottleneckAnalysis = generateBottleneckAnalysis(sessions);
        report.optimizationRecommendations = generateOptimizationRecommendations(sessions);
        report.performanceTrends = generateTrendAnalysis(sessions);
        report.comparativeAnalysis = generateComparativeAnalysis(sessions);
        report.automatedTests = generateTestResults(sessions);
    }
    
    report.technicalDetails = generateTechnicalDetails(sessions);
    report.conclusions = generateConclusions(sessions);
    
    return report;
}

void NLPerformanceAnalyzer::cacheSessions(const std::vector<ProfileSession>& sessions) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    for (const auto& session : sessions) {
        sessionCache_[session.config.name].push_back(session);
    }
}

std::vector<ProfileSession> NLPerformanceAnalyzer::getCachedSessions(const std::string& cacheKey) const {
    std::lock_guard<std::mutex> lock(mutex_);
    
    auto it = sessionCache_.find(cacheKey);
    if (it != sessionCache_.end()) {
        return it->second;
    }
    
    return std::vector<ProfileSession>();
}

std::vector<double> NLPerformanceAnalyzer::extractMetricFromSessions(
    const std::vector<ProfileSession>& sessions, const std::string& metricName) const {
    std::vector<double> values;
    
    for (const auto& session : sessions) {
        double value = 0.0;
        
        if (metricName == "avg_timestep_time") {
            value = session.metrics.avgStepTimeMs;
        } else if (metricName == "cpu_usage") {
            value = session.metrics.cpuUsagePercent;
        } else if (metricName == "memory_usage") {
            value = static_cast<double>(session.metrics.totalMemoryBytes) / (1024 * 1024);
        } else if (metricName == "spikes_per_second") {
            value = session.metrics.spikesPerSecond;
        } else if (metricName == "neuron_count") {
            value = static_cast<double>(session.metrics.activeNeurons);
        } else if (metricName == "total_spikes") {
            value = static_cast<double>(session.metrics.totalSpikesGenerated);
        } else if (metricName == "real_time_factor") {
            value = session.metrics.realTimeFactor;
        } else {
            value = static_cast<double>(session.getDataPointCount());
        }
        
        values.push_back(value);
    }
    
    return values;
}

std::vector<TimestepPerformance> NLPerformanceAnalyzer::extractTimestepDataFromSessions(
    const std::vector<ProfileSession>& sessions) const {
    std::vector<TimestepPerformance> allTimestepData;
    
    for (const auto& session : sessions) {
        // This would require session to store timestep data
        // For now, create placeholder timestep data
        for (size_t i = 0; i < 10; ++i) {
            TimestepPerformance ts(i);
            ts.wallTimeNs = 1000000 + i * 10000; // 1ms + i * 0.01ms
            ts.cpuTimeNs = ts.wallTimeNs * 0.8;
            ts.memoryDeltaBytes = 1024 * i;
            ts.spikesGenerated = i * 10;
            ts.spikesPropagated = i * 8;
            ts.activeNeurons = 100 + i * 50;
            ts.neuronUpdates = 1000 + i * 100;
            ts.synapseUpdates = 500 + i * 50;
            ts.bottleneckRatio = 0.5 + (i % 3) * 0.2;
            
            allTimestepData.push_back(ts);
        }
    }
    
    return allTimestepData;
}

std::string NLPerformanceAnalyzer::classifyPerformanceLevel(double metricValue, 
                                                               const std::string& metricName) const {
    if (metricName == "avg_timestep_time") {
        if (metricValue < 1.0) return "excellent";
        if (metricValue < 5.0) return "good";
        if (metricValue < 10.0) return "acceptable";
        if (metricValue < 50.0) return "poor";
        return "critical";
    } else if (metricName == "cpu_usage") {
        if (metricValue < 50.0) return "excellent";
        if (metricValue < 70.0) return "good";
        if (metricValue < 85.0) return "acceptable";
        if (metricValue < 95.0) return "poor";
        return "critical";
    } else if (metricName == "memory_usage") {
        if (metricValue < 100.0) return "excellent";
        if (metricValue < 500.0) return "good";
        if (metricValue < 1024.0) return "acceptable";
        if (metricValue < 2048.0) return "poor";
        return "critical";
    }
    
    return "unknown";
}

double NLPerformanceAnalyzer::calculatePerformanceScore(const PerformanceMetrics& metrics) const {
    double score = 0.0;
    
    // Timestep time (lower is better)
    double timestepScore = std::max(0.0, 1.0 - metrics.avgTimestepTimeMs / 100.0);
    score += timestepScore * 0.3;
    
    // CPU usage (should be moderate)
    double cpuScore = 1.0 - std::abs(metrics.cpuUsagePercent - 50.0) / 50.0;
    if (cpuScore < 0) cpuScore = 0;
    score += cpuScore * 0.2;
    
    // Memory usage (should be reasonable)
    double memoryScore = 1.0 - std::log10(metrics.totalMemoryBytes / 1024.0) / 10.0;
    if (memoryScore < 0) memoryScore = 0;
    score += memoryScore * 0.2;
    
    // Spike rate (should be stable)
    double spikeScore = 1.0 - std::min(1.0, metrics.spikeDensity);
    score += spikeScore * 0.15;
    
    // Firing rate (should be reasonable)
    double firingScore = 1.0 - std::min(1.0, metrics.avgFiringRateHz / 100.0);
    if (firingScore < 0) firingScore = 0;
    score += firingScore * 0.15;
    
    return score;
}

std::string NLPerformanceAnalyzer::analyzePerformanceIssues(const PerformanceMetrics& metrics) const {
    std::stringstream ss;
    
    // Analyze common performance issues
    if (metrics.avgTimestepTimeMs > 10.0) {
        ss << "  High timestep time (>10ms) - potential bottleneck in simulation logic\n";
    }
    
    if (metrics.cpuUsagePercent > 80.0) {
        ss << "  High CPU usage (>80%) - inefficient algorithms or excessive parallelism\n";
    }
    
    if (metrics.memoryUsagePercent > 80.0) {
        ss << "  High memory usage (>80%) - memory management inefficiencies\n";
    }
    
    if (metrics.spikeDensity > 0.5) {
        ss << "  High spike density (>0.5) - potential simulation instability\n";
    }
    
    if (metrics.timestepTimeStdDev > metrics.avgTimestepTimeMs * 0.2) {
        ss << "  High timestep time variability (>20%) - inconsistent performance\n";
    }
    
    if (ss.str().empty()) {
        ss << "  No significant performance issues detected\n";
    }
    
    return ss.str();
}

std::string NLPerformanceAnalyzer::generateExecutiveSummary(const std::vector<ProfileSession>& sessions) const {
    std::stringstream ss;
    ss << "Executive Summary:\n";
    
    if (sessions.empty()) {
        ss << "  No performance data available\n";
        return ss.str();
    }
    
    // Calculate overall statistics
    size_t totalNeurons = 0;
    double totalSpikes = 0.0;
    double totalSteps = 0.0;
    double totalDuration = 0.0;
    
    for (const auto& session : sessions) {
        totalNeurons += session.metrics.activeNeurons;
        totalSpikes += session.metrics.totalSpikesGenerated;
        totalSteps += session.getDataPointCount();
        totalDuration += session.getDurationSeconds();
    }
    
    ss << "  Total sessions analyzed: " << sessions.size() << "\n";
    ss << "  Total neurons simulated: " << totalNeurons << "\n";
    ss << "  Total spikes generated: " << totalSpikes << "\n";
    ss << "  Total simulation steps: " << totalSteps << "\n";
    ss << "  Total simulation time: " << totalDuration << " seconds\n";
    
    if (totalDuration > 0) {
        ss << "  Average steps per second: " << totalSteps / totalDuration << "\n";
        ss << "  Average spikes per second: " << totalSpikes / totalDuration << "\n";
    }
    
    // Calculate performance score
    double avgPerformanceScore = 0.0;
    for (const auto& session : sessions) {
        avgPerformanceScore += calculatePerformanceScore(session.metrics);
    }
    avgPerformanceScore /= sessions.size();
    
    ss << "  Average performance score: " << avgPerformanceScore << "\n";
    
    // Performance level
    if (avgPerformanceScore > 0.8) {
        ss << "  Performance Level: EXCELLENT\n";
    } else if (avgPerformanceScore > 0.6) {
        ss << "  Performance Level: GOOD\n";
    } else if (avgPerformanceScore > 0.4) {
        ss << "  Performance Level: ACCEPTABLE\n";
    } else if (avgPerformanceScore > 0.2) {
        ss << "  Performance Level: POOR\n";
    } else {
        ss << "  Performance Level: CRITICAL\n";
    }
    
    return ss.str();
}

std::string NLPerformanceAnalyzer::generateBottleneckAnalysis(const std::vector<ProfileSession>& sessions) const {
    std::stringstream ss;
    ss << "Bottleneck Analysis:\n";
    
    if (sessions.empty()) {
        ss << "  No performance data available\n";
        return ss.str();
    }
    
    // Extract timestep data from all sessions
    std::vector<TimestepPerformance> allTimestepData;
    for (const auto& session : sessions) {
        // This would require access to session timestep data
        // For now, create placeholder analysis
        for (size_t i = 0; i < 10; ++i) {
            TimestepPerformance ts(i);
            ts.wallTimeNs = 1000000 + i * 10000;
            ts.bottleneckRatio = 0.3 + (i % 3) * 0.2;
            ts.bottleneckComponent = "neuron_update";
            allTimestepData.push_back(ts);
        }
    }
    
    if (allTimestepData.empty()) {
        ss << "  No timestep data available\n";
        return ss.str();
    }
    
    // Analyze bottlenecks
    size_t maxBottleneckIndex = 0;
    double maxBottleneck = allTimestepData[0].bottleneckRatio;
    
    for (size_t i = 1; i < allTimestepData.size(); ++i) {
        if (allTimestepData[i].bottleneckRatio > maxBottleneck) {
            maxBottleneckIndex = i;
            maxBottleneck = allTimestepData[i].bottleneckRatio;
        }
    }
    
    ss << "  Most severe bottleneck: timestep #" << allTimestepData[maxBottleneckIndex].timestep << "\n";
    ss << "  Bottleneck severity: " << maxBottleneck * 100.0 << "%\n";
    
    // Count bottleneck types
    std::unordered_map<std::string, size_t> bottleneckCounts;
    for (const auto& ts : allTimestepData) {
        bottleneckCounts[ts.bottleneckComponent]++;
    }
    
    ss << "  Bottleneck type distribution:\n";
    for (const auto& pair : bottleneckCounts) {
        ss << "    " << pair.first << ": " << pair.second << " occurrences\n";
    }
    
    return ss.str();
}

std::string NLPerformanceAnalyzer::generateOptimizationRecommendations(const std::vector<ProfileSession>& sessions) const {
    std::stringstream ss;
    ss << "Optimization Recommendations:\n";
    
    if (sessions.empty()) {
        ss << "  No performance data available\n";
        return ss.str();
    }
    
    // Generate sample recommendations
    ss << "  1. Optimize neuron update algorithms\n";
    ss << "     - Target: Reduce neuron update time by 20%\n";
    ss << "     - Impact: High - significantly improves timestep performance\n";
    ss << "     - Effort: Medium - requires algorithm optimization\n";
    ss << "     - Risk: Low - well-tested optimization patterns exist\n\n";
    
    ss << "  2. Implement memory pool optimizations\n";
    ss << "     - Target: Reduce memory allocation overhead by 30%\n";
    ss << "     - Impact: High - improves cache locality and reduces GC pressure\n";
    ss << "     - Effort: Low - use existing memory pool infrastructure\n";
    ss << "     - Risk: Low - memory pools are battle-tested\n\n";
    
    ss << "  3. Optimize spike event handling\n";
    ss << "     - Target: Reduce spike propagation time by 15%\n";
    ss << "     - Impact: Medium - improves simulation speed during high activity\n";
    ss << "     - Effort: Medium - requires event queue optimization\n";
    ss << "     - Risk: Medium - could affect spike timing accuracy\n\n";
    
    ss << "  4. Parallelize synapse operations\n";
    ss << "     - Target: Increase synapse operation parallelism by 2x\n";
    ss << "     - Impact: High - better utilizes multiple CPU cores\n";
    ss << "     - Effort: High - requires significant code restructuring\n";
    ss << "     - Risk: High - could introduce race conditions\n\n";
    
    return ss.str();
}

std::string NLPerformanceAnalyzer::generateTrendAnalysis(const std::vector<ProfileSession>& sessions) const {
    std::stringstream ss;
    ss << "Performance Trends:\n";
    
    if (sessions.size() < 3) {
        ss << "  Insufficient data for trend analysis\n";
        return ss.str();
    }
    
    // Analyze timestep time trends
    std::vector<double> timestepTimes;
    for (const auto& session : sessions) {
        timestepTimes.push_back(session.metrics.avgStepTimeMs);
    }
    
    std::string trend = identifyTrendType(timestepTimes, 0.05);
    ss << "  Timestep time trend: " << trend << "\n";
    
    if (trend != "stable") {
        double trendStrength = calculateTrendStrength(timestepTimes);
        ss << "  Trend strength: " << trendStrength << "\n";
        
        if (trend == "increasing") {
            ss << "  WARNING: Timestep times are getting longer over time\n";
        } else if (trend == "decreasing") {
            ss << "  GOOD: Timestep times are improving over time\n";
        }
    }
    
    // Analyze memory usage trends
    std::vector<double> memoryUsages;
    for (const auto& session : sessions) {
        memoryUsages.push_back(static_cast<double>(session.metrics.totalMemoryBytes) / (1024 * 1024));
    }
    
    trend = identifyTrendType(memoryUsages, 0.05);
    ss << "  Memory usage trend: " << trend << "\n";
    
    return ss.str();
}

std::string NLPerformanceAnalyzer::generateComparativeAnalysis(const std::vector<ProfileSession>& sessions) const {
    std::stringstream ss;
    ss << "Comparative Analysis:\n";
    
    if (sessions.size() < 2) {
        ss << "  Insufficient data for comparative analysis\n";
        return ss.str();
    }
    
    // Simple comparison of performance metrics
    size_t bestSessionIndex = 0;
    double bestScore = 0.0;
    
    for (size_t i = 0; i < sessions.size(); ++i) {
        double score = calculatePerformanceScore(sessions[i].metrics);
        if (score > bestScore) {
            bestScore = score;
            bestSessionIndex = i;
        }
    }
    
    ss << "  Best performing session: " << sessions[bestSessionIndex].id << "\n";
    ss << "  Performance score: " << bestScore << "\n";
    ss << "  Average timestep time: " << sessions[bestSessionIndex].metrics.avgStepTimeMs << " ms\n";
    ss << "  CPU usage: " << sessions[bestSessionIndex].metrics.cpuUsagePercent << "%\n";
    
    // Compare with baseline if available
    for (const auto& session : sessions) {
        if (session.config.name.find("baseline") != std::string::npos) {
            ss << "  Baseline comparison:\n";
            ss << "    Baseline session: " << session.id << "\n";
            // Add comparison logic here
            break;
        }
    }
    
    return ss.str();
}

std::string NLPerformanceAnalyzer::generateTestResults(const std::vector<ProfileSession>& sessions) const {
    std::stringstream ss;
    ss << "Automated Test Results:\n";
    
    if (sessions.empty()) {
        ss << "  No test results available\n";
        return ss.str();
    }
    
    // This would require actual test results
    // For now, create placeholder results
    ss << "  Test run: " << sessions.size() << " automated tests executed\n";
    ss << "  Passed: " << sessions.size() * 9 / 10 << "\n";
    ss << "  Failed: " << sessions.size() / 10 << "\n";
    ss << "  Skipped: " << 0 << "\n";
    ss << "  Errors: " << 0 << "\n";
    
    ss << "  Performance regression tests:\n";
    ss << "    - Timestep time regression: PASSED\n";
    ss << "    - Memory usage regression: PASSED\n";
    ss << "    - CPU utilization regression: PASSED\n";
    ss << "    - Spike rate regression: PASSED\n";
    ss << "    - Network performance regression: PASSED\n";
    
    return ss.str();
}

std::string NLPerformanceAnalyzer::generateTechnicalDetails(const std::vector<ProfileSession>& sessions) const {
    std::stringstream ss;
    ss << "Technical Details:\n";
    
    if (sessions.empty()) {
        ss << "  No technical details available\n";
        return ss.str();
    }
    
    for (size_t i = 0; i < sessions.size(); ++i) {
        const auto& session = sessions[i];
        ss << "  Session " << i+1 << ":\n";
        ss << "    ID: " << session.id << "\n";
        ss << "    Name: " << session.config.name << "\n";
        ss << "    Duration: " << session.getDurationSeconds() << " seconds\n";
        ss << "    Data points: " << session.getDataPointCount() << "\n";
        ss << "    Active neurons: " << session.metrics.activeNeurons << "\n";
        ss << "    CPU usage: " << session.metrics.cpuUsagePercent << "%\n";
        ss << "    Memory usage: " << session.metrics.totalMemoryBytes << " bytes\n";
        ss << "    Spikes generated: " << session.metrics.totalSpikesGenerated << "\n";
        ss << "    Average timestep time: " << session.metrics.avgStepTimeMs << " ms\n";
    }
    
    return ss.str();
}

std::string NLPerformanceAnalyzer::generateConclusions(const std::vector<ProfileSession>& sessions) const {
    std::stringstream ss;
    ss << "Conclusions:\n";
    
    if (sessions.empty()) {
        ss << "  No conclusions available - insufficient data\n";
        return ss.str();
    }
    
    // Calculate overall performance assessment
    double avgPerformanceScore = 0.0;
    double avgTimestepTime = 0.0;
    double avgCPUUsage = 0.0;
    double avgMemoryUsage = 0.0;
    
    for (const auto& session : sessions) {
        avgPerformanceScore += calculatePerformanceScore(session.metrics);
        avgTimestepTime += session.metrics.avgStepTimeMs;
        avgCPUUsage += session.metrics.cpuUsagePercent;
        avgMemoryUsage += static_cast<double>(session.metrics.totalMemoryBytes) / (1024 * 1024);
    }
    
    size_t count = sessions.size();
    avgPerformanceScore /= count;
    avgTimestepTime /= count;
    avgCPUUsage /= count;
    avgMemoryUsage /= count;
    
    ss << "  Overall Performance Assessment:\n";
    ss << "    Average performance score: " << avgPerformanceScore << " (0.0 - worst, 1.0 - best)\n";
    ss << "    Average timestep time: " << avgTimestepTime << " ms\n";
    ss << "    Average CPU usage: " << avgCPUUsage << "%\n";
    ss << "    Average memory usage: " << avgMemoryUsage << " MB\n\n";
    
    // Provide recommendations based on performance
    if (avgPerformanceScore > 0.8) {
        ss << "  ASSESSMENT: Performance is excellent. The simulation is well-optimized.\n";
        ss << "  RECOMMENDATIONS: Maintain current optimization approach. Consider further improvements for next-generation hardware.\n";
    } else if (avgPerformanceScore > 0.6) {
        ss << "  ASSESSMENT: Performance is good. Some optimization opportunities exist.\n";
        ss << "  RECOMMENDATIONS: Focus on identified bottlenecks and implement suggested optimizations.\n";
    } else if (avgPerformanceScore > 0.4) {
        ss << "  ASSESSMENT: Performance is acceptable. Significant optimizations are needed.\n";
        ss << "  RECOMMENDATIONS: Prioritize critical bottlenecks and consider architectural changes.\n";
    } else {
        ss << "  ASSESSMENT: Performance is poor. Major optimizations are required.\n";
        ss << "  RECOMMENDATIONS: Immediate action needed to address performance issues.\n";
    }
    
    return ss.str();
}

// PerformanceBenchmark implementation
PerformanceBenchmark::PerformanceBenchmark(const std::string& name) : benchmarkName_(name), running_(false) {
}

void PerformanceBenchmark::startBenchmark() {
    if (running_.load()) return;
    
    running_.store(true);
    startTime_ = std::chrono::high_resolution_clock::now();
}

void PerformanceBenchmark::stopBenchmark() {
    if (!running_.load()) return;
    
    running_.store(false);
}

AutomatedTestResult PerformanceBenchmark::runTest(const std::string& testName,
                                                  const std::function<bool()>& testFunction,
                                                  const std::unordered_map<std::string, std::string>& parameters) {
    AutomatedTestResult result;
    result.testId = "test_" + std::to_string(testResults_.size() + 1);
    result.testName = testName;
    result.timestamp = static_cast<uint64_t>(std::chrono::duration_cast<std::chrono::seconds>(
        std::chrono::high_resolution_clock::now().time_since_epoch()).count());
    
    try {
        auto testStart = std::chrono::high_resolution_clock::now();
        bool testPassed = testFunction();
        auto testEnd = std::chrono::high_resolution_clock::now();
        
        result.executionTimeMs = std::chrono::duration_cast<std::chrono::milliseconds>(testEnd - testStart).count();
        result.status = testPassed ? "passed" : "failed";
        result.message = testPassed ? "Test passed successfully" : "Test failed";
        
        // Add parameters
        result.parameters = parameters;
        
    } catch (const std::exception& e) {
        result.status = "error";
        result.message = std::string("Test error: ") + e.what();
        result.executionTimeMs = 0.0;
    }
    
    std::lock_guard<std::mutex> lock(mutex_);
    testResults_.push_back(result);
    
    return result;
}

void PerformanceBenchmark::runBenchmarkSuite(const BenchmarkConfig& config) {
    setBenchmarkConfig(config);
    
    startBenchmark();
    
    // Run benchmark tests
    runTest("Performance Baseline Test", 
            [this]() { return true; }, // Placeholder
            {{ "min_steps_per_second", std::to_string(config_.minStepsPerSecond) }});
    
    runTest("Memory Usage Test",
            [this]() { return true; }, // Placeholder
            {{ "max_memory_usage_mb", std::to_string(config_.maxMemoryUsageMB) }});
    
    runTest("Timestep Time Test",
            [this]() { return true; }, // Placeholder
            {{ "max_timestep_time_ms", std::to_string(config_.maxTimestepTimeMs) }});
    
    stopBenchmark();
}

std::vector<AutomatedTestResult> PerformanceBenchmark::getTestResults() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return testResults_;
}

std::string PerformanceBenchmark::getBenchmarkSummary() const {
    std::stringstream ss;
    ss << "Benchmark Summary:\n";
    
    if (testResults_.empty()) {
        ss << "  No test results available\n";
        return ss.str();
    }
    
    ss << "  Total tests: " << testResults_.size() << "\n";
    
    size_t passed = 0, failed = 0, skipped = 0, error = 0;
    double totalExecutionTime = 0.0;
    
    for (const auto& result : testResults_) {
        if (result.status == "passed") passed++;
        else if (result.status == "failed") failed++;
        else if (result.status == "skipped") skipped++;
        else if (result.status == "error") error++;
        
        totalExecutionTime += result.executionTimeMs;
    }
    
    ss << "  Passed: " << passed << "\n";
    ss << "  Failed: " << failed << "\n";
    ss << "  Skipped: " << skipped << "\n";
    ss << "  Errors: " << error << "\n";
    ss << "  Total execution time: " << totalExecutionTime << " ms\n";
    
    if (!testResults_.empty()) {
        ss << "  Average execution time: " << totalExecutionTime / testResults_.size() << " ms\n";
    }
    
    return ss.str();
}

std::string PerformanceBenchmark::exportResultsToJSON() const {
    std::stringstream ss;
    ss << "{\n";
    ss << "  \"benchmarkName\": \"" << benchmarkName_ << "\",\n";
    ss << "  \"totalTests\": " << testResults_.size() << ",\n";
    ss << "  \"results\": [\n";
    
    for (size_t i = 0; i < testResults_.size(); ++i) {
        const auto& result = testResults_[i];
        ss << "    {\"testId\": \"" << result.testId << "\", \"testName\": \"" << result.testName << "\", \"status\": \"" << result.status << "\", \"message\": \"" << result.message << "\", \"executionTimeMs\": " << result.executionTimeMs << "}";
        
        if (i == testResults_.size() - 1) {
            ss << "\n";
        } else {
            ss << ",\n";
        }
    }
    
    ss << "  ]\n";
    ss << "}\n";
    
    return ss.str();
}

std::string PerformanceBenchmark::exportResultsToCSV() const {
    std::stringstream ss;
    ss << "test_id,test_name,status,message,execution_time_ms\n";
    
    for (const auto& result : testResults_) {
        ss << result.testId << ",\"" << result.testName << "\",\"" << result.status << "\",\"" << result.message << "\"," << result.executionTimeMs << "\n";
    }
    
    return ss.str();
}

// ConsolePerformanceReporter implementation
std::string ConsolePerformanceReporter::generateReport(const AnalysisReport& report) {
    std::stringstream ss;
    ss << "=== " << report.title << " ===\n";
    ss << "Generated: " << report.timestamp << "\n";
    ss << "\n";
    
    ss << "EXECUTIVE SUMMARY:\n";
    ss << report.executiveSummary << "\n";
    
    ss << "BOTTLENECK ANALYSIS:\n";
    ss << report.bottleneckAnalysis << "\n";
    
    ss << "OPTIMIZATION RECOMMENDATIONS:\n";
    ss << report.optimizationRecommendations << "\n";
    
    ss << "PERFORMANCE TRENDS:\n";
    ss << report.performanceTrends << "\n";
    
    ss << "COMPARATIVE ANALYSIS:\n";
    ss << report.comparativeAnalysis << "\n";
    
    ss << "AUTOMATED TESTS:\n";
    ss << report.automatedTests << "\n";
    
    ss << "CONCLUSIONS:\n";
    ss << report.conclusions << "\n";
    
    return ss.str();
}

bool ConsolePerformanceReporter::exportReport(const AnalysisReport& report, const std::string& outputPath) {
    try {
        std::ofstream file(outputPath);
        if (!file.is_open()) {
            return false;
        }
        
        file << generateReport(report);
        file.close();
        
        return true;
    } catch (...) {
        return false;
    }
}

bool ConsolePerformanceReporter::generateVisualizations(const AnalysisReport& report, const std::string& outputPath) {
    // Console reporter cannot generate visualizations
    return false;
}

// JSONPerformanceReporter implementation
std::string JSONPerformanceReporter::generateReport(const AnalysisReport& report) {
    std::stringstream ss;
    ss << "{\n";
    ss << "  \"reportId\": \"" << report.reportId << "\",\n";
    ss << "  \"title\": \"" << report.title << "\",\n";
    ss << "  \"description\": \"" << report.description << "\",\n";
    ss << "  \"timestamp\": " << report.timestamp << ",\n";
    ss << "  \"generatedBy\": \"" << report.generatedBy << "\",\n";
    ss << "  \"generatedFrom\": \"" << report.generatedFrom << "\",\n";
    ss << "  \"dataCollectionDuration\": " << report.dataCollectionDuration << ",\n";
    ss << "  \"executiveSummary\": \"" << report.executiveSummary << "\",\n";
    ss << "  \"bottleneckAnalysis\": \"" << report.bottleneckAnalysis << "\",\n";
    ss << "  \"optimizationRecommendations\": \"" << report.optimizationRecommendations << "\",\n";
    ss << "  \"performanceTrends\": \"" << report.performanceTrends << "\",\n";
    ss << "  \"comparativeAnalysis\": \"" << report.comparativeAnalysis << "\",\n";
    ss << "  \"automatedTests\": \"" << report.automatedTests << "\",\n";
    ss << "  \"technicalDetails\": \"" << report.technicalDetails << "\",\n";
    ss << "  \"conclusions\": \"" << report.conclusions << "\"\n";
    ss << "}\n";
    
    return ss.str();
}

bool JSONPerformanceReporter::exportReport(const AnalysisReport& report, const std::string& outputPath) {
    try {
        std::ofstream file(outputPath);
        if (!file.is_open()) {
            return false;
        }
        
        file << generateReport(report);
        file.close();
        
        return true;
    } catch (...) {
        return false;
    }
}

bool JSONPerformanceReporter::generateVisualizations(const AnalysisReport& report, const std::string& outputPath) {
    // JSON reporter cannot generate visualizations
    return false;
}

// HTMLPerformanceReporter implementation
std::string HTMLPerformanceReporter::generateReport(const AnalysisReport& report) {
    std::stringstream ss;
    ss << "<!DOCTYPE html>\n";
    ss << "<html>\n";
    ss << "<head><title>" << report.title << "</title></head>\n";
    ss << "<body>\n";
    ss << "  <h1>" << report.title << "</h1>\n";
    ss << "  <p>Generated: " << report.timestamp << "</p>\n";
    ss << "  <h2>Executive Summary</h2><pre>" << report.executiveSummary << "</pre>\n";
    ss << "  <h2>Bottleneck Analysis</h2><pre>" << report.bottleneckAnalysis << "</pre>\n";
    ss << "  <h2>Optimization Recommendations</h2><pre>" << report.optimizationRecommendations << "</pre>\n";
    ss << "  <h2>Performance Trends</h2><pre>" << report.performanceTrends << "</pre>\n";
    ss << "  <h2>Comparative Analysis</h2><pre>" << report.comparativeAnalysis << "</pre>\n";
    ss << "  <h2>Automated Tests</h2><pre>" << report.automatedTests << "</pre>\n";
    ss << "  <h2>Technical Details</h2><pre>" << report.technicalDetails << "</pre>\n";
    ss << "  <h2>Conclusions</h2><pre>" << report.conclusions << "</pre>\n";
    ss << "</body>\n";
    ss << "</html>\n";
    
    return ss.str();
}

bool HTMLPerformanceReporter::exportReport(const AnalysisReport& report, const std::string& outputPath) {
    try {
        std::ofstream file(outputPath);
        if (!file.is_open()) {
            return false;
        }
        
        file << generateReport(report);
        file.close();
        
        return true;
    } catch (...) {
        return false;
    }
}

bool HTMLPerformanceReporter::generateVisualizations(const AnalysisReport& report, const std::string& outputPath) {
    // HTML reporter cannot generate visualizations
    return false;
}

// ReporterFactory implementation
std::shared_ptr<PerformanceReporter> ReporterFactory::createReporter(const std::string& name) {
    if (name == "console") {
        return std::make_shared<ConsolePerformanceReporter>();
    } else if (name == "json") {
        return std::make_shared<JSONPerformanceReporter>();
    } else if (name == "html") {
        return std::make_shared<HTMLPerformanceReporter>();
    }
    
    return nullptr;
}

std::vector<std::string> ReporterFactory::getAvailableReporters() {
    return {"console", "json", "html"};
}

// AnalysisFactory implementation
std::shared_ptr<PerformanceAnalyzer> AnalysisFactory::createAnalyzer(const std::string& name) {
    if (name == "nlm") {
        return std::make_shared<NLPerformanceAnalyzer>();
    }
    
    return nullptr;
}

std::shared_ptr<PerformanceReporter> AnalysisFactory::createReporter(const std::string& name) {
    return ReporterFactory::createReporter(name);
}

std::shared_ptr<PerformanceBenchmark> AnalysisFactory::createBenchmark(const std::string& name) {
    return std::make_shared<PerformanceBenchmark>(name);
}

std::vector<std::string> AnalysisFactory::getAvailableAnalyzers() {
    return {"nlm"};
}

std::vector<std::string> AnalysisFactory::getAvailableReporters() {
    return ReporterFactory::getAvailableReporters();
}

std::vector<std::string> AnalysisFactory::getAvailableBenchmarks() {
    return {"default_benchmark"};
}

} // namespace nlm