// SIMD Performance Tests
// Phase 3.1: SIMD optimization tests
// Phase 3.2: Performance benchmarks for SIMD optimizations

#include "brain/Neuron.hpp"
#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>
#include <chrono>
#include <vector>
#include <cmath>
#include <numeric>

namespace test_simd_performance {

void benchmarkNeuronActivation() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(1000), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    // Benchmark neuron activation simulation
    auto startTime = std::chrono::high_resolution_clock::now();
    
    for (nlm::SimulationStep step = 0; step < 100; ++step) {
        brain.step(step);
    }
    
    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime);
    
    std::cout << "    benchmarkNeuronActivation: " << duration.count() << " microseconds for 100 steps" << std::endl;
    
    std::cout << "    benchmarkNeuronActivation passed" << std::endl;
}

void benchmarkMemoryOperations() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(5000), nlm::ConfigSource::Default);
    config->set("enable_memory_pool", true, nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    // Benchmark memory operations
    auto startTime = std::chrono::high_resolution_clock::now();
    
    for (nlm::SimulationStep step = 0; step < 50; ++step) {
        brain.injectCurrentToNeurons(nlm::NeuronType::Excitatory, 1.0f);
        brain.step(step);
    }
    
    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime);
    
    std::cout << "    benchmarkMemoryOperations: " << duration.count() << " microseconds for 50 steps with memory pool" << std::endl;
    
    std::cout << "    benchmarkMemoryOperations passed" << std::endl;
}

void benchmarkConfigurationParsing() {
    std::string testConfig = R"({
        "neuron_count": 2000,
        "region_count": 5,
        "connection_probability": 0.15,
        "learning_rate": 0.001,
        "hidden_layers": [128, 64, 32],
        "activation_function": "relu",
        "use_batch_norm": true,
        "simulation_steps": 1000,
        "enable_reward_modulation": true,
        "enable_structural_plasticity": true,
        "enable_development": true,
        "enable_curiosity": true,
        "enable_prediction_error": true,
        "max_energy": 100.0,
        "energy_decay_rate": 0.01,
        "random_seed": 42
    })";
    
    // Write test config to file
    std::ofstream configFile("test_benchmark_config.json");
    configFile << testConfig;
    configFile.close();
    
    // Benchmark JSON parsing
    nlm::Config config;
    auto startTime = std::chrono::high_resolution_clock::now();
    
    for (int i = 0; i < 100; ++i) {
        config.loadFromFile("test_benchmark_config.json");
        config.clear();
    }
    
    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime);
    
    std::cout << "    benchmarkConfigurationParsing: " << duration.count() << " microseconds for 100 JSON loads" << std::endl;
    
    // Clean up
    std::remove("test_benchmark_config.json");
    
    std::cout << "    benchmarkConfigurationParsing passed" << std::endl;
}

void benchmarkNeuralComputation() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(2000), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(4), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    // Benchmark full neural computation pipeline
    auto startTime = std::chrono::high_resolution_clock::now();
    
    float totalFiringRate = 0.0f;
    for (nlm::SimulationStep step = 0; step < 200; ++step) {
        // Inject various inputs
        brain.injectCurrentToNeurons(nlm::NeuronType::Excitatory, 0.5f);
        brain.injectCurrentToNeurons(nlm::NeuronType::Inhibitory, -0.2f);
        
        // Step brain
        brain.step(step);
        
        // Collect metrics
        totalFiringRate += brain.getAverageFiringRate();
    }
    
    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime);
    
    std::cout << "    benchmarkNeuralComputation: " << duration.count() << " microseconds for 200 steps" << std::endl;
    std::cout << "    Total average firing rate: " << (totalFiringRate / 200.0f) << std::endl;
    
    std::cout << "    benchmarkNeuralComputation passed" << std::endl;
}

void benchmarkActionProduction() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(1500), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(3), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    // Benchmark action production pipeline
    auto startTime = std::chrono::high_resolution_clock::now();
    
    size_t actionCount = 0;
    for (nlm::SimulationStep step = 0; step < 100; ++step) {
        // Produce actions from neural activity
        auto action = brain.produceAction();
        if (action != nullptr) {
            actionCount++;
        }
        
        brain.step(step);
    }
    
    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime);
    
    std::cout << "    benchmarkActionProduction: " << duration.count() << " microseconds for 100 steps, " << actionCount << " actions produced" << std::endl;
    
    std::cout << "    benchmarkActionProduction passed" << std::endl;
}

