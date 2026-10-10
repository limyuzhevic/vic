// Enhanced Config Tests
#include "core/Config/EnhancedConfig.hpp"
#include "core/Config/ConfigSchema.hpp"
#include <cassert>
#include <iostream>
#include <vector>
#include <map>

namespace test_enhanced_config {

void testBasicConfiguration() {
    nlm::Config config;
    assert(!config.has("test"));
    
    config.set("int_value", 42);
    assert(config.has("int_value"));
    
    auto intVal = config.get<int>("int_value");
    assert(intVal.has_value());
    assert(*intVal == 42);
    
    std::cout << "    testBasicConfiguration passed" << std::endl;
}

void testConfigurationValidation() {
    nlm::Config config;
    nlm::ConfigSchema schema;
    
    // Add a parameter with validation
    nlm::ConfigParameter param("positive_number", nlm::ConfigType::Integer, "A positive number", "Validation");
    param.defaultValue(1).range(1, 100).positive();
    schema.addParameter(param);
    
    // Test valid value
    config.set("positive_number", 50);
    assert(config.validate(schema));
    
    // Test invalid value (negative)
    config.set("positive_number", -10);
    assert(!config.validate(schema));
    
    // Test invalid value (out of range)
    config.set("positive_number", 200);
    assert(!config.validate(schema));
    
    std::cout << "    testConfigurationValidation passed" << std::endl;
}

void testConfigurationProfiles() {
    nlm::Config config;
    nlm::ConfigSchema schema;
    
    // Add parameters with profile-specific defaults
    nlm::ConfigParameter param("simulation_speed", nlm::ConfigType::Float, "Simulation speed", "Performance");
    param.defaultValue(1.0).range(0.1, 10.0);
    param.setProfileDefault("Research", 0.5);
    param.setProfileDefault("Production", 5.0);
    schema.addParameter(param);
    
    // Test research profile
    config.loadFromProfile(nlm::ConfigProfile::Research, schema);
    auto researchSpeed = config.get<double>("simulation_speed");
    assert(researchSpeed.has_value() && *researchSpeed == 0.5);
    
    // Test production profile
    config.loadFromProfile(nlm::ConfigProfile::Production, schema);
    auto productionSpeed = config.get<double>("simulation_speed");
    assert(productionSpeed.has_value() && *productionSpeed == 5.0);
    
    std::cout << "    testConfigurationProfiles passed" << std::endl;
}

void testConfigurationBackup() {
    nlm::Config config;
    
    // Set some values
    config.set("random_seed", 123);
    config.set("neuron_count", 500);
    
    // Create backup
    assert(config.createBackup("test_backup", "Test backup"));
    
    // Modify values
    config.set("random_seed", 456);
    config.set("neuron_count", 600);
    
    // Restore from backup
    assert(config.restoreBackup("test_backup"));
    
    // Verify values are restored
    assert(config.get<int>("random_seed").has_value() && *config.get<int>("random_seed") == 123);
    assert(config.get<int>("neuron_count").has_value() && *config.get<int>("neuron_count") == 500);
    
    std::cout << "    testConfigurationBackup passed" << std::endl;
}

void testConfigurationDiff() {
    nlm::Config config1;
    nlm::Config config2;
    
    config1.set("random_seed", 123);
    config1.set("neuron_count", 500);
    
    config2.set("random_seed", 123);
    config2.set("neuron_count", 600);
    config2.set("log_level", "DEBUG");
    
    // Test diff
    std::string diff = config1.diff(config2);
    assert(diff.find("neuron_count") != std::string::npos);
    assert(diff.find("log_level") != std::string::npos);
    
    std::cout << "    testConfigurationDiff passed" << std::endl;
}

void testConfigurationHelp() {
    nlm::Config config;
    nlm::ConfigSchema schema;
    
    // Add a parameter with description
    nlm::ConfigParameter param("log_level", nlm::ConfigType::String, "Logging level (DEBUG, INFO, WARNING, ERROR)", "Logging");
    param.defaultValue("INFO");
    schema.addParameter(param);
    
    // Get help for the parameter
    std::string help = config.getHelp("log_level", schema);
    assert(help.find("log_level") != std::string::npos);
    assert(help.find("Logging level") != std::string::npos);
    assert(help.find("INFO") != std::string::npos);
    
    std::cout << "    testConfigurationHelp passed" << std::endl;
}

void testConfigurationCategories() {
    nlm::Config config;
    nlm::ConfigSchema schema;
    
    // Add parameters to different categories
    nlm::ConfigParameter param1("random_seed", nlm::ConfigType::Integer, "Random seed", "General");
    schema.addParameter(param1);
    
    nlm::ConfigParameter param2("neuron_count", nlm::ConfigType::Integer, "Neuron count", "Neural");
    schema.addParameter(param2);
    
    nlm::ConfigParameter param3("log_level", nlm::ConfigType::String, "Log level", "Logging");
    schema.addParameter(param3);
    
    // Test category help
    std::string generalHelp = config.getCategoryHelp("General", schema);
    assert(generalHelp.find("random_seed") != std::string::npos);
    
    std::string neuralHelp = config.getCategoryHelp("Neural", schema);
    assert(neuralHelp.find("neuron_count") != std::string::npos);
    
    std::cout << "    testConfigurationCategories passed" << std::endl;
}

void testConfigurationValidationScore() {
    nlm::Config config;
    nlm::ConfigSchema schema;
    
    // Add a parameter
    nlm::ConfigParameter param("valid_param", nlm::ConfigType::Integer, "A valid parameter", "Test");
    param.defaultValue(1).range(1, 100);
    schema.addParameter(param);
    
    // Test with valid value
    config.set("valid_param", 50);
    double score1 = config.getValidationScore(schema);
    assert(score1 > 0.9);
    
    // Test with invalid value
    config.set("valid_param", 200);
    double score2 = config.getValidationScore(schema);
    assert(score2 < 0.5);
    
    std::cout << "    testConfigurationValidationScore passed" << std::endl;
}

void testConfigurationMerging() {
    nlm::Config config1;
    nlm::Config config2;
    
    config1.set("param1", 100);
    config1.set("param2", "value1");
    
    config2.set("param2", "value2");
    config2.set("param3", 200);
    
    // Merge config2 into config1
    config1.merge(config2);
    
    // Verify that param1 is preserved, param2 is updated, and param3 is added
    assert(config1.get<int>("param1").has_value() && *config1.get<int>("param1") == 100);
    assert(config1.get<std::string>("param2").has_value() && *config1.get<std::string>("param2") == "value2");
    assert(config1.get<int>("param3").has_value() && *config1.get<int>("param3") == 200);
    
    std::cout << "    testConfigurationMerging passed" << std::endl;
}

void testConfigurationSmartDefaults() {
    nlm::Config config;
    nlm::ConfigSchema schema;
    
    // Add parameters with smart defaults
    nlm::ConfigParameter param("learning_rate", nlm::ConfigType::Float, "Learning rate", "Plasticity");
    param.defaultValue(0.01).range(0.001, 0.1);
    param.setProfileDefault("Research", 0.005);
    param.setProfileDefault("Production", 0.05);
    schema.addParameter(param);
    
    // Test smart defaults for research profile
    config.setProfile(nlm::ConfigProfile::Research, schema);
    auto researchRate = config.get<double>("learning_rate");
    assert(researchRate.has_value() && *researchRate == 0.005);
    
    // Test smart defaults for production profile
    config.setProfile(nlm::ConfigProfile::Production, schema);
    auto productionRate = config.get<double>("learning_rate");
    assert(productionRate.has_value() && *productionRate == 0.05);
    
    std::cout << "    testConfigurationSmartDefaults passed" << std::endl;
}

void testConfigurationErrorHandling() {
    nlm::Config config;
    nlm::ConfigSchema schema;
    
    // Add a parameter with validation
    nlm::ConfigParameter param("required_param", nlm::ConfigType::Integer, "A required parameter", "Test");
    param.defaultValue(1).required();
    schema.addParameter(param);
    
    // Test error for missing parameter
    auto errors = config.validateParameter("required_param", schema);
    assert(!errors.empty());
    assert(errors[0].type == nlm::ConfigErrorType::InvalidValue);
    
    std::cout << "    testConfigurationErrorHandling passed" << std::endl;
}

void runAll() {
    testBasicConfiguration();
    testConfigurationValidation();
    testConfigurationProfiles();
    testConfigurationBackup();
    testConfigurationDiff();
    testConfigurationHelp();
    testConfigurationCategories();
    testConfigurationValidationScore();
    testConfigurationMerging();
    testConfigurationSmartDefaults();
    testConfigurationErrorHandling();
    
    std::cout << std::endl;
    std::cout << "All enhanced configuration tests passed!" << std::endl;
    std::cout << "==========================================" << std::endl;
}

} // namespace test_enhanced_config