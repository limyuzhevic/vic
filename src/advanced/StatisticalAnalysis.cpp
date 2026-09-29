#pragma once

#include <algorithm>
#include <cmath>
#include <fstream>
#include <limits>
#include <map>
#include <memory>
#include <numeric>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>
#include <random>
#include "../core/Config/Config.hpp"
#include "../brain/Brain.hpp"

namespace nlm {

// Statistical dataset
struct StatisticalData {
    std::string datasetId;
    std::string description;
    std::vector<double> values;
    std::vector<std::string> labels;
    std::map<std::string, std::vector<double>> timeSeries;
    double mean;
    double variance;
    double stddev;
    double min;
    double max;
    int sampleSize;
    
    StatisticalData(const std::string& id, const std::string& desc)
        : datasetId(id), description(desc), mean(0.0), variance(0.0),
          stddev(0.0), min(0.0), max(0.0), sampleSize(0) {}
};

// Statistical distribution
class StatisticalDistribution {
protected:
    std::string distributionName;
    std::map<std::string, double> parameters;
    double mean;
    double variance;
    std::mt19937 rng;
    
public:
    StatisticalDistribution(const std::string& name, const std::map<std::string, double>& params)
        : distributionName(name), parameters(params), rng(std::random_device{}()) {
        calculateMoments();
    }
    
    virtual ~StatisticalDistribution() = default;
    
    virtual double pdf(double x) const = 0;
    virtual double cdf(double x) const = 0;
    virtual double quantile(double p) const = 0;
    virtual double sample() const = 0;
    
    std::string getDistributionName() const { return distributionName; }
    double getMean() const { return mean; }
    double getVariance() const { return variance; }
    double getStdDev() const { return std::sqrt(variance); }
    
protected:
    void calculateMoments() {
        mean = 0.0;
        variance = 0.0;
        
        // In real implementation, would calculate moments based on distribution type
        if (distributionName == "normal") {
            if (parameters.find("mu") != parameters.end() && parameters.find("sigma") != parameters.end()) {
                mean = parameters.at("mu");
                variance = parameters.at("sigma") * parameters.at("sigma");
            }
        } else if (distributionName == "uniform") {
            if (parameters.find("a") != parameters.end() && parameters.find("b") != parameters.end()) {
                mean = (parameters.at("a") + parameters.at("b")) / 2.0;
                variance = (parameters.at("b") - parameters.at("a")) * (parameters.at("b") - parameters.at("a")) / 12.0;
            }
        }
        // Add other distributions
    }
};

// Normal distribution
class NormalDistribution : public StatisticalDistribution {
public:
    NormalDistribution(double mu = 0.0, double sigma = 1.0)
        : StatisticalDistribution("normal", {{ "mu", mu }, { "sigma", sigma }}) {}
    
    double pdf(double x) const override {
        double sigma = parameters.at("sigma");
        double mu = parameters.at("mu");
        return (1.0 / (sigma * std::sqrt(2.0 * M_PI))) * 
               std::exp(-0.5 * std::pow((x - mu) / sigma, 2));
    }
    
    double cdf(double x) const override {
        double sigma = parameters.at("sigma");
        double mu = parameters.at("mu");
        return 0.5 * (1.0 + std::erf((x - mu) / (sigma * std::sqrt(2.0))));
    }
    
    double quantile(double p) const override {
        double sigma = parameters.at("sigma");
        double mu = parameters.at("mu");
        return mu + sigma * std::sqrt(2.0) * std::erf(std::cinv(p));
    }
    
    double sample() const override {
        double sigma = parameters.at("sigma");
        double mu = parameters.at("mu");
        return mu + sigma * std::normal_distribution<double>(0.0, 1.0)(rng);
    }
};

// Descriptive statistics calculator
class DescriptiveStatistics {
public:
    static double calculateMean(const std::vector<double>& data) {
        if (data.empty()) return 0.0;
        return std::accumulate(data.begin(), data.end(), 0.0) / data.size();
    }
    
    static double calculateVariance(const std::vector<double>& data, double mean) {
        if (data.size() <= 1) return 0.0;
        
        double sumSq = 0.0;
        for (double val : data) {
            double diff = val - mean;
            sumSq += diff * diff;
        }
        
        return sumSq / (data.size() - 1);
    }
    
