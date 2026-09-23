#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <chrono>
#include <algorithm>
#include <numeric>
#include <cmath>
#include <map>

namespace nlm {

/**
 * PerformanceAnalyzer - Analyzes performance data and identifies optimization opportunities
 * 
 * Provides comprehensive performance analysis including:
 * - Bottleneck identification
 * - Performance regression detection
 * - Configuration comparison
 * - Workload characterization
 * - Optimization recommendations
 * - Performance trend analysis
 */
class PerformanceAnalyzer {
public:
    /**
     * Performance comparison data
     */
    struct PerformanceComparison {
        std::string testName;
        std::string configuration1;
        std::string configuration2;
        double performanceGain{0.0};
        double confidence{0.0};
        std::string statisticalSignificance;
        std::string testMethodology;
        std::chrono::high_resolution_clock::time_point timestamp;
        std::unordered_map<std::string, double> metrics1;
        std::unordered_map<std::string, double> metrics2;
    };

    /**
     * Regression detection result
     */
    struct RegressionResult {
        std::string metricName;
        std::string severity;
        double regressionValue{0.0};
        double threshold{0.0};
        std::chrono::high_resolution_clock::time_point detectionTime;
        std::string regressionType;
        std::string recommendedAction;
        bool isBlocking{false};
    };

    /**
     * Workload characterization
     */
    struct WorkloadCharacteristics {
        std::string workloadType;
        double cpuIntensity{0.0};
        double memoryIntensity{0.0};
        double ioIntensity{0.0};
        double parallelizability{0.0};
        size_t typicalBatchSize{0};
        double typicalStepTimeMs{0.0};
        size_t peakMemoryUsage{0};
        std::vector<double> performanceProfile;
        std::vector<size_t> memoryProfile;
    };

    /**
     * Configuration analysis
     */
    struct ConfigurationAnalysis {
        std::string configurationName;
        double overallScore{0.0};
        std::unordered_map<std::string, double> metricScores;
        std::vector<std::string> strengths;
        std::vector<std::string> weaknesses;
        std::vector<std::string> optimizationSuggestions;
        double executionTime{0.0};
        size_t memoryUsage{0};
        size_t peakMemoryUsage{0};
    };

    /**
     * Performance trend analysis
     */
    struct PerformanceTrend {
        std::string metricName;
        std::vector<double> values;
        std::vector<double> timestamps;
        double trendCoefficient{0.0};
        std::string trendDirection;
        double averageValue{0.0};
        double stdDeviation{0.0};
        double rateOfChange{0.0};
        std::string analysis;
    };

    /**
     * Initialize analyzer with performance data
     */
    void initialize(const std::vector<double>& data, const std::vector<double>& timestamps = {});

    /**
     * Analyze for bottlenecks
     */
    std::vector<BottleneckAnalysis> analyzeBottlenecks(const std::vector<PerformanceData>& performanceData) const;

    /**
     * Detect performance regressions
     */
    std::vector<RegressionResult> detectRegressions(const std::vector<PerformanceComparison>& comparisons) const;

    /**
     * Compare performance between configurations
     */
    std::vector<PerformanceComparison> compareConfigurations(
        const std::vector<ConfigurationAnalysis>& configs) const;

    /**
     * Characterize workload
     */
    WorkloadCharacteristics characterizeWorkload(const std::vector<PerformanceData>& performanceData) const;

    /**
     * Analyze performance trends
     */
    std::vector<PerformanceTrend> analyzeTrends(const std::vector<PerformanceData>& performanceData) const;

    /**
     * Generate performance report
     */
    std::string generateReport() const;

    /**
     * Get configuration analysis
     */
    ConfigurationAnalysis getConfigurationAnalysis(const std::string& configurationName) const;

    /**
     * Get workload characteristics
     */
    WorkloadCharacteristics getWorkloadCharacteristics() const { return workloadCharacteristics_; }

    /**
     * Get bottleneck analysis
     */
    const std::vector<BottleneckAnalysis>& getBottleneckAnalysis() const { return bottleneckAnalysis_; }

    /**
     * Get regression analysis
     */
    const std::vector<RegressionResult>& getRegressionAnalysis() const { return regressionAnalysis_; }

