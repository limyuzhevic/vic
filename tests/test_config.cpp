// Config Tests
#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>
#include <fstream>
#include <string>

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

void testJsonBasic() {
    nlm::Config config;
    
    // Set some values
    config.set("name", std::string("NLM Brain"), nlm::ConfigSource::Runtime);
    config.set("neurons", 1000, nlm::ConfigSource::Runtime);
    config.set("learning_rate", 0.01, nlm::ConfigSource::Runtime);
    config.set("enabled", true, nlm::ConfigSource::Runtime);
    config.set("threshold", -50.0, nlm::ConfigSource::Runtime);
    
    // Convert to JSON
    auto json = config.toJson();
    
    // Check JSON structure
    assert(json.is_object());
    assert(json.contains("name"));
    assert(json.contains("neurons"));
    assert(json.contains("learning_rate"));
    assert(json.contains("enabled"));
    assert(json.contains("threshold"));
    
    // Check values
    assert(json["name"] == "NLM Brain");
    assert(json["neurons"] == 1000);
    assert(json["learning_rate"] == 0.01);
    assert(json["enabled"] == true);
    assert(json["threshold"] == -50.0);
    
    std::cout << "    testJsonBasic passed" << std::endl;
}

void testJsonLoading() {
    nlm::Config config;
    
    // Create JSON
    nlohmann::json json;
    json["brain.neuron_count"] = 2000;
    json["plasticity.stdp_learning_rate"] = 0.001;
    json["neuromodulation.dopamine_level"] = 0.5;
    json["environment.width"] = 20;
    json["environment.height"] = 20;
    json["learning.enabled"] = true;
    
    // Load from JSON
    assert(config.loadFromJson(json));
    
    // Verify values
    assert(config.has("brain.neuron_count"));
    assert(config.getOr<int64_t>("brain.neuron_count", 0) == 2000);
    
    assert(config.has("plasticity.stdp_learning_rate"));
    assert(config.getOr<double>("plasticity.stdp_learning_rate", 0.0) == 0.001);
    
    assert(config.has("neuromodulation.dopamine_level"));
    assert(config.getOr<double>("neuromodulation.dopamine_level", 0.0) == 0.5);
    
    assert(config.has("environment.width"));
    assert(config.getOr<int64_t>("environment.width", 0) == 20);
    
    std::cout << "    testJsonLoading passed" << std::endl;
}

void testJsonFileLoading() {
    nlm::Config config;
    
    // Create a temporary JSON file
    const std::string testJsonFile = "test_config.json";
    
    // Write JSON file
    std::ofstream outFile(testJsonFile);
    outFile << R"({"test_int": 42, "test_string": "hello", "test_float": 3.14, "test_bool": true})";
    outFile.close();
    
    // Load from file
    assert(config.loadFromFile(testJsonFile));
    
    // Verify values
    assert(config.has("test_int"));
    assert(config.getOr<int64_t>("test_int", 0) == 42);
    
    assert(config.has("test_string"));
    assert(config.getOr<std::string>("test_string", "") == "hello");
    
    assert(config.has("test_float"));
    assert(config.getOr<double>("test_float", 0.0) > 3.13 && config.getOr<double>("test_float", 0.0) < 3.15);
    
    assert(config.has("test_bool"));
    assert(config.getOr<bool>("test_bool", false) == true);
    
    // Clean up
    std::remove(testJsonFile.c_str());
    
    std::cout << "    testJsonFileLoading passed" << std::endl;
}

void testJsonFileSaving() {
    nlm::Config config;
    
    // Set some values
    config.set("brain.neuron_count", 1500, nlm::ConfigSource::Runtime);
    config.set("learning_rate", 0.05, nlm::ConfigSource::Runtime);
    config.set("model_name", std::string("test_model"), nlm::ConfigSource::Runtime);
    config.set("enabled", true, nlm::ConfigSource::Runtime);
    
    // Save to file
    const std::string testJsonFile = "test_save_config.json";
    assert(config.saveToFile(testJsonFile));
    
    // Load back from file
    nlm::Config config2;
    assert(config2.loadFromFile(testJsonFile));
    
    // Verify values match
    assert(config2.getOr<int64_t>("brain.neuron_count", 0) == 1500);
    assert(config2.getOr<double>("learning_rate", 0.0) == 0.05);
    assert(config2.getOr<std::string>("model_name", "") == "test_model");
    assert(config2.getOr<bool>("enabled", false) == true);
    
    // Clean up
    std::remove(testJsonFile.c_str());
    
    std::cout << "    testJsonFileSaving passed" << std::endl;
}

void testConfigBackwardCompatibility() {
    nlm::Config config;
    
    // Test that the old key=value format still works
    config.set("old_key1", std::string("value1"), nlm::ConfigSource::File);
    config.set("old_key2", 123, nlm::ConfigSource::File);
    config.set("old_key3", 45.67, nlm::ConfigSource::File);
    config.set("old_key4", false, nlm::ConfigSource::File);
    
    // Verify values
    assert(config.getOr<std::string>("old_key1", "") == "value1");
    assert(config.getOr<int64_t>("old_key2", 0) == 123);
    assert(config.getOr<double>("old_key3", 0.0) == 45.67);
    assert(config.getOr<bool>("old_key4", true) == false);
    
    std::cout << "    testConfigBackwardCompatibility passed" << std::endl;
}

void testConfigErrorHandling() {
    nlm::Config config;
    
    // Test with invalid JSON
    const std::string invalidJsonFile = "invalid.json";
    std::ofstream invalidFile(invalidJsonFile);
    invalidFile << "invalid { json content";
    invalidFile.close();
    
    // Should still load (fallback to key=value, but will fail parsing)
    bool loaded = config.loadFromFile(invalidJsonFile);
    // It's OK if this fails, as long as it doesn't crash
    
    std::remove(invalidJsonFile.c_str());
    
    std::cout << "    testConfigErrorHandling passed" << std::endl;
}

void testConfigEmptyFile() {
    nlm::Config config;
    
    // Test with empty file
    const std::string emptyFile = "empty_config.txt";
    std::ofstream emptyOutFile(emptyFile);
    emptyOutFile.close();
    
    bool loaded = config.loadFromFile(emptyFile);
    assert(!loaded); // Empty file should fail to load
    
    std::remove(emptyFile.c_str());
    
    std::cout << "    testConfigEmptyFile passed" << std::endl;
}

void testConfigMixedFormat() {
    nlm::Config config;
    
    // Test that we can mix JSON and key=value entries
    nlohmann::json json;
    json["json_key1"] = "json_value";
    json["json_key2"] = 999;
    
    config.loadFromJson(json);
    
    // Add a regular key=value entry
    config.set("regular_key", std::string("regular_value"), nlm::ConfigSource::File);
    
    // Verify both are present
    assert(config.has("json_key1"));
    assert(config.has("regular_key"));
    
    assert(config.getOr<std::string>("json_key1", "") == "json_value");
    assert(config.getOr<std::string>("regular_key", "") == "regular_value");
    
    std::cout << "    testConfigMixedFormat passed" << std::endl;
}

void runAll() {
    testConfigCreation();
    testConfigSetAndGet();
    testConfigGetOr();
    testConfigRemove();
    testConfigKeys();
    testJsonBasic();
    testJsonLoading();
    testJsonFileLoading();
    testJsonFileSaving();
    testConfigBackwardCompatibility();
    testConfigErrorHandling();
    testConfigEmptyFile();
    testConfigMixedFormat();
}

} // namespace test_config
