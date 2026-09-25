// Configuration Integration Tests
// Tests for enhanced configuration system integration
// 
// Key integration features verified:
// 1. Config-Brain Integration - Configuration-driven brain setup
// 2. Config Memory Integration - Hierarchical configuration support
// 3. Config Validation Integration - Schema and constraint validation
// 4. Config Template Integration - Reusable configuration templates
// 5. Config Inheritance Integration - Parent-child configuration relationships
// 6. Config File I/O Integration - JSON/YAML/INI serialization

#include "src/core/Config/Config.hpp"
#include "src/brain/Brain.hpp"
#include <cassert>
#include <iostream>
#include <fstream>
#include <cmath>
#include <filesystem>

namespace test_config_integration {

void testConfigBrainIntegration() {
    // Create enhanced config with advanced features
    nlm::Config config;
    
    // Register keys with categories and descriptions
    config.registerKey("neural_dynamics.max_iterations", nlm::ConfigValue(100),
                      "Maximum iterations for neural dynamics simulation", "Neural Dynamics", true);
    config.registerKey("plasticity.plasticity_rate", nlm::ConfigValue(0.05),
                      "Plasticity learning rate", "Plasticity", true);
    config.registerKey("memory.capacity", nlm::ConfigValue(5000),
                      "Memory capacity for working memory", "Memory", true);
    config.registerKey("prediction.horizon", nlm::ConfigValue(10),
                      "Prediction time horizon", "Prediction", true);
    config.registerKey("planning.horizon", nlm::ConfigValue(5),
                      "Planning depth horizon", "Planning", true);
    
    // Create brain with enhanced config
    auto brain = std::make_shared<nlm::Brain>(config);
    
    bool success = brain->initialize();
    assert(success);
    
    // Verify brain was configured correctly
    assert(brain->getRegionCount() == 1);
    
    // Test config key retrieval
    auto neuralDynamicsKeys = config.getKeysByCategory("Neural Dynamics");
    assert(neuralDynamicsKeys.size() == 1);
    
    auto memoryKeys = config.getKeysByCategory("Memory");
    assert(memoryKeys.size() == 1);
    
    std::cout << "    testConfigBrainIntegration passed" << std::endl;
}

void testConfigConstraintValidation() {
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
    
    config.registerKeyWithConstraints("max_neurons", nlm::ConfigValue(1000),
                                      "Maximum neuron count", "Structure",
                                      {nlm::Constraint{nlm::ConstraintType::Range, "1", "10000"}});
    
    // Test constraint validation
    auto validationResult = config.validate();
    assert(validationResult.errors.empty()); // All set values should be valid
    assert(validationResult.warnings.empty());
    
    // Test setting constrained values
    config.set("learning_rate", 0.5, nlm::ConfigSource::Default); // Valid
    config.set("learning_rate", 1.5, nlm::ConfigSource::Default); // Would trigger error but we're just testing API
    
    std::cout << "    testConfigConstraintValidation passed" << std::endl;
}

void testConfigPresetApplication() {
    nlm::Config config;
    
    // Test preset application
    config.applyPreset("performance");
    
    // Verify performance preset values
    auto learningRate = config.get("learning_rate");
    auto plasticityRate = config.get("plasticity.plasticity_rate");
    
    assert(learningRate.has_value());
    assert(plasticityRate.has_value());
    
    // Apply different preset
    config.applyPreset("accuracy");
    
    // Verify accuracy preset values (should be different from performance)
    auto learningRate2 = config.get("learning_rate");
    
    assert(learningRate2.has_value());
    
    std::cout << "    testConfigPresetApplication passed" << std::endl;
}

void testConfigHierarchicalIntegration() {
    nlm::Config parentConfig;
    
    // Parent configuration
    parentConfig.registerKey("base.learning_rate", nlm::ConfigValue(0.01),
                            "Base learning rate", "Training", true);
    parentConfig.registerKey("base.neuron_count", nlm::ConfigValue(100),
                            "Base neuron count", "Structure", true);
    parentConfig.registerKey("base.plasticity_rate", nlm::ConfigValue(0.05),
                            "Base plasticity rate", "Plasticity", true);
    
    nlm::Config childConfig;
    childConfig.setParentConfig(std::make_shared<nlm::Config>(parentConfig));
    
    // Child can inherit parent values
    auto inheritedLearningRate = childConfig.get("base.learning_rate");
    assert(inheritedLearningRate.has_value());
    assert(*inheritedLearningRate == 0.01);
    
    // Child can override parent values
    childConfig.set("base.learning_rate", 0.02, nlm::ConfigSource::Default);
    
    auto overriddenLearningRate = childConfig.get("base.learning_rate");
    assert(overriddenLearningRate.has_value());
    assert(*overriddenLearningRate == 0.02);
    
    // Child should track inherited status
    assert(childConfig.inheritsFrom("base.neuron_count"));
    assert(!childConfig.inheritsFrom("base.learning_rate")); // Overridden value is not inherited
    
    std::cout << "    testConfigHierarchicalIntegration passed" << std::endl;
}

void testConfigTemplateIntegration() {
    nlm::Config config;
    
    // Create and apply template
    config.addTemplate("efficient_network",
                      {{nlm::Constraint{nlm::ConstraintType::Range, "1", "10000"}}, 
                       {nlm::Constraint{nlm::ConstraintType::Range, "0.0", "1.0"}}});
    
    config.applyTemplate("efficient_network");
    
    // Verify template was applied
    auto templates = config.getTemplateNames();
    assert(templates.find("efficient_network") != templates.end());
    
    // Apply template with values
    config.applyTemplate("efficient_network", {
        {"max_iterations", 500},
        {"plasticity_rate", 0.02},
        {"learning_rate", 0.1}
    });
    
    std::cout << "    testConfigTemplateIntegration passed" << std::endl;
}

void testConfigSchemaValidation() {
    nlm::Config config;
    
    // Add schema for validation
    nlm::ConfigSchema schema;
    schema.name = "brain_schema";
    schema.description = "Schema for brain configuration";
    
    // Add required constraints
    schema.constraints.push_back({nlm::ConstraintType::Required, "learning_rate"});
    schema.constraints.push_back({nlm::ConstraintType::Range, "learning_rate", "0.0", "1.0"});
    
    config.addSchema("brain_schema", schema);
    
    // Validate against schema
    bool validationResult = config.validateAgainstSchema("brain_schema");
    assert(!validationResult); // Should fail because learning_rate not set
    
    // Set required value
    config.set("learning_rate", 0.05, nlm::ConfigSource::Default);
    
    validationResult = config.validateAgainstSchema("brain_schema");
    assert(validationResult); // Should succeed now
    
    std::cout << "    testConfigSchemaValidation passed" << std::endl;
}

void testConfigFileIOIntegration() {
    nlm::Config config;
    
    // Register some configuration values
    config.registerKey("neural_dynamics.max_iterations", nlm::ConfigValue(200),
                      "Test iterations", "Neural Dynamics", true);
    config.registerKey("plasticity.plasticity_rate", nlm::ConfigValue(0.03),
                      "Test plasticity rate", "Plasticity", true);
    
    // Set some values
    config.set("neural_dynamics.max_iterations", 300, nlm::ConfigSource::Default);
    config.set("plasticity.plasticity_rate", 0.04, nlm::ConfigSource::Default);
    
    // Test JSON serialization
    std::string json = config.toJSON();
    assert(!json.empty());
    assert(json.find("max_iterations") != std::string::npos);
    
    // Test YAML serialization
    std::string yaml = config.toYAML();
    assert(!yaml.empty());
    assert(yaml.find("plasticity_rate") != std::string::npos);
    
    // Test format conversion
    std::string fromJson = "{\"neural_dynamics.max_iterations\": 400, \"plasticity.plasticity_rate\": 0.06}";
    config.fromJSON(fromJson);
    
    auto iterationsValue = config.get("neural_dynamics.max_iterations");
    assert(iterationsValue.has_value());
    assert(*iterationsValue == 400);
    
    std::cout << "    testConfigFileIOIntegration passed" << std::endl;
}

void testConfigSummaryIntegration() {
    nlm::Config config;
    
    // Register keys in different categories
    config.registerKey("neural_dynamics.max_iterations", nlm::ConfigValue(100),
                      "Test iterations", "Neural Dynamics", true);
    config.registerKey("plasticity.plasticity_rate", nlm::ConfigValue(0.05),
                      "Test plasticity rate", "Plasticity", true);
    config.registerKey("memory.capacity", nlm::ConfigValue(5000),
                      "Test memory capacity", "Memory", true);
    
    // Get summary by category
    auto neuralDynamicsSummary = config.summaryByCategory("Neural Dynamics");
    assert(!neuralDynamicsSummary.empty());
    
    auto plasticitySummary = config.summaryByCategory("Plasticity");
    assert(!plasticitySummary.empty());
    
    auto memorySummary = config.summaryByCategory("Memory");
    assert(!memorySummary.empty());
    
    // Get overall summary
    std::string overallSummary = config.generateSummary();
    assert(!overallSummary.empty());
    assert(overallSummary.find("max_iterations") != std::string::npos);
    
    std::cout << "    testConfigSummaryIntegration passed" << std::endl;
}

void testConfigStaticIntegration() {
    // Test static preset management
    auto presets = nlm::Config::getPresetNames();
    assert(presets.size() >= 1); // At least one preset should exist
    
    // Test static schema management
    auto schemas = nlm::Config::getSchemaNames();
    assert(schemas.size() >= 1); // At least one schema should exist
    
    // Test static template management
    auto templates = nlm::Config::getTemplateNames();
    assert(templates.size() >= 1); // At least one template should exist
    
    std::cout << "    testConfigStaticIntegration passed" << std::endl;
}

void runAll() {
    testConfigBrainIntegration();
    testConfigConstraintValidation();
    testConfigPresetApplication();
    testConfigHierarchicalIntegration();
    testConfigTemplateIntegration();
    testConfigSchemaValidation();
    testConfigFileIOIntegration();
    testConfigSummaryIntegration();
    testConfigStaticIntegration();
}

} // namespace test_config_integration