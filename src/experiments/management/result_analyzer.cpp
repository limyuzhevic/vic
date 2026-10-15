#include "result_analyzer.hpp"
#include "enhanced_experiments.hpp"
#include "../core/Logger/Logger.hpp"
#include <nlohmann/json.hpp>
#include <algorithm>
#include <cmath>
#include <numeric>

using json = nlohmann::json;

namespace nlm {

// ResultAnalyzer implementation
ResultAnalyzer::ExperimentStatistics ResultAnalyzer::calculateStatistics(const std::vector<double>& data) const {
    ExperimentStatistics stats;
    stats.rawData = data;
    stats.count = data.size();
    
    if (stats.count == 0) {
        return stats;
    }
    
    // Calculate basic statistics
    stats.mean = calculateMean(data);
    stats.median = calculatePercentile(data, 50.0);
    stats.stddev = calculateStandardDeviation(data, stats.mean);
    stats.min = *std::min_element(data.begin(), data.end());
    stats.max = *std::max_element(data.begin(), data.end());
    stats.percentile25 = calculatePercentile(data, 25.0);
    stats.percentile75 = calculatePercentile(data, 75.0);
    
    return stats;
}

ResultAnalyzer::ComparisonResult ResultAnalyzer::compareExperiments(const std::vector<double>& results1,
                                                                    const std::vector<double>& results2) const {
    ComparisonResult comparison;
    comparison.experiment1Name = "Experiment 1";
    comparison.experiment2Name = "Experiment 2";
    
    if (results1.empty() || results2.empty()) {
        comparison.performanceDifference = 0.0;
        comparison.confidenceLevel = 0.0;
        comparison.isSignificant = false;
        comparison.interpretation = "Insufficient data for comparison";
        return comparison;
    }
    
    // Calculate statistics
    ExperimentStatistics stats1 = calculateStatistics(results1);
    ExperimentStatistics stats2 = calculateStatistics(results2);
    
    // Calculate performance difference
    comparison.performanceDifference = stats2.mean - stats1.mean;
    
    // Perform t-test for significance
    StatisticalTestResult tTest = performStatisticalTest(results1, results2, StatisticalTest::TTest);
    comparison.isSignificant = tTest.isSignificant;
    comparison.confidenceLevel = 1.0 - tTest.pValue;
    
    // Generate interpretation
    if (comparison.isSignificant) {
        if (comparison.performanceDifference > 0) {
            comparison.interpretation = "Experiment 2 performed significantly better than Experiment 1";
        } else {
            comparison.interpretation = "Experiment 1 performed significantly better than Experiment 2";
        }
    } else {
        comparison.interpretation = "No significant difference between experiments";
    }
    
    return comparison;
}

ResultAnalyzer::VisualizationData ResultAnalyzer::generateTimeSeriesData(const std::shared_ptr<BaseExperiment>& experiment) const {
    VisualizationData data;
    data.chartType = "time_series";
    data.title = "Experiment Progress: " + experiment->getName();
    data.xAxisLabel = "Step";
    data.yAxisLabel = "Progress / Metric";
    
    const auto& stepData = experiment->getStepData();
    
    for (size_t i = 0; i < stepData.size(); ++i) {
        data.xValues.push_back(static_cast<double>(stepData[i].step));
        data.yValues.push_back(static_cast<double>(stepData[i].avgFiringRate));
    }
    
    return data;
}

ResultAnalyzer::VisualizationData ResultAnalyzer::generateMetricComparisonData(
    const std::vector<std::shared_ptr<BaseExperiment>>& experiments,
    const std::string& metricName) const {
    VisualizationData data;
    data.chartType = "bar_chart";
    data.title = "Metric Comparison: " + metricName;
    data.xAxisLabel = "Experiment";
    data.yAxisLabel = metricName;
    
    for (const auto& experiment : experiments) {
        data.xValues.push_back(experiment->getName());
        
        double metricValue = 0.0;
        if (experiment->hasMetric(metricName)) {
            metricValue = experiment->getMetric(metricName);
        }
        data.yValues.push_back(metricValue);
    }
    
    return data;
}

std::string ResultAnalyzer::generateCSV(const std::vector<std::shared_ptr<BaseExperiment>>& experiments) const {
    std::stringstream csv;
    
    // Header
    csv << "Experiment Name,Type,Performance Score,Mean Metric,Std Dev,Min,Max" << std::endl;
    
    for (const auto& experiment : experiments) {
        auto metrics = experiment->getMetrics();
        double sum = 0.0;
        for (const auto& metric : metrics) {
            sum += metric.second;
        }
        double mean = metrics.empty() ? 0.0 : sum / metrics.size();
        
        ExperimentStatistics stats;
        if (!metrics.empty()) {
            std::vector<double> metricValues;
            for (const auto& metric : metrics) {
                metricValues.push_back(metric.second);
            }
            stats = calculateStatistics(metricValues);
        }
        
        csv << experiment->getName() << ",";
        csv << experiment->getExperimentType() << ",";
        csv << experiment->calculatePerformanceScore() << ",";
        csv << mean << ",";
        csv << stats.stddev << ",";
        csv << stats.min << ",";
        csv << stats.max << std::endl;
    }
    
    return csv.str();
}

std::string ResultAnalyzer::generateJSON(const std::vector<std::shared_ptr<BaseExperiment>>& experiments) const {
    json j;
    j["experiments"] = json::array();
    
    for (const auto& experiment : experiments) {
        json expJson;
        expJson["name"] = experiment->getName();
        expJson["type"] = experiment->getExperimentType();
        expJson["performance_score"] = experiment->calculatePerformanceScore();
        
        auto metrics = experiment->getMetrics();
        j["metrics"] = json::object();
        for (const auto& metric : metrics) {
            j["metrics"][metric.first] = metric.second;
        }
        
        expJson["metrics"] = j["metrics"];
        j["experiments"].push_back(expJson);
    }
    
    return j.dump(2);
}

ResultAnalyzer::StatisticalTestResult ResultAnalyzer::performStatisticalTest(
    const std::vector<double>& data1,
    const std::vector<double>& data2,
    StatisticalTest test) const {
    StatisticalTestResult result;
    result.test = test;
    
    if (data1.size() < 2 || data2.size() < 2) {
        result.pValue = 1.0;
        result.isSignificant = false;
        result.description = "Insufficient data for statistical test";
        return result;
    }
    
    switch (test) {
        case StatisticalTest::TTest:
            // Simplified t-test (would use proper statistical library in production)
            double mean1 = calculateMean(data1);
            double mean2 = calculateMean(data2);
            double stddev1 = calculateStandardDeviation(data1, mean1);
            double stddev2 = calculateStandardDeviation(data2, mean2);
            double n1 = data1.size();
            double n2 = data2.size();
            
            double pooledStddev = std::sqrt(((n1-1)*stddev1*stddev1 + (n2-1)*stddev2*stddev2) / (n1 + n2 - 2));
            double tStatistic = (mean2 - mean1) / (pooledStddev * std::sqrt(1.0/n1 + 1.0/n2));
            
            // Approximate p-value for t-test
            result.pValue = 2.0 * (1.0 - std::erf(std::abs(tStatistic) / std::sqrt(2.0)));
            break;
            
        case StatisticalTest::MannWhitneyU:
            // Simplified Mann-Whitney U test (would use proper statistical library in production)
            result.pValue = 0.05; // Placeholder
            break;
            
        case StatisticalTest::KolmogorovSmirnov:
            // Simplified Kolmogorov-Smirnov test (would use proper statistical library in production)
            result.pValue = 0.05; // Placeholder
            break;
    }
    
    result.isSignificant = result.pValue < 0.05;
    
    switch (test) {
        case StatisticalTest::TTest:
            result.description = "Two-sample t-test comparing experiment results";
            break;
        case StatisticalTest::MannWhitneyU:
            result.description = "Mann-Whitney U test comparing experiment results (non-parametric)";
            break;
        case StatisticalTest::KolmogorovSmirnov:
            result.description = "Kolmogorov-Smirnov test comparing experiment distributions";
            break;
    }
    
    return result;
}

bool ResultAnalyzer::exportResults(const std::string& filepath, const std::string& format) const {
    // This is a placeholder - actual implementation would need to store experiment data
    std::cerr << "Warning: exportResults not implemented - needs experiment data storage" << std::endl;
    return false;
}

std::string ResultAnalyzer::doubleToString(double value, int precision) const {
    std::stringstream ss;
    ss << std::fixed << std::setprecision(precision) << value;
    return ss.str();
}

} // namespace nlm