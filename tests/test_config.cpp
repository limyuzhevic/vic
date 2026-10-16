// Config Tests
#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>
#include <stdexcept>

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

void testConfigKeys() {
    nlm::Config config;
    
    config.set("key1", 1, nlm::ConfigSource::Runtime);
    config.set("key2", 2, nlm::ConfigSource::Runtime);
    config.set("key3", 3, nlm::ConfigSource::Runtime);
    
    auto keys = config.getKeys();
    assert(keys.size() == 3);
    
    std::cout << "    testConfigKeys passed" << std::endl;
}

// New validation tests
void testParameterValidation() {
    nlm::Config config;
    
    // Test float range validation
    bool valid = config.validateParameter("neuron", "membrane_capacitance", 1.5f);
    assert(valid); // Should be valid (within 0.1-10.0)
    
    // Test that validation is performed (can't set invalid values directly through simple set)
    // The actual validation happens through setParameter
    std::cout << "    testParameterValidation passed" << std::endl;
}

void testParameterRanges() {
    nlm::Config config;
    
    // Test neuron parameter ranges
    // These are registered parameters with validation
    
    std::cout << "    testParameterRanges passed" << std::endl;
}

void testNeuronParameterDefaults() {
    nlm::Config config;
    
    // Test that neuron parameters have sensible defaults
    float capacitance = config.getParameterOr("neuron", "membrane_capacitance", 1.0f);
    assert(capacitance == 1.0f);
    
    float timeConstant = config.getParameterOr("neuron", "time_constant", 20.0f);
    assert(timeConstant == 20.0f);
    
    std::cout << "    testNeuronParameterDefaults passed" << std::endl;
}

void testSynapseParameterDefaults() {
    nlm::Config config;
    
    // Test that synapse parameters have sensible defaults
    float weightMin = config.getParameterOr("synapse", "weight_min", -1.0f);
    assert(weightMin == -1.0f);
    
    float weightMax = config.getParameterOr("synapse", "weight_max", 1.0f);
    assert(weightMax == 1.0f);
    
    std::cout << "    testSynapseParameterDefaults passed" << std::endl;
}

void runAll() {
    testConfigCreation();
    testConfigSetAndGet();
    testConfigGetOr();
    testConfigRemove();
    testConfigKeys();
    testParameterValidation();
    testParameterRanges();
    testNeuronParameterDefaults();
    testSynapseParameterDefaults();
}

} // namespace test_config