    /**
     * Check if analysis was successful
     */
    bool isSuccessful() const { return successful_; }

private:
    bool successful_{false};
    std::vector<double> data_;
    std::vector<double> timestamps_;
    std::unordered_map<std::string, ConfigurationAnalysis> configurationAnalysis_;
    std::vector<BottleneckAnalysis> bottleneckAnalysis_;
    std::vector<RegressionResult> regressionAnalysis_;
    WorkloadCharacteristics workloadCharacteristics_;
    std::vector<PerformanceTrend> performanceTrends_;

    // Analysis utilities
    void calculateStatistics(std::vector<double>& mean, std::vector<double>& variance, 
                           const std::vector<double>& data) const;

    void identifyTrends(std::vector<PerformanceTrend>& trends) const;

    void detectBottlenecksInternal(std::vector<BottleneckAnalysis>& bottlenecks,
                                   const std::vector<PerformanceData>& performanceData) const;

    void detectRegressionsInternal(std::vector<RegressionResult>& regressions,
                                   const std::vector<PerformanceComparison>& comparisons) const;

    void characterizeWorkloadInternal(WorkloadCharacteristics& characteristics,
                                      const std::vector<PerformanceData>& performanceData) const;

    double calculateTrend(const std::vector<double>& values) const;

    std::string determineTrendDirection(double coefficient) const;

    double calculateCorrelation(const std::vector<double>& x, const std::vector<double>& y) const;

    double calculateLinearRegression(const std::vector<double>& x, const std::vector<double>& y,
                                    double& intercept, double& slope) const;

    bool detectRegression(double currentValue, double previousValue, 
                        double threshold, const std::string& regressionType) const;
};

/**
 * PerformanceAnalyzer implementation
 */
struct PerformanceAnalyzer {
private:
    bool successful_;
    std::vector<double> data_;
    std::vector<double> timestamps_;
    std::unordered_map<std::string, ConfigurationAnalysis> configurationAnalysis_;
    std::vector<BottleneckAnalysis> bottleneckAnalysis_;
    std::vector<RegressionResult> regressionAnalysis_;
    WorkloadCharacteristics workloadCharacteristics_;
    std::vector<PerformanceTrend> performanceTrends_;

    // Helper classes for performance analysis
    struct PerformanceStatistics {
        double mean{0.0};
        double variance{0.0};
        double stdDeviation{0.0};
        double min{0.0};
        double max{0.0};
        double range{0.0};
        size_t count{0};
        
        PerformanceStatistics() = default;
        
        void calculate(const std::vector<double>& values) {
            if (values.empty()) return;
            
            mean = std::accumulate(values.begin(), values.end(), 0.0) / values.size();
            
            double sumSquares = 0.0;
            for (double value : values) {
                sumSquares += (value - mean) * (value - mean);
            }
            variance = sumSquares / values.size();
            stdDeviation = std::sqrt(variance);
            min = *std::min_element(values.begin(), values.end());
            max = *std::max_element(values.begin(), values.end());
            range = max - min;
            count = values.size();
        }
    };

    struct TrendAnalysis {
        double coefficient{0.0};
        std::string direction;
        std::vector<double> predictedValues;
        std::vector<double> residuals;
        double rSquared{0.0};
        
        TrendAnalysis() = default;
        
        void calculate(const std::vector<double>& x, const std::vector<double>& y) {
            double intercept = 0.0;
            double slope = 0.0;
            calculateLinearRegression(x, y, intercept, slope);
            coefficient = slope;
            direction = determineTrendDirection(coefficient);
            
            // Calculate predicted values and R-squared
            predictedValues.clear();
            residuals.clear();
            double ssTotal = 0.0, ssResidual = 0.0, yMean = std::accumulate(y.begin(), y.end(), 0.0) / y.size();
            
            for (size_t i = 0; i < x.size(); ++i) {
                double predicted = intercept + slope * x[i];
                predictedValues.push_back(predicted);
                residuals.push_back(y[i] - predicted);
                ssTotal += (y[i] - yMean) * (y[i] - yMean);
                ssResidual += residuals.back() * residuals.back();
            }
            
            if (ssTotal > 0) {
                rSquared = 1.0 - (ssResidual / ssTotal);
            }
        }
    };

