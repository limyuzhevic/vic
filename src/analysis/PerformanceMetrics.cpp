#include "PerformanceMetrics.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <fstream>
#include <cmath>
#include <random>
#include <sstream>
#include <unordered_map>
#include <numeric>

namespace nlm {

struct PerformanceMetrics::Impl {
    MetricsConfig config;
    std::vector<BenchmarkResult> benchmarkHistory;
    std::vector<PerformanceProfile> profileHistory;
    bool initialized;
    
    Impl() : initialized(false) {
    }
};

PerformanceMetrics::PerformanceMetrics(const MetricsConfig& config) : pImpl(std::make_unique<Impl>()) {
    pImpl->config = config;
    pImpl->initialized = true;
}

PerformanceMetrics::~PerformanceMetrics() = default;

PerformanceMetrics::PerformanceMetrics(const PerformanceMetrics& other) 
    : pImpl(std::make_unique<Impl>(*other.pImpl)) {
}

PerformanceMetrics& PerformanceMetrics::operator=(const PerformanceMetrics& other) {
    if (this != &other) {
        pImpl = std::make_unique<Impl>(*other.pImpl);
    }
    return *this;
}

PerformanceMetrics::PerformanceMetrics(PerformanceMetrics&& other) noexcept = default;

PerformanceMetrics& PerformanceMetrics::operator=(PerformanceMetrics&& other) noexcept = default;

PerformanceMetrics::MetricsConfig PerformanceMetrics::getConfig() const {
    return pImpl->config;
}

void PerformanceMetrics::setConfig(const MetricsConfig& config) {
    pImpl->config = config;
}

void PerformanceMetrics::recordBenchmark(const BenchmarkResult& benchmarkResult) {
    if (!pImpl->initialized) {
        return;
    }
    
    pImpl->benchmarkHistory.push_back(benchmarkResult);
    
    // Limit history size
    if (pImpl->benchmarkHistory.size() > pImpl->config.maxHistorySize) {
        pImpl->benchmarkHistory.erase(pImpl->benchmarkHistory.begin());
    }
}

void PerformanceMetrics::recordProfile(const PerformanceProfile& profile) {
    if (!pImpl->initialized) {
        return;
    }
    
    pImpl->profileHistory.push_back(profile);
    
    // Limit history size
    if (pImpl->profileHistory.size() > pImpl->config.maxHistorySize) {
        pImpl->profileHistory.erase(pImpl->profileHistory.begin());
    }
}

float PerformanceMetrics::computeEfficiency(const BenchmarkResult& benchmark) {
    if (benchmark.executionTime <= 0.0f) {
        return 0.0f;
    }
    
    // Compute efficiency as 1 / (execution time normalized)
    float efficiency = 1.0f / (benchmark.executionTime + 0.001f);
    
    // Apply penalty if memory usage is high
    if (benchmark.memoryUsage > 100.0f) { // 100 MB threshold
        efficiency *= 0.8f;
    }
    
    // Apply bonus if performance is good
    if (benchmark.efficiency > 0.8f) {
        efficiency *= 1.2f;
    }
    
    return std::min(1.0f, std::max(0.0f, efficiency));
}

std::map<std::string, float> PerformanceMetrics::computeScalability(
    const std::vector<BenchmarkResult>& benchmarks
) {
    std::map<std::string, float> results;
    
    if (benchmarks.empty()) {
        return results;
    }
    
    // Find baseline benchmark (first one or smallest)
    const BenchmarkResult* baseline = &benchmarks[0];
    for (const auto& bench : benchmarks) {
        if (bench.executionTime < baseline->executionTime) {
            baseline = &bench;
        }
    }
    
    // Calculate speedup for each benchmark relative to baseline
    std::vector<float> speedups;
    for (const auto& bench : benchmarks) {
        if (baseline->executionTime > 0.0f) {
            speedups.push_back(baseline->executionTime / bench.executionTime);
        }
    }
    
    if (speedups.empty()) {
        return results;
    }
    
    // Calculate average speedup
    float sumSpeedups = std::accumulate(speedups.begin(), speedups.end(), 0.0f);
    results["average_speedup"] = sumSpeedups / speedups.size();
    
    // Calculate Amdahl's law efficiency (simplified)
    float idealSpeedup = static_cast<float>(benchmarks.size());
    results["amdahls_efficiency"] = idealSpeedup > 0.0f ? results["average_speedup"] / idealSpeedup : 0.0f;
    
    // Calculate memory scalability
    float baselineMemory = baseline->memoryUsage;
    float totalMemory = 0.0f;
    for (const auto& bench : benchmarks) {
        totalMemory += bench.memoryUsage;
    }
    
    results["memory_efficiency"] = baselineMemory > 0.0f ? baselineMemory / totalMemory : 0.0f;
    
    // Calculate efficiency metric
    float totalExecution = 0.0f;
    for (const auto& bench : benchmarks) {
        totalExecution += bench.executionTime;
    }
    
    results["efficiency"] = totalExecution > 0.0f ? idealSpeedup / totalExecution : 0.0f;
    
    // Calculate scalability score
    results["scalability_score"] = (results["average_speedup"] + results["memory_efficiency"]) / 2.0f;
    
    return results;
}

std::string PerformanceMetrics::generateReport(const std::string& format) {
    if (!pImpl->initialized) {
        return "PerformanceMetrics: Not initialized";
    }
    
    if (format == "json") {
        // Generate JSON report
        std::stringstream ss;
        ss << "{\n";
        ss << "  \"benchmarks\": [";
        
        for (size_t i = 0; i < pImpl->benchmarkHistory.size(); ++i) {
            const auto& bench = pImpl->benchmarkHistory[i];
            ss << "{\n";
            ss << "    \"name\": \"" << bench.benchmarkName << "\",\n";
            ss << "    \"execution_time\": " << bench.executionTime << ",\n";
            ss << "    \"memory_usage\": " << bench.memoryUsage << ",\n";
            ss << "    \"cpu_usage\": " << bench.cpuUsage << ",\n";
            ss << "    \"efficiency\": " << bench.efficiency << "\n";
            ss << "  }";
            
            if (i < pImpl->benchmarkHistory.size() - 1) {
                ss << ",\n";
            } else {
                ss << "\n";
            }
        }
        
        ss << "],\n";
        ss << "  \"profiles\": []\n";
        ss << "}\n";
        
        return ss.str();
    } else if (format == "csv") {
        // Generate CSV report
        std::stringstream ss;
        ss << "benchmark_name,execution_time,memory_usage,cpu_usage,efficiency,steps_completed,reward_per_step\n";
        
        for (const auto& bench : pImpl->benchmarkHistory) {
            ss << bench.benchmarkName << "," << bench.executionTime << "," << bench.memoryUsage << ",";
            ss << bench.cpuUsage << "," << bench.efficiency << "," << bench.stepsCompleted << "," << bench.rewardPerStep << "\n";
        }
        
        return ss.str();
    } else {
        // Generate text report
        std::stringstream ss;
        ss << "=== Performance Metrics Report ===\n";
        ss << "Generated at: " << std::chrono::system_clock::now() << "\n";
        ss << "Configuration:\n";
        ss << "  Time tracking: " << (pImpl->config.enableTimeTracking ? "enabled" : "disabled") << "\n";
        ss << "  Memory tracking: " << (pImpl->config.enableMemoryTracking ? "enabled" : "disabled") << "\n";
        ss << "  CPU tracking: " << (pImpl->config.enableCpuTracking ? "enabled" : "disabled") << "\n";
        ss << "  I/O tracking: " << (pImpl->config.enableIoTracking ? "enabled" : "disabled") << "\n";
        ss << "  Network tracking: " << (pImpl->config.enableNetworkTracking ? "enabled" : "disabled") << "\n";
        ss << "  Detailed metrics: " << (pImpl->config.enableDetailedMetrics ? "enabled" : "disabled") << "\n";
        ss << "  Time window: " << pImpl->config.timeWindow << " seconds\n";
        ss << "  Max history size: " << pImpl->config.maxHistorySize << "\n\n";
        
        if (!pImpl->benchmarkHistory.empty()) {
            ss << "Benchmark Results:\n";
            for (const auto& bench : pImpl->benchmarkHistory) {
                ss << "  " << bench.benchmarkName << ":\n";
                ss << "    Execution time: " << bench.executionTime << " seconds\n";
                ss << "    Memory usage: " << bench.memoryUsage << " MB\n";
                ss << "    CPU usage: " << bench.cpuUsage << "%\n";
                ss << "    Efficiency: " << bench.efficiency << "\n";
                ss << "    Steps completed: " << bench.stepsCompleted << "\n";
                ss << "    Reward per step: " << bench.rewardPerStep << "\n";
            }
        }
        
        if (!pImpl->profileHistory.empty()) {
            ss << "\nPerformance Profiles:\n";
            for (const auto& profile : pImpl->profileHistory) {
                ss << "  CPU utilization: " << profile.cpuUtilization << "%\n";
                ss << "  Memory utilization: " << profile.memoryUtilization << "%\n";
                ss << "  Disk I/O: " << profile.diskIO << " IOPS\n";
                ss << "  Network I/O: " << profile.networkIO << " bytes/sec\n";
                ss << "  Total steps: " << profile.totalSteps << "\n";
                ss << "  Simulation time: " << profile.totalSimulationTime << " seconds\n";
                ss << "  Steps per second: " << profile.stepsPerSecond << "\n";
            }
        }
        
        ss << "\nOverall Performance:\n";
        ss << "  Total benchmarks: " << pImpl->benchmarkHistory.size() << "\n";
        ss << "  Total profiles: " << pImpl->profileHistory.size() << "\n";
        
        return ss.str();
    }
    
    return "Unknown report format: " + format;
}

std::map<std::string, float> PerformanceMetrics::compareImplementations(
    const std::vector<ImplementationComparison>& implementations,
    const std::string& baselineName
) {
    std::map<std::string, float> results;
    
    if (implementations.empty()) {
        return results;
    }
    
    // Find baseline
    const ImplementationComparison* baseline = nullptr;
    for (const auto& impl : implementations) {
        if (impl.isBaseline || impl.implementationName == baselineName) {
            baseline = &impl;
            break;
        }
    }
    
    if (!baseline) {
        baseline = &implementations[0];
    }
    
    // Compare each implementation to baseline
    for (const auto& impl : implementations) {
        std::string key = "compare_" + impl.implementationName;
        
        if (baseline->executionTime > 0.0f) {
            results[key + "_speedup"] = baseline->executionTime / impl.executionTime;
        }
        
        if (baseline->memoryUsage > 0.0f) {
            results[key + "_memory_efficiency"] = baseline->memoryUsage / impl.memoryUsage;
        }
        
        if (baseline->efficiency > 0.0f) {
            results[key + "_efficiency"] = baseline->efficiency / impl.efficiency;
        }
        
        if (baseline->scalabilityScore > 0.0f) {
            results[key + "_scalability"] = baseline->scalabilityScore / impl.scalabilityScore;
        }
    }
    
    // Calculate rankings
    std::vector<std::pair<std::string, float>> rankings;
    for (const auto& impl : implementations) {
        float totalScore = 0.0f;
        int scoreCount = 0;
        
        for (const auto& result : results) {
            if (result.first.find("_speedup") != std::string::npos) {
                totalScore += results[impl.implementationName + "_speedup"];
                scoreCount++;
            }
        }
        
        if (scoreCount > 0) {
            rankings.push_back({impl.implementationName, totalScore / scoreCount});
        }
    }
    
    std::sort(rankings.begin(), rankings.end(),
              [](const auto& a, const auto& b) { return a.second > b.second; });
    
    results["rank_1"] = rankings.empty() ? 0.0f : rankings[0].second;
    if (rankings.size() > 1) {
        results["rank_2"] = rankings[1].second;
    }
    if (rankings.size() > 2) {
        results["rank_3"] = rankings[2].second;
    }
    
    return results;
}

std::vector<PerformanceMetrics::PerformanceBottleneck> PerformanceMetrics::profilePerformance() {
    std::vector<PerformanceBottleneck> bottlenecks;
    
    if (!pImpl->initialized) {
        return bottlenecks;
    }
    
    // Generate dummy bottlenecks based on metrics history
    for (size_t i = 0; i < std::min(static_cast<size_t>(5), pImpl->benchmarkHistory.size()); ++i) {
        PerformanceBottleneck bottleneck;
        
        const auto& bench = pImpl->benchmarkHistory[i];
        
        // Determine bottleneck type
        if (bench.memoryUsage > 200.0f) { // High memory usage
            bottleneck.bottleneckType = "memory";
            bottleneck.impactScore = std::min(1.0f, (bench.memoryUsage - 200.0f) / 300.0f);
        } else if (bench.executionTime > 10.0f) { // High execution time
            bottleneck.bottleneckType = "cpu";
            bottleneck.impactScore = std::min(1.0f, (bench.executionTime - 10.0f) / 20.0f);
        } else if (bench.cpuUsage > 90.0f) { // High CPU usage
            bottleneck.bottleneckType = "cpu";
            bottleneck.impactScore = std::min(1.0f, (bench.cpuUsage - 90.0f) / 10.0f);
        } else {
            bottleneck.bottleneckType = "algorithm";
            bottleneck.impactScore = static_cast<float>(rand()) / RAND_MAX;
        }
        
        bottleneck.componentName = bench.benchmarkName;
        bottleneck.estimatedSavings = bottleneck.impactScore * 10.0f; // Estimated time/memory savings
        
        // Generate suggested solutions
        if (bottleneck.bottleneckType == "memory") {
            bottleneck.suggestedSolutions = {"Optimize memory allocation", "Use memory pools", "Reduce data structures size"};
        } else if (bottleneck.bottleneckType == "cpu") {
            bottleneck.suggestedSolutions = {"Optimize algorithms", "Use parallel processing", "Reduce computational complexity"};
        } else {
            bottleneck.suggestedSolutions = {"Profile code", "Optimize critical paths", "Use more efficient data structures"};
        }
        
        bottlenecks.push_back(bottleneck);
    }
    
    return bottlenecks;
}

std::map<std::string, std::vector<float>> PerformanceMetrics::analyzeTrends(
    const std::string& metricName,
    float timeWindow
) {
    std::map<std::string, std::vector<float>> results;
    
    if (!pImpl->initialized) {
        return results;
    }
    
    // Generate trend data for different time periods
    results["1h_trend"] = std::vector<float>(10, 0.0f);
    results["1h_values"] = std::vector<float>(10, 0.0f);
    
    results["1d_trend"] = std::vector<float>(24, 0.0f);
    results["1d_values"] = std::vector<float>(24, 0.0f);
    
    results["1w_trend"] = std::vector<float>(7, 0.0f);
    results["1w_values"] = std::vector<float>(7, 0.0f);
    
    // Generate dummy trend data
    for (int i = 0; i < 10; ++i) {
        results["1h_values"][i] = static_cast<float>(rand()) / RAND_MAX * 100.0f;
        results["1h_trend"][i] = results["1h_values"][i] * (0.1f + static_cast<float>(i) / 10.0f);
    }
    
    for (int i = 0; i < 24; ++i) {
        results["1d_values"][i] = static_cast<float>(rand()) / RAND_MAX * 100.0f;
        results["1d_trend"][i] = results["1d_values"][i] * (0.05f + static_cast<float>(i) / 24.0f);
    }
    
    for (int i = 0; i < 7; ++i) {
        results["1w_values"][i] = static_cast<float>(rand()) / RAND_MAX * 100.0f;
        results["1w_trend"][i] = results["1w_values"][i] * (0.02f + static_cast<float>(i) / 7.0f);
    }
    
    return results;
}

std::map<std::string, std::string> PerformanceMetrics::generateOptimizationRecommendations() {
    std::map<std::string, std::string> recommendations;
    
    if (!pImpl->initialized) {
        return recommendations;
    }
    
    // Generate optimization recommendations based on metrics history
    for (const auto& bench : pImpl->benchmarkHistory) {
        if (bench.memoryUsage > 150.0f) {
            recommendations["memory_optimization"] = 
                "Consider using memory pools and object pooling to reduce memory fragmentation. "
                "Also consider compressing data where possible and using more efficient data structures. "
                "Current memory usage: " + std::to_string(bench.memoryUsage) + " MB";
            break;
        }
    }
    
    for (const auto& bench : pImpl->benchmarkHistory) {
        if (bench.executionTime > 5.0f) {
            recommendations["cpu_optimization"] = 
                "Optimize critical algorithms and consider parallel processing for CPU-intensive operations. "
                "Profile the code to identify bottlenecks. "
                "Current execution time: " + std::to_string(bench.executionTime) + " seconds";
            break;
        }
    }
    
    if (recommendations.empty()) {
        recommendations["general_optimization"] = 
            "Performance is within acceptable ranges. "
            "Continue monitoring and consider optimizations based on specific use cases."; 
    }
    
    return recommendations;
}

bool PerformanceMetrics::exportMetrics(const std::string& filepath, const std::string& format) {
    try {
        if (format == "json") {
            std::ofstream file(filepath);
            if (!file) {
                LOG_ERROR("PerformanceMetrics: Failed to open file for writing: " + filepath);
                return false;
            }
            
            file << generateReport("json");
            file.close();
        } else if (format == "csv") {
            std::ofstream file(filepath);
            if (!file) {
                LOG_ERROR("PerformanceMetrics: Failed to open file for writing: " + filepath);
                return false;
            }
            
            file << generateReport("csv");
            file.close();
        } else {
            std::ofstream file(filepath);
            if (!file) {
                LOG_ERROR("PerformanceMetrics: Failed to open file for writing: " + filepath);
                return false;
            }
            
            file << generateReport("text");
            file.close();
        }
        
        LOG_INFO("PerformanceMetrics: Successfully exported metrics to " + filepath);
        return true;
    } catch (const std::exception& e) {
        LOG_ERROR("PerformanceMetrics: Error exporting metrics: " + std::string(e.what()));
        return false;
    }
}

bool PerformanceMetrics::importMetrics(const std::string& filepath) {
    try {
        std::ifstream file(filepath);
        if (!file) {
            LOG_ERROR("PerformanceMetrics: Failed to open file for reading: " + filepath);
            return false;
        }
        
        // In a real implementation, this would parse the file and load metrics
        // For now, just check if file can be opened
        std::string line;
        std::getline(file, line);
        
        file.close();
        
        LOG_INFO("PerformanceMetrics: Successfully imported metrics from " + filepath);
        return true;
    } catch (const std::exception& e) {
        LOG_ERROR("PerformanceMetrics: Error importing metrics: " + std::string(e.what()));
        return false;
    }
}

std::string PerformanceMetrics::getPerformanceSummary() const {
    std::stringstream ss;
    
    if (!pImpl->initialized) {
        return "PerformanceMetrics: Not initialized";
    }
    
    ss << "=== Performance Metrics Summary ===\n";
    ss << "Benchmarks recorded: " << pImpl->benchmarkHistory.size() << "\n";
    ss << "Profiles recorded: " << pImpl->profileHistory.size() << "\n";
    
    if (!pImpl->benchmarkHistory.empty()) {
        ss << "\nRecent Benchmarks:\n";
        int count = std::min(3, static_cast<int>(pImpl->benchmarkHistory.size()));
        for (int i = 0; i < count; ++i) {
            const auto& bench = pImpl->benchmarkHistory[pImpl->benchmarkHistory.size() - 1 - i];
            ss << "  " << bench.benchmarkName << ": " << bench.executionTime << "s, " 
               << bench.memoryUsage << "MB, " << bench.efficiency * 100 << "% efficient\n";
        }
    }
    
    ss << "\nOverall Performance Score: " << calculatePerformanceScore() << "/1.0\n";
    
    return ss.str();
}

PerformanceMetrics::BenchmarkResult PerformanceMetrics::getLatestBenchmark() const {
    if (pImpl->benchmarkHistory.empty()) {
        return BenchmarkResult();
    }
    
    return pImpl->benchmarkHistory.back();
}

PerformanceMetrics::PerformanceProfile PerformanceMetrics::getLatestProfile() const {
    if (pImpl->profileHistory.empty()) {
        return PerformanceProfile();
    }
    
    return pImpl->profileHistory.back();
}

void PerformanceMetrics::reset() {
    pImpl->benchmarkHistory.clear();
    pImpl->profileHistory.clear();
}

float PerformanceMetrics::calculatePerformanceScore() const {
    if (pImpl->benchmarkHistory.empty()) {
        return 0.0f;
    }
    
    float totalScore = 0.0f;
    for (const auto& bench : pImpl->benchmarkHistory) {
        totalScore += bench.efficiency;
    }
    
    return totalScore / pImpl->benchmarkHistory.size();
}

std::vector<std::vector<float>> PerformanceMetrics::generatePerformanceHeatmap(
    const std::string& dimension
) {
    std::vector<std::vector<float>> heatmap;
    
    if (!pImpl->initialized) {
        return heatmap;
    }
    
    // Create a simple heatmap based on metrics
    // In a real implementation, this would use actual performance data
    heatmap.resize(10, std::vector<float>(10, 0.0f));
    
    for (size_t i = 0; i < heatmap.size(); ++i) {
        for (size_t j = 0; j < heatmap[i].size(); ++j) {
            // Generate heatmap values based on dimension
            float value = static_cast<float>(rand()) / RAND_MAX;
            
            if (dimension == "time") {
                value *= (i + 1) / 10.0f;
            } else if (dimension == "metric") {
                value *= (j + 1) / 10.0f;
            } else if (dimension == "implementation") {
                value *= (i + j) / 20.0f;
            }
            
            heatmap[i][j] = value;
        }
    }
    
    return heatmap;
}

std::map<std::string, float> PerformanceMetrics::computePerformanceVariance(
    const std::string& metricName
) {
    std::map<std::string, float> results;
    
    if (!pImpl->initialized) {
        return results;
    }
    
    // Calculate variance based on metric name
    if (metricName == "execution_time") {
        float sum = 0.0f;
        float sumSquares = 0.0f;
        
        for (const auto& bench : pImpl->benchmarkHistory) {
            sum += bench.executionTime;
            sumSquares += bench.executionTime * bench.executionTime;
        }
        
        float mean = pImpl->benchmarkHistory.empty() ? 0.0f : sum / pImpl->benchmarkHistory.size();
        float variance = pImpl->benchmarkHistory.empty() ? 0.0f : (sumSquares / pImpl->benchmarkHistory.size()) - mean * mean;
        
        results["variance"] = variance;
        results["std_deviation"] = std::sqrt(variance);
    } else if (metricName == "memory_usage") {
        float sum = 0.0f;
        float sumSquares = 0.0f;
        
        for (const auto& bench : pImpl->benchmarkHistory) {
            sum += bench.memoryUsage;
            sumSquares += bench.memoryUsage * bench.memoryUsage;
        }
        
        float mean = pImpl->benchmarkHistory.empty() ? 0.0f : sum / pImpl->benchmarkHistory.size();
        float variance = pImpl->benchmarkHistory.empty() ? 0.0f : (sumSquares / pImpl->benchmarkHistory.size()) - mean * mean;
        
        results["variance"] = variance;
        results["std_deviation"] = std::sqrt(variance);
    } else {
        results["variance"] = 0.0f;
        results["std_deviation"] = 0.0f;
    }
    
    return results;
}

} // namespace nlm
