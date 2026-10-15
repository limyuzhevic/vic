#pragma once

/**
 * Statistical Analysis Utilities for NLM Benchmarking
 * 
 * Provides comprehensive statistical analysis functions for benchmark results,
 * including descriptive statistics, hypothesis testing, and distribution analysis.
 */

#include <vector>
#include <cmath>
#include <algorithm>
#include <numeric>
#include <stdexcept>
#include <map>
#include <string>
#include <numeric>

namespace nlm {

/**
 * Statistical Analysis Utilities
 * 
 * Class providing static methods for statistical analysis of benchmark results.
 * Designed for numerical accuracy and performance.
 */
class StatisticalUtils {
public:
    // ========== BASIC STATISTICS ==========
    
    /**
     * Calculate arithmetic mean of a vector of values
     * @param values Input values
     * @return Mean value
     */
    static double mean(const std::vector<double>& values) {
        if (values.empty()) {
            return 0.0;
        }
        return std::accumulate(values.begin(), values.end(), 0.0) / values.size();
    }
    
    /**
     * Calculate median (middle value) of a sorted vector
     * @param values Input values (will be sorted)
     * @return Median value
     */
    static double median(std::vector<double> values) {
        if (values.empty()) {
            return 0.0;
        }
        std::sort(values.begin(), values.end());
        size_t n = values.size();
        if (n % 2 == 0) {
            return (values[n/2 - 1] + values[n/2]) / 2.0;
        } else {
            return values[n/2];
        }
    }
    
    /**
     * Calculate standard deviation (population)
     * @param values Input values
     * @param biasCorrected If true, use sample standard deviation (n-1)
     * @return Standard deviation
     */
    static double standardDeviation(const std::vector<double>& values, 
                                   bool biasCorrected = false) {
        if (values.size() <= 1) return 0.0;
        
        double m = mean(values);
        double sumSquared = 0.0;
        for (double x : values) {
            sumSquared += (x - m) * (x - m);
        }
        
        double divisor = biasCorrected ? values.size() - 1 : values.size();
        return std::sqrt(sumSquared / divisor);
    }
    
    /**
     * Calculate variance
     * @param values Input values
     * @param biasCorrected If true, use sample variance (n-1)
     * @return Variance
     */
    static double variance(const std::vector<double>& values, 
                          bool biasCorrected = false) {
        double stdDev = standardDeviation(values, biasCorrected);
        return stdDev * stdDev;
    }
    
    /**
     * Calculate coefficient of variation (stdDev/mean)
     * @param values Input values
     * @return Coefficient of variation (0 if mean is 0)
     */
    static double coefficientOfVariation(const std::vector<double>& values) {
        double m = mean(values);
        if (std::abs(m) < 1e-10) return 0.0;
        return standardDeviation(values) / m;
    }
    
    /**
     * Calculate minimum value
     * @param values Input values
     * @return Minimum value
     */
    static double min(const std::vector<double>& values) {
        if (values.empty()) return 0.0;
        return *std::min_element(values.begin(), values.end());
    }
    
    /**
     * Calculate maximum value
     * @param values Input values
     * @return Maximum value
     */
    static double max(const std::vector<double>& values) {
        if (values.empty()) return 0.0;
        return *std::max_element(values.begin(), values.end());
    }
    
    /**
     * Calculate range (max - min)
     * @param values Input values
     * @return Range
     */
    static double range(const std::vector<double>& values) {
        if (values.empty()) return 0.0;
        return max(values) - min(values);
    }
    
    // ========== CENTRAL TENDENCY ==========
    
    /**
     * Calculate geometric mean (useful for growth rates)
     * @param values Input values (must be positive)
     * @return Geometric mean
     */
    static double geometricMean(const std::vector<double>& values) {
        if (values.empty()) return 0.0;
        double product = 1.0;
        for (double x : values) {
            if (x <= 0.0) return 0.0; // Invalid for non-positive values
            product *= x;
        }
        return std::pow(product, 1.0 / values.size());
    }
    
    /**
     * Calculate harmonic mean (useful for rates)
     * @param values Input values (must be positive)
     * @return Harmonic mean
     */
    static double harmonicMean(const std::vector<double>& values) {
        if (values.empty()) return 0.0;
        double sum = 0.0;
        for (double x : values) {
            if (x <= 0.0) return 0.0; // Invalid for non-positive values
            sum += 1.0 / x;
        }
        return values.size() / sum;
    }
    
    // ========== DISTRIBUTIVE STATISTICS ==========
    