    static double calculateStdDev(double variance) {
        return std::sqrt(std::max(0.0, variance));
    }
    
    static double calculateMedian(std::vector<double> data) {
        if (data.empty()) return 0.0;
        
        std::sort(data.begin(), data.end());
        size_t n = data.size();
        
        if (n % 2 == 0) {
            return (data[n/2 - 1] + data[n/2]) / 2.0;
        } else {
            return data[n/2];
        }
    }
    
    static double calculateIQR(std::vector<double> data) {
        if (data.size() < 4) return 0.0;
        
        std::sort(data.begin(), data.end());
        size_t n = data.size();
        
        size_t q1Index = n / 4;
        size_t q3Index = 3 * n / 4;
        
        double q1 = data[q1Index];
        double q3 = data[q3Index];
        
        return q3 - q1;
    }
    
    static double calculateSkewness(const std::vector<double>& data, double mean, double stddev) {
        if (stddev <= 0.0 || data.size() <= 2) return 0.0;
        
        double n = data.size();
        double sum = 0.0;
        
        for (double val : data) {
            sum += std::pow((val - mean) / stddev, 3);
        }
        
        return (n / ((n - 1) * (n - 2))) * sum;
    }
    
    static double calculateKurtosis(const std::vector<double>& data, double mean, double stddev) {
        if (stddev <= 0.0 || data.size() <= 3) return 0.0;
        
        double n = data.size();
        double sum = 0.0;
        
        for (double val : data) {
            sum += std::pow((val - mean) / stddev, 4);
        }
        
        return ((n * (n + 1)) / ((n - 1) * (n - 2) * (n - 3))) * sum - 
               (3 * (n - 1) * (n - 1)) / ((n - 2) * (n - 3));
    }
};

// Hypothesis testing
class HypothesisTesting {
public:
    struct HypothesisTestResult {
        std::string testName;
        std::string nullHypothesis;
        std::string alternativeHypothesis;
        double testStatistic;
        double pValue;
        bool rejectNullHypothesis;
        double confidenceLevel;
        double effectSize;
        std::string conclusion;
    };
    
    // t-test for single sample
    static HypothesisTestResult tTestSingleSample(const std::vector<double>& data, 
                                                  double populationMean,
                                                  double alpha = 0.05) {
        HypothesisTestResult result;
        result.testName = "One-sample t-test";
        result.nullHypothesis = "Sample mean = population mean";
        result.alternativeHypothesis = "Sample mean ≠ population mean";
        
        double sampleMean = DescriptiveStatistics::calculateMean(data);
        double sampleStd = std::sqrt(DescriptiveStatistics::calculateVariance(data, sampleMean));
        double n = data.size();
        
        result.testStatistic = (sampleMean - populationMean) / (sampleStd / std::sqrt(n));
        
        // Calculate p-value using t-distribution (simplified)
        result.pValue = calculateT_pValue(result.testStatistic, n - 1);
        
        result.rejectNullHypothesis = result.pValue < alpha;
        result.confidenceLevel = 1.0 - alpha;
        result.effectSize = (sampleMean - populationMean) / sampleStd;
        
        result.conclusion = result.rejectNullHypothesis ? 
            "Reject null hypothesis" : "Fail to reject null hypothesis";
        
        return result;
    }
    
