#pragma once

/**
 * Phase6DemoIntegration.hpp - Comprehensive Phase 6 Integration Demo
 * 
 * This header file defines the Phase6IntegrationDemo class that orchestrates
 * comprehensive integration testing of all brain systems in NLM.
 * 
 * The demo provides:
 * - Multiple integration test scenarios
 * - Performance benchmarking
 * - Memory leak detection
 * - Cross-system interaction testing
 * - Comprehensive reporting and export capabilities
 * - Progress monitoring and detailed analytics
 */

#include <string>
#include <vector>
#include <chrono>
#include <memory>

namespace nlm {

/**
 * Performance metrics for benchmarking
 */
struct PerformanceMetrics {
    size_t neuronCount;
    uint64_t steps;
    float totalReward;
    float avgFiringRate;
    double duration;
    float rewardPerStep;
    float performanceScore;  // reward/sec
    
    PerformanceMetrics() : neuronCount(0), steps(0), totalReward(0.0f), 
                          avgFiringRate(0.0f), duration(0.0), rewardPerStep(0.0f), performanceScore(0.0f) {}
};

/**
 * Results from performance benchmarking
 */
struct PerformanceBenchmarkResults {
    std::vector<PerformanceMetrics> benchmarks;
    
    PerformanceBenchmarkResults() {}
};

/**
 * Phase 6 Integration Demo Class
 * 
 * Orchestrates comprehensive integration testing of all brain systems.
 * Provides multiple demo scenarios and detailed reporting capabilities.
 */
class Phase6IntegrationDemo {
public:
    Phase6IntegrationDemo();
    ~Phase6IntegrationDemo();
    
    /**
     * Run quick integration verification test
     * Lightweight test for basic integration verification
     */
    void runQuickIntegrationTest();
    
    /**
     * Run full integration test
     * Comprehensive integration test with detailed verification
     */
    void runFullIntegrationTest();
    
    /**
     * Run stress test
     * High-load stress testing of brain systems
     */
    void runStressTest();
    
    /**
     * Run memory leak test
     * Detect memory leaks over multiple iterations
     */
    void runMemoryLeakTest();
    
    /**
     * Run performance benchmark
     * Benchmark performance across different brain sizes
     */
    void runPerformanceBenchmark();
    
    /**
     * Run cross-system interaction test
     * Test interaction between different brain systems
     */
    void runCrossSystemInteractionTest();
    
    /**
     * Run comprehensive demo
     * Execute all demo scenarios in sequence
     */
    void runComprehensiveDemo();
    
    /**
     * Generate final integration report
     * Create comprehensive HTML report of all test results
     */
    void generateFinalReport();
    
    /**
     * Export integration report
     * Export detailed integration results to file
     */
    void exportIntegrationReport(const struct Phase6IntegrationResult& result, 
                                 const struct Phase6Config& config, 
                                 int integrationPercentage);
    
    /**
     * Export performance metrics
     * Export performance metrics to JSON file
     */
    void exportPerformanceMetrics(const struct Phase6IntegrationResult& result, double experimentDuration);
    
    /**
     * Export stress test report
     * Export stress test results to file
     */
    void exportStressTestReport(const struct Phase6IntegrationResult& result,
                                const struct Phase6Config& config,
                                double duration);
    
    /**
     * Export memory leak report
     * Export memory leak analysis to file
     */
    void exportMemoryLeakReport(const std::vector<double>& memoryUsage,
                                double avgMemoryUsage,
                                double memoryGrowthRate,
                                bool hasMemoryLeak);
    
    /**
     * Export benchmark report
     * Export benchmark results to file
     */
    void exportBenchmarkReport(const PerformanceBenchmarkResults& results);
    
    /**
     * Export cross-system report
     * Export cross-system interaction analysis to file
     */
    void exportCrossSystemReport(const struct Phase6IntegrationResult& result,
                                 const struct Phase6Config& config,
                                 int crossSystemScore,
                                 bool crossSystemOK);
private:
    /**
     * Initialize logging system for demo
     */
    void initializeLogger();
    
    /**
     * Shutdown logging system
     */
    void shutdownLogger();
    
    /**
     * Setup memory monitoring infrastructure
     */
    void setupMemoryMonitoring();
    
    /**
     * Initialize metrics collection
     */
    void initializeMetrics();
    
    /**
     * Update demo metrics with results
     */
    void updateDemoMetrics(const std::string& demoName, bool success, double duration);
    
    /**
     * Update demo metrics with score
     */
    void updateDemoMetrics(const std::string& demoName, int score, double duration);
    
    /**
     * Get current timestamp as string
     */
    std::string getCurrentTimestamp();
};

} // namespace nlm