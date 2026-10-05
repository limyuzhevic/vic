// Config Tests with JSON/YAML Support
#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>
#include <fstream>
#include <filesystem>

namespace test_config {

void testConfigCreation() {
    nlm::Config config;
    assert(!config.has("test"));
    
    std::cout << "    testConfigCreation passed" << std::endl;
}

void testConfigSetAndGet() {
    nlm::Config config;
    
    config.set("int_value", 42, nlm::ConfigSource::Runtime);
    assert(config.has("int_value"));
    
    auto intVal = config.get<int>("int_value");
    assert(intVal.has_value());
    assert(*intVal == 42);
    
    config.set("double_value", 3.14, nlm::ConfigSource::Runtime);
    auto doubleVal = config.get<double>("double_value");
    assert(doubleVal.has_value());
    assert(*doubleVal > 3.13 && *doubleVal < 3.15);
    
    config.set("string_value", std::string("hello"), nlm::ConfigSource::Runtime);
    auto stringVal = config.get<std::string>("string_value");
    assert(stringVal.has_value());
    assert(*stringVal == "hello");
    
    config.set("bool_value", true, nlm::ConfigSource::Runtime);
    auto boolVal = config.get<bool>("bool_value");
    assert(boolVal.has_value());
    assert(*boolVal == true);
    
    std::cout << "    testConfigSetAndGet passed" << std::endl;
}

void testConfigGetOr() {
    nlm::Config config;
    
    int val = config.getOr<int>("missing", 100);
    assert(val == 100);
    
    config.set("existing", 50, nlm::ConfigSource::Runtime);
    val = config.getOr<int>("existing", 100);
    assert(val == 50);
    
    std::cout << "    testConfigGetOr passed" << std::endl;
}

void testConfigRemove() {
    nlm::Config config;
    
    config.set("test", 123, nlm::ConfigSource::Runtime);
    assert(config.has("test"));
    
    config.remove("test");
    assert(!config.has("test"));
    
    std::cout << "    testConfigRemove passed" << std::endl;
}

void testConfigKeys() {
    nlm::Config config;
    
    config.set("key1", 1, nlm::ConfigSource::Runtime);
    config.set("key2", 2, nlm::ConfigSource::Runtime);
    config.set("key3", 3, nlm::ConfigSource::Runtime);
    
    auto keys = config.getKeys();
    assert(keys.size() == 3);
    
    std::cout << "    testConfigKeys passed" << std::endl;
}

void testJSONParsing() {
    nlm::Config config;
    
    // Create a test JSON file
    std::string jsonContent = R"({
        "int_val": 42,
        "double_val": 3.14,
        "string_val": "hello",
        "bool_val": true,
        "int_array": [1, 2, 3],
        "double_array": [1.1, 2.2, 3.3],
        "string_array": ["a", "b", "c"]
    })";
    
    std::ofstream jsonFile("test_config.json");
    jsonFile << jsonContent;
    jsonFile.close();
    
    // Load JSON file
    bool success = config.loadFromFile("test_config.json");
    assert(success);
    
    // Verify loaded values
    assert(config.has("int_val"));
    assert(config.get<int>("int_val").value() == 42);
    
    assert(config.has("double_val"));
    assert(config.get<double>("double_val").value() > 3.13 && config.get<double>("double_val").value() < 3.15);
    
    assert(config.has("string_val"));
    assert(config.get<std::string>("string_val").value() == "hello");
    
    assert(config.has("bool_val"));
    assert(config.get<bool>("bool_val").value() == true);
    
    // Clean up
    std::filesystem::remove("test_config.json");
    
    std::cout << "    testJSONParsing passed" << std::endl;
}

void testYAMLParsing() {
    nlm::Config config;
    
    // Create a test YAML file
    std::string yamlContent = R"(
int_val: 42
double_val: 3.14
string_val: hello
bool_val: true
int_array:
  - 1
  - 2
  - 3
double_array:
  - 1.1
  - 2.2
  - 3.3
string_array:
  - a
  - b
  - c
)";
    
    std::ofstream yamlFile("test_config.yaml");
    yamlFile << yamlContent;
    yamlFile.close();
    
    // Load YAML file
    bool success = config.loadFromFile("test_config.yaml");
    assert(success);
    
    // Verify loaded values
    assert(config.has("int_val"));
    assert(config.get<int>("int_val").value() == 42);
    
    assert(config.has("double_val"));
    assert(config.get<double>("double_val").value() > 3.13 && config.get<double>("double_val").value() < 3.15);
    
    assert(config.has("string_val"));
    assert(config.get<std::string>("string_val").value() == "hello");
    
    assert(config.has("bool_val"));
    assert(config.get<bool>("bool_val").value() == true);
    
    // Clean up
    std::filesystem::remove("test_config.yaml");
    
    std::cout << "    testYAMLParsing passed" << std::endl;
}

