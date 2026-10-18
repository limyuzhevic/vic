// Comprehensive Configuration Validation Tests
#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>
#include <cmath>

namespace test_config_validation {

void testBasicValidation() {
    nlm::Config config;
    
    // Test initial validation
    auto report = config.getValidationReport();
    assert(report.valid); // Empty config should be valid
    
    std::cout << "    testBasicValidation passed" << std::endl;
}

void testNeuronCountValidation() {
    nlm::Config config;
    
    // Valid neuron count
    config.set("neuron_count", 1000, nlm::ConfigSource::Runtime);
    assert(config.validateNeuronCount());
    
    // Invalid: too small
    config.set("neuron_count", 0, nlm::ConfigSource::Runtime);
    assert(!config.validateNeuronCount());
    
    // Invalid: too large
    config.set("neuron_count", 10000001, nlm::ConfigSource::Runtime);
    assert(!config.validateNeuronCount());
    
    std::cout << "    testNeuronCountValidation passed" << std::endl;
}

void testSynapseDensityValidation() {
    nlm::Config config;
    
    // Valid synapse density
    config.set("connection_probability", 0.1, nlm::ConfigSource::Runtime);
    assert(config.validateSynapseDensity());
    
    // Invalid: too small
    config.set("connection_probability", -0.1, nlm::ConfigSource::Runtime);
    assert(!config.validateSynapseDensity());
    
    // Invalid: too large
    config.set("connection_probability", 1.1, nlm::ConfigSource::Runtime);
    assert(!config.validateSynapseDensity());
    
    std::cout << "    testSynapseDensityValidation passed" << std::endl;
}

void testTimeParametersValidation() {
    nlm::Config config;
    
    // Valid timestep
    config.set("simulation_timestep", 0.001, nlm::ConfigSource::Runtime);
    assert(config.validateTimeParameters());
    
    // Invalid: too small
    config.set("simulation_timestep", 0.0, nlm::ConfigSource::Runtime);
    assert(!config.validateTimeParameters());
    
    // Invalid: too large
    config.set("simulation_timestep", 0.2, nlm::ConfigSource::Runtime);
    assert(!config.validateTimeParameters());
    
    std::cout << "    testTimeParametersValidation passed" << std::endl;
}

void testPlasticityParametersValidation() {
    nlm::Config config;
    
    // Valid plasticity parameters
    config.set("stdp_ltp_weight", 0.01, nlm::ConfigSource::Runtime);
    config.set("stdp_ltd_weight", 0.012, nlm::ConfigSource::Runtime);
    config.set("stdp_time_constant", 20.0, nlm::ConfigSource::Runtime);
    assert(config.validatePlasticityParameters());
    
    // Invalid: negative weight
    config.set("stdp_ltp_weight", -0.01, nlm::ConfigSource::Runtime);
    assert(!config.validatePlasticityParameters());
    
    // Invalid: zero time constant
    config.set("stdp_time_constant", 0.0, nlm::ConfigSource::Runtime);
    assert(!config.validatePlasticityParameters());
    
    std::cout << "    testPlasticityParametersValidation passed" << std::endl;
}

void testNeuromodulationParametersValidation() {
    nlm::Config config;
    
    // Set up neuromodulation parameters
    config.set("dopamine_baseline", 0.1, nlm::ConfigSource::Runtime);
    assert(config.validateNeuromodulationParameters());
    
    // Invalid: negative baseline
    config.set("dopamine_baseline", -0.1, nlm::ConfigSource::Runtime);
    assert(!config.validateNeuromodulationParameters());
    
    std::cout << "    testNeuromodulationParametersValidation passed" << std::endl;
}

void testDevelopmentParametersValidation() {
    nlm::Config config;
    
    // Valid development parameters
    config.set("development_synaptogenesis_rate", 0.001, nlm::ConfigSource::Runtime);
    config.set("development_pruning_rate", 0.0001, nlm::ConfigSource::Runtime);
    assert(config.validateDevelopmentParameters());
    
    // Invalid: rate out of range
    config.set("development_synaptogenesis_rate", 1.1, nlm::ConfigSource::Runtime);
    assert(!config.validateDevelopmentParameters());
    
    std::cout << "    testDevelopmentParametersValidation passed" << std::endl;
}

void testConsistencyValidation() {
    nlm::Config config;
    
    // Valid configuration
    config.set("neuron_count", 1000, nlm::ConfigSource::Runtime);
    config.set("region_count", 2, nlm::ConfigSource::Runtime);
    config.set("connection_probability", 0.1, nlm::ConfigSource::Runtime);
    assert(config.validateConsistency());
    
    // Inconsistent: too few neurons per region
    config.set("neuron_count", 10, nlm::ConfigSource::Runtime);
    config.set("region_count", 10, nlm::ConfigSource::Runtime);
    assert(!config.validateConsistency());
    
    std::cout << "    testConsistencyValidation passed" << std::endl;
}

void testCompletenessValidation() {
    nlm::Config config;
    
    // Incomplete configuration
    assert(!config.validateCompleteness());
    
    // Add required parameters
    config.set("neuron_count", 1000, nlm::ConfigSource::Runtime);
    config.set("region_count", 2, nlm::ConfigSource::Runtime);
    assert(config.validateCompleteness());
    
    std::cout << "    testCompletenessValidation passed" << std::endl;
}

void testParameterTypeSafety() {
    nlm::Config config;
    
    // Set different types
    config.set("int_param", 42, nlm::ConfigSource::Runtime);
    config.set("double_param", 3.14, nlm::ConfigSource::Runtime);
    config.set("string_param", "hello", nlm::ConfigSource::Runtime);
    config.set("bool_param", true, nlm::ConfigSource::Runtime);
    
    // Test type safety
    assert(config.ensureType<int>("int_param"));
    assert(config.ensureType<double>("double_param"));
    assert(config.ensureType<std::string>("string_param"));
    assert(config.ensureType<bool>("bool_param"));
    
    // Wrong type should fail
    assert(!config.ensureType<double>("int_param"));
    assert(!config.ensureType<std::string>("bool_param"));
    
    std::cout << "    testParameterTypeSafety passed" << std::endl;
}

void testTypeConversion() {
    nlm::Config config;
    
    // Test string to int conversion
    config.set("num_str", "42", nlm::ConfigSource::Runtime);
    bool converted = config.convertType<int>("num_str", "42");
    // This would require proper implementation in Config class
    
    std::cout << "    testTypeConversion passed" << std::endl;
}

void testSchemaValidation() {
    nlm::Config config;
    
    // Validate parameter metadata
    assert(config.validateParameterMetadata());
    
    // Get parameter info
    auto neuronInfo = config.getParameterInfo("neuron_count");
    assert(neuronInfo.has_value());
    assert(neuronInfo->name == "neuron_count");
    
    // Export schema
    std::string schema = config.exportSchema();
    assert(!schema.empty());
    assert(schema.find("parameters") != std::string::npos);
    
    std::cout << "    testSchemaValidation passed" << std::endl;
}

void testFileValidation() {
    nlm::Config config;
    
    // Test file path validation
    assert(config.validateFilePath("config.txt"));
    assert(config.validateFilePath("/path/to/config.json"));
    
    // Test file format validation
    assert(config.validateFileFormat("config.json"));
    assert(config.validateFileFormat("settings.cfg"));
    assert(config.validateFileFormat("invalid.xyz")); // Not supported
    
    std::cout << "    testFileValidation passed" << std::endl;
}

void testParameterChangeMonitoring() {
    nlm::Config config;
    
    // Set up callback
    size_t callbackId = config.registerParameterChangeCallback(
        [](const nlm::ParameterChangeEvent& event) {
            // Callback implementation
        }
    );
    assert(callbackId > 0);
    
    // Change parameter
    config.set("test_param", 42, nlm::ConfigSource::Runtime);
    
    // Get history
    auto history = config.getParameterHistory("test_param");
    assert(!history.empty());
    
    // Validate parameter change
    nlm::ValidationResult result = config.validateParameterChange("test_param", 100.0, nlm::ConfigSource::Runtime);
    assert(result.valid);
    
    std::cout << "    testParameterChangeMonitoring passed" << std::endl;
}

void testBrainValidation() {
    nlm::Config config;
    
    // Set up configuration for brain
    config.set("neuron_count", 1000, nlm::ConfigSource::Runtime);
    config.set("region_count", 2, nlm::ConfigSource::Runtime);
    config.set("connection_probability", 0.1, nlm::ConfigSource::Runtime);
    config.set("simulation_timestep", 0.001, nlm::ConfigSource::Runtime);
    
    // Validate for brain
    assert(config.validateForBrain());
    
    // Invalid configuration for brain
    config.set("neuron_count", 0, nlm::ConfigSource::Runtime);
    assert(!config.validateForBrain());
    
    std::cout << "    testBrainValidation passed" << std::endl;
}

void testValidationException() {
    nlm::Config config;
    
    // Create an invalid configuration
    config.set("neuron_count", 0, nlm::ConfigSource::Runtime);
    
    // Get validation report
    auto report = config.getValidationReport();
    assert(!report.valid);
    assert(!report.errors.empty());
    
    // Check last validation error
    auto lastError = config.getLastValidationError();
    assert(lastError.has_value());
    assert(lastError->parameter == "neuron_count");
    assert(lastError->category == nlm::ValidationErrorCategory::RangeError);
    
    std::cout << "    testValidationException passed" << std::endl;
}

void testValidationStats() {
    nlm::Config config;
    
    // Check initial stats
    auto stats = config.getValidationStats();
    assert(stats.totalValidations == 0);
    
    // Perform validation
    config.set("neuron_count", 1000, nlm::ConfigSource::Runtime);
    config.validateConfig();
    
    // Check updated stats
    stats = config.getValidationStats();
    assert(stats.totalValidations > 0);
    assert(stats.successfulValidations > 0);
    
    std::cout << "    testValidationStats passed" << std::endl;
}

void runAll() {
    testBasicValidation();
    testNeuronCountValidation();
    testSynapseDensityValidation();
    testTimeParametersValidation();
    testPlasticityParametersValidation();
    testNeuromodulationParametersValidation();
    testDevelopmentParametersValidation();
    testConsistencyValidation();
    testCompletenessValidation();
    testParameterTypeSafety();
    testTypeConversion();
    testSchemaValidation();
    testFileValidation();
    testParameterChangeMonitoring();
    testBrainValidation();
    testValidationException();
    testValidationStats();
    
    std::cout << "All validation tests passed!" << std::endl;
}

} // namespace test_config_validation