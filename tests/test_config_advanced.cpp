#include <iostream>
#include <cassert>
#include <cmath>
#include <vector>
#include <algorithm>
#include <filesystem>
#include "core/Config/Config.hpp"

namespace test_config_advanced {

void testFileIOJSON() {
    std::cout << "Testing file I/O (JSON)..." << std::endl;
    
    nlm::Config config;
    
    config.set("int_value", 42, nlm::ConfigSource::Runtime);
    config.set("double_value", 3.14, nlm::ConfigSource::Runtime);
    config.set("string_value", std::string("hello world"), nlm::ConfigSource::Runtime);
    config.set("bool_value", true, nlm::ConfigSource::Runtime);
    
    config.set("int_array", std::vector<int>{1, 2, 3}, nlm::ConfigSource::Runtime);
    config.set("double_array", std::vector<double>{1.1, 2.2, 3.3}, nlm::ConfigSource::Runtime);
    config.set("string_array", std::vector<std::string>{"a", "b", "c"}, nlm::ConfigSource::Runtime);
    
    const std::string json_file = "/tmp/test_config.json";
    bool saved = config.saveToFile(json_file);
    assert(saved);
    
    nlm::Config config2;
    bool loaded = config2.loadFromFile(json_file);
    assert(loaded);
    
    auto int_val = config2.get<int>("int_value");
    assert(int_val.has_value() && *int_val == 42);
    
    auto double_val = config2.get<double>("double_value");
    assert(double_val.has_value() && fabs(*double_val - 3.14) < 0.01);
    
    auto string_val = config2.get<std::string>("string_value");
    assert(string_val.has_value() && *string_val == "hello world");
    
    auto bool_val = config2.get<bool>("bool_value");
    assert(bool_val.has_value() && *bool_val == true);
    
    auto int_array = config2.get<std::vector<int>>("int_array");
    assert(int_array.has_value() && int_array->size() == 3);
    
    auto double_array = config2.get<std::vector<double>>("double_array");
    assert(double_array.has_value() && double_array->size() == 3);
    
    auto string_array = config2.get<std::vector<std::string>>("string_array");
    assert(string_array.has_value() && string_array->size() == 3);
    
    std::filesystem::remove(json_file);
    
    std::cout << "  PASSED" << std::endl;
}

void testFileIOYAML() {
    std::cout << "Testing file I/O (YAML)..." << std::endl;
    
    nlm::Config config;
    
    config.set("name", std::string("test_agent"), nlm::ConfigSource::Runtime);
    config.set("version", 1.0, nlm::ConfigSource::Runtime);
    config.set("enabled", true, nlm::ConfigSource::Runtime);
    config.set("parameters", std::vector<double>{0.1, 0.2, 0.3}, nlm::ConfigSource::Runtime);
    
    const std::string yaml_file = "/tmp/test_config.yaml";
    bool saved = config.saveToFile(yaml_file);
    assert(saved);
    
    nlm::Config config2;
    bool loaded = config2.loadFromFile(yaml_file);
    assert(loaded);
    
    auto name_val = config2.get<std::string>("name");
    assert(name_val.has_value() && *name_val == "test_agent");
    
    auto version_val = config2.get<double>("version");
    assert(version_val.has_value() && fabs(*version_val - 1.0) < 0.001);
    
    auto enabled_val = config2.get<bool>("enabled");
    assert(enabled_val.has_value() && *enabled_val == true);
    
    auto parameters_val = config2.get<std::vector<double>>("parameters");
    assert(parameters_val.has_value() && parameters_val->size() == 3);
    
    std::filesystem::remove(yaml_file);
    
    std::cout << "  PASSED" << std::endl;
}

void testErrorHandlingMalformedFiles() {
    std::cout << "Testing error handling for malformed files..." << std::endl;
    
    nlm::Config config;
    
    const std::string malformed_json = "/tmp/malformed.json";
    std::ofstream file(malformed_json);
    file << "{" << std::endl;
    file << "  "invalid_key": 123" << std::endl;
    file << "}" << std::endl;
    file.close();
    
    nlm::Config config2;
    bool loaded = config2.loadFromFile(malformed_json);
    assert(loaded);
    
    std::filesystem::remove(malformed_json);
    
    const std::string malformed_ini = "/tmp/malformed.ini";
    std::ofstream file2(malformed_ini);
    file2 << "[section]" << std::endl;
    file2 << "key = value = extra" << std::endl;
    file2.close();
    
    nlm::Config config3;
    loaded = config3.loadFromFile(malformed_ini);
    assert(loaded);
    
    std::filesystem::remove(malformed_ini);
    
    const std::string non_existent = "/tmp/does_not_exist.txt";
    nlm::Config config4;
    loaded = config4.loadFromFile(non_existent);
    assert(!loaded);
    
    std::cout << "  PASSED" << std::endl;
}

void testConfigurationValidation() {
    std::cout << "Testing configuration validation..." << std::endl;
    
    nlm::Config config;
    
    config.set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config.set("region_count", static_cast<int64_t>(5), nlm::ConfigSource::Default);
    config.set("connection_probability", 0.1, nlm::ConfigSource::Default);
    config.set("learning_rate", 0.01, nlm::ConfigSource::Default);
    config.set("max_memory", 1000, nlm::ConfigSource::Default);
    
    assert(config.has("neuron_count"));
    assert(config.has("region_count"));
    assert(config.has("connection_probability"));
    assert(config.has("learning_rate"));
    assert(config.has("max_memory"));
    
    auto neuron_count = config.get<int64_t>("neuron_count");
    assert(neuron_count.has_value() && *neuron_count == 100);
    
    auto region_count = config.get<int64_t>("region_count");
    assert(region_count.has_value() && *region_count == 5);
    
    auto connection_prob = config.get<double>("connection_probability");
    assert(connection_prob.has_value() && fabs(*connection_prob - 0.1) < 0.001);
    
    auto learning_rate = config.get<double>("learning_rate");
    assert(learning_rate.has_value() && fabs(*learning_rate - 0.01) < 0.0001);
    
    auto max_memory = config.get<int64_t>("max_memory");
    assert(max_memory.has_value() && *max_memory == 1000);
    
    config.remove("neuron_count");
    assert(!config.has("neuron_count"));
    
    int64_t missing = config.getOr<int64_t>("neuron_count", 0);
    assert(missing == 0);
    
    std::string keys_summary = config.summary();
    assert(!keys_summary.empty());
    
    std::cout << "  PASSED" << std::endl;
}

void testConfigOperations() {
    std::cout << "Testing config operations..." << std::endl;
    
    nlm::Config config;
    
    config.set("key1", 1, nlm::ConfigSource::Runtime);
    config.set("key2", 2, nlm::ConfigSource::Runtime);
    config.set("key3", 3, nlm::ConfigSource::Runtime);
    
    auto keys = config.getKeys();
    assert(keys.size() == 3);
    
    std::vector<std::string> expected_keys = {"key1", "key2", "key3"};
    for (const auto& key : expected_keys) {
        assert(std::find(keys.begin(), keys.end(), key) != keys.end());
    }
    
    config.set("key2", 20, nlm::ConfigSource::Runtime);
    auto val = config.get<int64_t>("key2");
    assert(val.has_value() && *val == 20);
    
    config.clear();
    assert(config.getKeys().size() == 0);
    
    config.set("test", std::string("value"), nlm::ConfigSource::Runtime);
    config.remove("test");
    assert(!config.has("test"));
    
    config.set("preserve", 123, nlm::ConfigSource::Default);
    config.set("overwrite", 456, nlm::ConfigSource::Runtime);
    assert(config.has("preserve"));
    assert(config.has("overwrite"));
    
    config.clear();
    
    std::cout << "  PASSED" << std::endl;
}

void runAll() {
    std::cout << "=== Advanced Config Tests ===" << std::endl;
    
    testFileIOJSON();
    testFileIOYAML();
    testErrorHandlingMalformedFiles();
    testConfigurationValidation();
    testConfigOperations();
    
    std::cout << "=== All Advanced Config Tests PASSED ===" << std::endl;
}

} // namespace test_config_advanced
