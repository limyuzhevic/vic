// Benchmark.cpp
// Implementation of performance benchmarking for NLM

#include "Benchmark.hpp"
#include <fstream>
#include <iomanip>
#include <chrono>
#include <algorithm>

namespace nlm {

Benchmark::Benchmark() {}

Benchmark::~Benchmark() {}

Benchmark::Result Benchmark::benchmark(std::function<void()> func, size_t iterations) {
    Result result;
    result.iterations = iterations;
    
    // Warm up
    for (size_t i = 0; i < 100; ++i) {
        func();
    }
    
    // Actual benchmarking
    auto start = std::chrono::high_resolution_clock::now();
    for (size_t i = 0; i < iterations; ++i) {
        func();
    }
    auto end = std::chrono::high_resolution_clock::now();
    
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    double timeMs = static_cast<double>(duration.count()) / 1000.0;
    
    result.timeMs = timeMs;
    result.averageTimeMs = timeMs / iterations;
    
    return result;
}

void Benchmark::saveResults(const std::vector<Result>& results, const std::string& filename) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        return;
    }
    
    file << "=== NLM Benchmark Results ===\n";
    file << "Generated on: " << getCurrentTimestamp() << "\n\n";
    
    file << std::left << std::setw(30) << "Test Name"
         << std::setw(15) << "Iterations"
         << std::setw(15) << "Total Time (ms)"
         << std::setw(20) << "Average Time (ms)"
         << std::setw(15) << "Performance (ops/ms)" << "\n";
    
    file << std::string(100, '-') << "\n";
    
    for (const auto& result : results) {
        double opsPerMs = static_cast<double>(result.iterations) / (result.timeMs / 1000.0);
        file << std::left << std::setw(30) << result.name
             << std::setw(15) << result.iterations
             << std::setw(15) << std::fixed << std::setprecision(6) << result.timeMs
             << std::setw(20) << std::fixed << std::setprecision(6) << result.averageTimeMs
             << std::setw(15) << std::fixed << std::setprecision(2) << opsPerMs << "\n";
    }
    
    file.close();
}

std::string Benchmark::getCurrentTimestamp() {
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    std::stringstream ss;
    ss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
    return ss.str();
}

}} // namespace nlm