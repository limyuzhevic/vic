#pragma once

#include <vector>
#include <string>
#include <map>
#include <algorithm>
#include <cmath>
#include <numeric>
#include <random>
#include <memory>
#include <limits>
#include <unordered_map>
#include <unordered_set>

namespace nlm {

/**
 * Experiment comparison and statistical analysis tools
 * for comparing results across different experiments and performing
 * statistical significance testing.
 */
class ExperimentComparator {
public:
    // Statistical test results
    struct StatisticalTestResult {
        std::string testName;
        std::string metric1;
        std::string metric2;
        double statistic;
        double pValue;
        double effectSize;
        std::string interpretation;
        
        StatisticalTestResult() : statistic(0.0), pValue(1.0), effectSize(0.0) {}
        
        bool isSignificant(double alpha = 0.05) const;
        std::string getEffectSizeLabel() const;
    };
    
    // Experiment comparison metrics
    struct ExperimentComparison {
        std::string experiment1Name;
        std::string experiment2Name;
        std::string metric;
        double value1;
        double value2;
        double difference;
        double percentageChange;
        double confidenceInterval;
        double pValue;
        bool isBetter;
        
        ExperimentComparison() : value1(0.0), value2(0.0), difference(0.0), 
                               percentageChange(0.0), confidenceInterval(0.0), 
                               pValue(1.0), isBetter(false) {}
    };
    
    // Comparison configuration
    struct ComparisonConfig {
        bool enableStatisticalTests;
        double significanceLevel;
        size_t bootstrapIterations;
        std::vector<std::string> metricsToCompare;
        std::string outputFormat;
        bool calculateEffectSizes;
        
        ComparisonConfig() : enableStatisticalTests(true), significanceLevel(0.05),
                           bootstrapIterations(1000), outputFormat("text"), 
                           calculateEffectSizes(true) {}
    };
    
    ExperimentComparator();
    ~ExperimentComparator() = default;
    
    // Experiment comparison
    std::vector<ExperimentComparison> compareExperiments(
        const std::vector<std::string>& experimentNames,
        const std::map<std::string, std::shared_ptr<ExperimentResults>>& resultsMap
    );
    
    // Statistical testing
    StatisticalTestResult performTTest(const std::vector<double>& sample1, 
                                      const std::vector<double>& sample2,
                                      const std::string& testName = "T-Test");
    
    StatisticalTestResult performMannWhitneyUTest(const std::vector<double>& sample1,
                                                 const std::vector<double>& sample2,
                                                 const std::string& testName = "Mann-Whitney U");
    
    StatisticalTestResult performWilcoxonSignedRankTest(const std::vector<double>& sample1,
                                                       const std::vector<double>& sample2,
                                                       const std::string& testName = "Wilcoxon Signed-Rank");
    
    // Correlation and regression analysis
    StatisticalTestResult calculatePearsonCorrelation(const std::vector<double>& x, const std::vector<double>& y);
    StatisticalTestResult calculateSpearmanCorrelation(const std::vector<double>& x, const std::vector<double>& y);
    
    StatisticalTestResult calculateLinearRegression(const std::vector<double>& x, const std::vector<double>& y);
    StatisticalTestResult calculateLogisticRegression(const std::vector<double>& x, const std::vector<double>& y);
    
    // Effect size calculations
    double calculateCohenD(const std::vector<double>& sample1, const std::vector<double>& sample2);
    double calculateGlassDelta(const std::vector<double>& sample1, const std::vector<double>& sample2);
    double calculateHedgesG(const std::vector<double>& sample1, const std::vector<double>& sample2);
    
    // Ranking and ordering
    std::vector<std::string> rankExperiments(const std::map<std::string, double>& scores,
                                             bool ascending = false);
    
    // Bootstrap analysis
    StatisticalTestResult bootstrapAnalysis(const std::vector<double>& data,
                                            double confidenceLevel = 0.95,
                                            size_t iterations = 1000);
    
    // Meta-analysis
    StatisticalTestResult metaAnalysis(const std::vector<StatisticalTestResult>& results);
    
    // Result export
    std::string exportResults(const std::vector<ExperimentComparison>& comparisons,
                             const ComparisonConfig& config) const;
    
    // Configuration management
    void setConfig(const ComparisonConfig& config);
    const ComparisonConfig& getConfig() const;
    
    // Validation
    bool validateData(const std::map<std::string, std::shared_ptr<ExperimentResults>>& resultsMap) const;
    
private:
    ComparisonConfig config;
    
    // Statistical helper functions
    double calculateMean(const std::vector<double>& values) const;
    double calculateStandardDeviation(const std::vector<double>& values, double mean) const;
    double calculateVariance(const std::vector<double>& values, double mean) const;
    
    std::mt19937_64 rng;
    std::uniform_real_distribution<double> dist;
};

} // namespace nlm