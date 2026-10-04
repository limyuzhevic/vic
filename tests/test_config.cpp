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

void testConfigValidation() {
    nlm::Config config;
    
    // Test that we can set values
    config.set("simulation_timestep", 0.001, nlm::ConfigSource::Runtime);
    assert(config.has("simulation_timestep"));
    
    auto val = config.get<double>("simulation_timestep");
    assert(val.has_value());
    assert(*val == 0.001);
    
    std::cout << "    testConfigValidation passed" << std::endl;
}

void testConfigTypes() {
    nlm::Config config;
    
    // Test various types
    config.set("int_val", 42, nlm::ConfigSource::Runtime);
    assert(config.get<int>("int_val").has_value());
    
    config.set("double_val", 3.14159, nlm::ConfigSource::Runtime);
    assert(config.get<double>("double_val").has_value());
    
    config.set("bool_val", true, nlm::ConfigSource::Runtime);
    assert(config.get<bool>("bool_val").has_value());
    
    config.set("string_val", "test_string", nlm::ConfigSource::Runtime);
    assert(config.get<std::string>("string_val").has_value());
    
    std::cout << "    testConfigTypes passed" << std::endl;
}

void runAll() {
    testConfigCreation();
    testConfigSetAndGet();
    testConfigGetOr();
    testConfigValidation();
    testConfigTypes();
}

} // namespace test_config
