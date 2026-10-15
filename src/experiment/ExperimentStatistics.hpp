#pragma once

#include <string>
#include <memory>
#include <vector>
#include <map>
#include <algorithm>
#include <numeric>
#include <cmath>
#include <random>
#include <chrono>
#include <functional>
#include <stdexcept>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <unordered_map>
#include <unordered_set>
#include "ExperimentProtocol.hpp"
#include "ExperimentResults.hpp"
#include "ExperimentMetrics.hpp"

namespace nlm {

// Statistical test types
enum class StatisticalTestType {
    MeanTest,
    VarianceTest,
    CorrelationTest,
    RegressionTest,
    TTest,
    AnovaTest,
    ChiSquareTest,
    KolmogorovSmirnovTest,
    MannWhitneyUTest,
    KruskalWallisTest,
    FriedmanTest,
    WilcoxonSignedRankTest,
    CustomTest
};

// Statistical distribution types
enum class StatisticalDistribution {
    Normal,
    Uniform,
    Exponential,
    Poisson,
    Binomial,
    Geometric,
    Gamma,
    Beta,
    ChiSquare,
    StudentT,
    FDistribution,
    Weibull,
    LogNormal,
    Cauchy,
    Laplace,
    BetaBinomial,
    Hypergeometric,
    Custom
};

// Confidence interval
struct ConfidenceInterval {
    double lowerBound;
    double upperBound;
    double confidenceLevel;
    std::string statisticName;
    std::chrono::system_clock::time_point calculationTime;
    
    ConfidenceInterval() : lowerBound(0.0), upperBound(0.0), confidenceLevel(0.95) {}
    ConfidenceInterval(double lower, double upper, double level, const std::string& name)
        : lowerBound(lower), upperBound(upper), confidenceLevel(level), statisticName(name),
          calculationTime(std::chrono::system_clock::now()) {}
};

// Hypothesis test result
struct HypothesisTestResult {
    std::string testName;
    StatisticalTestType testType;
    std::string hypothesis;
    double testStatistic;
    double pValue;
    double significanceLevel;
    bool rejectNullHypothesis;
    std::string conclusion;
    std::chrono::system_clock::time_point testTime;
    std::map<std::string, double> testParameters;
    std::map<std::string, std::string> testMetadata;
    
    HypothesisTestResult() : testStatistic(0.0), pValue(1.0), significanceLevel(0.05), 
                           rejectNullHypothesis(false), testTime(std::chrono::system_clock::now()) {}
    HypothesisTestResult(const std::string& name, StatisticalTestType type, double statistic, double pvalue)
        : testName(name), testType(type), testStatistic(statistic), pValue(pvalue),
          significanceLevel(0.05), rejectNullHypothesis(false), testTime(std::chrono::system_clock::now()) {}
};

// Probability distribution
struct ProbabilityDistribution {
    std::string distributionName;
    StatisticalDistribution distributionType;
    std::map<std::string, double> parameters;
    std::function<double(double)> probabilityDensityFunction;
    std::function<double(double)> cumulativeDistributionFunction;
    std::function<double(double)> inverseCumulativeDistributionFunction;
    std::chrono::system_clock::time_point definitionTime;
    
    ProbabilityDistribution() : definitionTime(std::chrono::system_clock::now()) {}
    ProbabilityDistribution(const std::string& name, StatisticalDistribution type)
        : distributionName(name), distributionType(type), definitionTime(std::chrono::system_clock::now()) {}
};

class ExperimentStatistics {
public:
    ExperimentStatistics();
    ~ExperimentStatistics();
    
    // Disable copying, enable moving
    ExperimentStatistics(const ExperimentStatistics&) = delete;
    ExperimentStatistics& operator=(const ExperimentStatistics&) = delete;
    ExperimentStatistics(ExperimentStatistics&&) noexcept;
    ExperimentStatistics& operator=(ExperimentStatistics&&) noexcept;
    
    // Descriptive statistics
    double calculateMean(const std::vector<double>& data) const;
    double calculateStandardDeviation(const std::vector<double>& data, double mean = 0.0) const;
    double calculateVariance(const std::vector<double>& data, double mean = 0.0) const;
    double calculateMedian(const std::vector<double>& data) const;
    double calculateMode(const std::vector<double>& data) const;
    double calculateRange(const std::vector<double>& data) const;
    double calculateInterquartileRange(const std::vector<double>& data) const;
    double calculateMeanAbsoluteDeviation(const std::vector<double>& data, double mean = 0.0) const;
    double calculateMedianAbsoluteDeviation(const std::vector<double>& data, double median = 0.0) const;
    double calculateSkewness(const std::vector<double>& data) const;
    double calculateKurtosis(const std::vector<double>& data) const;
    
    // Percentiles and quantiles
    double calculatePercentile(const std::vector<double>& data, double percentile) const;
    double calculateQuantile(const std::vector<double>& data, double quantile) const;
    std::vector<double> calculateAllPercentiles(const std::vector<double>& data) const;
    std::vector<double> calculateQuantiles(const std::vector<double>& data, 
                                           const std::vector<double>& quantiles) const;
    
    // Correlation and regression
    double calculateCorrelation(const std::vector<double>& x, const std::vector<double>& y) const;
    double calculatePearsonCorrelation(const std::vector<double>& x, const std::vector<double>& y) const;
    double calculateSpearmanCorrelation(const std::vector<double>& x, const std::vector<double>& y) const;
    double calculateKendallCorrelation(const std::vector<double>& x, const std::vector<double>& y) const;
    
