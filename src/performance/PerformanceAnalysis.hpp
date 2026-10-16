// Performance Analysis Tools
// Provides performance analysis and optimization recommendations
// Integrates with existing performance infrastructure

#pragma once

#include <chrono>
#include <memory>
#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <algorithm>
#include <numeric>
#include <cmath>
#include <iomanip>
#include <sstream>
#include <cstdint>
#include <map>
#include <functional>
#include <set>

#include "performance/Performance.hpp"
#include "performance/PerformanceMonitoring.hpp"

namespace nlm {

/**
 * Performance Analysis Result
 */
struct PerformanceAnalysis {
    std::string analysisName;
    double score;  // 0.0 to 1.0, higher is better
    std::vector<std::string> recommendations;
    std::vector<std::string> warnings;
    std::unordered_map<std::string, double> metrics;
    
    PerformanceAnalysis(const std::string& name = "") 
        : analysisName(name), score(0.0) {}
};

/**
 * Performance Bottleneck Analysis
 */
struct BottleneckAnalysis {
    std::string bottleneckType;  // "memory", "cpu", "cache", "simd", "parallel"
    double severity;  // 0.0 to 1.0
    std::string location;  // Function or component name
    double impact;  // Performance impact
    std::string recommendation;
    
    BottleneckAnalysis(const std::string& type = "", double sev = 0.0, 
                      const std::string& loc = "", double impact = 0.0,
                      const std::string& rec = "")
        : bottleneckType(type), severity(sev), location(loc), impact(impact), recommendation(rec) {}
};

/**
 * Performance Analyzer - Analyzes performance data for optimization opportunities
 */
class PerformanceAnalyzer {
public:
    PerformanceAnalyzer();
    ~PerformanceAnalyzer() = default;
    
    // Disable copying
    PerformanceAnalyzer(const PerformanceAnalyzer&) = delete;
    PerformanceAnalyzer& operator=(const PerformanceAnalyzer&) = delete;
    
    /**
     * Analyze performance samples
     */
    PerformanceAnalysis analyzeSamples(const std::vector<PerformanceSample>& samples) const;
    
    /**
     * Analyze performance metrics
     */
    PerformanceAnalysis analyzeMetrics(const std::unordered_map<PerformanceMetric, double>& metrics) const;
    
    /**
     * Detect performance bottlenecks
     */
    std::vector<BottleneckAnalysis> detectBottlenecks(const std::vector<PerformanceSample>& samples) const;
    
    /**
     * Analyze SIMD efficiency
     */
    PerformanceAnalysis analyzeSIMDEfficiency(const std::vector<PerformanceSample>& samples) const;
    
    /**
     * Analyze memory efficiency
     */
    PerformanceAnalysis analyzeMemoryEfficiency(const std::vector<PerformanceSample>& samples) const;
    
    /**
     * Analyze parallel efficiency
     */
    PerformanceAnalysis analyzeParallelEfficiency(const std::vector<PerformanceSample>& samples) const;
    
    /**
     * Generate optimization recommendations
     */
    std::vector<std::string> generateRecommendations(const PerformanceAnalysis& analysis) const;
    
    /**
     * Calculate performance score (0.0 to 1.0)
     */
    double calculatePerformanceScore(const PerformanceAnalysis& analysis) const;
    
    /**
     * Compare performance across different configurations
     */
    std::vector<std::string> comparePerformance(const std::vector<PerformanceAnalysis>& analyses) const;
    
    /**
     * Get singleton instance
     */
    static PerformanceAnalyzer& getInstance() {
        static PerformanceAnalyzer instance;
        return instance;
    }
    
private:
    double calculateSIMDEfficiencyScore(const std::vector<PerformanceSample>& samples) const;
    double calculateMemoryEfficiencyScore(const std::vector<PerformanceSample>& samples) const;
    double calculateParallelEfficiencyScore(const std::vector<PerformanceSample>& samples) const;
    double calculateNumericalStabilityScore(const std::vector<PerformanceSample>& samples) const;
    std::string getSIMDRecommendation(double efficiency) const;
    std::string getMemoryRecommendation(double efficiency) const;
    std::string getParallelRecommendation(double efficiency) const;
};

/**
 * Numerical Stability Analysis
 */
class NumericalStabilityAnalyzer {
public:
    NumericalStabilityAnalyzer();
    ~NumericalStabilityAnalyzer() = default;
    