    // Independent t-test
    static HypothesisTestResult tTestIndependent(const std::vector<double>& sample1,
                                                 const std::vector<double>& sample2,
                                                 double alpha = 0.05) {
        HypothesisTestResult result;
        result.testName = "Independent t-test";
        result.nullHypothesis = "Means of two populations are equal";
        result.alternativeHypothesis = "Means of two populations are different";
        
        double mean1 = DescriptiveStatistics::calculateMean(sample1);
        double mean2 = DescriptiveStatistics::calculateMean(sample2);
        double var1 = DescriptiveStatistics::calculateVariance(sample1, mean1);
        double var2 = DescriptiveStatistics::calculateVariance(sample2, mean2);
        double n1 = sample1.size();
        double n2 = sample2.size();
        
        result.testStatistic = (mean1 - mean2) / 
                              std::sqrt(var1/n1 + var2/n2);
        
        double df = (var1/n1 + var2/n2) * (var1/n1 + var2/n2) / 
                   ((var1/n1)*(var1/n1)/(n1-1) + (var2/n2)*(var2/n2)/(n2-1));
        
        result.pValue = calculateT_pValue(std::abs(result.testStatistic), df);
        
        result.rejectNullHypothesis = result.pValue < alpha;
        result.confidenceLevel = 1.0 - alpha;
        result.effectSize = (mean1 - mean2) / std::sqrt((var1*(n1-1) + var2*(n2-1)) / (n1 + n2 - 2));
        
        result.conclusion = result.rejectNullHypothesis ? 
            "Reject null hypothesis" : "Fail to reject null hypothesis";
        
        return result;
    }
    
    // ANOVA
    static HypothesisTestResult anova(const std::vector<std::vector<double>>& groups,
                                     double alpha = 0.05) {
        HypothesisTestResult result;
        result.testName = "One-way ANOVA";
        result.nullHypothesis = "All group means are equal";
        result.alternativeHypothesis = "At least one group mean is different";
        
        // Simplified ANOVA implementation
        double overallMean = 0.0;
        double totalSumSquares = 0.0;
        double betweenGroupSumSquares = 0.0;
        double withinGroupSumSquares = 0.0;
        
        int totalN = 0;
        for (const auto& group : groups) {
            totalN += group.size();
            double groupMean = DescriptiveStatistics::calculateMean(group);
            overallMean += groupMean * group.size();
        }
        overallMean /= totalN;
        
        for (const auto& group : groups) {
            double groupMean = DescriptiveStatistics::calculateMean(group);
            for (double val : group) {
                totalSumSquares += (val - overallMean) * (val - overallMean);
                betweenGroupSumSquares += (val - groupMean) * (val - groupMean);
                withinGroupSumSquares += (val - overallMean) * (val - overallMean);
            }
        }
        
        result.testStatistic = (betweenGroupSumSquares / (groups.size() - 1)) /
                              (withinGroupSumSquares / (totalN - groups.size()));
        
        // Calculate p-value using F-distribution (simplified)
        result.pValue = calculateF_pValue(result.testStatistic, 
                                         groups.size() - 1, totalN - groups.size());
        
        result.rejectNullHypothesis = result.pValue < alpha;
        result.confidenceLevel = 1.0 - alpha;
        result.effectSize = std::sqrt(betweenGroupSumSquares / totalSumSquares);
        
        result.conclusion = result.rejectNullHypothesis ? 
            "Reject null hypothesis" : "Fail to reject null hypothesis";
        
        return result;
    }
    
private:
    static double calculateT_pValue(double t, double df) {
        // Simplified t-distribution CDF approximation
        return 2.0 * (1.0 - 0.5 * (1.0 + std::erf(std::abs(t) / std::sqrt(2.0))));
    }
    
    static double calculateF_pValue(double f, double df1, double df2) {
        // Simplified F-distribution CDF approximation
        return 1.0 - 0.5 * (1.0 + std::erf(std::sqrt(2.0 * f) / std::sqrt(df1 * df2)));
    }
};

// Statistical analysis manager
class StatisticalAnalysis {
private:
    std::vector<StatisticalData> datasets;
    std::vector<std::unique_ptr<StatisticalDistribution>> distributions;
    std::vector<HypothesisTesting::HypothesisTestResult> testResults;
    std::vector<std::vector<double>> correlationMatrices;
    std::vector<std::vector<double>> covarianceMatrices;
    std::unordered_map<std::string, double> statisticalMetrics;
    double confidenceLevel;
    double significanceLevel;
    
public:
    StatisticalAnalysis(const Config& config) {
        confidenceLevel = config.getOr("stats.confidence_level", 0.95);
        significanceLevel = config.getOr("stats.significance_level", 0.05);
    }
    
