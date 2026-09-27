// ConfigParser Tests
#include "core/Config/ConfigParser.hpp"
#include <cassert>
#include <iostream>
#include <filesystem>

namespace test_config_parser {

void testJSONParsing() {
    nlm::ConfigParser parser;
    
    // Test simple JSON
    std::string jsonStr = R"({
        "neuron_count": 100,
        "connection_probability": 0.1,
        "enable_plasticity": true,
        "name": "TestNetwork",
        "weights": [0.1, 0.2, 0.3, 0.4, 0.5],
        "parameters": {
            "learning_rate": 0.01,
            "decay_rate": 0.99
        }
    })";
    
    auto config = parser.parseJSON(jsonStr);
    
    // Verify parsing
    assert(config.find("neuron_count") != config.end());
    assert(std::get<int>(config["neuron_count"]) == 100);
    
    assert(config.find("connection_probability") != config.end());
    assert(std::get<double>(config["connection_probability"]) == 0.1);
    
    assert(config.find("enable_plasticity") != config.end());
    assert(std::get<bool>(config["enable_plasticity"]) == true);
    
    assert(config.find("name") != config.end());
    assert(std::get<std::string>(config["name"]) == "TestNetwork");
    
    assert(config.find("weights") != config.end());
    assert(std::get<std::vector<double>>(config["weights"]).size() == 5);
    
    assert(config.find("parameters") != config.end());
    auto& params = std::get<std::unordered_map<std::string, std::variant<int, double, bool, std::string, std::vector<int>, std::vector<double>, std::vector<std::string>>>>(config["parameters"]);
    assert(params.find("learning_rate") != params.end());
    assert(std::get<double>(params["learning_rate"]) == 0.01);
    
    std::cout << "    testJSONParsing passed" << std::endl;
}

void testYAMLParsing() {
    nlm::ConfigParser parser;
    
    // Test YAML string
    std::string yamlStr = R"(
neuron_count: 50
connection_probability: 0.05
enable_plasticity: false
name: "YAMLNetwork"
weights: [0.2, 0.4, 0.6, 0.8, 1.0]
parameters:
    learning_rate: 0.005
    decay_rate: 0.95
    enable_learning: true
)";
    
    auto config = parser.parseYAML(yamlStr);
    
    // Verify parsing
    assert(config.find("neuron_count") != config.end());
    assert(std::get<int>(config["neuron_count"]) == 50);
    
    assert(config.find("connection_probability") != config.end());
    assert(std::get<double>(config["connection_probability"]) == 0.05);
    
    assert(config.find("enable_plasticity") != config.end());
    assert(std::get<bool>(config["enable_plasticity"]) == false);
    
    assert(config.find("name") != config.end());
    assert(std::get<std::string>(config["name"]) == "YAMLNetwork");
    
    assert(config.find("weights") != config.end());
    assert(std::get<std::vector<double>>(config["weights"]).size() == 5);
    
    assert(config.find("parameters") != config.end());
    auto& params = std::get<std::unordered_map<std::string, std::variant<int, double, bool, std::string, std::vector<int>, std::vector<double>, std::vector<std::string>>>>(config["parameters"]);
    assert(params.find("learning_rate") != params.end());
    assert(std::get<double>(params["learning_rate"]) == 0.005);
    assert(params.find("enable_learning") != params.end());
    assert(std::get<bool>(params["enable_learning"]) == true);
    
    std::cout << "    testYAMLParsing passed" << std::endl;
}

void testFileTypeDetection() {
    nlm::ConfigParser parser;
    
    // Test JSON file detection
    assert(parser.isJSONFile("config.json") == true);
    assert(parser.isJSONFile("settings.JSON") == true);
    assert(parser.isJSONFile("config.yml") == false);
    assert(parser.isJSONFile("config.yaml") == false);
    
    // Test YAML file detection
    assert(parser.isYAMLFile("config.yml") == true);
    assert(parser.isYAMLFile("settings.YAML") == true);
    assert(parser.isYAMLFile("config.json") == false);
    assert(parser.isYAMLFile("config.txt") == false);
    
    // Test content-based detection (simulate with files)
    std::string jsonContent = R"({"key": "value"})";
    std::string yamlContent = R"(key: value)";
    
    // Note: This would require actual file writing for full testing
    // For now, we rely on extension-based detection
    
    std::cout << "    testFileTypeDetection passed" << std::endl;
}