    struct ConfigurationComparator {
        double calculateSimilarity(const ConfigurationAnalysis& config1, const ConfigurationAnalysis& config2) const {
            double totalDifference = 0.0;
            double maxDifference = 0.0;
            
            for (const auto& pair : config1.metricScores) {
                std::string key = pair.first;
                double value1 = pair.second;
                double value2 = config2.metricScores.at(key);
                double difference = std::abs(value1 - value2);
                totalDifference += difference;
                maxDifference = std::max(maxDifference, difference);
            }
            
            // Normalize to [0, 1]
            double similarity = 1.0 - (totalDifference / (config1.metricScores.size() * maxDifference + 1e-10));
            return std::max(0.0, similarity);
        }
        
        PerformanceComparison compare(const ConfigurationAnalysis& config1, const ConfigurationAnalysis& config2) const {
            PerformanceComparison comparison;
            comparison.configuration1 = config1.configurationName;
            comparison.configuration2 = config2.configurationName;
            
            // Calculate performance gain based on overall score
            comparison.performanceGain = config2.overallScore - config1.overallScore;
            
            // Calculate confidence based on metric score differences
            double totalDiff = 0.0;
            for (const auto& pair : config1.metricScores) {
                std::string key = pair.first;
                double diff = std::abs(config1.metricScores.at(key) - config2.metricScores.at(key));
                totalDiff += diff;
            }
            comparison.confidence = std::min(1.0, 1.0 - totalDiff / config1.metricScores.size());
            
            comparison.statisticalSignificance = (comparison.confidence > 0.95) ? "High" : 
                                                (comparison.confidence > 0.8) ? "Medium" : "Low";
            
            comparison.timestamp = std::chrono::high_resolution_clock::now();
            comparison.metrics1 = config1.metricScores;
            comparison.metrics2 = config2.metricScores;
            
            return comparison;
        }
    };

    struct BottleneckDetector {
        std::vector<BottleneckAnalysis> analyze(const std::vector<PerformanceData>& performanceData) const {
            std::vector<BottleneckAnalysis> bottlenecks;
            
            for (const auto& data : performanceData) {
                // High CPU bottleneck
                if (data.cpuTimeMs > 100.0) {
                    BottleneckAnalysis bottleneck;
                    bottleneck.category = data.category;
                    bottleneck.name = "High CPU Time";
                    bottleneck.impactScore = data.cpuTimeMs / 1000.0;
                    bottleneck.severity = std::min(data.cpuTimeMs / 100.0, 10.0);
                    bottleneck.description = "Component " + data.name + " has high CPU time: " + 
                                           std::to_string(data.cpuTimeMs) + "ms";
                    bottleneck.recommendation = "Consider optimizing CPU-intensive operations or parallelizing";
                    bottleneck.affectedComponents = 1;
                    bottleneck.timeLostMs = data.cpuTimeMs - data.avgTimeMs;
                    bottlenecks.push_back(bottleneck);
                }
                
                // High memory bottleneck
                if (data.peakMemoryBytes > 100 * 1024 * 1024) {
                    BottleneckAnalysis bottleneck;
                    bottleneck.category = data.category;
                    bottleneck.name = "High Memory Usage";
                    bottleneck.impactScore = static_cast<double>(data.peakMemoryBytes) / (1024 * 1024 * 1024);
                    bottleneck.severity = std::min(data.peakMemoryBytes / (10 * 1024 * 1024), 10.0);
                    bottleneck.description = "Component " + data.name + " has high memory usage: " + 
                                           std::to_string(data.peakMemoryBytes / (1024 * 1024)) + " MB";
                    bottleneck.recommendation = "Consider memory-efficient data structures or streaming";
                    bottleneck.affectedComponents = 1;
                    bottleneck.timeLostMs = data.cpuTimeMs;
                    bottlenecks.push_back(bottleneck);
                }
                
                // Low performance bottleneck
                if (data.avgTimeMs > 50.0) {
                    BottleneckAnalysis bottleneck;
                    bottleneck.category = data.category;
                    bottleneck.name = "Slow Performance";
                    bottleneck.impactScore = data.avgTimeMs / 1000.0;
                    bottleneck.severity = std::min(data.avgTimeMs / 50.0, 10.0);
                    bottleneck.description = "Component " + data.name + " has slow average performance: " + 
                                           std::to_string(data.avgTimeMs) + "ms";
                    bottleneck.recommendation = "Consider algorithmic improvements or caching";
                    bottleneck.affectedComponents = 1;
                    bottleneck.timeLostMs = data.avgTimeMs;
                    bottlenecks.push_back(bottleneck);
                }
            }
            
            return bottlenecks;
        }
    };

