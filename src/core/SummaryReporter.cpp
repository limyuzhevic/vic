// Implementation of SummaryReporter for NLM

#include "SummaryReporter.hpp"
#include <algorithm>
#include <numeric>
#include <cmath>
#include <ctime>

namespace nlm {

struct SummaryReporter::Impl {
    bool useColors;
    bool includeStatistics;
    bool includeRawData;
    bool includeBrainData;
    bool includePerformanceData;
    bool includeExperimentData;
    ExportConfig exportConfig;
    
    std::vector<ExperimentResultSummary> experimentResults;
    std::vector<PerformanceSummary> performanceResults;
    std::vector<StatisticalAnalysis> statisticalAnalyses;
};

SummaryReporter::SummaryReporter() : pImpl(std::make_unique<Impl>()) {
    pImpl->useColors = true;
    pImpl->includeStatistics = true;
    pImpl->includeRawData = false;
    pImpl->includeBrainData = false;
    pImpl->includePerformanceData = false;
    pImpl->includeExperimentData = false;
}

SummaryReporter::~SummaryReporter() {
}

void SummaryReporter::setUseColors(bool useColors) {
    pImpl->useColors = useColors;
}

void SummaryReporter::setIncludeStatistics(bool includeStatistics) {
    pImpl->includeStatistics = includeStatistics;
}

void SummaryReporter::setIncludeRawData(bool includeRawData) {
    pImpl->includeRawData = includeRawData;
}

void SummaryReporter::setExportConfig(const ExportConfig& config) {
    pImpl->exportConfig = config;
}

void SummaryReporter::setIncludeBrainData(bool include) {
    pImpl->includeBrainData = include;
}

void SummaryReporter::setIncludePerformanceData(bool include) {
    pImpl->includePerformanceData = include;
}

void SummaryReporter::setIncludeExperimentData(bool include) {
    pImpl->includeExperimentData = include;
}

std::string SummaryReporter::generateExperimentSummary(const ExperimentResultSummary& result) const {
    std::stringstream ss;
    
    ss << "=== Experiment Summary ===" << std::endl;
    ss << "Experiment Name: " << result.experimentName << std::endl;
    ss << "Experiment Type: " << result.experimentType << std::endl;
    ss << "Status: " << result.status << std::endl;
    ss << "Start Time: " << formatTimestamp(result.startTime) << std::endl;
    ss << "End Time: " << formatTimestamp(result.endTime) << std::endl;
    ss << "Duration: " << formatDuration(result.executionTime) << std::endl;
    ss << "Success: " << (result.success ? "Yes" : "No") << std::endl;
    
    if (pImpl->includeRawData) {
        ss << std::endl << "Raw Data:" << std::endl;
        ss << "  Execution Time: " << result.executionTime << " s" << std::endl;
        ss << "  Memory Usage: " << formatBytes(result.memoryUsage) << std::endl;
        ss << "  Peak Memory: " << formatBytes(result.peakMemoryUsage) << std::endl;
        ss << "  Throughput: " << result.throughput << " ops/sec" << std::endl;
        ss << "  Efficiency: " << formatPercentage(result.efficiency) << std::endl;
        ss << "  Operations: " << result.operationsCompleted << std::endl;
    }
    
    if (!result.metrics.empty()) {
        ss << std::endl << "Metrics:" << std::endl;
        for (const auto& metric : result.metrics) {
            ss << "  " << metric.first << ": " << formatValue(metric.second, pImpl->exportConfig.decimalFormat) << std::endl;
        }
    }
    
    if (!result.notes.empty()) {
        ss << std::endl << "Notes: " << result.notes << std::endl;
    }
    
    return ss.str();
}

std::string SummaryReporter::generateExperimentComparison(const ComparisonResult& comparison) const {
    std::stringstream ss;
    
    ss << "=== Experiment Comparison ===" << std::endl;
    ss << "Comparison Name: " << comparison.comparisonName << std::endl;
    ss << "Generated: " << formatTimestamp(comparison.timestamp) << std::endl;
    ss << std::endl;
    
    if (comparison.runs.empty()) {
        ss << "No experiments to compare." << std::endl;
        return ss.str();
    }
    
    ss << "Experiments Compared: " << comparison.runs.size() << std::endl;
    ss << std::endl;
    
    // Display each experiment result
    for (size_t i = 0; i < comparison.runs.size(); ++i) {
        ss << "Experiment " << i + 1 << ": " << comparison.runs[i].experimentName << std::endl;
        ss << "  Status: " << comparison.runs[i].status << std::endl;
        ss << "  Duration: " << formatDuration(comparison.runs[i].executionTime) << std::endl;
        ss << "  Success: " << (comparison.runs[i].success ? "Yes" : "No") << std::endl;
        ss << "  Efficiency: " << formatPercentage(comparison.runs[i].efficiency) << std::endl;
        
        if (i < comparison.runs.size() - 1) {
            ss << std::endl;
        }
    }
    
    // Display improvements and degradations
    if (!comparison.improvements.empty()) {
        ss << std::endl << "Improvements:" << std::endl;
        for (const auto& improvement : comparison.improvements) {
            ss << "  " << improvement.first << ": " << formatValue(improvement.second, pImpl->exportConfig.decimalFormat) << std::endl;
        }
    }
    
    if (!comparison.degradations.empty()) {
        ss << std::endl << "Degradations:" << std::endl;
        for (const auto& degradation : comparison.degradations) {
            ss << "  " << degradation.first << ": " << formatValue(degradation.second, pImpl->exportConfig.decimalFormat) << std::endl;
        }
    }
    
    return ss.str();
}

std::string SummaryReporter::generateExperimentStatistics(const StatisticalAnalysis& stats) const {
    std::stringstream ss;
    
    ss << "=== Statistical Analysis ===" << std::endl;
    ss << "Analysis Type: " << stats.analysisType << std::endl;
    ss << "Timestamp: " << formatTimestamp(stats.timestamp) << std::endl;
    ss << std::endl;
    
    ss << "Basic Statistics:" << std::endl;
    ss << "  Count: " << stats.data.size() << std::endl;
    ss << "  Mean: " << formatValue(stats.mean, pImpl->exportConfig.decimalFormat) << std::endl;
    ss << "  Median: " << formatValue(stats.median, pImpl->exportConfig.decimalFormat) << std::endl;
    ss << "  Mode: " << formatValue(stats.mode, pImpl->exportConfig.decimalFormat) << std::endl;
    ss << "  Standard Deviation: " << formatValue(stats.standardDeviation, pImpl->exportConfig.decimalFormat) << std::endl;
    ss << "  Variance: " << formatValue(stats.variance, pImpl->exportConfig.decimalFormat) << std::endl;
    ss << std::endl;
    
    ss << "Range:" << std::endl;
    ss << "  Minimum: " << formatValue(stats.minValue, pImpl->exportConfig.decimalFormat) << std::endl;
    ss << "  Maximum: " << formatValue(stats.maxValue, pImpl->exportConfig.decimalFormat) << std::endl;
    ss << "  Range: " << formatValue(stats.range, pImpl->exportConfig.decimalFormat) << std::endl;
    ss << std::endl;
    
    ss << "Distribution:" << std::endl;
    ss << "  Skewness: " << formatValue(stats.skewness, pImpl->exportConfig.decimalFormat) << std::endl;
    ss << "  Kurtosis: " << formatValue(stats.kurtosis, pImpl->exportConfig.decimalFormat) << std::endl;
    
    return ss.str();
}

std::string SummaryReporter::generatePerformanceSummary(const PerformanceSummary& summary) const {
    std::stringstream ss;
    
    ss << "=== Performance Summary ===" << std::endl;
    ss << "Benchmark Name: " << summary.benchmarkName << std::endl;
    ss << "Timestamp: " << formatTimestamp(summary.timestamp) << std::endl;
    ss << std::endl;
    
    ss << "Performance Metrics:" << std::endl;
    ss << "  Average Execution Time: " << formatDuration(summary.avgExecutionTime) << std::endl;
    ss << "  Average CPU Usage: " << formatPercentage(summary.avgCPUUsage) << std::endl;
    ss << "  Average Memory Usage: " << formatBytes(static_cast<size_t>(summary.avgMemoryUsage * 1024 * 1024)) << std::endl;
    ss << "  Average Throughput: " << summary.avgThroughput << " ops/sec" << std::endl;
    ss << "  Average Latency: " << formatDuration(summary.avgLatency / 1000.0) << std::endl;
    ss << "  Reliability: " << formatPercentage(summary.reliability) << std::endl;
    ss << std::endl;
    
    if (pImpl->includeStatistics) {
        ss << "Statistical Information:" << std::endl;
        ss << "  Execution Time Std Dev: " << formatDuration(summary.stdDevExecutionTime) << std::endl;
        ss << "  Memory Usage Std Dev: " << formatBytes(static_cast<size_t>(summary.stdDevMemoryUsage * 1024 * 1024)) << std::endl;
        ss << std::endl;
    }
    
    if (pImpl->includeRawData && !summary.executionTimes.empty()) {
        ss << "Raw Data (first 10 values):" << std::endl;
        size_t count = std::min<size_t>(10, summary.executionTimes.size());
        for (size_t i = 0; i < count; ++i) {
            ss << "  " << i + 1 << ": " << formatDuration(summary.executionTimes[i]) << std::endl;
        }
        ss << "  ..." << std::endl;
    }
    
    return ss.str();
}

std::string SummaryReporter::generatePerformanceReport(const std::vector<PerformanceSummary>& summaries) const {
    std::stringstream ss;
    
    ss << "=== Performance Report ===" << std::endl;
    ss << "Generated: " << formatTimestamp(std::chrono::system_clock::now()) << std::endl;
    ss << std::endl;
    
    if (summaries.empty()) {
        ss << "No performance data available." << std::endl;
        return ss.str();
    }
    
    ss << "Benchmarks Executed: " << summaries.size() << std::endl;
    ss << std::endl;
    
    // Aggregate statistics
    double totalExecutionTime = 0.0;
    double totalCPUUsage = 0.0;
    double totalMemoryUsage = 0.0;
    double totalThroughput = 0.0;
    
    for (const auto& summary : summaries) {
        totalExecutionTime += summary.avgExecutionTime;
        totalCPUUsage += summary.avgCPUUsage;
        totalMemoryUsage += summary.avgMemoryUsage;
        totalThroughput += summary.avgThroughput;
    }
    
    size_t count = summaries.size();
    
    ss << "Aggregate Performance:" << std::endl;
    ss << "  Average Execution Time: " << formatDuration(totalExecutionTime / count) << std::endl;
    ss << "  Average CPU Usage: " << formatPercentage(totalCPUUsage / count) << std::endl;
    ss << "  Average Memory Usage: " << formatBytes(static_cast<size_t>(totalMemoryUsage / count * 1024 * 1024)) << std::endl;
    ss << "  Average Throughput: " << totalThroughput / count << " ops/sec" << std::endl;
    ss << std::endl;
    
    // Individual benchmarks
    ss << "Individual Benchmarks:" << std::endl;
    for (const auto& summary : summaries) {
        ss << "  " << summary.benchmarkName << ": "
           << formatDuration(summary.avgExecutionTime) << " / "
           << formatPercentage(summary.avgCPUUsage) << " / "
           << formatBytes(static_cast<size_t>(summary.avgMemoryUsage * 1024 * 1024))
           << " / " << summary.avgThroughput << " ops/sec" << std::endl;
    }
    
    return ss.str();
}

std::string SummaryReporter::generateStatisticalAnalysisReport(const StatisticalAnalysis& analysis) const {
    std::stringstream ss;
    
    ss << "=== Statistical Analysis Report ===" << std::endl;
    ss << "Analysis Type: " << analysis.analysisType << std::endl;
    ss << "Timestamp: " << formatTimestamp(analysis.timestamp) << std::endl;
    ss << std::endl;
    
    ss << "Descriptive Statistics:" << std::endl;
    ss << "  Number of Data Points: " << analysis.data.size() << std::endl;
    ss << "  Mean: " << formatValue(analysis.mean, pImpl->exportConfig.decimalFormat) << std::endl;
    ss << "  Median: " << formatValue(analysis.median, pImpl->exportConfig.decimalFormat) << std::endl;
    ss << "  Mode: " << formatValue(analysis.mode, pImpl->exportConfig.decimalFormat) << std::endl;
    ss << "  Standard Deviation: " << formatValue(analysis.standardDeviation, pImpl->exportConfig.decimalFormat) << std::endl;
    ss << "  Variance: " << formatValue(analysis.variance, pImpl->exportConfig.decimalFormat) << std::endl;
    ss << "  Minimum: " << formatValue(analysis.minValue, pImpl->exportConfig.decimalFormat) << std::endl;
    ss << "  Maximum: " << formatValue(analysis.maxValue, pImpl->exportConfig.decimalFormat) << std::endl;
    ss << "  Range: " << formatValue(analysis.range, pImpl->exportConfig.decimalFormat) << std::endl;
    ss << "  Skewness: " << formatValue(analysis.skewness, pImpl->exportConfig.decimalFormat) << std::endl;
    ss << "  Kurtosis: " << formatValue(analysis.kurtosis, pImpl->exportConfig.decimalFormat) << std::endl;
    
    return ss.str();
}

std::string SummaryReporter::generateStatisticalAnalysisSummary(const std::vector<StatisticalAnalysis>& analyses) const {
    std::stringstream ss;
    
    ss << "=== Statistical Analysis Summary ===" << std::endl;
    ss << "Generated: " << formatTimestamp(std::chrono::system_clock::now()) << std::endl;
    ss << std::endl;
    
    if (analyses.empty()) {
        ss << "No statistical analyses available." << std::endl;
        return ss.str();
    }
    
    ss << "Analyses Performed: " << analyses.size() << std::endl;
    ss << std::endl;
    
    // Summary by type
    std::map<std::string, size_t> analysisTypeCounts;
    for (const auto& analysis : analyses) {
        analysisTypeCounts[analysis.analysisType]++;
    }
    
    ss << "Analyses by Type:" << std::endl;
    for (const auto& pair : analysisTypeCounts) {
        ss << "  " << pair.first << ": " << pair.second << std::endl;
    }
    
    // Overall statistics
    double totalDataPoints = 0.0;
    double totalMean = 0.0;
    for (const auto& analysis : analyses) {
        totalDataPoints += analysis.data.size();
        totalMean += analysis.mean;
    }
    
    if (!analyses.empty()) {
        totalMean /= analyses.size();
    }
    
    ss << std::endl << "Overall Statistics:" << std::endl;
    ss << "  Total Data Points: " << static_cast<size_t>(totalDataPoints) << std::endl;
    ss << "  Average Mean: " << formatValue(totalMean, pImpl->exportConfig.decimalFormat) << std::endl;
    
    return ss.str();
}

std::string SummaryReporter::generateComparisonReport(const ComparisonResult& comparison) const {
    std::stringstream ss;
    
    ss << "=== Comparison Report ===" << std::endl;
    ss << "Comparison Name: " << comparison.comparisonName << std::endl;
    ss << "Generated: " << formatTimestamp(comparison.timestamp) << std::endl;
    ss << std::endl;
    
    if (comparison.runs.empty()) {
        ss << "No comparisons available." << std::endl;
        return ss.str();
    }
    
    ss << "Number of Runs: " << comparison.runs.size() << std::endl;
    ss << std::endl;
    
    // Display comparisons for each statistic
    for (const auto& pair : comparison.statistics) {
        const auto& analysis = pair.second;
        ss << "Statistic: " << pair.first << std::endl;
        ss << "  Mean: " << formatValue(analysis.mean, pImpl->exportConfig.decimalFormat) << std::endl;
        ss << "  Standard Deviation: " << formatValue(analysis.standardDeviation, pImpl->exportConfig.decimalFormat) << std::endl;
        ss << "  Minimum: " << formatValue(analysis.minValue, pImpl->exportConfig.decimalFormat) << std::endl;
        ss << "  Maximum: " << formatValue(analysis.maxValue, pImpl->exportConfig.decimalFormat) << std::endl;
        ss << std::endl;
    }
    
    return ss.str();
}

std::string SummaryReporter::generatePerformanceComparison(const std::vector<PerformanceSummary>& summaries) const {
    std::stringstream ss;
    
    ss << "=== Performance Comparison ===" << std::endl;
    ss << "Generated: " << formatTimestamp(std::chrono::system_clock::now()) << std::endl;
    ss << std::endl;
    
    if (summaries.size() < 2) {
        ss << "Need at least 2 benchmarks for comparison." << std::endl;
        return ss.str();
    }
    
    ss << "Benchmarks Compared: " << summaries.size() << std::endl;
    ss << std::endl;
    
    // Sort by execution time
    auto sortedSummaries = summaries;
    std::sort(sortedSummaries.begin(), sortedSummaries.end(),
              [](const PerformanceSummary& a, const PerformanceSummary& b) {
                  return a.avgExecutionTime < b.avgExecutionTime;
              });
    
    ss << "Performance Ranking (by execution time):" << std::endl;
    for (size_t i = 0; i < sortedSummaries.size(); ++i) {
        ss << "  " << i + 1 << ". " << sortedSummaries[i].benchmarkName 
           << ": " << formatDuration(sortedSummaries[i].avgExecutionTime);
        
        if (i == 0) {
            ss << " (Best)";
        } else if (i == sortedSummaries.size() - 1) {
            ss << " (Worst)";
        }
        
        ss << std::endl;
    }
    
    return ss.str();
}

std::string SummaryReporter::generateComprehensiveReport(const std::vector<ExperimentResultSummary>& experiments,
                                                         const std::vector<PerformanceSummary>& performances,
                                                         const std::vector<StatisticalAnalysis>& statistics) const {
    std::stringstream ss;
    
    ss << "=== Comprehensive NLM Report ===" << std::endl;
    ss << "Generated: " << formatTimestamp(std::chrono::system_clock::now()) << std::endl;
    ss << std::endl;
    
    ss << "Executive Summary:" << std::endl;
    ss << "  Total Experiments: " << experiments.size() << std::endl;
    ss << "  Total Performance Benchmarks: " << performances.size() << std::endl;
    ss << "  Total Statistical Analyses: " << statistics.size() << std::endl;
    ss << std::endl;
    
    // Experiment summary
    if (!experiments.empty()) {
        ss << "=== Experiment Summary ===" << std::endl;
        
        size_t successfulExperiments = 0;
        double totalExperimentTime = 0.0;
        
        for (const auto& experiment : experiments) {
            if (experiment.success) {
                successfulExperiments++;
            }
            totalExperimentTime += experiment.executionTime;
        }
        
        ss << "Success Rate: " << (successfulExperiments * 100.0 / experiments.size()) << "%" << std::endl;
        ss << "Average Execution Time: " << formatDuration(totalExperimentTime / experiments.size()) << std::endl;
        ss << std::endl;
        
        // Display top 5 experiments by efficiency
        auto sortedExperiments = experiments;
        std::sort(sortedExperiments.begin(), sortedExperiments.end(),
                  [](const ExperimentResultSummary& a, const ExperimentResultSummary& b) {
                      return a.efficiency > b.efficiency;
                  });
        
        ss << "Top 5 Experiments by Efficiency:" << std::endl;
        size_t count = std::min<size_t>(5, experiments.size());
        for (size_t i = 0; i < count; ++i) {
            ss << "  " << i + 1 << ". " << sortedExperiments[i].experimentName 
               << ": " << formatPercentage(sortedExperiments[i].efficiency);
            
            if (sortedExperiments[i].success) {
                ss << " (Success)";
            } else {
                ss << " (Failed)";
            }
            
            ss << std::endl;
        }
        
        ss << std::endl;
    }
    
    // Performance summary
    if (!performances.empty()) {
        ss << "=== Performance Summary ===" << std::endl;
        
        double totalThroughput = 0.0;
        double totalMemoryUsage = 0.0;
        
        for (const auto& performance : performances) {
            totalThroughput += performance.avgThroughput;
            totalMemoryUsage += performance.avgMemoryUsage;
        }
        
        ss << "Average Throughput: " << totalThroughput / performances.size() << " ops/sec" << std::endl;
        ss << "Average Memory Usage: " << formatBytes(static_cast<size_t>(totalMemoryUsage / performances.size() * 1024 * 1024)) << std::endl;
        ss << std::endl;
    }
    
    // Statistical analysis summary
    if (!statistics.empty()) {
        ss << "=== Statistical Analysis Summary ===" << std::endl;
        ss << "Number of Analyses: " << statistics.size() << std::endl;
        
        // Display analysis types
        std::map<std::string, size_t> analysisTypeCounts;
        for (const auto& analysis : statistics) {
            analysisTypeCounts[analysis.analysisType]++;
        }
        
        ss << "Analyses by Type:" << std::endl;
        for (const auto& pair : analysisTypeCounts) {
            ss << "  " << pair.first << ": " << pair.second << std::endl;
        }
        
        ss << std::endl;
    }
    
    return ss.str();
}

StatisticalAnalysis SummaryReporter::calculateStatistics(const std::vector<double>& data) const {
    StatisticalAnalysis analysis;
    
    if (data.empty()) {
        return analysis;
    }
    
    analysis.data = data;
    analysis.analysisType = "Descriptive Statistics";
    analysis.timestamp = std::chrono::system_clock::now();
    
    // Calculate basic statistics
    analysis.mean = calculateMean(data);
    analysis.median = calculateMedian(data);
    analysis.mode = calculateMode(data);
    analysis.variance = calculateVariance(data, analysis.mean);
    analysis.standardDeviation = std::sqrt(analysis.variance);
    analysis.minValue = calculateMin(data);
    analysis.maxValue = calculateMax(data);
    analysis.range = calculateRange(data);
    analysis.skewness = calculateSkewness(data, analysis.mean, analysis.standardDeviation);
    analysis.kurtosis = calculateKurtosis(data, analysis.mean, analysis.standardDeviation);
    
    return analysis;
}

std::string SummaryReporter::formatStatisticalResults(const StatisticalAnalysis& stats) const {
    std::stringstream ss;
    
    ss << "Statistics:" << std::endl;
    ss << "  Mean: " << formatValue(stats.mean, pImpl->exportConfig.decimalFormat) << std::endl;
    ss << "  Median: " << formatValue(stats.median, pImpl->exportConfig.decimalFormat) << std::endl;
    ss << "  Mode: " << formatValue(stats.mode, pImpl->exportConfig.decimalFormat) << std::endl;
    ss << "  Std Dev: " << formatValue(stats.standardDeviation, pImpl->exportConfig.decimalFormat) << std::endl;
    ss << "  Variance: " << formatValue(stats.variance, pImpl->exportConfig.decimalFormat) << std::endl;
    ss << "  Range: " << formatValue(stats.range, pImpl->exportConfig.decimalFormat) << std::endl;
    
    return ss.str();
}

bool SummaryReporter::exportExperimentSummary(const std::string& filepath, const ExperimentResultSummary& result,
                                               const std::string& format) const {
    std::ofstream file(filepath);
    if (!file) {
        return false;
    }
    
    if (format == "json") {
        // JSON export (simplified)
        file << "{\n";
        file << "  \"experimentName\": \"" << result.experimentName << "\"," << std::endl;
        file << "  \"experimentType\": \"" << result.experimentType << "\"," << std::endl;
        file << "  \"success\": " << (result.success ? "true" : "false") << "," << std::endl;
        file << "  \"executionTime\": " << result.executionTime << "," << std::endl;
        file << "  \"memoryUsage\": " << result.memoryUsage << "," << std::endl;
        file << "  \"throughput\": " << result.throughput << "\n";
        file << "}" << std::endl;
    } else {
        // Text format
        file << generateExperimentSummary(result);
    }
    
    return true;
}

bool SummaryReporter::exportPerformanceSummary(const std::string& filepath, const PerformanceSummary& summary,
                                                const std::string& format) const {
    std::ofstream file(filepath);
    if (!file) {
        return false;
    }
    
    if (format == "json") {
        // JSON export (simplified)
        file << "{\n";
        file << "  \"benchmarkName\": \"" << summary.benchmarkName << "\"," << std::endl;
        file << "  \"avgExecutionTime\": " << summary.avgExecutionTime << "," << std::endl;
        file << "  \"avgCPUUsage\": " << summary.avgCPUUsage << "," << std::endl;
        file << "  \"avgMemoryUsage\": " << summary.avgMemoryUsage << "," << std::endl;
        file << "  \"avgThroughput\": " << summary.avgThroughput << "\n";
        file << "}" << std::endl;
    } else {
        // Text format
        file << generatePerformanceSummary(summary);
    }
    
    return true;
}

bool SummaryReporter::exportStatisticalAnalysis(const std::string& filepath, const StatisticalAnalysis& analysis,
                                                 const std::string& format) const {
    std::ofstream file(filepath);
    if (!file) {
        return false;
    }
    
    if (format == "json") {
        // JSON export (simplified)
        file << "{\n";
        file << "  \"analysisType\": \"" << analysis.analysisType << "\"," << std::endl;
        file << "  \"mean\": " << analysis.mean << "," << std::endl;
        file << "  \"median\": " << analysis.median << "," << std::endl;
        file << "  \"standardDeviation\": " << analysis.standardDeviation << "," << std::endl;
        file << "  \"minValue\": " << analysis.minValue << "," << std::endl;
        file << "  \"maxValue\": " << analysis.maxValue << "\n";
        file << "}" << std::endl;
    } else {
        // Text format
        file << generateStatisticalAnalysisReport(analysis);
    }
    
    return true;
}

bool SummaryReporter::exportComparisonReport(const std::string& filepath, const ComparisonResult& comparison,
                                             const std::string& format) const {
    std::ofstream file(filepath);
    if (!file) {
        return false;
    }
    
    if (format == "json") {
        // JSON export (simplified)
        file << "{\n";
        file << "  \"comparisonName\": \"" << comparison.comparisonName << "\"," << std::endl;
        file << "  \"runs\": [" << std::endl;
        for (size_t i = 0; i < comparison.runs.size(); ++i) {
            file << "    {\"experimentName\": \"" << comparison.runs[i].experimentName << "\"";
            if (i < comparison.runs.size() - 1) {
                file << "," << std::endl;
            } else {
                file << std::endl;
            }
        }
        file << "  ]," << std::endl;
        file << "  \"statistics\": [" << std::endl;
        for (size_t i = 0; i < comparison.statistics.size(); ++i) {
            file << "    {\"name\": \"" << comparison.statistics[i].first << "\"";
            if (i < comparison.statistics.size() - 1) {
                file << "," << std::endl;
            } else {
                file << std::endl;
            }
        }
        file << "  ]\n";
        file << "}" << std::endl;
    } else {
        // Text format
        file << generateComparisonReport(comparison);
    }
    
    return true;
}

std::string SummaryReporter::formatExperimentResult(const ExperimentResultSummary& result) const {
    std::stringstream ss;
    
    ss << result.experimentName << ": "
       << (result.success ? "SUCCESS" : "FAILED") << " - "
       << formatDuration(result.executionTime) << " - "
       << formatBytes(result.memoryUsage) << " - "
       << result.throughput << " ops/sec";
    
    return ss.str();
}

std::string SummaryReporter::formatPerformanceResult(const PerformanceSummary& summary) const {
    std::stringstream ss;
    
    ss << summary.benchmarkName << ": "
       << formatDuration(summary.avgExecutionTime) << " - "
       << formatPercentage(summary.avgCPUUsage) << " - "
       << formatBytes(static_cast<size_t>(summary.avgMemoryUsage * 1024 * 1024)) << " - "
       << summary.avgThroughput << " ops/sec";
    
    return ss.str();
}

std::string SummaryReporter::formatAnalysisResult(const StatisticalAnalysis& analysis) const {
    std::stringstream ss;
    
    ss << analysis.analysisType << ": "
       << formatValue(analysis.mean, pImpl->exportConfig.decimalFormat) << " ± "
       << formatValue(analysis.standardDeviation, pImpl->exportConfig.decimalFormat) << " (n="
       << analysis.data.size() << ")";
    
    return ss.str();
}

void SummaryReporter::addExperimentResult(const ExperimentResultSummary& result) {
    pImpl->experimentResults.push_back(result);
}

void SummaryReporter::addPerformanceResult(const PerformanceSummary& result) {
    pImpl->performanceResults.push_back(result);
}

void SummaryReporter::addStatisticalAnalysis(const StatisticalAnalysis& analysis) {
    pImpl->statisticalAnalyses.push_back(analysis);
}

void SummaryReporter::clearExperimentResults() {
    pImpl->experimentResults.clear();
}

void SummaryReporter::clearPerformanceResults() {
    pImpl->performanceResults.clear();
}

void SummaryReporter::clearStatisticalAnalyses() {
    pImpl->statisticalAnalyses.clear();
}

std::string SummaryReporter::getExperimentReportTemplate() {
    return "Experiment Name: {name}\n"
           "Experiment Type: {type}\n"
           "Status: {status}\n"
           "Duration: {duration}\n"
           "Success: {success}\n"
           "Efficiency: {efficiency}%\n"
           "Throughput: {throughput} ops/sec\n";
}

std::string SummaryReporter::getPerformanceReportTemplate() {
    return "Benchmark: {benchmark}\n"
           "Execution Time: {time}\n"
           "CPU Usage: {cpu_usage}%\n"
           "Memory Usage: {memory_usage}\n"
           "Throughput: {throughput} ops/sec\n"
           "Latency: {latency}\n";
}

std::string SummaryReporter::getAnalysisReportTemplate() {
    return "Analysis Type: {type}\n"
           "Data Points: {count}\n"
           "Mean: {mean}\n"
           "Std Dev: {std_dev}\n"
           "Min: {min}\n"
           "Max: {max}\n"
           "Range: {range}\n";
}

std::string SummaryReporter::getComparisonReportTemplate() {
    return "Comparison: {name}\n"
           "Runs: {run_count}\n"
           "Best: {best_name} ({best_value})\n"
           "Worst: {worst_name} ({worst_value})\n"
           "Average: {avg_value}\n";
}

std::string SummaryReporter::formatTimestamp(std::chrono::system_clock::time_point timestamp) const {
    auto time = std::chrono::system_clock::to_time_t(timestamp);
    std::stringstream ss;
    ss << std::put_time(std::localtime(&time), pImpl->exportConfig.timeFormat.c_str());
    return ss.str();
}

std::string SummaryReporter::formatDuration(double seconds) const {
    std::stringstream ss;
    
    auto hours = static_cast<int>(seconds / 3600);
    seconds = seconds - hours * 3600;
    auto minutes = static_cast<int>(seconds / 60);
    seconds = seconds - minutes * 60;
    
    ss << hours << "h " << minutes << "m " << std::fixed << std::setprecision(1) << seconds << "s";
    
    return ss.str();
}

std::string SummaryReporter::formatBytes(size_t bytes) const {
    static const std::string units[] = {"B", "KB", "MB", "GB", "TB"};
    double value = static_cast<double>(bytes);
    int unitIndex = 0;
    
    while (value >= 1024.0 && unitIndex < 4) {
        value /= 1024.0;
        unitIndex++;
    }
    
    std::stringstream ss;
    ss << std::fixed << std::setprecision(1) << value << " " << units[unitIndex];
    
    return ss.str();
}

std::string SummaryReporter::formatPercentage(double value) const {
    std::stringstream ss;
    ss << std::fixed << std::setprecision(1) << value << "%";
    return ss.str();
}

std::string SummaryReporter::formatValue(double value, const std::string& format) const {
    std::stringstream ss;
    ss << std::fixed << std::setprecision(1) << value;
    return ss.str();
}

double SummaryReporter::calculateMean(const std::vector<double>& data) const {
    if (data.empty()) return 0.0;
    
    double sum = std::accumulate(data.begin(), data.end(), 0.0);
    return sum / data.size();
}

double SummaryReporter::calculateMedian(std::vector<double> data) const {
    if (data.empty()) return 0.0;
    
    std::sort(data.begin(), data.end());
    size_t n = data.size();
    
    if (n % 2 == 0) {
        return (data[n/2 - 1] + data[n/2]) / 2.0;
    } else {
        return data[n/2];
    }
}

double SummaryReporter::calculateMode(const std::vector<double>& data) const {
    if (data.empty()) return 0.0;
    
    std::map<double, size_t> frequency;
    for (double value : data) {
        frequency[value]++;
    }
    
    double mode = data[0];
    size_t maxFrequency = 0;
    
    for (const auto& pair : frequency) {
        if (pair.second > maxFrequency) {
            mode = pair.first;
            maxFrequency = pair.second;
        }
    }
    
    return mode;
}

double SummaryReporter::calculateVariance(const std::vector<double>& data, double mean) const {
    if (data.empty()) return 0.0;
    
    double sum = 0.0;
    for (double value : data) {
        sum += (value - mean) * (value - mean);
    }
    
    return sum / data.size();
}

double SummaryReporter::calculateStandardDeviation(const std::vector<double>& data, double mean) const {
    return std::sqrt(calculateVariance(data, mean));
}

double SummaryReporter::calculateMin(const std::vector<double>& data) const {
    if (data.empty()) return 0.0;
    
    return *std::min_element(data.begin(), data.end());
}

double SummaryReporter::calculateMax(const std::vector<double>& data) const {
    if (data.empty()) return 0.0;
    
    return *std::max_element(data.begin(), data.end());
}

double SummaryReporter::calculateRange(const std::vector<double>& data) const {
    if (data.empty()) return 0.0;
    
    return calculateMax(data) - calculateMin(data);
}

double SummaryReporter::calculateSkewness(const std::vector<double>& data, double mean, double stdDev) const {
    if (data.empty() || stdDev == 0.0) return 0.0;
    
    double n = static_cast<double>(data.size());
    double sum = 0.0;
    
    for (double value : data) {
        sum += std::pow((value - mean) / stdDev, 3);
    }
    
    return sum / n;
}

double SummaryReporter::calculateKurtosis(const std::vector<double>& data, double mean, double stdDev) const {
    if (data.empty() || stdDev == 0.0) return 0.0;
    
    double n = static_cast<double>(data.size());
    double sum = 0.0;
    
    for (double value : data) {
        sum += std::pow((value - mean) / stdDev, 4);
    }
    
    return sum / n - 3.0;
}

} // namespace nlm
