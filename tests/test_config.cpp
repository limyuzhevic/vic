// Config Tests
#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>
#include <fstream>
#include <sstream>

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

void testConfigFormatDetection() {
    nlm::Config config;
    
    // Test format detection
    assert(config.detectFormat("config.ini") == nlm::ConfigFormat::INI);
    assert(config.detectFormat("config.json") == nlm::ConfigFormat::JSON);
    assert(config.detectFormat("config.yaml") == nlm::ConfigFormat::YAML);
    assert(config.detectFormat("config.yml") == nlm::ConfigFormat::YAML);
    assert(config.detectFormat("config.cfg") == nlm::ConfigFormat::INI);
    assert(config.detectFormat("config.conf") == nlm::ConfigFormat::INI);
    assert(config.detectFormat("unknown.txt") == nlm::ConfigFormat::Unknown);
    assert(config.detectFormat("no_extension") == nlm::ConfigFormat::Unknown);
    
    std::cout << "    testConfigFormatDetection passed" << std::endl;
}

void testConfigINIFileFormat() {
    nlm::Config config;
    
    // Create a temporary INI file
    std::ofstream file("test_config.ini");
    file << "[section1]\n";
    file << "int_key = 42\n";
    file << "double_key = 3.14\n";
    file << "string_key = \"hello\"\n";
    file << "bool_key = true\n";
    file << "\n";
    file << "[section2]\n";
    file << "list_key = [1, 2, 3]\n";
    file.close();
    
    // Load the file
    ConfigResult result = config.loadFromFile("test_config.ini");
    assert(result.success);
    
    // Verify values
    assert(config.has("section1.int_key"));
    auto val = config.get<int>("section1.int_key");
    assert(val.has_value() && *val == 42);
    
    assert(config.has("section1.double_key"));
    val = config.get<double>("section1.double_key");
    assert(val.has_value() && *val > 3.13 && *val < 3.15);
    
    assert(config.has("section1.string_key"));
    auto strVal = config.get<std::string>("section1.string_key");
    assert(strVal.has_value() && *strVal == "hello");
    
    assert(config.has("section1.bool_key"));
    auto boolVal = config.get<bool>("section1.bool_key");
    assert(boolVal.has_value() && *boolVal == true);
    
    assert(config.has("section2.list_key"));
    auto listVal = config.get<std::vector<double>>("section2.list_key");
    assert(listVal.has_value() && listVal->size() == 3);
    
    std::cout << "    testConfigINIFileFormat passed" << std::endl;
}

void testConfigJSONFileFormat() {
#ifdef HAS_NLOHMANN_JSON
    nlm::Config config;
    
    // Create a temporary JSON file
    std::ofstream file("test_config.json");
    file << "{\n";
    file << "  \"section1\": {\n";
    file << "    \"int_key\": 42,\n";
    file << "    \"double_key\": 3.14,\n";
    file << "    \"string_key\": \"hello\",\n";
    file << "    \"bool_key\": true,\n";
    file << "    \"array_key\": [1.0, 2.0, 3.0]\n";
    file << "  },\n";
    file << "  \"section2\": {\n";
    file << "    \"nested\": {\n";
    file << "      \"value\": \"nested_value\"\n";
    file << "    }\n";
    file << "}\n";
    file.close();
    
    // Load the file
    ConfigResult result = config.loadFromFileWithFormat("test_config.json", nlm::ConfigFormat::JSON);
    assert(result.success);
    
    // Verify values
    assert(config.has("section1.int_key"));
    auto val = config.get<int>("section1.int_key");
    assert(val.has_value() && *val == 42);
    
    assert(config.has("section1.string_key"));
    auto strVal = config.get<std::string>("section1.string_key");
    assert(strVal.has_value() && *strVal == "hello");
    
    assert(config.has("section1.array_key"));
    auto arrayVal = config.get<std::vector<double>>("section1.array_key");
    assert(arrayVal.has_value() && arrayVal->size() == 3);
    
    std::cout << "    testConfigJSONFileFormat passed" << std::endl;
#endif
}

void testConfigResult() {
    nlm::Config config;
    
    // Test successful operations
    ConfigResult success = ConfigResult::Success();
    assert(success.success);
    assert(success.error == nlm::ConfigError::None);
    assert(success.errorMessage.empty());
    
    // Test error operations
    ConfigResult error = ConfigResult::Error(nlm::ConfigError::ParseError, "Test error message");
    assert(!error.success);
    assert(error.error == nlm::ConfigError::ParseError);
    assert(error.errorMessage == "Test error message");
    
    std::cout << "    testConfigResult passed" << std::endl;
}

void testConfigErrorHandling() {
    nlm::Config config;
    
    // Test loading non-existent file
    ConfigResult result = config.loadFromFile("non_existent_file.txt");
    assert(!result.success);
    assert(result.error == nlm::ConfigError::FileNotFound);
    assert(!result.errorMessage.empty());
    
    std::cout << "    testConfigErrorHandling passed" << std::endl;
}

void testConfigSaveFile() {
    nlm::Config config;
    
    // Add some test data
    config.set("test_int", 42, nlm::ConfigSource::Runtime);
    config.set("test_string", "hello", nlm::ConfigSource::Runtime);
    config.set("test_bool", true, nlm::ConfigSource::Runtime);
    
    // Save to file
    ConfigResult result = config.saveToFile("test_output.ini");
    assert(result.success);
    
    // Clear and reload
    config.clear();
    result = config.loadFromFile("test_output.ini");
    assert(result.success);
    
    // Verify data
    assert(config.has("test_int"));
    assert(config.get<int>("test_int").has_value() && *config.get<int>("test_int") == 42);
    
    std::cout << "    testConfigSaveFile passed" << std::endl;
}

void runAll() {
    testConfigCreation();
    testConfigSetAndGet();
    testConfigGetOr();
    testConfigRemove();
    testConfigKeys();
    testConfigFormatDetection();
    testConfigINIFileFormat();
    testConfigJSONFileFormat();
    testConfigResult();
    testConfigErrorHandling();
    testConfigSaveFile();
}

} // namespace test_config