    std::pair<double, double> calculateLinearRegression(const std::vector<double>& x, const std::vector<double>& y) const;
    std::pair<double, double> calculateLogisticRegression(const std::vector<double>& x, const std::vector<double>& y) const;
    std::pair<double, double> calculatePolynomialRegression(const std::vector<double>& x, const std::vector<double>& y, int degree) const;
    
    // Statistical tests
    HypothesisTestResult performTTest(const std::vector<double>& sample1, const std::vector<double>& sample2) const;
    HypothesisTestResult performANOVA(const std::vector<std::vector<double>>& samples) const;
    HypothesisTestResult performChiSquareTest(const std::vector<std::vector<int>>& contingencyTable) const;
    HypothesisTestResult performKolmogorovSmirnovTest(const std::vector<double>& sample1, const std::vector<double>& sample2) const;
    HypothesisTestResult performMannWhitneyUTest(const std::vector<double>& sample1, const std::vector<double>& sample2) const;
    HypothesisTestResult performKruskalWallisTest(const std::vector<std::vector<double>>& samples) const;
    
    // Confidence intervals
    ConfidenceInterval calculateMeanConfidenceInterval(const std::vector<double>& data, double confidenceLevel = 0.95) const;
    ConfidenceInterval calculateProportionConfidenceInterval(double proportion, size_t sampleSize, double confidenceLevel = 0.95) const;
    ConfidenceInterval calculateDifferenceConfidenceInterval(const std::vector<double>& sample1, const std::vector<double>& sample2, double confidenceLevel = 0.95) const;
    
    // Distribution fitting
    StatisticalDistribution fitDistribution(const std::vector<double>& data, StatisticalDistribution distributionType) const;
    bool estimateDistributionParameters(const std::vector<double>& data, StatisticalDistribution distributionType, std::map<std::string, double>& parameters) const;
    double calculateGoodnessOfFit(const std::vector<double>& data, StatisticalDistribution distribution, 
                                 const std::map<std::string, double>& parameters) const;
    
    // Time series analysis
    double calculateAutocorrelation(const std::vector<double>& data, int lag) const;
    double calculatePartialAutocorrelation(const std::vector<double>& data, int lag) const;
    std::vector<double> calculateMovingAverage(const std::vector<double>& data, int windowSize) const;
    std::vector<double> calculateExponentialMovingAverage(const std::vector<double>& data, double alpha) const;
    
    // Multivariate statistics
    std::map<std::string, double> calculatePrincipalComponents(const std::vector<std::vector<double>>& data) const;
    double calculateMultivariateSkewness(const std::vector<std::vector<double>>& data) const;
    double calculateMultivariateKurtosis(const std::vector<std::vector<double>>& data) const;
    
    // Non-parametric statistics
    double calculateWilcoxonSignedRankTest(const std::vector<double>& sample1, const std::vector<double>& sample2) const;
    double calculateFriedmanTest(const std::vector<std::vector<double>>& samples) const;
    
    // Statistical validation
    bool validateStatisticalAssumptions(const std::vector<double>& data) const;
    bool validateNormality(const std::vector<double>& data) const;
    bool validateHomoscedasticity(const std::vector<std::vector<double>>& samples) const;
    bool validateIndependence(const std::vector<double>& data) const;
    
    // Statistical modeling
    std::vector<double> simulateFromDistribution(StatisticalDistribution distribution,
                                                 const std::map<std::string, double>& parameters,
                                                 size_t sampleSize) const;
    double calculateBayesianPosterior(const std::vector<double>& data, 
                                     StatisticalDistribution prior,
                                     StatisticalDistribution likelihood) const;
    
    // Statistical visualization
    bool generateHistogram(const std::vector<double>& data, const std::string& outputPath) const;
    bool generateBoxPlot(const std::vector<double>& data, const std::string& outputPath) const;
    bool generateScatterPlot(const std::vector<double>& x, const std::vector<double>& y, 
                            const std::string& outputPath) const;
    bool generateQ-QPlot(const std::vector<double>& data, const std::string& outputPath) const;
    
    // Statistical export
    bool exportStatisticsToJSON(const std::string& filepath) const;
    bool exportStatisticsToCSV(const std::string& filepath) const;
    bool exportStatisticsToXML(const std::string& filepath) const;
    bool exportStatisticsToDatabase(const std::string& connectionString, const std::string& tableName) const;
    
    // Statistical import
    bool importStatisticsFromJSON(const std::string& filepath);
    bool importStatisticsFromCSV(const std::string& filepath);
    bool importStatisticsFromXML(const std::string& filepath);
    bool importStatisticsFromDatabase(const std::string& connectionString, const std::string& tableName);
    
    // Statistical analysis integration
    std::vector<HypothesisTestResult> performCompleteStatisticalAnalysis(const std::vector<std::vector<double>>& datasets) const;
    std::map<std::string, ConfidenceInterval> calculateAllConfidenceIntervals(const std::vector<double>& data) const;
    std::map<std::string, HypothesisTestResult> performAllStatisticalTests(const std::vector<std::vector<double>>& datasets) const;
    
    // Statistical optimization
    bool optimizeStatisticalParameters(const std::vector<double>& data, StatisticalDistribution distributionType);
    bool enableStatisticalValidation(bool enable);
    bool setStatisticalSignificanceLevel(double alpha);
    bool enableRobustStatisticalMethods(bool enable);
    
    // Statistical cleanup
    bool cleanupStatisticalData(const std::string& experimentName);
    bool cleanupOldStatisticalResults(const std::chrono::system_clock::time_point& cutoffTime);
    bool resetStatisticalAnalysis(const std::string& experimentName);
    bool clearAllStatisticalData();
    std::vector<std::string> getStatisticalAnalysisExperiments() const;
    size_t getStatisticalAnalysisCount() const;
    
protected:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm