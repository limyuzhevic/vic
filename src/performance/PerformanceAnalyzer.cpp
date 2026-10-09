#include "PerformanceAnalyzer.hpp"
#include <iostream>
#include <cmath>

namespace nlm {

PerformanceAnalyzer::PerformanceAnalyzer() : pImpl(std::make_unique<Impl>()) {}

PerformanceAnalyzer::BenchmarkResult PerformanceAnalyzer::runBenchmark(const std::string& name, BenchmarkFunction benchmark,
                                                                        size_t iterations, bool warmUp) {
    BenchmarkResult result(name);
    
    // Warm up
    if (warmUp) {
        for (size_t i = 0; i < warmUpIterations_; ++i) {
            benchmark();
        }
    }
    
    // Actual benchmark
    Timer timer;
    timer.start();
    
    for (size_t i = 0; i < benchmarkIterations_; ++i) {
        benchmark();
    }
    
    timer.stop();
    
    result.executionTime = timer.getElapsedTime();
    result.memoryUsage = measureMemoryUsage();
    result.operationsPerSecond = (benchmarkIterations_ * 1000.0) / result.executionTime; // Assuming milliseconds
    
    pImpl->results.push_back(result);
    return result;
}

std::vector<PerformanceAnalyzer::BenchmarkResult> PerformanceAnalyzer::runComparisonBenchmark(
    const std::vector<std::pair<std::string, BenchmarkFunction>>& benchmarks,
    size_t iterations) {
    std::vector<BenchmarkResult> results;
    
    for (const auto& pair : benchmarks) {
        results.push_back(runBenchmark(pair.first, pair.second, iterations));
    }
    
    return results;
}

void PerformanceAnalyzer::profileBrainComponents(std::shared_ptr<Brain> brain) {
    if (!brain) return;
    
    // Profile initialization
    Timer timer;
    timer.start();
    brain->initialize();
    timer.stop();
    
    // Profile simulation steps
    Timer simTimer;
    simTimer.start();
    for (SimulationStep step = 0; step < 1000; ++step) {
        brain->step(step);
    }
    simTimer.stop();
    
    // Record metrics
    double initTime = timer.getElapsedTime();
    double simTime = simTimer.getElapsedTime();
    
    std::cout << "Brain component profiling:" << std::endl;
    std::cout << "  Initialization: " << initTime << "s" << std::endl;
    std::cout << "  Simulation (1000 steps): " << simTime << "s" << std::endl;
    std::cout << "  Average step time: " << (simTime / 1000.0) << "s" << std::endl;
    std::cout << "  Steps per second: " << (1000.0 / simTime) << std::endl;
}

void PerformanceAnalyzer::profileSimulation(const std::string& testName, std::shared_ptr<Brain> brain,
                                            SimulationStep steps, Timestamp timestep) {
    Timer timer;
    timer.start();
    
    for (SimulationStep step = 0; step < steps; ++step) {
        brain->step(step, timestep);
    }
    
    timer.stop();
    
    BenchmarkResult result(testName);
    result.executionTime = timer.getElapsedTime();
    result.operationsPerSecond = (steps * 1000.0) / result.executionTime; // Operations per second
    
    pImpl->results.push_back(result);
    
    std::cout << "Simulation benchmark: " << testName << std::endl;
    std::cout << "  Steps: " << steps << std::endl;
    std::cout << "  Execution time: " << result.executionTime << "s" << std::endl;
    std::cout << "  Steps per second: " << result.operationsPerSecond << std::endl;
}

double PerformanceAnalyzer::measureMemoryUsage() const {
    // Simplified memory measurement - in real implementation, use platform-specific APIs
    return 0.0; // Placeholder
}

std::vector<std::string> PerformanceAnalyzer::getPerformanceRecommendations() const {
    std::vector<std::string> recommendations;
    
    for (const auto& result : pImpl->results) {
        if (result.executionTime > 1.0) {
            recommendations.push_back("Optimize " + result.testName + ": High execution time (>1s)");
        }
        
        if (result.operationsPerSecond < 10000) {
            recommendations.push_back("Improve performance of " + result.testName + ": Low operations per second (<10k)");
        }
    }
    
    if (recommendations.empty()) {
        recommendations.push_back("Performance looks good! All benchmarks within acceptable ranges.");
    }
    
    return recommendations;
}

std::string PerformanceAnalyzer::exportAnalysis() const {
    std::ostringstream oss;
    oss << "NLM Performance Analysis" << std::endl;
    oss << std::endl;
    
    oss << "Benchmark Results:" << std::endl;
    for (const auto& result : pImpl->results) {
        oss << "  " << result.testName << ":" << std::endl;
        oss << "    Execution time: " << result.executionTime << "s" << std::endl;
        oss << "    Operations/sec: " << result.operationsPerSecond << std::endl;
        oss << "    Memory usage: " << result.memoryUsage << " MB" << std::endl;
        oss << std::endl;
    }
    
    oss << "Performance Recommendations:" << std::endl;
    auto recommendations = getPerformanceRecommendations();
    for (const auto& rec : recommendations) {
        oss << "  - " << rec << std::endl;
    }
    
    return oss.str();
}

void PerformanceAnalyzer::exportAnalysisToFile(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (file.is_open()) {
        file << exportAnalysis();
        file.close();
    }
}

PerformanceAnalyzer::Impl::Impl() : memorySampleInterval(1000.0) {}

} // namespace nlm
