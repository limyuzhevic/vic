#!/usr/bin/env g++
// Test program to verify configuration improvements

#include "src/core/Config/Config.hpp"
#include <iostream>
#include <cassert>

int main() {
    std::cout << "Testing NLM Configuration System Improvements..." << std::endl;
    
    // Test 1: Basic configuration operations
    {
        nlm::Config config;
        config.set("test_int", 42, nlm::ConfigSource::Runtime);
        config.set("test_double", 3.14, nlm::ConfigSource::Runtime);
        config.set("test_string", std::string("hello"), nlm::ConfigSource::Runtime);
        config.set("test_bool", true, nlm::ConfigSource::Runtime);
        
        assert(config.has("test_int"));
        assert(config.has("test_double"));
        assert(config.has("test_string"));
        assert(config.has("test_bool"));
        
        auto intVal = config.get<int>("test_int");
        assert(intVal.has_value() && *intVal == 42);
        
        auto doubleVal = config.get<double>("test_double");
        assert(doubleVal.has_value() && *doubleVal > 3.13 && *doubleVal < 3.15);
        
        auto stringVal = config.get<std::string>("test_string");
        assert(stringVal.has_value() && *stringVal == "hello");
        
        auto boolVal = config.get<bool>("test_bool");
        assert(boolVal.has_value() && *boolVal == true);
        
        std::cout << "  ✓ Basic configuration operations passed" << std::endl;
    }
    
    // Test 2: Configuration validation
    {
        nlm::Config config;
        config.set("simulation_timestep", 0.001, nlm::ConfigSource::Runtime);
        config.set("neuron_count", 100, nlm::ConfigSource::Runtime);
        config.set("log_level", std::string("INFO"), nlm::ConfigSource::Runtime);
        config.set("visualization_enabled", true, nlm::ConfigSource::Runtime);
        
        std::vector<std::string> errors;
        assert(config.validate(errors));
        assert(errors.empty());
        
        // Test invalid configuration
        nlm::Config config2;
        config2.set("simulation_timestep", -0.001, nlm::ConfigSource::Runtime);
        
        std::vector<std::string> errors2;
        assert(!config2.validate(errors2));
        assert(!errors2.empty());
        
        std::cout << "  ✓ Configuration validation passed" << std::endl;
    }
    
    // Test 3: Value type detection
    {
        nlm::Config config;
        config.set("int_value", 42, nlm::ConfigSource::Runtime);
        config.set("double_value", 3.14, nlm::ConfigSource::Runtime);
        config.set("string_value", std::string("hello"), nlm::ConfigSource::Runtime);
        config.set("bool_value", true, nlm::ConfigSource::Runtime);
        
        assert(config.getValueType("int_value") == "int");
        assert(config.getValueType("double_value") == "double");
        assert(config.getValueType("string_value") == "string");
        assert(config.getValueType("bool_value") == "bool");
        assert(config.getValueType("nonexistent") == "NOT_FOUND");
        
        std::cout << "  ✓ Value type detection passed" << std::endl;
    }
    
    // Test 4: Type conversion
    {
        nlm::Config config;
        config.set("int_value", 42, nlm::ConfigSource::Runtime);
        
        assert(config.canConvertTo<int>("int_value"));
        assert(config.canConvertTo<int64_t>("int_value"));
        assert(config.canConvertTo<uint64_t>("int_value"));
        assert(config.canConvertTo<float>("int_value"));
        assert(config.canConvertTo<double>("int_value"));
        assert(!config.canConvertTo<bool>("int_value"));
        assert(!config.canConvertTo<std::string>("int_value"));
        
        std::cout << "  ✓ Type conversion checks passed" << std::endl;
    }
    
    // Test 5: Enhanced get with validation
    {
        nlm::Config config;
        config.set("int_value", 42, nlm::ConfigSource::Runtime);
        
        auto intVal = config.getWithValidation<int>("int_value");
        assert(intVal.has_value() && *intVal == 42);
        
        auto doubleVal = config.getWithValidation<double>("int_value");
        assert(doubleVal.has_value() && *doubleVal == 42.0);
        
        // Test failed conversion
        config.set("string_value", std::string("hello"), nlm::ConfigSource::Runtime);
        auto invalidVal = config.getWithValidation<int>("string_value");
        assert(!invalidVal.has_value());
        
        std::cout << "  ✓ Enhanced get with validation passed" << std::endl;
    }
    
    // Test 6: Configuration save/load
    {
        nlm::Config config;
        config.set("random_seed", 42, nlm::ConfigSource::Runtime);
        config.set("simulation_timestep", 0.001, nlm::ConfigSource::Runtime);
        config.set("neuron_count", 500, nlm::ConfigSource::Runtime);
        config.set("log_level", std::string("INFO"), nlm::ConfigSource::Runtime);
        config.set("visualization_enabled", false, nlm::ConfigSource::Runtime);
        
        // Test saving to file
        assert(config.saveToFile("test_config.cfg"));
        
        // Test loading from file
        nlm::Config config2;
        assert(config2.loadFromFile("test_config.cfg"));
        
        assert(config2.has("random_seed"));
        assert(config2.has("simulation_timestep"));
        assert(config2.has("neuron_count"));
        assert(config2.has("log_level"));
        assert(config2.has("visualization_enabled"));
        
        // Clean up
        std::remove("test_config.cfg");
        
        std::cout << "  ✓ Configuration save/load operations passed" << std::endl;
    }
    
    std::cout << std::endl;
    std::cout << "All configuration system tests passed! ✓" << std::endl;
    return 0;
}
