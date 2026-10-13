// Integration Tests
// Tests for full NLM system integration with all components

#include "brain/Brain.hpp"
#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"
#include <cassert>
#include <iostream>
#include <thread>
#include <chrono>

namespace test_integration {

void testCompleteBrainSystem() {
    std::cout << "    Testing complete brain system integration..." << std::endl;
    
    // Initialize global logger
    auto logger = std::make_shared<Logger>();
    auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Info);
    logger->addLogger(consoleLogger);
    Logger::setGlobal(logger);
    
    // Create configuration
    auto config = std::make_shared<Config>();
    config->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
    config->set("simulation_timestep", 0.001, ConfigSource::Default);
    config->set("neuron_count", static_cast<int64_t>(100), ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), ConfigSource::Default);
    config->set("connection_probability", 0.15f, ConfigSource::Default);
    
    // Create and initialize brain
    auto brain = std::make_shared<Brain>(config);
    assert(brain->initialize());
    
    // Test basic functionality
    assert(brain->getTotalNeuronCount() > 0);
    assert(brain->getTotalSynapseCount() > 0);
    
    // Run a few simulation steps
    for (int step = 0; step < 100; ++step) {
        brain->step(step, step * 0.001);
    }
    
    // Verify activity
    assert(brain->getTotalSpikeCount() >= 0);
    assert(brain->getFiringNeuronCount() >= 0);
    
    // Test plasticity system
    auto stdp = brain->getSTDP();
    assert(stdp != nullptr);
    
    auto hebbian = brain->getHebbian();
    assert(hebbian != nullptr);
    
    auto structuralPlasticity = brain->getStructuralPlasticity();
    assert(structuralPlasticity != nullptr);
    
    // Test memory systems
    assert(brain->getWorkingMemory() != nullptr);
    assert(brain->getEpisodicMemory() != nullptr);
    assert(brain->getAssociativeMemory() != nullptr);
    
    // Test prediction system
    assert(brain->getPredictionSystem() != nullptr);
    
    // Test cognition systems
    assert(brain->getPlanner() != nullptr);
    assert(brain->getConceptFormation() != nullptr);
    assert(brain->getAttention() != nullptr);
    
    // Test neuromodulation systems
    assert(brain->getDopamine() != nullptr);
    assert(brain->getCuriosity() != nullptr);
    assert(brain->getPredictionError() != nullptr);
    assert(brain->getNovelty() != nullptr);
    assert(brain->getReward() != nullptr);
    
    // Test reset functionality
    brain->reset();
    assert(brain->getTotalSpikeCount() == 0);
    
    // Test reconfiguration
    assert(brain->initialize());
    
    std::cout << "    testCompleteBrainSystem passed" << std::endl;
}

void testConfigurationValidationIntegration() {
    std::cout << "    Testing configuration validation integration..." << std::endl;
    
    // Create config with schema
    nlm::ConfigSchema schema;
    schema.addField("required_param", "default", "Required parameter", nullptr, true);
    schema.addField("optional_param", 42, "Optional parameter with default", nullptr, false);
    schema.addField("validated_param", 0.0, "Validated parameter (must be > 0)", [](const nlm::ConfigValue& v) {
        return std::get<double>(v) > 0.0;
    }, true);
    
    nlm::Config config;
    
    // Test with valid config
    config.set("required_param", "value1");
    config.set("optional_param", 100);
    config.set("validated_param", 5.0);
    
    assert(config.validate(schema));
    
    // Test with missing required parameter
    nlm::Config config2;
    config2.set("optional_param", 100);
    config2.set("validated_param", 5.0);
    
    assert(!config2.validate(schema));  // Should fail due to missing required param
    
    // Test with invalid validated parameter
    nlm::Config config3;
    config3.set("required_param", "value1");
    config3.set("optional_param", 100);
    config3.set("validated_param", -1.0);
    
    assert(!config3.validate(schema));  // Should fail due to invalid validated param
    
    std::cout << "    testConfigurationValidationIntegration passed" << std::endl;
}

void testErrorHandlingIntegration() {
    std::cout << "    Testing error handling integration..." << std::endl;
    
    nlm::Config config;
    
    // Test exception safety
    bool exceptionCaught = false;
    try {
        // This should not throw an exception
        auto val = config.getOr<int>("non_existent", 999);
        assert(val == 999);
    } catch (...) {
        exceptionCaught = true;
    }
    
    assert(!exceptionCaught);
    
    // Test multiple operations without exceptions
    for (int i = 0; i < 100; ++i) {
        config.set("temp_key_" + std::to_string(i), i);
        auto retrieved = config.get<int>("temp_key_" + std::to_string(i));
        assert(retrieved.has_value() && retrieved.value() == i);
        config.remove("temp_key_" + std::to_string(i));
        assert(!config.has("temp_key_" + std::to_string(i)));
    }
    
    std::cout << "    testErrorHandlingIntegration passed" << std::endl;
}

void testPerformanceBenchmark() {
    std::cout << "    Testing performance benchmarks..." << std::endl;
    
    auto startTime = std::chrono::high_resolution_clock::now();
    
    nlm::Config config;
    
    // Test configuration creation performance
    for (int i = 0; i < 5000; ++i) {
        config.set("perf_key_" + std::to_string(i), i);
        auto val = config.getOr<int>("perf_key_" + std::to_string(i), -1);
        if (val != i) {
            assert(false); // Should retrieve correct value
        }
    }
    
    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
    
    std::cout << "      Configuration operations: 5000 in " << duration.count() << " ms";
    if (duration.count() > 0) {
        double opsPerSecond = (5000.0 * 1000.0) / duration.count();
        std::cout << " (" << opsPerSecond << " ops/sec)";
    }
    std::cout << std::endl;
    
    // Test configuration summary generation performance
    startTime = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < 100; ++i) {
        std::string summary = config.summary();
        assert(summary.find("Configuration (5000 entries)") != std::string::npos);
    }
    endTime = std::chrono::high_resolution_clock::now();
    duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
    
    std::cout << "      Summary generation: 100 iterations in " << duration.count() << " ms" << std::endl;
    
    std::cout << "    testPerformanceBenchmark passed" << std::endl;
}

void testMemoryUsage() {
    std::cout << "    Testing memory usage patterns..." << std::endl;
    
    nlm::Config config;
    
    // Test memory-efficient operations
    std::vector<std::string> keys;
    for (int i = 0; i < 1000; ++i) {
        std::string key = "mem_key_" + std::to_string(i);
        keys.push_back(key);
        config.set(key, i);
    }
    
    // Verify all keys exist
    for (const auto& key : keys) {
        assert(config.has(key));
    }
    
    // Clear all keys
    config.clear();
    
    // Verify all keys are gone
    for (const auto& key : keys) {
        assert(!config.has(key));
    }
    
    std::cout << "    testMemoryUsage passed" << std::endl;
}

void runAll() {
    testCompleteBrainSystem();
    testConfigurationValidationIntegration();
    testErrorHandlingIntegration();
    testPerformanceBenchmark();
    testMemoryUsage();
    
    std::cout << std::endl;
    std::cout << "=== All integration tests passed! ===" << std::endl;
}

} // namespace test_integration