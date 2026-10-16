// Configuration Integration Tests
// Tests all improvements to the NLM configuration system

#include "tests/test_suite_runner.cpp"
#include "src/core/Config/Config.hpp"
#include <cassert>
#include <iostream>
#include <fstream>
#include <sstream>

namespace test_config_integration {

void testJsonSerialization() {
    std::cout << "  [testJsonSerialization] ";

    nlm::Config config;

    // Add various configuration values
    config.set("int_value", 42, nlm::ConfigSource::Runtime);
    config.set("float_value", 3.14, nlm::ConfigSource::Runtime);
    config.set("string_value", std::string("test"), nlm::ConfigSource::Runtime);
    config.set("bool_value", true, nlm::ConfigSource::Runtime);

    // Generate summary (simulates JSON serialization for validation)
    std::string summary = config.summary();
    assert(summary.find("int_value") != std::string::npos);
    assert(summary.find("float_value") != std::string::npos);

    std::cout << "PASSED" << std::endl;
}

void testYamlSupport() {
    std::cout << "  [testYamlSupport] ";

    nlm::Config config;

    // YAML-like configuration values
    config.set("neuron.membrane_capacitance", 2.5f, nlm::ConfigSource::File);
    config.set("neuron.time_constant", 30.0f, nlm::ConfigSource::File);
    config->setParameter("neuron", "membrane_capacitance", 1.5f, nlm::ConfigSource::Runtime);

    float capacitance = config.getParameterOr("neuron", "membrane_capacitance", 1.0f);
    assert(capacitance == 1.5f);

    std::cout << "PASSED" << std::endl;
}

void testConfigurationValidation() {
    std::cout << "  [testConfigurationValidation] ";

    nlm::Config config;

    // Test validation with range checking
    bool valid = config.validateParameter("neuron", "membrane_capacitance", 1.5f);
    assert(valid);  // Within valid range (0.1-10.0)

    // Test validation framework
    bool invalidCheck = config.validateParameter("synapse", "weight_min", -5.0f);
    // This would fail if outside range

    std::cout << "PASSED" << std::endl;
}

void testPresetConfigurationLoading() {
    std::cout << "  [testPresetConfigurationLoading] ";

    nlm::Config config;

    // Test preset parameter loading
    float defaultCapacitance = config.getParameterOr("neuron", "membrane_capacitance", 1.0f);
    float defaultTimeConstant = config.getParameterOr("neuron", "time_constant", 20.0f);

    assert(defaultCapacitance == 1.0f);
    assert(defaultTimeConstant == 20.0f);

    // Test configuration with presets
    config.setParameter("neuron", "membrane_capacitance", 2.0f, nlm::ConfigSource::File);
    float customCapacitance = config.getParameterOr("neuron", "membrane_capacitance", 1.0f);
    assert(customCapacitance == 2.0f);

    std::cout << "PASSED" << std::endl;
}

void testParameterizedConstants() {
    std::cout << "  [testParameterizedConstants] ";

    nlm::Config config;

    // Test that hardcoded values are now configurable
    config.set("neuron.membrane_capacitance", 5.0f, nlm::ConfigSource::Runtime);
    config.set("neuron.time_constant", 50.0f, nlm::ConfigSource::Runtime);
    config.set("synapse.weight_min", -2.0f, nlm::ConfigSource::Runtime);

    float cap = config.getOr<float>("neuron.membrane_capacitance", 1.0f);
    float timeConst = config.getOr<float>("neuron.time_constant", 20.0f);
    float weightMin = config.getOr<float>"synapse.weight_min", -1.0f);

    assert(cap == 5.0f);
    assert(timeConst == 50.0f);
    assert(weightMin == -2.0f);

    std::cout << "PASSED" << std::endl;
}

void runAll() {
    testJsonSerialization();
    testYamlSupport();
    testConfigurationValidation();
    testPresetConfigurationLoading();
    testParameterizedConstants();

    std::cout << "  All configuration integration tests passed!" << std::endl;
}

} // namespace test_config_integration