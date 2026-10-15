#pragma once

/**
 * Performance Benchmark Module
 * 
 * Comprehensive benchmarking and performance validation for NLM
 * Measures performance of neural simulation operations
 * Validates performance improvements from optimization passes
 */

#include <chrono>
#include <string>
#include <vector>
#include <unordered_map>
#include <memory>
#include <functional>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <iomanip>
#include <cmath>

namespace nlm {

// Benchmark result structure
struct BenchmarkResult {
    std::string test_name;
    std::string test_description;
    double execution_time_ms = 0.0;
    uint64_t operations_performed = 0;
    double throughput_ops_per_second = 0.0;
    std::string unit;
    double memory_usage_mb = 0.0;
    double cpu_usage_percent = 0.0;
    std::string optimization_applied;
    uint64_t iteration = 0;
    uint64_t total_iterations = 1;
    double confidence_interval = 0.0;
    uint64_t timestamp_ns = 0;
    std::string status; // "PASSED", "FAILED", "SKIPPED"
    std::string error_message;
};

// Benchmark configuration
struct BenchmarkConfig {
    bool enable_benchmarks = true;
    uint32_t warmup_iterations = 10;
    uint32_t measurement_iterations = 100;
    std::string output_file = "";
    std::string report_format = "detailed"; // "detailed", "summary", "json"
    bool enable_validation = true;
    double min_throughput_ops_per_second = 0.0;
    double max_memory_usage_mb = 0.0;
    bool enable_cross_platform_comparison = false;
};

// Benchmark test base class
class BenchmarkTest {
public:
    virtual ~BenchmarkTest() = default;
    virtual std::string getName() const = 0;
    virtual std::string getDescription() const = 0;
    virtual void setup() {}
    virtual void teardown() {}
    virtual void run(uint32_t iteration) = 0;
    virtual std::string getUnit() const = 0;
    virtual std::string getOptimizationApplied() const = 0;
};

/**
 * Neural Simulation Performance Benchmark Suite
 */
class NeuralSimulationBenchmark {
private:
    BenchmarkConfig config;
    std::vector<BenchmarkResult> results;
    std::vector<std::unique_ptr<BenchmarkTest>> tests;
    bool initialized = false;
    uint64_t iteration_counter = 0;

public:
    NeuralSimulationBenchmark(const BenchmarkConfig& config = BenchmarkConfig());
    ~NeuralSimulationBenchmark();
    
    // Test registration
    void registerTest(std::unique_ptr<BenchmarkTest> test);
    void unregisterTest(const std::string& name);
    
    // Benchmark execution
    void runAllBenchmarks();
    BenchmarkResult runBenchmark(const std::string& test_name);
    void warmupBenchmark(const std::string& test_name);
    
    // Configuration
    void configure(const BenchmarkConfig& new_config);
    
    // Results management
    std::vector<BenchmarkResult> getResults() const;
    BenchmarkResult getResult(const std::string& test_name, uint32_t iteration = 0) const;
    void clearResults();
    
    // Report generation
    void generateReport(const std::string& filename) const;
    void generateDetailedReport(const std::string& filename) const;
    void generateSummaryReport(const std::string& filename) const;
    void generateJSONReport(const std::string& filename) const;
    
    // Utilities
    void printResults() const;
    double calculateAverageExecutionTime(const std::string& test_name) const;
    double calculateThroughput(const std::string& test_name) const;
    bool validateResults(const BenchmarkResult& result) const;
    
    // State management
    void reset();
    bool isInitialized() const { return initialized; }
    void setInitialized(bool value) { initialized = value; }
};

/**
 * Neural Network Creation Benchmark
 */
class NeuralNetworkCreationBenchmark : public BenchmarkTest {
private:
    uint32_t neuron_count = 1000;
    uint32_t connection_probability = 50;
    uint32_t warmup_steps = 100;

public:
    std::string getName() const override { return "NeuralNetworkCreation"; }
    std::string getDescription() const override { 
        return "Benchmark neural network creation performance"; 
    }
    void setup() override {}
    void teardown() override {}
    void run(uint32_t iteration) override;
    std::string getUnit() const override { return "neurons_per_second"; }
    std::string getOptimizationApplied() const override { return "Memory pool allocation and optimized data structures"; }
    
