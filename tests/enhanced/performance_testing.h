// Performance Benchmarks for NLM Neural Computation
// Comprehensive performance testing for critical neural components

#ifndef PERFORMANCE_TESTING_H
#define PERFORMANCE_TESTING_H

#include "nlm_test_framework.h"
#include "brain/Neuron.hpp"
#include "brain/Synapse.hpp"
#include "plasticity/STDP.hpp"
#include "plasticity/Hebbian.hpp"
#include "core/Random/Random.hpp"
#include <gtest/gtest.h>
#include <benchmark/benchmark.h>
#include <algorithm>
#include <random>
#include <chrono>
#include <memory>
#include <vector>

// Performance measurement utilities
namespace nlm_test {
    // Benchmark framework for neural operations
    class NeuralBenchmark {
    private:
        std::string name_;
        double setup_time_us_;
        double execution_time_us_;
        uint64_t operations_count_;
        
    public:
        NeuralBenchmark(const std::string& name) : name_(name), setup_time_us_(0), 
                                                   execution_time_us_(0), operations_count_(0) {}
        
        void setSetupTime(double time_us) { setup_time_us_ = time_us; }
        void setExecutionTime(double time_us) { execution_time_us_ = time_us; }
        void setOperationsCount(uint64_t count) { operations_count_ = count; }
        
        double getOperationsPerSecond() const {
            double total_time = setup_time_us_ + execution_time_us_;
            if (total_time <= 0) return 0.0;
            return (operations_count_ * 1000000.0) / total_time;
        }
        
        double getAverageTimePerOperationNs() const {
            if (operations_count_ == 0) return 0.0;
            return (execution_time_us_ * 1000.0) / operations_count_;
        }
        
        void report() const {
            std::cout << "[BENCHMARK] " << name_ << ": " 
                      << getOperationsPerSecond() << " ops/sec, "
                      << getAverageTimePerOperationNs() << " ns/op" << std::endl;
        }
    };
    
    // Memory bandwidth measurement
    class MemoryBenchmark {
    public:
        static double measureBandwidth(size_t bytes, int iterations = 1000) {
            auto start = std::chrono::high_resolution_clock::now();
            
            for (int i = 0; i < iterations; ++i) {
                // Allocate and access memory
                void* ptr = malloc(bytes);
                if (ptr) {
                    volatile char value = 0;
                    for (size_t j = 0; j < bytes; j += sizeof(volatile char)) {
                        value += static_cast<volatile char>(ptr[j % bytes]);
                    }
                    free(ptr);
                }
            }
            
            auto end = std::chrono::high_resolution_clock::now();
            double duration_us = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
            
            double bandwidth_gb_s = (static_cast<double>(bytes) * iterations * 1e-9) / (duration_us * 1e-6);
            return bandwidth_gb_s;
        }
    };
}

// Performance Test Fixtures
class PerformanceTestBase : public ::testing::Test {
protected:
    nlm::RandomGenerator rng_;
    
    void SetUp() override {
        nlm_test::TestSeedManager::initSeed(42);
        rng_ = nlm::RandomGenerator(42);
    }
    
    // Helper methods for performance benchmarking
    template<typename Func>
    double benchmarkOperation(Func&& func, uint64_t iterations = 10000) {
        auto start = std::chrono::high_resolution_clock::now();
        
        for (uint64_t i = 0; i < iterations; ++i) {
            func(i);
        }
        
        auto end = std::chrono::high_resolution_clock::now();
        double duration_us = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
        
        return duration_us / iterations;
    }
    
    template<typename T>
    void benchmarkConstructor(uint64_t iterations = 10000) {
        auto start = std::chrono::high_resolution_clock::now();
        
        for (uint64_t i = 0; i < iterations; ++i) {
            T obj;
            obj = T(); // Reset for next iteration
        }
        
        auto end = std::chrono::high_resolution_clock::now();
        double duration_us = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
        
        std::cout << "[PERF] Constructor " << typeid(T).name() << ": " 
                  << (duration_us / iterations) << " us/op, " 
                  << (iterations * 1000000.0 / duration_us) << " ops/sec" << std::endl;
    }
};

