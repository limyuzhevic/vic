// BenchmarkSuite implementation
// Benchmark suite management implementation

#include "BenchmarkSuite.hpp"
#include <string>
#include <vector>
#include <memory>
#include <mutex>
#include <unordered_map>
#include <algorithm>
#include <numeric>
#include <cmath>

namespace nlm {

BenchmarkSuite::BenchmarkSuite(const std::string& name) {
    suiteName = name;
    autoTeardown = true;
    validateBeforeRun = true;
    successThreshold = 0.95;
    maxRetries = 3;
}

void BenchmarkSuite::addBenchmark(std::unique_ptr<Benchmark> benchmark) {
    std::lock_guard<std::mutex> lock(suiteMutex);
    benchmarks.push_back(std::move(benchmark));
}

void BenchmarkSuite::removeBenchmark(const std::string& name) {
    std::lock_guard<std::mutex> lock(suiteMutex);
    for (auto it = benchmarks.begin(); it != benchmarks.end(); ++it) {
        if ((*it)->getName() == name) {
            benchmarks.erase(it);
            return;
        }
    }
}

bool BenchmarkSuite::hasBenchmark(const std::string& name) const {
    for (const auto& benchmark : benchmarks) {
        if (benchmark->getName() == name) {
            return true;
        }
    }
    return false;
}

void BenchmarkSuite::setAutoTeardown(bool enable) {
    autoTeardown = enable;
}

void BenchmarkSuite::setValidateBeforeRun(bool enable) {
    validateBeforeRun = enable;
}

void BenchmarkSuite::setSuccessThreshold(double threshold) {
    successThreshold = threshold;
}

void BenchmarkSuite::setMaxRetries(size_t retries) {
    maxRetries = retries;
}

bool BenchmarkSuite::getAutoTeardown() const {
    return autoTeardown;
}

bool BenchmarkSuite::getValidateBeforeRun() const {
    return validateBeforeRun;
}

double BenchmarkSuite::getSuccessThreshold() const {
    return successThreshold;
}

size_t BenchmarkSuite::getMaxRetries() const {
    return maxRetries;
}

std::string BenchmarkSuite::getName() const {
    return suiteName;
}

size_t BenchmarkSuite::getBenchmarkCount() const {
    return benchmarks.size();
}

std::vector<std::string> BenchmarkSuite::getBenchmarkNames() const {
    std::vector<std::string> names;
    for (const auto& benchmark : benchmarks) {
        names.push_back(benchmark->getName());
    }
    return names;
}

std::vector<BenchmarkCategory> BenchmarkSuite::getBenchmarkCategories() const {
    std::vector<BenchmarkCategory> categories;
    for (const auto& benchmark : benchmarks) {
        categories.push_back(benchmark->getCategory());
    }
    return categories;
}

BenchmarkCategory BenchmarkSuite::getBenchmarkCategory(const std::string& name) const {
    for (const auto& benchmark : benchmarks) {
        if (benchmark->getName() == name) {
            return benchmark->getCategory();
        }
    }
    return BenchmarkCategory::CPU_PERFORMANCE;
}

BenchmarkResult BenchmarkSuite::runBenchmark(const std::string& name) {
    for (const auto& benchmark : benchmarks) {
        if (benchmark->getName() == name) {
            return runBenchmarkWithRetry(name);
        }
    }
    
    BenchmarkResult result;
    result.benchmarkName = name;
    result.passed = false;
    result.errorMessage = "Benchmark not found";
    return result;
}

BenchmarkResult BenchmarkSuite::runBenchmarkWithRetry(const std::string& name) {
    BenchmarkResult result;
    size_t attempt = 0;
    
    while (attempt < maxRetries) {
        for (const auto& benchmark : benchmarks) {
            if (benchmark->getName() == name) {
                if (validateBeforeRun && !benchmark->validate()) {
                    result.benchmarkName = name;
                    result.passed = false;
                    result.errorMessage = "Benchmark validation failed";
                    return result;
                }
                
                benchmark->setup();
                BenchmarkMetrics metrics = benchmark->run();
                benchmark->teardown();
                
                result.benchmarkName = name;
                result.category = benchmark->getCategory();
                result.metrics.push_back(metrics);
                result.averageExecutionTime = calculateAverageExecutionTime(name);
                result.standardDeviation = calculateStandardDeviation(name);
                result.minimumTime = metrics.executionTime;
                result.maximumTime = metrics.executionTime;
                result.successRate = 1.0;
                result.passed = true;
                
                updateExecutionStats(name, metrics.executionTime, true);
                
                return result;
            }
        }
        
        attempt++;
        std::this_thread::sleep_for(std::chrono::seconds(1)); // Wait before retry
    }
    
    result.benchmarkName = name;
    result.passed = false;
    result.errorMessage = "Maximum retries exceeded";
    return result;
}

std::vector<BenchmarkResult> BenchmarkSuite::runAllBenchmarks() {
    std::vector<BenchmarkResult> results;
    
    for (const auto& benchmark : benchmarks) {
        BenchmarkResult result = runBenchmarkWithRetry(benchmark->getName());
        results.push_back(result);
    }
    
    return results;
}

Benchmark* BenchmarkSuite::getBenchmark(const std::string& name) const {
    for (const auto& benchmark : benchmarks) {
        if (benchmark->getName() == name) {
            return benchmark.get();
        }
    }
    return nullptr;
}

std::string BenchmarkSuite::getBenchmarkDescription(const std::string& name) const {
    for (const auto& benchmark : benchmarks) {
        if (benchmark->getName() == name) {
            return benchmark->getDescription();
        }
    }
    return "";
}

