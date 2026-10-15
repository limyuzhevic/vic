// Config Tests
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

void testConfigJsonSaveAndLoad() {
    nlm::Config config;
    
    // Set some test values including different types
    config.set("int_value", 42, nlm::ConfigSource::Runtime);
    config.set("double_value", 3.14159, nlm::ConfigSource::Runtime);
    config.set("string_value", "test_string", nlm::ConfigSource::Runtime);
    config.set("bool_value", true, nlm::ConfigSource::Runtime);
    config.set("vector_int", std::vector<int>({1, 2, 3}), nlm::ConfigSource::Runtime);
    
    // Create a temporary file for testing
    std::string tempFile = "test_config.json";
    
    // Save configuration to JSON file
    bool saveSuccess = config.saveToFile(tempFile);
    assert(saveSuccess);
    
    // Verify file was created
    std::ifstream file(tempFile);
    assert(file.good());
    file.close();
    
    // Clear and reload from file
    config.clear();
    bool loadSuccess = config.loadFromFile(tempFile);
    assert(loadSuccess);
    
    // Verify values were loaded correctly
    assert(config.has("int_value"));
    assert(config.has("double_value"));
    assert(config.has("string_value"));
    assert(config.has("bool_value"));
    assert(config.has("vector_int"));
    
    std::cout << "    testConfigJsonSaveAndLoad passed" << std::endl;
    
    // Clean up
    std::remove(tempFile.c_str());
}

void testConfigBackwardCompatibility() {
    nlm::Config config;
    
    // Test simple key=value format (old format)
    std::string tempFile = "test_config_old_format.cfg";
    
    // Create old format file
    std::ofstream oldFormatFile(tempFile);
    oldFormatFile << "# Test configuration file\n";
    oldFormatFile << "int_val = 100\n";
    oldFormatFile << "double_val = 2.71828\n";
    oldFormatFile << "string_val = \"hello\"\n";
    oldFormatFile << "bool_val = true\n";
    oldFormatFile << "# Another comment line\n";
    oldFormatFile.close();
    
    // Load old format file
    bool loadSuccess = config.loadFromFile(tempFile);
    assert(loadSuccess);
    
    // Verify values were loaded
    assert(config.has("int_val"));
    assert(config.has("double_val"));
    assert(config.has("string_val"));
    assert(config.has("bool_val"));
    
    std::cout << "    testConfigBackwardCompatibility passed" << std::endl;
    
    // Clean up
    std::remove(tempFile.c_str());
}

void testConfigCommandLine() {
    nlm::Config config;
    
    // Simulate command line arguments
    int argc = 5;
    const char* argv[] = {
        "test_program",
        "--config_key1=value1",
        "--config_key2", "value2",
        "-config_key3", "value3",
        "-config_key4=value4"
    };
    
    bool loadSuccess = config.loadFromArgs(argc, const_cast<char**>(argv));
    assert(loadSuccess);
    
    // Verify command line arguments were loaded
    assert(config.has("config_key1"));
    assert(config.has("config_key2"));
    assert(config.has("config_key3"));
    assert(config.has("config_key4"));
    
    std::cout << "    testConfigCommandLine passed" << std::endl;
}

void runAll() {
    testConfigCreation();
    testConfigSetAndGet();
    testConfigGetOr();
    testConfigRemove();
    testConfigKeys();
    testConfigJsonSaveAndLoad();
    testConfigBackwardCompatibility();
    testConfigCommandLine();
}

} // namespace test_config
