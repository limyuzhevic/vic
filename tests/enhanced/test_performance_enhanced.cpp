// Performance Benchmarks Implementation
// Actual performance testing for neural computation components

#include "enhanced/performance_testing.h"
#include <iostream>
#include <iomanip>

// Neuron Performance Benchmark Tests
TEST_F(NeuronPerformanceTest, NeuronCreationPerformance) {
    testNeuronCreationPerformance();
}

TEST_F(NeuronPerformanceTest, NeuronStateUpdatePerformance) {
    testNeuronStateUpdatePerformance();
}

TEST_F(NeuronPerformanceTest, NeuronFiringSimulationPerformance) {
    testNeuronFiringSimulationPerformance();
}

TEST_F(NeuronPerformanceTest, NeuronMemoryAccessPerformance) {
    testNeuronMemoryAccessPerformance();
}

// Synapse Performance Benchmark Tests
TEST_F(SynapsePerformanceTest, SynapseCreationPerformance) {
    testSynapseCreationPerformance();
}

TEST_F(SynapsePerformanceTest, SynapseTransmissionPerformance) {
    testSynapseTransmissionPerformance();
}

// Plasticity Rule Performance Benchmark Tests
TEST_F(PlasticityPerformanceTest, STDPPotenciesPerformance) {
    testSTDPPotenciesPerformance();
}

TEST_F(PlasticityPerformanceTest, HebbianLearningPerformance) {
    testHebbianLearningPerformance();
}

// Scalability Tests
TEST_F(ScalabilityTest, LinearScalingWithNeurons) {
    testLinearScalingWithNeurons();
}

TEST_F(ScalabilityTest, MemoryUsageScaling) {
    testMemoryUsageScaling();
}

// Memory Bandwidth Tests
TEST_F(MemoryBandwidthTest, MemoryBandwidth) {
    testMemoryBandwidth();
}

// Comprehensive Performance Integration Test
class ComprehensivePerformanceTest : public PerformanceTestBase {
protected:
    void SetUp() override {
        PerformanceTestBase::SetUp();
        
        // Create a larger neural network for realistic performance testing
        for (int i = 0; i < 500; ++i) {
            neurons_.emplace_back(std::make_unique<nlm::Neuron>(nlm::NeuronId(i)));
            neurons_.back()->initializeRandom(rng_);
        }
        
        for (int i = 0; i < 1000; ++i) {
            synapses_.emplace_back(std::make_unique<nlm::Synapse>(
                nlm::SynapseId(i), nlm::NeuronId(i % 500), nlm::NeuronId((i + 1) % 500)));
            synapses_.back()->setType(nlm::SynapseType::Excitatory);
            synapses_.back()->setWeight(0.5f + (rng_.nextFloat() * 0.5f));
        }
        
        stdp_ = std::make_unique<nlm::STDP>();
    }
    
    void TearDown() override {
        neurons_.clear();
        synapses_.clear();
        stdp_.reset();
        PerformanceTestBase::TearDown();
    }
    
    std::vector<std::unique_ptr<nlm::Neuron>> neurons_;
    std::vector<std::unique_ptr<nlm::Synapse>> synapses_;
    std::unique_ptr<nlm::STDP> stdp_;
    
    void testComprehensiveNeuralNetworkPerformance() {
        const int operations_per_iteration = 100;
        const int iterations = 100;
        
        nlm_test::PerformanceTimer timer("ComprehensiveNeuralNetworkPerformance");
        
        auto start = std::chrono::high_resolution_clock::now();
        
        for (int iter = 0; iter < iterations; ++iter) {
            // Simulate neural network operations
            for (int i = 0; i < operations_per_iteration; ++i) {
                int neuron_idx = rand() % neurons_.size();
                int synapse_idx = rand() % synapses_.size();
                
                auto& neuron = *neurons_[neuron_idx];
                auto& synapse = *synapses_[synapse_idx];
                
                // Neuron operations
                neuron.setMembranePotential(-70.0f + (rng_.nextFloat() * 20.0f));
                neuron.injectCurrent(rng_.nextFloat() * 5.0f);
                
                // Synapse operations
                synapse.receiveExcitatoryInput(neuron.getTotalCurrent() * synapse.getWeight());
                
                // Plasticity operations (less frequent)
                if (i % 10 == 0) {
                    std::vector<nlm::Timestamp> pre_spikes = {0.0, 10.0, 20.0};
                    std::vector<nlm::Timestamp> post_spikes = {5.0, 15.0, 25.0};
                    stdp_->update(&synapse, pre_spikes, post_spikes, 0.001);
                }
            }
        }
        
        auto end = std::chrono::high_resolution_clock::now();
        double total_duration_us = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
        
        nlm_test::NeuralBenchmark benchmark("ComprehensiveNeuralNetwork");
        benchmark.setSetupTime(0);
        benchmark.setExecutionTime(total_duration_us);
        benchmark.setOperationsCount(iterations * operations_per_iteration);
        benchmark.report();
        
        double ops_per_second = benchmark.getOperationsPerSecond();
        double time_per_op_ns = benchmark.getAverageTimePerOperationNs();
        
        // Performance assertions
        EXPECT_GT(ops_per_second, 1000.0) << "Neural network performance should be > 1000 ops/sec";
        EXPECT_LT(time_per_op_ns, 100000.0) << "Time per operation should be < 100us";
        
        std::cout << "Comprehensive neural network performance: " << ops_per_second << " ops/sec" << std::endl;
        std::cout << "Average operation time: " << time_per_op_ns << " ns" << std::endl;
    }
    
