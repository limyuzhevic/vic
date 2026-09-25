// Enhanced Config Tests demonstrating advanced configuration features
#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>
#include <fstream>
#include <sstream>

namespace test_config_advanced {

void testAdvancedKeyRegistration() {
    nlm::Config config;
    
    // Register keys with descriptions and categories
    config.registerKey("neural_dynamics.max_iterations", nlm::ConfigValue(100),
                      "Maximum iterations for neural dynamics simulation", "Neural Dynamics", true);
    config.registerKey("plasticity.plasticity_rate", nlm::ConfigValue(0.05),
                      "Plasticity learning rate", "Plasticity", true);
    config.registerKey("memory.capacity", nlm::ConfigValue(5000),
                      "Memory capacity for working memory", "Memory", true);
    config.registerKey("sensory.sensitivity", nlm::ConfigValue(1.0),
                      "Sensory system sensitivity", "Sensory", false);
    
    // Check that keys are registered with correct categories
    auto neuralDynamicsKeys = config.getKeysByCategory("Neural Dynamics");
    assert(neuralDynamicsKeys.size() == 1);
    assert(neuralDynamicsKeys[0] == "neural_dynamics.max_iterations");
    
    auto plasticityKeys = config.getKeysByCategory("Plasticity");
    assert(plasticityKeys.size() == 1);
    assert(plasticityKeys[0] == "plasticity.plasticity_rate");
    
    std::cout << "    testAdvancedKeyRegistration passed" << std::endl;
}

void testConfigurationConstraints() {
    nlm::Config config;
    
    // Register keys with constraints
    config.registerKeyWithConstraints("learning_rate", nlm::ConfigValue(0.01),
                                      "Learning rate for neural networks", "Training",
                                      {nlm::Constraint{nlm::ConstraintType::Range, "0.0", "1.0"}});
    
    config.registerKeyWithConstraints("population_size", nlm::ConfigValue(100),
                                      "Population size for evolutionary algorithm", "Evolution",
                                      {nlm::Constraint{nlm::ConstraintType::Range, "10", "10000"}});
    
    config.registerKeyWithConstraints("connection_probability", nlm::ConfigValue(0.5),
                                      "Probability of creating connections", "Structure",
                                      {nlm::Constraint{nlm::ConstraintType::Range, "0.0", "1.0"}});
    
    // Valid values should be accepted
    config.set("learning_rate", 0.1, nlm::ConfigSource::Runtime);
    config.set("population_size", 1000, nlm::ConfigSource::Runtime);
    config.set("connection_probability", 0.25, nlm::ConfigSource::Runtime);
    
    assert(config.has("learning_rate"));
    assert(config.has("population_size"));
    assert(config.has("connection_probability"));
    
    // Test that validation works
    auto validationResult = config.validate();
    assert(validationResult.valid);
    
    std::cout << "    testConfigurationConstraints passed" << std::endl;
}

void testPresetConfigurations() {
    nlm::Config config;
    
    // Create and apply presets
    nlm::Preset performancePreset;
    performancePreset.name = "performance";
    performancePreset.description = "High-performance configuration";
    performancePreset.values = {
        {"neural_dynamics.max_iterations", nlm::ConfigValue(100)},
        {"plasticity.plasticity_rate", nlm::ConfigValue(0.01)},
        {"memory.capacity", nlm::ConfigValue(1000)}
    };
    performancePreset.dependencies = {};
    config.addPreset(performancePreset);
    
    nlm::Preset accuracyPreset;
    accuracyPreset.name = "accuracy";
    accuracyPreset.description = "High-accuracy configuration";
    accuracyPreset.values = {
        {"neural_dynamics.max_iterations", nlm::ConfigValue(1000)},
        {"plasticity.plasticity_rate", nlm::ConfigValue(0.1)},
        {"memory.capacity", nlm::ConfigValue(10000)}
    };
    accuracyPreset.dependencies = {};
    config.addPreset(accuracyPreset);
    
    nlm::Preset balancedPreset;
    balancedPreset.name = "balanced";
    balancedPreset.description = "Balanced configuration between performance and accuracy";
    balancedPreset.values = {
        {"neural_dynamics.max_iterations", nlm::ConfigValue(500)},
        {"plasticity.plasticity_rate", nlm::ConfigValue(0.05)},
        {"memory.capacity", nlm::ConfigValue(5000)}
    };
    balancedPreset.dependencies = {"performance", "accuracy"};
    config.addPreset(balancedPreset);
    
    // Test applying presets
    assert(config.applyPreset("performance"));
    assert(config.get<int>("neural_dynamics.max_iterations") == 100);
    
    assert(config.applyPreset("accuracy"));
    assert(config.get<int>("neural_dynamics.max_iterations") == 1000);
    
    assert(config.applyPreset("balanced"));
    assert(config.get<int>("neural_dynamics.max_iterations") == 500);
    
    auto presetNames = config.getPresetNames();
    assert(presetNames.size() == 3);
    
    std::cout << "    testPresetConfigurations passed" << std::endl;
}

void testHierarchicalConfiguration() {
    nlm::Config parentConfig;
    nlm::Config childConfig;
    
    // Set parent configuration
    parentConfig.registerKey("base.learning_rate", nlm::ConfigValue(0.01),
                            "Base learning rate", "Training", true);
    parentConfig.registerKey("base.population_size", nlm::ConfigValue(100),
                            "Base population size", "Evolution", true);
    
    // Set child configuration with parent reference
    childConfig.setParentConfig(std::make_shared<nlm::Config>(parentConfig));
    
    // Child can access parent's values
    auto learningRate = childConfig.getOr<double>("base.learning_rate", 0.001);
    assert(learningRate == 0.01);
    
    auto populationSize = childConfig.getOr<int>("base.population_size", 50);
    assert(populationSize == 100);
    
    // Child can override parent values
    childConfig.set("base.learning_rate", 0.05, nlm::ConfigSource::Runtime);
    learningRate = childConfig.getOr<double>("base.learning_rate", 0.001);
    assert(learningRate == 0.05); // Override works
    
    std::cout << "    testHierarchicalConfiguration passed" << std::endl;
}

void testConfigurationTemplates() {
    nlm::Config config;
    
    // Create templates
    nlm::Config performanceTemplate;
    performanceTemplate.registerKey("neural_dynamics.max_iterations", nlm::ConfigValue(100),
                                   "Maximum iterations", "Neural Dynamics", true);
    performanceTemplate.registerKey("memory.capacity", nlm::ConfigValue(1000),
                                   "Memory capacity", "Memory", true);
    
    config.addTemplate("performance", performanceTemplate);
    
    nlm::Config accuracyTemplate;
    accuracyTemplate.registerKey("neural_dynamics.max_iterations", nlm::ConfigValue(1000),
                                "Maximum iterations", "Neural Dynamics", true);
    accuracyTemplate.registerKey("memory.capacity", nlm::ConfigValue(10000),
                                "Memory capacity", "Memory", true);
    
    config.addTemplate("accuracy", accuracyTemplate);
    
    // Apply templates
    assert(config.applyTemplate("performance"));
    assert(config.get<int>("neural_dynamics.max_iterations") == 100);
    assert(config.get<int>("memory.capacity") == 1000);
    
    assert(config.applyTemplate("accuracy"));
    assert(config.get<int>("neural_dynamics.max_iterations") == 1000);
    assert(config.get<int>("memory.capacity") == 10000);
    
    auto templateNames = config.getTemplateNames();
    assert(templateNames.size() == 2);
    
    std::cout << "    testConfigurationTemplates passed" << std::endl;
}

void testConfigurationSchema() {
    nlm::Config config;
    
    // Create schema
    nlm::ConfigSchema schema;
    schema.name = "default_schema";
    schema.description = "Default configuration schema for testing";
    
    // Register keys for schema
    config.registerKey("test.key1", nlm::ConfigValue("value1"), "Test key 1", "Test", true);
    config.registerKey("test.key2", nlm::ConfigValue(42), "Test key 2", "Test", true);
    config.registerKey("test.key3", nlm::ConfigValue(3.14), "Test key 3", "Test", true);
    
    // Add schema to config
    config.addSchema(schema);
    
    // Validate against schema
    bool valid = config.validateAgainstSchema("default_schema");
    assert(valid);
    
    // Test invalid configuration (missing required key)
    nlm::Config invalidConfig;
    invalidConfig.addSchema(schema);
    bool invalid = invalidConfig.validateAgainstSchema("default_schema");
    assert(!invalid); // Should be invalid due to missing required keys
    
    std::cout << "    testConfigurationSchema passed" << std::endl;
}

void testFileIOAdvanced() {
    nlm::Config config;
    
    // Register keys
    config.registerKey("file_test.string", nlm::ConfigValue("initial"), "Test string value", "File Test", true);
    config.registerKey("file_test.integer", nlm::ConfigValue(42), "Test integer value", "File Test", true);
    config.registerKey("file_test.boolean", nlm::ConfigValue(true), "Test boolean value", "File Test", true);
    
    // Set some values
    config.set("file_test.string", "modified", nlm::ConfigSource::Runtime);
    config.set("file_test.integer", 100, nlm::ConfigSource::Runtime);
    config.set("file_test.boolean", false, nlm::ConfigSource::Runtime);
    
    // Save to JSON file
    std::string testFile = "./test_config.json";
    bool saved = config.saveToFile(testFile, "json");
    assert(saved);
    
    // Clear config
    config.clear();
    
    // Load from JSON file
    bool loaded = config.loadFromFile(testFile, "json");
    assert(loaded);
    
    // Verify values
    auto stringVal = config.get<std::string>("file_test.string");
    assert(stringVal.has_value());
    assert(*stringVal == "modified");
    
    auto intVal = config.get<int>("file_test.integer");
    assert(intVal.has_value());
    assert(*intVal == 100);
    
    auto boolVal = config.get<bool>("file_test.boolean");
    assert(boolVal.has_value());
    assert(*boolVal == false);
    
    // Clean up
    std::remove(testFile.c_str());
    
    std::cout << "    testFileIOAdvanced passed" << std::endl;
}

void testConfigurationValidation() {
    nlm::Config config;
    
    // Register keys with constraints
    config.registerKeyWithConstraints("rate", nlm::ConfigValue(0.05),
                                      "Learning rate", "Training",
                                      {nlm::Constraint{nlm::ConstraintType::Range, "0.0", "1.0"}});
    
    config.registerKeyWithConstraints("iterations", nlm::ConfigValue(100),
                                      "Number of iterations", "Training",
                                      {nlm::Constraint{nlm::ConstraintType::Range, "1", "1000"}});
    
    // Valid configuration should pass validation
    config.set("rate", 0.1, nlm::ConfigSource::Runtime);
    config.set("iterations", 500, nlm::ConfigSource::Runtime);
    
    nlm::ValidationResult result = config.validate();
    assert(result.valid);
    assert(result.errors.empty());
    assert(!result.warnings.empty());
    
    // Invalid configuration should fail validation
    nlm::Config invalidConfig;
    invalidConfig.registerKeyWithConstraints("rate", nlm::ConfigValue(0.05),
                                            "Learning rate", "Training",
                                            {nlm::Constraint{nlm::ConstraintType::Range, "0.0", "1.0"}});
    
    // This violates the constraint
    invalidConfig.set("rate", 1.5, nlm::ConfigSource::Runtime);
    
    result = invalidConfig.validate();
    assert(!result.valid);
    assert(!result.errors.empty());
    
    std::cout << "    testConfigurationValidation passed" << std::endl;
}

void testConfigurationSummary() {
    nlm::Config config;
    
    // Register keys in different categories
    config.registerKey("cat1.key1", nlm::ConfigValue("value1"), "Key 1", "Category 1", true);
    config.registerKey("cat1.key2", nlm::ConfigValue(42), "Key 2", "Category 1", false);
    config.registerKey("cat2.key1", nlm::ConfigValue(3.14), "Key 1", "Category 2", true);
    
    // Set some values
    config.set("cat1.key2", 100, nlm::ConfigSource::Runtime);
    config.set("cat2.key1", 2.71, nlm::ConfigSource::Runtime);
    
    // Test basic summary
    std::string basicSummary = config.summary();
    assert(basicSummary.find("3 entries") != std::string::npos);
    
    // Test category summary
    std::string categorySummary = config.summaryByCategory();
    assert(categorySummary.find("Category 1 (2 entries)") != std::string::npos);
    assert(categorySummary.find("Category 2 (1 entries)") != std::string::npos);
    
    std::cout << "    testConfigurationSummary passed" << std::endl;
}

void testStaticPresetManagement() {
    // Test that static preset management works
    std::map<std::string, nlm::Preset> presets = nlm::Config::getDefaultPresets();
    assert(presets.find("performance") != presets.end());
    assert(presets.find("accuracy") != presets.end());
    assert(presets.find("balanced") != presets.end());
    
    // Test that schema is available
    nlm::ConfigSchema schema = nlm::Config::getDefaultSchema();
    assert(schema.name == "default_schema");
    assert(!schema.entries.empty());
    
    std::cout << "    testStaticPresetManagement passed" << std::endl;
}

void runAll() {
    testAdvancedKeyRegistration();
    testConfigurationConstraints();
    testPresetConfigurations();
    testHierarchicalConfiguration();
    testConfigurationTemplates();
    testConfigurationSchema();
    testFileIOAdvanced();
    testConfigurationValidation();
    testConfigurationSummary();
    testStaticPresetManagement();
    
    std::cout << std::endl;
    std::cout << "=== All Advanced Config Tests Passed! ===" << std::endl;
}

} // namespace test_config_advanced
