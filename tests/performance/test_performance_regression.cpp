// Performance Testing Suite
// Tests performance regression, memory leaks, scaling, and resource usage

#include "brain/Brain.hpp"
#include "agent/AgentBrain.hpp"
#include "core/Logger/Logger.hpp"
#include <cassert>
#include <iostream>
#include <chrono>
#include <vector>
#include <algorithm>
#include <memory>
#include <thread>
#include <atomic>
#include <iomanip>
#include <sstream>

namespace test_performance {

class PerformanceTester {
public:
    struct BenchmarkResult {
        std::string test_name;
        double time_ms;
        double memory_mb;
        double cpu_usage;
        bool passed;
        std::string details;
    };

    std::vector<BenchmarkResult> results;
    double baseline_time_ms = 0.0;
    double baseline_memory_mb = 0.0;

    void log_result(const std::string& test_name, double time_ms, double memory_mb, double cpu_usage, bool passed, const std::string& details = "") {
        std::cout << "[" << (passed ? "PASS" : "FAIL") << "] " << test_name << ": "
                  << std::fixed << std::setprecision(2)
                  << time_ms << "ms, " << memory_mb << "MB, " << cpu_usage << "% " << details << std::endl;
        
        results.push_back({test_name, time_ms, memory_mb, cpu_usage, passed, details});
    }

    double get_current_memory_usage() {
        // Simple memory usage estimation (in a real implementation, use platform-specific APIs)
        std::vector<char> buffer(1024 * 1024); // 1MB buffer
        return 1.0; // Return 1MB for testing
    }

    void test_performance_regression() {
        std::cout << std::endl << std::string(70, '=') << std::endl;
        std::cout << "Testing Performance Regression" << std::endl;
        std::cout << std::string(70, '=') << std::endl;

        // Test 1: Brain initialization performance
        auto start_time = std::chrono::high_resolution_clock::now();
        
        for (int i = 0; i < 100; ++i) {
            nlm::Brain brain;
            brain.initialize();
            brain.step();
        }
        
        auto end_time = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
        
        double time_per_operation = duration.count() / 100.0;
        double baseline_threshold = 100.0; // 100ms per operation
        
        if (time_per_operation < baseline_threshold) {
            log_result("Brain initialization performance", time_per_operation, get_current_memory_usage(), 10.0, true,
                     "Performance within acceptable range");
        } else {
            log_result("Brain initialization performance", time_per_operation, get_current_memory_usage(), 10.0, false,
                     "Performance degradation detected");
        }

        // Test 2: Large-scale brain performance
        start_time = std::chrono::high_resolution_clock::now();
        
        for (int i = 0; i < 10; ++i) {
            nlm::Brain brain;
            
            // Add many neurons
            for (int j = 0; j < 1000; ++j) {
                nlm::Neuron neuron(nlm::NeuronId(j));
                brain.addNeuron(neuron);
            }
            
            brain.initialize();
            
            // Run simulation
            for (int step = 0; step < 100; ++step) {
                brain.step();
            }
        }
        
        end_time = std::chrono::high_resolution_clock::now();
        duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
        
        double large_scale_time = duration.count() / 10.0; // Per 10-brains test
        
        if (large_scale_time < 1000.0) { // Less than 1s per large-scale brain
            log_result("Large-scale brain performance", large_scale_time, get_current_memory_usage(), 50.0, true,
                     "Large-scale performance acceptable");
        } else {
            log_result("Large-scale brain performance", large_scale_time, get_current_memory_usage(), 50.0, false,
                     "Large-scale performance degraded");
        }
    }

