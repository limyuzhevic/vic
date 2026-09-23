#include "PerformanceAnalyzer.hpp"

namespace nlm {

void PerformanceAnalyzer::initialize(const std::vector<double>& data, const std::vector<double>& timestamps) {
    data_ = data;
    timestamps_ = timestamps.empty() ? 
        std::vector<double>(data.size(), static_cast<double>(data.size())) : timestamps;
    
    if (!data_.empty()) {
        successful_ = true;
    }
}

std::vector<BottleneckAnalysis> PerformanceAnalyzer::analyzeBottlenecks(const std::vector<PerformanceData>& performanceData) const {
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

std::vector<RegressionResult> PerformanceAnalyzer::detectRegressions(const std::vector<PerformanceComparison>& comparisons) const {
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

std::vector<PerformanceComparison> PerformanceAnalyzer::compareConfigurations(
    const std::vector<ConfigurationAnalysis>& configs) const {
    std::vector<PerformanceComparison> comparisons;
    
    for (size_t i = 0; i < configs.size(); ++i) {
        for (size_t j = i + 1; j < configs.size(); ++j) {
            // Create comparison
            PerformanceComparison comparison;
            comparison.testName = "Configuration Comparison";
            comparison.configuration1 = configs[i].configurationName;
            comparison.configuration2 = configs[j].configurationName;
            
            // Calculate performance gain based on overall score
            comparison.performanceGain = configs[j].overallScore - configs[i].overallScore;
            
            // Calculate confidence based on metric score differences
            double totalDiff = 0.0;
            for (const auto& pair : configs[i].metricScores) {
                std::string key = pair.first;
                double value1 = pair.second;
                double value2 = configs[j].metricScores.at(key);
                double diff = std::abs(value1 - value2);
                totalDiff += diff;
            }
            comparison.confidence = std::min(1.0, 1.0 - totalDiff / configs[i].metricScores.size());
            
            comparison.statisticalSignificance = (comparison.confidence > 0.95) ? "High" : 
                                                (comparison.confidence > 0.8) ? "Medium" : "Low";
            
            comparison.timestamp = std::chrono::high_resolution_clock::now();
            comparison.metrics1 = configs[i].metricScores;
            comparison.metrics2 = configs[j].metricScores;
            
            comparisons.push_back(comparison);
        }
    }
    
    return comparisons;
}

WorkloadCharacteristics PerformanceAnalyzer::characterizeWorkload(const std::vector<PerformanceData>& performanceData) const {
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

std::vector<PerformanceTrend> PerformanceAnalyzer::analyzeTrends(const std::vector<PerformanceData>& performanceData) const {
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

std::string PerformanceAnalyzer::generateReport() const {
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
    auto trends = analyzeTrends(std::vector<PerformanceData>{});
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
    
    // Generate bottleneck analysis
    if (!bottleneckAnalysis_.empty()) {
        report << "Bottleneck Analysis:\n";
        for (const auto& bottleneck : bottleneckAnalysis_) {
            report << "  " << bottleneck.name << " (Severity: " << bottleneck.severity << ")\n";
            report << "    Impact: " << bottleneck.impactScore << "\n";
            report << "    Description: " << bottleneck.description << "\n";
            report << "    Recommendation: " << bottleneck.recommendation << "\n\n";
        }
    }
    
    // Generate regression analysis
    if (!regressionAnalysis_.empty()) {
        report << "Regression Analysis:\n";
        for (const auto& regression : regressionAnalysis_) {
            report << "  " << regression.metricName << "\n";
            report << "    Severity: " << regression.severity << "\n";
            report << "    Regression Value: " << regression.regressionValue << "\n";
            report << "    Recommended Action: " << regression.recommendedAction << "\n\n";
        }
    }
    
    return report.str();
}

void PerformanceAnalyzer::calculateStatistics(std::vector<double>& mean, std::vector<double>& variance,
                                             const std::vector<double>& data) const {
    if (data.empty()) return;
    
    mean = std::vector<double>(data.size());
    variance = std::vector<double>(data.size());
    
    for (size_t i = 0; i < data.size(); ++i) {
        mean[i] = data[i];
    }
    
    // Simple moving average
    for (size_t i = 1; i < data.size(); ++i) {
        mean[i] = (mean[i - 1] * 0.9) + (data[i] * 0.1);
    }
    
    // Calculate variance
    for (size_t i = 0; i < data.size(); ++i) {
        variance[i] = 0.0;
        for (size_t j = 0; j < data.size(); ++j) {
            variance[i] += (data[j] - mean[i]) * (data[j] - mean[i]);
        }
        variance[i] /= data.size();
    }
}

void PerformanceAnalyzer::identifyTrends(std::vector<PerformanceTrend>& trends) const {
    // Implementation for trend identification
}

void PerformanceAnalyzer::detectBottlenecksInternal(std::vector<BottleneckAnalysis>& bottlenecks,
                                                   const std::vector<PerformanceData>& performanceData) const {
    // Implementation for bottleneck detection
}

void PerformanceAnalyzer::detectRegressionsInternal(std::vector<RegressionResult>& regressions,
                                                   const std::vector<PerformanceComparison>& comparisons) const {
    // Implementation for regression detection
}

void PerformanceAnalyzer::characterizeWorkloadInternal(WorkloadCharacteristics& characteristics,
                                                      const std::vector<PerformanceData>& performanceData) const {
    // Implementation for workload characterization
}

void PerformanceAnalyzer::analyzeForRegressions(std::vector<RegressionResult>& regressions,
                                               const std::vector<PerformanceComparison>& comparisons) const {
    // Implementation for regression analysis
}

void PerformanceAnalyzer::calculatePerformanceMetrics(std::vector<double>& metrics) const {
    if (data_.empty()) return;
    
    metrics.clear();
    metrics.resize(data_.size());
    
    for (size_t i = 0; i < data_.size(); ++i) {
        metrics[i] = data_[i];
    }
}

void PerformanceAnalyzer::generatePerformanceProfile(std::vector<PerformanceData>& profile) const {
    if (data_.empty()) return;
    
    profile.clear();
    
    for (size_t i = 0; i < data_.size(); ++i) {
        PerformanceData data;
        data.name = "Metric_" + std::to_string(i);
        data.category = "Performance";
        data.cpuTimeMs = data_[i];
        data.wallTimeMs = data_[i] * 1.1;
        data.memoryBytes = static_cast<size_t>(data_[i] * 1024);
        data.peakMemoryBytes = data.memoryBytes * 2;
        data.callCount = 1;
        data.avgTimeMs = data_[i];
        data.minTimeMs = data_[i] * 0.5;
        data.maxTimeMs = data_[i] * 1.5;
        data.stdDevTimeMs = data_[i] * 0.2;
        
        profile.push_back(data);
    }
}

} // namespace nlm