 size_t BenchmarkSuite::getBenchmarkExpectedNeurons(const std::string& name) const {
    for (const auto& benchmark : benchmarks) {
        if (benchmark->getName() == name) {
            return benchmark->getExpectedNeurons();
        }
    }
    return 1000;
}

size_t BenchmarkSuite::getBenchmarkExpectedSteps(const std::string& name) const {
    for (const auto& benchmark : benchmarks) {
        if (benchmark->getName() == name) {
            return benchmark->getExpectedSteps();
        }
    }
    return 1000;
}

void BenchmarkSuite::setBaseline(const std::string& benchmarkName, const BenchmarkMetrics& metrics) {
    std::lock_guard<std::mutex> lock(suiteMutex);
    baselineResults[benchmarkName].push_back(metrics);
}

void BenchmarkSuite::setBaseline(const std::string& benchmarkName, const std::vector<BenchmarkMetrics>& metrics) {
    std::lock_guard<std::mutex> lock(suiteMutex);
    baselineResults[benchmarkName] = metrics;
}

bool BenchmarkSuite::hasBaseline(const std::string& benchmarkName) const {
    std::lock_guard<std::mutex> lock(suiteMutex);
    return baselineResults.find(benchmarkName) != baselineResults.end();
}

std::vector<BenchmarkMetrics> BenchmarkSuite::getBaselineResults(const std::string& benchmarkName) const {
    std::lock_guard<std::mutex> lock(suiteMutex);
    if (baselineResults.find(benchmarkName) != baselineResults.end()) {
        return baselineResults.at(benchmarkName);
    }
    return std::vector<BenchmarkMetrics>();
}

void BenchmarkSuite::clearBaselines() {
    std::lock_guard<std::mutex> lock(suiteMutex);
    baselineResults.clear();
}

BenchmarkComparison BenchmarkSuite::compareWithBaseline(const std::string& benchmarkName, const BenchmarkMetrics& testMetrics) const {
    BenchmarkComparison comparison;
    comparison.baselineBenchmark = benchmarkName;
    comparison.testBenchmark = benchmarkName + "_test";
    comparison.performanceImprovement = calculateImprovement(benchmarkName, comparison.testBenchmark);
    comparison.executionTimeRatio = testMetrics.executionTime / (calculateAverageExecutionTime(benchmarkName) + 0.001);
    comparison.memoryImprovement = static_cast<double>(getAverageMemoryUsage(benchmarkName)) / testMetrics.memoryUsage;
    comparison.isRegressed = detectRegression(benchmarkName, 0.05);
    comparison.confidenceLevel = 0.0; // Placeholder
    comparison.analysis = "Placeholder analysis";
    
    return comparison;
}

std::vector<BenchmarkComparison> BenchmarkSuite::compareAllWithBaseline(const std::string& baselineName) const {
    std::vector<BenchmarkComparison> comparisons;
    
    for (const auto& benchmark : benchmarks) {
        if (benchmark->getName() != baselineName && hasBaseline(benchmark->getName())) {
            // Get test metrics (placeholder)
            BenchmarkMetrics testMetrics;
            testMetrics.executionTime = 1.0;
            testMetrics.memoryUsage = 1024;
            
            BenchmarkComparison comparison = compareWithBaseline(benchmark->getName(), testMetrics);
            comparisons.push_back(comparison);
        }
    }
    
    return comparisons;
}

double BenchmarkSuite::calculateAverageExecutionTime(const std::string& benchmarkName) const {
    std::vector<double> times;
    for (const auto& metrics : baselineResults) {
        if (metrics.first == benchmarkName) {
            for (const auto& metric : metrics.second) {
                times.push_back(metric.executionTime);
            }
            break;
        }
    }
    
    if (times.empty()) return 0.0;
    
    double sum = std::accumulate(times.begin(), times.end(), 0.0);
    return sum / times.size();
}

double BenchmarkSuite::calculateStandardDeviation(const std::string& benchmarkName) const {
    std::vector<double> times;
    for (const auto& metrics : baselineResults) {
        if (metrics.first == benchmarkName) {
            for (const auto& metric : metrics.second) {
                times.push_back(metric.executionTime);
            }
            break;
        }
    }
    
    if (times.size() <= 1) return 0.0;
    
    double mean = calculateAverageExecutionTime(benchmarkName);
    double sumSq = 0.0;
    for (const auto& time : times) {
        sumSq += (time - mean) * (time - mean);
    }
    
    return std::sqrt(sumSq / (times.size() - 1));
}

double BenchmarkSuite::calculateSuccessRate(const std::string& benchmarkName) const {
    return successThreshold;
}

BenchmarkMetrics BenchmarkSuite::calculateAverageMetrics(const std::string& benchmarkName) const {
    BenchmarkMetrics avgMetrics;
    avgMetrics.benchmarkName = benchmarkName;
    avgMetrics.executionTime = calculateAverageExecutionTime(benchmarkName);
    avgMetrics.memoryUsage = getAverageMemoryUsage(benchmarkName);
    return avgMetrics;
}

bool BenchmarkSuite::detectRegression(const std::string& benchmarkName, double threshold) const {
    double improvement = calculateImprovement(benchmarkName, benchmarkName + "_test");
    return improvement < -threshold;
}

double BenchmarkSuite::calculateImprovement(const std::string& baselineName, const std::string& testName) const {
    double baselineTime = calculateAverageExecutionTime(baselineName);
    double testTime = 1.0; // Placeholder
    
    if (baselineTime == 0.0) return 0.0;
    
    return (baselineTime - testTime) / baselineTime;
}

std::vector<BenchmarkResult> BenchmarkSuite::getBenchmarksByCategory(BenchmarkCategory category) const {
    std::vector<BenchmarkResult> results;
    
    for (const auto& benchmark : benchmarks) {
        if (benchmark->getCategory() == category) {
            BenchmarkResult result;
            result.benchmarkName = benchmark->getName();
            result.category = category;
            results.push_back(result);
        }
    }
    
    return results;
}

std::vector<BenchmarkResult> BenchmarkSuite::getBenchmarksByType(const std::string& type) const {
    std::vector<BenchmarkResult> results;
    
    for (const auto& benchmark : benchmarks) {
        std::string desc = benchmark->getDescription();
        if (desc.find(type) != std::string::npos) {
            BenchmarkResult result;
            result.benchmarkName = benchmark->getName();
            result.category = benchmark->getCategory();
            results.push_back(result);
        }
    }
    
    return results;
}

double BenchmarkSuite::getOverallSuccessRate() const {
    return successThreshold;
}

size_t BenchmarkSuite::getTotalExecutionCount() const {
    size_t total = 0;
    for (const auto& [name, count] : executionCounts) {
        total += count;
    }
    return total;
}

std::unordered_map<std::string, size_t> BenchmarkSuite::getExecutionCounts() const {
    return executionCounts;
}

std::unordered_map<std::string, double> BenchmarkSuite::getAverageTimes() const {
    std::unordered_map<std::string, double> times;
    for (const auto& benchmark : benchmarks) {
        times[benchmark->getName()] = calculateAverageExecutionTime(benchmark->getName());
    }
    return times;
}

std::vector<BenchmarkResult> BenchmarkSuite::filterResults(bool passedOnly,
                                                          BenchmarkCategory category,
                                                          const std::string& benchmarkName) const {
    std::vector<BenchmarkResult> results;
    
    for (const auto& benchmark : benchmarks) {
        if (!benchmarkName.empty() && benchmark->getName() != benchmarkName) {
            continue;
        }
        
        if (category != BenchmarkCategory::CPU_PERFORMANCE && benchmark->getCategory() != category) {
            continue;
        }
        
        BenchmarkResult result;
        result.benchmarkName = benchmark->getName();
        result.category = benchmark->getCategory();
        
        if (passedOnly) {
            result.passed = true;
        }
        
        results.push_back(result);
    }
    
    return results;
}

void BenchmarkSuite::updateExecutionStats(const std::string& benchmarkName, double executionTime, bool success) {
    executionCounts[benchmarkName]++;
    averageTimes[benchmarkName] = calculateAverageExecutionTime(benchmarkName);
}

bool BenchmarkSuite::validateAllBenchmarks() const {
    for (const auto& benchmark : benchmarks) {
        if (!benchmark->validate()) {
            return false;
        }
    }
    return true;
}

std::vector<std::string> BenchmarkSuite::getValidationErrors(const std::string& benchmarkName) const {
    return std::vector<std::string>();
}

void BenchmarkSuite::exportResults(const std::string& filename, const std::string& format) const {
    // Implementation for exporting results
}

std::string BenchmarkSuite::formatResults(const std::string& format) const {
    return "Formatted results placeholder";
}

std::string BenchmarkSuite::benchmarkCategoryToString(BenchmarkCategory category) {
    switch (category) {
        case BenchmarkCategory::CPU_PERFORMANCE: return "CPU_PERFORMANCE";
        case BenchmarkCategory::MEMORY_PERFORMANCE: return "MEMORY_PERFORMANCE";
        case BenchmarkCategory::THREAD_PERFORMANCE: return "THREAD_PERFORMANCE";
        case BenchmarkCategory::IO_PERFORMANCE: return "IO_PERFORMANCE";
        case BenchmarkCategory::SIMULATION_PERFORMANCE: return "SIMULATION_PERFORMANCE";
        case BenchmarkCategory::HARDWARE_PERFORMANCE: return "HARDWARE_PERFORMANCE";
        case BenchmarkCategory::SCALABILITY: return "SCALABILITY";
        case BenchmarkCategory::STRESS_TESTING: return "STRESS_TESTING";
        case BenchmarkCategory::LOAD_TESTING: return "LOAD_TESTING";
        case BenchmarkCategory::REGRESSION_TESTING: return "REGRESSION_TESTING";
        default: return "UNKNOWN";
    }
}

BenchmarkCategory BenchmarkSuite::stringToBenchmarkCategory(const std::string& str) {
    if (str == "CPU_PERFORMANCE") return BenchmarkCategory::CPU_PERFORMANCE;
    if (str == "MEMORY_PERFORMANCE") return BenchmarkCategory::MEMORY_PERFORMANCE;
    if (str == "THREAD_PERFORMANCE") return BenchmarkCategory::THREAD_PERFORMANCE;
    if (str == "IO_PERFORMANCE") return BenchmarkCategory::IO_PERFORMANCE;
    if (str == "SIMULATION_PERFORMANCE") return BenchmarkCategory::SIMULATION_PERFORMANCE;
    if (str == "HARDWARE_PERFORMANCE") return BenchmarkCategory::HARDWARE_PERFORMANCE;
    if (str == "SCALABILITY") return BenchmarkCategory::SCALABILITY;
    if (str == "STRESS_TESTING") return BenchmarkCategory::STRESS_TESTING;
    if (str == "LOAD_TESTING") return BenchmarkCategory::LOAD_TESTING;
    if (str == "REGRESSION_TESTING") return BenchmarkCategory::REGRESSION_TESTING;
    return BenchmarkCategory::CPU_PERFORMANCE;
}

size_t BenchmarkSuite::getAverageMemoryUsage(const std::string& benchmarkName) const {
    size_t total = 0;
    size_t count = 0;
    
    for (const auto& metrics : baselineResults) {
        if (metrics.first == benchmarkName) {
            for (const auto& metric : metrics.second) {
                total += metric.memoryUsage;
                count++;
            }
        }
    }
    
    return count > 0 ? total / count : 0;
}

} // namespace nlm