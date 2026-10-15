// Performance tests for improved classes
#include "brain/Brain.hpp"
#include "brain/Neuron.hpp"
#include "brain/Synapse.hpp"
#include "core/Config/Config.hpp"
#include "core/Random/Random.hpp"
#include <cassert>
#include <iostream>
#include <chrono>
#include <vector>
#include <memory>
#include <algorithm>

namespace test_performance {

void benchmarkConfigOperations() {
    std::cout << "    Benchmarking Config operations..." << std::endl;
    
    // Test config creation and basic operations
    auto start = std::chrono::high_resolution_clock::now();
    
    for (int i = 0; i < 1000; ++i) {
        nlm::Config config;
        config.set("key1", 42, nlm::ConfigSource::Runtime);
        config.set("key2", 3.14, nlm::ConfigSource::Runtime);
        config.set("key3", std::string("test"), nlm::ConfigSource::Runtime);
        config.getOr<int>("key1", 0);
        config.getOr<double>("key2", 0.0);
        config.getOr<std::string>("key3", "");
        config.has("key1");
        config.remove("key1");
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    
    std::cout << "      Config operations: " << duration.count() << "ms for 1000 iterations" << std::endl;
    
    // Test batch operations
    start = std::chrono::high_resolution_clock::now();
    
    std::unordered_map<std::string, nlm::ConfigValue> batchValues;
    for (int i = 0; i < 100; ++i) {
        batchValues["batch_key_" + std::to_string(i)] = i;
    }
    
    for (int i = 0; i < 100; ++i) {
        nlm::Config config;
        config.setBatch(batchValues, nlm::ConfigSource::Runtime);
    }
    
    end = std::chrono::high_resolution_clock::now();
    duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    
    std::cout << "      Batch operations: " << duration.count() << "ms for 100 iterations" << std::endl;
    
    std::cout << "    testConfigOperationsPerformance passed" << std::endl;
}

void benchmarkBrainInitialization() {
    std::cout << "    Benchmarking Brain initialization..." << std::endl;
    
    // Test with small configuration
    auto start = std::chrono::high_resolution_clock::now();
    
    for (int i = 0; i < 50; ++i) {
        auto config = std::make_shared<nlm::Config>();
        config->set("neuron_count", 100, nlm::ConfigSource::Default);
        config->set("region_count", 1, nlm::ConfigSource::Default);
        config->set("connection_probability", 0.1, nlm::ConfigSource::Default);
        
        nlm::Brain brain(config);
        brain.initialize();
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    
    std::cout << "      Brain initialization: " << duration.count() << "ms for 50 iterations" << std::endl;
    
    // Test with medium configuration
    start = std::chrono::high_resolution_clock::now();
    
    for (int i = 0; i < 20; ++i) {
        auto config = std::make_shared<nlm::Config>();
        config->set("neuron_count", 500, nlm::ConfigSource::Default);
        config->set("region_count", 2, nlm::ConfigSource::Default);
        config->set("connection_probability", 0.15, nlm::ConfigSource::Default);
        config->set("stdp_ltp_weight", 0.02, nlm::ConfigSource::Default);
        
        nlm::Brain brain(config);
        brain.initialize();
        
        // Run a few steps
        for (nlm::SimulationStep step = 0; step < 10; ++step) {
            brain.step(step);
        }
    }
    
    end = std::chrono::high_resolution_clock::now();
    duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    
    std::cout << "      Medium brain simulation: " << duration.count() << "ms for 20 iterations" << std::endl;
    
    std::cout << "    testBrainInitializationPerformance passed" << std::endl;
}

void benchmarkNeuronOperations() {
    std::cout << "    Benchmarking Neuron operations..." << std::endl;
    
    auto start = std::chrono::high_resolution_clock::now();
    
    for (int i = 0; i < 1000; ++i) {
        nlm::Neuron neuron(nlm::NeuronId(i % 1000));
        
        // Various neuron operations
        neuron.setType(nlm::NeuronType::Excitatory);
        neuron.setMembranePotential(-65.0f);
        neuron.setThreshold(-50.0f);
        neuron.setRestingPotential(-70.0f);
        neuron.setResetPotential(-70.0f);
        neuron.setRefractoryPeriod(5);
        neuron.setLeakConductance(10.0f);
        
        neuron.injectCurrent(5.0f);
        neuron.receiveExcitatoryInput(2.0f);
        neuron.receiveInhibitoryInput(1.0f);
        
        neuron.recordSpike(0.1f);
        neuron.getMembranePotential();
        neuron.getThreshold();
        neuron.getRestingPotential();
        neuron.getResetPotential();
        neuron.getLeakConductance();
        neuron.getRefractoryPeriod();
        neuron.isFiring();
        neuron.isRefractory();
        
        neuron.reset();
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    
    std::cout << "      Neuron operations: " << duration.count() << "ms for 1000 iterations" << std::endl;
    
    // Test with random initialization
    start = std::chrono::high_resolution_clock::now();
    
    nlm::RandomGenerator rng(42);
    
    for (int i = 0; i < 100; ++i) {
        nlm::Neuron neuron(nlm::NeuronId(i));
        neuron.initializeRandom(rng);
    }
    
    end = std::chrono::high_resolution_clock::now();
    duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    
    std::cout << "      Neuron random init: " << duration.count() << "ms for 100 iterations" << std::endl;
    
    std::cout << "    testNeuronOperationsPerformance passed" << std::endl;
}

void benchmarkSynapseOperations() {
    std::cout << "    Benchmarking Synapse operations..." << std::endl;
    
    auto start = std::chrono::high_resolution_clock::now();
    
    for (int i = 0; i < 1000; ++i) {
        nlm::Synapse synapse(nlm::SynapseId(i % 1000), nlm::NeuronId(i % 100), nlm::NeuronId((i + 1) % 100));
        
        // Various synapse operations
        synapse.setType(nlm::SynapseType::Excitatory);
        synapse.setWeight(0.5f);
        synapse.setDelay(1);
        synapse.setEligibilityTrace(0.0f);
        synapse.setEfficacy(1.0f);
        
        synapse.getWeight();
        synapse.getDelay();
        synapse.getType();
        synapse.getEligibilityTrace();
        synapse.getEfficacy();
        synapse.isExcitatory();
        synapse.isInhibitory();
        
        synapse.recordPreSpike(0.1f);
        synapse.recordPostSpike(0.2f);
        synapse.getPreSpikeHistory();
        synapse.getPostSpikeHistory();
        
        synapse.reset();
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    
    std::cout << "      Synapse operations: " << duration.count() << "ms for 1000 iterations" << std::endl;
    
    // Test with random initialization
    start = std::chrono::high_resolution_clock::now();
    
    nlm::RandomGenerator rng(42);
    
    for (int i = 0; i < 100; ++i) {
        nlm::Synapse synapse(nlm::SynapseId(i), nlm::NeuronId(i), nlm::NeuronId(i + 1));
        synapse.initializeRandom(rng);
    }
    
    end = std::chrono::high_resolution_clock::now();
    duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    
    std::cout << "      Synapse random init: " << duration.count() << "ms for 100 iterations" << std::endl;
    
    std::cout << "    testSynapseOperationsPerformance passed" << std::endl;
}

void benchmarkConfigMemoryUsage() {
    std::cout << "    Benchmarking Config memory usage..." << std::endl;
    
    // Test config with many entries
    auto start = std::chrono::high_resolution_clock::now();
    
    for (int i = 0; i < 10; ++i) {
        nlm::Config config;
        
        // Add many entries
        for (int j = 0; j < 1000; ++j) {
            config.set(std::to_string(j), j, nlm::ConfigSource::Runtime);
        }
        
        // Access all entries
        for (int j = 0; j < 1000; ++j) {
            auto val = config.get<int>(std::to_string(j));
            assert(val.has_value());
            assert(val.value() == j);
        }
        
        // Get keys
        auto keys = config.getKeys();
        assert(keys.size() == 1000);
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    
    std::cout << "      Config memory ops: " << duration.count() << "ms for 10 configs with 1000 entries each" << std::endl;
    
    std::cout << "    testConfigMemoryUsagePerformance passed" << std::endl;
}

void benchmarkBrainSimulation() {
    std::cout << "    Benchmarking Brain simulation..." << std::endl;
    
    // Test small brain simulation
    auto start = std::chrono::high_resolution_clock::now();
    
    for (int i = 0; i < 50; ++i) {
        auto config = std::make_shared<nlm::Config>();
        config->set("neuron_count", 50, nlm::ConfigSource::Default);
        config->set("region_count", 1, nlm::ConfigSource::Default);
        config->set("connection_probability", 0.1, nlm::ConfigSource::Default);
        
        nlm::Brain brain(config);
        brain.initialize();
        
        // Run simulation
        for (nlm::SimulationStep step = 0; step < 100; ++step) {
            brain.step(step);
        }
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    
    std::cout << "      Small brain simulation: " << duration.count() << "ms for 50 simulations" << std::endl;
    
    // Test medium brain simulation
    start = std::chrono::high_resolution_clock::now();
    
    for (int i = 0; i < 20; ++i) {
        auto config = std::make_shared<nlm::Config>();
        config->set("neuron_count", 200, nlm::ConfigSource::Default);
        config->set("region_count", 2, nlm::ConfigSource::Default);
        config->set("connection_probability", 0.15, nlm::ConfigSource::Default);
        config->set("stdp_ltp_weight", 0.02, nlm::ConfigSource::Default);
        config->set("stdp_ltd_weight", 0.015, nlm::ConfigSource::Default);
        
        nlm::Brain brain(config);
        brain.initialize();
        
        // Run simulation with memory system integration
        for (nlm::SimulationStep step = 0; step < 200; ++step) {
            brain.step(step);
            
            // Occasionally test action production
            if (step % 20 == 0) {
                auto action = brain.produceAction();
                // Just verify it doesn't crash
            }
        }
    }
    
    end = std::chrono::high_resolution_clock::now();
    duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    
    std::cout << "      Medium brain simulation: " << duration.count() << "ms for 20 simulations" << std::endl;
    
    std::cout << "    testBrainSimulationPerformance passed" << std::endl;
}

void runAll() {
    std::cout << "Running Performance tests..." << std::endl;
    benchmarkConfigOperations();
    benchmarkBrainInitialization();
    benchmarkNeuronOperations();
    benchmarkSynapseOperations();
    benchmarkConfigMemoryUsage();
    benchmarkBrainSimulation();
}

} // namespace test_performance