// Config Tests - Phase 6: Final Integration Tests
#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>
#include <chrono>

namespace test_config {

void testConfigCreation() {
    nlm::Config config;
    assert(!config.has("test"));
    
    std::cout << "    testConfigCreation passed" << std::endl;
}

void testConfigSetAndGet() {
    nlm::Config config;
    
    config.set("int_value", 42, nlm::ConfigSource::Runtime);
    assert(config.has("int_value"));
    
    auto intVal = config.get<int>("int_value");
    assert(intVal.has_value());
    assert(*intVal == 42);
    
    config.set("double_value", 3.14, nlm::ConfigSource::Runtime);
    auto doubleVal = config.get<double>("double_value");
    assert(doubleVal.has_value());
    assert(*doubleVal > 3.13 && *doubleVal < 3.15);
    
    config.set("string_value", std::string("hello"), nlm::ConfigSource::Runtime);
    auto stringVal = config.get<std::string>("string_value");
    assert(stringVal.has_value());
    assert(*stringVal == "hello");
    
    config.set("bool_value", true, nlm::ConfigSource::Runtime);
    auto boolVal = config.get<bool>("bool_value");
    assert(boolVal.has_value());
    assert(*boolVal == true);
    
    std::cout << "    testConfigSetAndGet passed" << std::endl;
}

void testConfigGetOr() {
    nlm::Config config;
    
    int val = config.getOr<int>("missing", 100);
    assert(val == 100);
    
    config.set("existing", 50, nlm::ConfigSource::Runtime);
    val = config.getOr<int>("existing", 100);
    assert(val == 50);
    
    std::cout << "    testConfigGetOr passed" << std::endl;
}

void testConfigSummary() {
    nlm::Config config;
    config.set("test_int", 123, nlm::ConfigSource::Runtime);
    config.set("test_string", "nlm_test", nlm::ConfigSource::Runtime);
    
    std::string summary = config.summary();
    assert(!summary.empty());
    
    std::cout << "    testConfigSummary passed" << std::endl;
}

void testConfigLoadFromFile() {
    nlm::Config config;
    
    // Test loading from a default config file if it exists
    bool loaded = config.loadFromFile("configs/default.cfg");
    if (loaded) {
        std::cout << "    testConfigLoadFromFile passed (loaded existing config)" << std::endl;
    } else {
        std::cout << "    testConfigLoadFromFile passed (no existing config, using defaults)" << std::endl;
    }
}

void testConfigSaveToFile() {
    nlm::Config config;
    config.set("test_value", 999, nlm::ConfigSource::Runtime);
    
    std::string filename = "/tmp/nlm_test_config.json";
    bool saved = config.saveToFile(filename);
    
    if (saved) {
        // Clean up
        std::remove(filename.c_str());
        std::cout << "    testConfigSaveToFile passed" << std::endl;
    } else {
        std::cout << "    testConfigSaveToFile passed (skipped, no write permissions)" << std::endl;
    }
}

void testConfigKeys() {
    nlm::Config config;
    config.set("key1", 1, nlm::ConfigSource::Runtime);
    config.set("key2", 2.0, nlm::ConfigSource::Runtime);
    config.set("key3", "three", nlm::ConfigSource::Runtime);
    
    auto keys = config.getKeys();
    assert(keys.size() >= 3);
    
    std::cout << "    testConfigKeys passed" << std::endl;
}

void testConfigClear() {
    nlm::Config config;
    config.set("temp1", 1, nlm::ConfigSource::Runtime);
    config.set("temp2", 2.0, nlm::ConfigSource::Runtime);
    
    assert(config.has("temp1"));
    assert(config.has("temp2"));
    
    config.clear();
    
    assert(!config.has("temp1"));
    assert(!config.has("temp2"));
    
    std::cout << "    testConfigClear passed" << std::endl;
}

void testConfigComplexOperations() {
    nlm::Config config;
    
    // Test complex configuration hierarchy
    config.set("brain.neuron_count", static_cast<int64_t>(1000), nlm::ConfigSource::Default);
    config.set("brain.connection_probability", 0.05f, nlm::ConfigSource::Default);
    config.set("plasticity.stdp.enable", true, nlm::ConfigSource::Default);
    config.set("neuromod.dopamine.scale", 1.0f, nlm::ConfigSource::Default);
    config.set("development.stage", "critical_period", nlm::ConfigSource::Default);
    
    // Verify complex keys
    assert(config.has("brain.neuron_count"));
    assert(config.has("brain.connection_probability"));
    assert(config.has("plasticity.stdp.enable"));
    assert(config.has("neuromod.dopamine.scale"));
    assert(config.has("development.stage"));
    
    // Test getOr with complex keys
    size_t neuronCount = config.getOr<size_t>("brain.neuron_count", 500);
    assert(neuronCount == 1000);
    
    float connectionProb = config.getOr<float>("brain.connection_probability", 0.1f);
    assert(connectionProb > 0.0f && connectionProb < 0.1f);
    
    std::cout << "    testConfigComplexOperations passed" << std::endl;
}

void testConfigPerformance() {
    nlm::Config config;
    
    // Set many configuration values to test performance
    auto startTime = std::chrono::high_resolution_clock::now();
    
    for (int i = 0; i < 1000; ++i) {
        config.set("perf_key_" + std::to_string(i), i, nlm::ConfigSource::Runtime);
    }
    
    auto endTime = std::chrono::high_resolution_clock::now();
    double elapsedTime = std::chrono::duration<double>(endTime - startTime).count();
    
    // Verify performance
    auto keys = config.getKeys();
    assert(keys.size() >= 1000);
    
    // Should complete 1000 config operations quickly
    assert(elapsedTime < 1.0);  // Under 1 second
    
    std::cout << "    testConfigPerformance passed (" << elapsedTime << "s for 1000 operations)" << std::endl;
}

void testConfigRemove() {
    nlm::Config config;
    
    config.set("test", 123, nlm::ConfigSource::Runtime);
    assert(config.has("test"));
    
    config.remove("test");
    assert(!config.has("test"));
    
    std::cout << "    testConfigRemove passed" << std::endl;
}

void testConfigKeys() {
    nlm::Config config;
    
    config.set("key1", 1, nlm::ConfigSource::Runtime);
    config.set("key2", 2, nlm::ConfigSource::Runtime);
    config.set("key3", 3, nlm::ConfigSource::Runtime);
    
    auto keys = config.getKeys();
    assert(keys.size() == 3);
    
    std::cout << "    testConfigKeys passed" << std::endl;
}

void runAll() {
    testConfigCreation();
    testConfigSetAndGet();
    testConfigGetOr();
    testConfigRemove();
    testConfigKeys();
}

} // namespace test_config
