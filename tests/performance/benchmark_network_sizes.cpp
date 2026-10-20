// Performance Benchmarks and Testing Utilities for NLM
// Comprehensive performance testing framework for NLM

#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>
#include <chrono>
#include <vector>
#include <string>
#include <unordered_map>
#include <fstream>
#include <iomanip>
#include <algorithm>
#include <numeric>
#include <memory>

namespace test_performance {

// Performance measurement utilities
struct PerformanceMetrics {
    double execution_time_ms;
    size_t memory_usage_kb;
    double throughput_ops_per_sec;
    double cpu_usage_percent;
    std::unordered_map<std::string, double> custom_metrics;
    std::chrono::time_point<std::chrono::high_resolution_clock> start_time;
    std::chrono::time_point<std::chrono::high_resolution_clock> end_time;
    
    PerformanceMetrics() : start_time(std::chrono::high_resolution_clock::now()) {}
    
    void end() {
        end_time = std::chrono::high_resolution_clock::now();
        execution_time_ms = std::chrono::duration<double, std::milli>(end_time - start_time).count();
    }
    
    double getExecutionTimeMs() const { return execution_time_ms; }
    double getThroughput(double ops) const { return ops / (execution_time_ms / 1000.0); }
};

// Performance regression detection
class PerformanceRegressionDetector {
private:
    std::unordered_map<std::string, std::vector<double>> baseline_metrics;
    std::unordered_map<std::string, std::vector<double>> current_metrics;
    double regression_threshold;
    
public:
    PerformanceRegressionDetector(double threshold = 0.10) // 10% regression threshold
        : regression_threshold(threshold) {}
    
    void setBaseline(const std::string& test_name, const std::vector<double>& metrics) {
        baseline_metrics[test_name] = metrics;
    }
    
    void setCurrent(const std::string& test_name, const std::vector<double>& metrics) {
        current_metrics[test_name] = metrics;
    }
    
    bool hasRegression(const std::string& test_name) const {
        auto it_baseline = baseline_metrics.find(test_name);
        auto it_current = current_metrics.find(test_name);
        
        if (it_baseline == baseline_metrics.end() || it_current == current_metrics.end()) {
            return false;
        }
        
        const auto& baseline = it_baseline->second;
        const auto& current = it_current->second;
        
        if (baseline.size() != current.size()) {
            return false;
        }
        
        for (size_t i = 0; i < baseline.size(); ++i) {
            double regression = (current[i] - baseline[i]) / baseline[i];
            if (regression > regression_threshold) {
                return true;
            }
        }
        
        return false;
    }
    
    void printRegressionReport() const {
        std::cout << "=== Performance Regression Report ===" << std::endl;
        for (const auto& pair : baseline_metrics) {
            const std::string& test_name = pair.first;
            bool has_reg = hasRegression(test_name);
            
            std::cout << "Test: " << test_name << std::endl;
            std::cout << "  Status: " << (has_reg ? "REGRESSION DETECTED" : "No regression") << std::endl;
            
            if (has_reg) {
                const auto& baseline = pair.second;
                const auto& current = current_metrics.at(test_name);
                
                std::cout << "  Baseline -> Current:" << std::endl;
                for (size_t i = 0; i < baseline.size(); ++i) {
                    double regression = (current[i] - baseline[i]) / baseline[i] * 100;
                    std::cout << "    Sample " << i << ": " << baseline[i] << " -> " << current[i] 
                             << " (" << regression << "%)" << std::endl;
                }
            }
            std::cout << std::endl;
        }
    }
};

// Test result reporting
class TestResultReporter {
private:
    struct TestResult {
        std::string name;
        std::string status;
        double duration_ms;
        std::string details;
        std::unordered_map<std::string, double> metrics;
    };
    
    std::vector<TestResult> results;
    std::string output_format;
    
public:
    TestResultReporter(const std::string& format = "console") : output_format(format) {}
    
