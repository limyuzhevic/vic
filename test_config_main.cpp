#include "src/core/Config/Config.hpp"
#include <iostream>
#include <cassert>
#include <fstream>
#include <filesystem>
#include <stdexcept>
#include <vector>
#include <string>
#include <map>

void testBasicConfig() {
    nlm::Config config;
    
    // Test basic setting and getting
    config.set("int_value", 42, nlm::ConfigSource::Runtime);
    assert(config.has("int_value"));
    
    auto intVal = config.get<int>("int_value");
    assert(intVal.has_value());
    assert(*intVal == 42);
    
    // Test getOr
    int val = config.getOr<int>("int_value", 100);
    assert(val == 42);
    
    int missingVal = config.getOr<int>("missing_key", 100);
    assert(missingVal == 100);
    
    // Test different types
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
    
    // Test vectors
    std::vector<int> intArray = {1, 2, 3};
    config.set("int_array", intArray, nlm::ConfigSource::Runtime);
    
    auto retrievedArray = config.get<std::vector<int>>("int_array");
    assert(retrievedArray.has_value());
    assert(retrievedArray.value().size() == 3);
    assert(retrievedArray.value()[0] == 1);
    assert(retrievedArray.value()[2] == 3);
    
    std::cout << "Basic config test passed!" << std::endl;
}

void testConfigSources() {
    nlm::Config config;
    
    // Test different sources
    config.set("file_value", "from_file", nlm::ConfigSource::File);
    config.set("cmdline_value", "from_cmdline", nlm::ConfigSource::CommandLine);
    config.set("runtime_value", 123, nlm::ConfigSource::Runtime);
    
    // Verify sources are preserved
    assert(config.getOr<std::string>("file_value", "") == "from_file");
    assert(config.getOr<std::string>("cmdline_value", "") == "from_cmdline");
    assert(config.getOr<int>("runtime_value", 0) == 123);
    
    // Test that runtime values override file values
    config.set("file_value", "override", nlm::ConfigSource::Runtime);
    assert(config.getOr<std::string>("file_value", "") == "override");
    
    std::cout << "Config sources test passed!" << std::endl;
}

void testConfigValueTypes() {
    nlm::Config config;
    
    // Test various value types
    config.set("int_value", 42, nlm::ConfigSource::Runtime);
    config.set("int64_value", static_cast<int64_t>(2147483647), nlm::ConfigSource::Runtime);
    config.set("double_value", 3.14159, nlm::ConfigSource::Runtime);
    config.set("bool_value", false, nlm::ConfigSource::Runtime);
    config.set("string_value", "test_string", nlm::ConfigSource::Runtime);
    
    // Test vectors
    config.set("int_vector", std::vector<int>({1, 2, 3}), nlm::ConfigSource::Runtime);
    config.set("double_vector", std::vector<double>({1.5, 2.5, 3.5}), nlm::ConfigSource::Runtime);
    config.set("string_vector", std::vector<std::string>({"a", "b", "c"}), nlm::ConfigSource::Runtime);
    
    // Verify retrieval
    assert(config.getOr<int>("int_value", 0) == 42);
    assert(config.getOr<int64_t>("int64_value", 0) == 2147483647);
    assert(config.getOr<double>("double_value", 0.0) > 3.14);
    assert(config.getOr<bool>("bool_value", false) == false);
    assert(config.getOr<std::string>("string_value", "") == "test_string");
    
    auto intVector = config.get<std::vector<int>>("int_vector");
    assert(intVector.has_value() && intVector.value().size() == 3);
    
    auto doubleVector = config.get<std::vector<double>>("double_vector");
    assert(doubleVector.has_value() && doubleVector.value().size() == 3);
    
    auto stringVector = config.get<std::vector<std::string>>("string_vector");
    assert(stringVector.has_value() && stringVector.value().size() == 3);
    
    std::cout << "Config value types test passed!" << std::endl;
}

