#include "PerformanceBenchmark.hpp"
#include <chrono>
#include <fstream>
#include <iostream>
#include <numeric>
#include <algorithm>
#include <cmath>

namespace nlm {

NeuralSimulationBenchmark::NeuralSimulationBenchmark(const BenchmarkConfig& config) : config(config) {
}

NeuralSimulationBenchmark::~NeuralSimulationBenchmark() {
}

void NeuralSimulationBenchmark::registerTest(std::unique_ptr<BenchmarkTest> test) {
    tests.push_back(std::move(test));
}

void NeuralSimulationBenchmark::unregisterTest(const std::string& name) {
    auto it = std::remove_if(tests.begin(), tests.end(),
        [&name](const std::unique_ptr<BenchmarkTest>& test) {
            return test->getName() == name;
        });
    tests.erase(it, tests.end());
}

void NeuralSimulationBenchmark::runAllBenchmarks() {
    for (auto& test : tests) {
        std::cout << "Running benchmark: " << test->getName() << " - " << test->getDescription() << std::endl;
        warmupBenchmark(test->getName());
        BenchmarkResult result;
        for (uint32_t iteration = 0; iteration < config.measurement_iterations; ++iteration) {
            auto start_time = std::chrono::high_resolution_clock::now();
            test->run(iteration);
            auto end_time = std::chrono::high_resolution_clock::now();
            
            auto duration_ns = std::chrono::duration_cast<std::chrono::nanoseconds>(end_time - start_time).count();
            double execution_time_ms = duration_ns / 1000000.0;
            
            result.test_name = test->getName();
            result.test_description = test->getDescription();
            result.execution_time_ms += execution_time_ms;
            result.optimization_applied = test->getOptimizationApplied();
            result.iteration = iteration;
            result.total_iterations = config.measurement_iterations;
            result.timestamp_ns = std::chrono::steady_clock::now().time_since_epoch().count();
            result.status = "PASSED";
        }
        
        result.execution_time_ms /= config.measurement_iterations;
        result.unit = test->getUnit();
        result.throughput_ops_per_second = calculateThroughput(test->getName());
        result.status = validateResults(result) ? "PASSED" : "FAILED";
        
        results.push_back(result);
    }
}

BenchmarkResult NeuralSimulationBenchmark::runBenchmark(const std::string& test_name) {
    BenchmarkResult result;
    result.test_name = test_name;
    result.status = "FAILED";
    
    for (auto& test : tests) {
        if (test->getName() == test_name) {
            warmupBenchmark(test_name);
            auto start_time = std::chrono::high_resolution_clock::now();
            test->run(0);
            auto end_time = std::chrono::high_resolution_clock::now();
            
            auto duration_ns = std::chrono::duration_cast<std::chrono::nanoseconds>(end_time - start_time).count();
            double execution_time_ms = duration_ns / 1000000.0;
            
            result.test_name = test->getName();
            result.test_description = test->getDescription();
            result.execution_time_ms = execution_time_ms;
            result.optimization_applied = test->getOptimizationApplied();
            result.unit = test->getUnit();
            result.throughput_ops_per_second = calculateThroughput(test_name);
            result.iteration = 0;
            result.total_iterations = 1;
            result.timestamp_ns = std::chrono::steady_clock::now().time_since_epoch().count();
            result.status = validateResults(result) ? "PASSED" : "FAILED";
            
            results.push_back(result);
            break;
        }
    }
    
    return result;
}

void NeuralSimulationBenchmark::warmupBenchmark(const std::string& test_name) {
    for (auto& test : tests) {
        if (test->getName() == test_name) {
            for (uint32_t iteration = 0; iteration < config.warmup_iterations; ++iteration) {
                test->run(iteration);
            }
            break;
        }
    }
}

void NeuralSimulationBenchmark::configure(const BenchmarkConfig& new_config) {
    config = new_config;
}

std::vector<BenchmarkResult> NeuralSimulationBenchmark::getResults() const {
    return results;
}

BenchmarkResult NeuralSimulationBenchmark::getResult(const std::string& test_name, uint32_t iteration) const {
    for (const auto& result : results) {
        if (result.test_name == test_name && (iteration == 0 || result.iteration == iteration)) {
            return result;
        }
    }
    return BenchmarkResult();
}

void NeuralSimulationBenchmark::clearResults() {
    results.clear();
}

void NeuralSimulationBenchmark::generateReport(const std::string& filename) const {
    if (config.output_file.empty()) {
        config.output_file = filename;
    }
    
    if (config.report_format == "detailed") {
        generateDetailedReport(config.output_file);
    } else if (config.report_format == "summary") {
        generateSummaryReport(config.output_file);
    } else if (config.report_format == "json") {
        generateJSONReport(config.output_file);
    }
}

void NeuralSimulationBenchmark::generateDetailedReport(const std::string& filename) const {
    std::ofstream file(filename);
    if (!file.is_open()) return;
    
    file << "NLM Performance Benchmark Report" << std::endl;
    file << "=================================" << std::endl;
    file << "Generated at: " << std::chrono::system_clock::now().time_since_epoch().count() << std::endl;
    file << std::endl;
    
    for (const auto& result : results) {
        file << "Test: " << result.test_name << std::endl;
        file << "Description: " << result.test_description << std::endl;
        file << "Optimization Applied: " << result.optimization_applied << std::endl;
        file << "Execution Time: " << std::fixed << std::setprecision(3) << result.execution_time_ms << " ms" << std::endl;
        file << "Throughput: " << std::fixed << std::setprecision(2) << result.throughput_ops_per_second << " " << result.unit << "/s" << std::endl;
        file << "Status: " << result.status << std::endl;
        if (!result.error_message.empty()) {
            file << "Error: " << result.error_message << std::endl;
        }
        file << std::endl;
    }
    
    file.close();
}

void NeuralSimulationBenchmark::generateSummaryReport(const std::string& filename) const {
    std::ofstream file(filename);
    if (!file.is_open()) return;
    
    file << "NLM Performance Summary" << std::endl;
    file << "=======================" << std::endl;
    
    for (const auto& result : results) {
        file << result.test_name << ": " << result.status << ", " 
             << std::fixed << std::setprecision(2) << result.throughput_ops_per_second << " " << result.unit << "/s" << std::endl;
    }
    
    file.close();
}

void NeuralSimulationBenchmark::generateJSONReport(const std::string& filename) const {
    std::ofstream file(filename);
    if (!file.is_open()) return;
    
    file << "{\n  \"benchmark_results\": [" << std::endl;
    
    for (size_t i = 0; i < results.size(); ++i) {
        const auto& result = results[i];
        file << "    {\"test_name\": \"" << result.test_name << "\", \""
             << "execution_time_ms\": " << std::fixed << std::setprecision(3) << result.execution_time_ms << "\"";
        if (i < results.size() - 1) {
            file << ",\" << std::endl;
        } else {
            file << std::endl;
        }
    }
    
    file << "  ]\n}";
    
    file.close();
}

void NeuralSimulationBenchmark::printResults() const {
    std::cout << "Performance Benchmark Results:" << std::endl;
    std::cout << "=============================" << std::endl;
    
    for (const auto& result : results) {
        std::cout << result.test_name << ": " << std::fixed << std::setprecision(3) << result.execution_time_ms << " ms, "
                  << std::fixed << std::setprecision(2) << result.throughput_ops_per_second << " " << result.unit << "/s, "
                  << result.status << std::endl;
    }
}

double NeuralSimulationBenchmark::calculateAverageExecutionTime(const std::string& test_name) const {
    std::vector<double> times;
    for (const auto& result : results) {
        if (result.test_name == test_name) {
            times.push_back(result.execution_time_ms);
        }
    }
    
    if (times.empty()) return 0.0;
    
    double sum = std::accumulate(times.begin(), times.end(), 0.0);
    return sum / times.size();
}

double NeuralSimulationBenchmark::calculateThroughput(const std::string& test_name) const {
    for (const auto& result : results) {
        if (result.test_name == test_name) {
            return result.throughput_ops_per_second;
        }
    }
    return 0.0;
}

bool NeuralSimulationBenchmark::validateResults(const BenchmarkResult& result) const {
    if (!config.enable_validation) return true;
    
    if (config.min_throughput_ops_per_second > 0 && 
        result.throughput_ops_per_second < config.min_throughput_ops_per_second) {
        result.status = "FAILED - Throughput below minimum";
        result.error_message = "Throughput is " + std::to_string(result.throughput_ops_per_second) + 
                              " which is below minimum of " + std::to_string(config.min_throughput_ops_per_second);
        return false;
    }
    
    if (config.max_memory_usage_mb > 0 && 
        result.memory_usage_mb > config.max_memory_usage_mb) {
        result.status = "FAILED - Memory usage above maximum";
        result.error_message = "Memory usage is " + std::to_string(result.memory_usage_mb) + 
                              " MB which is above maximum of " + std::to_string(config.max_memory_usage_mb) + " MB";
        return false;
    }
    
    return true;
}

void NeuralSimulationBenchmark::reset() {
    results.clear();
    iteration_counter = 0;
}

// Neural Network Creation Benchmark Implementation
void NeuralNetworkCreationBenchmark::run(uint32_t iteration) {
    auto start_time = std::chrono::high_resolution_clock::now();
    
    // Simulate neural network creation
    // In real implementation, this would create a neural network with the specified parameters
    volatile double dummy = 0.0; // Prevent optimization
    for (uint32_t i = 0; i < neuron_count * 2; ++i) {
        dummy += i * 0.1;
    }
    
    auto end_time = std::chrono::high_resolution_clock::now();
    auto duration_ns = std::chrono::duration_cast<std::chrono::nanoseconds>(end_time - start_time).count();
    
    // Update performance metrics
    // This would be passed to the performance monitor in real implementation
}

// Simulation Step Benchmark Implementation
void SimulationStepBenchmark::run(uint32_t iteration) {
    auto start_time = std::chrono::high_resolution_clock::now();
    
    // Simulate simulation steps
    volatile double dummy = 0.0; // Prevent optimization
    for (uint32_t i = 0; i < steps_per_iteration * 100; ++i) {
        dummy += i * 0.01;
    }
    
    auto end_time = std::chrono::high_resolution_clock::now();
    auto duration_ns = std::chrono::duration_cast<std::chrono::nanoseconds>(end_time - start_time).count();
}

// Memory Access Benchmark Implementation
void MemoryAccessBenchmark::run(uint32_t iteration) {
    auto start_time = std::chrono::high_resolution_clock::now();
    
    // Simulate memory access patterns
    std::vector<double> memory_block(neuron_count, 0.0);
    volatile double result = 0.0; // Prevent optimization
    
    for (uint32_t i = 0; i < access_count; ++i) {
        size_t index = i % neuron_count;
        result += memory_block[index];
    }
    
    auto end_time = std::chrono::high_resolution_clock::now();
    auto duration_ns = std::chrono::duration_cast<std::chrono::nanoseconds>(end_time - start_time).count();
}

// Spike Processing Benchmark Implementation
void SpikeProcessingBenchmark::run(uint32_t iteration) {
    auto start_time = std::chrono::high_resolution_clock::now();
    
    // Simulate spike processing
    volatile double dummy = 0.0; // Prevent optimization
    for (uint32_t i = 0; i < spike_count * connections_per_spike; ++i) {
        dummy += i * 0.001;
    }
    
    auto end_time = std::chrono::high_resolution_clock::now();
    auto duration_ns = std::chrono::duration_cast<std::chrono::nanoseconds>(end_time - start_time).count();
}

// Multithreading Benchmark Implementation
void MultithreadingBenchmark::run(uint32_t iteration) {
    auto start_time = std::chrono::high_resolution_clock::now();
    
    // Simulate multithreaded processing
    volatile double dummy = 0.0; // Prevent optimization
    for (uint32_t i = 0; i < steps_per_iteration * 1000; ++i) {
        dummy += i * 0.0001;
    }
    
    auto end_time = std::chrono::high_resolution_clock::now();
    auto duration_ns = std::chrono::duration_cast<std::chrono::nanoseconds>(end_time - start_time).count();
}

// Performance Comparison Benchmark Implementation
void PerformanceComparisonBenchmark::run(uint32_t iteration) {
    auto start_time = std::chrono::high_resolution_clock::now();
    
    // Simulate performance comparison
    volatile double dummy = 0.0; // Prevent optimization
    for (uint32_t i = 0; i < iteration_count * 1000; ++i) {
        dummy += i * 0.0001;
    }
    
    auto end_time = std::chrono::high_resolution_clock::now();
    auto duration_ns = std::chrono::duration_cast<std::chrono::nanoseconds>(end_time - start_time).count();
}

} // namespace nlm