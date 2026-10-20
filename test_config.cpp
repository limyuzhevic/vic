#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>
#include <fstream>
#include <filesystem>
#include <vector>

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

void testFileFormatDetection() {
    assert(nlm::Config::detectFileFormat("config.cfg") == nlm::ConfigFormat::SimpleKeyValue);
    assert(nlm::Config::detectFileFormat("config.config") == nlm::ConfigFormat::SimpleKeyValue);
    assert(nlm::Config::detectFileFormat("config.txt") == nlm::ConfigFormat::SimpleKeyValue);
    assert(nlm::Config::detectFileFormat("config.json") == nlm::ConfigFormat::JSON);
    assert(nlm::Config::detectFileFormat("config.yaml") == nlm::ConfigFormat::YAML);
    assert(nlm::Config::detectFileFormat("config.yml") == nlm::ConfigFormat::YAML);
    assert(nlm::Config::detectFileFormat("config.unknown") == nlm::ConfigFormat::Unknown);
    
    std::cout << "    testFileFormatDetection passed" << std::endl;
}

void testSimpleKeyValueFormat() {
    nlm::Config config;
    
    config.set("int_value", 42);
    config.set("double_value", 3.14);
    config.set("string_value", std::string("hello"));
    config.set("bool_value", true);
    
    // Test saving
    config.saveToFile("test_config.cfg");
    
    // Test loading
    nlm::Config config2;
    assert(config2.loadFromFile("test_config.cfg"));
    
    assert(config2.get<int>("int_value").has_value());
    assert(config2.get<double>("double_value").has_value());
    assert(config2.get<std::string>("string_value").has_value());
    assert(config2.get<bool>("bool_value").has_value());
    
    std::cout << "    testSimpleKeyValueFormat passed" << std::endl;
}

void testJSONFormat() {
    nlm::Config config;
    
    config.set("int_value", 42);
    config.set("double_value", 3.14);
    config.set("string_value", std::string("hello"));
    config.set("bool_value", true);
    config.set("nested.number", 123);
    config.set("nested.text", std::string("world"));
    
    // Test saving as JSON
    config.saveToFile("test_config.json");
    
    // Test loading as JSON
    nlm::Config config2;
    assert(config2.loadFromFile("test_config.json"));
    
    assert(config2.get<int>("int_value").has_value());
    assert(config2.get<double>("double_value").has_value());
    assert(config2.get<std::string>("string_value").has_value());
    assert(config2.get<bool>("bool_value").has_value());
    
    // Test nested access
    assert(config2.get<int>("nested.number").has_value());
    assert(config2.get<std::string>("nested.text").has_value());
    
    std::cout << "    testJSONFormat passed" << std::endl;
}

void testYAMLFormat() {
    nlm::Config config;
    
    config.set("int_value", 42);
    config.set("double_value", 3.14);
    config.set("string_value", std::string("hello"));
    config.set("bool_value", true);
    
    // Test saving as YAML
    config.saveToFile("test_config.yaml");
    
    // Test loading as YAML
    nlm::Config config2;
    assert(config2.loadFromFile("test_config.yaml"));
    
    assert(config2.get<int>("int_value").has_value());
    assert(config2.get<double>("double_value").has_value());
    assert(config2.get<std::string>("string_value").has_value());
    assert(config2.get<bool>("bool_value").has_value());
    
    std::cout << "    testYAMLFormat passed" << std::endl;
}

void testNestedStructures() {
    nlm::Config config;
    
    // Test nested key setting
    config.set("brain.neuron_count", 1000);
    config.set("brain.synaptic_weight", 0.01);
    config.set("development.growth_rate", 0.1);
    config.set("neuromodulation.dopamine_level", 0.5);
    
    // Verify nested keys are accessible
    assert(config.has("brain.neuron_count"));
    assert(config.has("brain.synaptic_weight"));
    assert(config.has("development.growth_rate"));
    assert(config.has("neuromodulation.dopamine_level"));
    
    std::cout << "    testNestedStructures passed" << std::endl;
}

void testDataTypes() {
    nlm::Config config;
    
    // Test different data types
    config.set("integer", 42);
    config.set("long", static_cast<int64_t>(9223372036854775807));
    config.set("double", 3.141592653589793);
    config.set("boolean", true);
    config.set("string", std::string("test_string"));
    
    config.set("int_array", std::vector<int>({1, 2, 3, 4, 5}));
    config.set("double_array", std::vector<double>({1.1, 2.2, 3.3}));
    config.set("string_array", std::vector<std::string>({"a", "b", "c"}));
    
    // Verify retrieval
    assert(config.get<int>("integer").has_value());
    assert(config.get<int64_t>("long").has_value());
    assert(config.get<double>("double").has_value());
    assert(config.get<bool>("boolean").has_value());
    assert(config.get<std::string>("string").has_value());
    
    std::cout << "    testDataTypes passed" << std::endl;
}

void testErrorHandling() {
    nlm::Config config;
    
    // Test loading malformed JSON
    std::ofstream malformed_json("malformed.json");
    malformed_json << "{ invalid json }";
    malformed_json.close();
    
    nlm::Config config1;
    assert(!config1.loadFromFile("malformed.json"));
    
    // Test loading malformed YAML
    std::ofstream malformed_yaml("malformed.yaml");
    malformed_yaml << "invalid: [";
    malformed_yaml.close();
    
    nlm::Config config2;
    assert(!config2.loadFromFile("malformed.yaml"));
    
    std::cout << "    testErrorHandling passed" << std::endl;
}

int main() {
    std::cout << "Running Config tests..." << std::endl;
    
    testConfigCreation();
    testConfigSetAndGet();
    testConfigGetOr();
    testConfigRemove();
    testConfigKeys();
    testFileFormatDetection();
    testSimpleKeyValueFormat();
    testJSONFormat();
    testYAMLFormat();
    testNestedStructures();
    testDataTypes();
    testErrorHandling();
    
    std::cout << "\nAll tests passed!" << std::endl;
    return 0;
}
