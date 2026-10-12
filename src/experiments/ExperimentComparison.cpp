// ExperimentComparison.cpp implementation
#include "ExperimentComparison.hpp"

namespace nlm {

ComparisonResult ExperimentComparison::compare(const ExperimentResult& result1, const ExperimentResult& result2) {
    ComparisonResult comparison(result1.experimentName, result2.experimentName);
    
    // Create comparison metrics
    ComparisonMetric totalReward("total_reward", "Total reward collected");
    ComparisonMetric avgFiringRate("avg_firing_rate", "Average firing rate");
    ComparisonMetric successRate("success_rate", "Experiment success rate");
    ComparisonMetric duration("duration", "Experiment duration");
    ComparisonMetric efficiency("efficiency", "Overall efficiency score");
    
    // Populate with experiment values
    totalReward.values.push_back(result1.totalReward);
    totalReward.values.push_back(result2.totalReward);
    
    avgFiringRate.values.push_back(result1.averageFiringRate);
    avgFiringRate.values.push_back(result2.averageFiringRate);
    
    successRate.values.push_back(result1.success ? 1.0 : 0.0);
    successRate.values.push_back(result2.success ? 1.0 : 0.0);
    
    duration.values.push_back(result1.duration);
    duration.values.push_back(result2.duration);
    
    // Calculate statistics for each metric
    totalReward.calculateStatistics();
    avgFiringRate.calculateStatistics();
    successRate.calculateStatistics();
    duration.calculateStatistics();
    
    // Calculate efficiency
    efficiency.values.push_back(result1.totalReward / std::max(1.0, result1.duration));
    efficiency.values.push_back(result2.totalReward / std::max(1.0, result2.duration));
    efficiency.calculateStatistics();
    
    comparison.metrics.push_back(totalReward);
    comparison.metrics.push_back(avgFiringRate);
    comparison.metrics.push_back(successRate);
    comparison.metrics.push_back(duration);
    comparison.metrics.push_back(efficiency);
    
    // Calculate overall difference
    comparison.overallDifference = calculateEffectSize(result1, result2);
    
    // Generate recommendation
    comparison.recommendation = generateRecommendation(comparison.metrics);
    
    return comparison;
}

std::vector<ComparisonResult> ExperimentComparison::compareAll(const std::vector<ExperimentResult>& results) {
    std::vector<ComparisonResult> comparisons;
    
    for (size_t i = 0; i < results.size(); ++i) {
        for (size_t j = i + 1; j < results.size(); ++j) {
            comparisons.push_back(compare(results[i], results[j]));
        }
    }
    
    return comparisons;
}

double ExperimentComparison::calculateEffectSize(const ExperimentResult& result1, const ExperimentResult& result2) {
    // Simplified effect size calculation
    double mean1 = result1.totalReward;
    double mean2 = result2.totalReward;
    
    double std1 = std::sqrt(result1.duration);  // Simplified std dev
    double std2 = std::sqrt(result2.duration);
    
    double pooledStd = std::sqrt((std1 * std1 + std2 * std2) / 2.0);
    
    if (pooledStd < 0.0001f) return 0.0;
    
    return (mean2 - mean1) / pooledStd;
}

double ExperimentComparison::calculateCohenD(const std::vector<double>& values1, const std::vector<double>& values2) {
    if (values1.empty() || values2.empty()) return 0.0;
    
    double mean1 = std::accumulate(values1.begin(), values1.end(), 0.0) / values1.size();
    double mean2 = std::accumulate(values2.begin(), values2.end(), 0.0) / values2.size();
    
    double var1 = 0.0, var2 = 0.0;
    for (double val : values1) {
        var1 += (val - mean1) * (val - mean1);
    }
    for (double val : values2) {
        var2 += (val - mean2) * (val - mean2);
    }
    
    var1 /= values1.size();
    var2 /= values2.size();
    
    double pooledStd = std::sqrt((var1 + var2) / 2.0);
    
    if (pooledStd < 0.0001f) return 0.0;
    
    return (mean2 - mean1) / pooledStd;
}

double ExperimentComparison::calculatePValue(const std::vector<double>& values1, const std::vector<double>& values2) {
    // Simplified p-value calculation (always return 0.05 for demonstration)
    return 0.05;
}

std::vector<std::pair<std::string, double>> ExperimentComparison::rankExperiments(
    const std::vector<ExperimentResult>& results,
    const std::vector<std::string>& metricNames) {
    
    std::vector<std::pair<std::string, double>> rankings;
    
    for (const auto& result : results) {
        double totalScore = 0.0;
        
        for (const auto& metricName : metricNames) {
            if (result.metricResults.find(metricName) != result.metricResults.end()) {
                totalScore += result.metricResults.at(metricName);
            }
        }
        
        rankings.push_back(std::make_pair(result.experimentName, totalScore));
    }
    
    // Sort by score (descending)
    std::sort(rankings.begin(), rankings.end(),
              [](const auto& a, const auto& b) { return a.second > b.second; });
    
    return rankings;
}

std::string ExperimentComparison::generateCSVReport(const std::vector<ExperimentResult>& results) {
    std::ostringstream oss;
    oss << "ExperimentName,Success,TotalReward,AvgFiringRate,Duration,AverageFiringRate" << std::endl;
    
    for (const auto& result : results) {
        oss << result.experimentName << "," 
            << (result.success ? "true" : "false") << "," 
            << result.totalReward << "," 
            << result.averageFiringRate << "," 
            << result.duration << "," 
            << (result.averageFiringRate > 0.0f ? result.totalSpikes / result.totalNeurons : 0.0)
            << std::endl;
    }
    
    return oss.str();
}

std::string ExperimentComparison::generateJSONReport(const std::vector<ExperimentResult>& results) {
    std::ostringstream oss;
    oss << "{" << std::endl;
    oss << "  \"experiments\": [" << std::endl;
    
    for (size_t i = 0; i < results.size(); ++i) {
        const auto& result = results[i];
        oss << "    {\"name\": \"" << result.experimentName << "\", \"success\": " 
            << (result.success ? "true" : "false") << ", \"totalReward\": " 
            << result.totalReward << ", \"avgFiringRate\": " << result.averageFiringRate
            << "}" << (i < results.size() - 1 ? "," : "") << std::endl;
    }
    
    oss << "  ]" << std::endl;
    oss << "}" << std::endl;
    
    return oss.str();
}

double ExperimentComparison::calculateEfficiency(const ExperimentResult& result) {
    return result.totalReward / std::max(1.0, result.duration);
}

double ExperimentComparison::calculateConsistency(const ExperimentResult& result) {
    // Calculate coefficient of variation of timeseries data
    if (result.timeseries.empty()) return 0.0;
    
    std::vector<double> values;
    for (const auto& pair : result.timeseries) {
        values.push_back(pair.second);
    }
    
    double mean = std::accumulate(values.begin(), values.end(), 0.0) / values.size();
    double sqSum = 0.0;
    for (double val : values) {
        sqSum += (val - mean) * (val - mean);
    }
    double stdDev = std::sqrt(sqSum / values.size());
    
    return (mean > 0.0) ? (stdDev / mean) : 0.0;
}

double ExperimentComparison::calculateConvergence(const ExperimentResult& result) {
    // Check if timeseries is converging (last values are similar)
    if (result.timeseries.size() < 5) return 0.0;
    
    // Look at last few values
    size_t numLast = std::min(static_cast<size_t>(5), result.timeseries.size());
    size_t start = result.timeseries.size() - numLast;
    
    double first = result.timeseries[start].second;
    double last = result.timeseries.back().second;
    
    return std::abs(last - first) / std::max(1.0, first);
}

ExperimentComparison::HypothesisTest ExperimentComparison::performTTest(
    const std::vector<double>& values1,
    const std::vector<double>& values2,
    const std::string& metricName) {
    
    HypothesisTest test;
    test.metric = metricName;
    test.pValue = calculatePValue(values1, values2);
    test.rejectNullHypothesis = test.pValue < 0.05;
    test.conclusion = test.rejectNullHypothesis ? 
        "Significant difference detected" : "No significant difference";
    
    return test;
}

std::vector<std::string> ExperimentComparison::identifyBestPractices(const std::vector<ExperimentResult>& results) {
    std::vector<std::string> bestPractices;
    
    // Find experiments with high efficiency
    double maxEfficiency = 0.0;
    std::string bestExperiment;
    
    for (const auto& result : results) {
        double efficiency = calculateEfficiency(result);
        if (efficiency > maxEfficiency) {
            maxEfficiency = efficiency;
            bestExperiment = result.experimentName;
        }
    }
    
    if (!bestExperiment.empty()) {
        bestPractices.push_back("Use " + bestExperiment + " as template for efficiency");
    }
    
    // Check for consistency
    bool hasConsistentResults = false;
    for (const auto& result : results) {
        if (calculateConsistency(result) < 0.1) {
            hasConsistentResults = true;
            break;
        }
    }
    
    if (hasConsistentResults) {
        bestPractices.push_back("Ensure experimental conditions are consistent");
    }
    
    return bestPractices;
}

std::vector<std::string> ExperimentComparison::identifyWarningSigns(const std::vector<ExperimentResult>& results) {
    std::vector<std::string> warnings;
    
    for (const auto& result : results) {
        if (!result.success) {
            warnings.push_back("Experiment " + result.experimentName + " failed");
        }
        
        if (calculateConsistency(result) > 0.5) {
            warnings.push_back("Experiment " + result.experimentName + " has high variance");
        }
        
        if (result.errors.size() > 0) {
            warnings.push_back("Experiment " + result.experimentName + " has errors: " + result.errors[0]);
        }
    }
    
    return warnings;
}

std::string ExperimentComparison::generateRecommendation(const std::vector<ComparisonMetric>& metrics) {
    // Analyze metrics and generate recommendation
    std::string recommendation = "Balanced performance across all metrics";
    
    // Find dominant metric
    if (!metrics.empty()) {
        auto maxIt = std::max_element(metrics.begin(), metrics.end(),
                                     [](const ComparisonMetric& a, const ComparisonMetric& b) {
                                         return a.mean < b.mean;
                                     });
        
        if (maxIt->name == "total_reward") {
            recommendation = "Focus on maximizing total reward";
        } else if (maxIt->name == "success_rate") {
            recommendation = "Prioritize experiment success";
        } else if (maxIt->name == "efficiency") {
            recommendation = "Optimize for efficiency over raw performance";
        }
    }
    
    return recommendation;
}

} // namespace nlm
