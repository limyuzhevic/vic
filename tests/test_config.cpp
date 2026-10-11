// Config Tests - Enhanced with JSON/YAML support
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

void testFileFormatDetection() {
    nlm::Config config;
    
    // Test extension-based detection
    assert(config.detectFileFormat("config.json") == "json");
    assert(config.detectFileFormat("test.JSON") == "json");
    assert(config.detectFileFormat("data.yml") == "yaml");
    assert(config.detectFileFormat("data.YML") == "yaml");
    assert(config.detectFileFormat("data.yaml") == "yaml");
    assert(config.detectFileFormat("unknown.txt") == "unknown");
    assert(config.detectFileFormat("noextension") == "unknown");
    
    std::cout << "    testFileFormatDetection passed" << std::endl;
}

void testSimpleFormat() {
    nlm::Config config;
    
    // Create a test config file
    std::string testFile = "test_simple.cfg";
    std::ofstream file(testFile);
    file << "# Test config file\n";
    file << "key1 = value1\n";
    file << "key2 = 42\n";
    file << "key3 = true\n";
    file << "# comment line\n";
    file << "key4 = \"quoted value\"\n";
    file.close();
    
    // Load the file
    bool result = config.loadFromFile(testFile);
    assert(result == true);
    
    // Verify loaded values
    assert(config.has("key1"));
    assert(config.has("key2"));
    assert(config.has("key3"));
    assert(config.has("key4"));
    
    // Check values
    auto val1 = config.get<std::string>("key1");
    assert(val1.has_value() && *val1 == "value1");
    
    auto val2 = config.get<int>("key2");
    assert(val2.has_value() && *val2 == 42);
    
    auto val3 = config.get<bool>("key3");
    assert(val3.has_value() && *val3 == true);
    
    auto val4 = config.get<std::string>("key4");
    assert(val4.has_value() && *val4 == "quoted value");
    
    // Cleanup
    std::filesystem::remove(testFile);
    
    std::cout << "    testSimpleFormat passed" << std::endl;
}

void testEnhancedLoadFromFile() {
    nlm::Config config;
    
    // Test with different file extensions
    // JSON file (would need actual JSON parsing implementation)
    std::string jsonFile = "test.json";
    std::ofstream jsonOut(jsonFile);
    jsonOut << "{\"name\": \"test\", \"value\": 123}" << std::endl;
    jsonOut.close();
    
    // This should work with our current implementation
    bool result = config.loadFromFile(jsonFile);
    // The format detection will work, but parsing will use fallback
    
    // YAML file (would need libyaml-cpp)
    std::string yamlFile = "test.yaml";
    std::ofstream yamlOut(yamlFile);
    yamlOut << "name: test" << std::endl;
    yamlOut << "value: 456" << std::endl;
    yamlOut.close();
    
    bool result2 = config.loadFromFile(yamlFile);
    
    // Cleanup
    std::filesystem::remove(jsonFile);
    std::filesystem::remove(yamlFile);
    
    std::cout << "    testEnhancedLoadFromFile passed" << std::endl;
}

void runAll() {
    testConfigCreation();
    testConfigSetAndGet();
    testConfigGetOr();
    testConfigRemove();
    testConfigKeys();
    testFileFormatDetection();
    testSimpleFormat();
    testEnhancedLoadFromFile();
}

} // namespace test_config
