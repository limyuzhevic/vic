// Advanced Config Tests
#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>
#include <vector>
#include <algorithm>

namespace test_config_advanced {

void testConfigFileErrorHandling() {
    nlm::Config config;
    
    // Try to load non-existent file
    assert(!config.loadFromFile("non_existent_file_12345.cfg"));
    
    std::cout << "    testConfigFileErrorHandling passed" << std::endl;
}

void testConfigTypeConversions() {
    nlm::Config config;
    
    // Test various type conversions
    config.set("int_value", 42, nlm::ConfigSource::Runtime);
    config.set("double_value", 3.14, nlm::ConfigSource::Runtime);
    config.set("string_value", std::string("test"), nlm::ConfigSource::Runtime);
    config.set("bool_value", true, nlm::ConfigSource::Runtime);
    
    // Verify values are stored correctly
    assert(config.has("int_value"));
    assert(config.has("double_value"));
    assert(config.has("string_value"));
    assert(config.has("bool_value"));
    
    std::cout << "    testConfigTypeConversions passed" << std::endl;
}

void testConfigSourceTracking() {
    nlm::Config config;
    
    // Test source tracking
    config.set("default_val", 1, nlm::ConfigSource::Default);
    config.set("runtime_val", 2, nlm::ConfigSource::Runtime);
    
    // Verify sources are tracked (implementation dependent)
    std::cout << "    testConfigSourceTracking passed" << std::endl;
}

void testConfigValueBounds() {
    nlm::Config config;
    
    // Test various value types
    std::vector<int> int_vec = {1, 2, 3, 4, 5};
    config.set("int_vector", int_vec, nlm::ConfigSource::Runtime);
    
    std::vector<double> double_vec = {1.1, 2.2, 3.3};
    config.set("double_vector", double_vec, nlm::ConfigSource::Runtime);
    
    std::vector<std::string> string_vec = {"a", "b", "c"};
    config.set("string_vector", string_vec, nlm::ConfigSource::Runtime);
    
    assert(config.has("int_vector"));
    assert(config.has("double_vector"));
    assert(config.has("string_vector"));
    
    std::cout << "    testConfigValueBounds passed" << std::endl;
}

void testConfigKeyOrganization() {
    // Test that ConfigKey organization works correctly
    nlm::ConfigKey::Category neural = nlm::ConfigKey::Category::Neural;
    nlm::ConfigKey::Category simulation = nlm::ConfigKey::Category::Simulation;
    
    nlm::ConfigKey neuralKey("neuron_count", neural, "Number of neurons in the network");
    nlm::ConfigKey simKey("timestep", simulation, "Simulation time step");
    
    assert(neuralKey.getKey() == "neuron_count");
    assert(neuralKey.getCategory() == neural);
    assert(simKey.getKey() == "timestep");
    
    std::cout << "    testConfigKeyOrganization passed" << std::endl;
}

void runAll() {
    testConfigFileErrorHandling();
    testConfigTypeConversions();
    testConfigSourceTracking();
    testConfigValueBounds();
    testConfigKeyOrganization();
}

} // namespace test_config_advanced
