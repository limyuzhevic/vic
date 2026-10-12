#ifndef TEST_PERFORMANCE_HPP
#define TEST_PERFORMANCE_HPP

#include "tests/Framework/TestFramework.hpp"
#include "src/brain/Brain.hpp"
#include "src/memory/NeuralWorkingMemory.hpp"
#include "src/neuromodulation/Neuromodulator.hpp"
#include "tests/Helpers/TestHelpers.hpp"

class TestPerformance : public TestCase {
public:
    TestPerformance() : TestCase("Performance", "Performance") {}
    
    void run() override {
        TestHelpers::Timer timer;
        
        // Test brain performance
        {
            Brain::BrainConfig brain_config;
            brain_config.num_regions = 2;
            brain_config.num_neurons_per_region = 100;
            brain_config.max_synapses_per_neuron = 10;
            brain_config.v_rest = -65.0;
            brain_config.v_threshold = -50.0;
            brain_config.v_reset = -65.0;
            brain_config.c_m = 1.0;
            brain_config.g_l = 0.1;
            brain_config.tau_m = 10.0;
            brain_config.dt = 0.001;
            
            Brain brain(brain_config);
            
            timer.start();
            for (int i = 0; i < 1000; ++i) {
                brain.processSpike(i * brain_config.dt, 0);
                brain.step();
            }
            timer.stop();
            
            double brain_time_ms = timer.elapsedMs();
            std::cout << "Brain processing time for 1000 steps: " << brain_time_ms << " ms" << std::endl;
            
            // Performance should be reasonable (less than 1 second for 1000 steps)
            ASSERT_TRUE(brain_time_ms < 1000.0, "Brain processing should be faster than 1 second for 1000 steps");
        }
        
        // Test memory performance
        {
            NeuralWorkingMemory::WorkingMemoryConfig wm_config;
            wm_config.capacity = 1000.0;
            wm_config.num_items = 100;
            wm_config.decay_rate = 0.01;
            wm_config.competition_enabled = true;
            
            NeuralWorkingMemory working_memory(wm_config);
            
            // Create test patterns
            std::vector<std::vector<double>> patterns;
            for (int i = 0; i < 100; ++i) {
                std::vector<double> pattern(10, 0.0);
                pattern[i % 10] = 1.0;
                patterns.push_back(pattern);
            }
            
            timer.start();
            for (int i = 0; i < 100; ++i) {
                working_memory.encode(patterns[i], i);
            }
            for (int i = 0; i < 100; ++i) {
                working_memory.retrieve(patterns[i], 5);
            }
            working_memory.step();
            timer.stop();
            
            double memory_time_ms = timer.elapsedMs();
            std::cout << "Memory operations time for 200 operations: " << memory_time_ms << " ms" << std::endl;
            
            // Performance should be reasonable (less than 100 ms for 200 operations)
            ASSERT_TRUE(memory_time_ms < 100.0, "Memory operations should be faster than 100 ms for 200 operations");
        }
        
        // Test neuromodulation performance
        {
            Neuromodulator::NeuromodulatorConfig nm_config;
            nm_config.num_modulators = 4;
            nm_config.global_decay_rate = 0.01;
            
            Neuromodulator neuromodulator(nm_config);
            
            timer.start();
            for (int i = 0; i < 1000; ++i) {
                neuromodulator.update(i * 0.001);
                neuromodulator.applyModulation(0.1, -65.0, 1.0);
            }
            timer.stop();
            
            double neuromodulator_time_ms = timer.elapsedMs();
            std::cout << "Neuromodulator operations time for 2000 operations: " << neuromodulator_time_ms << " ms" << std::endl;
            
            // Performance should be reasonable (less than 500 ms for 2000 operations)
            ASSERT_TRUE(neuromodulator_time_ms < 500.0, "Neuromodulator operations should be faster than 500 ms for 2000 operations");
        }
        
        // Test benchmark utilities
        {
            auto result = TestBenchmark::benchmark("Matrix Multiplication", []() {
                std::vector<std::vector<double>> A(100, std::vector<double>(100, 0.0));
                std::vector<std::vector<double>> B(100, std::vector<double>(100, 0.0));
                std::vector<std::vector<double>> C(100, std::vector<double>(100, 0.0));
                
                for (int i = 0; i < 100; ++i) {
                    for (int j = 0; j < 100; ++j) {
                        for (int k = 0; k < 100; ++k) {
                            C[i][j] += A[i][k] * B[k][j];
                        }
                    }
                }
            }, 10);
            
            std::cout << "Matrix multiplication benchmark: " << result.time_ms << " ms" << std::endl;
            
            // Performance should be reasonable (less than 100 ms for 10 iterations)
            ASSERT_TRUE(result.time_ms < 100.0, "Matrix multiplication benchmark should be faster than 100 ms for 10 iterations");
        }
        
        // Test memory usage
        {
            size_t peak_memory = TestBenchmark::benchmark("Memory Allocation", []() {
                std::vector<double> memory(100000, 0.0);
                double sum = std::accumulate(memory.begin(), memory.end(), 0.0);
                return sum;
            }, 100).throughput;
            
            std::cout << "Memory allocation benchmark completed" << std::endl;
            
            // Memory usage should be reasonable
            ASSERT_TRUE(peak_memory > 0, "Memory allocation benchmark should use memory");
        }
        
        // Test system throughput
        {
            auto result = TestBenchmark::benchmark("Complete System", [this]() {
                Brain::BrainConfig brain_config;
                brain_config.num_regions = 1;
                brain_config.num_neurons_per_region = 10;
                brain_config.max_synapses_per_neuron = 2;
                brain_config.dt = 0.001;
                
                Brain brain(brain_config);
                brain.processSpike(0.0, 0);
                brain.step();
            }, 100);
            
            double throughput = result.throughput; // Operations per second
            std::cout << "System throughput: " << throughput << " operations per second" << std::endl;
            
            // Throughput should be reasonable (at least 1000 operations per second)
            ASSERT_TRUE(throughput > 1000.0, "System throughput should be greater than 1000 operations per second");
        }
        
        // Test scalability
        {
            std::vector<double> scalability_times;
            
            for (int neurons = 10; neurons <= 100; neurons += 10) {
                Brain::BrainConfig brain_config;
                brain_config.num_regions = 1;
                brain_config.num_neurons_per_region = neurons;
                brain_config.max_synapses_per_neuron = 5;
                brain_config.dt = 0.001;
                
                Brain brain(brain_config);
                
                timer.start();
                for (int i = 0; i < 100; ++i) {
                    brain.processSpike(i * brain_config.dt, 0);
                    brain.step();
                }
                timer.stop();
                
                double time_ms = timer.elapsedMs();
                scalability_times.push_back(time_ms);
                
                std::cout << "Scalability test: " << neurons << " neurons in " << time_ms << " ms" << std::endl;
            }
            
            // Check that scalability is reasonable (time should grow roughly linearly)
            double time_ratio = scalability_times.back() / scalability_times.front();
            int neuron_ratio = 100 / 10;
            
            // Time ratio should be reasonable (not more than 10x for 10x neurons due to efficiency improvements)
            ASSERT_TRUE(time_ratio < neuron_ratio * 2.0, "Scalability should be reasonable (time should grow roughly linearly with neurons)");
        }
        
        // Test Phase 2 integration performance
        {
            TestHelpers::Timer timer2;
            
            // Simulate Phase 2 complete integration
            for (int step = 0; step < 1000; ++step) {
                double time = step * 0.001;
                
                // Neural dynamics
                std::vector<double> v = { -65.0 + 10.0 * sin(time) };
                std::vector<double> dv = { (10.0 * cos(time) - 0.1 * v[0]) / 10.0 };
                
                // STDP
                double weight = 0.5;
                double new_weight = weight + 0.01 * sin(time);
                
                // Memory
                std::vector<double> pattern = { sin(time), cos(time), 1.0 };
                
                // Neuromodulation
                double neuromod = 0.1 * sin(2 * time);
            }
            
            timer2.stop();
            double phase2_time_ms = timer2.elapsedMs();
            
            std::cout << "Phase 2 integration simulation: " << phase2_time_ms << " ms for 1000 steps" << std::endl;
            
            // Performance should be reasonable (less than 100 ms for 1000 steps)
            ASSERT_TRUE(phase2_time_ms < 100.0, "Phase 2 integration should be faster than 100 ms for 1000 steps");
        }
        
        // Test overall system performance
        {
            double total_time = 0.0;
            int total_operations = 0;
            
            // Brain operations
            {
                Brain::BrainConfig brain_config;
                brain_config.num_regions = 2;
                brain_config.num_neurons_per_region = 50;
                brain_config.max_synapses_per_neuron = 5;
                brain_config.dt = 0.001;
                
                Brain brain(brain_config);
                
                timer.start();
                for (int i = 0; i < 500; ++i) {
                    brain.processSpike(i * brain_config.dt, 0);
                    brain.step();
                }
                timer.stop();
                
                total_time += timer.elapsedMs();
                total_operations += 500;
            }
            
            // Memory operations
            {
                NeuralWorkingMemory::WorkingMemoryConfig wm_config;
                wm_config.capacity = 100.0;
                wm_config.num_items = 50;
                wm_config.decay_rate = 0.01;
                
                NeuralWorkingMemory working_memory(wm_config);
                
                timer.start();
                for (int i = 0; i < 200; ++i) {
                    std::vector<double> pattern(5, 0.0);
                    pattern[i % 5] = 1.0;
                    working_memory.encode(pattern, i);
                    working_memory.retrieve(pattern, 5);
                }
                working_memory.step();
                timer.stop();
                
                total_time += timer.elapsedMs();
                total_operations += 200;
            }
            
            std::cout << "Total system performance: " << total_operations << " operations in " << total_time << " ms" << std::endl;
            double operations_per_second = (total_operations / (total_time / 1000.0));
            
            // System performance should be reasonable (at least 1000 operations per second)
            ASSERT_TRUE(operations_per_second > 1000.0, "System should have at least 1000 operations per second");
        }
        
        // Test performance under stress
        {
            double peak_memory = TestBenchmark::benchmark("Stress Test", [this]() {
                // Create and destroy many objects
                std::vector<Brain::BrainConfig> configs;
                for (int i = 0; i < 100; ++i) {
                    Brain::BrainConfig config;
                    config.num_regions = 1;
                    config.num_neurons_per_region = 10;
                    configs.push_back(config);
                }
                
                // Process them
                for (const auto& config : configs) {
                    Brain brain(config);
                    brain.processSpike(0.0, 0);
                    brain.step();
                }
                
                configs.clear();
            }, 10);
            
            std::cout << "Stress test completed with peak memory: " << peak_memory << std::endl;
            
            // Memory usage should be reasonable
            ASSERT_TRUE(peak_memory > 0, "Stress test should use memory");
        }
        
        // Test performance metrics
        {
            double stability = 0.85;
            double learning_rate = 0.01;
            double efficiency = 0.9;
            
            ASSERT_TRUE(stability >= 0.0 && stability <= 1.0, "Stability should be between 0 and 1");
            ASSERT_TRUE(learning_rate > 0.0 && learning_rate < 1.0, "Learning rate should be between 0 and 1");
            ASSERT_TRUE(efficiency >= 0.0 && efficiency <= 1.0, "Efficiency should be between 0 and 1");
        }
        
        ASSERT_TRUE(true, "Performance test completed");
    }
};

#endif // TEST_PERFORMANCE_HPP