void testNestedJSON() {
    nlm::Config config;
    
    // Create a test JSON file with nested structure
    std::string jsonContent = R"({
        "brain": {
            "neuron_count": 500,
            "connection_probability": 0.15
        },
        "environment": {
            "width": 10,
            "height": 10
        }
    })";
    
    std::ofstream jsonFile("test_nested.json");
    jsonFile << jsonContent;
    jsonFile.close();
    
    // Load JSON file
    bool success = config.loadFromFile("test_nested.json");
    assert(success);
    
    // Verify nested values
    assert(config.has("brain.neuron_count"));
    assert(config.get<int>("brain.neuron_count").value() == 500);
    
    assert(config.has("brain.connection_probability"));
    assert(config.get<double>("brain.connection_probability").value() > 0.14 && config.get<double>("brain.connection_probability").value() < 0.16);
    
    assert(config.has("environment.width"));
    assert(config.get<int>("environment.width").value() == 10);
    
    // Clean up
    std::filesystem::remove("test_nested.json");
    
    std::cout << "    testNestedJSON passed" << std::endl;
}

void testLegacyFormat() {
    nlm::Config config;
    
    // Create a test legacy config file
    std::string legacyContent = R"(
# Legacy configuration
random_seed = 42
simulation_timestep = 0.001
neuron_count = 1000
connection_probability = 0.1
)";
    
    std::ofstream legacyFile("test_config.cfg");
    legacyFile << legacyContent;
    legacyFile.close();
    
    // Load legacy file
    bool success = config.loadFromFile("test_config.cfg");
    assert(success);
    
    // Verify loaded values
    assert(config.has("random_seed"));
    assert(config.get<int>("random_seed").value() == 42);
    
    assert(config.has("simulation_timestep"));
    assert(config.get<double>("simulation_timestep").value() > 0.0009 && config.get<double>("simulation_timestep").value() < 0.0011);
    
    assert(config.has("neuron_count"));
    assert(config.get<int>("neuron_count").value() == 1000);
    
    assert(config.has("connection_probability"));
    assert(config.get<double>("connection_probability").value() > 0.09 && config.get<double>("connection_probability").value() < 0.11);
    
    // Clean up
    std::filesystem::remove("test_config.cfg");
    
    std::cout << "    testLegacyFormat passed" << std::endl;
}

void testErrorHandling() {
    nlm::Config config;
    
    // Test loading non-existent file
    bool success = config.loadFromFile("non_existent_file.json");
    assert(!success);
    
    // Test loading malformed JSON
    std::ofstream badJsonFile("bad_json.json");
    badJsonFile << "{ invalid json }";
    badJsonFile.close();
    
    success = config.loadFromFile("bad_json.json");
    assert(!success);  // Should fail due to malformed JSON
    
    std::filesystem::remove("bad_json.json");
    
    // Test loading malformed YAML
    std::ofstream badYamlFile("bad_yaml.yaml");
    badYamlFile << "invalid: [yaml: [unclosed";
    badYamlFile.close();
    
    success = config.loadFromFile("bad_yaml.yaml");
    assert(!success);  // Should fail due to malformed YAML
    
    std::filesystem::remove("bad_yaml.yaml");
    
    std::cout << "    testErrorHandling passed" << std::endl;
}