    struct RegressionDetector {
        std::vector<RegressionResult> detect(const std::vector<PerformanceComparison>& comparisons) const {
            std::vector<RegressionResult> regressions;
            
            for (const auto& comparison : comparisons) {
                for (const auto& pair : comparison.metrics1) {
                    std::string metricName = pair.first;
                    double oldValue = pair.second;
                    double newValue = comparison.metrics2.at(metricName);
                    
                    // Detect performance regression (increase in execution time)
                    if (metricName == "cpu_time" || metricName == "execution_time" || 
                        metricName == "wall_time") {
                        double regression = newValue - oldValue;
                        if (regression > 0.1 * oldValue) { // > 10% regression
                            RegressionResult regression;
                            regression.metricName = metricName;
                            regression.severity = (regression > 0.5 * oldValue) ? "High" : 
                                                (regression > 0.2 * oldValue) ? "Medium" : "Low";
                            regression.regressionValue = regression;
                            regression.threshold = 0.1 * oldValue;
                            regression.detectionTime = comparison.timestamp;
                            regression.regressionType = "Performance Degradation";
                            regression.recommendedAction = "Investigate and fix performance regression";
                            regression.isBlocking = (regression.severity == "High");
                            regressions.push_back(regression);
                        }
                    }
                }
            }
            
            return regressions;
        }
    };

    struct WorkloadCharacterizer {
        WorkloadCharacteristics characterize(const std::vector<PerformanceData>& performanceData) const {
            WorkloadCharacteristics characteristics;
            
            if (performanceData.empty()) return characteristics;
            
            // Calculate intensities
            double totalCpuTime = 0.0;
            double totalMemory = 0.0;
            double totalSteps = 0.0;
            
            for (const auto& data : performanceData) {
                totalCpuTime += data.cpuTimeMs;
                totalMemory += data.peakMemoryBytes;
                totalSteps += data.callCount;
            }
            
            size_t numDataPoints = performanceData.size();
            characteristics.cpuIntensity = totalCpuTime / numDataPoints / 1000.0; // Normalize
            characteristics.memoryIntensity = totalMemory / numDataPoints / (1024 * 1024); // MB
            
            // Calculate step time variability
            std::vector<double> stepTimes;
            for (const auto& data : performanceData) {
                stepTimes.push_back(data.avgTimeMs);
            }
            
            double meanStepTime = std::accumulate(stepTimes.begin(), stepTimes.end(), 0.0) / stepTimes.size();
            double variance = 0.0;
            for (double time : stepTimes) {
                variance += (time - meanStepTime) * (time - meanStepTime);
            }
            variance /= stepTimes.size();
            
            characteristics.cpuUtilizationPercent = std::min(100.0, meanStepTime * 10.0);
            characteristics.parallelizability = 1.0 - (std::sqrt(variance) / (meanStepTime + 1e-10));
            
            characteristics.typicalBatchSize = static_cast<size_t>(meanStepTime);
            characteristics.typicalStepTimeMs = meanStepTime;
            characteristics.peakMemoryUsage = static_cast<size_t>(totalMemory);
            
            // Generate performance profile
            for (const auto& data : performanceData) {
                characteristics.performanceProfile.push_back(data.avgTimeMs);
                characteristics.memoryProfile.push_back(data.peakMemoryBytes);
            }
            
            // Determine workload type based on characteristics
            if (characteristics.cpuIntensity > 5.0 && characteristics.memoryIntensity > 50.0) {
                characteristics.workloadType = "CPU and Memory Intensive";
            } else if (characteristics.cpuIntensity > 5.0) {
                characteristics.workloadType = "CPU Intensive";
            } else if (characteristics.memoryIntensity > 50.0) {
                characteristics.workloadType = "Memory Intensive";
            } else {
                characteristics.workloadType = "Balanced";
            }
            
            return characteristics;
        }
    };

