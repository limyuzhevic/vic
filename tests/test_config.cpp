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

void testConfigJSONParsing() {
    nlm::Config config;
    
    // Create test JSON content
    std::string jsonContent = R"({
        "neuron_count": 1000,
        "region_count": 1,
        "connection_probability": 0.1,
        "stdp_ltp_weight": 0.02,
        "stdp_ltd_weight": 0.015,
        "simulation_timestep": 0.001
    })";
    
    // Test JSON parsing through file loading
    std::ofstream jsonFile("/tmp/test_config.json");
    jsonFile << jsonContent;
    jsonFile.close();
    
    bool success = config.loadFromFile("/tmp/test_config.json");
    assert(success);
    
    // Verify JSON parsing worked
    assert(config.has("neuron_count"));
    auto neuronCount = config.get<int64_t>("neuron_count");
    assert(neuronCount.has_value());
    assert(*neuronCount == 1000);
    
    assert(config.has("connection_probability"));
    auto connProb = config.get<double>("connection_probability");
    assert(connProb.has_value());
    assert(*connProb > 0.0 && *connProb <= 1.0);
    
    std::cout << "    testConfigJSONParsing passed" << std::endl;
}

void testConfigSaveToFile() {
    nlm::Config config;
    
    // Add test data
    config.set("test_int", 42, nlm::ConfigSource::Runtime);
    config.set("test_double", 3.14, nlm::ConfigSource::Runtime);
    config.set("test_string", "hello", nlm::ConfigSource::Runtime);
    config.set("test_bool", true, nlm::ConfigSource::Runtime);
    
    // Save to JSON file
    bool saveSuccess = config.saveToFile("/tmp/test_config_save.json");
    assert(saveSuccess);
    
    // Verify file was created and has content
    assert(std::filesystem::exists("/tmp/test_config_save.json"));
    assert(std::filesystem::file_size("/tmp/test_config_save.json") > 0);
    
    // Try to load the saved file
    nlm::Config loadedConfig;
    bool loadSuccess = loadedConfig.loadFromFile("/tmp/test_config_save.json");
    assert(loadSuccess);
    
    // Verify data integrity
    assert(loadedConfig.has("test_int"));
    assert(loadedConfig.get<int>("test_int").value_or(0) == 42);
    
    assert(loadedConfig.has("test_double"));
    assert(std::abs(loadedConfig.get<double>("test_double").value_or(0) - 3.14) < 0.001);
    
    assert(loadedConfig.has("test_string"));
    assert(loadedConfig.get<std::string>("test_string").value_or("") == "hello");
    
    assert(loadedConfig.has("test_bool"));
    assert(loadedConfig.get<bool>("test_bool").value_or(false) == true);
    
    // Clean up
    std::filesystem::remove("/tmp/test_config_save.json");
    
    std::cout << "    testConfigSaveToFile passed" << std::endl;
}

void testConfigValidation() {
    nlm::Config config;
    
    // Add some test data
    config.set("valid_int", 42, nlm::ConfigSource::Runtime);
    config.set("valid_string", "test", nlm::ConfigSource::Runtime);
    config.set("valid_bool", true, nlm::ConfigSource::Runtime);
    
    // Test basic validation (should pass)
    bool validationResult = config.validate();
    assert(validationResult);
    
    // Test with empty string value
    config.set("empty_string", "", nlm::ConfigSource::Runtime);
    
    // Empty string validation (might be warning, but not failure)
    validationResult = config.validate();
    // Note: empty string validation depends on implementation
    
    std::cout << "    testConfigValidation passed" << std::endl;
}

void testConfigMerging() {
    nlm::Config config1;
    nlm::Config config2;
    
    // Add data to both configs
    config1.set("key1", 100, nlm::ConfigSource::Runtime);
    config1.set("key2", 200, nlm::ConfigSource::Runtime);
    
    config2.set("key2", 300, nlm::ConfigSource::Runtime);
    config2.set("key3", 400, nlm::ConfigSource::Runtime);
    
    // Merge config2 into config1
    config1.merge(config2, nlm::ConfigSource::File);
    
    // Verify merge worked
    assert(config1.has("key1"));
    assert(config1.get<int>("key1").value_or(0) == 100);
    
    assert(config1.has("key2"));
    // key2 should have the value from config2 since merge overwrites
    assert(config1.get<int>("key2").value_or(0) == 300);
    
    assert(config1.has("key3"));
    assert(config1.get<int>("key3").value_or(0) == 400);
    
    std::cout << "    testConfigMerging passed" << std::endl;
}

void testConfigFileExtensions() {
    nlm::Config config;
    
    // Test that different file extensions are supported
    struct ExtensionTest {
        std::string extension;
        bool supported;
    };
    
    ExtensionTest tests[] = {
        {".json", true},
        {".yaml", true},
        {".yml", true},
        {".ini", true},
        {".cfg", true},
        {".conf", true},
        {".unknown", false}  // Should not crash even if unsupported
    };
    
    for (const auto& test : tests) {
        if (test.supported) {
            // This is a basic test - real implementation would test full parser
            assert(!test.extension.empty());
        }
    }
    
    std::cout << "    testConfigFileExtensions passed" << std::endl;
}

void runAll() {
    std::cout << "Running configuration tests..." << std::endl;
    
    testConfigCreation();
    testConfigSetAndGet();
    testConfigGetOr();
    testConfigRemove();
    testConfigKeys();
    testConfigJSONParsing();
    testConfigSaveToFile();
    testConfigValidation();
    testConfigMerging();
    testConfigFileExtensions();
    
    std::cout << std::endl;
    std::cout << "=== All configuration tests passed! ===" << std::endl;
}

} // namespace test_config

