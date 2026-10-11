// NLM Integration Tests
// Scalability Tests

#include "framework/TestFramework.hpp"
#include "framework/TestFixtures.hpp"
#include "brain/NeuromodulationSystem.hpp"
#include <cassert>
#include <cmath>
#include <thread>
#include <chrono>

class TestScalability : public TestCase {
public:
    TestScalability() : TestCase("Scalability") {}
    
    void testLinearScaling() {
        // Test that system scales linearly with increasing complexity
        TestFramework framework;
        BrainSystemFixture brain_fixture(framework, "LinearScaling");
        
        Brain& brain = brain_fixture.getBrain();
        
        // Measure performance with different sizes
        std::vector<int> test_sizes = {100, 500, 1000, 2000};
        std::vector<double> processing_times;
        
        for (int size : test_sizes) {
            // Create test scenario with specified size
            std::vector<double> sensory_input(size, 0.5);
            
            auto start_time = std::chrono::high_resolution_clock::now();
            
            // Process through brain system
            std::vector<double> output = brain.processSensoryInput(sensory_input);
            
            auto end_time = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
            
            processing_times.push_back(duration.count());
            
            assert(output.size() == size);
        }
        
        // Check that scaling is reasonable (not exponential)
        if (processing_times.size() >= 2) {
            double ratio_1 = processing_times[1] / processing_times[0]; // 500/100
            double ratio_2 = processing_times[2] / processing_times[1]; // 1000/500
            double ratio_3 = processing_times[3] / processing_times[2]; // 2000/1000
            
            // Each step should be less than 10x to indicate linear scaling
            assert(ratio_1 < 10.0);
            assert(ratio_2 < 10.0);
            assert(ratio_3 < 10.0);
        }
    }
    
    void testMemoryScaling() {
        TestFramework framework;
        MemorySystemFixture memory_fixture(framework, "MemoryScaling");
        
        MemorySystem& memory = memory_fixture.getMemorySystem();
        
        // Test memory usage with increasing data sizes
        std::vector<size_t> test_sizes = {1000, 5000, 10000, 20000};
        std::vector<size_t> memory_usages;
        
        for (size_t size : test_sizes) {
            std::vector<double> data(size, 0.0);
            for (size_t i = 0; i < size; ++i) {
                data[i] = static_cast<double>(rand()) / RAND_MAX;
            }
            
            memory.storePattern(data);
            size_t memory_usage = memory.getCurrentMemoryUsage();
            memory_usages.push_back(memory_usage);
            
            assert(memory_usage > 0);
        }
        
        // Memory should scale approximately linearly (less than 5x for each step)
        if (memory_usages.size() >= 2) {
            double ratio_1 = static_cast<double>(memory_usages[1]) / memory_usages[0]; // 5000/1000
            double ratio_2 = static_cast<double>(memory_usages[2]) / memory_usages[1]; // 10000/5000
            double ratio_3 = static_cast<double>(memory_usages[3]) / memory_usages[2]; // 20000/10000
            
            assert(ratio_1 < 5.0);
            assert(ratio_2 < 5.0);
            assert(ratio_3 < 5.0);
        }
    }
    
    void testConcurrencyScaling() {
        TestFramework framework;
        
        // Test concurrent processing with increasing thread counts
        std::vector<int> thread_counts = {1, 2, 4, 8};
        std::vector<double> processing_times;
        
        for (int thread_count : thread_counts) {
            std::vector<std::thread> threads;
            auto start_time = std::chrono::high_resolution_clock::now();
            
            // Create and start threads
            for (int i = 0; i < thread_count; ++i) {
                threads.emplace_back([thread_count]() {
                    // Each thread performs computational work
                    double result = 0.0;
                    for (int j = 0; j < 100000; ++j) {
                        result += std::sin(j * 0.01) * std::cos(j * 0.02);
                    }
                });
            }
            
            // Wait for all threads to complete
            for (auto& thread : threads) {
                thread.join();
            }
            
            auto end_time = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
            
            processing_times.push_back(duration.count());
        }
        
        // Check that scaling is reasonable (not worse than linear)
        if (processing_times.size() >= 2) {
            double ratio_1 = processing_times[1] / processing_times[0]; // 2 threads / 1 thread
            double ratio_2 = processing_times[2] / processing_times[1]; // 4 threads / 2 threads
            double ratio_3 = processing_times[3] / processing_times[2]; // 8 threads / 4 threads
            
            // Each step should be less than 5x to indicate good parallel scaling
            assert(ratio_1 < 5.0);
            assert(ratio_2 < 5.0);
            assert(ratio_3 < 5.0);
        }
    }
    
    void testLoadBalancing() {
        // Test that load is balanced across system components
        TestFramework framework;
        BrainSystemFixture brain_fixture(framework, "LoadBalancing");
        
        Brain& brain = brain_fixture.getBrain();
        const auto& neurons = brain_fixture.getNeurons();
        const auto& regions = brain_fixture.getRegions();
        
        // Measure computational load per region
        std::vector<double> regional_loads;
        for (const auto& region : regions) {
            // Simulate work for this region
            double region_load = 0.0;
            for (const auto& neuron : neurons) {
                if (neuron.region_id == region.id) {
                    // Process neuron (simplified)
                    double neuron_load = std::sin(neuron.position.x) * std::cos(neuron.position.y);
                    region_load += neuron_load;
                }
            }
            regional_loads.push_back(region_load);
        }
        
        // Check load balance (coefficient of variation < 0.5 indicates good balance)
        double sum = std::accumulate(regional_loads.begin(), regional_loads.end(), 0.0);
        double mean = sum / regional_loads.size();
        double sq_sum = std::inner_product(regional_loads.begin(), regional_loads.end(), regional_loads.begin(), 0.0);
        double stdev = std::sqrt(sq_sum / regional_loads.size() - mean * mean);
        double coefficient_of_variation = stdev / mean;
        
        assert(coefficient_of_variation < 0.5); // Good load balance
        
        // Test synaptic load distribution
        const auto& synapses = brain_fixture.getSynapses();
        std::map<int, double> pre_neuron_load;
        std::map<int, double> post_neuron_load;
        
        for (const auto& synapse : synapses) {
            pre_neuron_load[synapse.pre_neuron_id] += std::abs(synapse.weight);
            post_neuron_load[synapse.post_neuron_id] += std::abs(synapse.weight);
        }
        
        // Check that no neuron is overloaded (more than 3x average load)
        double avg_pre_load = sum / pre_neuron_load.size();
        double avg_post_load = sum / post_neuron_load.size();
        
        for (const auto& load : pre_neuron_load) {
            assert(load.second <= avg_pre_load * 3.0);
        }
        
        for (const auto& load : post_neuron_load) {
            assert(load.second <= avg_post_load * 3.0);
        }
    }
};