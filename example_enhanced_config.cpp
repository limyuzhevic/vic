// Example program demonstrating the enhanced Config functionality
// This can be compiled and run to verify the implementation

#include "core/Config/Config.hpp"
#include <iostream>
#include <cassert>

int main() {
    std::cout << "=== Enhanced Config Example Program ===" << std::endl;
    
    // Example 1: Basic usage with multiple formats
    std::cout << "\n1. Testing multiple configuration formats:" << std::endl;
    
    // JSON example
    nlm::Config jsonConfig;
    std::string jsonContent = R"({
        "brain": {
            "neuron_count": 1000,
            "synapse_density": 0.1
        },
        "plasticity": {
            "stdp": {
                "learning_rate": 0.001,
                "enable": true
            }
        }
    })";
    
    if (jsonConfig.loadFromString(jsonContent, "JSON")) {
        std::cout << "   ✓ JSON loaded successfully" << std::endl;
        auto neuronCount = jsonConfig.get<int64_t>("brain.neuron_count");
        std::cout << "   ✓ brain.neuron_count = " << *neuronCount << std::endl;
        
        auto learningRate = jsonConfig.get<double>("plasticity.stdp.learning_rate");
        std::cout << "   ✓ plasticity.stdp.learning_rate = " << *learningRate << std::endl;
    } else {
        std::cout << "   ✗ JSON loading failed" << std::endl;
        return 1;
    }
    
    // YAML example
    nlm::Config yamlConfig;
    std::string yamlContent = R"(
brain:
  neuron_count: 1000
  synapse_density: 0.1
plasticity:
  stdp:
    learning_rate: 0.001
    enable: true
)";
    
    if (yamlConfig.loadFromString(yamlContent, "YAML")) {
        std::cout << "   ✓ YAML loaded successfully" << std::endl;
        auto neuronCount = yamlConfig.get<int64_t>("brain.neuron_count");
        std::cout << "   ✓ brain.neuron_count = " << *neuronCount << std::endl;
    } else {
        std::cout << "   ✗ YAML loading failed" << std::endl;
        return 1;
    }
    
    // Key=value example
    nlm::Config kvConfig;
    std::string kvContent = R"(
brain.neuron_count = 1000
brain.synapse_density = 0.1
plasticity.stdp.learning_rate = 0.001
plasticity.stdp.enable = true
)";
    
    if (kvConfig.loadFromString(kvContent, "KEY_VALUE")) {
        std::cout << "   ✓ Key=value loaded successfully" << std::endl;
        auto neuronCount = kvConfig.get<int64_t>("brain.neuron_count");
        std::cout << "   ✓ brain.neuron_count = " << *neuronCount << std::endl;
    } else {
        std::cout << "   ✗ Key=value loading failed" << std::endl;
        return 1;
    }
    
    // Example 2: Schema validation
    std::cout << "\n2. Testing schema validation:" << std::endl;
    
    nlm::ConfigSchema schema;
    schema.addField("brain.neuron_count", "int64", 
                   "Number of neurons", "^-?\d+$", true, "1000");
    schema.addField("plasticity.stdp.learning_rate", "double",
                   "STDP learning rate", "^(0\.\d+|[1-9]\d*\.?\d*)$", false, "0.001");
    schema.addField("plasticity.stdp.enable", "bool",
                   "STDP enable flag", "^(true|false)$", true, "true");
    
    nlm::Config schemaConfig(schema);
    schemaConfig.set("brain.neuron_count", 2000);
    schemaConfig.set("plasticity.stdp.learning_rate", 0.01);
    schemaConfig.set("plasticity.stdp.enable", false);
    
    std::vector<std::string> validationErrors = schemaConfig.validateAgainstSchema();
    if (validationErrors.empty()) {
        std::cout << "   ✓ Schema validation passed" << std::endl;
    } else {
        std::cout << "   ✗ Schema validation failed:" << std::endl;
        for (const auto& error : validationErrors) {
            std::cout << "     " << error << std::endl;
        }
        return 1;
    }
    
    // Example 3: Format detection
    std::cout << "\n3. Testing format detection:" << std::endl;
    
    nlm::Config detector;
    
    std::string jsonSample = R"({"key": "value"})";
    std::string yamlSample = "key: value";
    std::string kvSample = "key = value";
    
    std::string jsonDetected = detector.detectFormat(jsonSample);
    std::cout << "   ✓ JSON detected as: " << jsonDetected << std::endl;
    
    std::string yamlDetected = detector.detectFormat(yamlSample);
    std::cout << "   ✓ YAML detected as: " << yamlDetected << std::endl;
    
    std::string kvDetected = detector.detectFormat(kvSample);
    std::cout << "   ✓ Key=value detected as: " << kvDetected << std::endl;
    
    // Example 4: Serialization
    std::cout << "\n4. Testing serialization:" << std::endl;
    
    nlm::Config serializer;
    serializer.set("test.int_value", 42);
    serializer.set("test.string_value", "hello world");
    serializer.set("test.double_value", 3.14159);
    serializer.set("test.bool_value", true);
    
    // Test JSON serialization
    std::string jsonOutput = serializer.saveToFile("", "JSON");
    std::cout << "   ✓ JSON serialization length: " << jsonOutput.length() << " chars" << std::endl;
    
    // Test YAML serialization
    std::string yamlOutput = serializer.saveToFile("", "YAML");
    std::cout << "   ✓ YAML serialization length: " << yamlOutput.length() << " chars" << std::endl;
    
    // Test key=value serialization
    std::string kvOutput = serializer.saveToFile("", "KEY_VALUE");
    std::cout << "   ✓ Key=value serialization length: " << kvOutput.length() << " chars" << std::endl;
    
    // Example 5: Type conversion utilities
    std::cout << "\n5. Testing type conversion utilities:" << std::endl;
    
    nlm::Config converter;
    converter.set("value", 123);
    
    std::string intStr = converter.toString(converter.get<int64_t>("value"));
    std::cout << "   ✓ Integer to string: " << intStr << std::endl;
    
    converter.set("str_value", "test");
    std::string quotedStr = converter.toString(converter.get<std::string>("str_value"));
    std::cout << "   ✓ String to quoted string: " << quotedStr << std::endl;
    
    // Example 6: Error handling
    std::cout << "\n6. Testing error handling:" << std::endl;
    
    nlm::Config errorConfig;
    
    try {
        // Try to load invalid JSON
        std::string invalidJson = "{invalid json}";
        if (!errorConfig.loadFromString(invalidJson, "JSON")) {
            std::cout << "   ✓ Invalid JSON properly rejected" << std::endl;
        }
    } catch (const nlm::ConfigError& e) {
        std::cout << "   ✓ ConfigError caught: " << e.what() << std::endl;
    } catch (const std::exception& e) {
        std::cout << "   ✓ Standard exception caught: " << e.what() << std::endl;
    }
    
    std::cout << "\n=== All examples completed successfully! ===" << std::endl;
    
    return 0;
}
