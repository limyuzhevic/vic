#pragma once

#include <vector>
#include <numeric>
#include <algorithm>
#include <cmath>
#include <map>
#include <unordered_map>
#include <string>
#include <sstream>
#include <iomanip>

namespace nlm {

class StatsCalculator {
private:
    struct DistributionInfo {
        double min = 0.0;
        double max = 0.0;
        double mean = 0.0;
        double median = 0.0;
        double variance = 0.0;
        double stddev = 0.0;
        double skewness = 0.0;
        double kurtosis = 0.0;
        size_t count = 0;
    };

    struct PercentileInfo {
        double p25 = 0.0;  // First quartile
        double p50 = 0.0;  // Median
        double p75 = 0.0;  // Third quartile
        double p90 = 0.0;  // 90th percentile
        double p95 = 0.0;  // 95th percentile
        double p99 = 0.0;  // 99th percentile
    };

    struct TimeSeriesStats {
        std::string name;
        DistributionInfo distribution;
        PercentileInfo percentiles;
        std::chrono::duration<double> duration = std::chrono::duration<double>(0.0);
        double throughput = 0.0;  // Operations per second
        double efficiency = 1.0;   // 0.0 to 1.0 scale
    };

public:
    StatsCalculator() = default;

    // Basic statistics
    static double calculateMean(const std::vector<double>& values) {
        if (values.empty()) return 0.0;
        double sum = std::accumulate(values.begin(), values.end(), 0.0);
        return sum / values.size();
    }

    static double calculateVariance(const std::vector<double>& values) {
        if (values.size() < 2) return 0.0;
        double mean = calculateMean(values);
        double sumSquares = 0.0;
        for (double value : values) {
            sumSquares += (value - mean) * (value - mean);
        }
        return sumSquares / (values.size() - 1);
    }

    static double calculateStdDev(const std::vector<double>& values) {
        return std::sqrt(calculateVariance(values));
    }

    static double calculateMin(const std::vector<double>& values) {
        if (values.empty()) return 0.0;
        return *std::min_element(values.begin(), values.end());
    }

    static double calculateMax(const std::vector<double>& values) {
        if (values.empty()) return 0.0;
        return *std::max_element(values.begin(), values.end());
    }

    static double calculateMedian(std::vector<double> values) {
        if (values.empty()) return 0.0;
        std::sort(values.begin(), values.end());
        size_t mid = values.size() / 2;
        if (values.size() % 2 == 0) {
            return (values[mid - 1] + values[mid]) / 2.0;
        } else {
            return values[mid];
        }
    }

    // Percentiles
    static double calculatePercentile(std::vector<double> values, double percentile) {
        if (values.empty() || percentile < 0.0 || percentile > 100.0) return 0.0;
        
        std::sort(values.begin(), values.end());
        double index = (percentile / 100.0) * (values.size() - 1);
        
        if (index == static_cast<size_t>(index)) {
            return values[static_cast<size_t>(index)];
        } else {
            size_t lower = static_cast<size_t>(index);
            size_t upper = lower + 1;
            double weight = index - lower;
            return values[lower] * (1.0 - weight) + values[upper] * weight;
        }
    }

    static PercentileInfo calculatePercentiles(const std::vector<double>& values) {
        PercentileInfo info;
        if (values.empty()) return info;
        
        info.p25 = calculatePercentile(values, 25.0);
        info.p50 = calculatePercentile(values, 50.0);  // Median
        info.p75 = calculatePercentile(values, 75.0);
        info.p90 = calculatePercentile(values, 90.0);
        info.p95 = calculatePercentile(values, 95.0);
        info.p99 = calculatePercentile(values, 99.0);
        
        return info;
    }

    // Skewness and Kurtosis
    static double calculateSkewness(const std::vector<double>& values) {
        if (values.size() < 3) return 0.0;
        
        double mean = calculateMean(values);
        double stddev = calculateStdDev(values);
        if (stddev < 1e-10) return 0.0;
        
        double sumCubed = 0.0;
        for (double value : values) {
            sumCubed += std::pow(value - mean, 3);
        }
        
        return (values.size() / ((values.size() - 1) * (values.size() - 2))) * 
               (sumCubed / values.size()) / (stddev * stddev * stddev);
    }