// Neuron Performance Benchmarks
class NeuronPerformanceTest : public PerformanceTestBase {
protected:
    void testNeuronCreationPerformance() {
        const uint64_t iterations = 10000;
        
        auto start = std::chrono::high_resolution_clock::now();
        
        for (uint64_t i = 0; i < iterations; ++i) {
            nlm::Neuron neuron(nlm::NeuronId(i % 1000));
            neuron.initializeRandom(rng_);
        }
        
        auto end = std::chrono::high_resolution_clock::now();
        double duration_us = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
        
        nlm_test::NeuralBenchmark benchmark("NeuronCreation");
        benchmark.setSetupTime(0); // No setup overhead for constructor
        benchmark.setExecutionTime(duration_us);
        benchmark.setOperationsCount(iterations);
        benchmark.report();
        
        // Performance assertions
        EXPECT_LT(benchmark.getAverageTimePerOperationNs(), 10000.0) 
            << "Neuron creation should be under 10us per operation";
    }
    
    void testNeuronStateUpdatePerformance() {
        const uint64_t iterations = 10000;
        nlm::Neuron neuron(nlm::NeuronId(1));
        neuron.initializeRandom(rng_);
        
        auto start = std::chrono::high_resolution_clock::now();
        
        for (uint64_t i = 0; i < iterations; ++i) {
            neuron.setMembranePotential(-70.0f + (rng_.nextFloat() * 20.0f));
            neuron.injectCurrent(rng_.nextFloat() * 10.0f);
            neuron.addToMembranePotential(0.1f);
        }
        
        auto end = std::chrono::high_resolution_clock::now();
        double duration_us = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
        
        nlm_test::NeuralBenchmark benchmark("NeuronStateUpdate");
        benchmark.setSetupTime(0);
        benchmark.setExecutionTime(duration_us);
        benchmark.setOperationsCount(iterations * 3); // 3 operations per iteration
        benchmark.report();
        
        EXPECT_LT(benchmark.getAverageTimePerOperationNs(), 1000.0)
            << "Neuron state updates should be under 1us per operation";
    }
    
    void testNeuronFiringSimulationPerformance() {
        const uint64_t iterations = 1000;
        nlm::Neuron neuron(nlm::NeuronId(1));
        neuron.initializeRandom(rng_);
        
        auto start = std::chrono::high_resolution_clock::now();
        
        for (uint64_t i = 0; i < iterations; ++i) {
            // Simulate firing cycle
            neuron.setMembranePotential(-70.0f);
            neuron.setFiringState(nlm::FiringState::Active);
            neuron.recordSpike(0.1f * (i % 100));
            neuron.reset();
        }
        
        auto end = std::chrono::high_resolution_clock::now();
        double duration_us = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
        
        nlm_test::NeuralBenchmark benchmark("NeuronFiringSimulation");
        benchmark.setSetupTime(0);
        benchmark.setExecutionTime(duration_us);
        benchmark.setOperationsCount(iterations * 5); // 5 operations per firing
        benchmark.report();
        
        EXPECT_LT(benchmark.getAverageTimePerOperationNs(), 5000.0)
            << "Neuron firing simulation should be under 5us per operation";
    }
    
    void testNeuronMemoryAccessPerformance() {
        const uint64_t iterations = 10000;
        std::vector<std::unique_ptr<nlm::Neuron>> neurons;
        neurons.reserve(100);
        
        for (int i = 0; i < 100; ++i) {
            neurons.emplace_back(std::make_unique<nlm::Neuron>(nlm::NeuronId(i)));
            neurons.back()->initializeRandom(rng_);
        }
        
        auto start = std::chrono::high_resolution_clock::now();
        
        for (uint64_t i = 0; i < iterations; ++i) {
            size_t index = i % neurons.size();
            auto& neuron = *neurons[index];
            
            // Access various neuron properties
            volatile float potential = neuron.getMembranePotential();
            volatile float threshold = neuron.getThreshold();
            volatile bool is_firing = neuron.isFiring();
            volatile bool is_refractory = neuron.isRefractory();
            volatile float current = neuron.getTotalCurrent();
            
            // Use the values to prevent optimization
            if (potential > 0) {}
            if (threshold > 0) {}
            if (is_firing) {}
            if (is_refractory) {}
            if (current > 0) {}
        }
        
        auto end = std::chrono::high_resolution_clock::now();
        double duration_us = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
        
        nlm_test::NeuralBenchmark benchmark("NeuronMemoryAccess");
        benchmark.setSetupTime(0);
        benchmark.setExecutionTime(duration_us);
        benchmark.setOperationsCount(iterations * 5); // 5 memory accesses per iteration
        benchmark.report();
        
        EXPECT_LT(benchmark.getAverageTimePerOperationNs(), 100.0)
            << "Neuron memory access should be under 100ns per operation";
    }
};

