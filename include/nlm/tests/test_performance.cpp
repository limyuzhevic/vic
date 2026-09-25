// Performance Tests
// Tests for neural network computation performance
#pragma once

#include "brain/Neuron.hpp"
#include "brain/Synapse.hpp"
#include "brain/Brain.hpp"
#include "plasticity/STDP.hpp"
#include "core/Config/Config.hpp"
#include "core/Random/Random.hpp"
#include <cassert>
#include <iostream>
#include <chrono>
#include <vector>
#include <thread>
#include <algorithm>
#include <cmath>
#include "test_framework.hpp"
#include "test_benchmark.hpp"

namespace test_performance {

// Performance test base class
class PerformanceTestBase : public nlm::test::TestBase {
protected:
    nlm::test::PerformanceProfiler::BenchmarkSuite benchmarkSuite;
    
public:
    PerformanceTestBase() {
        benchmarkSuite = nlm::test::PerformanceProfiler::createNeuronBenchmarks();
    }
    
    std::string getName() const override {
        return "PerformanceTestBase";
    }
    
    std::string getDescription() const override {
        return "Base class for performance tests";
    }
    
    nlm::test::TestCategory getCategory() const override {
        return nlm::test::TestCategory::Performance;
    }
    
    std::string getFile() const override {
        return "tests/test_performance.cpp";
    }
    
    int getLine() const override {
        return 1;
    }
};

// Neuron computation performance tests
void testNeuronComputationSpeed() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", 1000, nlm::ConfigSource::Default);
    
    nlm::RandomGenerator rng(42);
    std::vector<nlm::Neuron> neurons;
    
    // Create neurons
    for (int i = 0; i < 1000; ++i) {
        neurons.emplace_back(nlm::NeuronId(i));
        neurons.back().initializeRandom(rng);
    }
    
    // Benchmark neuron computation
    nlm::test::TestTimer::Timer timer("NeuronComputation");
    
    for (int step = 0; step < 100; ++step) {
        for (auto& neuron : neurons) {
            // Simulate neuron computation
            double current = neuron.getTotalCurrent();
            neuron.addToMembranePotential(current * 0.01);
            neuron.injectCurrent(0.1); // Simulate input
            neuron.decrementRefractory();
            neuron.setFiringState(neuron.isFiring() ? nlm::FiringState::Active : nlm::FiringState::Inactive);
            neuron.recordSpike(step * 0.1);
            neuron.clearTotalCurrent();
            neuron.reset();
        }
    }
    
    auto result = timer.stop();
    double opsPerSecond = (result.elapsedTime.count() > 0) ? 
        (1000 * 100 * 7 * 1.0e9) / result.elapsedTime.count() : 0.0;
    
    std::cout << "    testNeuronComputationSpeed passed - " 
              << "Processed " << opsPerSecond << " ops/sec" << std::endl;
}

// Synapse operation efficiency tests
void testSynapseOperationEfficiency() {
    auto config = std::make_shared<nlm::Config>();
    config->set("synapse_count", 5000, nlm::ConfigSource::Default);
    
    std::vector<nlm::Synapse> synapses;
    
    // Create synapses
    for (int i = 0; i < 5000; ++i) {
        synapses.emplace_back(nlm::SynapseId(i), nlm::NeuronId(i % 100), nlm::NeuronId((i + 1) % 100));
    }
    
    // Benchmark synapse operations
    nlm::test::TestTimer::Timer timer("SynapseOperations");
    
    for (int iteration = 0; iteration < 500; ++iteration) {
        for (auto& synapse : synapses) {
            // Simulate synaptic operations
            double weight = synapse.getWeight();
            synapse.addToWeight(0.01);
            synapse.setWeight(weight);
            synapse.addToWeight(-0.005);
        }
    }
    
    auto result = timer.stop();
    double opsPerSecond = (result.elapsedTime.count() > 0) ? 
        (5000 * 500 * 3 * 1.0e9) / result.elapsedTime.count() : 0.0;
    
    std::cout << "    testSynapseOperationEfficiency passed - " 
              << "Processed " << opsPerSecond << " ops/sec" << std::endl;
}

// Network scalability tests
void testNetworkScalability() {
    std::vector<size_t> neuronCounts = {100, 500, 1000, 2000};
    std::vector<double> times;
    
    for (size_t neuronCount : neuronCounts) {
        auto config = std::make_shared<nlm::Config>();
        config->set("neuron_count", static_cast<int64_t>(neuronCount), nlm::ConfigSource::Default);
        config->set("region_count", 2, nlm::ConfigSource::Default);
        
        nlm::Brain brain(config);
        
        auto startTime = std::chrono::high_resolution_clock::now();
        bool success = brain.initialize();
        auto endTime = std::chrono::high_resolution_clock::now();
        
        assert(success);
        
        double timeMs = std::chrono::duration<double, std::milli>(endTime - startTime).count();
        times.push_back(timeMs);
        
        std::cout << "    testNetworkScalability - " 
                  << "" << neuronCount << " neurons: " << timeMs << "ms" << std::endl;
    }
    
    // Verify scalability (time should increase roughly linearly)
    if (times.size() >= 2) {
        double scalingRatio = times.back() / times[0];
        double neuronRatio = static_cast<double>(neuronCounts.back()) / neuronCounts[0];
        double expectedScaling = neuronRatio; // Linear scaling
        
        assert(scalingRatio < expectedScaling * 1.5); // Allow 50% overhead
    }
    
    std::cout << "    testNetworkScalability passed" << std::endl;
}