    /**
     * Calculate quantiles (percentiles)
     * @param values Input values
     * @param quantile Quantile value (0.0 to 1.0)
     * @return Quantile value
     */
    static double quantile(std::vector<double> values, double quantile) {
        if (values.empty() || quantile < 0.0 || quantile > 1.0) {
            return 0.0;
        }
        std::sort(values.begin(), values.end());
        double pos = quantile * (values.size() - 1);
        size_t lower = static_cast<size_t>(std::floor(pos));
        size_t upper = static_cast<size_t>(std::ceil(pos));
        
        if (lower == upper) {
            return values[lower];
        }
        
        double weight = pos - lower;
        return values[lower] * (1.0 - weight) + values[upper] * weight;
    }
    
    /**
     * Calculate quartiles (25%, 50%, 75%)
     * @param values Input values
     * @param q1 Reference to store 25th percentile
     * @param q2 Reference to store 50th percentile (median)
     * @param q3 Reference to store 75th percentile
     */
    static void quartiles(const std::vector<double>& values, double& q1, double& q2, double& q3) {
        q1 = quantile(values, 0.25);
        q2 = median(values);
        q3 = quantile(values, 0.75);
    }
    
    /**
     * Calculate interquartile range (IQR = Q3 - Q1)
     * @param values Input values
     * @return IQR
     */
    static double interquartileRange(const std::vector<double>& values) {
        double q1, q2, q3;
        quartiles(values, q1, q2, q3);
        return q3 - q1;
    }
    
    // ========== DISTRIBUTION ANALYSIS ==========
    
    /**
     * Determine if data follows normal distribution (Shapiro-Wilk test approximation)
     * @param values Input values
     * @param alpha Significance level (0.05 for 95% confidence)
     * @return true if data appears normal
     */
    static bool isNormal(const std::vector<double>& values, double alpha = 0.05) {
        if (values.size() < 3) return true; // Cannot test
        
        double mean = StatisticalUtils::mean(values);
        double stdDev = StatisticalUtils::standardDeviation(values);
        
        // Calculate skewness and kurtosis
        double skewness = 0.0, kurtosis = 0.0;
        size_t n = values.size();
        
        for (double x : values) {
            double z = (x - mean) / stdDev;
            skewness += z * z * z;
            kurtosis += z * z * z * z;
        }
        
        skewness /= n;
        kurtosis /= n;
        
        // Approximate Shapiro-Wilk criterion
        double w = 1.0 - (std::abs(skewness) + std::abs(kurtosis - 3.0)) / 6.0;
        return w > alpha;
    }
    
    /**
     * Determine if data follows exponential distribution
     * @param values Input values (must be positive)
     * @return true if data appears exponential
     */
    static bool isExponential(const std::vector<double>& values) {
        // Exponential has mean = standard deviation for true exponential distribution
        double m = mean(values);
        double s = standardDeviation(values);
        return std::abs(s - m) / (m + 1e-10) < 0.1;
    }
    
    /**
     * Fit best distribution to data (simple heuristic)
     * @param values Input values
     * @return Name of best fitting distribution
     */
    static std::string fitDistribution(const std::vector<double>& values) {
        if (values.empty()) return "unknown";
        
        double skewness = 0.0, kurtosis = 0.0;
        double m = mean(values);
        double s = standardDeviation(values);
        size_t n = values.size();
        
        for (double x : values) {
            double z = (x - m) / s;
            skewness += z * z * z;
            kurtosis += z * z * z * z;
        }
        
        skewness /= n;
        kurtosis /= n;
        
        // Simple heuristic based on skewness and kurtosis
        if (std::abs(skewness) < 0.5 && std::abs(kurtosis - 3.0) < 1.0) {
            return "normal";
        } else if (skewness > 1.0) {
            return "right_skewed";
        } else if (skewness < -1.0) {
            return "left_skewed";
        } else if (kurtosis > 3.5) {
            return "heavy_tailed";
        } else {
            return "unknown";
        }
    }
    
    // ========== OUTLIER DETECTION ==========
    
    /**
     * Detect outliers using Z-score method
     * @param values Input values
     * @param zThreshold Z-score threshold (typically 2.0 or 3.0)
     * @return Vector of indices of outliers
     */
    static std::vector<size_t> detectOutliers(const std::vector<double>& values, 
                                             double zThreshold = 2.0) {
        std::vector<size_t> outliers;
        double m = mean(values);
        double s = standardDeviation(values);
        
        if (s < 1e-10) return outliers; // No deviation
        
        for (size_t i = 0; i < values.size(); ++i) {
            double z = std::abs((values[i] - m) / s);
            if (z > zThreshold) {
                outliers.push_back(i);
            }
        }
        
        return outliers;
    }
    