// Synapse Performance Benchmarks
class SynapsePerformanceTest : public PerformanceTestBase {
protected:
    void testSynapseCreationPerformance() {
        const uint64_t iterations = 10000;
        
        auto start = std::chrono::high_resolution_clock::now();
        
        for (uint64_t i = 0; i < iterations; ++i) {
            nlm::Synapse synapse(nlm::SynapseId(i % 1000), nlm::NeuronId(i % 100), nlm::NeuronId((i + 1) % 100));
            synapse.setType(nlm::SynapseType::Excitatory);
            synapse.setWeight(0.5f);
        }
        
        auto end = std::chrono::high_resolution_clock::now();
        double duration_us = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
        
        nlm_test::NeuralBenchmark benchmark("SynapseCreation");
        benchmark.setSetupTime(0);
        benchmark.setExecutionTime(duration_us);
        benchmark.setOperationsCount(iterations);
        benchmark.report();
        
        EXPECT_LT(benchmark.getAverageTimePerOperationNs(), 10000.0)
            << "Synapse creation should be under 10us per operation";
    }
    
    void testSynapseTransmissionPerformance() {
        const uint64_t iterations = 10000;
        nlm::Synapse synapse(nlm::SynapseId(1), nlm::NeuronId(1), nlm::NeuronId(2));
        synapse.setType(nlm::SynapseType::Excitatory);
        synapse.setWeight(0.8f);
        
        auto start = std::chrono::high_resolution_clock::now();
        
        for (uint64_t i = 0; i < iterations; ++i) {
            // Simulate synaptic transmission
            float effect = synapse.getWeight() * 10.0f; // Typical synaptic effect
            synapse.receiveExcitatoryInput(effect);
        }
        
        auto end = std::chrono::high_resolution_clock::now();
        double duration_us = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
        
        nlm_test::NeuralBenchmark benchmark("SynapseTransmission");
        benchmark.setSetupTime(0);
        benchmark.setExecutionTime(duration_us);
        benchmark.setOperationsCount(iterations);
        benchmark.report();
        
        EXPECT_LT(benchmark.getAverageTimePerOperationNs(), 1000.0)
            << "Synapse transmission should be under 1us per operation";
    }
};

// Plasticity Rule Performance Benchmarks
class PlasticityPerformanceTest : public PerformanceTestBase {
protected:
    void testSTDPPotenciesPerformance() {
        const uint64_t iterations = 1000;
        nlm::STDP stdp;
        std::vector<nlm::Timestamp> pre_spikes = {0.0, 10.0, 20.0, 30.0};
        std::vector<nlm::Timestamp> post_spikes = {5.0, 15.0, 25.0, 35.0};
        
        auto start = std::chrono::high_resolution_clock::now();
        
        for (uint64_t i = 0; i < iterations; ++i) {
            nlm::Synapse synapse(nlm::SynapseId(i), nlm::NeuronId(i % 10), nlm::NeuronId((i + 1) % 10));
            synapse.setType(nlm::SynapseType::Excitatory);
            synapse.setWeight(0.5f + (rng_.nextFloat() * 0.5f));
            
            stdp.update(&synapse, pre_spikes, post_spikes, 0.001);
        }
        
        auto end = std::chrono::high_resolution_clock::now();
        double duration_us = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
        
        nlm_test::NeuralBenchmark benchmark("STDPPotenciesUpdate");
        benchmark.setSetupTime(0);
        benchmark.setExecutionTime(duration_us);
        benchmark.setOperationsCount(iterations);
        benchmark.report();
        
        EXPECT_LT(benchmark.getAverageTimePerOperationNs(), 50000.0)
            << "STDP update should be under 50us per operation";
    }
    