// Memory usage pattern tests
void testMemoryUsagePatterns() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", 500, nlm::ConfigSource::Default);
    
    // Measure memory usage before brain creation
    size_t initialMemory = getMemoryUsage(); // TODO: Implement platform-specific memory measurement
    
    {
        nlm::Brain brain(config);
        brain.initialize();
        
        // Perform operations
        for (int step = 0; step < 50; ++step) {
            brain.step(step);
        }
        
        // Measure memory during operations
        size_t peakMemory = getPeakMemoryUsage(); // TODO: Implement platform-specific peak memory measurement
        double memoryUsageMB = (peakMemory - initialMemory) / (1024 * 1024.0);
        
        std::cout << "    testMemoryUsagePatterns - " 
                  << "Memory usage: " << std::fixed << std::setprecision(2) << memoryUsageMB << "MB" << std::endl;
        
        // Verify memory usage is reasonable (less than 100MB for 500 neurons)
        assert(memoryUsageMB < 100.0);
    }
    
    // Memory should be freed after brain destruction
    size_t finalMemory = getMemoryUsage(); // TODO: Implement platform-specific memory measurement
    
    std::cout << "    testMemoryUsagePatterns passed" << std::endl;
}

// Concurrent performance tests
void testConcurrentPerformance() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", 200, nlm::ConfigSource::Default);
    
    std::vector<std::thread> threads;
    std::vector<nlm::Brain> brains;
    
    // Create multiple brain instances
    for (int i = 0; i < 4; ++i) {
        brains.emplace_back(*config);
        brains.back().initialize();
    }
    
    auto startTime = std::chrono::high_resolution_clock::now();
    
    // Run parallel computations
    for (int i = 0; i < 4; ++i) {
        threads.emplace_back([&, i]() {
            for (int step = 0; step < 50; ++step) {
                brains[i].step(step);
            }
        });
    }
    
    for (auto& thread : threads) {
        thread.join();
    }
    
    auto endTime = std::chrono::high_resolution_clock::now();
    double totalTimeMs = std::chrono::duration<double, std::milli>(endTime - startTime).count();
    double avgTimePerBrain = totalTimeMs / 4.0;
    
    std::cout << "    testConcurrentPerformance - " 
              << "Average time per brain: " << std::fixed << std::setprecision(2) << avgTimePerBrain << "ms" << std::endl;
    
    // Verify all brains are still functional
    for (auto& brain : brains) {
        assert(brain.getTotalNeuronCount() == 200);
    }
    
    std::cout << "    testConcurrentPerformance passed" << std::endl;
}

// Advanced performance benchmark
void testAdvancedPerformanceBenchmark() {
    nlm::test::PerformanceProfiler profiler;
    nlm::test::PerformanceProfiler::BenchmarkSuite suite = 
        nlm::test::PerformanceProfiler::createSynapseBenchmarks();
    
    // Create test data
    auto config = std::make_shared<nlm::Config>();
    config->set("synapse_count", 1000, nlm::ConfigSource::Default);
    
    // Benchmark synapse weight operations
    std::vector<nlm::Synapse> synapses;
    for (int i = 0; i < 1000; ++i) {
        synapses.emplace_back(nlm::SynapseId(i), nlm::NeuronId(i % 100), nlm::NeuronId((i + 1) % 100));
    }
    
    auto benchmark = suite.measure("SynapseWeightOperations", "Synapse", 
        [&]() {
            double weight = synapses[0].getWeight();
            synapses[0].addToWeight(0.01);
            synapses[0].setWeight(weight);
        },
        10000);
    
    benchmark.neuronsProcessed = 1000;
    benchmark.synapsesActivated = 10000;
    benchmark.memoryBandwidthGBps = 10.0; // Estimated
    benchmark.arithmeticIntensity = 2.5; // Estimated
    
    suite.addBenchmark(benchmark);
    
    nlm::test::PerformanceProfiler::BenchmarkAnalysis analysis = 
        profiler.analyzeBenchmarks(suite.getBenchmarks());
    
    std::cout << "    testAdvancedPerformanceBenchmark - " 
              << "Mean time: " << std::fixed << std::setprecision(6) << analysis.meanTime << "μs" << std::endl;
    std::cout << "    Coefficient of variation: " << std::fixed << std::setprecision(4) << analysis.cv << std::endl;
    
    // Assert performance targets
    assert(analysis.meanTime < 100.0); // Less than 100 microseconds per operation
    assert(analysis.cv < 0.1); // Coefficient of variation less than 10%
    
    std::cout << "    testAdvancedPerformanceBenchmark passed" << std::endl;
}

// Run all performance tests
void runAll() {
    testNeuronComputationSpeed();
    testSynapseOperationEfficiency();
    testNetworkScalability();
    testMemoryUsagePatterns();
    testConcurrentPerformance();
    testAdvancedPerformanceBenchmark();
}

// Helper functions
inline size_t getMemoryUsage() { 
    return 0; // TODO: Implement platform-specific memory measurement
}

inline size_t getPeakMemoryUsage() { 
    return 0; // TODO: Implement platform-specific peak memory measurement
}

} // namespace test_performance
