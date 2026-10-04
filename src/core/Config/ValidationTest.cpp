#include "core/Config/Config.hpp"
#include <iostream>

int main() {
    std::cout << "=== NLM Configuration Validation Test ===" << std::endl;
    
    // Create a config object
    auto config = std::make_shared<nlm::Config>();
    
    // Test 1: Set some valid values
    std::cout << "\nTest 1: Setting valid configuration values..." << std::endl;
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Runtime);
    config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Runtime);
    config->set("simulation_timestep", 0.005, nlm::ConfigSource::Runtime);
    config->set("connection_probability", 0.15f, nlm::ConfigSource::Runtime);
    
    std::cout << "Set values:" << std::endl;
    std::cout << "  neuron_count: " << config->getOr<int64_t>("neuron_count", 0) << std::endl;
    std::cout << "  region_count: " << config->getOr<int64_t>("region_count", 0) << std::endl;
    std::cout << "  simulation_timestep: " << config->getOr<double>("simulation_timestep", 0.0) << std::endl;
    std::cout << "  connection_probability: " << config->getOr<float>("connection_probability", 0.0f) << std::endl;
    
    // Test 2: Validate the configuration
    std::cout << "\nTest 2: Validating configuration..." << std::endl;
    bool isValid = config->validateConfiguration();
    std::cout << "Validation result: " << (isValid ? "PASS" : "FAIL") << std::endl;
    
    if (isValid) {
        std::cout << "Validation report:" << std::endl;
        std::cout << config->getValidationReport() << std::endl;
    } else {
        std::cout << "Last validation result:" << std::endl;
        const auto& validationResult = config->getLastValidationResult();
        for (const auto& error : validationResult.errors) {
            std::cout << "  ERROR: " << error << std::endl;
        }
        for (const auto& warning : validationResult.warnings) {
            std::cout << "  WARNING: " << warning << std::endl;
        }
    }
    
    // Test 3: Try to set invalid values
    std::cout << "\nTest 3: Testing invalid configuration values..." << std::endl;
    
    // This should fail validation (negative neuron count)
    config->set("neuron_count", static_cast<int64_t>(-10), nlm::ConfigSource::Runtime);
    std::cout << "Set neuron_count to -10 (invalid)..." << std::endl;
    
    // This should fail validation (probability > 1.0)
    config->set("connection_probability", 1.5f, nlm::ConfigSource::Runtime);
    std::cout << "Set connection_probability to 1.5 (invalid)..." << std::endl;
    
    // This should fail validation (timestep <= 0)
    config->set("simulation_timestep", 0.0, nlm::ConfigSource::Runtime);
    std::cout << "Set simulation_timestep to 0.0 (invalid)..." << std::endl;
    
    // Validate again
    std::cout << "\nValidating after setting invalid values..." << std::endl;
    isValid = config->validateConfiguration();
    std::cout << "Validation result: " << (isValid ? "PASS" : "FAIL") << std::endl;
    
    std::cout << "\nDetailed validation report:" << std::endl;
    std::cout << config->getValidationReport() << std::endl;
    
    // Test 4: Get validation result details
    std::cout << "\nTest 4: Validation result details..." << std::endl;
    const auto& validationResult = config->getLastValidationResult();
    std::cout << "Total errors: " << validationResult.errors.size() << std::endl;
    std::cout << "Total warnings: " << validationResult.warnings.size() << std::endl;
    
    // Test 5: Check individual validation results
    std::cout << "\nTest 5: Individual parameter validation..." << std::endl;
    
    // Reset to valid values for testing individual validation
    config->clear();
    config->set("neuron_count", static_cast<int64_t>(500), nlm::ConfigSource::Runtime);
    config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Runtime);
    
    // Get validation results for specific parameters
    auto neuronCountVal = config->get<int64_t>("neuron_count");
    auto regionCountVal = config->get<int64_t>("region_count");
    
    std::cout << "neuron_count validation:" << std::endl;
    std::cout << "  Value: " << (neuronCountVal.has_value() ? std::to_string(neuronCountVal.value()) : "none") << std::endl;
    std::cout << "  Is valid: " << (neuronCountVal.has_value() && neuronCountVal.value() > 0) << std::endl;
    
    std::cout << "region_count validation:" << std::endl;
    std::cout << "  Value: " << (regionCountVal.has_value() ? std::to_string(regionCountVal.value()) : "none") << std::endl;
    std::cout << "  Is valid: " << (regionCountVal.has_value() && regionCountVal.value() > 0) << std::endl;
    
    std::cout << "\n=== Configuration Validation Test Complete ===" << std::endl;
    return 0;
}
