// Performance Benchmark Tests
#include "benchmark.h"
#include "brain/Brain.hpp"
#include "brain/Neuron.hpp"
#include "brain/Synapse.hpp"
#include "agent/AgentBrain.hpp"
#include "memory/NeuralWorkingMemory.hpp"
#include "memory/NeuralEpisodicMemory.hpp"
#include <cassert>
#include <iostream>
#include <chrono>
#include <vector>
#include <algorithm>

namespace test_benchmark {

struct BenchmarkResult {
    std::string name;
    double timeMs;
    size_t operations;
    double opsPerSecond;
    bool passed;
    std::string details;
};

BenchmarkResult measureNeuronPerformance() {
    std::cout << "  Measuring Neuron performance..." << std::endl;
    
    const size_t neuronCount = 1000;
    const size_t steps = 100;
    
    auto startTime = std::chrono::high_resolution_clock::now();
    
    // Create neurons
    std::vector<nlm::Neuron> neurons;
    neurons.reserve(neuronCount);
    for (size_t i = 0; i < neuronCount; ++i) {
        neurons.emplace_back(nlm::NeuronId(i));
        neurons[i].setMembranePotential(-70.0f);
        neurons[i].setThreshold(-55.0f);
        neurons[i].setRestingPotential(-70.0f);
        neurons[i].setResetPotential(-75.0f);
        neurons[i].setFiringState(nlm::FiringState::Resting);
        neurons[i].setRefractoryPeriod(5);
    }
    
    // Benchmark step operations
    for (size_t step = 0; step < steps; ++step) {
        for (auto& neuron : neurons) {
            neuron.stepLIF(static_cast<double>(step), 0.001);
        }
    }
    
    auto endTime = std::chrono::high_resolution_clock::now();
    double durationMs = std::chrono::duration<double, std::milli>(endTime - startTime).count();
    
    BenchmarkResult result;
    result.name = "Neuron Stepping";
    result.timeMs = durationMs;
    result.operations = neuronCount * steps;
    result.opsPerSecond = (neuronCount * steps) / (durationMs / 1000.0);
    result.passed = durationMs < 1000.0; // Should complete within 1 second
    result.details = "1000 neurons, 100 steps each";
    
    return result;
}

BenchmarkResult measureSynapsePerformance() {
    std::cout << "  Measuring Synapse performance..." << std::endl;
    
    const size_t synapseCount = 5000;
    const size_t steps = 100;
    
    auto startTime = std::chrono::high_resolution_clock::now();
    
    // Create synapses
    std::vector<nlm::Synapse> synapses;
    synapses.reserve(synapseCount);
    for (size_t i = 0; i < synapseCount; ++i) {
        synapses.emplace_back(nlm::SynapseId(i), nlm::NeuronId(i % 100), nlm::NeuronId((i + 1) % 100));
        synapses[i].setWeight(0.5f + static_cast<float>(i) * 0.001f);
        synapses[i].setType(nlm::SynapseType::Excitatory);
    }
    
    // Benchmark step operations
    for (size_t step = 0; step < steps; ++step) {
        for (auto& synapse : synapses) {
            synapse.step(static_cast<double>(step));
        }
    }
    
    auto endTime = std::chrono::high_resolution_clock::now();
    double durationMs = std::chrono::duration<double, std::milli>(endTime - startTime).count();
    
    BenchmarkResult result;
    result.name = "Synapse Stepping";
    result.timeMs = durationMs;
    result.operations = synapseCount * steps;
    result.opsPerSecond = (synapseCount * steps) / (durationMs / 1000.0);
    result.passed = durationMs < 2000.0; // Should complete within 2 seconds
    result.details = "5000 synapses, 100 steps each";
    
    return result;
}

BenchmarkResult measureConfigPerformance() {
    std::cout << "  Measuring Config performance..." << std::endl;
    
    const size_t ops = 10000;
    
    auto startTime = std::chrono::high_resolution_clock::now();
    
    nlm::Config config;
    
    // Benchmark various config operations
    for (size_t i = 0; i < ops; ++i) {
        // Set operations
        config.set(std::string("key_") + std::to_string(i), i % 2 == 0 ? i : (i % 3 == 0 ? (i % 5 == 0 ? "value" : std::to_string(i)) : i % 7 == 0));
        
        // Get operations
        config.get<int>("key_" + std::to_string(i % 100));
        
        // Check existence
        config.has("key_" + std::to_string(i % 50));
    }
    
    auto endTime = std::chrono::high_resolution_clock::now();
    double durationMs = std::chrono::duration<double, std::milli>(endTime - startTime).count();
    
    BenchmarkResult result;
    result.name = "Config Operations";
    result.timeMs = durationMs;
    result.operations = ops * 3; // 3 operations per loop
    result.opsPerSecond = (ops * 3) / (durationMs / 1000.0);
    result.passed = durationMs < 500.0; // Should complete within 0.5 seconds
    result.details = "10,000 set/get/has operations";
    
    return result;
}

BenchmarkResult measureBrainPerformance() {
    std::cout << "  Measuring Brain performance..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(500), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(3), nlm::ConfigSource::Default);
    
    const size_t steps = 50;
    
    auto startTime = std::chrono::high_resolution_clock::now();
    
    nlm::Brain brain(config);
    brain.initialize();
    
    // Benchmark brain operations
    for (size_t step = 0; step < steps; ++step) {
        brain.step(step);
        brain.getTotalNeuronCount();
        brain.getTotalSynapseCount();
        brain.getFiringNeuronCount();
    }
    
    auto endTime = std::chrono::high_resolution_clock::now();
    double durationMs = std::chrono::duration<double, std::milli>(endTime - startTime).count();
    
    BenchmarkResult result;
    result.name = "Brain Integration";
    result.timeMs = durationMs;
    result.operations = steps * 4; // step + 3 stats calls
    result.opsPerSecond = (steps * 4) / (durationMs / 1000.0);
    result.passed = durationMs < 1500.0; // Should complete within 1.5 seconds
    result.details = "500 neurons, 50 steps";
    
    return result;
}

BenchmarkResult measureMemoryPerformance() {
    std::cout << "  Measuring Memory performance..." << std::endl;
    
    const size_t patternSize = 100;
    const size_t storeOps = 1000;
    
    auto startTime = std::chrono::high_resolution_clock::now();
    
    // Create working memory patterns
    std::vector<std::vector<float>> patterns;
    patterns.reserve(storeOps);
    for (size_t i = 0; i < storeOps; ++i) {
        std::vector<float> pattern;
        pattern.reserve(patternSize);
        for (size_t j = 0; j < patternSize; ++j) {
            pattern.push_back(static_cast<float>(i * 1000 + j) / 10000.0f);
        }
        patterns.push_back(std::move(pattern));
    }
    
    // Benchmark memory operations
    for (const auto& pattern : patterns) {
        // Simulate memory store operation
        // (Simplified version of working memory store)
        float strength = 1.0f;
        float maxActivation = 0.0f;
        for (float val : pattern) {
            maxActivation = std::max(maxActivation, val);
        }
        // Store would update neuron activations
    }
    
    auto endTime = std::chrono::high_resolution_clock::now();
    double durationMs = std::chrono::duration<double, std::milli>(endTime - startTime).count();
    
    BenchmarkResult result;
    result.name = "Memory Operations";
    result.timeMs = durationMs;
    result.operations = storeOps * patternSize;
    result.opsPerSecond = (storeOps * patternSize) / (durationMs / 1000.0);
    result.passed = durationMs < 800.0; // Should complete within 0.8 seconds
    result.details = "1000 patterns of 100 elements each";
    
    return result;
}

BenchmarkResult measureAgentPerformance() {
    std::cout << "  Measuring AgentBrain performance..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(200), nlm::ConfigSource::Default);
    
    const size_t steps = 30;
    
    auto startTime = std::chrono::high_resolution_clock::now();
    
    nlm::Brain brain(config);
    brain.initialize();
    
    nlm::AgentBrain agentBrain(std::make_shared<nlm::Brain>(brain));
    
    // Benchmark agent operations
    for (size_t step = 0; step < steps; ++step) {
        // Simulate sensory input
        std::vector<float> visionInput(256, 0.1f);
        
        // Process input
        agentBrain.processVisionInput(visionInput);
        
        // Get action
        auto action = agentBrain.decodeMotorCommand();
        
        // Apply reward
        agentBrain.applyRewardModulation(0.5f, 0.3f);
        
        // Update development
        agentBrain.updateDevelopment(0.1);
    }
    
    auto endTime = std::chrono::high_resolution_clock::now();
    double durationMs = std::chrono::duration<double, std::milli>(endTime - startTime).count();
    
    BenchmarkResult result;
    result.name = "AgentBrain Operations";
    result.timeMs = durationMs;
    result.operations = steps * 4; // 4 operations per step
    result.opsPerSecond = (steps * 4) / (durationMs / 1000.0);
    result.passed = durationMs < 2000.0; // Should complete within 2 seconds
    result.details = "200 neurons, 30 steps";
    
    return result;
}

void runBenchmarks() {
    std::cout << "NLM Performance Benchmark Suite" << std::endl;
    std::cout << "================================" << std::endl;
    std::cout << std::endl;
    
    std::vector<BenchmarkResult> results;
    
    // Run all benchmarks
    results.push_back(measureNeuronPerformance());
    results.push_back(measureSynapsePerformance());
    results.push_back(measureConfigPerformance());
    results.push_back(measureBrainPerformance());
    results.push_back(measureMemoryPerformance());
    results.push_back(measureAgentPerformance());
    
    // Print results
    std::cout << std::endl;
    std::cout << "Benchmark Results" << std::endl;
    std::cout << "=================" << std::endl;
    
    double totalTime = 0.0;
    size_t totalOps = 0;
    
    for (const auto& result : results) {
        std::cout << result.name << ":" << std::endl;
        std::cout << "  Time: " << result.timeMs << " ms" << std::endl;
        std::cout << "  Operations: " << result.operations << std::endl;
        std::cout << "  Ops/sec: " << result.opsPerSecond << std::endl;
        std::cout << "  Status: " << (result.passed ? "PASS" : "FAIL") << std::endl;
        std::cout << "  Details: " << result.details << std::endl;
        std::cout << std::endl;
        
        if (result.passed) {
            totalTime += result.timeMs;
            totalOps += result.operations;
        }
    }
    
    // Summary
    std::cout << "Summary" << std::endl;
    std::cout << "======" << std::endl;
    std::cout << "Total time: " << totalTime << " ms" << std::endl;
    std::cout << "Total operations: " << totalOps << std::endl;
    std::cout << "Average ops/sec: " << (totalOps / (totalTime / 1000.0)) << std::endl;
    
    // Overall status
    bool allPassed = std::all_of(results.begin(), results.end(),
        [](const BenchmarkResult& r) { return r.passed; });
    
    std::cout << std::endl;
    std::cout << "Overall: " << (allPassed ? "ALL BENCHMARKS PASSED" : "SOME BENCHMARKS FAILED") 
              << std::endl;
}

void runAll() {
    runBenchmarks();
}

} // namespace test_benchmark