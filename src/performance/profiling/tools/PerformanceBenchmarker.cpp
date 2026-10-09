#include "PerformanceBenchmarker.hpp"
#include <chrono>
#include <sstream>
#include <iomanip>

namespace nlm {
namespace profiling {

// PerformanceBenchmarker implementation

void PerformanceBenchmarker::runBenchmark(const std::string& name, std::function<void()> testFunction) {
    runBenchmark(name, testFunction, {});
}

void PerformanceBenchmarker::runBenchmark(const std::string& name, std::function<void()> testFunction,
                                         const std::map<std::string, std::string>& tags) {
    BenchmarkResult result;
    result.testName = name;
    result.success = false;
    result.timestamp = 0.0; // Simplified - could use real time
    
    auto startTime = std::chrono::high_resolution_clock::now();
    try {
        testFunction();
        result.success = true;
        auto endTime = std::chrono::high_resolution_clock::now();
        result.duration = std::chrono::duration<double>(endTime - startTime).count();
    } catch (const std::exception& e) {
        result.errorMessage = e.what();
        auto endTime = std::chrono::high_resolution_clock::now();
        result.duration = std::chrono::duration<double>(endTime - startTime).count();
    }
    
    std::lock_guard<std::mutex> lock(pImpl_->mutex);
    benchmarkResults_.push_back(result);
}

PerformanceBenchmarker::BenchmarkResult PerformanceBenchmarker::getLastResult(const std::string& name) const {
    std::lock_guard<std::mutex> lock(pImpl_->mutex);
    
    auto it = std::find_if(benchmarkResults_.rbegin(), benchmarkResults_.rend(),
                          [&name](const BenchmarkResult& result) { return result.testName == name; });n    
    if (it != benchmarkResults_.rend()) {
        return *it;
    }
    
    return BenchmarkResult{}; // Empty result
}

std::vector<PerformanceBenchmarker::BenchmarkResult> PerformanceBenchmarker::getAllResults(const std::string& name) const {
    std::vector<BenchmarkResult> result;
    std::lock_guard<std::mutex> lock(pImpl_->mutex);
    
    for (const auto& benchmark : benchmarkResults_) {
        if (name.empty() || benchmark.testName == name) {
            result.push_back(benchmark);
        }
    }
    
    return result;
}

template<typename Func>
double PerformanceBenchmarker::measureExecutionTime(Func&& func) {
    auto startTime = std::chrono::high_resolution_clock::now();
    func();
    auto endTime = std::chrono::high_resolution_clock::now();
    return std::chrono::duration<double>(endTime - startTime).count();
}

template<typename Func>
double PerformanceBenchmarker::measureMemoryUsage(Func&& func) {
    // Simplified memory measurement - could use more sophisticated tools
    func();
    return 42.0; // Dummy value
}

template<typename Func>
double PerformanceBenchmarker::measureCpuUsage(Func&& func) {
    // Simplified CPU measurement
    func();
    return 50.0; // Dummy value
}

double PerformanceBenchmarker::getAverageDuration(const std::string& name) const {
    auto results = getAllResults(name);
    if (results.empty()) return 0.0;
    
    double total = 0.0;
    for (const auto& result : results) {
        if (result.success) {
            total += result.duration;
        }
    }
    
    return total / results.size();
}

double PerformanceBenchmarker::getMinDuration(const std::string& name) const {
    auto results = getAllResults(name);
    if (results.empty()) return 0.0;
    
    double min = std::numeric_limits<double>::max();
    for (const auto& result : results) {
        if (result.success && result.duration < min) {
            min = result.duration;
        }
    }
    
    return min;
}

double PerformanceBenchmarker::getMaxDuration(const std::string& name) const {
    auto results = getAllResults(name);
    if (results.empty()) return 0.0;
    
    double max = 0.0;
    for (const auto& result : results) {
        if (result.success && result.duration > max) {
            max = result.duration;
        }
    }
    
    return max;
}

double PerformanceBenchmarker::getStandardDeviation(const std::string& name) const {
    auto results = getAllResults(name);
    if (results.empty()) return 0.0;
    
    double mean = getAverageDuration(name);
    double sumSquared = 0.0;
    for (const auto& result : results) {
        if (result.success) {
            double diff = result.duration - mean;
            sumSquared += diff * diff;
        }
    }
    
    return std::sqrt(sumSquared / results.size());
}

std::vector<std::string> PerformanceBenchmarker::findPerformanceRegressions(
    const std::string& baselineName, const std::string& currentName,
    double threshold) const {
    std::vector<std::string> regressions;
    
    auto baselineResults = getAllResults(baselineName);
    auto currentResults = getAllResults(currentName);
    
    if (baselineResults.empty() || currentResults.empty()) {
        return regressions;
    }
    
    std::map<std::string, double> baselineDurations;
    std::map<std::string, double> currentDurations;
    
    for (const auto& result : baselineResults) {
        baselineDurations[result.testName] = result.duration;
    }
    
    for (const auto& result : currentResults) {
        currentDurations[result.testName] = result.duration;
    }
    
    for (const auto& pair : currentDurations) {
        const std::string& testName = pair.first;
        double currentDuration = pair.second;
        
        auto baselineIt = baselineDurations.find(testName);
        if (baselineIt != baselineDurations.end()) {
            double baselineDuration = baselineIt->second;
            if (baselineDuration > 0.0) {
                double regression = (currentDuration - baselineDuration) / baselineDuration;
                if (regression > threshold) {
                    std::stringstream ss;
                    ss << "Regression in " << testName << ": "
                       << (regression * 100) << "% slower (baseline: "
                       << baselineDuration << "s, current: " << currentDuration << "s)";
                    regressions.push_back(ss.str());
                }
            }
        }
    }
    
    return regressions;
}

bool PerformanceBenchmarker::exportBenchmarkReport(const std::string& filepath) const {
    // TODO: Implement benchmark report export
    return false;
}

bool PerformanceBenchmarker::exportPerformanceTrends(const std::string& filepath) const {
    // TODO: Implement performance trends export
    return false;
}

void PerformanceBenchmarker::integrateWithProfiler(PerformanceProfiler& profiler) {
    // TODO: Integrate with PerformanceProfiler
}

} // namespace profiling
} // namespace nlm