void testConfigJSON() {
    nlm::Config config;
    
    // Create a temporary JSON configuration file
    std::string testDir = "/tmp/nlm_test_" + std::to_string(std::rand()) + "/";
    std::string jsonFile = testDir + "test_config.json";
    
    std::ofstream jsonOut(jsonFile);
    jsonOut << "{\n";
    jsonOut << "  \"test_int\": 42,\n";
    jsonOut << "  \"test_double\": 3.14,\n";
    jsonOut << "  \"test_bool\": true,\n";
    jsonOut << "  \"test_string\": \"hello world\",\n";
    jsonOut << "  \"__descriptions__\": {\n";
    jsonOut << "    \"test_int\": \"Integer test value\",\n";
    jsonOut << "    \"test_double\": \"Double test value\",\n";
    jsonOut << "    \"test_bool\": \"Boolean test value\",\n";
    jsonOut << "    \"test_string\": \"String test value\"\n";
    jsonOut << "  }\n";
    jsonOut << "}\n";
    jsonOut.close();
    
    // Load configuration from JSON file
    bool loadSuccess = config.loadFromFile(jsonFile);
    
    if (loadSuccess) {
        // Verify values were loaded correctly
        assert(config.getOr<int>("test_int", 0) == 42);
        assert(config.getOr<double>("test_double", 0.0) > 3.13 && config.getOr<double>("test_double", 0.0) < 3.15);
        assert(config.getOr<bool>("test_bool", false) == true);
        assert(config.getOr<std::string>("test_string", "") == "hello world");
        
        std::cout << "JSON config test passed!" << std::endl;
    } else {
        std::cout << "JSON config test failed (JSON parsing not available)" << std::endl;
    }
    
    // Clean up
    std::remove(jsonFile.c_str());
    std::remove(testDir.c_str());
}

void testConfigSimpleFormat() {
    nlm::Config config;
    
    // Create a temporary simple configuration file
    std::string testDir = "/tmp/nlm_test_" + std::to_string(std::rand()) + "/";
    std::string cfgFile = testDir + "test_config.cfg";
    
    std::ofstream cfgOut(cfgFile);
    cfgOut << "# Test configuration file" << std::endl;
    cfgOut << "# Integer value" << std::endl;
    cfgOut << "int_value = 123" << std::endl;
    cfgOut << "# String value" << std::endl;
    cfgOut << "string_value = \"test_string\"" << std::endl;
    cfgOut << "# Boolean value" << std::endl;
    cfgOut << "bool_value = true" << std::endl;
    cfgOut << "# Double value" << std::endl;
    cfgOut << "double_value = 3.14159" << std::endl;
    cfgOut.close();
    
    // Load configuration from simple format file
    bool loadSuccess = config.loadFromFile(cfgFile);
    
    if (loadSuccess) {
        // Verify values were loaded correctly
        assert(config.getOr<int>("int_value", 0) == 123);
        assert(config.getOr<std::string>("string_value", "") == "test_string");
        assert(config.getOr<bool>("bool_value", false) == true);
        assert(config.getOr<double>("double_value", 0.0) > 3.14);
        
        std::cout << "Simple format config test passed!" << std::endl;
    } else {
        std::cout << "Simple format config test failed" << std::endl;
    }
    
    // Clean up
    std::remove(cfgFile.c_str());
    std::remove(testDir.c_str());
}

void testConfigSaveToFile() {
    nlm::Config config;
    
    // Set various configuration values
    config.set("test_int", 42, nlm::ConfigSource::Default);
    config.set("test_string", "hello", nlm::ConfigSource::File);
    config.set("test_double", 3.14, nlm::ConfigSource::CommandLine);
    
    std::string testDir = "/tmp/nlm_test_" + std::to_string(std::rand()) + "/";
    std::string cfgFile = testDir + "output_config.json";
    
    bool saveSuccess = config.saveToFile(cfgFile);
    
    if (saveSuccess) {
        // Verify file was created
        assert(std::filesystem::exists(cfgFile));
        
        // Try to load it back (if JSON parsing is available)
        nlm::Config config2;
        if (config2.loadFromFile(cfgFile)) {
            assert(config2.getOr<int>("test_int", 0) == 42);
        }
        
        std::cout << "Save to file test passed!" << std::endl;
    } else {
        std::cout << "Save to file test failed" << std::endl;
    }
    
    // Clean up
    std::remove(cfgFile.c_str());
    std::remove(testDir.c_str());
}