    void testRealisticWorkloadPerformance() {
        const int simulation_steps = 1000;
        const int neurons_per_step = 50;
        
        nlm_test::PerformanceTimer timer("RealisticWorkloadPerformance");
        
        auto start = std::chrono::high_resolution_clock::now();
        
        for (int step = 0; step < simulation_steps; ++step) {
            // Simulate one timestep of neural network activity
            for (int i = 0; i < neurons_per_step; ++i) {
                int neuron_idx = rand() % neurons_.size();
                int synapse_idx = rand() % synapses_.size();
                
                auto& neuron = *neurons_[neuron_idx];
                auto& synapse = *synapses_[synapse_idx];
                
                // Realistic neural computation
                float external_input = rng_.nextFloat() * 2.0f - 1.0f; // -1 to 1
                neuron.injectCurrent(external_input);
                
                float synaptic_input = synapse.getWeight() * neuron.getTotalCurrent();
                synapse.receiveExcitatoryInput(synaptic_input);
                
                // Update neuron state
                neuron.addToMembranePotential(0.1f * external_input);
                
                // Check for firing
                if (neuron.getMembranePotential() > -50.0f) {
                    neuron.setFiringState(nlm::FiringState::Active);
                    neuron.recordSpike(static_cast<float>(step));
                }
                
                // STDP plasticity
                if (step % 5 == 0) { // Less frequent plasticity
                    std::vector<nlm::Timestamp> pre_spikes = {0.0, 10.0};
                    std::vector<nlm::Timestamp> post_spikes = {5.0, 15.0};
                    stdp_->update(&synapse, pre_spikes, post_spikes, 0.001);
                }
            }
        }
        
        auto end = std::chrono::high_resolution_clock::now();
        double total_duration_us = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
        
        nlm_test::NeuralBenchmark benchmark("RealisticWorkload");
        benchmark.setSetupTime(0);
        benchmark.setExecutionTime(total_duration_us);
        benchmark.setOperationsCount(simulation_steps * neurons_per_step);
        benchmark.report();
        
        double ops_per_second = benchmark.getOperationsPerSecond();
        std::cout << "Realistic workload performance: " << ops_per_second << " ops/sec" << std::endl;
        
        // Performance assertions for realistic workload
        EXPECT_GT(ops_per_second, 5000.0) << "Realistic workload should be > 5000 ops/sec";
    }
};

TEST_F(ComprehensivePerformanceTest, ComprehensiveNeuralNetworkPerformance) {
    testComprehensiveNeuralNetworkPerformance();
}

TEST_F(ComprehensivePerformanceTest, RealisticWorkloadPerformance) {
    testRealisticWorkloadPerformance();
}

// Advanced Memory Performance Tests
class MemoryPerformanceTest : public ::testing::Test {
protected:
    void testMemoryAllocationEfficiency() {
        const size_t test_sizes[] = {64, 256, 1024, 4096, 16384, 65536};
        const int iterations = 1000;
        
        std::cout << "[MEMORY EFFICIENCY] Testing allocation/deallocation efficiency" << std::endl;
        
        for (size_t size : test_sizes) {
            auto start = std::chrono::high_resolution_clock::now();
            
            for (int i = 0; i < iterations; ++i) {
                void* ptr = malloc(size);
                if (ptr) {
                    // Access memory to prevent optimization
                    volatile char value = 0;
                    for (size_t j = 0; j < size; j += sizeof(volatile char)) {
                        value += static_cast<volatile char>(ptr[j % size]);
                    }
                    free(ptr);
                }
            }
            
            auto end = std::chrono::high_resolution_clock::now();
            double duration_us = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
            
            double throughput_mb_s = (static_cast<double>(size) * iterations * 1e-6) / (duration_us * 1e-6);
            double ops_per_second = (iterations * 1000000.0) / duration_us;
            
            std::cout << "  Size: " << std::setw(6) << size << " bytes | "
                      << "Throughput: " << std::setw(8) << std::fixed << std::setprecision(2) 
                      << throughput_mb_s << " MB/s | "
                      << "Ops/sec: " << std::setw(10) << std::fixed << std::setprecision(0) 
                      << ops_per_second << " | "
                      << "Time/op: " << std::setw(6) << std::fixed << std::setprecision(2)
                      << (duration_us / iterations) << " us" << std::endl;
            
            // Verify reasonable performance
            EXPECT_GT(throughput_mb_s, 100.0) << "Memory throughput too low";
        }
    }
    