    /**
     * Analyze numerical stability from samples
     */
    PerformanceAnalysis analyzeNumericalStability(const std::vector<PerformanceSample>& samples) const;
    
    /**
     * Detect numerical instability patterns
     */
    std::vector<std::string> detectInstabilityPatterns(const std::vector<PerformanceSample>& samples) const;
    
    /**
     * Calculate numerical stability metrics
     */
    struct NumericalMetrics {
        double maxError;
        double meanError;
        double errorStddev;
        double stabilityScore;
        uint64_t errorCount;
        uint64_t finiteCheckCount;
        
        NumericalMetrics() : maxError(0.0), meanError(0.0), errorStddev(0.0), 
                           stabilityScore(1.0), errorCount(0), finiteCheckCount(0) {}
    };
    
    NumericalMetrics calculateNumericalMetrics(const std::vector<PerformanceSample>& samples) const;
    
    /**
     * Generate numerical stability recommendations
     */
    std::vector<std::string> generateStabilityRecommendations(const NumericalMetrics& metrics) const;
    
    /**
     * Get singleton instance
     */
    static NumericalStabilityAnalyzer& getInstance() {
        static NumericalStabilityAnalyzer instance;
        return instance;
    }
    
private:
    double calculateStabilityScore(const NumericalMetrics& metrics) const;
    std::string getErrorAnalysis(const NumericalMetrics& metrics) const;
};

/**
 * Performance Optimizer - Provides optimization recommendations based on analysis
 */
class PerformanceOptimizer {
public:
    PerformanceOptimizer();
    ~PerformanceOptimizer() = default;
    
    /**
     * Optimize SIMD usage
     */
    struct SIMDOptimization {
        bool enableAVX512;
        bool enableAVX2;
        bool enableSSE;
        size_t recommendedSIMDWidth;
        std::string recommendation;
        
        SIMDOptimization() : enableAVX512(false), enableAVX2(false), enableSSE(false),
                           recommendedSIMDWidth(1), recommendation("") {}
    };
    
    SIMDOptimization optimizeSIMDUsage(const std::vector<PerformanceSample>& samples) const;
    
    /**
     * Optimize memory usage
     */
    struct MemoryOptimization {
        bool enableMemoryPool;
        size_t recommendedPoolSize;
        bool enableSOA;
        size_t recommendedCacheLineSize;
        std::string recommendation;
        
        MemoryOptimization() : enableMemoryPool(false), recommendedPoolSize(1024),
                            enableSOA(false), recommendedCacheLineSize(64), recommendation("") {}
    };
    
    MemoryOptimization optimizeMemoryUsage(const std::vector<PerformanceSample>& samples) const;
    
    /**
     * Optimize parallel processing
     */
    struct ParallelOptimization {
        size_t recommendedThreadCount;
        bool enableWorkStealing;
        size_t recommendedGrainSize;
        std::string recommendation;
        
        ParallelOptimization() : recommendedThreadCount(1), enableWorkStealing(false),
                               recommendedGrainSize(100), recommendation("") {}
    };
    
    ParallelOptimization optimizeParallelProcessing(const std::vector<PerformanceSample>& samples) const;
    
    /**
     * Generate comprehensive optimization plan
     */
    struct OptimizationPlan {
        std::string name;
        double expectedSpeedup;
        double expectedMemoryReduction;
        double expectedNumericalImprovement;
        std::vector<std::string> actions;
        std::vector<std::string> warnings;
        uint64_t estimatedImplementationEffort;  // in hours
        
        OptimizationPlan(const std::string& n = "", double speedup = 0.0,
                        double memReduction = 0.0, double numImprove = 0.0,
                        uint64_t effort = 0)
            : name(n), expectedSpeedup(speedup), expectedMemoryReduction(memReduction),
              expectedNumericalImprovement(numImprove), estimatedImplementationEffort(effort) {}
    };
    
    OptimizationPlan generateOptimizationPlan(const PerformanceAnalysis& analysis) const;
    
