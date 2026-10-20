// Configuration Validation Tests
// Tests for configuration schema validation and error handling in the NLM configuration system

#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>
#include <vector>

namespace test_config_validation {

void testSchemaCreation() {
    nlm::ConfigSchema schema;
    
    // Create test properties
    nlm::ConfigSchema::ConfigProperty neuronCountProp {
        "brain.neuron_count",
        "int",
        1000,
        [](const std::string&, const nlm::ConfigValue& value) {
            int count = std::get<int>(value);
            return count > 0 && count <= 100000;
        },
        "Number of neurons (must be 1-100000)"
    };
    
    nlm::ConfigSchema::ConfigProperty learningRateProp {
        "plasticity.stdp.learning_rate",
        "double",
        0.001,
        [](const std::string&, const nlm::ConfigValue& value) {
            double rate = std::get<double>(value);
            return rate >= 0.0 && rate <= 1.0;
        },
        "STDP learning rate (must be 0.0-1.0)"
    };
    
    nlm::ConfigSchema::ConfigProperty timestepProp {
        "simulation.timestep",
        "double",
        0.001,
        [](const std::string&, const nlm::ConfigValue& value) {
            double timestep = std::get<double>(value);
            return timestep > 0.0 && timestep <= 1.0;
        },
        "Simulation timestep (must be > 0.0 and <= 1.0)"
    };
    
    schema.addProperty(neuronCountProp);
    schema.addProperty(learningRateProp);
    schema.addProperty(timestepProp);
    
    // Verify properties were added
    assert(schema.getProperties().size() == 3);
    
    std::cout << "    testSchemaCreation passed" << std::endl;
}

void testSchemaValidationPass() {
    nlm::Config config;
    
    // Create schema
    nlm::ConfigSchema schema;
    schema.addProperty(nlm::ConfigSchema::ConfigProperty {
        "brain.neuron_count",
        "int",
        1000,
        [](const std::string&, const nlm::ConfigValue& value) {
            int count = std::get<int>(value);
            return count > 0 && count <= 100000;
        },
        "Neuron count validator"
    });
    
    config.setSchema(schema);
    
    // Add valid values
    config.set("brain.neuron_count", 5000, nlm::ConfigSource::Runtime);
    config.set("plasticity.stdp.learning_rate", 0.01, nlm::ConfigSource::Runtime);
    
    // Validate should pass for known properties
    assert(config.validate("brain.neuron_count", 5000));
    
    std::cout << "    testSchemaValidationPass passed" << std::endl;
}

void testSchemaValidationFail() {
    nlm::Config config;
    
    // Create schema with strict validation
    nlm::ConfigSchema schema;
    schema.addProperty(nlm::ConfigSchema::ConfigProperty {
        "brain.neuron_count",
        "int",
        1000,
        [](const std::string&, const nlm::ConfigValue& value) {
            int count = std::get<int>(value);
            return count >= 100 && count <= 10000;  // Strict range
        },
        "Neuron count (must be 100-10000)"
    });
    
    config.setSchema(schema);
    
    // Add value outside range
    config.set("brain.neuron_count", 50000, nlm::ConfigSource::Runtime);
    
    // Validation should fail
    assert(!config.validate("brain.neuron_count", 50000));
    
    std::cout << "    testSchemaValidationFail passed" << std::endl;
}

void testConfigFileValidation() {
    nlm::Config config;
    
    // Load configuration
    assert(config.loadFromFile("configs/nlm_config.json"));
    
    // Create schema for validation
    nlm::ConfigSchema schema;
    schema.addProperty(nlm::ConfigSchema::ConfigProperty {
        "brain.neuron_count",
        "int",
        1000,
        [](const std::string&, const nlm::ConfigValue& value) {
            int count = std::get<int>(value);
            return count >= 1000 && count <= 5000;
        },
        "Neuron count validation"
    });
    
    config.setSchema(schema);
    
    // Validate each property in the config
    auto keys = config.getKeys();
    for (const auto& key : keys) {
        // Create a temporary entry for validation
        nlm::ConfigEntry entry;
        entry.key = key;
        
        // Get the value from config
        auto intVal = config.get<int>(key);
        if (intVal.has_value()) {
            entry.value = *intVal;
        } else {
            auto doubleVal = config.get<double>(key);
            if (doubleVal.has_value()) {
                entry.value = *doubleVal;
            } else {
                auto stringVal = config.get<std::string>(key);
                if (stringVal.has_value()) {
                    entry.value = *stringVal;
                } else {
                    auto boolVal = config.get<bool>(key);
                    if (boolVal.has_value()) {
                        entry.value = *boolVal;
                    }
                }
            }
        }
        
        entry.source = nlm::ConfigSource::File;
        entry.description = "";
        
        // Validate the entry
        if (schema.findProperty(key).has_value()) {
            assert(schema.validate(entry));
        }
    }
    
    std::cout << "    testConfigFileValidation passed" << std::endl;
}

void testValidationErrorMessages() {
    nlm::Config config;
    
    // Create schema with custom validation
    nlm::ConfigSchema schema;
    schema.addProperty(nlm::ConfigSchema::ConfigProperty {
        "brain.neuron_count",
        "int",
        1000,
        [](const std::string&, const nlm::ConfigValue& value) {
            int count = std::get<int>(value);
            if (count < 100) {
                throw nlm::ConfigValidationError("Neuron count too low (minimum 100)");
            }
            if (count > 10000) {
                throw nlm::ConfigValidationError("Neuron count too high (maximum 10000)");
            }
            return true;
        },
        "Neuron count with error messages"
    });
    
    config.setSchema(schema);
    
    // Test error handling for low values
    try {
        config.set("brain.neuron_count", 50);
        assert(false); // Should have thrown an exception
    } catch (const nlm::ConfigValidationError& e) {
        assert(std::string(e.what()).find("too low") != std::string::npos);
    }
    
    // Test error handling for high values
    try {
        config.set("brain.neuron_count", 20000);
        assert(false); // Should have thrown an exception
    } catch (const nlm::ConfigValidationError& e) {
        assert(std::string(e.what()).find("too high") != std::string::npos);
    }
    
    std::cout << "    testValidationErrorMessages passed" << std::endl;
}

void testDefaultValues() {
    nlm::Config config;
    
    // Create schema with default values
    nlm::ConfigSchema schema;
    
    // Add properties with defaults
    nlm::ConfigSchema::ConfigProperty defaultProp {
        "brain.neuron_count",
        "int",
        1000,  // Default value
        nullptr,  // No custom validator
        "Default neuron count"
    };
    
    schema.addProperty(defaultProp);
    
    // Set schema
    config.setSchema(schema);
    
    // Test that missing keys use default values
    auto neuronCount = config.getOr<int>("brain.neuron_count", 0);
    assert(neuronCount == 1000);
    
    // Test that we can override default values
    config.set("brain.neuron_count", 2000);
    assert(config.getOr<int>("brain.neuron_count", 0) == 2000);
    
    std::cout << "    testDefaultValues passed" << std::endl;
}

void testValidationSummary() {
    nlm::Config config;
    
    // Load configuration
    assert(config.loadFromFile("configs/nlm_config.json"));
    
    // Create schema for comprehensive validation
    nlm::ConfigSchema schema;
    
    // Add multiple properties with different types
    auto addStringProp = [&schema](const std::string& key, const std::string& defaultValue) {
        nlm::ConfigSchema::ConfigProperty prop {
            key,
            "string",
            defaultValue,
            nullptr,
            "String property"
        };
        schema.addProperty(prop);
    };
    
    addStringProp("brain.name", "Default Brain");
    addStringProp("simulation.type", "integration_test");
    
    auto addIntProp = [&schema](const std::string& key, int defaultValue) {
        nlm::ConfigSchema::ConfigProperty prop {
            key,
            "int",
            defaultValue,
            [](const std::string&, const nlm::ConfigValue& value) {
                int val = std::get<int>(value);
                return val >= 0;
            },
            "Integer property"
        };
        schema.addProperty(prop);
    };
    
    addIntProp("brain.version", 1);
    
    config.setSchema(schema);
    
    // Validate configuration summary
    auto keys = config.getKeys();
    assert(keys.size() > 0);
    
    // Check that we can access and validate all properties
    for (const auto& key : keys) {
        if (config.has(key)) {
            // Create temporary entry for validation
            nlm::ConfigEntry entry;
            entry.key = key;
            
            // Get the value based on type
            auto val = config.get<std::string>(key);
            if (val.has_value()) {
                entry.value = *val;
            } else {
                val = config.get<int>(key);
                if (val.has_value()) {
                    entry.value = *val;
                } else {
                    auto doubleVal = config.get<double>(key);
                    if (doubleVal.has_value()) {
                        entry.value = *doubleVal;
                    } else {
                        auto boolVal = config.get<bool>(key);
                        if (boolVal.has_value()) {
                            entry.value = *boolVal;
                        }
                    }
                }
            }
            
            entry.source = nlm::ConfigSource::File;
            entry.description = "";
            
            // Schema validation
            if (schema.findProperty(key).has_value()) {
                assert(schema.validate(entry));
            }
        }
    }
    
    std::cout << "    testValidationSummary passed" << std::endl;
}

void runAll() {
    testSchemaCreation();
    testSchemaValidationPass();
    testSchemaValidationFail();
    testConfigFileValidation();
    testValidationErrorMessages();
    testDefaultValues();
    testValidationSummary();
}

} // namespace test_config_validation

// Simple test for configuration validation
int main() {
    std::cout << "=== NLM Configuration Validation Tests ===" << std::endl;
    test_config_validation::runAll();
    std::cout << "=== All configuration validation tests passed! ===" << std::endl;
    return 0;
}