    void testCachePerformance() {
        const size_t small_array_size = 1024;  // Should fit in cache
        const size_t large_array_size = 1024 * 1024;  // Won't fit in cache
        
        // Small array (cache-friendly)
        std::vector<int> small_array(small_array_size);
        std::iota(small_array.begin(), small_array.end(), 0);
        
        auto start = std::chrono::high_resolution_clock::now();
        
        for (int i = 0; i < 1000; ++i) {
            // Sequential access (cache-friendly)
            int sum = 0;
            for (size_t j = 0; j < small_array.size(); ++j) {
                sum += small_array[j];
            }
            volatile int cache_sum = sum; // Prevent optimization
        }
        
        auto end = std::chrono::high_resolution_clock::now();
        double small_duration_us = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
        
        // Large array (cache-unfriendly)
        std::vector<int> large_array(large_array_size);
        std::iota(large_array.begin(), large_array.end(), 0);
        
        start = std::chrono::high_resolution_clock::now();
        
        for (int i = 0; i < 100; ++i) {
            // Random access (cache-unfriendly)
            int sum = 0;
            for (size_t j = 0; j < large_array.size(); j += 100) { // Step through array
                sum += large_array[j];
            }
            volatile int cache_sum = sum; // Prevent optimization
        }
        
        end = std::chrono::high_resolution_clock::now();
        double large_duration_us = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
        
        std::cout << "[CACHE] Small array (sequential): " << (small_duration_us / 1000.0) << " us/1000 iterations" << std::endl;
        std::cout << "[CACHE] Large array (random): " << (large_duration_us / 100.0) << " us/100 iterations" << std::endl;
        
        // Cache performance should be reasonable
        EXPECT_LT(small_duration_us, 1000.0) << "Small array access too slow";
        EXPECT_LT(large_duration_us, 100000.0) << "Large array access too slow";
    }
    
    void testMemoryAllocationPattern() {
        const int pattern_tests = 5;
        std::vector<std::string> patterns = {
            "sequential", "random", "repeated", "alternating", "gaussian"
        };
        
        std::cout << "[ALLOCATION PATTERNS] Testing different allocation patterns" << std::endl;
        
        for (const auto& pattern : patterns) {
            auto start = std::chrono::high_resolution_clock::now();
            
            for (int i = 0; i < 10000; ++i) {
                size_t index = 0;
                
                if (pattern == "sequential") {
                    index = i % 100;
                } else if (pattern == "random") {
                    index = rand() % 100;
                } else if (pattern == "repeated") {
                    index = i % 10; // Repeat every 10 iterations
                } else if (pattern == "alternating") {
                    index = (i % 2) * 50; // Alternate between first and second half
                } else if (pattern == "gaussian") {
                    // Simplified Gaussian-like distribution
                    double gaussian_val = (rand() % 2000 - 1000) / 1000.0; // -1 to 1
                    index = static_cast<size_t>(std::max(0.0, std::min(99.0, gaussian_val * 50 + 50)));
                }
                
                // Simulate allocation at index
                void* ptr = malloc(index + 100); // Varying sizes
                if (ptr) {
                    free(ptr);
                }
            }
            
            auto end = std::chrono::high_resolution_clock::now();
            double duration_us = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
            
            std::cout << "  Pattern " << std::setw(12) << pattern << ": " 
                      << std::setw(10) << (duration_us / 10000.0) << " us/iteration" 
                      << " | " << std::setw(8) << (10000 * 1000000.0 / duration_us) << " ops/sec" << std::endl;
        }
    }
};

TEST_F(MemoryPerformanceTest, MemoryAllocationEfficiency) {
    testMemoryAllocationEfficiency();
}

TEST_F(MemoryPerformanceTest, CachePerformance) {
    testCachePerformance();
}

TEST_F(MemoryPerformanceTest, MemoryAllocationPattern) {
    testMemoryAllocationPattern();
}