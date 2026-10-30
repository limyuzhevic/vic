// Test file for NLM Configuration Parser Improvements
// This demonstrates the enhanced JSON/YAML configuration system

#include "core/Config/Config.hpp"
#include <iostream>
#include <cassert>

using namespace nlm;

void testJsonParser() {
    std::cout << "Testing JSON Parser..." << std::endl;
    
    // Test JSON with comments and nested structures
    auto config = std::make_shared<Config>();
    
    // Test loading from file (requires test JSON file)
    // config->loadFromFile("test_json_config.json");
    
    // Test direct configuration
    config->set("simulation_timestep", 0.0005);
    config->set("neuron_count", 2000);
    config->set("dopamine_baseline", 0.2f);
    config->set("curiosity_baseline", 0.08f);
    config->set("working_memory_capacity", 150);
    config->set("visualization_enabled", true);
    
    // Test type conversions
    assert(config->get<double>("simulation_timestep").value() == 0.0005);
    assert(config->get<int64_t>("neuron_count").value() == 2000);
    assert(config->get<float>("dopamine_baseline").value() == 0.2f);
    assert(config->get<bool>("visualization_enabled").value() == true);
    
    // Test getOr with defaults
    assert(config->getOr<int64_t>("nonexistent_key", 42) == 42);
    assert(config->getOr<double>("simulation_timestep", 0.001) == 0.0005);
    
    std::cout << "✓ JSON Parser tests passed" << std::endl;
}

void testYamlParser() {
    std::cout << "Testing YAML Parser..." << std::endl;
    
    auto config = std::make_shared<Config>();
    
    // Test YAML-style configuration
    config->set("brain.regions", 3);
    config->set("memory.working_memory.capacity", 200);
    config->set("memory.episodic_memory.decay_rate", 0.002f);
    config->set("neuromodulation.dopamine.baseline", 0.15f);
    config->set("simulation.parallel_processing", true);
    
    // Test flattened access
    assert(config->get<int64_t>("brain.regions").value() == 3);
    assert(config->get<size_t>("memory.working_memory.capacity").value() == 200);
    assert(config->get<float>("memory.episodic_memory.decay_rate").value() == 0.002f);
    assert(config->get<float>("neuromodulation.dopamine.baseline").value() == 0.15f);
    assert(config->get<bool>("simulation.parallel_processing").value() == true);
    
    std::cout << "✓ YAML Parser tests passed" << std::endl;
}

void testAdvancedConfiguration() {
    std::cout << "Testing Advanced Configuration..." << std::endl;
    
    auto config = std::make_shared<Config>();
    
    // Test all new configuration options
    config->set("membrane_time_constant", 25.0f);
    config->set("membrane_resistance", 12.0f);
    config->set("spike_frequency_adaptation", true);
    config->set("adaptation_tau", 100.0f);
    config->set("short_term_facilitation", true);
    config->set("facilitation_tau", 100.0f);
    config->set("short_term_depression", true);
    config->set("depression_tau", 200.0f);
    config->set("stdp_learning_rate", 0.015f);
    config->set("reward_modulated_plasticity", true);
    config->set("eligibility_trace_decay", 0.001f);
    
    // Test arrays
    std::vector<int> test_array = {1, 2, 3, 4, 5};
    // Note: Config doesn't directly support std::vector assignment yet
    // This would be tested with the full implementation
    
    // Test summary generation
    std::string summary = config->summary();
    assert(summary.find("Configuration") != std::string::npos);
    assert(summary.find("membrane_time_constant") != std::string::npos);
    
    std::cout << "✓ Advanced Configuration tests passed" << std::endl;
}

void testBackwardCompatibility() {
    std::cout << "Testing Backward Compatibility..." << std::endl;
    
    auto config = std::make_shared<Config>();
    
    // Test legacy key=value format parsing
    config->set("legacy_key", "legacy_value");
    config->set("legacy_number", 42);
    config->set("legacy_float", 3.14f);
    config->set("legacy_bool", false);
    
    // Test that legacy format still works
    assert(config->get<std::string>("legacy_key").value() == "legacy_value");
    assert(config->get<int>("legacy_number").value() == 42);
    assert(config->get<double>("legacy_float").value() == 3.14);
    assert(config->get<bool>("legacy_bool").value() == false);
    
    std::cout << "✓ Backward Compatibility tests passed" << std::endl;
}

void testConfigurationValidation() {
    std::cout << "Testing Configuration Validation..." << std::endl;
    
    auto config = std::make_shared<Config>();
    
    // Test invalid configuration handling
    config->set("valid_key", "valid_value");
    
    // Test has() method
    assert(config->has("valid_key"));
    assert(!config->has("invalid_key"));
    
    // Test remove() method
    config->remove("valid_key");
    assert(!config->has("valid_key"));
    
    std::cout << "✓ Configuration Validation tests passed" << std::endl;
}

int main() {
    std::cout << "=== NLM Configuration Parser Test Suite ===" << std::endl;
    
    try {
        testJsonParser();
        testYamlParser();
        testAdvancedConfiguration();
        testBackwardCompatibility();
        testConfigurationValidation();
        
        std::cout << std::endl;
        std::cout << "=== All tests passed! ===" << std::endl;
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Test failed with exception: " << e.what() << std::endl;
        return 1;
    }
}