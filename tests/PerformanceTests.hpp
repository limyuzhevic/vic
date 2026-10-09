// Performance Integration Tests
// 
// These tests benchmark system performance under various load conditions
// and validate that the system scales appropriately.

#ifndef NLM_PERFORMANCE_TESTS_HPP
#define NLM_PERFORMANCE_TESTS_HPP

#include "IntegrationTestFramework.hpp"
#include "performance/ParallelProcessing.hpp"
#include "performance/MemoryPool.hpp"
#include "performance/SparseConnectivity.hpp"
#include <cassert>
#include <vector>
#include <iostream>
#include <chrono>
#include <algorithm>
#include <thread>

namespace nlm {
namespace performance_tests {

// Test basic system performance under load
class BasicPerformanceTest : public integration_tests::IntegrationTestBase {
public:
    void setupConfiguration() override {
        config_->set("neuron_count", 500);
        config_->set("region_count", 3);
        config_->set("connection_probability", 0.06f);
        config_->set("simulation_timestep", 0.001);
        config_->set("random_seed", 33333u);
        config_->set("parallel_enabled", true);
        config_->set("max_threads", 4);
    }
    
    void setupSystem() override {
        brain_ = std::make_shared<Brain>(config_);
        assertCondition(brain_->initialize(), "Brain initialization failed");
    }
    
    void runTest() override {
        const int iterations = 50;
        const int stepsPerIteration = 100;
        double totalTime = 0.0;
        size_t totalSpikes = 0;
        
        for (int iteration = 0; iteration < iterations; ++iteration) {
            auto start = std::chrono::high_resolution_clock::now();
            
            // Run neural computation
            for (int step = 0; step < stepsPerIteration; ++step) {
                brain_->step(step);
                
                // Inject varying input patterns
                if (step % 10 == 0) {
                    brain_->injectCurrentToNeurons(NeuronType::Sensory, 30.0f + (iteration % 5) * 10.0f);
                }
            }
            
            auto end = std::chrono::high_resolution_clock::now();
            double duration = std::chrono::duration<double>(end - start).count();
            totalTime += duration;
            totalSpikes += brain_->getFiringNeuronCount();
            
            // Record metrics
            recordMetric("iteration_" + std::to_string(iteration) + ".duration", duration);
            recordMetric("iteration_" + std::to_string(iteration) + ".spikes", 
                        static_cast<double>(brain_->getFiringNeuronCount()));
            
            std::cout << "    Iteration " << iteration << ": " 
                      << "duration=" << duration << "s, "
                      << "spikes=" << brain_->getFiringNeuronCount() << std::endl;
        }
        
        // Calculate performance metrics
        double avgDuration = totalTime / iterations;
        double avgSpikesPerSecond = totalSpikes / totalTime / 1000.0;  // thousands per second
        
        recordMetric("avg_duration_per_iteration", avgDuration);
        recordMetric("avg_spikes_per_thousand_steps", avgSpikesPerSecond);
        
        // Performance benchmarks
        assertCondition(avgDuration < 10.0, "Average iteration time should be < 10s");
        assertCondition(avgSpikesPerSecond > 1.0, "Should process at least 1K spikes per second");
        
        std::cout << "    Basic performance test PASSED: "
                  << "avg_duration=" << avgDuration << "s, "
                  << "spikes_per_1000s=" << avgSpikesPerSecond << std::endl;
    }
};

// Test scalability with increasing system size
class ScalabilityTest : public integration_tests::IntegrationTestBase {
public:
    void setupConfiguration() override {
        config_->set("neuron_count", 200);
        config_->set("region_count", 2);
        config_->set("connection_probability", 0.08f);
        config_->set("simulation_timestep", 0.001);
        config_->set("random_seed", 44444u);
    }
    
    void setupSystem() override {
        brain_ = std::make_shared<Brain>(config_);
        assertCondition(brain_->initialize(), "Brain initialization failed");
    }
    