    // Add dataset
    void addDataset(const std::string& id, const std::string& description,
                    const std::vector<double>& values) {
        StatisticalData data(id, description);
        data.values = values;
        data.sampleSize = values.size();
        
        if (!values.empty()) {
            data.mean = DescriptiveStatistics::calculateMean(values);
            data.variance = DescriptiveStatistics::calculateVariance(values, data.mean);
            data.stddev = DescriptiveStatistics::calculateStdDev(data.variance);
            data.min = *std::min_element(values.begin(), values.end());
            data.max = *std::max_element(values.begin(), values.end());
        }
        
        datasets.push_back(data);
    }
    
    // Perform t-test
    HypothesisTesting::HypothesisTestResult performTTest(const std::string& dataset1Id,
                                                        const std::string& dataset2Id,
                                                        bool independent = true) {
        HypothesisTesting::HypothesisTestResult result;
        
        // Find datasets
        StatisticalData* data1 = nullptr;
        StatisticalData* data2 = nullptr;
        
        for (auto& data : datasets) {
            if (data.datasetId == dataset1Id) data1 = &data;
            if (data.datasetId == dataset2Id) data2 = &data;
        }
        
        if (!data1 || !data2) {
            throw std::runtime_error("Dataset not found");
        }
        
        if (independent) {
            result = HypothesisTesting::tTestIndependent(data1->values, data2->values, significanceLevel);
        } else {
            // Paired t-test would be implemented here
            result.testName = "Paired t-test (not implemented)";
            result.conclusion = "Test not implemented in this version";
        }
        
        testResults.push_back(result);
        return result;
    }
    
    // Calculate correlation matrix
    std::vector<std::vector<double>> calculateCorrelation(const std::vector<std::vector<double>>& datasets) {
        if (datasets.empty()) return std::vector<std::vector<double>>();
        
        size_t n = datasets.size();
        std::vector<std::vector<double>> correlation(n, std::vector<double>(n, 0.0));
        
        for (size_t i = 0; i < n; ++i) {
            for (size_t j = 0; j < n; ++j) {
                if (i == j) {
                    correlation[i][j] = 1.0;
                } else {
                    correlation[i][j] = calculateCorrelationCoefficient(datasets[i], datasets[j]);
                }
            }
        }
        
        correlationMatrices = correlation;
        return correlation;
    }
    
    // Calculate covariance matrix
    std::vector<std::vector<double>> calculateCovariance(const std::vector<std::vector<double>>& datasets) {
        if (datasets.empty()) return std::vector<std::vector<double>>();
        
        size_t n = datasets.size();
        std::vector<std::vector<double>> covariance(n, std::vector<double>(n, 0.0));
        
        for (size_t i = 0; i < n; ++i) {
            for (size_t j = 0; j < n; ++j) {
                covariance[i][j] = calculateCovarianceCoefficient(datasets[i], datasets[j]);
            }
        }
        
        covarianceMatrices = covariance;
        return covariance;
    }
    
    // Perform regression analysis
    struct RegressionResult {
        std::string regressionType;
        std::vector<double> coefficients;
        std::vector<double> predictions;
        double r2;
        double adjustedR2;
        double rmse;
        double fStatistic;
        double pValue;
    };
    