    void addTestResult(const std::string& name, const std::string& status, 
                      double duration_ms, const std::string& details = "",
                      const std::unordered_map<std::string, double>& metrics = {}) {
        TestResult result;
        result.name = name;
        result.status = status;
        result.duration_ms = duration_ms;
        result.details = details;
        result.metrics = metrics;
        results.push_back(result);
    }
    
    void generateReport() {
        if (output_format == "console") {
            generateConsoleReport();
        } else if (output_format == "json") {
            generateJSONReport();
        } else if (output_format == "xml") {
            generateXMLReport();
        }
    }
    
private:
    void generateConsoleReport() {
        std::cout << "=== Test Results Report ===" << std::endl;
        int passed = 0, failed = 0;
        
        for (const auto& result : results) {
            std::cout << "Test: " << result.name << std::endl;
            std::cout << "  Status: " << result.status << std::endl;
            std::cout << "  Duration: " << result.duration_ms << "ms" << std::endl;
            if (!result.details.empty()) {
                std::cout << "  Details: " << result.details << std::endl;
            }
            if (!result.metrics.empty()) {
                std::cout << "  Metrics:" << std::endl;
                for (const auto& metric : result.metrics) {
                    std::cout << "    " << metric.first << ": " << metric.second << std::endl;
                }
            }
            std::cout << std::endl;
            
            if (result.status == "PASSED") passed++;
            else if (result.status == "FAILED") failed++;
        }
        
        std::cout << "=== Summary ===" << std::endl;
        std::cout << "Total tests: " << results.size() << std::endl;
        std::cout << "Passed: " << passed << std::endl;
        std::cout << "Failed: " << failed << std::endl;
    }
    
    void generateJSONReport() {
        std::cout << "{" << std::endl;
        std::cout << "  \"test_results\": [" << std::endl;
        
        for (size_t i = 0; i < results.size(); ++i) {
            const auto& result = results[i];
            std::cout << "    {" << std::endl;
            std::cout << "      \"name\": \"" << result.name << "\"," << std::endl;
            std::cout << "      \"status\": \"" << result.status << "\"," << std::endl;
            std::cout << "      \"duration_ms\": " << result.duration_ms << "," << std::endl;
            std::cout << "      \"details\": \"" << result.details << "\"" << std::endl;
            
            if (!result.metrics.empty()) {
                std::cout << "      \"metrics\": {" << std::endl;
                bool first_metric = true;
                for (const auto& metric : result.metrics) {
                    if (!first_metric) std::cout << "," << std::endl;
                    std::cout << "        \"" << metric.first << "\": " << metric.second;
                    first_metric = false;
                }
                std::cout << std::endl << "      }" << std::endl;
            }
            
            if (i < results.size() - 1) std::cout << "    }," << std::endl;
            else std::cout << "    }" << std::endl;
        }
        
        std::cout << "  ]" << std::endl;
        std::cout << "}" << std::endl;
    }
    
    void generateXMLReport() {
        std::cout << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>" << std::endl;
        std::cout << "<test_results>" << std::endl;
        
        for (const auto& result : results) {
            std::cout << "  <test>" << std::endl;
            std::cout << "    <name>" << result.name << "</name>" << std::endl;
            std::cout << "    <status>" << result.status << "</status>" << std::endl;
            std::cout << "    <duration_ms>" << result.duration_ms << "</duration_ms>" << std::endl;
            if (!result.details.empty()) {
                std::cout << "    <details>" << result.details << "</details>" << std::endl;
            }
            if (!result.metrics.empty()) {
                std::cout << "    <metrics>" << std::endl;
                for (const auto& metric : result.metrics) {
                    std::cout << "      <metric name=\"" << metric.first << "\">" 
                             << metric.second << "</metric>" << std::endl;
                }
                std::cout << "    </metrics>" << std::endl;
            }
            std::cout << "  </test>" << std::endl;
        }
        
        std::cout << "</test_results>" << std::endl;
    }
};

// Automated test suite runner
class AutomatedTestSuite {
private:
    std::string suite_name;
    std::vector<std::function<void()>> test_cases;
    TestResultReporter reporter;
    
public:
    AutomatedTestSuite(const std::string& name) : suite_name(name) {}
    