    /**
     * Get singleton instance
     */
    static PerformanceOptimizer& getInstance() {
        static PerformanceOptimizer instance;
        return instance;
    }
    
private:
    double estimateSIMDOptimizationImpact(double currentEfficiency) const;
    double estimateMemoryOptimizationImpact(double currentUsage) const;
    double estimateParallelOptimizationImpact(double currentUtilization) const;
};

/**
 * Performance Benchmark Suite
 */
class PerformanceBenchmarkSuite {
public:
    PerformanceBenchmarkSuite();
    ~PerformanceBenchmarkSuite() = default;
    
    /**
     * Run all benchmarks
     */
    std::vector<BenchmarkingFramework::BenchmarkResult> runAllBenchmarks() const;
    
    /**
     * Run scalability benchmarks
     */
    std::vector<BenchmarkingFramework::BenchmarkResult> runScalabilityBenchmarks() const;
    
    /**
     * Run numerical stability benchmarks
     */
    std::vector<BenchmarkingFramework::BenchmarkResult> runNumericalStabilityBenchmarks() const;
    
    /**
     * Run architecture-specific benchmarks
     */
    std::vector<BenchmarkingFramework::BenchmarkResult> runArchitectureBenchmarks() const;
    
    /**
     * Generate benchmark comparison report
     */
    std::string generateBenchmarkReport(const std::vector<BenchmarkingFramework::BenchmarkResult>& results) const;
    
    /**
     * Get baseline benchmarks
     */
    std::vector<BenchmarkingFramework::BenchmarkResult> getBaselineBenchmarks() const;
    
    /**
     * Save benchmark results
     */
    bool saveBenchmarkResults(const std::string& filename) const;
    
    /**
     * Load benchmark results
     */
    bool loadBenchmarkResults(const std::string& filename);
    
    /**
     * Get singleton instance
     */
    static PerformanceBenchmarkSuite& getInstance() {
        static PerformanceBenchmarkSuite instance;
        return instance;
    }
    
private:
    std::vector<BenchmarkingFramework::BenchmarkResult> runMicroBenchmarks() const;
    std::vector<BenchmarkingFramework::BenchmarkResult> runScalingBenchmarks() const;
    std::vector<BenchmarkingFramework::BenchmarkResult> runNumericsBenchmarks() const;
    std::vector<BenchmarkingFramework::BenchmarkResult> runSIMDBenchmarks() const;
    std::vector<BenchmarkingFramework::BenchmarkResult> runMemoryBenchmarks() const;
};

/**
 * Performance Configuration
 */
struct PerformanceConfig {
    // SIMD configuration
    bool enableSIMD;
    bool enableAVX512;
    bool enableAVX2;
    bool enableSSE;
    
    // Memory configuration
    bool enableMemoryPool;
    size_t memoryPoolSize;
    bool enableSOA;
    size_t cacheLineSize;
    
    // Parallel configuration
    bool enableParallelProcessing;
    size_t threadCount;
    bool enableWorkStealing;
    
    // Event queue configuration
    bool enableEventQueue;
    size_t spikeQueueCapacity;
    bool enableDelayedSpikeQueue;
    
    // Monitoring configuration
    bool enablePerformanceMonitoring;
    std::chrono::milliseconds monitoringInterval;
    bool enableNumericalStabilityMonitoring;
    bool enableProfiling;
    
    // Optimization configuration
    bool enableAutomaticOptimization;
    double optimizationThreshold;
    
    PerformanceConfig()
        : enableSIMD(true), enableAVX512(true), enableAVX2(true), enableSSE(true)
        , enableMemoryPool(true), memoryPoolSize(1024), enableSOA(true), cacheLineSize(64)
        , enableParallelProcessing(true), threadCount(std::thread::hardware_concurrency())
        , enableWorkStealing(true)
        , enableEventQueue(true), spikeQueueCapacity(16384), enableDelayedSpikeQueue(true)
        , enablePerformanceMonitoring(true), monitoringInterval(100)
        , enableNumericalStabilityMonitoring(true), enableProfiling(true)
        , enableAutomaticOptimization(true), optimizationThreshold(0.95) {}
};

/**
 * Performance Configuration Manager
 */
class PerformanceConfigManager {
public:
    PerformanceConfigManager();
    ~PerformanceConfigManager() = default;
    
    /**
     * Load configuration from file
     */
    bool loadFromFile(const std::string& filename);
    
    /**
     * Save configuration to file
     */
    bool saveToFile(const std::string& filename) const;
    
