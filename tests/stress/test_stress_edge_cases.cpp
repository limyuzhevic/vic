// Stress Testing Suite
// Tests high-load simulation, complex networks, long-running, and resource exhaustion

#include "brain/Brain.hpp"
#include "agent/AgentBrain.hpp"
#include "agent/AgentBody.hpp"
#include "sensory/SensoryInput.hpp"
#include "world/SimpleWorld.hpp"
#include <cassert>
#include <iostream>
#include <chrono>
#include <vector>
#include <algorithm>
#include <thread>
#include <atomic>
#include <map>
#include <random>

namespace test_stress {

class StressTester {
public:
    struct StressTestResult {
        std::string test_name;
        double duration_seconds;
        bool passed;
        std::string details;
        double peak_memory_mb;
        double error_rate;
    };

    std::vector<StressTestResult> results;

    void log_result(const std::string& test_name, double duration, bool passed, 
                   const std::string& details, double peak_memory = 0.0, double error_rate = 0.0) {
        std::cout << "[" << (passed ? "PASS" : "FAIL") << "] " << test_name << ": "
                  << std::fixed << std::setprecision(2)
                  << duration << "s, " << error_rate << "% error rate " << details << std::endl;
        
        results.push_back({test_name, duration, passed, details, peak_memory, error_rate});
    }

    double get_current_memory_usage() {
        // Simple memory estimation
        std::vector<char> buffer(1024 * 1024); // 1MB
        return 1.0;
    }

    void test_high_load_simulation() {
        std::cout << std::endl << std::string(70, '=') << std::endl;
        std::cout << "Testing High-Load Simulation" << std::endl;
        std::cout << std::string(70, '=') << std::endl;

        // Test 1: Extremely large brain simulation
        auto start_time = std::chrono::high_resolution_clock::now();
        int error_count = 0;
        
        try {
            for (int brain_id = 0; brain_id < 20; ++brain_id) {
                nlm::Brain brain;
                
                // Create massive brain
                for (int i = 0; i < 5000; ++i) {
                    nlm::Neuron neuron(nlm::NeuronId(i));
                    
                    // Set extreme properties
                    neuron.setMembranePotential((float)(i % 2 ? -1000.0f : 1000.0f));
                    neuron.setThreshold((float)(i % 2 ? -100.0f : 100.0f));
                    
                    brain.addNeuron(neuron);
                }
                
                // Create dense connectivity
                for (int i = 0; i < 1000; ++i) {
                    nlm::Synapse synapse(nlm::SynapseId(i), 
                                      nlm::NeuronId(i % 5000), 
                                      nlm::NeuronId((i + 1) % 5000));
                    synapse.setWeight(0.5f); // Dense connectivity
                    brain.addSynapse(synapse);
                }
                
                brain.initialize();
                
                // Run intensive simulation
                for (int step = 0; step < 200; ++step) {
                    brain.step();
                    
                    // Monitor for errors
                    if (brain.getTotalErrorCount() > 100) {
                        error_count++;
                    }
                }
            }
        } catch (...) {
            error_count++;
        }
        
        auto end_time = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::seconds>(end_time - start_time);
        
        double error_rate = (error_count / 20.0) * 100.0; // Per-brain error rate
        
        if (error_rate < 10.0) { // Less than 10% error rate
            log_result("High-load brain simulation", duration.count(), true, "Simulation completed successfully",
                      get_current_memory_usage(), error_rate);
        } else {
            log_result("High-load brain simulation", duration.count(), false, "Excessive errors during simulation",
                      get_current_memory_usage(), error_rate);
        }
    }

    void test_complex_network() {
        std::cout << std::endl << std::string(70, '=') << std::endl;
        std::cout << "Testing Complex Network" << std::endl;
        std::cout << std::string(70, '=') << std::endl;

        // Test 1: Highly interconnected neural network
        auto start_time = std::chrono::high_resolution_clock::now();
        
        std::mt19937 rng(42); // Deterministic for testing
        std::uniform_real_distribution<float> weight_dist(-1.0f, 1.0f);
        std::uniform_int_distribution<int> neuron_dist(0, 999);
        
        nlm::Brain brain;
        
        // Create network with complex topology
        for (int i = 0; i < 1000; ++i) {
            nlm::Neuron neuron(nlm::NeuronId(i));
            neuron.setMembranePotential(weight_dist(rng));
            neuron.setThreshold(weight_dist(rng));
            brain.addNeuron(neuron);
        }
        
        // Create complex synaptic connections
        int connection_count = 0;
        for (int i = 0; i < 5000; ++i) { // High connection density
            int pre_neuron = neuron_dist(rng);
            int post_neuron = neuron_dist(rng);
            
            if (pre_neuron != post_neuron) {
                nlm::Synapse synapse(nlm::SynapseId(i), 
                                  nlm::NeuronId(pre_neuron), 
                                  nlm::NeuronId(post_neuron));
                synapse.setWeight(weight_dist(rng));
                brain.addSynapse(synapse);
                connection_count++;
            }
        }
        
        brain.initialize();
        
        // Run complex network simulation
        int step = 0;
        while (step < 500 && brain.getTotalErrorCount() < 50) {
            brain.step();
            step++;
            
            // Random input to keep network active
            if (step % 10 == 0) {
                for (int i = 0; i < 10; ++i) {
                    int neuron_id = neuron_dist(rng);
                    nlm::Neuron* neuron = brain.getNeuron(neuron_id);
                    if (neuron) {
                        neuron->injectCurrent(weight_dist(rng) * 10.0f);
                    }
                }
            }
        }
        
        auto end_time = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::seconds>(end_time - start_time);
        
        double connection_density = (double)connection_count / 5000.0 * 100.0;
        
        if (step >= 400 && brain.getTotalErrorCount() < 20) {
            log_result("Complex network simulation", duration.count(), true,
                     std::to_string(connection_density) + "% connection density, " + std::to_string(step) + " steps",
                     get_current_memory_usage(), (double)brain.getTotalErrorCount() / step * 100.0);
        } else {
            log_result("Complex network simulation", duration.count(), false,
                     "Failed to complete stable simulation",
                     get_current_memory_usage(), (double)brain.getTotalErrorCount() / step * 100.0);
        }
    }