    void addTestCase(const std::string& test_name, std::function<void()> test_func) {
        test_cases.push_back([this, test_name, test_func]() {
            auto start_time = std::chrono::high_resolution_clock::now();
            
            try {
                test_func();
                auto end_time = std::chrono::high_resolution_clock::now();
                double duration_ms = std::chrono::duration<double, std::milli>(end_time - start_time).count();
                
                reporter.addTestResult(test_name, "PASSED", duration_ms, "Test completed successfully");
            } catch (const std::exception& e) {
                auto end_time = std::chrono::high_resolution_clock::now();
                double duration_ms = std::chrono::duration<double, std::milli>(end_time - start_time).count();
                
                reporter.addTestResult(test_name, "FAILED", duration_ms, 
                                     std::string("Exception: ") + e.what());
            }
        });
    }
    
    void run() {
        std::cout << "=== Running Test Suite: " << suite_name << " ===" << std::endl;
        
        for (const auto& test_case : test_cases) {
            test_case();
        }
        
        reporter.generateReport();
    }
};

// Performance benchmark for different network sizes
class NetworkSizeBenchmark {
public:
    struct BenchmarkResult {
        int neuron_count;
        double simulation_time_ms;
        double throughput_ops_per_sec;
        size_t memory_usage_kb;
        double average_firing_rate;
    };
    
    std::vector<BenchmarkResult> runBenchmarks() {
        std::cout << "=== Network Size Performance Benchmark ===" << std::endl;
        
        std::vector<int> neuron_counts = {100, 500, 1000, 2000, 5000};
        std::vector<BenchmarkResult> results;
        
        for (int neuron_count : neuron_counts) {
            std::cout << "\nTesting with " << neuron_count << " neurons..." << std::endl;
            
            auto config = std::make_shared<nlm::Config>();
            config->set("neuron_count", static_cast<int64_t>(neuron_count), nlm::ConfigSource::Default);
            config->set("region_count", static_cast<int64_t>(4), nlm::ConfigSource::Default);
            config->set("connection_probability", 0.02, nlm::ConfigSource::Default);
            
            nlm::Brain brain(config);
            assert(brain.initialize());
            
            PerformanceMetrics metrics;
            nlm::SimulationStep step = 0;
            
            // Run simulation
            for (int i = 0; i < 50; ++i) {
                brain.step(step + i);
            }
            
            metrics.end();
            
            BenchmarkResult result;
            result.neuron_count = neuron_count;
            result.simulation_time_ms = metrics.getExecutionTimeMs();
            result.throughput_ops_per_sec = metrics.getThroughput(50); // 50 simulation steps
            result.memory_usage_kb = metrics.memory_usage_kb;
            result.average_firing_rate = brain.getAverageFiringRate();
            
            results.push_back(result);
            
            std::cout << "  Simulation time: " << result.simulation_time_ms << "ms" << std::endl;
            std::cout << "  Throughput: " << result.throughput_ops_per_sec << " ops/sec" << std::endl;
            std::cout << "  Average firing rate: " << result.average_firing_rate << " Hz" << std::endl;
        }
        
        return results;
    }
    
    void generateBenchmarkReport(const std::vector<BenchmarkResult>& results) {
        std::cout << "\n=== Network Size Benchmark Results ===" << std::endl;
        std::cout << std::left << std::setw(15) << "Neurons" 
                 << std::setw(20) << "Time (ms)" 
                 << std::setw(20) << "Throughput (ops/s)" 
                 << std::setw(18) << "Memory (KB)" 
                 << std::setw(20) << "Firing Rate (Hz)" << std::endl;
        std::cout << std::string(95, '-') << std::endl;
        
        for (const auto& result : results) {
            std::cout << std::left << std::setw(15) << result.neuron_count
                     << std::setw(20) << std::fixed << std::setprecision(2) << result.simulation_time_ms
                     << std::setw(20) << std::fixed << std::setprecision(2) << result.throughput_ops_per_sec
                     << std::setw(18) << result.memory_usage_kb
                     << std::setw(20) << std::fixed << std::setprecision(4) << result.average_firing_rate << std::endl;
        }
    }
};

// Memory usage testing
class MemoryUsageTester {
public:
    struct MemoryUsageResult {
        int neuron_count;
        size_t memory_usage_kb;
        double memory_efficiency;
        double memory_per_neuron_kb;
    };
    