    /**
     * Detect outliers using IQR method
     * @param values Input values
     * @param iqrMultiplier IQR multiplier (typically 1.5 or 3.0)
     * @return Vector of indices of outliers
     */
    static std::vector<size_t> detectOutliersIQR(const std::vector<double>& values, 
                                                 double iqrMultiplier = 1.5) {
        std::vector<size_t> outliers;
        double q1, q2, q3;
        quartiles(values, q1, q2, q3);
        double iqr = q3 - q1;
        double lower = q1 - iqrMultiplier * iqr;
        double upper = q3 + iqrMultiplier * iqr;
        
        for (size_t i = 0; i < values.size(); ++i) {
            if (values[i] < lower || values[i] > upper) {
                outliers.push_back(i);
            }
        }
        
        return outliers;
    }
    
    // ========== CONFIDENCE INTERVALS ==========
    
    /**
     * Calculate confidence interval for mean using t-distribution
     * @param values Input values
     * @param confidenceLevel Confidence level (0.95 for 95% CI)
     * @return Pair of (lower_bound, upper_bound)
     */
    static std::pair<double, double> confidenceIntervalMean(
        const std::vector<double>& values, double confidenceLevel = 0.95) {
        if (values.size() <= 1) {
            return {mean(values), mean(values)};
        }
        
        double m = mean(values);
        double s = standardDeviation(values, true); // sample std dev
        double n = values.size();
        
        // Approximate t-value for common confidence levels
        double tValue;
        if (confidenceLevel >= 0.999) tValue = 4.0;
        else if (confidenceLevel >= 0.999) tValue = 3.5;
        else if (confidenceLevel >= 0.99) tValue = 2.8;
        else if (confidenceLevel >= 0.95) tValue = 2.0;
        else if (confidenceLevel >= 0.90) tValue = 1.7;
        else tValue = 1.3;
        
        double margin = tValue * s / std::sqrt(n);
        
        return {m - margin, m + margin};
    }
    
    /**
     * Calculate confidence interval for median using bootstrap
     * @param values Input values
     * @param confidenceLevel Confidence level (0.95 for 95% CI)
     * @param numBootstrapSamples Number of bootstrap samples
     * @return Pair of (lower_bound, upper_bound)
     */
    static std::pair<double, double> confidenceIntervalMedian(
        const std::vector<double>& values, double confidenceLevel = 0.95,
        size_t numBootstrapSamples = 1000) {
        if (values.empty()) return {0.0, 0.0};
        
        std::vector<double> bootstrapMedians;
        bootstrapMedians.reserve(numBootstrapSamples);
        
        std::vector<double> data = values;
        std::mt19937 rng(42); // Fixed seed for reproducibility
        std::uniform_int_distribution<size_t> dist(0, values.size() - 1);
        
        for (size_t i = 0; i < numBootstrapSamples; ++i) {
            std::vector<double> sample;
            sample.reserve(values.size());
            for (size_t j = 0; j < values.size(); ++j) {
                sample.push_back(data[dist(rng)]);
            }
            bootstrapMedians.push_back(median(sample));
        }
        
        std::sort(bootstrapMedians.begin(), bootstrapMedians.end());
        size_t lowerIndex = static_cast<size_t>((1.0 - confidenceLevel) / 2 * numBootstrapSamples);
        size_t upperIndex = static_cast<size_t>((1.0 + confidenceLevel) / 2 * numBootstrapSamples);
        
        if (lowerIndex >= numBootstrapSamples) lowerIndex = 0;
        if (upperIndex >= numBootstrapSamples) upperIndex = numBootstrapSamples - 1;
        
        return {bootstrapMedians[lowerIndex], bootstrapMedians[upperIndex]};
    }
    
    // ========== HYPOTHESIS TESTING ==========
    
    /**
     * Perform two-sample t-test (assuming equal variance)
     * @param sample1 First sample
     * @param sample2 Second sample
     * @return t-statistic
     */
    static double tTest(const std::vector<double>& sample1, const std::vector<double>& sample2) {
        if (sample1.empty() || sample2.empty()) return 0.0;
        
        double m1 = mean(sample1);
        double m2 = mean(sample2);
        double s1 = standardDeviation(sample1, true);
        double s2 = standardDeviation(sample2, true);
        size_t n1 = sample1.size(), n2 = sample2.size();
        
        double pooledVariance = ((n1 - 1) * s1 * s1 + (n2 - 1) * s2 * s2) / (n1 + n2 - 2);
        double pooledStd = std::sqrt(pooledVariance);
        
        if (pooledStd < 1e-10) return 0.0;
        
        return (m1 - m2) / (pooledStd * std::sqrt(1.0/n1 + 1.0/n2));
    }
    
