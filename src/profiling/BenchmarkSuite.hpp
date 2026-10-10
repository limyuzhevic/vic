// Benchmark Suite Header
#pragma once

#include <string>
#include <vector>
#include <functional>
#include <chrono>

namespace nlm {

/**
 * Benchmark suite for NLM performance testing
 */
class BenchmarkSuite {
public:
    struct BenchmarkData {
        std::string name;
        std::function<void()> function;
        int iterations;
        double totalTime;
    };
    
    BenchmarkSuite();
    ~BenchmarkSuite();
    
    /**
     * Add a benchmark to the suite
     */
    void addBenchmark(const std::string& name, std::function<void()> benchmarkFunction);
    
    /**
     * Run all benchmarks in the suite
     */
    void runBenchmarks();
    
    /**
     * Set default number of iterations for benchmarks
     */
    void setIterations(size_t iterations) {
        _defaultIterations = iterations;
    }
    
private:
    void runBenchmark(BenchmarkData& benchmark);
    void generateReport();
    
    std::vector<BenchmarkData> _benchmarks;
    size_t _defaultIterations;
};

} // namespace nlm