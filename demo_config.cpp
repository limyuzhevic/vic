#!/usr/bin/env cpp
// Simple demonstration of the enhanced Config class

#include "src/core/Config/Config.hpp"
#include <iostream>
#include <string>

int main() {
    std::cout << "=== Enhanced Config Class Demonstration ===" << std::endl;
    std::cout << std::endl;
    
    // Create a configuration object
    nlm::Config config;
    
    // 1. Register keys with categories and descriptions
    std::cout << "1. Advanced Key Registration:" << std::endl;
    config.registerKey("neural_dynamics.max_iterations", nlm::ConfigValue(100),
                      "Maximum iterations for neural dynamics simulation", 
                      "Neural Dynamics", true);
    
    config.registerKey("plasticity.plasticity_rate", nlm::ConfigValue(0.05),
                      "Plasticity learning rate", "Plasticity", true);
    
    config.registerKey("memory.capacity", nlm::ConfigValue(5000),
                      "Memory capacity for working memory", "Memory", true);
    
    config.registerKey("sensory.sensitivity", nlm::ConfigValue(1.0),
                      "Sensory system sensitivity", "Sensory", false);
    
    std::cout << "   Registered keys in categories:" << std::endl;
    auto neuralKeys = config.getKeysByCategory("Neural Dynamics");
    for (const auto& key : neuralKeys) {
        std::cout << "     - " << key << std::endl;
    }
    std::cout << std::endl;
    
    // 2. Demonstrate constraints
    std::cout << "2. Configuration Constraints:" << std::endl;
    config.registerKeyWithConstraints("learning_rate", nlm::ConfigValue(0.01),
                                      "Learning rate for neural networks", "Training",
                                      {nlm::Constraint{nlm::ConstraintType::Range, "0.0", "1.0"}});
    
    config.registerKeyWithConstraints("population_size", nlm::ConfigValue(100),
                                      "Population size for evolutionary algorithm", "Evolution",
                                      {nlm::Constraint{nlm::ConstraintType::Range, "10", "10000"}});
    
    config.set("learning_rate", 0.1, nlm::ConfigSource::Runtime);
    config.set("population_size", 500, nlm::ConfigSource::Runtime);
    
    std::cout << "   Set constrained values: learning_rate=0.1, population_size=500" << std::endl;
    
    auto validationResult = config.validate();
    std::cout << "   Validation: " << (validationResult.valid ? "PASSED" : "FAILED") << std::endl;
    std::cout << std::endl;
    
    // 3. Demonstrate presets
    std::cout << "3. Preset Configuration:" << std::endl;
    nlm::Preset performancePreset;
    performancePreset.name = "performance";
    performancePreset.description = "High-performance configuration";
    performancePreset.values = {
        {"neural_dynamics.max_iterations", nlm::ConfigValue(50)},
        {"memory.capacity", nlm::ConfigValue(2000)}
    };
    performancePreset.dependencies = {};
    config.addPreset(performancePreset);
    
    config.applyPreset("performance");
    std::cout << "   Applied 'performance' preset" << std::endl;
    std::cout << "   neural_dynamics.max_iterations = " << config.getOr<int>("neural_dynamics.max_iterations", 0) << std::endl;
    std::cout << std::endl;
    
    // 4. Demonstrate hierarchical configuration
    std::cout << "4. Hierarchical Configuration:" << std::endl;
    nlm::Config parentConfig;
    parentConfig.registerKey("base.learning_rate", nlm::ConfigValue(0.01),
                            "Base learning rate", "Training", true);
    
    nlm::Config childConfig;
    childConfig.setParentConfig(std::make_shared<nlm::Config>(parentConfig));
    
    auto inheritedLearningRate = childConfig.getOr<double>("base.learning_rate", 0.001);
    std::cout << "   Child inherited learning_rate = " << inheritedLearningRate << std::endl;
    
    childConfig.set("base.learning_rate", 0.05, nlm::ConfigSource::Runtime);
    auto overriddenLearningRate = childConfig.getOr<double>("base.learning_rate", 0.001);
    std::cout << "   Child overrode learning_rate = " << overriddenLearningRate << std::endl;
    std::cout << std::endl;
    
    // 5. Demonstrate file I/O
    std::cout << "5. File I/O Operations:" << std::endl;
    config.registerKey("file_test.string", nlm::ConfigValue("test"), "Test string value", "File Test", true);
    config.set("file_test.string", "modified_value", nlm::ConfigSource::Runtime);
    
    std::string testFile = "./demo_config.json";
    bool saved = config.saveToFile(testFile, "json");
    std::cout << "   Saved configuration to " << testFile << " (" << (saved ? "success" : "failed") << ")" << std::endl;
    
    nlm::Config loadedConfig;
    bool loaded = loadedConfig.loadFromFile(testFile, "json");
    std::cout << "   Loaded configuration from " << testFile << " (" << (loaded ? "success" : "failed") << ")" << std::endl;
    
    if (loaded) {
        auto loadedValue = loadedConfig.get<std::string>("file_test.string");
        std::cout << "   Loaded value: " << (loadedValue.has_value() ? *loadedValue : "N/A") << std::endl;
    }
    
    std::remove(testFile.c_str());
    std::cout << std::endl;
    
    // 6. Demonstrate schema validation
    std::cout << "6. Schema Validation:" << std::endl;
    nlm::ConfigSchema schema;
    schema.name = "demo_schema";
    schema.description = "Demo configuration schema";
    
    config.addSchema(schema);
    bool schemaValid = config.validateAgainstSchema("demo_schema");
    std::cout << "   Configuration valid against schema: " << (schemaValid ? "YES" : "NO") << std::endl;
    std::cout << std::endl;
    
    // 7. Show configuration summary
    std::cout << "7. Configuration Summary:" << std::endl;
    std::cout << config.summary() << std::endl;
    
    std::cout << "=== Demonstration Complete ===" << std::endl;
    return 0;
}
