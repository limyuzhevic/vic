// Configuration Comparison Tool
// Allows running multiple experimental configurations and comparing results

#pragma once

#include "experiments/Phase6IntegratedExperiment.hpp"
#include <iostream>
#include <vector>
#include <algorithm>
#include <chrono>

namespace nlm {

class ConfigComparator {
public:
    struct ComparisonResult {
        std::string configName;
        Phase6IntegrationResult experimentResult;
        float performanceScore;
        float stabilityScore;
        std::vector<float> metricHistory;
    };
    
    ConfigComparator() = default;
    ~ConfigComparator() = default;
    
    void addConfig(const std::string& name, const Phase6Config& config);
    std::vector<ComparisonResult> runComparison();
    
private:
    struct NamedConfig {
        std::string name;
        Phase6Config config;
    };
    
    std::vector<NamedConfig> configs_;
    
    float calculatePerformanceScore(const Phase6IntegrationResult& result);
    float calculateStabilityScore(const Phase6IntegrationResult& result);
    std::vector<float> collectMetricHistory(const Phase6IntegrationResult& result);
    void printResultSummary(const ComparisonResult& result);
    void printComparisonReport(const std::vector<ComparisonResult>& results);
    void printStatisticalAnalysis(const std::vector<ComparisonResult>& results);
};

} // namespace nlm