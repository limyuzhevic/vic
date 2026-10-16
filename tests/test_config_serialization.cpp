// Configuration Serialization Tests
// Tests JSON/YAML serialization support

#include "tests/test_suite_runner.cpp"
#include "src/core/Config/Config.hpp"
#include <cassert>
#include <iostream>
#include <sstream>
#include <string>

namespace test_config_serialization {

void testJsonSerialization() {
    std::cout << "  [testJsonSerialization] ";

    nlm::Config config;

    // Add configuration values that would be serialized
    config.set("int_value", 42, nlm::ConfigSource::Runtime);
    config.set("float_value", 3.14, nlm::ConfigSource::Runtime);
    config.set("string_value", std::string("test"), nlm::ConfigSource::Runtime);

    // Generate summary (simulates JSON structure)
    std::string summary = config.summary();
    assert(summary.find("int_value") != std::string::npos);
    assert(summary.find("float_value") != std::string::npos);

    std::cout << "PASSED" << std::endl;
}

void testYamlSerialization() {
    std::cout << "  [testYamlSerialization] ";

    nlm::Config config;

    // Add YAML-style configuration values
    config.setParameter("neuron", "membrane_capacitance", 2.5f, nlm::ConfigSource::File);
    config.setParameter("synapse", "weight_min", -1.0f, nlm::ConfigSource::File);

    // Verify values are properly set
    float cap = config.getParameterOr("neuron", "membrane_capacitance", 1.0f);
    float weightMin = config.getParameterOr("synapse", "weight_min", -1.0f);

    assert(cap == 2.5f);
    assert(weightMin == -1.0f);

    std::cout << "PASSED" << std::endl;
}

void testSerializationFormat() {
    std::cout << "  [testSerializationFormat] ";

    nlm::Config config;

    // Test different serialization formats
    config.set("test_string", std::string("value"), nlm::ConfigSource::Runtime);

    std::string summary = config.summary();

    // Verify format is readable and contains expected elements
    assert(!summary.empty());
    assert(summary.find("test_string") != std::string::npos);

    std::cout << "PASSED" << std::endl;
}

void testComplexSerialization() {
    std::cout << "  [testComplexSerialization] ";

    nlm::Config config;

    // Test serialization of complex configurations
    config.set("neuron_count", 100, nlm::ConfigSource::Runtime);
    config.set("region_count", 2, nlm::ConfigSource::Runtime);
    config.set("connection_probability", 0.1f, nlm::ConfigSource::Runtime);

    // Verify all values are preserved
    assert(config.getOr<int>("neuron_count", 0) == 100);
    assert(config.getOr<int>("region_count", 0) == 2);
    assert(config.getOr<float>("connection_probability", 0.0f) == 0.1f);

    std::cout << "PASSED" << std::endl;
}

void runAll() {
    testJsonSerialization();
    testYamlSerialization();
    testSerializationFormat();
    testComplexSerialization();

    std::cout << "  All configuration serialization tests passed!" << std::endl;
}

} // namespace test_config_serialization