    /**
     * Get current configuration
     */
    PerformanceConfig getConfig() const { return config_; }
    
    /**
     * Update configuration
     */
    void updateConfig(const PerformanceConfig& newConfig) { config_ = newConfig; }
    
    /**
     * Get configuration for target scale
     */
    PerformanceConfig getConfigForScale(size_t neuronCount) const;
    
    /**
     * Generate configuration report
     */
    std::string generateConfigReport() const;
    
    /**
     * Get singleton instance
     */
    static PerformanceConfigManager& getInstance() {
        static PerformanceConfigManager instance;
        return instance;
    }
    
private:
    PerformanceConfig config_;
    std::unordered_map<size_t, PerformanceConfig> scaleConfigs_;
};

/**
 * Performance Regression Testing Framework
 */
class PerformanceRegressionTester {
public:
    PerformanceRegressionTester();
    ~PerformanceRegressionTester() = default;
    
    /**
     * Add baseline performance data
     */
    void addBaseline(const std::string& baselineName, 
                     const std::vector<BenchmarkingFramework::BenchmarkResult>& results);
    
    /**
     * Test current performance against baseline
     */
    struct RegressionTestResult {
        std::string testName;
        bool passed;
        double performanceChange;  // percentage
        double confidence;  // statistical confidence
        std::string recommendation;
        std::string details;
        
        RegressionTestResult(const std::string& name = "", bool pass = false,
                            double change = 0.0, double confidence = 0.0,
                            const std::string& rec = "", const std::string& det = "")
            : testName(name), passed(pass), performanceChange(change),
              confidence(confidence), recommendation(rec), details(det) {}
    };
    
    RegressionTestResult testRegression(const std::string& baselineName,
                                       const BenchmarkingFramework::BenchmarkResult& currentResult) const;
    
    /**
     * Run comprehensive regression tests
     */
    std::vector<RegressionTestResult> runAllRegressionTests(
        const std::vector<BenchmarkingFramework::BenchmarkResult>& currentResults) const;
    
    /**
     * Get all baseline names
     */
    std::vector<std::string> getBaselineNames() const;
    
    /**
     * Save baseline data
     */
    bool saveBaseline(const std::string& filename) const;
    
    /**
     * Load baseline data
     */
    bool loadBaseline(const std::string& filename);
    
    /**
     * Generate regression test report
     */
    std::string generateRegressionReport(const std::vector<RegressionTestResult>& results) const;
    
    /**
     * Get singleton instance
     */nlm {
    static PerformanceRegressionTester& getInstance() {
        static PerformanceRegressionTester instance;
        return instance;
    }
    
private:
    std::unordered_map<std::string, std::vector<BenchmarkingFramework::BenchmarkResult>> baselines_;
    
    static constexpr double REGRESSION_THRESHOLD = 0.05;  // 5% threshold
    static constexpr double CONFIDENCE_THRESHOLD = 0.95;  // 95% confidence
};

/**
 * Performance Documentation Generator
 */
class PerformanceDocumentationGenerator {
public:
    PerformanceDocumentationGenerator();
    ~PerformanceDocumentationGenerator() = default;
    
    /**
     * Generate comprehensive performance documentation
     */
    std::string generatePerformanceDocumentation() const;
    
    /**
     * Generate API documentation for performance modules
     */
    std::string generateAPIDocumentation() const;
    
    /**
     * Generate performance configuration documentation
     */
    std::string generateConfigDocumentation() const;
    
    /**
     * Generate optimization guide documentation
     */
    std::string generateOptimizationGuide() const;
    
    /**
     * Generate numerical stability guide documentation
     */
    std::string generateNumericalStabilityGuide() const;
    
    /**
     * Generate benchmark documentation
     */
    std::string generateBenchmarkDocumentation() const;
    
    /**
     * Save documentation to files
     */
    bool saveDocumentation(const std::string& outputDir) const;
    
    /**
     * Get singleton instance
     */
    static PerformanceDocumentationGenerator& getInstance() {
        static PerformanceDocumentationGenerator instance;
        return instance;
    }
    
private:
    std::string generateHeader(const std::string& title) const;
    std::string generateSection(const std::string& title, const std::string& content) const;
    std::string generateTable(const std::vector<std::vector<std::string>>& rows) const;
};

} // namespace nlm