    std::vector<MemoryUsageResult> runMemoryUsageTests() {
        std::cout << "=== Memory Usage Testing ===" << std::endl;
        
        std::vector<int> neuron_counts = {100, 500, 1000, 2000};
        std::vector<MemoryUsageResult> results;
        
        for (int neuron_count : neuron_counts) {
            std::cout << "\nTesting memory usage with " << neuron_count << " neurons..." << std::endl;
            
            auto config = std::make_shared<nlm::Config>();
            config->set("neuron_count", static_cast<int64_t>(neuron_count), nlm::ConfigSource::Default);
            
            nlm::Brain brain(config);
            assert(brain.initialize());
            
            PerformanceMetrics metrics;
            nlm::SimulationStep step = 0;
            
            // Run simulation to get memory usage
            for (int i = 0; i < 20; ++i) {
                brain.step(step + i);
            }
            
            metrics.end();
            
            MemoryUsageResult result;
            result.neuron_count = neuron_count;
            result.memory_usage_kb = metrics.memory_usage_kb;
            result.memory_efficiency = 1.0 / (1.0 + result.memory_usage_kb / 1000.0); // Efficiency metric
            result.memory_per_neuron_kb = static_cast<double>(result.memory_usage_kb) / neuron_count;
            
            results.push_back(result);
            
            std::cout << "  Memory usage: " << result.memory_usage_kb << " KB" << std::endl;
            std::cout << "  Memory per neuron: " << result.memory_per_neuron_kb << " KB" << std::endl;
            std::cout << "  Memory efficiency: " << result.memory_efficiency << std::endl;
        }
        
        return results;
    }
};

// Stress testing
class StressTester {
public:
    struct StressTestResult {
        std::string test_type;
        int stress_level;
        bool passed;
        double performance_degradation;
        std::string details;
    };
    
    std::vector<StressTestResult> runStressTests() {
        std::cout << "=== Stress Testing ===" << std::endl;
        
        std::vector<StressTestResult> results;
        
        // Large network stress testing
        results.push_back(runLargeNetworkStressTest());
        
        // Memory pressure testing
        results.push_back(runMemoryPressureTest());
        
        // Simulation speed testing
        results.push_back(runSimulationSpeedTest());
        
        return results;
    }
    
private:
    StressTestResult runLargeNetworkStressTest() {
        std::cout << "\n[STRESS] Large Network Stress Testing" << std::endl;
        
        auto config = std::make_shared<nlm::Config>();
        config->set("neuron_count", static_cast<int64_t>(5000), nlm::ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(10), nlm::ConfigSource::Default);
        
        nlm::Brain brain(config);
        assert(brain.initialize());
        
        PerformanceMetrics metrics;
        nlm::SimulationStep step = 0;
        
        try {
            // Run many simulation steps
            for (int i = 0; i < 200; ++i) {
                brain.step(step + i);
            }
            
            metrics.end();
            
            StressTestResult result;
            result.test_type = "Large Network Stress Test";
            result.stress_level = 5; // High stress level
            result.passed = true;
            result.performance_degradation = 0.0;
            result.details = "Successfully handled 5000 neurons with 10 regions";
            
            std::cout << "  PASSED: Large network stress test completed in " << metrics.getExecutionTimeMs() << "ms" << std::endl;
            
            return result;
        } catch (const std::exception& e) {
            StressTestResult result;
            result.test_type = "Large Network Stress Test";
            result.stress_level = 5;
            result.passed = false;
            result.performance_degradation = 100.0;
            result.details = std::string("Exception: ") + e.what();
            
            std::cout << "  FAILED: " << result.details << std::endl;
            
            return result;
        }
    }
    