    void runTest() override {
        std::vector<double> scalingResults;
        
        // Test different system sizes
        std::vector<size_t> neuronCounts = {200, 400, 600, 800};
        
        for (size_t neuronCount : neuronCounts) {
            config_->set("neuron_count", neuronCount);
            
            // Reinitialize brain with new size
            brain_->reset();
            brain_->initialize();
            
            auto start = std::chrono::high_resolution_clock::now();
            
            // Run scaled simulation
            for (int step = 0; step < 100; ++step) {
                brain_->step(step);
            }
            
            auto end = std::chrono::high_resolution_clock::now();
            double duration = std::chrono::duration<double>(end - start).count();
            
            double spikesPerNeuron = static_cast<double>(brain_->getFiringNeuronCount()) / neuronCount;
            
            recordMetric("neuron_count_" + std::to_string(neuronCount) + ".duration", duration);
            recordMetric("neuron_count_" + std::to_string(neuronCount) + ".spikes_per_neuron", spikesPerNeuron);
            
            scalingResults.push_back(spikesPerNeuron);
            
            std::cout << "    Scale test neurons=" << neuronCount << ": "
                      << "duration=" << duration << "s, "
                      << "spikes_per_neuron=" << spikesPerNeuron << std::endl;
        }
        
        // Verify scalability (should not degrade significantly)
        double avgSpikesPerNeuron = calculateAverage(scalingResults);
        double minSpikesPerNeuron = *std::min_element(scalingResults.begin(), scalingResults.end());
        double maxSpikesPerNeuron = *std::max_element(scalingResults.begin(), scalingResults.end());
        
        assertCondition(maxSpikesPerNeuron / minSpikesPerNeuron < 3.0, 
                       "Scalability should be reasonable (ratio < 3.0)");
        
        recordMetric("avg_spikes_per_neuron", avgSpikesPerNeuron);
        recordMetric("spikes_per_neuron_range", maxSpikesPerNeuron / minSpikesPerNeuron);
        
        std::cout << "    Scalability test PASSED: "
                  << "avg_spikes_per_neuron=" << avgSpikesPerNeuron << ", "
                  << "range_ratio=" << maxSpikesPerNeuron / minSpikesPerNeuron << std::endl;
    }
    
private:
    double calculateAverage(const std::vector<double>& values) {
        if (values.empty()) return 0.0;
        double sum = std::accumulate(values.begin(), values.end(), 0.0);
        return sum / values.size();
    }
};

// Test memory efficiency and memory pool performance
class MemoryEfficiencyTest : public integration_tests::IntegrationTestBase {
public:
    void setupConfiguration() override {
        config_->set("neuron_count", 300);
        config_->set("region_count", 2);
        config_->set("connection_probability", 0.07f);
        config_->set("simulation_timestep", 0.001);
        config_->set("random_seed", 55555u);
        config_->set("memory_pool_enabled", true);
        config_->set("sparse_connectivity_enabled", true);
    }
    
    void setupSystem() override {
        brain_ = std::make_shared<Brain>(config_);
        assertCondition(brain_->initialize(), "Brain initialization failed");
    }
    
    void runTest() override {
        const int stressIterations = 100;
        const int stepsPerIteration = 50;
        double totalMemoryUsage = 0.0;
        size_t totalMemoryAllocations = 0;
        
        for (int iteration = 0; iteration < stressIterations; ++iteration) {
            auto start = std::chrono::high_resolution_clock::now();
            
            // Run memory-intensive simulation
            for (int step = 0; step < stepsPerIteration; ++step) {
                brain_->step(step);
                
                // Create memory pressure with varying activity patterns
                if (iteration % 10 == 0) {
                    // High activity
                    for (size_t i = 0; i < std::min(size_t(20), brain_->getTotalNeuronCount()); ++i) {
                        brain_->injectCurrentToNeurons(static_cast<NeuronType>(i % 3), 50.0f);
                    }
                } else {
                    // Normal activity
                    brain_->injectCurrentToNeurons(NeuronType::Sensory, 20.0f);
                }
            }
            
            auto end = std::chrono::high_resolution_clock::now();
            double duration = std::chrono::duration<double>(end - start).count();
            totalMemoryUsage += duration;
            
            // Simulate memory tracking
            totalMemoryAllocations += brain_->getTotalNeuronCount() / 10;
            
            // Record memory metrics
            recordMetric("stress_iteration_" + std::to_string(iteration) + ".duration", duration);
            recordMetric("stress_iteration_" + std::to_string(iteration) + ".memory_allocations", 
                        static_cast<double>(totalMemoryAllocations));
            
            std::cout << "    Stress iteration " << iteration << ": " 
                      << "duration=" << duration << "s, "
                      << "memory_allocations=" << totalMemoryAllocations << std::endl;
        }
        
        // Calculate memory efficiency metrics
        double avgMemoryUsage = totalMemoryUsage / stressIterations;
        double memoryAllocationsPerSecond = totalMemoryAllocations / totalMemoryUsage;
        
        recordMetric("avg_memory_usage_per_iteration", avgMemoryUsage);
        recordMetric("memory_allocations_per_second", memoryAllocationsPerSecond);
        
        // Memory efficiency benchmarks
        assertCondition(avgMemoryUsage < 5.0, "Average memory usage should be < 5s");
        assertCondition(memoryAllocationsPerSecond > 100.0, 
                       "Should allocate memory at reasonable rate");
        
        std::cout << "    Memory efficiency test PASSED: "
                  << "avg_memory_usage=" << avgMemoryUsage << "s, "
                  << "allocations_per_second=" << memoryAllocationsPerSecond << std::endl;
    }
};

// Benchmark plasticity computation performance
class PlasticityPerformanceTest : public integration_tests::IntegrationTestBase {
public:
    void setupConfiguration() override {
        config_->set("neuron_count", 400);
        config_->set("region_count", 2);
        config_->set("connection_probability", 0.09f);
        config_->set("simulation_timestep", 0.001);
        config_->set("random_seed", 66666u);
        config_->set("stdp_enabled", true);
        config_->set("hebbian_enabled", true);
        config_->set("structural_plasticity_enabled", true);
    }
    