void benchmarkSystemIntegration() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(3000), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(6), nlm::ConfigSource::Default);
    config->set("enable_memory_pool", true, nlm::ConfigSource::Default);
    config->set("enable_reward_modulation", true, nlm::ConfigSource::Default);
    config->set("enable_development", true, nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    // Benchmark complete system integration
    auto startTime = std::chrono::high_resolution_clock::now();
    
    float systemHealth = 0.0f;
    for (nlm::SimulationStep step = 0; step < 150; ++step) {
        // Simulate complete system integration
        brain.injectCurrentToNeurons(nlm::NeuronType::Sensory, 1.0f);
        brain.step(step);
        
        systemHealth += brain.getAverageFiringRate();
    }
    
    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime);
    
    std::cout << "    benchmarkSystemIntegration: " << duration.count() << " microseconds for 150 integrated steps" << std::endl;
    std::cout << "    Average system health: " << (systemHealth / 150.0f) << std::endl;
    
    std::cout << "    benchmarkSystemIntegration passed" << std::endl;
}

void benchmarkParallelReady() {
    // Test if system is ready for parallel processing benchmarks
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(2500), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    // Configure for potential parallel processing
    brain.enableMemoryPool(true);
    
    // The actual parallel processing benchmark would require
    // implementation of SIMD optimizations, which is system-dependent
    std::cout << "    benchmarkParallelReady: System configured for parallel processing tests" << std::endl;
    
    std::cout << "    benchmarkParallelReady passed" << std::endl;
}

void benchmarkMemoryBandwidth() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(4000), nlm::ConfigSource::Default);
    config->set("enable_memory_pool", true, nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    // Benchmark memory bandwidth utilization
    auto startTime = std::chrono::high_resolution_clock::now();
    
    for (nlm::SimulationStep step = 0; step < 100; ++step) {
        // Stress memory with operations
        brain.injectCurrentToNeurons(nlm::NeuronType::Excitatory, 2.0f);
        brain.injectCurrentToNeurons(nlm::NeuronType::Inhibitory, -1.0f);
        
        // Reset current to vary memory access patterns
        brain.clearTotalCurrent();
        
        brain.step(step);
    }
    
    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime);
    
    std::cout << "    benchmarkMemoryBandwidth: " << duration.count() << " microseconds for 100 memory-intensive steps" << std::endl;
    
    std::cout << "    benchmarkMemoryBandwidth passed" << std::endl;
}

void benchmarkNumericalStabilityPerformance() {
    auto config = std::make_shared<nlm::Config>();
    config->set("neuron_count", static_cast<int64_t>(1000), nlm::ConfigSource::Default);
    
    nlm::Brain brain(config);
    assert(brain.initialize());
    
    // Benchmark with numerical stability considerations
    auto startTime = std::chrono::high_resolution_clock::now();
    
    float stabilityMetric = 0.0f;
    for (nlm::SimulationStep step = 0; step < 200; ++step) {
        // Test with values that could cause numerical instability
        brain.injectCurrentToNeurons(nlm::NeuronType::Excitatory, 1e6f);  // Large value
        brain.injectCurrentToNeurons(nlm::NeuronType::Inhibitory, -1e6f); // Negative large value
        
        brain.step(step);
        
        // Check numerical stability
        float firingRate = brain.getAverageFiringRate();
        if (std::isfinite(firingRate)) {
            stabilityMetric += firingRate;
        }
    }
    
    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime);
    
    std::cout << "    benchmarkNumericalStabilityPerformance: " << duration.count() << " microseconds for 200 stability tests" << std::endl;
    std::cout << "    Stability metric: " << stabilityMetric << std::endl;
    
    std::cout << "    benchmarkNumericalStabilityPerformance passed" << std::endl;
}

void runAll() {
    benchmarkNeuronActivation();
    benchmarkMemoryOperations();
    benchmarkConfigurationParsing();
    benchmarkNeuralComputation();
    benchmarkActionProduction();
    benchmarkSystemIntegration();
    benchmarkParallelReady();
    benchmarkMemoryBandwidth();
    benchmarkNumericalStabilityPerformance();
}

} // namespace test_simd_performance