    struct TrendAnalyzer {
        std::vector<PerformanceTrend> analyze(const std::vector<PerformanceData>& performanceData) const {
            std::vector<PerformanceTrend> trends;
            
            // Group data by category and name for trend analysis
            std::unordered_map<std::string, std::vector<double>> categoryTrends;
            std::unordered_map<std::string, std::vector<double>> nameTrends;
            
            for (const auto& data : performanceData) {
                std::string categoryKey = data.category.empty() ? "unknown" : data.category;
                std::string nameKey = data.name.empty() ? "unknown" : data.name;
                
                categoryTrends[categoryKey].push_back(data.avgTimeMs);
                nameTrends[nameKey].push_back(data.avgTimeMs);
            }
            
            // Analyze trends for categories
            for (const auto& pair : categoryTrends) {
                PerformanceTrend trend;
                trend.metricName = pair.first;
                trend.values = pair.second;
                trend.timestamps = std::vector<double>(trend.values.size(), static_cast<double>(trend.values.size()));
                trend.coefficient = calculateTrend(trend.values);
                trend.direction = determineTrendDirection(trend.coefficient);
                
                double sum = std::accumulate(trend.values.begin(), trend.values.end(), 0.0);
                trend.averageValue = sum / trend.values.size();
                
                double variance = 0.0;
                for (double value : trend.values) {
                    variance += (value - trend.averageValue) * (value - trend.averageValue);
                }
                variance /= trend.values.size();
                trend.stdDeviation = std::sqrt(variance);
                
                trend.rateOfChange = trend.coefficient;
                trend.analysis = "Trend analysis for " + trend.metricName + ": " + trend.direction +
                                " (coefficient: " + std::to_string(trend.coefficient) + ")";
                
                trends.push_back(trend);
            }
            
            return trends;
        }
        
        double calculateTrend(const std::vector<double>& values) const {
            if (values.size() < 2) return 0.0;
            
            std::vector<double> x;
            for (size_t i = 0; i < values.size(); ++i) {
                x.push_back(static_cast<double>(i));
            }
            
            double intercept = 0.0, slope = 0.0;
            calculateLinearRegression(x, values, intercept, slope);
            return slope;
        }
        
        std::string determineTrendDirection(double coefficient) const {
            if (std::abs(coefficient) < 0.01) return "Stable";
            return coefficient > 0.0 ? "Increasing" : "Decreasing";
        }
        
        double calculateCorrelation(const std::vector<double>& x, const std::vector<double>& y) const {
            if (x.size() != y.size() || x.size() < 2) return 0.0;
            
            double xMean = std::accumulate(x.begin(), x.end(), 0.0) / x.size();
            double yMean = std::accumulate(y.begin(), y.end(), 0.0) / y.size();
            
            double numerator = 0.0, xDenom = 0.0, yDenom = 0.0;
            for (size_t i = 0; i < x.size(); ++i) {
                numerator += (x[i] - xMean) * (y[i] - yMean);
                xDenom += (x[i] - xMean) * (x[i] - xMean);
                yDenom += (y[i] - yMean) * (y[i] - yMean);
            }
            
            if (xDenom == 0.0 || yDenom == 0.0) return 0.0;
            
            return numerator / std::sqrt(xDenom * yDenom);
        }
        
        double calculateLinearRegression(const std::vector<double>& x, const std::vector<double>& y,
                                        double& intercept, double& slope) const {
            if (x.size() != y.size() || x.size() < 2) return 0.0;
            
            double xMean = std::accumulate(x.begin(), x.end(), 0.0) / x.size();
            double yMean = std::accumulate(y.begin(), y.end(), 0.0) / y.size();
            
            double numerator = 0.0, denominator = 0.0;
            for (size_t i = 0; i < x.size(); ++i) {
                numerator += (x[i] - xMean) * (y[i] - yMean);
                denominator += (x[i] - xMean) * (x[i] - xMean);
            }
            
            if (denominator == 0.0) return 0.0;
            
            slope = numerator / denominator;
            intercept = yMean - slope * xMean;
            
            return calculateCorrelation(x, y);
        }
        
        bool detectRegression(double currentValue, double previousValue, 
                            double threshold, const std::string& regressionType) const {
            return (regressionType == "performance" && currentValue > previousValue * (1.0 + threshold)) ||
                   (regressionType == "memory" && currentValue > previousValue * (1.0 + threshold)) ||
                   (regressionType == "cpu" && currentValue > previousValue * (1.0 + threshold));
        }
    };

public:
    PerformanceAnalyzer() : successful_(false) {}

