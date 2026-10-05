// NLM Performance Header
// Main header for performance monitoring and profiling capabilities
// Includes all performance-related headers and provides unified interface

#pragma once

#include "performance/PerformanceProfiler.hpp"
#include "performance/PerformanceMonitor.hpp"
#include "performance/PerformanceAnalyzer.hpp"

namespace nlm {

// Global performance profiler instance
extern PerformanceProfiler performanceProfiler;

// Global system performance monitor instance
extern SystemPerformanceMonitor systemPerformanceMonitor;

// Global performance analyzer instance
extern PerformanceAnalyzer performanceAnalyzer;

// Convenience functions for performance profiling
inline void startProfile(const std::string& operationName) {
    performanceProfiler.startProfile(operationName);
}

inline double stopProfile(const std::string& operationName) {
    return performanceProfiler.stopProfile(operationName);
}

inline void addPerformanceMetric(const std::string& metricName, double value) {
    performanceProfiler.addCustomMetric(metricName, value);
}

inline double getPerformanceMetric(const std::string& metricName) {
    return performanceProfiler.getCustomMetric(metricName);
}

inline std::string generatePerformanceReport() {
    return performanceProfiler.generatePerformanceReport();
}

inline bool exportPerformanceData(const std::string& filename) {
    return performanceProfiler.exportPerformanceData(filename);
}

inline void startSystemMonitoring() {
    systemPerformanceMonitor.startMonitoring();
}

inline void stopSystemMonitoring() {
    systemPerformanceMonitor.stopMonitoring();
}

inline SystemPerformanceMonitor::SystemMetrics getSystemMetrics() {
    return systemPerformanceMonitor.getSystemMetrics();
}

inline SystemPerformanceMonitor::ApplicationMetrics getApplicationMetrics() {
    return systemPerformanceMonitor.getApplicationMetrics();
}

inline SystemPerformanceMonitor::CombinedMetrics getCombinedMetrics() {
    return systemPerformanceMonitor.getCombinedMetrics();
}

inline SystemPerformanceMonitor::PerformanceAnalysis analyzePerformance() {
    return systemPerformanceMonitor.analyzePerformance();
}

inline std::vector<SystemPerformanceMonitor::PerformanceComparison> 
compareWithBaseline(const std::string& baselineFile) {
    return systemPerformanceMonitor.compareWithBaseline(baselineFile);
}

inline std::vector<std::string> getOptimizationRecommendations() {
    return systemPerformanceMonitor.getOptimizationRecommendations();
}

inline SystemPerformanceMonitor::BenchmarkResult runBenchmark(
    const std::string& name,
    const std::shared_ptr<Config>& config) {
    return systemPerformanceMonitor.runBenchmark(name, config);
}

inline bool profilePerformance(const std::string& name,
                               const std::function<void()>& workload,
                               int duration) {
    return systemPerformanceMonitor.profilePerformance(name, workload, duration);
}

inline bool analyzePerformanceProfile(const std::string& profileFile,
                                      const std::string& outputFile = "") {
    return systemPerformanceMonitor.analyzePerformanceProfile(profileFile, outputFile);
}

// Performance testing and benchmarking functions
inline TestResults runTestSuite(TestSuite& testSuite, const std::shared_ptr<Config>& config) {
    return testRunner.runTestSuite(testSuite, config);
}

inline BenchmarkResults runBenchmarkSuite(BenchmarkSuite& benchmarkSuite,
                                          const std::shared_ptr<Config>& config) {
    return testRunner.runBenchmarkSuite(benchmarkSuite, config);
}

inline TestResults runTestMultipleIterations(TestCase& testCase,
                                            const std::shared_ptr<Config>& config,
                                            int iterations = 5) {
    return testRunner.runTestMultipleIterations(testCase, config, iterations);
}

inline BenchmarkResults runBenchmarkMultipleIterations(BenchmarkCase& benchmarkCase,
                                                       const std::shared_ptr<Config>& config,
                                                       int iterations = 5) {
    return testRunner.runBenchmarkMultipleIterations(benchmarkCase, config, iterations);
}

// Performance result export functions
inline bool exportTestResults(const std::string& filename,
                             const TestResults& results) {
    return testRunner.exportTestResults(filename, results);
}

inline bool exportBenchmarkResults(const std::string& filename,
                                   const BenchmarkResults& results) {
    return testRunner.exportBenchmarkResults(filename, results);
}

} // namespace nlm