    void setupSystem() override {
        brain_ = std::make_shared<Brain>(config_);
        assertCondition(brain_->initialize(), "Brain initialization failed");
    }
    
    void runTest() override {
        const int plasticityIterations = 200;
        double totalPlasticityTime = 0.0;
        size_t totalPlasticityEvents = 0;
        
        for (int iteration = 0; iteration < plasticityIterations; ++iteration) {
            auto start = std::chrono::high_resolution_clock::now();
            
            // Run plasticity computation
            for (int step = 0; step < 50; ++step) {
                // Create correlated activity to trigger plasticity
                if (step % 5 == 0) {
                    brain_->injectCurrentToNeurons(NeuronType::Sensory, 40.0f);
                }
                if (step % 7 == 0) {
                    brain_->injectCurrentToNeurons(static_cast<NeuronType>((step + 2) % 3), 40.0f);
                }
                
                brain_->step(step);
                
                // Count plasticity events
                totalPlasticityEvents += brain_->getPlasticityEventCount();
            }
            
            auto end = std::chrono::high_resolution_clock::now();
            double duration = std::chrono::duration<double>(end - start).count();
            totalPlasticityTime += duration;
            
            recordMetric("plasticity_iteration_" + std::to_string(iteration) + ".duration", duration);
            recordMetric("plasticity_iteration_" + std::to_string(iteration) + ".events", 
                        static_cast<double>(brain_->getPlasticityEventCount()));
            
            std::cout << "    Plasticity iteration " << iteration << ": " 
                      << "duration=" << duration << "s, "
                      << "events=" << brain_->getPlasticityEventCount() << std::endl;
        }
        
        // Calculate plasticity performance metrics
        double avgPlasticityTime = totalPlasticityTime / plasticityIterations;
        double eventsPerSecond = totalPlasticityEvents / totalPlasticityTime;
        
        recordMetric("avg_plasticity_time_per_iteration", avgPlasticityTime);
        recordMetric("plasticity_events_per_second", eventsPerSecond);
        
        // Plasticity performance benchmarks
        assertCondition(avgPlasticityTime < 2.0, "Average plasticity time should be < 2s");
        assertCondition(eventsPerSecond > 10.0, "Should process at least 10 plasticity events per second");
        
        std::cout << "    Plasticity performance test PASSED: "
                  << "avg_plasticity_time=" << avgPlasticityTime << "s, "
                  << "events_per_second=" << eventsPerSecond << std::endl;
    }
};

// Performance test runner
class PerformanceTestRunner {
public:
    static void runAllTests() {
        std::cout << "=== NLM Performance Integration Tests ===" << std::endl;
        
        BasicPerformanceTest basicTest;
        std::cout << "Running Basic Performance Test..." << std::endl;
        basicTest.run();
        if (basicTest.getMetrics().success) {
            std::cout << "  PASSED (duration: " << basicTest.getMetrics().duration << "s)" << std::endl;
        } else {
            std::cout << "  FAILED: " << basicTest.getMetrics().failureReason << std::endl;
        }
        
        ScalabilityTest scaleTest;
        std::cout << "Running Scalability Test..." << std::endl;
        scaleTest.run();
        if (scaleTest.getMetrics().success) {
            std::cout << "  PASSED (duration: " << scaleTest.getMetrics().duration << "s)" << std::endl;
        } else {
            std::cout << "  FAILED: " << scaleTest.getMetrics().failureReason << std::endl;
        }
        
        MemoryEfficiencyTest memoryTest;
        std::cout << "Running Memory Efficiency Test..." << std::endl;
        memoryTest.run();
        if (memoryTest.getMetrics().success) {
            std::cout << "  PASSED (duration: " << memoryTest.getMetrics().duration << "s)" << std::endl;
        } else {
            std::cout << "  FAILED: " << memoryTest.getMetrics().failureReason << std::endl;
        }
        
        PlasticityPerformanceTest plasticTest;
        std::cout << "Running Plasticity Performance Test..." << std::endl;
        plasticTest.run();
        if (plasticTest.getMetrics().success) {
            std::cout << "  PASSED (duration: " << plasticTest.getMetrics().duration << "s)" << std::endl;
        } else {
            std::cout << "  FAILED: " << plasticTest.getMetrics().failureReason << std::endl;
        }
        
        std::cout << "=== Performance Tests Complete ===" << std::endl;
    }
};

} // namespace performance_tests
} // namespace nlm

#endif // NLM_PERFORMANCE_TESTS_HPP
