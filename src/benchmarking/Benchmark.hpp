#pragma once

// Performance benchmarking interface for NLM
// Provides tools for benchmarking brain performance, simulation efficiency, and comparison

#include <string>
#include <memory>

namespace benchmark {

struct PerformanceMetrics;
struct BenchmarkConfig;

class PerformanceBenchmarker;
class BenchmarkComparator;

// Forward declarations for Python bindings
void bindBenchmark(py::module_ &m);

// Benchmark configuration helper functions
BenchmarkConfig createQuickBenchmark(std::string name, uint64_t duration = 30);
BenchmarkConfig createDetailedBenchmark(std::string name, uint64_t targetSteps = 1000);
BenchmarkConfig createStressBenchmark(std::string name, uint64_t duration = 120);

// Performance analysis helper functions
std::string analyzePerformance(const PerformanceMetrics& metrics);
std::string compareBenchmarks(const std::vector<PerformanceBenchmarker>& benchmarks);

} // namespace benchmark