    /**
     * Perform F-test (variance ratio test)
     * @param sample1 First sample
     * @param sample2 Second sample
     * @return F-statistic
     */
    static double fTest(const std::vector<double>& sample1, const std::vector<double>& sample2) {
        if (sample1.empty() || sample2.empty()) return 0.0;
        
        double v1 = variance(sample1, true);
        double v2 = variance(sample2, true);
        
        if (v2 < v1) std::swap(v1, v2); // Larger variance in numerator
        
        if (v2 < 1e-10) return 0.0;
        
        return v1 / v2;
    }
    
    /**
     * Detect performance regression between baseline and current results
     * @param baseline Baseline performance values
     * @param current Current performance values
     * @param threshold Threshold for regression detection (e.g., 0.05 for 5%)
     * @return true if regression detected
     */
    static bool detectRegression(const std::vector<double>& baseline,
                                 const std::vector<double>& current,
                                 double threshold = 0.05) {
        if (baseline.empty() || current.empty() || baseline.size() != current.size()) {
            return false;
        }
        
        double baselineMean = mean(baseline);
        double currentMean = mean(current);
        
        if (baselineMean < 1e-10) return false;
        
        double degradation = (baselineMean - currentMean) / baselineMean;
        return degradation > threshold;
    }
    
    // ========== PERFORMANCE METRICS ==========
    
    /**
     * Calculate efficiency score (achieved vs target)
     * @param achieved Achieved value
     * @param target Target value
     * @param higherIsBetter If true, higher values are better
     * @return Efficiency score (0.0 to 1.0)
     */
    static double efficiencyScore(double achieved, double target, bool higherIsBetter = true) {
        if (target <= 0.0) return 0.0;
        
        if (higherIsBetter) {
            return std::min(achieved / target, 1.0); // Cap at 100%
        } else {
            return std::min(target / achieved, 1.0); // Cap at 100%
        }
    }
    
    /**
     * Calculate memory efficiency ratio
     * @param usedBytes Memory actually used
     * @param totalBytes Total memory allocated
     * @return Memory efficiency (0.0 to 1.0)
     */
    static double memoryEfficiency(size_t usedBytes, size_t totalBytes) {
        if (totalBytes == 0) return 0.0;
        return static_cast<double>(usedBytes) / totalBytes;
    }
    
    /**
     * Calculate parallel efficiency
     * @param threads Number of threads used
     * @param parallelTime Time with threads
     * @param sequentialTime Time without threads
     * @return Parallel efficiency (0.0 to 1.0)
     */
    static double parallelEfficiency(int threads, double parallelTime, double sequentialTime) {
        if (threads <= 1 || sequentialTime <= 0.0) return 0.0;
        
        double idealSpeedup = static_cast<double>(threads);
        double actualSpeedup = sequentialTime / parallelTime;
        
        return actualSpeedup / idealSpeedup;
    }
    
    // ========== DATA UTILITIES ==========
    
    /**
     * Calculate geometric mean of ratios (useful for performance multipliers)
     * @param numerators Numerator values
     * @param denominators Denominator values
     * @return Geometric mean of ratios
     */
    static double geometricMeanRatio(const std::vector<double>& numerators,
                                     const std::vector<double>& denominators) {
        if (numerators.size() != denominators.size() || numerators.empty()) {
            return 0.0;
        }
        
        std::vector<double> ratios;
        ratios.reserve(numerators.size());
        
        for (size_t i = 0; i < numerators.size(); ++i) {
            if (denominators[i] > 0.0) {
                ratios.push_back(numerators[i] / denominators[i]);
            }
        }
        
        return geometricMean(ratios);
    }
    
    /**
     * Calculate harmonic mean of speeds (useful for throughput)
     * @param speeds Speed values (e.g., operations per second)
     * @return Harmonic mean speed
     */
    static double harmonicMeanSpeed(const std::vector<double>& speeds) {
        return harmonicMean(speeds);
    }
    
    /**
     * Calculate coefficient of variation (stdDev/mean) as percentage
     * @param values Input values
     * @return Coefficient of variation percentage
     */
    static double coefficientOfVariationPercent(const std::vector<double>& values) {
        return coefficientOfVariation(values) * 100.0;
    }
    
    /**
     * Calculate percentiles
     * @param values Input values
     * @param percentiles Vector of percentile values (0.0 to 100.0)
     * @return Vector of percentile values
     */
    static std::vector<double> percentiles(const std::vector<double>& values,
                                           const std::vector<double>& percentiles) {
        std::vector<double> result;
        result.reserve(percentiles.size());
        
        for (double p : percentiles) {
            result.push_back(quantile(values, p / 100.0));
        }
        
        return result;
    }
    
