// ConfigManager Tests
#include "core/ConfigManager.hpp"
#include <cassert>
#include <iostream>

namespace test_config_manager {

void testConfigManagerCreation() {
    nlm::ConfigManager manager;
    
    assert(manager.getConfig() != nullptr);
    assert(manager.isValid());
    assert(manager.getValidationErrors().empty());
    
    std::cout << "    testConfigManagerCreation passed" << std::endl;
}

void testConfigManagerLoadFromFile() {
    nlm::ConfigManager manager("configs/default.cfg");
    
    assert(manager.getConfig() != nullptr);
    assert(manager.isValid());
    
    // Check that some expected values are loaded
    assert(manager.get<int64_t>("neuron_count") > 0);
    assert(manager.get<double>("simulation_timestep") > 0.0);
    
    std::cout << "    testConfigManagerLoadFromFile passed" << std::endl;
}

void testConfigManagerValidation() {
    nlm::ConfigManager manager;
    
    // Set an invalid value (negative neuron count)
    manager.set("neuron_count", -5);
    
    assert(!manager.isValid());
    assert(!manager.getValidationErrors().empty());
    
    std::cout << "    testConfigManagerValidation passed" << std::endl;
}

void testConfigManagerValidationPassed() {
    nlm::ConfigManager manager;
    
    // Set valid values
    manager.set("neuron_count", 1000);
    manager.set("simulation_timestep", 0.001);
    manager.set("connection_probability", 0.15);
    
    assert(manager.isValid());
    assert(manager.getValidationErrors().empty());
    
    std::cout << "    testConfigManagerValidationPassed passed" << std::endl;
}

void testConfigManagerSaveAndReload() {
    nlm::ConfigManager manager;
    
    // Set some values
    manager.set("neuron_count", 2000);
    manager.set("simulation_timestep", 0.01);
    
    // Save to file
    assert(manager.saveToFile("test_config.json"));
    
    // Reload
    nlm::ConfigManager manager2("test_config.json");
    
    assert(manager2.get<int64_t>("neuron_count") == 2000);
    assert(manager2.get<double>("simulation_timestep") == 0.01);
    
    std::cout << "    testConfigManagerSaveAndReload passed" << std::endl;
}

void runAll() {
    testConfigManagerCreation();
    testConfigManagerLoadFromFile();
    testConfigManagerValidation();
    testConfigManagerValidationPassed();
    testConfigManagerSaveAndReload();
}

} // namespace test_config_manager