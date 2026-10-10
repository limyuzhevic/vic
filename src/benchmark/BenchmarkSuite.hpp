#pragma once

#include "BenchmarkRunner.hpp"
#include <string>
#include <vector>
#include <memory>
#include <mutex>
#include <unordered_map>
#include <algorithm>
#include <numeric>
#include <cmath>

namespace nlm {

class BenchmarkSuite {
private:
    std::string suiteName;
    std::vector<std::unique_ptr<Benchmark>> benchmarks;
    std::unordered_map<std::string, std::vector<BenchmarkMetrics>> baselineResults;
    mutable std::mutex suiteMutex;
    
    // Suite configuration
    bool autoTeardown;           // Automatically teardown after each run
    bool validateBeforeRun;      // Validate benchmarks before running
    double successThreshold;     // Minimum success rate threshold
    size_t maxRetries;           // Maximum retries for failed benchmarks
    
    // Statistics
    std::unordered_map<std::string, size_t> executionCounts;
    std::unordered_map<std::string, double> averageTimes;
    
public:
    explicit BenchmarkSuite(const std::string& name);
    ~BenchmarkSuite() = default;
    
    // Benchmark management
    void addBenchmark(std::unique_ptr<Benchmark> benchmark);
    void removeBenchmark(const std::string& name);
    bool hasBenchmark(const std::string& name) const;
    
    // Suite configuration
    void setAutoTeardown(bool enable);
    void setValidateBeforeRun(bool enable);
    void setSuccessThreshold(double threshold);
    void setMaxRetries(size_t retries);
    
    bool getAutoTeardown() const;
    bool getValidateBeforeRun() const;
    double getSuccessThreshold() const;
    size_t getMaxRetries() const;
    
    // Benchmark information
    std::string getName() const;
    size_t getBenchmarkCount() const;
    std::vector<std::string> getBenchmarkNames() const;
    std::vector<BenchmarkCategory> getBenchmarkCategories() const;
    BenchmarkCategory getBenchmarkCategory(const std::string& name) const;
    
    // Benchmark execution
    BenchmarkResult runBenchmark(const std::string& name);
    std::vector<BenchmarkResult> runAllBenchmarks();
    
    // Single benchmark execution with retry logic
    BenchmarkResult runBenchmarkWithRetry(const std::string& name);
    
    // Benchmark information retrieval
    Benchmark* getBenchmark(const std::string& name) const;
    std::string getBenchmarkDescription(const std::string& name) const;
    size_t getBenchmarkExpectedNeurons(const std::string& name) const;
    size_t getBenchmarkExpectedSteps(const std::string& name) const;
    
    // Baseline management
    void setBaseline(const std::string& benchmarkName, const BenchmarkMetrics& metrics);
    void setBaseline(const std::string& benchmarkName, const std::vector<BenchmarkMetrics>& metrics);
    bool hasBaseline(const std::string& benchmarkName) const;
    std::vector<BenchmarkMetrics> getBaselineResults(const std::string& benchmarkName) const;
    void clearBaselines();
    
    // Baseline comparison
    BenchmarkComparison compareWithBaseline(const std::string& benchmarkName, const BenchmarkMetrics& testMetrics) const;
    std::vector<BenchmarkComparison> compareAllWithBaseline(const std::string& baselineName) const;
    
    // Statistical analysis
    double calculateAverageExecutionTime(const std::string& benchmarkName) const;
    double calculateStandardDeviation(const std::string& benchmarkName) const;
    double calculateSuccessRate(const std::string& benchmarkName) const;
    BenchmarkMetrics calculateAverageMetrics(const std::string& benchmarkName) const;
    
    // Performance analysis
    bool detectRegression(const std::string& benchmarkName, double threshold = 0.05) const;
    double calculateImprovement(const std::string& baselineName, const std::string& testName) const;
    
    // Benchmark categorization
    std::vector<BenchmarkResult> getBenchmarksByCategory(BenchmarkCategory category) const;
    std::vector<BenchmarkResult> getBenchmarksByType(const std::string& type) const;
    
    // Suite-level statistics
    double getOverallSuccessRate() const;
    size_t getTotalExecutionCount() const;
    std::unordered_map<std::string, size_t> getExecutionCounts() const;
    std::unordered_map<std::string, double> getAverageTimes() const;
    
    // Results analysis
    std::vector<BenchmarkResult> filterResults(bool passedOnly = false, 
                                               BenchmarkCategory category = BenchmarkCategory::CPU_PERFORMANCE,
                                               const std::string& benchmarkName = "") const;
    
    // Suite execution statistics
    void updateExecutionStats(const std::string& benchmarkName, double executionTime, bool success);
    
    // Suite validation
    bool validateAllBenchmarks() const;
    std::vector<std::string> getValidationErrors(const std::string& benchmarkName) const;
    
    // Export functionality
    void exportResults(const std::string& filename, const std::string& format = "json") const;
    std::string formatResults(const std::string& format = "text") const;
    
    // Utility functions
    static std::string benchmarkCategoryToString(BenchmarkCategory category);
    static BenchmarkCategory stringToBenchmarkCategory(const std::string& str);
};

} // namespace nlm