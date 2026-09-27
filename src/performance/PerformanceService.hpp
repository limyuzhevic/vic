// NLM Performance Optimization and Monitoring
//
// Performance monitoring and optimization tools for advanced users
// Provides performance analysis, benchmarking, and optimization recommendations
// Helps users identify performance bottlenecks and optimize neural simulations

#pragma once

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <map>
#include <algorithm>
#include <memory>
#include <sstream>
#include <iomanip>
#include <unordered_map>
#include <set>
#include <chrono>
#include <ctime>
#include <functional>

namespace nlm {

// Simple performance metrics
struct PerformanceMetrics {
    std::chrono::high_resolution_clock::time_point startTime;
    std::chrono::high_resolution_clock::time_point endTime;
    double cpuUsage;
    double memoryUsage;
    size_t memoryAllocated;
    size_t processCount;
    double simulationSpeed;
    double totalSimulationTime;
    double efficiencyScore;
    
    PerformanceMetrics() : cpuUsage(0.0), memoryUsage(0.0), memoryAllocated(0), 
                             processCount(0), simulationSpeed(0.0), 
                             totalSimulationTime(0.0), efficiencyScore(0.0) {
        startTime = std::chrono::high_resolution_clock::now();
    }
    
    void endTiming() {
        endTime = std::chrono::high_resolution_clock::now();
        totalSimulationTime = std::chrono::duration<double>(endTime - startTime).count();
        efficiencyScore = calculateEfficiency();
    }
    
    double calculateEfficiency() const {
        if (totalSimulationTime > 0) {
            return (1.0 / totalSimulationTime) * 100.0;
        }
        return 0.0;
    }
    
    double getStepsPerSecond(size_t steps) const {
        if (totalSimulationTime > 0) {
            return static_cast<double>(steps) / totalSimulationTime;
        }
        return 0.0;
    }
};

// Simple performance analyzer
class PerformanceAnalyzer {
public:
    PerformanceAnalyzer();
    ~PerformanceAnalyzer();
    
    // Performance measurement
    void startMeasurement();
    void stopMeasurement();
    bool isMeasuring() const;
    PerformanceMetrics getCurrentMetrics() const;
    
    // Performance analysis
    std::string analyzePerformance(const PerformanceMetrics& metrics) const;
    std::vector<std::string> getOptimizationRecommendations(const PerformanceMetrics& metrics) const;
    double calculatePerformanceScore(const PerformanceMetrics& metrics) const;
    
    // Benchmarking
    PerformanceMetrics runBenchmark(size_t neuronCount, size_t steps, double timeStep = 0.001);
    
    // Statistics
    double getAverageCPUUsage() const;
    
    // Export
    bool exportMetrics(const std::string& filename, const PerformanceMetrics& metrics);
    std::string generatePerformanceReport(const PerformanceMetrics& metrics) const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Benchmarking system
class BenchmarkSystem {
public:
    BenchmarkSystem();
    ~BenchmarkSystem();
    
    // Benchmark execution
    PerformanceMetrics runBenchmark(const std::string& name, size_t neuronCount, size_t steps);
    
    // Analysis
    std::string analyzeBenchmarkResults(const std::vector<PerformanceMetrics>& results) const;
    std::map<std::string, double> compareBenchmarks(const std::vector<PerformanceMetrics>& results,
                                                    const std::vector<std::string>& labels) const;
    
    // Export
    bool exportBenchmarkResults(const std::string& filename, const std::vector<PerformanceMetrics>& results);
    std::string generateBenchmarkReport(const std::vector<PerformanceMetrics>& results,
                                        const std::vector<std::string>& labels) const;
    
    // Statistics
    double getAverageBenchmarkScore() const;
    std::string getBenchmarkSummary() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Performance service
class PerformanceService {
public:
    static PerformanceService& getInstance();
    static void initialize();
    static void shutdown();
    
    // Performance analysis
    static PerformanceAnalyzer& getPerformanceAnalyzer();
    static PerformanceMetrics analyzePerformance(size_t neuronCount, size_t steps, double timeStep = 0.001);
    
    // Benchmarking
    static BenchmarkSystem& getBenchmarkSystem();
    static PerformanceMetrics runBenchmark(const std::string& name, size_t neuronCount, size_t steps);
    
    // Statistics
    static std::map<std::string, size_t> getSystemStatistics();
    static void resetStatistics();
    
private:
    PerformanceService();
    ~PerformanceService();
    PerformanceService(const PerformanceService&) = delete;
    PerformanceService& operator=(const PerformanceService&) = delete;
    
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Helper functions
namespace PerfUtils {
    static std::string formatNumber(double value, int decimals = 2);
    static std::string formatBytes(size_t bytes);
    static std::string formatDuration(double seconds);
    static double calculateEfficiency(double time);
    static size_t calculateSpeed(size_t steps, double time);
    static std::string getPerformanceRating(double score);
    static std::vector<std::string> getPerformanceTips();
    static std::string getSystemInfo();
}}

} // namespace nlm