    RegressionResult performLinearRegression(const std::vector<std::vector<double>>& X,
                                             const std::vector<double>& y) {
        RegressionResult result;
        result.regressionType = "Multiple linear regression";
        
        size_t n = X.size();
        size_t p = X[0].size();
        
        // Add intercept column
        std::vector<std::vector<double>> X_with_intercept(n, std::vector<double>(p + 1, 0.0));
        for (size_t i = 0; i < n; ++i) {
            X_with_intercept[i][0] = 1.0;  // Intercept
            for (size_t j = 0; j < p; ++j) {
                X_with_intercept[i][j + 1] = X[i][j];
            }
        }
        
        // Calculate coefficients using normal equation
        // (X^T X)^(-1) X^T y
        std::vector<std::vector<double>> XtX(p + 1, std::vector<double>(p + 1, 0.0));
        std::vector<double> Xty(p + 1, 0.0);
        
        for (size_t i = 0; i < n; ++i) {
            for (size_t j = 0; j < p + 1; ++j) {
                for (size_t k = 0; k < p + 1; ++k) {
                    XtX[j][k] += X_with_intercept[i][j] * X_with_intercept[i][k];
                }
                Xty[j] += X_with_intercept[i][j] * y[i];
            }
        }
        
        // Solve normal equation (simplified - in real implementation would use matrix inversion)
        result.coefficients = solveLinearSystem(XtX, Xty);
        
        // Calculate predictions
        result.predictions.resize(n);
        for (size_t i = 0; i < n; ++i) {
            double prediction = result.coefficients[0];  // Intercept
            for (size_t j = 0; j < p; ++j) {
                prediction += result.coefficients[j + 1] * X[i][j];
            }
            result.predictions[i] = prediction;
        }
        
        // Calculate R-squared
        double yMean = DescriptiveStatistics::calculateMean(y);
        double ssTotal = 0.0, ssResidual = 0.0;
        
        for (size_t i = 0; i < n; ++i) {
            ssTotal += (y[i] - yMean) * (y[i] - yMean);
            ssResidual += (y[i] - result.predictions[i]) * (y[i] - result.predictions[i]);
        }
        
        result.r2 = (ssTotal - ssResidual) / ssTotal;
        result.adjustedR2 = 1.0 - (1.0 - result.r2) * (n - 1) / (n - p - 1);
        
        result.rmse = std::sqrt(ssResidual / n);
        
        // Calculate F-statistic
        double ssRegression = ssTotal - ssResidual;
        result.fStatistic = (ssRegression / p) / (ssResidual / (n - p - 1));
        
        // Calculate p-value
        result.pValue = calculateF_pValue(result.fStatistic, p, n - p - 1);
        
        return result;
    }
    
    // Calculate time series statistics
    struct TimeSeriesStats {
        std::vector<double> movingAverage;
        std::vector<double> movingStdDev;
        std::vector<double> volatility;
        std::vector<double> autocorrelation;
        std::vector<double> partialAutocorrelation;
    };
    
    TimeSeriesStats analyzeTimeSeries(const std::string& datasetId, int windowSize = 10) {
        TimeSeriesStats stats;
        
        // Find dataset
        StatisticalData* data = nullptr;
        for (auto& d : datasets) {
            if (d.datasetId == datasetId) {
                data = &d;
                break;
            }
        }
        
        if (!data) {
            throw std::runtime_error("Dataset not found");
        }
        
        // Calculate moving statistics
        stats.movingAverage.resize(data->values.size());
        stats.movingStdDev.resize(data->values.size());
        
        for (size_t i = 0; i < data->values.size(); ++i) {
            size_t start = std::max(0LL, static_cast<long long>(i) - windowSize + 1);
            size_t end = i + 1;
            
            std::vector<double> window(data->values.begin() + start, data->values.begin() + end);
            
            if (!window.empty()) {
                stats.movingAverage[i] = DescriptiveStatistics::calculateMean(window);
                double variance = DescriptiveStatistics::calculateVariance(window, stats.movingAverage[i]);
                stats.movingStdDev[i] = DescriptiveStatistics::calculateStdDev(variance);
            }
        }
        
        // Calculate volatility (rolling standard deviation)
        stats.volatility = stats.movingStdDev;
        
        // Calculate autocorrelation
        stats.autocorrelation.resize(std::min(20, static_cast<int>(data->values.size())));
        for (int lag = 0; lag < stats.autocorrelation.size(); ++lag) {
            stats.autocorrelation[lag] = calculateAutocorrelation(data->values, lag);
        }
        
        // Calculate partial autocorrelation
        stats.partialAutocorrelation.resize(std::min(10, static_cast<int>(data->values.size())));
        for (int lag = 0; lag < stats.partialAutocorrelation.size(); ++lag) {
            stats.partialAutocorrelation[lag] = calculatePartialAutocorrelation(data->values, lag);
        }
        
        return stats;
    }
    
