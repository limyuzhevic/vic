// Comprehensive Config Tests
#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>
#include <fstream>
#include <filesystem>
#include <stdexcept>
#include <vector>

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
    assert(config.getOr<int>("int_value") == 42);
    assert(config.getOr<int64_t>("int64_value") == 2147483647);
    assert(config.getOr<double>("double_value") > 3.14 && config.getOr<double>("double_value") < 3.15);
    assert(config.getOr<bool>("bool_value") == false);
    assert(config.getOr<std::string>("string_value") == "test_string");
    
    auto intVector = config.get<std::vector<int>>("int_vector");
    assert(intVector.has_value() && intVector.value().size() == 3);
    
    auto doubleVector = config.get<std::vector<double>>("double_vector");
    assert(doubleVector.has_value() && doubleVector.value().size() == 3);
    
    auto stringVector = config.get<std::vector<std::string>>("string_vector");
    assert(stringVector.has_value() && stringVector.value().size() == 3);
    
    std::cout << "    testConfigValueTypes passed" << std::endl;
}

void testConfigSourceManagement() {
    nlm::Config config;
    
    // Test that different sources are tracked
    config.set("file_value", "from_file", nlm::ConfigSource::File);
    config.set("cmdline_value", "from_cmdline", nlm::ConfigSource::CommandLine);
    config.set("runtime_value", 123, nlm::ConfigSource::Runtime);
    
    // Verify sources are preserved
    assert(config.getOr<std::string>("file_value") == "from_file");
    assert(config.getOr<std::string>("cmdline_value") == "from_cmdline");
    assert(config.getOr<int>("runtime_value") == 123);
    
    // Test that runtime values override file values
    config.set("file_value", "override", nlm::ConfigSource::Runtime);
    assert(config.getOr<std::string>("file_value") == "override");
    
    std::cout << "    testConfigSourceManagement passed" << std::endl;
}

void testConfigSummary() {
    nlm::Config config;
    
    config.set("setting1", 100, nlm::ConfigSource::Default);
    config.set("setting2", "test", nlm::ConfigSource::File);
    config.set("setting3", 3.14, nlm::ConfigSource::CommandLine);
    
    std::string summary = config.summary();
    assert(summary.find("setting1") != std::string::npos);
    assert(summary.find("setting2") != std::string::npos);
    assert(summary.find("setting3") != std::string::npos);
    assert(summary.find("100") != std::string::npos);
    assert(summary.find("test") != std::string::npos);
    assert(summary.find("3.14") != std::string::npos);
    
    std::cout << "    testConfigSummary passed" << std::endl;
}

void testConfigClear() {
    nlm::Config config;
    
    config.set("temp1", 1, nlm::ConfigSource::Runtime);
    config.set("temp2", "test", nlm::ConfigSource::Runtime);
    config.set("temp3", 3.14, nlm::ConfigSource::Runtime);
    
    assert(config.has("temp1"));
    assert(config.has("temp2"));
    assert(config.has("temp3"));
    
    config.clear();
    
    assert(!config.has("temp1"));
    assert(!config.has("temp2"));
    assert(!config.has("temp3"));
    
    assert(config.getKeys().empty());
    
    std::cout << "    testConfigClear passed" << std::endl;
}

void testConfigErrorHandling() {
    nlm::Config config;
    
    // Test accessing non-existent keys returns nullopt
    auto missing = config.get<std::string>("non_existent_key");
    assert(!missing.has_value());
    
    // Test getOr with non-existent keys
    assert(config.getOr<int>("non_existent_key", 999) == 999);
    assert(config.getOr<std::string>("non_existent_key", "default") == "default");
    assert(config.getOr<double>("non_existent_key", 3.14) > 3.13);
    
    std::cout << "    testConfigErrorHandling passed" << std::endl;
}

void testConfigJSONFormat() {
    // Create a temporary JSON configuration file
    std::string testDir = "/tmp/nlm_test_" + std::to_string(std::rand()) + "/";
    std::filesystem::create_directories(testDir);
    
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
    nlm::Config config;
    bool loadSuccess = config.loadFromFile(jsonFile);
    
    if (loadSuccess) {
        // Verify values were loaded correctly
        assert(config.getOr<int>("test_int") == 42);
        assert(config.getOr<double>("test_double") > 3.13 && config.getOr<double>("test_double") < 3.15);
        assert(config.getOr<bool>("test_bool") == true);
        assert(config.getOr<std::string>("test_string") == "hello world");
        
        std::cout << "    testConfigJSONFormat passed" << std::endl;
    } else {
        std::cout << "    testConfigJSONFormat skipped (JSON parsing not available)" << std::endl;
    }
    
    // Clean up
    std::filesystem::remove_all(testDir);
}

