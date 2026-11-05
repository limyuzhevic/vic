// Config Tests
#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>
#include <filesystem>
#include <fstream>

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

void testConfigToJSON() {
    nlm::Config config;
    config.set("int_value", 42, nlm::ConfigSource::Runtime);
    config.set("double_value", 3.14, nlm::ConfigSource::Runtime);
    config.set("string_value", std::string("hello"), nlm::ConfigSource::Runtime);
    
    std::string json = config.toJSON();
    assert(!json.empty());
    
    // Parse back and verify
    nlm::Config config2;
    assert(config2.fromJSON(json));
    assert(config2.has("int_value"));
    assert(config2.has("double_value"));
    assert(config2.has("string_value"));
    
    std::cout << "    testConfigToJSON passed" << std::endl;
}

void testConfigToYAML() {
    nlm::Config config;
    config.set("int_value", 42, nlm::ConfigSource::Runtime);
    config.set("double_value", 3.14, nlm::ConfigSource::Runtime);
    config.set("string_value", std::string("hello"), nlm::ConfigSource::Runtime);
    
    std::string yaml = config.toYAML();
    assert(!yaml.empty());
    
    // Parse back and verify
    nlm::Config config2;
    assert(config2.fromYAML(yaml));
    assert(config2.has("int_value"));
    assert(config2.has("double_value"));
    assert(config2.has("string_value"));
    
    std::cout << "    testConfigToYAML passed" << std::endl;
}

void testConfigFromJSON() {
    std::string json = R"({"int_value": 42, "double_value": 3.14, "string_value": "hello"})";
    
    nlm::Config config;
    assert(config.fromJSON(json));
    assert(config.has("int_value"));
    assert(config.has("double_value"));
    assert(config.has("string_value"));
    
    std::cout << "    testConfigFromJSON passed" << std::endl;
}

void testConfigFromYAML() {
    std::string yaml = """int_value: 42
double_value: 3.14
string_value: hello""";
    
    nlm::Config config;
    assert(config.fromYAML(yaml));
    assert(config.has("int_value"));
    assert(config.has("double_value"));
    assert(config.has("string_value"));
    
    std::cout << "    testConfigFromYAML passed" << std::endl;
}

void testConfigValidation() {
    nlm::Config config;
    
    // Test empty config validation
    std::vector<std::string> errors = config.validate();
    assert(errors.empty()); // Empty config should be valid
    
    // Add some config values
    config.set("brain.neuron_count", 1000, nlm::ConfigSource::Runtime);
    config.set("brain.synapse_density", 0.1, nlm::ConfigSource::Runtime);
    
    // Test validation with some values
    errors = config.validate();
    assert(errors.empty()); // Config with values should still be valid
    
    std::cout << "    testConfigValidation passed" << std::endl;
}

void testConfigIsValid() {
    nlm::Config config;
    assert(config.isValid()); // Empty config should be valid
    
    config.set("brain.neuron_count", 100, nlm::ConfigSource::Runtime);
    assert(config.isValid()); // Config with values should be valid
    
    std::cout << "    testConfigIsValid passed" << std::endl;
}

void testConfigGetMissingKeys() {
    nlm::Config config;
    std::vector<std::string> required = {"brain.neuron_count", "brain.synapse_density", "brain.connection_probability"};
    
    // Test missing keys detection
    std::vector<std::string> missing = config.getMissingKeys(required);
    assert(missing.size() == 3); // All keys missing
    assert(std::find(missing.begin(), missing.end(), "brain.neuron_count") != missing.end());
    assert(std::find(missing.begin(), missing.end(), "brain.synapse_density") != missing.end());
    assert(std::find(missing.begin(), missing.end(), "brain.connection_probability") != missing.end());
    
    std::cout << "    testConfigGetMissingKeys passed" << std::endl;
}

void testConfigStats() {
    nlm::Config config;
    std::string stats = config.getStats();
    assert(!stats.empty());
    assert(stats.find("Configuration") != std::string::npos);
    
    // Add some values
    config.set("brain.neuron_count", 1000, nlm::ConfigSource::Runtime);
    config.set("brain.synapse_density", 0.1, nlm::ConfigSource::Runtime);
    
    stats = config.getStats();
    assert(stats.find("1 entries") != std::string::npos);
    
    std::cout << "    testConfigStats passed" << std::endl;
}

void testConfigHelp() {
    std::string help = nlm::Config::getHelp();
    assert(!help.empty());
    assert(help.find("Configuration") != std::string::npos);
    
    std::cout << "    testConfigHelp passed" << std::endl;
}

void testConfigAllKeys() {
    std::vector<std::string> keys = nlm::Config::getAllKeys();
    // This should return a list of all possible configuration keys
    assert(!keys.empty());
    
    std::cout << "    testConfigAllKeys passed" << std::endl;
}

void testConfigExample() {
    std::string example = nlm::Config::getExample();
    assert(!example.empty());
    
    std::cout << "    testConfigExample passed" << std::endl;
}

void runAll() {
    testConfigCreation();
    testConfigSetAndGet();
    testConfigGetOr();
    testConfigRemove();
    testConfigKeys();
    testConfigToJSON();
    testConfigToYAML();
    testConfigFromJSON();
    testConfigFromYAML();
    testConfigValidation();
    testConfigIsValid();
    testConfigGetMissingKeys();
    testConfigStats();
    testConfigHelp();
    testConfigAllKeys();
    testConfigExample();
}

} // namespace test_config