void testFlattenConfig() {
    nlm::ConfigParser parser;
    
    // Create nested config
    std::unordered_map<std::string, std::variant<int, double, bool, std::string, std::vector<int>, std::vector<double>, std::vector<std::string>, std::unordered_map<std::string, std::variant<int, double, bool, std::string, std::vector<int>, std::vector<double>, std::vector<std::string>>>>> nestedConfig;
    
    nestedConfig["layer1"] = std::unordered_map<std::string, std::variant<int, double, bool, std::string, std::vector<int>, std::vector<double>, std::vector<std::string>, std::unordered_map<std::string, std::variant<int, double, bool, std::string, std::vector<int>, std::vector<double>, std::vector<std::string>>>>>{
        {"neurons", 100},
        {"activation", "relu"},
        {"weights", std::vector<double>{0.1, 0.2, 0.3}}
    };
    
    nestedConfig["layer2"] = std::unordered_map<std::string, std::variant<int, double, bool, std::string, std::vector<int>, std::vector<double>, std::vector<std::string>, std::unordered_map<std::string, std::variant<int, double, bool, std::string, std::vector<int>, std::vector<double>, std::vector<std::string>>>>>{
        {"neurons", 50},
        {"activation", "sigmoid"}
    };
    
    std::vector<std::pair<std::string, std::variant<int, double, bool, std::string, std::vector<int>, std::vector<double>, std::vector<std::string>>>> flatConfig;
    parser.flattenConfig(nestedConfig, flatConfig);
    
    // Verify flattened keys
    bool foundLayer1Neurons = false;
    bool foundLayer1Activation = false;
    bool foundLayer2Neurons = false;
    bool foundLayer2Activation = false;
    
    for (const auto& pair : flatConfig) {
        if (pair.first == "layer1.neurons") {
            assert(std::get<int>(pair.second) == 100);
            foundLayer1Neurons = true;
        } else if (pair.first == "layer1.activation") {
            assert(std::get<std::string>(pair.second) == "relu");
            foundLayer1Activation = true;
        } else if (pair.first == "layer2.neurons") {
            assert(std::get<int>(pair.second) == 50);
            foundLayer2Neurons = true;
        } else if (pair.first == "layer2.activation") {
            assert(std::get<std::string>(pair.second) == "sigmoid");
            foundLayer2Activation = true;
        }
    }
    
    assert(foundLayer1Neurons);
    assert(foundLayer1Activation);
    assert(foundLayer2Neurons);
    assert(foundLayer2Activation);
    
    std::cout << "    testFlattenConfig passed" << std::endl;
}

void testConfigEntryToJSON() {
    nlm::ConfigParser parser;
    
    // Create a ConfigEntry
    nlm::ConfigEntry entry;
    entry.key = "test_key";
    entry.value = 42;
    entry.source = nlm::ConfigSource::File;
    entry.description = "Test configuration entry";
    
    auto json = parser.configEntryToJSON(entry);
    
    // Verify JSON structure
    assert(json["key"] == "test_key");
    assert(json["value"] == 42);
    assert(json["source"] == static_cast<int>(nlm::ConfigSource::File));
    assert(json["description"] == "Test configuration entry");
    
    std::cout << "    testConfigEntryToJSON passed" << std::endl;
}

void testConfigEntryToYAML() {
    nlm::ConfigParser parser;
    
    // Create a ConfigEntry
    nlm::ConfigEntry entry;
    entry.key = "yaml_key";
    entry.value = 3.14;
    entry.source = nlm::ConfigSource::CommandLine;
    entry.description = "YAML test entry";
    
    auto yaml = parser.configEntryToYAML(entry);
    
    // Verify YAML structure (basic checks)
    assert(yaml["key"].as<std::string>() == "yaml_key");
    assert(yaml["value"].as<double>() == 3.14);
    assert(yaml["source"].as<int>() == static_cast<int>(nlm::ConfigSource::CommandLine));
    assert(yaml["description"].as<std::string>() == "YAML test entry");
    
    std::cout << "    testConfigEntryToYAML passed" << std::endl;
}

void testValidation() {
    nlm::ConfigParser parser;
    
    // Test valid config
    std::unordered_map<std::string, std::variant<int, double, bool, std::string, std::vector<int>, std::vector<double>, std::vector<std::string>, std::unordered_map<std::string, std::variant<int, double, bool, std::string, std::vector<int>, std::vector<double>, std::vector<std::string>>>>> validConfig;
    validConfig["valid_key"] = 123;
    validConfig["nested"] = std::unordered_map<std::string, std::variant<int, double, bool, std::string, std::vector<int>, std::vector<double>, std::vector<std::string>, std::unordered_map<std::string, std::variant<int, double, bool, std::string, std::vector<int>, std::vector<double>, std::vector<std::string>>>>>{
        {"sub_key", "value"}
    };
    
    assert(parser.validateConfig(validConfig) == true);
    
    // Test invalid config (empty key)
    std::unordered_map<std::string, std::variant<int, double, bool, std::string, std::vector<int>, std::vector<double>, std::vector<std::string>, std::unordered_map<std::string, std::variant<int, double, bool, std::string, std::vector<int>, std::vector<double>, std::vector<std::string>>>>> invalidConfig;
    invalidConfig[""] = 123; // Empty key
    
    // This should throw ValidationError
    try {
        parser.validateConfig(invalidConfig);
        assert(false); // Should not reach here
    } catch (const nlm::ConfigParser::ValidationError& e) {
        // Expected
    }
    
    std::cout << "    testValidation passed" << std::endl;
}

void runAll() {
    std::cout << "Running ConfigParser tests..." << std::endl;
    
    testJSONParsing();
    testYAMLParsing();
    testFileTypeDetection();
    testFlattenConfig();
    testConfigEntryToJSON();
    testConfigEntryToYAML();
    testValidation();
    
    std::cout << "All ConfigParser tests PASSED" << std::endl;
}

} // namespace test_config_parser