    void initialize(const std::vector<double>& data, const std::vector<double>& timestamps) {
        data_ = data;
        timestamps_ = timestamps.empty() ? 
            std::vector<double>(data.size(), static_cast<double>(data.size())) : timestamps;
        
        if (!data_.empty()) {
            successful_ = true;
        }
    }

    std::vector<BottleneckAnalysis> analyzeBottlenecks(const std::vector<PerformanceData>& performanceData) const {
        if (!successful_) return {};
        
        BottleneckDetector detector;
        return detector.analyze(performanceData);
    }

    std::vector<RegressionResult> detectRegressions(const std::vector<PerformanceComparison>& comparisons) const {
        if (!successful_) return {};
        
        RegressionDetector detector;
        return detector.detect(comparisons);
    }

    std::vector<PerformanceComparison> compareConfigurations(
        const std::vector<ConfigurationAnalysis>& configs) const {
        if (!successful_) return {};
        
        ConfigurationComparator comparator;
        std::vector<PerformanceComparison> comparisons;
        
        for (size_t i = 0; i < configs.size(); ++i) {
            for (size_t j = i + 1; j < configs.size(); ++j) {
                comparisons.push_back(comparator.compare(configs[i], configs[j]));
            }
        }
        
        return comparisons;
    }

    WorkloadCharacteristics characterizeWorkload(const std::vector<PerformanceData>& performanceData) const {
        if (!successful_) return WorkloadCharacteristics{};
        
        WorkloadCharacterizer characterizer;
        return characterizer.characterize(performanceData);
    }

    std::vector<PerformanceTrend> analyzeTrends(const std::vector<PerformanceData>& performanceData) const {
        if (!successful_) return {};
        
        TrendAnalyzer analyzer;
        return analyzer.analyze(performanceData);
    }

    std::string generateReport() const {
        std::ostringstream report;
        
        report << "=== Performance Analysis Report ===\n";
        report << "Generated: " << std::chrono::system_clock::now() << "\n\n";
        
        if (data_.empty()) {
            report << "No data available for analysis.\n";
            return report.str();
        }
        
        report << "Data Statistics:\n";
        report << "  Data Points: " << data_.size() << "\n";
        report << "  Average: " << std::accumulate(data_.begin(), data_.end(), 0.0) / data_.size() << "\n";
        report << "  Min: " << *std::min_element(data_.begin(), data_.end()) << "\n";
        report << "  Max: " << *std::max_element(data_.begin(), data_.end()) << "\n\n";
        
        // Generate trend analysis
        TrendAnalyzer trendAnalyzer;
        auto trends = trendAnalyzer.analyze(std::vector<PerformanceData>{});
        for (const auto& trend : trends) {
            report << "Trend Analysis: " << trend.metricName << "\n";
            report << "  Direction: " << trend.direction << "\n";
            report << "  Rate of Change: " << trend.rateOfChange << "\n\n";
        }
        
        // Generate configuration analysis
        report << "Configuration Analysis:\n";
        for (const auto& pair : configurationAnalysis_) {
            report << "  " << pair.first << ":\n";
            report << "    Overall Score: " << pair.second.overallScore << "\n";
            report << "    Execution Time: " << pair.second.executionTime << " ms\n";
            report << "    Memory Usage: " << pair.second.memoryUsage << " bytes\n\n";
        }
        
        return report.str();
    }

    ConfigurationAnalysis getConfigurationAnalysis(const std::string& configurationName) const {
        auto it = configurationAnalysis_.find(configurationName);
        if (it != configurationAnalysis_.end()) {
            return it->second;
        }
        return ConfigurationAnalysis{};
    }

    void addConfigurationAnalysis(const ConfigurationAnalysis& analysis) {
        configurationAnalysis_[analysis.configurationName] = analysis;
    }

    void addBottleneckAnalysis(const BottleneckAnalysis& bottleneck) {
        bottleneckAnalysis_.push_back(bottleneck);
    }

    void addRegressionAnalysis(const RegressionResult& regression) {
        regressionAnalysis_.push_back(regression);
    }

    void setWorkloadCharacteristics(const WorkloadCharacteristics& characteristics) {
        workloadCharacteristics_ = characteristics;
    }

    void addPerformanceTrend(const PerformanceTrend& trend) {
        performanceTrends_.push_back(trend);
    }
};

} // namespace nlm
