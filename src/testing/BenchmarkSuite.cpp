// Benchmark Suite Implementation for NLM

#include "BenchmarkSuite.hpp"
#include <numeric>
#include <algorithm>
#include <cmath>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <random>
#include <thread>

namespace nlm {
namespace testing {

// BenchmarkSuite implementation

void BenchmarkSuite::registerBenchmark(const std::string& name,
                                      const std::string& description,
                                      std::function<void()> benchmarkFunction,
                                      size_t iterations,
                                      const std::string& unit,
                                      const std::string& category) {
    std::lock_guard<std::mutex> lock(benchmarksMutex_);
    
    Benchmark bench(name, description, benchmarkFunction, iterations, unit, category);
    benchmarks_.push_back(bench);
    
    std::cout << "Registered benchmark: " << name << " (" << category << ")" << std::endl;
}

BenchmarkResult BenchmarkSuite::runBenchmark(const std::string& name) const {
    std::lock_guard<std::mutex> lock(benchmarksMutex_);
    
    for (const auto& benchmark : benchmarks_) {
        if (benchmark.name == name) {
            std::cout << "Running benchmark: " << name << std::endl;
            
            auto start = std::chrono::high_resolution_clock::now();
            benchmark.function();
            auto end = std::chrono::high_resolution_clock::now();
            
            auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start);
            double durationNs = static_cast<double>(duration.count());
            double avgDurationNs = durationNs; // Single iteration for now
            
            // Convert to appropriate unit
            double durationMs = avgDurationNs / 1000000.0;
            
            double durationValue;
            std::string actualUnit;
            
            if (durationMs < 1.0) {
                actualUnit = "us";
                durationValue = avgDurationNs / 1000.0;
            } else if (durationMs < 1000.0) {
                actualUnit = "ms";
                durationValue = durationMs;
            } else {
                actualUnit = "s";
                durationValue = durationMs / 1000.0;
            }
            
            BenchmarkResult result(name, durationValue, 0.0, 1, actualUnit);
            
            // Update benchmark's last result
            const_cast<Benchmark&>(benchmark).lastResult = result;
            
            std::cout << "  Result: " << durationValue << " " << actualUnit << std::endl;
            return result;
        }
    }
    
    std::cout << "Error: Benchmark not found: " << name << std::endl;
    return BenchmarkResult(name, 0.0, 0.0, 0, "ms");
}

void BenchmarkSuite::runAllBenchmarks() {
    std::cout << "=== Running All Benchmarks ===" << std::endl;
    
    for (const auto& benchmark : benchmarks_) {
        runBenchmark(benchmark.name);
    }
    
    std::cout << "All benchmarks completed" << std::endl;
}

std::vector<std::string> BenchmarkSuite::getBenchmarksByCategory(const std::string& category) const {
    std::vector<std::string> result;
    
    std::lock_guard<std::mutex> lock(benchmarksMutex_);
    
    for (const auto& benchmark : benchmarks_) {
        if (benchmark.category == category) {
            result.push_back(benchmark.name);
        }
    }
    
    return result;
}

std::vector<std::string> BenchmarkSuite::getAllBenchmarks() const {
    std::vector<std::string> result;
    
    std::lock_guard<std::mutex> lock(benchmarksMutex_);
    
    for (const auto& benchmark : benchmarks_) {
        result.push_back(benchmark.name);
    }
    
    return result;
}

void BenchmarkSuite::generateBenchmarkReport() const {
    std::cout << "\n=== Benchmark Report ===" << std::endl;
    
    std::cout << "Total benchmarks: " << benchmarks_.size() << std::endl;
    
    // Category summary
    std::unordered_map<std::string, size_t> categoryCounts;
    for (const auto& benchmark : benchmarks_) {
        categoryCounts[benchmark.category]++;
    }
    
    std::cout << "\nBenchmarks by category:" << std::endl;
    for (const auto& pair : categoryCounts) {
        std::cout << "  " << pair.first << ": " << pair.second << std::endl;
    }
    
    // Individual benchmark results
    std::cout << "\nIndividual Benchmark Results:" << std::endl;
    for (const auto& benchmark : benchmarks_) {
        std::cout << "  " << benchmark.name << ": " << benchmark.lastResult.duration 
                  << " " << benchmark.lastResult.unit << std::endl;
    }
}

void BenchmarkSuite::exportBenchmarkResults(const std::string& filename) const {
    std::ofstream file(filename);
    if (!file.is_open()) {
        std::cout << "Error: Cannot open file for writing benchmark results: " << filename << std::endl;
        return;
    }
    
    file << "Benchmark Results Export" << std::endl;
    file << "Generated: " << std::chrono::system_clock::to_time_t(std::chrono::system_clock::now()) << std::endl;
    file << "Total benchmarks: " << benchmarks_.size() << std::endl;
    
    file << "\nBenchmarks:" << std::endl;
    for (const auto& benchmark : benchmarks_) {
        file << "  Name: " << benchmark.name << std::endl;
        file << "  Description: " << benchmark.description << std::endl;
        file << "  Category: " << benchmark.category << std::endl;
        file << "  Iterations: " << benchmark.iterations << std::endl;
        file << "  Unit: " << benchmark.unit << std::endl;
        file << "  Last Result: " << benchmark.lastResult.duration << " " << benchmark.lastResult.unit 
             << " (" << benchmark.lastResult.iterations << " iterations)" << std::endl;
        file << std::endl;
    }
    
    file.close();
    std::cout << "Benchmark results exported to: " << filename << std::endl;
}

void BenchmarkSuite::compareBenchmarks(const std::string& baselineFile) const {
    std::cout << "Comparing benchmarks with baseline: " << baselineFile << std::endl;
    
    // In a real implementation, this would load the baseline and compare
    std::cout << "Benchmark comparison completed (simplified implementation)" << std::endl;
}

} // namespace testing
} // namespace nlm