void testFileExtensionDetection() {
    nlm::Config config;
    
    // Test JSON file (.json)
    std::string extension = config.detectFileFormat("test.json");
    assert(extension == "json");
    
    // Test JSON file (.jsonc)
    extension = config.detectFileFormat("test.jsonc");
    assert(extension == "jsonc");
    
    // Test YAML file (.yaml)
    extension = config.detectFileFormat("test.yaml");
    assert(extension == "yaml");
    
    // Test YAML file (.yml)
    extension = config.detectFileFormat("test.yml");
    assert(extension == "yml");
    
    // Test legacy file (.cfg)
    extension = config.detectFileFormat("test.cfg");
    assert(extension == "cfg");
    
    // Test unknown extension (should default to legacy)
    extension = config.detectFileFormat("test.unknown");
    assert(extension == "unknown");  // Not empty, so won't be legacy
    
    // Test empty filename
    extension = config.detectFileFormat("");
    assert(extension == "");
    
    // Test filename with no extension
    extension = config.detectFileFormat("filename");
    assert(extension == "");
    
    std::cout << "    testFileExtensionDetection passed" << std::endl;
}

void testConfigValueConversion() {
    nlm::Config config;
    
    // Test setting and getting different types
    config.set("int_val", 42);
    auto intVal = config.get<int>("int_val");
    assert(intVal.has_value() && *intVal == 42);
    
    config.set("double_val", 3.14159);
    auto doubleVal = config.get<double>("double_val");
    assert(doubleVal.has_value() && *doubleVal > 3.14 && *doubleVal < 3.15);
    
    config.set("string_val", std::string("test_string"));
    auto stringVal = config.get<std::string>("string_val");
    assert(stringVal.has_value() && *stringVal == "test_string");
    
    config.set("bool_val", true);
    auto boolVal = config.get<bool>("bool_val");
    assert(boolVal.has_value() && *boolVal == true);
    
    // Test integer arrays
    config.set("int_array", std::vector<int>({1, 2, 3, 4, 5}));
    auto intArray = config.get<std::vector<int>>("int_array");
    assert(intArray.has_value() && intArray->size() == 5);
    assert((*intArray)[0] == 1 && (*intArray)[4] == 5);
    
    // Test double arrays
    config.set("double_array", std::vector<double>({1.1, 2.2, 3.3}));
    auto doubleArray = config.get<std::vector<double>>("double_array");
    assert(doubleArray.has_value() && doubleArray->size() == 3);
    assert((*doubleArray)[1] == 2.2);
    
    // Test string arrays
    config.set("string_array", std::vector<std::string>({"a", "b", "c"}));
    auto stringArray = config.get<std::vector<std::string>>("string_array");
    assert(stringArray.has_value() && stringArray->size() == 3);
    assert((*stringArray)[0] == "a");
    
    // Test getOr with default values
    assert(config.getOr<int>("missing", 999) == 999);
    assert(config.getOr<std::string>("missing", "default") == "default");
    assert(config.getOr<double>("missing", 3.14) > 3.13);
    
    std::cout << "    testConfigValueConversion passed" << std::endl;
}

void testConfigSourceTracking() {
    nlm::Config config;
    
    // Test different sources
    config.set("default_val", 100);
    assert(config.get<int>("default_val").value() == 100);
    
    config.set("file_val", 200, nlm::ConfigSource::File);
    auto fileVal = config.get<int>("file_val");
    assert(fileVal.has_value() && *fileVal == 200);
    
    config.set("cmd_val", 300, nlm::ConfigSource::CommandLine);
    auto cmdVal = config.get<int>("cmd_val");
    assert(cmdVal.has_value() && *cmdVal == 300);
    
    config.set("runtime_val", 400, nlm::ConfigSource::Runtime);
    auto runtimeVal = config.get<int>("runtime_val");
    assert(runtimeVal.has_value() && *runtimeVal == 400);
    
    // Test summary includes source information
    std::string summary = config.summary();
    assert(summary.find("default_val = [100]") != std::string::npos);
    assert(summary.find("file_val = [200]") != std::string::npos);
    assert(summary.find("cmd_val = [300]") != std::string::npos);
    assert(summary.find("runtime_val = [400]") != std::string::npos);
    
    std::cout << "    testConfigSourceTracking passed" << std::endl;
}

void runAll() {
    testConfigCreation();
    testConfigSetAndGet();
    testConfigGetOr();
    testConfigRemove();
    testConfigKeys();
    testJSONParsing();
    testYAMLParsing();
    testNestedJSON();
    testLegacyFormat();
    testErrorHandling();
    testFileExtensionDetection();
    testConfigValueConversion();
    testConfigSourceTracking();
    testConfigLoadFromArgs();
    testConfigSaveToFile();
    testConfigTrimAndToLower();
    testConfigFileFormatDetection();
}

} // namespace test_config