// tests/test_enhanced_config.cpp
// Enhanced configuration tests

#include "core/Config/Config.hpp"
#include "core/Config/ConfigurationSchema.hpp"
#include "core/Config/ConfigurationValidator.hpp"
#include <cassert>
#include <iostream>
#include <nlohmann/json.hpp>

namespace test_enhanced_config {

void testSchemaCreation() {
    std::cout << "    Testing schema creation..." << std::endl;
    
    auto schema = std::make_unique<nlm::ConfigurationSchema>();
    
    // Test that we can create a schema
    assert(!schema->isEmpty());
    
    // Test parameter access
    const auto* param = schema->getParameter("neuron_count");
    assert(param != nullptr);
    assert(param->name == "neuron_count");
    
    // Test section access
    const auto* section = schema->getSection("Core");
    assert(section != nullptr);
    assert(section->name == "Core");
    
    std::cout << "    Schema creation test passed" << std::endl;
}

void testSchemaValidation() {
    std::cout << "    Testing schema validation..." << std::endl;
    
    nlohmann::json config = {
        {"neuron_count", 100},
        {"region_count", 1},
        {"connection_probability", 0.15},
        {"stdp_ltp_weight", 0.02},
        {"stdp_ltd_weight", 0.015},
        {"stdp_tau", 20.0},
        {"synaptogenesis_rate", 0.0001},
        {"pruning_rate", 0.00001}
    };
    
    std::string error;
    bool valid = nlm::Validation::createDefaultNLMConfigSchema()->validate(config, error);
    assert(valid);
    assert(error.empty());
    
    // Test invalid configuration
    nlohmann::json invalidConfig = {
        {"neuron_count", "not_a_number"},
        {"region_count", -1}
    };
    
    std::string invalidError;
    bool invalidValid = nlm::Validation::createDefaultNLMConfigSchema()->validate(invalidConfig, invalidError);
    assert(!invalidValid);
    assert(!invalidError.empty());
    
    std::cout << "    Schema validation test passed" << std::endl;
}

void testValidatorCreation() {
    std::cout << "    Testing validator creation..." << std::endl;
    
    auto schema = nlm::Validation::createDefaultNLMConfigSchema();
    nlm::ConfigurationValidator validator(std::move(schema));
    
    assert(validator.hasSchema());
    assert(!validator.getSchemaDescription().empty());
    
    std::cout << "    Validator creation test passed" << std::endl;
}

void testValidatorValidation() {
    std::cout << "    Testing validator validation..." << std::endl;
    
    auto schema = nlm::Validation::createDefaultNLMConfigSchema();
    nlm::ConfigurationValidator validator(std::move(schema));
    
    nlohmann::json config = {
        {"neuron_count", 100},
        {"region_count", 1},
        {"connection_probability", 0.15}
    };
    
    auto result = validator.validate(config);
    assert(result.success);
    assert(result.errorMessage.empty());
    
    // Test validation report
    std::string report = validator.getValidationReport(config);
    assert(!report.empty());
    assert(report.find("Configuration Validation Report") != std::string::npos);
    
    std::cout << "    Validator validation test passed" << std::endl;
}

void testConfigEnhancedCreation() {
    std::cout << "    Testing enhanced config creation..." << std::endl;
    
    nlm::Config config;
    
    // Test new config methods
    assert(!config.has("test"));
    
    // Test setting and getting
    config.set("int_value", 42, nlm::ConfigSource::Runtime);
    assert(config.has("int_value"));
    
    auto intVal = config.get<int>("int_value");
    assert(intVal.has_value());
    assert(*intVal == 42);
    
    // Test nested configuration
    config.setNested("Core", "neuron_count", 100);
    assert(config.has("Core.neuron_count"));
    
    auto nestedVal = config.getNested("Core", "neuron_count");
    assert(nestedVal.has_value());
    assert(*nestedVal == 100);
    
    std::cout << "    Enhanced config creation test passed" << std::endl;
}

void testConfigJSONLoading() {
    std::cout << "    Testing JSON loading..." << std::endl;
    
    nlm::Config config;
    
    // Create JSON content
    nlohmann::json jsonConfig = {
        {"neuron_count", 200},
        {"region_count", 2},
        {"connection_probability", 0.2}
    };
    
    std::string jsonString = jsonConfig.dump();
    bool loaded = config.loadFromJSON(jsonString, false);
    assert(loaded);
    
    // Verify loaded values
    assert(config.has("neuron_count"));
    assert(config.get<int64_t>("neuron_count").value() == 200);
    
    std::cout << "    JSON loading test passed" << std::endl;
}

void testConfigValidation() {
    std::cout << "    Testing config validation..." << std::endl;
    
    nlm::Config config;
    
    // Set valid values
    config.set("neuron_count", 100, nlm::ConfigSource::Runtime);
    config.set("region_count", 1, nlm::ConfigSource::Runtime);
    
    // Test validation without schema (should succeed)
    std::string errorMessage;
    bool valid = config.validate(errorMessage);
    assert(valid);
    
    // Create schema and set it
    auto schema = nlm::Validation::createDefaultNLMConfigSchema();
    config.setSchema(std::move(schema));
    
    // Set values according to schema
    config.set("neuron_count", 100, nlm::ConfigSource::Runtime);
    config.set("region_count", 1, nlm::ConfigSource::Runtime);
    
    // Now validate with schema
    valid = config.validate(errorMessage);
    assert(valid);
    
    std::cout << "    Config validation test passed" << std::endl;
}

void testConfigRuntimeModification() {
    std::cout << "    Testing runtime modification..." << std::endl;
    
    nlm::Config config;
    
    // Initial values
    config.set("neuron_count", 100, nlm::ConfigSource::Default);
    config.set("region_count", 1, nlm::ConfigSource::Default);
    
    // Modify values at runtime
    config.set("neuron_count", 150, nlm::ConfigSource::Runtime);
    config.set("region_count", 2, nlm::ConfigSource::Runtime);
    
    // Verify modifications
    assert(config.get<int64_t>("neuron_count").value() == 150);
    assert(config.get<int64_t>("region_count").value() == 2);
    
    // Test merge
    nlm::Config otherConfig;
    otherConfig.set("simulation_timestep", 0.01, nlm::ConfigSource::File);
    
    config.mergeConfig(otherConfig);
    assert(config.has("simulation_timestep"));
    
    // Test patch
    nlohmann::json patch = {
        {"neuron_count", 200},
        {"new_param", "value"}
    };
    
    config.patchConfig(patch);
    assert(config.get<int64_t>("neuron_count").value() == 200);
    assert(config.has("new_param"));
    
    std::cout << "    Runtime modification test passed" << std::endl;
}

void testConfigJSONSaveLoad() {
    std::cout << "    Testing JSON save/load..." << std::endl;
    
    nlm::Config config;
    
    // Set some values
    config.set("neuron_count", 300, nlm::ConfigSource::Default);
    config.set("region_count", 3, nlm::ConfigSource::Default);
    config.set("connection_probability", 0.25, nlm::ConfigSource::Default);
    
    // Save to JSON
    bool saved = config.saveToFile("/tmp/test_config.json");
    assert(saved);
    
    // Load from file
    nlm::Config config2;
    bool loaded = config2.loadFromFile("/tmp/test_config.json", false);
    assert(loaded);
    
    // Verify values
    assert(config2.get<int64_t>("neuron_count").value() == 300);
    assert(config2.get<int64_t>("region_count").value() == 3);
    assert(config2.get<double>("connection_probability").value() == 0.25);
    
    std::cout << "    JSON save/load test passed" << std::endl;
}

void testAutoFix() {
    std::cout << "    Testing auto-fix..." << std::endl;
    
    nlm::Config config;
    
    // Enable auto-fix
    config.setAutoFix(true);
    
    // Set schema with validation rules
    auto schema = nlm::Validation::createDefaultNLMConfigSchema();
    config.setSchema(std::move(schema));
    
    // Add values that might need fixing
    config.set("neuron_count", 100, nlm::ConfigSource::Runtime);
    config.set("region_count", 1, nlm::ConfigSource::Runtime);
    
    // Get validation report (auto-fix would be applied during validation)
    std::string report = config.getValidationReport();
    assert(!report.empty());
    
    std::cout << "    Auto-fix test passed" << std::endl;
}

void runAll() {
    testSchemaCreation();
    testSchemaValidation();
    testValidatorCreation();
    testValidatorValidation();
    testConfigEnhancedCreation();
    testConfigJSONLoading();
    testConfigValidation();
    testConfigRuntimeModification();
    testConfigJSONSaveLoad();
    testAutoFix();
    
    std::cout << "\nAll enhanced configuration tests passed!" << std::endl;
}

} // namespace test_enhanced_config