    static double calculateKurtosis(const std::vector<double>& values) {
        if (values.size() < 4) return 0.0;
        
        double mean = calculateMean(values);
        double variance = calculateVariance(values);
        if (variance < 1e-10) return 0.0;
        
        double sumFourth = 0.0;
        for (double value : values) {
            sumFourth += std::pow(value - mean, 4);
        }
        
        return (values.size() * (values.size() + 1)) / 
               ((values.size() - 1) * (values.size() - 2) * (values.size() - 3)) *
               (sumFourth / values.size()) / (variance * variance) - 
               3.0 * (values.size() - 1) * (values.size() - 1) / ((values.size() - 2) * (values.size() - 3));
    }

    // Distribution analysis
    static DistributionInfo analyzeDistribution(const std::vector<double>& values) {
        DistributionInfo info;
        
        if (values.empty()) return info;
        
        info.count = values.size();
        info.min = calculateMin(values);
        info.max = calculateMax(values);
        info.mean = calculateMean(values);
        info.median = calculateMedian(values);
        info.variance = calculateVariance(values);
        info.stddev = calculateStdDev(values);
        info.skewness = calculateSkewness(values);
        info.kurtosis = calculateKurtosis(values);
        
        return info;
    }

    // Time series analysis
    static TimeSeriesStats analyzeTimeSeries(const std::vector<double>& values,
                                            std::chrono::duration<double> duration) {
        TimeSeriesStats stats;
        stats.duration = duration;
        
        if (values.empty()) return stats;
        
        stats.name = "time_series";
        stats.distribution = analyzeDistribution(values);
        stats.percentiles = calculatePercentiles(values);
        
        if (duration.count() > 0.0) {
            stats.throughput = values.size() / duration.count();
        }
        
        // Calculate efficiency (normalized between 0 and 1 based on coefficient of variation)
        double coefficientOfVariation = stats.distribution.stddev / (stats.distribution.mean + 1e-10);
        stats.efficiency = 1.0 / (1.0 + coefficientOfVariation);
        
        return stats;
    }

    // Comparative analysis
    static std::vector<std::string> compareDistributions(
        const std::vector<std::vector<double>>& datasets,
        const std::vector<std::string>& names) {
        
        std::vector<std::string> comparisons;
        
        if (datasets.size() < 2) {
            comparisons.push_back("Need at least 2 datasets for comparison");
            return comparisons;
        }
        
        for (size_t i = 0; i < datasets.size(); ++i) {
            comparisons.push_back("=== Dataset: " + names[i] + " ===\n");
            
            auto stats = analyzeDistribution(datasets[i]);
            
            std::stringstream ss;
            ss << "Count: " << stats.count << "\n";
            ss << "Min: " << stats.min << "\n";
            ss << "Max: " << stats.max << "\n";
            ss << "Mean: " << stats.mean << "\n";
            ss << "Median: " << stats.median << "\n";
            ss << "StdDev: " << stats.stddev << "\n";
            ss << "Skewness: " << stats.skewness << "\n";
            ss << "Kurtosis: " << stats.kurtosis << "\n";
            
            comparisons.back() += ss.str();
        
            if (i < datasets.size() - 1) {
                comparisons.back() += "\n--- Comparison with next dataset ---\n";
                auto& currentStats = analyzeDistribution(datasets[i]);
                auto& nextStats = analyzeDistribution(datasets[i + 1]);
                
                double meanDiff = currentStats.mean - nextStats.mean;
                double stddevDiff = currentStats.stddev - nextStats.stddev;
                
                std::stringstream ss;
                ss << "Mean Difference: " << meanDiff << "\n";
                ss << "StdDev Difference: " << stddevDiff << "\n";
                ss << "More Variable: " << (currentStats.stddev > nextStats.stddev ? names[i] : names[i + 1]) << "\n";
                ss << "More Skewed: " << (std::abs(currentStats.skewness) > std::abs(nextStats.skewness) ? names[i] : names[i + 1]) << "\n";
                
                comparisons.back() += ss.str();
            }
        }
        
        return comparisons;
    }

