// BenchmarkRunner implementation
// Benchmark execution framework implementation

#include "BenchmarkRunner.hpp"
#include <chrono>
#include <vector>
#include <memory>
#include <map>
#include <mutex>
#include <atomic>
#include <algorithm>
#include <numeric>

namespace nlm {

BenchmarkRunner::BenchmarkRunner(const std::string& name) {
    runnerName = name;
    running = false;
    timeout = 300.0; // Default 5 minutes
    maxMemory = 1024 * 1024 * 1024; // Default 1GB
    parallelExecution = false;
    threadCount = 1;
}

void BenchmarkRunner::addSuite(std::unique_ptr<BenchmarkSuite> suite) {
    std::lock_guard<std::mutex> lock(runnerMutex);
    suites.push_back(std::move(suite));
}

void BenchmarkRunner::removeSuite(const std::string& name) {
    std::lock_guard<std::mutex> lock(runnerMutex);
    for (auto it = suites.begin(); it != suites.end(); ++it) {
        if ((*it)->getName() == name) {
            suites.erase(it);
            return;
        }
    }
}

bool BenchmarkRunner::hasSuite(const std::string& name) const {
    for (const auto& suite : suites) {
        if (suite->getName() == name) {
            return true;
        }
    }
    return false;
}

std::string BenchmarkRunner::getName() const {
    return runnerName;
}

size_t BenchmarkRunner::getSuiteCount() const {
    return suites.size();
}

std::vector<std::string> BenchmarkRunner::getSuiteNames() const {
    std::vector<std::string> names;
    for (const auto& suite : suites) {
        names.push_back(suite->getName());
    }
    return names;
}

BenchmarkResult BenchmarkRunner::runBenchmark(const std::string& suiteName, const std::string& benchmarkName) {
    BenchmarkResult result;
    
    for (const auto& suite : suites) {
        if (suite->getName() == suiteName) {
            if (suite->hasBenchmark(benchmarkName)) {
                auto benchmark = suite->getBenchmark(benchmarkName);
                if (benchmark && benchmark->validate()) {
                    benchmark->setup();
                    auto startTime = std::chrono::high_resolution_clock::now();
                    BenchmarkMetrics metrics = benchmark->run();
                    auto endTime = std::chrono::high_resolution_clock::now();
                    benchmark->teardown();
                    
                    result.benchmarkName = benchmarkName;
                    result.category = benchmark->getCategory();
                    result.metrics.push_back(metrics);
                    result.averageExecutionTime = metrics.executionTime;
                    result.standardDeviation = 0.0; // Placeholder
                    result.minimumTime = metrics.executionTime;
                    result.maximumTime = metrics.executionTime;
                    result.successRate = 1.0;
                    result.passed = true;
                    
                    return result;
                }
            }
        }
    }
    
    result.passed = false;
    result.errorMessage = "Benchmark not found or invalid";
    return result;
}

std::vector<BenchmarkResult> BenchmarkRunner::runSuite(const std::string& suiteName) {
    std::vector<BenchmarkResult> results;
    
    for (const auto& suite : suites) {
        if (suite->getName() == suiteName) {
            std::vector<BenchmarkResult> suiteResults = suite->runAllBenchmarks();
            results.insert(results.end(), suiteResults.begin(), suiteResults.end());
        }
    }
    
    return results;
}

std::vector<BenchmarkResult> BenchmarkRunner::runAllBenchmarks() {
    std::vector<BenchmarkResult> allResults;
    
    for (const auto& suite : suites) {
        std::vector<BenchmarkResult> suiteResults = suite->runAllBenchmarks();
        allResults.insert(allResults.end(), suiteResults.begin(), suiteResults.end());
    }
    
    return allResults;
}

BenchmarkComparison BenchmarkRunner::compareBenchmarks(const std::string& suiteName, 
                                                       const std::string& baselineName,
                                                       const std::string& testName) {
    BenchmarkComparison comparison;
    comparison.baselineBenchmark = baselineName;
    comparison.testBenchmark = testName;
    comparison.performanceImprovement = 0.0;
    comparison.executionTimeRatio = 1.0;
    comparison.memoryImprovement = 1.0;
    comparison.isRegressed = false;
    comparison.confidenceLevel = 0.0;
    comparison.analysis = "Placeholder analysis";
    
    return comparison;
}

std::vector<BenchmarkComparison> BenchmarkRunner::compareSuites(const std::string& baselineSuiteName, 
                                                                const std::string& testSuiteName) {
    return std::vector<BenchmarkComparison>();
}

void BenchmarkRunner::setTimeout(double timeout) {
    this->timeout = timeout;
}

void BenchmarkRunner::setMaxMemory(size_t maxMemory) {
    this->maxMemory = maxMemory;
}

void BenchmarkRunner::setParallelExecution(bool enable) {
    parallelExecution = enable;
}

void BenchmarkRunner::setThreadCount(size_t count) {
    threadCount = count;
}

void BenchmarkRunner::addComparator(const std::string& name, BenchmarkComparator comparator) {
    comparators[name] = comparator;
}

BenchmarkComparator BenchmarkRunner::getComparator(const std::string& name) const {
    if (comparators.find(name) != comparators.end()) {
        return comparators.at(name);
    }
    return BenchmarkComparator();
}

void BenchmarkRunner::clearResults() {
    // Implementation for clearing results
}

std::vector<BenchmarkResult> BenchmarkRunner::getResults(const std::string& suiteName, const std::string& benchmarkName) const {
    return std::vector<BenchmarkResult>();
}

std::map<std::string, std::vector<BenchmarkResult>> BenchmarkRunner::getAllResults() const {
    std::map<std::string, std::vector<BenchmarkResult>> allResults;
    
    for (const auto& suite : suites) {
        std::string suiteName = suite->getName();
        for (const auto& benchmarkName : suite->getBenchmarkNames()) {
            auto result = runBenchmark(suiteName, benchmarkName);
            allResults[benchmarkName].push_back(result);
        }
    }
    
    return allResults;
}

std::vector<BenchmarkResult> BenchmarkRunner::runStressTest(const std::string& suiteName, 
                                                            size_t iterationCount) {
    return std::vector<BenchmarkResult>();
}

std::vector<BenchmarkResult> BenchmarkRunner::runLoadTest(const std::string& suiteName,
                                                          size_t concurrentLoad) {
    return std::vector<BenchmarkResult>();
}

std::vector<BenchmarkResult> BenchmarkRunner::runScalabilityTest(const std::string& suiteName,
                                                                 const std::vector<size_t>& neuronCounts,
                                                                 const std::vector<size_t>& stepCounts) {
    return std::vector<BenchmarkResult>();
}

bool BenchmarkRunner::runRegressionTest(const std::string& suiteName, double regressionThreshold) {
    return false;
}

std::vector<BenchmarkComparison> BenchmarkRunner::detectRegressions(double regressionThreshold) const {
    return std::vector<BenchmarkComparison>();
}

} // namespace nlm