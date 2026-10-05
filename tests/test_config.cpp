// Config Tests
#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>

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

void testConfigRemove() {
    nlm::Config config;
    
    config.set("test", 123, nlm::ConfigSource::Runtime);
    assert(config.has("test"));
    
    config.remove("test");
    assert(!config.has("test"));
    
    std::cout << "    testConfigRemove passed" << std::endl;
}

void testConfigValidation() {
    nlm::Config config;
    
    // Should have no errors initially (all required keys are set by default in main.cpp)
    config.validate();
    assert(!config.hasErrors());
    
    // Test invalid neuron_count
    config.set("neuron_count", static_cast<int64_t>(0), nlm::ConfigSource::Runtime);
    config.validate();
    assert(config.hasErrors());
    auto errors = config.getErrors();
    assert(!errors.empty());
    bool foundNeuronCountError = false;
    for (const auto& error : errors) {
        if (error.key == "neuron_count" && 
            error.message.find("greater than 0") != std::string::npos) {
            foundNeuronCountError = true;
            break;
        }
    }
    assert(foundNeuronCountError);
    
    // Reset for next test
    config.set("neuron_count", static_cast<int64_t>(500), nlm::ConfigSource::Runtime);
    config.validate();
    assert(!config.hasErrors());
    
    // Test invalid simulation_timestep
    config.set("simulation_timestep", -0.1, nlm::ConfigSource::Runtime);
    config.validate();
    assert(config.hasErrors());
    bool foundTimestepError = false;
    for (const auto& error : config.getErrors()) {
        if (error.key == "simulation_timestep" && 
            error.message.find("positive") != std::string::npos) {
            foundTimestepError = true;
            break;
        }
    }
    assert(foundTimestepError);
    
    // Reset and test valid timestep
    config.set("simulation_timestep", 0.001, nlm::ConfigSource::Runtime);
    config.validate();
    assert(!config.hasErrors());
    
    std::cout << "    testConfigValidation passed" << std::endl;
}

void runAll() {
    testConfigCreation();
    testConfigSetAndGet();
    testConfigGetOr();
    testConfigRemove();
    testConfigKeys();
    testConfigValidation();
}