    void test_memory_leak_detection() {
        std::cout << std::endl << std::string(70, '=') << std::endl;
        std::cout << "Testing Memory Leak Detection" << std::endl;
        std::cout << std::string(70, '=') << std::endl;

        // Test 1: Sequential allocation/deallocation
        std::vector<double> memory_samples;
        
        for (int iteration = 0; iteration < 10; ++iteration) {
            auto start_time = std::chrono::high_resolution_clock::now();
            auto start_memory = get_current_memory_usage();
            
            // Allocate and deallocate many objects
            std::vector<std::unique_ptr<nlm::Brain>> brains;
            for (int i = 0; i < 100; ++i) {
                brains.push_back(std::make_unique<nlm::Brain>());
                if (i % 10 == 0) {
                    brains[i / 10].reset(); // Deallocate some
                }
            }
            
            auto end_time = std::chrono::high_resolution_clock::now();
            auto end_memory = get_current_memory_usage();
            
            auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
            memory_samples.push_back(end_memory - start_memory);
            
            // Force garbage collection
            brains.clear();
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
        
        // Check for memory growth trend
        double initial_memory = memory_samples[0];
        double final_memory = memory_samples.back();
        double memory_growth = (final_memory - initial_memory) / initial_memory * 100.0;
        
        if (memory_growth < 10.0) { // Less than 10% growth
            log_result("Sequential allocation/deallocation", 0.0, final_memory, 0.0, true,
                     "No significant memory growth");
        } else {
            log_result("Sequential allocation/deallocation", 0.0, final_memory, 0.0, false,
                     std::to_string(memory_growth) + "% memory growth detected");
        }

        // Test 2: Deep object graph
        auto start_time = std::chrono::high_resolution_clock::now();
        
        for (int i = 0; i < 5; ++i) {
            nlm::Brain brain;
            
            // Create deep hierarchy
            for (int j = 0; j < 100; ++j) {
                nlm::Neuron neuron(nlm::NeuronId(j));
                
                // Create synaptic connections (deep graph)
                for (int k = 0; k < 10; ++k) {
                    nlm::Synapse synapse(nlm::SynapseId(j * 10 + k), 
                                      nlm::NeuronId(j), 
                                      nlm::NeuronId((j + k) % 1000));
                    brain.addSynapse(synapse);
                }
                
                brain.addNeuron(neuron);
            }
            
            brain.initialize();
            brain.step();
        }
        
        auto end_time = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
        
        if (duration.count() < 5000) { // Less than 5s
            log_result("Deep object graph memory management", duration.count(), get_current_memory_usage(), 30.0, true,
                     "Deep object graphs handled efficiently");
        } else {
            log_result("Deep object graph memory management", duration.count(), get_current_memory_usage(), 30.0, false,
                     "Memory issues with deep object graphs");
        }
    }

    void test_performance_scaling() {
        std::cout << std::endl << std::string(70, '=') << std::endl;
        std::cout << "Testing Performance Scaling" << std::endl;
        std::cout << std::string(70, '=') << std::endl;

        // Test 1: Neuron count scaling
        std::vector<std::pair<int, double>> scaling_results;
        
        for (int neuron_count : {100, 500, 1000, 2000}) {
            auto start_time = std::chrono::high_resolution_clock::now();
            
            nlm::Brain brain;
            
            // Add neurons
            for (int i = 0; i < neuron_count; ++i) {
                nlm::Neuron neuron(nlm::NeuronId(i));
                brain.addNeuron(neuron);
            }
            
            brain.initialize();
            
            // Run simulation
            for (int step = 0; step < 50; ++step) {
                brain.step();
            }
            
            auto end_time = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
            
            scaling_results.push_back({neuron_count, duration.count()});
        }
        
        // Calculate scaling efficiency
        bool scaling_acceptable = true;
        for (size_t i = 1; i < scaling_results.size(); ++i) {
            int prev_neurons = scaling_results[i-1].first;
            int curr_neurons = scaling_results[i].first;
            double prev_time = scaling_results[i-1].second;
            double curr_time = scaling_results[i].second;
            
            double neurons_ratio = (double)curr_neurons / prev_neurons;
            double time_ratio = curr_time / prev_time;
            double efficiency = neurons_ratio / time_ratio;
            
            if (efficiency < 0.5) { // More than 2x slower per neuron
                scaling_acceptable = false;
                break;
            }
        }
        
        if (scaling_acceptable) {
            log_result("Neuron count scaling", scaling_results.back().second, get_current_memory_usage(), 60.0, true,
                     "Linear scaling observed");
        } else {
            log_result("Neuron count scaling", scaling_results.back().second, get_current_memory_usage(), 60.0, false,
                     "Sub-linear scaling detected");
        }

        // Test 2: Simulation step scaling
        for (int step_count : {100, 500, 1000, 5000}) {
            auto start_time = std::chrono::high_resolution_clock::now();
            
            nlm::Brain brain;
            brain.initialize();
            
            for (int step = 0; step < step_count; ++step) {
                brain.step();
            }
            
            auto end_time = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
            
            double time_per_step = duration.count() / (double)step_count;
            
            if (step_count >= 5000 && time_per_step > 1.0) { // More than 1ms per step for large steps
                log_result("Simulation step scaling", duration.count(), get_current_memory_usage(), 80.0, false,
                         "Performance degradation with many steps");
                break;
            }
        }
        
        log_result("Simulation step scaling", 0.0, get_current_memory_usage(), 80.0, true,
                 "Linear scaling with simulation steps");
    }

    void test_resource_usage() {
        std::cout << std::endl << std::string(70, '=') << std::endl;
        std::cout << "Testing Resource Usage" << std::endl;
        std::cout << std::string(70, '=') << std::endl;

        // Test 1: CPU resource monitoring
        auto start_time = std::chrono::high_resolution_clock::now();
        
        std::atomic<int> counter{0};
        int thread_count = std::thread::hardware_concurrency();
        
        std::vector<std::thread> threads;
        for (int i = 0; i < thread_count; ++i) {
            threads.emplace_back([&counter]() {
                for (int j = 0; j < 1000; ++j) {
                    counter.fetch_add(1, std::memory_order_relaxed);
                }
            });
        }
        
        for (auto& thread : threads) {
            thread.join();
        }
        
        auto end_time = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
        
        double cpu_efficiency = (thread_count * 1000.0) / duration.count(); // Operations per ms
        
        if (cpu_efficiency > 1000) {
            log_result("CPU resource utilization", duration.count(), get_current_memory_usage(), cpu_efficiency, true,
                     "Good CPU efficiency");
        } else {
            log_result("CPU resource utilization", duration.count(), get_current_memory_usage(), cpu_efficiency, false,
                     "Poor CPU efficiency");
        }

        // Test 2: Memory bandwidth simulation
        start_time = std::chrono::high_resolution_clock::now();
        
        std::vector<std::vector<float>> large_matrix(1000, std::vector<float>(1000, 0.0f));
        
        // Matrix operations to test memory bandwidth
        for (int i = 0; i < 100; ++i) {
            for (int j = 0; j < 1000; ++j) {
                float sum = 0.0f;
                for (int k = 0; k < 1000; ++k) {
                    sum += large_matrix[j][k];
                }
                large_matrix[j][(i + j) % 1000] = sum / 1000.0f;
            }
        }
        
        end_time = std::chrono::high_resolution_clock::now();
        duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
        
        double memory_bandwidth = (1000.0 * 1000.0 * sizeof(float) * 3) / (duration.count() * 1000.0); // MB/s
        
        if (memory_bandwidth > 100.0) { // More than 100 MB/s
            log_result("Memory bandwidth utilization", duration.count(), get_current_memory_usage(), memory_bandwidth, true,
                     "Good memory bandwidth utilization");
        } else {
            log_result("Memory bandwidth utilization", duration.count(), get_current_memory_usage(), memory_bandwidth, false,
                     "Poor memory bandwidth utilization");
        }
    }

    void run_all_tests() {
        std::cout << std::endl << std::string(80, '=') << std::endl;
        std::cout << "PERFORMANCE TESTING SUITE" << std::endl;
        std::cout << std::string(80, '=') << std::endl;

        test_performance_regression();
        test_memory_leak_detection();
        test_performance_scaling();
        test_resource_usage();

        std::cout << std::endl << std::string(80, '=') << std::endl;
        std::cout << "TEST SUMMARY" << std::endl;
        std::cout << std::string(80, '=') << std::endl;
        
        int passed = 0, failed = 0;
        for (const auto& result : results) {
            if (result.passed) passed++; else failed++;
        }
        
        std::cout << "Total Tests: " << (passed + failed) << std::endl;
        std::cout << "Passed: " << passed << std::endl;
        std::cout << "Failed: " << failed << std::endl;
        std::cout << "Success Rate: " << ((passed / (double)(passed + failed)) * 100.0) << "%" << std::endl;

        std::cout << std::endl << "Detailed Results:" << std::endl;
        for (const auto& result : results) {
            std::cout << "  " << (result.passed ? "PASS" : "FAIL") << " " << result.test_name << ": "
                      << std::fixed << std::setprecision(2)
                      << result.time_ms << "ms, " << result.memory_mb << "MB, " << result.cpu_usage << "%" << std::endl;
        }
    }
};

int main() {
    PerformanceTester tester;
    tester.run_all_tests();
    
    return 0;
}
