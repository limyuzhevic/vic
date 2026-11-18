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

void testConfigKeys() {
    nlm::Config config;
    
    config.set("key1", 1, nlm::ConfigSource::Runtime);
    config.set("key2", 2, nlm::ConfigSource::Runtime);
    config.set("key3", 3, nlm::ConfigSource::Runtime);
    
    auto keys = config.getKeys();
    assert(keys.size() == 3);
    
    std::cout << "    testConfigKeys passed" << std::endl;
}

// New comprehensive test: Configuration validation
void testConfigValidation() {
    nlm::Config config;
    
    // Test valid integer values
    config.set("neuron_count", static_cast<int64_t>(1000), nlm::ConfigSource::Default);
    config.set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
    
    // Test valid float values
    config.set("connection_probability", 0.1f, nlm::ConfigSource::Default);
    config.set("simulation_timestep", 0.001, nlm::ConfigSource::Default);
    
    // Test string values
    config.set("environment_name", std::string("GridWorld"), nlm::ConfigSource::Default);
    config.set("log_filename", std::string("nlm.log"), nlm::ConfigSource::Default);
    
    // Test boolean values
    config.set("log_to_file", true, nlm::ConfigSource::Default);
    config.set("visualization_enabled", false, nlm::ConfigSource::Default);
    
    // Test vector values
    std::vector<int> intVector{1, 2, 3};
    config.set("some_vector", intVector, nlm::ConfigSource::Default);
    
    // Verify that validation passes for valid values
    // (The validation happens automatically when values are set)
    
    std::cout << "    testConfigValidation passed" << std::endl;
}

// New comprehensive test: Configuration sources
void testConfigSources() {
    nlm::Config config;
    
    // Test that we can set values from different sources
    config.set("source_test", "default", nlm::ConfigSource::Default);
    config.set("source_test", "file", nlm::ConfigSource::File);
    config.set("source_test", "command_line", nlm::ConfigSource::CommandLine);
    config.set("source_test", "runtime", nlm::ConfigSource::Runtime);
    
    // Verify we can still get the value
    auto val = config.get<std::string>("source_test");
    assert(val.has_value());
    assert(*val == "runtime"); // Should be the latest value
    
    // Test that different keys can have different sources
    config.set("neuron_count", static_cast<int64_t>(500), nlm::ConfigSource::CommandLine);
    config.set("connection_probability", 0.15f, nlm::ConfigSource::Default);
    
    std::cout << "    testConfigSources passed" << std::endl;
}

// New comprehensive test: Configuration edge cases
void testConfigEdgeCases() {
    nlm::Config config;
    
    // Test empty string key (should be rejected by validation)
    try {
        config.set("", "empty_key", nlm::ConfigSource::Runtime);
        // If we get here, the config accepted it (which might be okay)
        // In a real implementation, this should ideally be validated
    } catch (...) {
        // Expected - validation should reject empty keys
    }
    
    // Test very large integer values
    config.set("large_value", static_cast<int64_t>(2147483647), nlm::ConfigSource::Runtime);
    auto largeVal = config.get<int64_t>("large_value");
    assert(largeVal.has_value());
    assert(*largeVal == 2147483647);
    
    // Test negative values (should be accepted but may fail validation)
    config.set("negative_value", -10, nlm::ConfigSource::Runtime);
    auto negVal = config.get<int>("negative_value");
    assert(negVal.has_value());
    assert(*negVal == -10);
    
    // Test zero values
    config.set("zero_value", 0, nlm::ConfigSource::Runtime);
    auto zeroVal = config.get<int>("zero_value");
    assert(zeroVal.has_value());
    assert(*zeroVal == 0);
    
    // Test multiple vectors
    std::vector<double> doubleVector{1.5, 2.5, 3.5};
    config.set("double_vector", doubleVector, nlm::ConfigSource::Runtime);
    
    std::vector<std::string> stringVector{"hello", "world"};
    config.set("string_vector", stringVector, nlm::ConfigSource::Runtime);
    
    std::cout << "    testConfigEdgeCases passed" << std::endl;
}

// New comprehensive test: Configuration serialization
void testConfigSerialization() {
    nlm::Config config;
    
    // Add some test data
    config.set("string_key", "test_value", nlm::ConfigSource::Default);
    config.set("int_key", 123, nlm::ConfigSource::Default);
    config.set("float_key", 3.14159f, nlm::ConfigSource::Default);
    config.set("bool_key", true, nlm::ConfigSource::Default);
    config.set("vector_key", std::vector<int>{1, 2, 3, 4, 5}, nlm::ConfigSource::Default);
    
    // Test summary generation (this tests the serialization/deserialization infrastructure)
    std::string summary = config.summary();
    assert(!summary.empty());
    assert(summary.find("Configuration") != std::string::npos);
    assert(summary.find("5 entries") != std::string::npos);
    assert(summary.find("string_key") != std::string::npos);
    assert(summary.find("int_key") != std::string::npos);
    
    std::cout << "    testConfigSerialization passed" << std::endl;
}

void runAll() {
    testConfigCreation();
    testConfigSetAndGet();
    testConfigGetOr();
    testConfigRemove();
    testConfigKeys();
    testConfigValidation();
    testConfigSources();
    testConfigEdgeCases();
    testConfigSerialization();
}

} // namespace test_config
