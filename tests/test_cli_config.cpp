// CLI Configuration Tests
// Tests configuration commands and operations

#include "tests/test_suite_runner.cpp"
#include "src/core/Config/Config.hpp"
#include <cassert>
#include <iostream>

namespace test_cli_config {

void testConfigurationCommands() {
    std::cout << "  [testConfigurationCommands] ";

    nlm::Config config;

    // Test configuration command operations
    config.set("neuron_count", 500, nlm::ConfigSource::File);
    config.set("region_count", 2, nlm::ConfigSource::File);

    assert(config.getOr<int>("neuron_count", 0) == 500);
    assert(config.getOr<int>("region_count", 0) == 2);

    std::cout << "PASSED" << std::endl;
}

void testConfigurationValidation() {
    std::cout << "  [testConfigurationValidation] ";

    nlm::Config config;

    // Test configuration validation
    bool neuronValid = config.validateParameter("neuron", "membrane_capacitance", 1.5f);
    assert(neuronValid);

    bool synapseValid = config.validateParameter("synapse", "weight_min", -1.0f);
    assert(synapseValid);

    std::cout << "PASSED" << std::endl;
}

void testConfigurationFileOperations() {
    std::cout << "  [testConfigurationFileOperations] ";

    nlm::Config config;

    // Test file operation simulation
    config.set("config_param", 42, nlm::ConfigSource::File);

    assert(config.getOr<int>("config_param", 0) == 42);

    std::cout << "PASSED" << std::endl;
}

void runAll() {
    testConfigurationCommands();
    testConfigurationValidation();
    testConfigurationFileOperations();

    std::cout << "  All CLI configuration tests passed!" << std::endl;
}

} // namespace test_cli_config