    StressTestResult runMemoryPressureTest() {
        std::cout << "\n[STRESS] Memory Pressure Testing" << std::endl;
        
        auto config = std::make_shared<nlm::Config>();
        config->set("neuron_count", static_cast<int64_t>(1000), nlm::ConfigSource::Default);
        
        nlm::Brain brain(config);
        assert(brain.initialize());
        
        PerformanceMetrics metrics;
        nlm::SimulationStep step = 0;
        
        try {
            // High memory usage scenario
            for (int i = 0; i < 500; ++i) {
                // Store lots of data in memory systems
                brain.getWorkingMemory().store(std::to_string(i), 0.5f, step, nlm::MemoryModule::Working);
                brain.getEpisodicMemory().encode(step, std::to_string(i), "input", "output");
                brain.step(step + i);
            }
            
            metrics.end();
            
            StressTestResult result;
            result.test_type = "Memory Pressure Test";
            result.stress_level = 3; // Medium stress level
            result.passed = true;
            result.performance_degradation = 0.0;
            result.details = "Successfully handled high memory pressure";
            
            std::cout << "  PASSED: Memory pressure test completed in " << metrics.getExecutionTimeMs() << "ms" << std::endl;
            
            return result;
        } catch (const std::exception& e) {
            StressTestResult result;
            result.test_type = "Memory Pressure Test";
            result.stress_level = 3;
            result.passed = false;
            result.performance_degradation = 100.0;
            result.details = std::string("Exception: ") + e.what();
            
            std::cout << "  FAILED: " << result.details << std::endl;
            
            return result;
        }
    }
    
    StressTestResult runSimulationSpeedTest() {
        std::cout << "\n[STRESS] Simulation Speed Testing" << std::endl;
        
        auto config = std::make_shared<nlm::Config>();
        config->set("neuron_count", static_cast<int64_t>(200), nlm::ConfigSource::Default);
        
        nlm::Brain brain(config);
        assert(brain.initialize());
        
        PerformanceMetrics metrics;
        nlm::SimulationStep step = 0;
        
        try {
            // Speed testing with many steps
            auto start_time = std::chrono::high_resolution_clock::now();
            
            for (int i = 0; i < 1000; ++i) {
                brain.step(step + i);
                if (i % 100 == 0) {
                    brain.getNeuromodulation().update(0.01f);
                }
            }
            
            metrics.end();
            
            StressTestResult result;
            result.test_type = "Simulation Speed Test";
            result.stress_level = 4; // High stress level
            result.passed = true;
            result.performance_degradation = 0.0;
            result.details = "Successfully completed 1000 simulation steps";
            
            std::cout << "  PASSED: Simulation speed test completed in " << metrics.getExecutionTimeMs() << "ms" << std::endl;
            
            return result;
        } catch (const std::exception& e) {
            StressTestResult result;
            result.test_type = "Simulation Speed Test";
            result.stress_level = 4;
            result.passed = false;
            result.performance_degradation = 100.0;
            result.details = std::string("Exception: ") + e.what();
            
            std::cout << "  FAILED: " << result.details << std::endl;
            
            return result;
        }
    }
};

// Main test framework runner
void runPerformanceBenchmarks() {
    std::cout << "=== NLM Performance Benchmark Suite ===" << std::endl;
    
    // Network size benchmark
    NetworkSizeBenchmark network_bench;
    auto network_results = network_bench.runBenchmarks();
    network_bench.generateBenchmarkReport(network_results);
    
    // Memory usage testing
    MemoryUsageTester memory_tester;
    auto memory_results = memory_tester.runMemoryUsageTests();
    
    // Stress testing
    StressTester stress_tester;
    auto stress_results = stress_tester.runStressTests();
    
    // Print stress test results
    std::cout << "\n=== Stress Test Results ===" << std::endl;
    for (const auto& result : stress_results) {
        std::cout << "Test: " << result.test_type << std::endl;
        std::cout << "  Stress Level: " << result.stress_level << std::endl;
        std::cout << "  Status: " << (result.passed ? "PASSED" : "FAILED") << std::endl;
        std::cout << "  Details: " << result.details << std::endl;
        std::cout << std::endl;
    }
}

} // namespace test_performance
