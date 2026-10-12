// ExperimentComparison - Compare results across different experiments
#pragma once

#include "ExperimentResult.hpp"
#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include <numeric>
#include <cmath>
#include <iomanip>
#include <sstream>
#include <unordered_map>

namespace nlm {

struct ComparisonMetric {
    std::string name;
    std::string description;
    std::vector<double> values;  // Values from each experiment
    double mean;
    double stdDev;
    double min;
    double max;
    double median;
    double iqr;  // Interquartile range
    
    ComparisonMetric(const std::string& n, const std::string& desc = "")
        : name(n), description(desc), mean(0.0), stdDev(0.0), min(0.0), max(0.0), median(0.0), iqr(0.0) {}
    
    void calculateStatistics() {
        if (values.empty()) return;
        
        // Basic statistics
        mean = std::accumulate(values.begin(), values.end(), 0.0) / values.size();
        
        double sqSum = 0.0;
        min = values[0];
        max = values[0];
        for (double val : values) {
            sqSum += (val - mean) * (val - mean);
            min = std::min(min, val);
            max = std::max(max, val);
        }
        stdDev = std::sqrt(sqSum / values.size());
        
        // Median and IQR
        std::vector<double> sorted = values;
        std::sort(sorted.begin(), sorted.end());
        median = sorted.size() % 2 == 0 ? 
            (sorted[sorted.size()/2 - 1] + sorted[sorted.size()/2]) / 2.0 : 
            sorted[sorted.size()/2];
        
        size_t q1Index = sorted.size() / 4;
        size_t q3Index = 3 * sorted.size() / 4;
        double q1 = sorted.size() % 2 == 0 && q1Index > 0 ? 
            (sorted[q1Index - 1] + sorted[q1Index]) / 2.0 : sorted[q1Index];
        double q3 = sorted.size() % 2 == 0 && q3Index > 0 ? 
            (sorted[q3Index - 1] + sorted[q3Index]) / 2.0 : sorted[q3Index];
        
        iqr = q3 - q1;
    }
    
    double getZScore(double value) const {
        if (stdDev < 0.0001f) return 0.0;
        return (value - mean) / stdDev;
    }
    
    bool isOutlier(double value) const {
        return std::abs(getZScore(value)) > 2.0;
    }
};

struct ComparisonResult {
    std::string experiment1Name;
    std::string experiment2Name;
    std::vector<ComparisonMetric> metrics;
    double overallDifference;
    std::string recommendation;
    
    ComparisonResult(const std::string& name1, const std::string& name2)
        : experiment1Name(name1), experiment2Name(name2), overallDifference(0.0) {}
    
    std::string generateReport() const {
        std::ostringstream oss;
        oss << "=== EXPERIMENT COMPARISON ===\n";
        oss << experiment1Name << " vs " << experiment2Name << "\n\n";
        
        oss << std::fixed << std::setprecision(4);
        oss << "Overall difference: " << overallDifference << "\n\n";
        
        for (const auto& metric : metrics) {
            oss << metric.name << " (" << metric.description << ")\n";
            oss << "  Mean: " << experiment1Name << " = " << getMetricValue(experiment1Name, metric.name)
                << ", " << experiment2Name << " = " << getMetricValue(experiment2Name, metric.name) << "\n";
            oss << "  Diff: " << metric.mean << " (" << metric.stdDev << " std)\n";
            oss << "  Median: " << metric.median << ", IQR: " << metric.iqr << "\n";
            
            // Identify outliers
            size_t outliers1 = countOutliers(experiment1Name, metric.name);
            size_t outliers2 = countOutliers(experiment2Name, metric.name);
            if (outliers1 > 0 || outliers2 > 0) {
                oss << "  Outliers: " << experiment1Name << " = " << outliers1 << ", "
                    << experiment2Name << " = " << outliers2 << "\n";
            }
            oss << "\n";
        }
        
        oss << "Recommendation: " << recommendation << "\n";
        return oss.str();
    }
    
private:
    double getMetricValue(const std::string& experimentName, const std::string& metricName) const {
        for (const auto& metric : metrics) {
            if (metric.name == metricName) {
                // This would need to be mapped from experiment results to comparison metrics
                return metric.mean; // Placeholder
            }
        }
        return 0.0;
    }
    
    size_t countOutliers(const std::string& experimentName, const std::string& metricName) const {
        // Simplified - would need proper mapping
        return 0;
    }
};

class ExperimentComparison {
public:
    // Single comparison
    static ComparisonResult compare(const ExperimentResult& result1, const ExperimentResult& result2);
    
    // Multiple experiment comparison
    static std::vector<ComparisonResult> compareAll(const std::vector<ExperimentResult>& results);
    
    // Statistical analysis
    static double calculateEffectSize(const ExperimentResult& result1, const ExperimentResult& result2);
    static double calculateCohenD(const std::vector<double>& values1, const std::vector<double>& values2);
    static double calculatePValue(const std::vector<double>& values1, const std::vector<double>& values2);
    
    // Ranking
    static std::vector<std::pair<std::string, double>> rankExperiments(const std::vector<ExperimentResult>& results,
                                                                       const std::vector<std::string>& metricNames);
    
    // Visualization data
    static std::string generateCSVReport(const std::vector<ExperimentResult>& results);
    static std::string generateJSONReport(const std::vector<ExperimentResult>& results);
    
    // Performance analysis
    static double calculateEfficiency(const ExperimentResult& result);
    static double calculateConsistency(const ExperimentResult& result);
    static double calculateConvergence(const ExperimentResult& result);
    
    // Hypothesis testing (simplified)
    static struct HypothesisTest {
        std::string metric;
        double pValue;
        bool rejectNullHypothesis;
        std::string conclusion;
    } performTTest(const std::vector<double>& values1, const std::vector<double>& values2,
                   const std::string& metricName);
    
    // Best practice identification
    static std::vector<std::string> identifyBestPractices(const std::vector<ExperimentResult>& results);
    static std::vector<std::string> identifyWarningSigns(const std::vector<ExperimentResult>& results);
};

} // namespace nlm