    // Anomaly detection
    static std::vector<size_t> detectOutliers(const std::vector<double>& values,
                                             double threshold = 2.0) {
        std::vector<size_t> outliers;
        
        if (values.size() < 3) return outliers;
        
        double mean = calculateMean(values);
        double stddev = calculateStdDev(values);
        
        if (stddev < 1e-10) return outliers;
        
        for (size_t i = 0; i < values.size(); ++i) {
            double zScore = std::abs(values[i] - mean) / stddev;
            if (zScore > threshold) {
                outliers.push_back(i);
            }
        }
        
        return outliers;
    }

    // Optimization suggestions
    static std::string generateOptimizationSuggestions(const TimeSeriesStats& stats) {
        std::stringstream ss;
        
        ss << "=== Performance Optimization Suggestions ===\n\n";
        
        // Based on coefficient of variation
        double cv = stats.distribution.stddev / (stats.distribution.mean + 1e-10);
        
        if (cv > 1.0) {
            ss << "⚠️  HIGH VARIABILITY DETECTED\n";
            ss << "  Coefficient of Variation: " << cv << "\n";
            ss << "  Suggestion: Consider adding caching or optimization to reduce variance\n\n";
        }
        
        if (stats.efficiency < 0.5) {
            ss << "⚠️  LOW EFFICIENCY DETECTED\n";
            ss << "  Efficiency Score: " << stats.efficiency << "\n";
            ss << "  Suggestion: Optimize resource utilization and reduce overhead\n\n";
        }
        
        if (stats.distribution.skewness > 1.0) {
            ss << "⚠️  POSITIVE SKEWNESS DETECTED\n";
            ss << "  Skewness: " << stats.distribution.skewness << "\n";
            ss << "  Suggestion: Check for long tail outliers that may indicate bottlenecks\n\n";
        }
        
        if (stats.distribution.kurtosis > 3.0) {
            ss << "⚠️  HIGH PEAKED DISTRIBUTION DETECTED\n";
            ss << "  Kurtosis: " << stats.distribution.kurtosis << "\n";
            ss << "  Suggestion: Investigate potential issues with extreme values\n\n";
        }
        
        if (cv < 0.1 && stats.efficiency > 0.8) {
            ss << "✅ STABLE PERFORMANCE DETECTED\n";
            ss << "  Low variance and high efficiency indicate good performance consistency\n\n";
        }
        
        return ss.str();
    }

    // Batch processing
    static std::vector<TimeSeriesStats> analyzeMultipleSeries(
        const std::vector<std::vector<std::pair<double, std::chrono::duration<double>>>>& timeSeries,
        const std::vector<std::string>& names) {
        
        std::vector<TimeSeriesStats> results;
        results.reserve(timeSeries.size());
        
        for (size_t i = 0; i < timeSeries.size(); ++i) {
            TimeSeriesStats stats;
            stats.name = names[i];
            
            std::vector<double> values;
            std::chrono::duration<double> duration = std::chrono::duration<double>(0.0);
            
            for (const auto& point : timeSeries[i]) {
                values.push_back(point.first);
                if (point.second > duration) {
                    duration = point.second;
                }
            }
            
            if (!values.empty()) {
                stats = analyzeTimeSeries(values, duration);
            }
            
            results.push_back(stats);
        }
        
        return results;
    }

    // Validation
    static bool validateData(const std::vector<double>& values) {
        if (values.empty()) {
            return false;
        }
        
        // Check for NaN or infinite values
        for (double value : values) {
            if (std::isnan(value) || std::isinf(value)) {
                return false;
            }
        }
        
        return true;
    }
};

} // namespace nlm