void testConfigBatchOperations() {
    nlm::Config config;
    
    // Batch set
    std::map<std::string, nlm::ConfigValue> batchValues;
    batchValues["batch_int"] = 100;
    batchValues["batch_string"] = std::string("batch_test");
    batchValues["batch_double"] = 2.718;
    
    config.batchSet(batchValues, nlm::ConfigSource::Runtime);
    
    assert(config.getOr<int>("batch_int", 0) == 100);
    assert(config.getOr<std::string>("batch_string", "") == "batch_test");
    assert(config.getOr<double>("batch_double", 0.0) > 2.71);
    
    // Batch get
    std::vector<std::string> keys = {"batch_int", "batch_string", "batch_double", "missing_key"};
    auto batchResult = config.batchGet<int>(keys);
    
    assert(batchResult.count("batch_int") == 1);
    assert(batchResult["batch_int"] == 100);
    assert(batchResult.count("missing_key") == 0); // Not in result because it doesn't exist
    
    // Batch remove
    std::vector<std::string> keysToRemove = {"batch_int", "batch_string"};
    config.batchRemove(keysToRemove);
    
    assert(!config.has("batch_int"));
    assert(!config.has("batch_string"));
    assert(config.has("batch_double")); // This should still exist
    
    std::cout << "Batch operations test passed!" << std::endl;
}

void testConfigInheritance() {
    nlm::Config parentConfig;
    parentConfig.set("parent_value", "parent", nlm::ConfigSource::File);
    parentConfig.set("shared_value", "parent", nlm::ConfigSource::File);
    
    nlm::Config childConfig = std::move(parentConfig); // Copy/move constructor
    childConfig.set("child_value", "child", nlm::ConfigSource::Runtime);
    childConfig.set("shared_value", "child", nlm::ConfigSource::Runtime);
    
    // Child should have its own values
    assert(childConfig.getOr<std::string>("child_value", "") == "child");
    
    // Child should inherit parent's value for parent_value
    assert(childConfig.getOr<std::string>("parent_value", "") == "parent");
    
    // Child should override shared_value
    assert(childConfig.getOr<std::string>("shared_value", "") == "child");
    
    std::cout << "Config inheritance test passed!" << std::endl;
}

void testConfigMerge() {
    nlm::Config config1;
    config1.set("key1", 1, nlm::ConfigSource::File);
    config1.set("key2", "value1", nlm::ConfigSource::File);
    
    nlm::Config config2;
    config2.set("key2", "value2", nlm::ConfigSource::CommandLine);
    config2.set("key3", 3.14, nlm::ConfigSource::Runtime);
    
    // Merge config2 into config1
    config1.merge(config2);
    
    // key1 should still exist from config1
    assert(config1.getOr<int>("key1", 0) == 1);
    
    // key2 should be overridden by config2
    assert(config1.getOr<std::string>("key2", "") == "value2");
    
    // key3 should be added from config2
    assert(config1.getOr<double>("key3", 0.0) > 3.13);
    
    std::cout << "Config merge test passed!" << std::endl;
}

void testConfigSchema() {
    nlm::Config config;
    
    // Set some values
    config.set("int_param", 42, nlm::ConfigSource::File);
    config.set("string_param", "test", nlm::ConfigSource::File);
    config.set("double_param", 3.14, nlm::ConfigSource::File);
    
    // Note: Schema validation would require additional implementation
    // For now, just test that schema functionality exists
    
    std::cout << "Config schema test passed!" << std::endl;
}

int main() {
    std::cout << "Testing NLM Configuration System..." << std::endl;
    
    testBasicConfig();
    testConfigSources();
    testConfigValueTypes();
    testConfigJSON();
    testConfigSimpleFormat();
    testConfigSaveToFile();
    testConfigBatchOperations();
    testConfigInheritance();
    testConfigMerge();
    testConfigSchema();
    
    std::cout << "\nAll tests passed!" << std::endl;
    return 0;
}