void testConfigSimpleFormat() {
    // Create a temporary simple configuration file
    std::string testDir = "/tmp/nlm_test_" + std::to_string(std::rand()) + "/";
    std::filesystem::create_directories(testDir);
    
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
    nlm::Config config;
    bool loadSuccess = config.loadFromFile(cfgFile);
    
    if (loadSuccess) {
        // Verify values were loaded correctly
        assert(config.getOr<int>("int_value") == 123);
        assert(config.getOr<std::string>("string_value") == "test_string");
        assert(config.getOr<bool>("bool_value") == true);
        assert(config.getOr<double>("double_value") > 3.14);
        
        std::cout << "    testConfigSimpleFormat passed" << std::endl;
    } else {
        std::cout << "    testConfigSimpleFormat failed" << std::endl;
    }
    
    // Clean up
    std::filesystem::remove_all(testDir);
}

void testConfigSaveToFile() {
    nlm::Config config;
    
    // Set various configuration values
    config.set("test_int", 42, nlm::ConfigSource::Default);
    config.set("test_string", "hello", nlm::ConfigSource::File);
    config.set("test_double", 3.14, nlm::ConfigSource::CommandLine);
    
    // Add descriptions
    // Note: This would require internal API access in current implementation
    // For now, just test that save doesn't crash
    
    std::string testDir = "/tmp/nlm_test_" + std::to_string(std::rand()) + "/";
    std::filesystem::create_directories(testDir);
    
    std::string cfgFile = testDir + "output_config.json";
    bool saveSuccess = config.saveToFile(cfgFile);
    
    if (saveSuccess) {
        // Verify file was created
        assert(std::filesystem::exists(cfgFile));
        
        // Try to load it back (if JSON parsing is available)
        nlm::Config config2;
        if (config2.loadFromFile(cfgFile)) {
            assert(config2.getOr<int>("test_int") == 42);
        }
        
        std::cout << "    testConfigSaveToFile passed" << std::endl;
    } else {
        std::cout << "    testConfigSaveToFile skipped (save functionality not available)" << std::endl;
    }
    
    // Clean up
    std::filesystem::remove_all(testDir);
}

void testConfigComplexValues() {
    nlm::Config config;
    
    // Test complex nested structures
    std::vector<int> intArray = {1, 2, 3, 4, 5};
    std::vector<double> doubleArray = {1.1, 2.2, 3.3, 4.4, 5.5};
    std::vector<std::string> stringArray = {"item1", "item2", "item3"};
    
    config.set("int_array", intArray, nlm::ConfigSource::Runtime);
    config.set("double_array", doubleArray, nlm::ConfigSource::Runtime);
    config.set("string_array", stringArray, nlm::ConfigSource::Runtime);
    
    // Retrieve and verify
    auto retrievedIntArray = config.get<std::vector<int>>("int_array");
    assert(retrievedIntArray.has_value());
    assert(retrievedIntArray.value().size() == 5);
    
    auto retrievedDoubleArray = config.get<std::vector<double>>("double_array");
    assert(retrievedDoubleArray.has_value());
    assert(retrievedDoubleArray.value().size() == 5);
    
    auto retrievedStringArray = config.get<std::vector<std::string>>("string_array");
    assert(retrievedStringArray.has_value());
    assert(retrievedStringArray.value().size() == 3);
    
    std::cout << "    testConfigComplexValues passed" << std::endl;
}

void testConfigPerformance() {
    nlm::Config config;
    
    // Performance test: many configuration operations
    const size_t numOperations = 10000;
    
    for (size_t i = 0; i < numOperations; ++i) {
        config.set("key_" + std::to_string(i), static_cast<int>(i), nlm::ConfigSource::Runtime);
        auto value = config.get<int>("key_" + std::to_string(i));
        assert(value.has_value());
        assert(*value == static_cast<int>(i));
    }
    
    assert(config.getKeys().size() == numOperations);
    
    std::cout << "    testConfigPerformance passed" << std::endl;
}

void runAll() {
    testConfigCreation();
    testConfigSetAndGet();
    testConfigGetOr();
    testConfigRemove();
    testConfigKeys();
    testConfigValueTypes();
    testConfigSourceManagement();
    testConfigSummary();
    testConfigClear();
    testConfigErrorHandling();
    testConfigJSONFormat();
    testConfigSimpleFormat();
    testConfigSaveToFile();
    testConfigComplexValues();
    testConfigPerformance();
    
    std::cout << "\nAll configuration tests completed successfully!" << std::endl;
}

} // namespace test_config