    void configure(uint32_t neurons, uint32_t connection_prob, uint32_t warmup) {
        neuron_count = neurons;
        connection_probability = connection_prob;
        warmup_steps = warmup;
    }
};

/**
 * Simulation Step Benchmark
 */
class SimulationStepBenchmark : public BenchmarkTest {
private:
    uint32_t steps_per_iteration = 100;
    uint32_t neuron_count = 5000;
    uint32_t connection_probability = 20;

public:
    std::string getName() const override { return "SimulationStep"; }
    std::string getDescription() const override { 
        return "Benchmark single simulation step performance"; 
    }
    void setup() override {}
    void teardown() override {}
    void run(uint32_t iteration) override;
    std::string getUnit() const override { return "steps_per_second"; }
    std::string getOptimizationApplied() const override { return "SoA data layout, SIMD vectorization, and multithreading"; }
    
    void configure(uint32_t steps, uint32_t neurons, uint32_t connection_prob) {
        steps_per_iteration = steps;
        neuron_count = neurons;
        connection_probability = connection_prob;
    }
};

/**
 * Memory Access Benchmark
 */
class MemoryAccessBenchmark : public BenchmarkTest {
private:
    uint32_t access_count = 100000;
    uint32_t neuron_count = 10000;

public:
    std::string getName() const override { return "MemoryAccess"; }
    std::string getDescription() const override { 
        return "Benchmark memory access patterns and cache efficiency"; 
    }
    void setup() override {}
    void teardown() override {}
    void run(uint32_t iteration) override;
    std::string getUnit() const override { return "accesses_per_second"; }
    std::string getOptimizationApplied() const override { return "Structure-of-Arrays and memory pool allocators"; }
    
    void configure(uint32_t accesses, uint32_t neurons) {
        access_count = accesses;
        neuron_count = neurons;
    }
};

/**
 * Spike Processing Benchmark
 */
class SpikeProcessingBenchmark : public BenchmarkTest {
private:
    uint32_t spike_count = 50000;
    uint32_t neuron_count = 20000;
    uint32_t connections_per_spike = 100;

public:
    std::string getName() const override { return "SpikeProcessing"; }
    std::string getDescription() const override { 
        return "Benchmark spike processing performance"; 
    }
    void setup() override {}
    void teardown() override {}
    void run(uint32_t iteration) override;
    std::string getUnit() const override { return "spikes_per_second"; }
    std::string getOptimizationApplied() const override { return "Event queue optimization and lock-free buffers"; }
    
    void configure(uint32_t spikes, uint32_t neurons, uint32_t connections) {
        spike_count = spikes;
        neuron_count = neurons;
        connections_per_spike = connections;
    }
};

/**
 * Multithreading Benchmark
 */
class MultithreadingBenchmark : public BenchmarkTest {
private:
    uint32_t thread_count = 4;
    uint32_t steps_per_iteration = 100;
    uint32_t neurons_per_thread = 5000;

public:
    std::string getName() const override { return "Multithreading"; }
    std::string getDescription() const override { 
        return "Benchmark parallel processing performance across multiple threads"; 
    }
    void setup() override {}
    void teardown() override {}
    void run(uint32_t iteration) override;
    std::string getUnit() const override { return "steps_per_second_per_thread"; }
    std::string getOptimizationApplied() const override { return "Thread-local processing and parallel population updates"; }
    
    void configure(uint32_t threads, uint32_t steps, uint32_t neurons) {
        thread_count = threads;
        steps_per_iteration = steps;
        neurons_per_thread = neurons;
    }
};

/**
 * Performance Comparison Benchmark
 */
class PerformanceComparisonBenchmark : public BenchmarkTest {
private:
    std::string baseline_version;
    std::string current_version;
    uint32_t iteration_count = 50;

public:
    std::string getName() const override { return "PerformanceComparison"; }
    std::string getDescription() const override { 
        return "Compare performance between baseline and optimized versions"; 
    }
    void setup() override {}
    void teardown() override {}
    void run(uint32_t iteration) override;
    std::string getUnit() const override { return "improvement_factor"; }
    std::string getOptimizationApplied() const override { return "All performance optimizations from Phase 5"; }
    
    void setVersions(const std::string& baseline, const std::string& current) {
        baseline_version = baseline;
        current_version = current;
    }
};

} // namespace nlm