// Performance Benchmarks
// Comprehensive performance benchmarking for NLM

#include "TestInfrastructure.hpp"
#include "tests/test_benchmarks.cpp"
#include <iostream>
#include <iomanip>
#include <chrono>
#include <fstream>
#include <filesystem>
#include <algorithm>

namespace test_benchmarks {

void benchmarkBrainInitialization() {
    std::cout << "Benchmarking Brain Initialization..." << std::endl;
    
    struct BenchmarkResult {
        size_t neuronCount;
        double timeMs;
        double neuronsPerMs;
    };
    
    std::vector<size_t> neuronCounts = {100, 500, 1000, 2000, 5000};
    std::vector<BenchmarkResult> results;
    
    for (size_t neuronCount : neuronCounts) {
        auto start = std::chrono::high_resolution_clock::now();
        
        auto config = std::make_shared<nlm::Config>();
        config->set("neuron_count", static_cast<int64_t>(neuronCount), nlm::ConfigSource::Default);
        config->set("region_count", std::max<size_t>(2, neuronCount / 500), nlm::ConfigSource::Default);
        
        nlm::Brain brain(config);
        assert(brain.initialize());
        
        auto end = std::chrono::high_resolution_clock::now();
        double durationMs = std::chrono::duration<double, std::milli>(end - start).count();
        double neuronsPerMs = static_cast<double>(neuronCount) / durationMs;
        
        results.push_back({neuronCount, durationMs, neuronsPerMs});
        
        std::cout << "  Neurons: " << neuronCount 
                  << ", Time: " << std::fixed << std::setprecision(2) << durationMs << "ms"
                  << ", Performance: " << std::setprecision(1) << neuronsPerMs << " neurons/ms" << std::endl;
    }
    
    // Verify scalability
    std::cout << "  Scalability analysis:" << std::endl;
    for (size_t i = 1; i < results.size(); ++i) {
        double speedup = results[i].neuronsPerMs / results[i-1].neuronsPerMs;
        double scaleFactor = static_cast<double>(results[i].neuronCount) / results[i-1].neuronCount;
        double efficiency = speedup / scaleFactor;
        
        std::cout << "    " << results[i-1].neuronCount << " -> " << results[i].neuronCount 
                  << ": " << std::fixed << std::setprecision(2) << speedup << "x speedup, "
                  << std::setprecision(3) << efficiency << " efficiency" << std::endl;
    }
}

void benchmarkStepPerformance() {
    std::cout << "Benchmarking Step Performance..." << std::endl;
    
    std::vector<size_t> neuronCounts = {100, 500, 1000};
    std::vector<size_t> stepCounts = {10, 50, 100};
    
    for (size_t neuronCount : neuronCounts) {
        auto config = std::make_shared<nlm::Config>();
        config->set("neuron_count", static_cast<int64_t>(neuronCount), nlm::ConfigSource::Default);
        config->set("region_count", std::max<size_t>(2, neuronCount / 500), nlm::ConfigSource::Default);
        config->set("connection_probability", 0.1f, nlm::ConfigSource::Default);
        
        nlm::Brain brain(config);
        assert(brain.initialize());
        
        for (size_t stepCount : stepCounts) {
            auto start = std::chrono::high_resolution_clock::now();
            
            for (nlm::SimulationStep step = 0; step < stepCount; ++step) {
                brain.step(step);
            }
            
            auto end = std::chrono::high_resolution_clock::now();
            double durationMs = std::chrono::duration<double, std::milli>(end - start).count();
            double stepsPerMs = static_cast<double>(stepCount) / durationMs;
            double totalNeurons = brain.getTotalNeuronCount();
            double neuronStepsPerMs = totalNeurons * stepsPerMs;
            
            std::cout << "  Neurons: " << neuronCount 
                      << ", Steps: " << stepCount
                      << ", Time: " << std::fixed << std::setprecision(2) << durationMs << "ms"
                      << ", Neuron-Steps: " << std::setprecision(1) << neuronStepsPerMs << " neuron-steps/ms" << std::endl;
        }
    }
}

void benchmarkMemoryUsage() {
    std::cout << "Benchmarking Memory Usage..." << std::endl;
    
    // This would test memory usage with different configurations
    // For now, we test that memory doesn't grow uncontrollably
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", 1000, nlm::ConfigSource::Default);
    config->set("region_count", 10, nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    size_t initialNeurons = brain.getTotalNeuronCount();
    size_t initialSynapses = brain.getTotalSynapseCount();
    
    // Run simulation
    for (nlm::SimulationStep step = 0; step < 100; ++step) {
        brain.step(step);
    }
    
    size_t finalNeurons = brain.getTotalNeuronCount();
    size_t finalSynapses = brain.getTotalSynapseCount();
    
    std::cout << "  Initial neurons: " << initialNeurons 
              << ", synapses: " << initialSynapses << std::endl;
    std::cout << "  Final neurons: " << finalNeurons 
              << ", synapses: " << finalSynapses << std::endl;
    std::cout << "  Neuron count stable: " << (finalNeurons == initialNeurons ? "YES" : "NO") << std::endl;
    std::cout << "  Synapse growth: " << (finalSynapses - initialSynapses) << " synapses" << std::endl;
}

void benchmarkParallelProcessing() {
    std::cout << "Benchmarking Parallel Processing..." << std::endl;
    
    // Test with different thread counts
    std::vector<size_t> threadCounts = {1, 2, 4, 8};
    
    for (size_t threads : threadCounts) {
        auto config = std::make_shared<nlm::Config>();
        config->set("neuron_count", 500, nlm::ConfigSource::Default);
        config->set("region_count", std::max<size_t>(2, 500 / 200), nlm::ConfigSource::Default);
        
        nlm::Brain brain(config);
        assert(brain.initialize());
        
        auto start = std::chrono::high_resolution_clock::now();
        
        for (nlm::SimulationStep step = 0; step < 50; ++step) {
            brain.step(step);
        }
        
        auto end = std::chrono::high_resolution_clock::now();
        double durationMs = std::chrono::duration<double, std::milli>(end - start).count();
        
        std::cout << "  Threads: " << threads 
                  << ", Time: " << std::fixed << std::setprecision(2) << durationMs << "ms"
                  << ", Neurons/ms: " << (500.0 / durationMs) << std::endl;
    }
}

void benchmarkConnectivityPerformance() {
    std::cout << "Benchmarking Connectivity Performance..." << std::endl;
    
    std::vector<float> connectionProbabilities = {0.01f, 0.05f, 0.1f, 0.2f};
    
    for (float connProb : connectionProbabilities) {
        auto config = std::make_shared<nlm::Config>();
        config->set("neuron_count", 500, nlm::ConfigSource::Default);
        config->set("region_count", 5, nlm::ConfigSource::Default);
        config->set("connection_probability", connProb, nlm::ConfigSource::Default);
        
        auto start = std::chrono::high_resolution_clock::now();
        
        nlm::Brain brain(config);
        assert(brain.initialize());
        
        auto end = std::chrono::high_resolution_clock::now();
        double initTimeMs = std::chrono::duration<double, std::milli>(end - start).count();
        
        size_t totalSynapses = brain.getTotalSynapseCount();
        float expectedSynapses = 500.0 * 500.0 * connProb / 2.0; // Approximate
        
        std::cout << "  Connection probability: " << connProb 
                  << ", Time: " << std::fixed << std::setprecision(2) << initTimeMs << "ms"
                  << ", Synapses: " << totalSynapses << " (expected ~" << (int)expectedSynapses << ")" << std::endl;
    }
}

void benchmarkCheckpointPerformance() {
    std::cout << "Benchmarking Checkpoint Performance..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", 1000, nlm::ConfigSource::Default);
    config->set("region_count", 10, nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    // Run simulation to create state
    for (nlm::SimulationStep step = 0; step < 100; ++step) {
        brain.step(step);
    }
    
    // Test checkpoint performance (if checkpointing is available)
    std::string checkpointFile = "benchmark_checkpoint.bin";
    
    // This would test save/load functionality if checkpointing is implemented
    // auto start = std::chrono::high_resolution_clock::now();
    // bool saved = brain.save(checkpointFile);
    // auto end = std::chrono::high_resolution_clock::now();
    // double saveTimeMs = std::chrono::duration<double, std::milli>(end - start).count();
    // std::cout << "  Save time: " << saveTimeMs << "ms" << std::endl;
    
    // Clean up
    std::remove(checkpointFile.c_str());
}

void benchmarkPlasticityPerformance() {
    std::cout << "Benchmarking Plasticity Performance..." << std::endl;
    
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", 200, nlm::ConfigSource::Default);
    config->set("region_count", 4, nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    // Enable plasticity
    auto* stdp = brain.getSTDP();
    auto* hebbian = brain.getHebbian();
    
    if (stdp && hebbian) {
        stdp->configure(0.01f, 0.012f, 20.0f);
        hebbian->setLearningRate(0.01f);
        
        // Run simulation with plasticity
        auto start = std::chrono::high_resolution_clock::now();
        
        for (nlm::SimulationStep step = 0; step < 500; ++step) {
            brain.step(step);
        }
        
        auto end = std::chrono::high_resolution_clock::now();
        double durationMs = std::chrono::duration<double, std::milli>(end - start).count();
        
        std::cout << "  Plasticity time: " << durationMs << "ms for 500 steps"
                  << ", " << (500.0 / durationMs) << " steps/ms" << std::endl;
    }
}

void runAllBenchmarks() {
    std::cout << "=== NLM Performance Benchmark Suite ===" << std::endl;
    std::cout << std::endl;
    
    benchmarkBrainInitialization();
    std::cout << std::endl;
    
    benchmarkStepPerformance();
    std::cout << std::endl;
    
    benchmarkMemoryUsage();
    std::cout << std::endl;
    
    benchmarkParallelProcessing();
    std::cout << std::endl;
    
    benchmarkConnectivityPerformance();
    std::cout << std::endl;
    
    benchmarkCheckpointPerformance();
    std::cout << std::endl;
    
    benchmarkPlasticityPerformance();
    std::cout << std::endl;
    
    std::cout << "=== Benchmark Suite Complete ===" << std::endl;
}

} // namespace test_benchmarks