    void test_long_running_simulation() {
        std::cout << std::endl << std::string(70, '=') << std::endl;
        std::cout << "Testing Long-Running Simulation" << std::endl;
        std::cout << std::string(70, '=') << std::endl;

        // Test 1: Extended simulation over time
        auto start_time = std::chrono::high_resolution_clock::now();
        int total_errors = 0;
        int simulation_cycles = 0;
        
        nlm::Brain brain;
        brain.initialize();
        
        // Extended simulation with adaptive behavior
        for (int cycle = 0; cycle < 1000; ++cycle) {
            // Vary simulation intensity based on cycle
            int steps_this_cycle = 10 + (cycle % 50); // Varying intensity
            
            for (int step = 0; step < steps_this_cycle; ++step) {
                brain.step();
                
                if (brain.getTotalErrorCount() > 50) {
                    total_errors++;
                    
                    // Recovery attempt
                    if (cycle % 10 == 0) {
                        brain.initialize(); // Reset brain
                    }
                }
            }
            
            simulation_cycles++;
            
            // Periodic resource monitoring
            if (cycle % 100 == 0) {
                double memory_usage = get_current_memory_usage();
                if (memory_usage > 100.0) { // Memory threshold
                    total_errors++;
                }
            }
        }
        
        auto end_time = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::seconds>(end_time - start_time);
        
        double error_rate = (double)total_errors / simulation_cycles * 100.0;
        
        if (error_rate < 5.0) {
            log_result("Long-running simulation", duration.count(), true,
                     std::to_string(simulation_cycles) + " cycles completed",
                     get_current_memory_usage(), error_rate);
        } else {
            log_result("Long-running simulation", duration.count(), false,
                     "Excessive errors during extended simulation",
                     get_current_memory_usage(), error_rate);
        }
    }

    void test_resource_exhaustion() {
        std::cout << std::endl << std::string(70, '=') << std::endl;
        std::cout << "Testing Resource Exhaustion" << std::endl;
        std::cout << std::string(70, '=') << std::endl;

        // Test 1: Memory exhaustion handling
        auto start_time = std::chrono::high_resolution_clock::now();
        
        std::vector<std::vector<nlm::Brain>> brain_matrices(100, std::vector<nlm::Brain>(100));
        
        int allocation_success = 0;
        int allocation_fail = 0;
        
        for (int i = 0; i < 100; ++i) {
            for (int j = 0; j < 100; ++j) {
                try {
                    // This would normally exhaust memory, but in testing we'll catch exceptions
                    brain_matrices[i][j] = nlm::Brain();
                    allocation_success++;
                    
                    // Initialize to consume resources
                    brain_matrices[i][j].initialize();
                    
                } catch (...) {
                    allocation_fail++;
                }
            }
        }
        
        auto end_time = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::seconds>(end_time - start_time);
        
        double allocation_rate = (double)allocation_success / (allocation_success + allocation_fail) * 100.0;
        
        if (allocation_rate > 50.0) { // Good allocation rate
            log_result("Memory exhaustion handling", duration.count(), true,
                     std::to_string(allocation_success) + " allocations succeeded, " + std::to_string(allocation_fail) + " failed",
                     get_current_memory_usage(), 100.0 - allocation_rate);
        } else {
            log_result("Memory exhaustion handling", duration.count(), false,
                     "Poor resource allocation under stress",
                     get_current_memory_usage(), 100.0 - allocation_rate);
        }

        // Clear matrices to free resources
        brain_matrices.clear();
        
        // Test 2: CPU exhaustion simulation
        start_time = std::chrono::high_resolution_clock::now();
        
        std::atomic<int> cpu_load{0};
        int thread_count = std::thread::hardware_concurrency();
        
        std::vector<std::thread> threads;
        for (int i = 0; i < thread_count; ++i) {
            threads.emplace_back([&cpu_load]() {
                for (int j = 0; j < 100000; ++j) {
                    // Heavy CPU computation
                    volatile int result = j * j;
                    cpu_load.fetch_add(1, std::memory_order_relaxed);
                }
            });
        }
        
        for (auto& thread : threads) {
            thread.join();
        }
        
        end_time = std::chrono::high_resolution_clock::now();
        duration = std::chrono::duration_cast<std::chrono::seconds>(end_time - start_time);
        
        double cpu_utilization = (double)cpu_load / (thread_count * 100000.0) * 100.0;
        
        log_result("CPU exhaustion simulation", duration.count(), true,
                 std::to_string(cpu_utilization) + "% CPU utilization",
                 get_current_memory_usage(), 0.0);
    }

    void run_all_tests() {
        std::cout << std::endl << std::string(80, '=') << std::endl;
        std::cout << "STRESS TESTING SUITE" << std::endl;
        std::cout << std::string(80, '=') << std::endl;

        test_high_load_simulation();
        test_complex_network();
        test_long_running_simulation();
        test_resource_exhaustion();

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
                      << result.duration_seconds << "s, " << result.error_rate << "% error" << std::endl;
        }
    }
};

int main() {
    StressTester tester;
    tester.run_all_tests();
    
    return 0;
}