    void testHebbianLearningPerformance() {
        const uint64_t iterations = 10000;
        nlm::Hebbian hebbian;
        nlm::Synapse synapse(nlm::SynapseId(1), nlm::NeuronId(1), nlm::NeuronId(2));
        synapse.setType(nlm::SynapseType::Excitatory);
        synapse.setWeight(0.5f);
        
        auto start = std::chrono::high_resolution_clock::now();
        
        for (uint64_t i = 0; i < iterations; ++i) {
            // Hebbian learning simulation
            float pre_activity = rng_.nextFloat();
            float post_activity = rng_.nextFloat();
            
            hebbian.update(&synapse, pre_activity, post_activity, 0.001);
        }
        
        auto end = std::chrono::high_resolution_clock::now();
        double duration_us = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
        
        nlm_test::NeuralBenchmark benchmark("HebbianLearning");
        benchmark.setSetupTime(0);
        benchmark.setExecutionTime(duration_us);
        benchmark.setOperationsCount(iterations);
        benchmark.report();
        
        EXPECT_LT(benchmark.getAverageTimePerOperationNs(), 1000.0)
            << "Hebbian learning should be under 1us per operation";
    }
};

// Scalability Benchmarks
class ScalabilityTest : public PerformanceTestBase {
protected:
    void testLinearScalingWithNeurons() {
        std::vector<uint64_t> neuron_counts = {100, 1000, 10000, 50000};
        
        for (uint64_t count : neuron_counts) {
            // Create neurons
            std::vector<std::unique_ptr<nlm::Neuron>> neurons;
            neurons.reserve(count);
            
            auto start = std::chrono::high_resolution_clock::now();
            
            for (uint64_t i = 0; i < count; ++i) {
                neurons.emplace_back(std::make_unique<nlm::Neuron>(nlm::NeuronId(i)));
                neurons.back()->initializeRandom(rng_);
            }
            
            auto end = std::chrono::high_resolution_clock::now();
            double duration_us = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
            double ops_per_sec = (count * 1000000.0) / duration_us;
            
            std::cout << "[SCALING] " << count << " neurons: " << ops_per_sec << " ops/sec" << std::endl;
            
            // Verify scalability - operations per neuron should remain reasonable
            EXPECT_LT(duration_us / count, 100.0) << "Scaling with neuron count failed";
            
            neurons.clear();
        }
    }
    
    void testMemoryUsageScaling() {
        std::vector<uint64_t> neuron_counts = {1000, 10000, 100000};
        
        for (uint64_t count : neuron_counts) {
            // Measure memory usage by creating many neurons
            std::vector<std::unique_ptr<nlm::Neuron>> neurons;
            neurons.reserve(count);
            
            auto start = std::chrono::high_resolution_clock::now();
            
            for (uint64_t i = 0; i < count; ++i) {
                neurons.emplace_back(std::make_unique<nlm::Neuron>(nlm::NeuronId(i)));
                neurons.back()->initializeRandom(rng_);
            }
            
            auto end = std::chrono::high_resolution_clock::now();
            double duration_us = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
            
            size_t memory_usage_bytes = neurons.size() * sizeof(nlm::Neuron);
            double memory_per_neuron_bytes = static_cast<double>(memory_usage_bytes) / neurons.size();
            
            std::cout << "[MEMORY] " << count << " neurons: " << memory_per_neuron_bytes << " bytes/neuron, "
                      << (duration_us / count) << " us/neuron" << std::endl;
            
            // Memory usage should be reasonable (less than 1KB per neuron)
            EXPECT_LT(memory_per_neuron_bytes, 1024.0) << "Memory usage per neuron too high";
            
            neurons.clear();
        }
    }
};

// Memory Bandwidth Benchmarks
class MemoryBandwidthTest : public ::testing::Test {
protected:
    void testMemoryBandwidth() {
        std::vector<size_t> buffer_sizes = {1024, 4096, 16384, 65536, 262144}; // 1KB to 256KB
        int iterations = 1000;
        
        for (size_t size : buffer_sizes) {
            double bandwidth_gb_s = nlm_test::MemoryBenchmark::measureBandwidth(size, iterations);
            
            std::cout << "[BANDWIDTH] " << size << " bytes: " << bandwidth_gb_s << " GB/s" << std::endl;
            
            // Expect reasonable memory bandwidth (at least 1 GB/s for small buffers)
            if (size <= 4096) {
                EXPECT_GT(bandwidth_gb_s, 1.0) << "Memory bandwidth too low for small buffers";
            }
        }
    }
};

#endif // PERFORMANCE_TESTING_H
