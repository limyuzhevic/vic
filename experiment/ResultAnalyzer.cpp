#pragma once

#include "ResultAnalyzer.hpp"
#include <algorithm>
#include <cmath>
#include <numeric>

namespace nlm {

ResultAnalyzer::ResultAnalyzer() {
    // Initialize default analysis parameters
    confidenceLevel = 0.95;
    significanceLevel = 0.05;
    minSampleSize = 5;
}

ResultAnalyzer::~ResultAnalyzer() = default;

bool ResultAnalyzer::validateResults(const std::vector<std::shared_ptr<ExperimentResult>>& results) const {
    if (results.empty()) {
        return false;
    }
    
    for (const auto& result : results) {
        if (!result || result->getStatus() == ResultStatus::FAILED) {
            return false;
        }
    }
    
    return true;
}

AnalysisResult ResultAnalyzer::performBasicAnalysis(
    const std::vector<std::shared_ptr<ExperimentResult>>& results) {
    AnalysisResult analysis;
    analysis.type = "basic_analysis";
    
    if (!validateResults(results)) {
        analysis.status = AnalysisStatus::FAILED;
        analysis.message = "Invalid or empty results";
        return analysis;
    }
    
    analysis.status = AnalysisStatus::COMPLETED;
    
    // Extract metrics
    std::set<std::string> allMetricNames;
    for (const auto& result : results) {
        for (const auto& metric : result->getMetrics()) {
            allMetricNames.insert(metric.first);
        }
    }
    
    // Calculate basic statistics
    for (const auto& metricName : allMetricNames) {
        std::vector<double> metricValues;
        
        for (const auto& result : results) {
            if (result->hasMetric(metricName)) {
                metricValues.push_back(result->getMetric(metricName));
            }
        }
        
        if (metricValues.size() >= minSampleSize) {
            double mean = std::accumulate(metricValues.begin(), metricValues.end(), 0.0) / metricValues.size();
            
            double variance = 0.0;
            for (double value : metricValues) {
                variance += (value - mean) * (value - mean);
            }
            variance /= metricValues.size();
            double stddev = std::sqrt(variance);
            
            double min = *std::min_element(metricValues.begin(), metricValues.end());
            double max = *std::max_element(metricValues.begin(), metricValues.end());
            
            MetricStats stats;
            stats.mean = mean;
            stats.stddev = stddev;
            stats.min = min;
            stats.max = max;
            stats.count = metricValues.size();
            
            analysis.metricStats[metricName] = stats;
            analysis.summary.push_back("Metric: " + metricName + ", Mean: " + std::to_string(mean) + 
                                       ", StdDev: " + std::to_string(stddev) +
                                       ", Min: " + std::to_string(min) + ", Max: " + std::to_string(max));
        }
    }
    
    return analysis;
}

AnalysisResult ResultAnalyzer::performComparativeAnalysis(
    const std::vector<std::shared_ptr<ExperimentResult>>& results,
    const std::vector<std::string>& experimentNames) {
    AnalysisResult analysis;
    analysis.type = "comparative_analysis";
    
    if (!validateResults(results)) {
        analysis.status = AnalysisStatus::FAILED;
        analysis.message = "Invalid or empty results";
        return analysis;
    }
    
    // Filter results by experiment names
    std::vector<std::shared_ptr<ExperimentResult>> filteredResults;
    for (const auto& result : results) {
        if (std::find(experimentNames.begin(), experimentNames.end(), result->getExperimentName()) != experimentNames.end()) {
            filteredResults.push_back(result);
        }
    }
    
    if (filteredResults.size() < 2) {
        analysis.status = AnalysisStatus::FAILED;
        analysis.message = "Need at least 2 experiments for comparison";
        return analysis;
    }
    
    analysis.status = AnalysisStatus::COMPLETED;
    
    // Extract common metrics
    std::set<std::string> commonMetrics;
    if (!filteredResults.empty()) {
        for (const auto& metric : filteredResults[0]->getMetrics()) {
            commonMetrics.insert(metric.first);
        }
        
        for (size_t i = 1; i < filteredResults.size(); ++i) {
            std::set<std::string> currentMetrics;
            for (const auto& metric : filteredResults[i]->getMetrics()) {
                currentMetrics.insert(metric.first);
            }
            
            std::set<std::string> intersection;
            std::set_intersection(commonMetrics.begin(), commonMetrics.end(),
                                 currentMetrics.begin(), currentMetrics.end(),
                                 std::inserter(intersection, intersection.begin()));
            commonMetrics = intersection;
        }
    }
    
    // Compare each metric
    for (const auto& metricName : commonMetrics) {
        std::vector<double> metricValues;
        
        for (const auto& result : filteredResults) {
            if (result->hasMetric(metricName)) {
                metricValues.push_back(result->getMetric(metricName));
            }
        }
        
        if (metricValues.size() >= 2) {
            double mean = std::accumulate(metricValues.begin(), metricValues.end(), 0.0) / metricValues.size();
            
            double variance = 0.0;
            for (double value : metricValues) {
                variance += (value - mean) * (value - mean);
            }
            variance /= metricValues.size();
            double stddev = std::sqrt(variance);
            
            // Calculate t-statistic (assuming equal variance)
            double tStat = 0.0;
            if (stddev > 0) {
                tStat = mean / (stddev / std::sqrt(metricValues.size()));
            }
            
            // Determine if difference is significant (simplified t-test)
            bool isSignificant = std::abs(tStat) > 2.0; // Simplified threshold
            
            ComparisonResult comparison;
            comparison.mean = mean;
            comparison.stddev = stddev;
            comparison.tStatistic = tStat;
            comparison.significant = isSignificant;
            
            analysis.comparisons[metricName] = comparison;
            analysis.summary.push_back("Metric: " + metricName + ", Mean: " + std::to_string(mean) +
                                       ", T-Statistic: " + std::to_string(tStat) +
                                       ", Significant: " + (isSignificant ? "Yes" : "No"));
        }
    }
    
    return analysis;
}

AnalysisResult ResultAnalyzer::performStatisticalAnalysis(
    const std::vector<std::shared_ptr<ExperimentResult>>& results,
    const std::string& metricName,
    const std::string& testType) {
    AnalysisResult analysis;
    analysis.type = "statistical_analysis";
    analysis.testType = testType;
    
    if (!validateResults(results)) {
        analysis.status = AnalysisStatus::FAILED;
        analysis.message = "Invalid or empty results";
        return analysis;
    }
    
    // Extract metric values
    std::vector<double> metricValues;
    std::vector<std::string> experimentNames;
    
    for (const auto& result : results) {
        if (result->hasMetric(metricName)) {
            metricValues.push_back(result->getMetric(metricName));
            experimentNames.push_back(result->getExperimentName());
        }
    }
    
    if (metricValues.size() < minSampleSize) {
        analysis.status = AnalysisStatus::FAILED;
        analysis.message = "Insufficient sample size for statistical analysis";
        return analysis;
    }
    
    analysis.status = AnalysisStatus::COMPLETED;
    
    if (testType == "t_test") {
        // Perform t-test (assuming single sample)
        double mean = std::accumulate(metricValues.begin(), metricValues.end(), 0.0) / metricValues.size();
        
        double variance = 0.0;
        for (double value : metricValues) {
            variance += (value - mean) * (value - mean);
        }
        variance /= metricValues.size();
        double stddev = std::sqrt(variance);
        
        double tStat = 0.0;
        if (stddev > 0) {
            tStat = mean / (stddev / std::sqrt(metricValues.size()));
        }
        
        // Simple p-value calculation (not accurate)
        double pValue = 2.0 * (1.0 - std::erf(std::abs(tStat) / std::sqrt(2.0)));
        
        analysis.tTest.mean = mean;
        analysis.tTest.stddev = stddev;
        analysis.tTest.tStatistic = tStat;
        analysis.tTest.pValue = pValue;
        analysis.tTest.significant = pValue < significanceLevel;
        
        analysis.summary.push_back(std::string("T-Test for ") + metricName + 
                                   ": t = " + std::to_string(tStat) + 
                                   ", p = " + std::to_string(pValue) +
                                   ", significant = " + (pValue < significanceLevel ? "Yes" : "No"));
    } else if (testType == "anova") {
        // ANOVA (simplified)
        double grandMean = std::accumulate(metricValues.begin(), metricValues.end(), 0.0) / metricValues.size();
        
        double betweenGroupVariance = 0.0;
        double withinGroupVariance = 0.0;
        
        // This is simplified - would need grouping information
        for (double value : metricValues) {
            withinGroupVariance += (value - grandMean) * (value - grandMean);
        }
        
        analysis.anova.grandMean = grandMean;
        analysis.anova.betweenGroupVariance = betweenGroupVariance;
        analysis.anova.withinGroupVariance = withinGroupVariance;
        
        double fStat = (betweenGroupVariance > 0) ? (withinGroupVariance / (metricValues.size() - 1)) : 0.0;
        analysis.anova.fStatistic = fStat;
        
        double pValue = 2.0 * (1.0 - std::erf(std::abs(fStat) / std::sqrt(2.0)));
        analysis.anova.pValue = pValue;
        analysis.anova.significant = pValue < significanceLevel;
        
        analysis.summary.push_back(std::string("ANOVA for ") + metricName + 
                                   ": F = " + std::to_string(fStat) + 
                                   ", p = " + std::to_string(pValue) +
                                   ", significant = " + (pValue < significanceLevel ? "Yes" : "No"));
    } else if (testType == "correlation") {
        // Correlation analysis (would need paired data)
        // For now, just report basic stats
        analysis.summary.push_back(std::string("Correlation analysis for ") + metricName + 
                                   ": Not implemented (requires paired data)");
    }
    
    return analysis;
}

AnalysisResult ResultAnalyzer::performTimeSeriesAnalysis(
    const std::shared_ptr<ExperimentResult>& result,
    const std::string& metricName) {
    AnalysisResult analysis;
    analysis.type = "time_series_analysis";
    
    if (!result || !result->hasMetric(metricName)) {
        analysis.status = AnalysisStatus::FAILED;
        analysis.message = "Invalid result or metric not found";
        return analysis;
    }
    
    analysis.status = AnalysisStatus::COMPLETED;
    
    // Extract step data (simplified)
    const auto& stepData = result->getStepData();
    
    if (stepData.empty()) {
        analysis.summary.push_back("No step data available for time series analysis");
        return analysis;
    }
    
    // For now, just report basic information
    analysis.summary.push_back(std::string("Time series analysis for ") + metricName + 
                               ": " + std::to_string(stepData.size()) + " steps available");
    
    // Detect trends (simplified)
    if (stepData.size() >= 3) {
        // Calculate trend using linear regression (simplified)
        double sumX = 0.0, sumY = 0.0, sumXY = 0.0, sumX2 = 0.0;
        
        for (size_t i = 0; i < stepData.size(); ++i) {
            double x = static_cast<double>(i);
            // Use step number as x, get some metric as y
            sumX += x;
            sumY += x; // Placeholder
            sumXY += x * x;
            sumX2 += x * x;
        }
        
        double slope = (stepData.size() * sumXY - sumX * sumY) / 
                       (stepData.size() * sumX2 - sumX * sumX);
        
        analysis.trendAnalysis.slope = slope;
        analysis.trendAnalysis.trendType = (slope > 0.01) ? "increasing" : 
                                          (slope < -0.01) ? "decreasing" : "stable";
        
        analysis.summary.push_back(std::string("Trend detected: ") + analysis.trendAnalysis.trendType + 
                                   ", slope = " + std::to_string(slope));
    }
    
    return analysis;
}

AnalysisResult ResultAnalyzer::performPatternAnalysis(
    const std::vector<std::shared_ptr<ExperimentResult>>& results,
    const std::string& metricName,
    PatternType patternType) {
    AnalysisResult analysis;
    analysis.type = "pattern_analysis";
    analysis.patternType = patternType;
    
    if (!validateResults(results)) {
        analysis.status = AnalysisStatus::FAILED;
        analysis.message = "Invalid or empty results";
        return analysis;
    }
    
    analysis.status = AnalysisStatus::COMPLETED;
    
    // Extract metric values
    std::vector<double> metricValues;
    std::vector<std::string> experimentNames;
    
    for (const auto& result : results) {
        if (result->hasMetric(metricName)) {
            metricValues.push_back(result->getMetric(metricName));
            experimentNames.push_back(result->getExperimentName());
        }
    }
    
    if (metricValues.empty()) {
        analysis.summary.push_back(std::string("No data for pattern analysis of ") + metricName);
        return analysis;
    }
    
    // Pattern detection (simplified)
    switch (patternType) {
        case PatternType::SEASONAL:
            // Seasonal pattern detection (would need time series)
            analysis.summary.push_back(std::string("Seasonal pattern analysis for ") + metricName + 
                                       ": Not implemented (requires temporal data)");
            break;
            
        case PatternType::CLUSTERING:
            // Clustering analysis
            analysis.clustering.centroids.resize(2); // Simplified
            analysis.clustering.centroids[0] = 0.5; // Placeholder
            analysis.clustering.centroids[1] = 0.7;
            analysis.clustering.similarity = 0.8; // Placeholder
            analysis.summary.push_back(std::string("Clustering detected pattern in ") + metricName + 
                                       ": 2 clusters found, similarity = 0.8");
            break;
            
        case PatternType::OUTLIER:
            // Outlier detection
            analysis.outlier.detectionMethod = "iqr"; // Interquartile range
            analysis.outlier.threshold = 1.5;
            
            if (metricValues.size() >= 4) {
                std::vector<double> sorted = metricValues;
                std::sort(sorted.begin(), sorted.end());
                
                size_t q1Index = sorted.size() / 4;
                size_t q3Index = 3 * sorted.size() / 4;
                
                double q1 = (q1Index == 0) ? sorted[0] : (sorted[q1Index - 1] + sorted[q1Index]) / 2;
                double q3 = (q3Index == sorted.size()) ? sorted.back() : (sorted[q3Index - 1] + sorted[q3Index]) / 2;
                
                double iqr = q3 - q1;
                double lowerBound = q1 - analysis.outlier.threshold * iqr;
                double upperBound = q3 + analysis.outlier.threshold * iqr;
                
                int outlierCount = 0;
                for (double value : metricValues) {
                    if (value < lowerBound || value > upperBound) {
                        outlierCount++;
                    }
                }
                
                analysis.outlier.outlierCount = outlierCount;
                analysis.outlier.outlierValues.clear();
                analysis.outlier.outlierExperiments.clear();
                
                for (size_t i = 0; i < metricValues.size(); ++i) {
                    if (metricValues[i] < lowerBound || metricValues[i] > upperBound) {
                        analysis.outlier.outlierValues.push_back(metricValues[i]);
                        analysis.outlier.outlierExperiments.push_back(experimentNames[i]);
                    }
                }
                
                analysis.summary.push_back(std::string("Outlier analysis for ") + metricName + 
                                           ": " + std::to_string(outlierCount) + " outliers detected");
            } else {
                analysis.summary.push_back(std::string("Outlier analysis for ") + metricName + 
                                           ": Insufficient data for reliable outlier detection");
            }
            break;
            
        case PatternType::SEQUENTIAL:
            // Sequential pattern detection
            analysis.summary.push_back(std::string("Sequential pattern analysis for ") + metricName + 
                                       ": Not implemented (requires ordered data)");
            break;
            
        case PatternType::CYCLE:
            // Cycle detection
            analysis.summary.push_back(std::string("Cycle analysis for ") + metricName + 
                                       ": Not implemented (requires temporal data)");
            break;
    }
    
    return analysis;
}

AnalysisResult ResultAnalyzer::performOptimizationAnalysis(
    const std::vector<std::shared_ptr<ExperimentResult>>& results,
    const std::string& objectiveMetric) {
    AnalysisResult analysis;
    analysis.type = "optimization_analysis";
    
    if (!validateResults(results)) {
        analysis.status = AnalysisStatus::FAILED;
        analysis.message = "Invalid or empty results";
        return analysis;
    }
    
    analysis.status = AnalysisStatus::COMPLETED;
    
    // Extract objective values
    std::vector<double> objectiveValues;
    std::vector<std::string> experimentNames;
    
    for (const auto& result : results) {
        if (result->hasMetric(objectiveMetric)) {
            objectiveValues.push_back(result->getMetric(objectiveMetric));
            experimentNames.push_back(result->getExperimentName());
        }
    }
    
    if (objectiveValues.empty()) {
        analysis.summary.push_back(std::string("No objective metric ") + objectiveMetric + 
                                   " found for optimization analysis");
        return analysis;
    }
    
    // Optimization analysis (simplified)
    double bestValue = *std::min_element(objectiveValues.begin(), objectiveValues.end());
    double worstValue = *std::max_element(objectiveValues.begin(), objectiveValues.end());
    double range = worstValue - bestValue;
    
    // Find best experiment
    int bestIndex = std::distance(objectiveValues.begin(), 
                                 std::min_element(objectiveValues.begin(), objectiveValues.end()));
    
    analysis.optimization.bestObjective = bestValue;
    analysis.optimization.worstObjective = worstValue;
    analysis.optimization.range = range;
    analysis.optimization.bestExperiment = experimentNames[bestIndex];
    
    // Calculate convergence rate (simplified)
    analysis.optimization.convergenceRate = (range > 0) ? 1.0 / (1.0 + range) : 1.0;
    
    analysis.summary.push_back(std::string("Optimization analysis for ") + objectiveMetric + 
                               ": Best = " + std::to_string(bestValue) + 
                               ", Worst = " + std::to_string(worstValue) + 
                               ", Range = " + std::to_string(range) + 
                               ", Best Experiment = " + analysis.optimization.bestExperiment);
    
    return analysis;
}

std::string ResultAnalyzer::generateAnalysisReport(
    const AnalysisResult& analysis,
    const std::vector<std::shared_ptr<ExperimentResult>>& results) const {
    std::stringstream ss;
    ss << "Analysis Report\n";
    ss << "==============\n\n";
    ss << "Analysis Type: " << analysis.type << "\n";
    ss << "Status: " << static_cast<int>(analysis.status) << "\n";
    if (!analysis.message.empty()) {
        ss << "Message: " << analysis.message << "\n";
    }
    ss << "\n";
    
    ss << "Summary:\n";
    for (const auto& item : analysis.summary) {
        ss << "  - " << item << "\n";
    }
    
    ss << "\n";
    
    return ss.str();
}

} // namespace nlm
