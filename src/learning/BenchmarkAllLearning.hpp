#pragma once

#include "../learning/ReconstructionError.hpp"
#include "../learning/PredictiveCoding.hpp"
#include "../learning/PolicyGradient.hpp"
#include "../learning/MetaLearning.hpp"
#include "../core/Config/Config.hpp"
#include "../core/Brain.hpp"
#include <iostream>
#include <vector>
#include <chrono>
#include <iomanip>

namespace nlm {

// BenchmarkAllLearning: Comprehensive benchmark of all learning mechanisms
// Tests the performance of reconstruction error, predictive coding,
// policy gradient, and meta-learning systems

class BenchmarkAllLearning {
public:
    BenchmarkAllLearning(std::shared_ptr<Config> config);
    ~BenchmarkAllLearning();
    
    // Run all learning mechanism benchmarks
    void runAllBenchmarks();
    
    // Benchmark reconstruction error learning
    void benchmarkReconstructionError(size_t duration = 500);
    
    // Benchmark predictive coding
    void benchmarkPredictiveCoding(size_t duration = 500);
    
    // Benchmark policy gradient learning
    void benchmarkPolicyGradient(size_t duration = 500);
    
    // Benchmark meta-learning
    void benchmarkMetaLearning(size_t duration = 500);
    
    // Print comprehensive benchmark results
    void printBenchmarkResults() const;
    
    // Get overall benchmark statistics
    struct BenchmarkStats {
        size_t reconstructionErrorScore;
        size_t predictiveCodingScore;
        size_t policyGradientScore;
        size_t metaLearningScore;
        float overallPerformance;
        float totalTimeSeconds;
    };
    
    BenchmarkStats getBenchmarkStats() const { return stats; }
    
private:
    std::shared_ptr<Config> config;
    std::shared_ptr<Brain> brain;
    BenchmarkStats stats;
    
    // Individual benchmark instances
    ReconstructionError reconstructionError;
    PredictiveCoding predictiveCoding;
    PolicyGradient policyGradient;
    MetaLearning metaLearning;
    
    // Timing
    std::chrono::high_resolution_clock::time_point startTime;
    
    void initializeBenchmarks();
    void runBenchmarkStep(size_t step);
    void updateBenchmarkStatistics();
};

} // namespace nlm