    /**
     * Normalize data to range [0, 1]
     * @param values Input values
     * @param targetMin Target minimum (default -inf)
     * @param targetMax Target maximum (default inf)
     * @return Normalized values
     */
    static std::vector<double> normalize(const std::vector<double>& values,
                                         double targetMin = -1e100, double targetMax = 1e100) {
        if (values.empty()) return {};
        
        double min = StatisticalUtils::min(values);
        double max = StatisticalUtils::max(values);
        
        if (std::abs(max - min) < 1e-10) {
            // All values are the same
            std::vector<double> result(values.size(), 0.5);
            return result;
        }
        
        std::vector<double> normalized;
        normalized.reserve(values.size());
        
        for (double x : values) {
            double normalizedValue = (x - min) / (max - min);
            normalizedValue = (normalizedValue - targetMin) / (targetMax - targetMin);
            normalized.push_back(normalizedValue);
        }
        
        return normalized;
    }
    
    /**
     * Calculate rank of each value (1 = smallest, n = largest)
     * @param values Input values
     * @param descending If true, highest gets rank 1
     * @return Vector of ranks
     */
    static std::vector<double> rank(const std::vector<double>& values, bool descending = false) {
        if (values.empty()) return {};
        
        std::vector<size_t> indices(values.size());
        std::iota(indices.begin(), indices.end(), 0);
        
        if (descending) {
            std::sort(indices.begin(), indices.end(),
                     [&values](size_t a, size_t b) { return values[a] > values[b]; });
        } else {
            std::sort(indices.begin(), indices.end(),
                     [&values](size_t a, size_t b) { return values[a] < values[b]; });
        }
        
        std::vector<double> ranks(values.size());
        for (size_t rank = 0; rank < indices.size(); ++rank) {
            ranks[indices[rank]] = descending ? rank + 1 : values.size() - rank;
        }
        
        return ranks;
    }
    
    /**
     * Calculate Spearman's rank correlation coefficient
     * @param x1 First dataset
     * @param x2 Second dataset
     * @return Correlation coefficient (-1 to 1)
     */
    static double spearmanCorrelation(const std::vector<double>& x1, const std::vector<double>& x2) {
        if (x1.size() != x2.size() || x1.size() < 2) return 0.0;
        
        std::vector<double> rank1 = rank(x1);
        std::vector<double> rank2 = rank(x2);
        
        return StatisticalUtils::pearsonCorrelation(rank1, rank2);
    }
    
    /**
     * Calculate Pearson correlation coefficient
     * @param x1 First dataset
     * @param x2 Second dataset
     * @return Correlation coefficient (-1 to 1)
     */
    static double pearsonCorrelation(const std::vector<double>& x1, const std::vector<double>& x2) {
        if (x1.size() != x2.size() || x1.size() < 2) return 0.0;
        
        double m1 = mean(x1), m2 = mean(x2);
        double stdDev1 = standardDeviation(x1, true);
        double stdDev2 = standardDeviation(x2, true);
        
        if (stdDev1 < 1e-10 || stdDev2 < 1e-10) return 0.0;
        
        double covariance = 0.0;
        for (size_t i = 0; i < x1.size(); ++i) {
            covariance += (x1[i] - m1) * (x2[i] - m2);
        }
        covariance /= x1.size() - 1;
        
        return covariance / (stdDev1 * stdDev2);
    }
    
    /**
     * Calculate linear regression slope and intercept
     * @param x Independent variable
     * @param y Dependent variable
     * @param intercept If true, also calculate intercept
     * @return Pair of (slope, intercept) or just slope if intercept=false
     */
    static std::pair<double, double> linearRegression(const std::vector<double>& x,
                                                      const std::vector<double>& y,
                                                      bool intercept = true) {
        if (x.size() != y.size() || x.size() < 2) return {0.0, 0.0};
        
        double meanX = mean(x);
        double meanY = mean(y);
        
        double numerator = 0.0, denominator = 0.0;
        for (size_t i = 0; i < x.size(); ++i) {
            numerator += (x[i] - meanX) * (y[i] - meanY);
            denominator += (x[i] - meanX) * (x[i] - meanX);
        }
        
        if (denominator < 1e-10) return {0.0, 0.0};
        
        double slope = numerator / denominator;
        double intercept = intercept ? meanY - slope * meanX : 0.0;
        
        return {slope, intercept};
    }
}; // class StatisticalUtils

} // namespace nlm