    std::string getStatisticalAnalysisStatus() const {
        std::stringstream ss;
        ss << "Statistical Analysis:\n";
        ss << "  Number of Datasets: " << datasets.size() << "\n";
        ss << "  Number of Test Results: " << testResults.size() << "\n";
        ss << "  Confidence Level: " << confidenceLevel << "\n";
        ss << "  Significance Level: " << significanceLevel << "\n";
        
        for (const auto& result : testResults) {
            ss << "  Test: " << result.testName << "\n";
            ss << "    p-value: " << result.pValue << "\n";
            ss << "    conclusion: " << result.conclusion << "\n";
        }
        
        return ss.str();
    }
    
private:
    double calculateCorrelationCoefficient(const std::vector<double>& x, const std::vector<double>& y) {
        if (x.size() != y.size() || x.size() <= 1) return 0.0;
        
        double xMean = DescriptiveStatistics::calculateMean(x);
        double yMean = DescriptiveStatistics::calculateMean(y);
        
        double numerator = 0.0, denomX = 0.0, denomY = 0.0;
        
        for (size_t i = 0; i < x.size(); ++i) {
            double xDiff = x[i] - xMean;
            double yDiff = y[i] - yMean;
            numerator += xDiff * yDiff;
            denomX += xDiff * xDiff;
            denomY += yDiff * yDiff;
        }
        
        if (denomX > 0 && denomY > 0) {
            return numerator / std::sqrt(denomX * denomY);
        }
        
        return 0.0;
    }
    
    double calculateCovarianceCoefficient(const std::vector<double>& x, const std::vector<double>& y) {
        if (x.size() != y.size() || x.size() <= 1) return 0.0;
        
        double xMean = DescriptiveStatistics::calculateMean(x);
        double yMean = DescriptiveStatistics::calculateMean(y);
        
        double covariance = 0.0;
        for (size_t i = 0; i < x.size(); ++i) {
            covariance += (x[i] - xMean) * (y[i] - yMean);
        }
        
        return covariance / (x.size() - 1);
    }
    
    std::vector<double> solveLinearSystem(const std::vector<std::vector<double>>& A,
                                         const std::vector<double>& b) {
        std::vector<double> x(b.size(), 0.0);
        
        // Simple Gaussian elimination (in real implementation would use proper matrix solver)
        size_t n = A.size();
        
        for (size_t i = 0; i < n; ++i) {
            // Find pivot
            size_t maxRow = i;
            for (size_t k = i + 1; k < n; ++k) {
                if (std::abs(A[k][i]) > std::abs(A[maxRow][i])) {
                    maxRow = k;
                }
            }
            
            // Swap rows
            if (maxRow != i) {
                std::swap(A[i], A[maxRow]);
                std::swap(b[i], b[maxRow]);
            }
            
            // Eliminate below
            for (size_t k = i + 1; k < n; ++k) {
                double factor = A[k][i] / A[i][i];
                for (size_t j = i; j < n; ++j) {
                    A[k][j] -= factor * A[i][j];
                }
                b[k] -= factor * b[i];
            }
        }
        
        // Back substitution
        for (int i = n - 1; i >= 0; --i) {
            x[i] = b[i];
            for (size_t j = i + 1; j < n; ++j) {
                x[i] -= A[i][j] * x[j];
            }
            x[i] /= A[i][i];
        }
        
        return x;
    }
    
    double calculateAutocorrelation(const std::vector<double>& data, int lag) {
        if (data.size() <= lag) return 0.0;
        
        double mean = DescriptiveStatistics::calculateMean(data);
        double numerator = 0.0, denominator = 0.0;
        
        for (size_t i = lag; i < data.size(); ++i) {
            double diff = data[i] - mean;
            numerator += diff * (data[i - lag] - mean);
        }
        
        for (double val : data) {
            denominator += (val - mean) * (val - mean);
        }
        
        if (denominator > 0) {
            return numerator / denominator;
        }
        
        return 0.0;
    }
    
    double calculatePartialAutocorrelation(const std::vector<double>& data, int lag) {
        // Simplified partial autocorrelation
        if (data.size() <= lag) return 0.0;
        
        // In real implementation, would use Yule-Walker equations or other method
        return calculateAutocorrelation(data, lag);
    }
    
    double calculateF_pValue(double f, double df1, double df2) {
        // Simplified F-distribution CDF approximation
        return 1.0 - 0.5 * (1.0 + std::erf(std::sqrt(2.0 * f) / std::sqrt(df1 * df2)));
    }
};

} // namespace nlm
