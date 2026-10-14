#include "core/Config/Config.hpp"
#include <iostream>
#include <fstream>
#include <cassert>

void testBasicFunctionality() {
    std::cout << "Testing basic configuration functionality..." << std::endl;
    
    nlm::Config config;
    
    // Test setting and getting values
    config.set("int_value", 42, nlm::ConfigSource::Runtime);
    config.set("double_value", 3.14, nlm::ConfigSource::Runtime);
    config.set("string_value", std::string("hello"), nlm::ConfigSource::Runtime);
    config.set("bool_value", true, nlm::ConfigSource::Runtime);
    
    auto intVal = config.get<int>("int_value");
    assert(intVal.has_value() && *intVal == 42);
    
    auto doubleVal = config.get<double>("double_value");
    assert(doubleVal.has_value() && *doubleVal > 3.13 && *doubleVal < 3.15);
    
    auto stringVal = config.get<std::string>("string_value");
    assert(stringVal.has_value() && *stringVal == "hello");
    
    auto boolVal = config.get<bool>("bool_value");
    assert(boolVal.has_value() && *boolVal == true);
    
    // Test getOr function
    int missingVal = config.getOr<int>("missing", 100);
    assert(missingVal == 100);
    
    int existingVal = config.getOr<int>("int_value", 100);
    assert(existingVal == 42);
    
    // Test has function
    assert(config.has("int_value"));
    assert(!config.has("nonexistent"));
    
    // Test remove function
    config.remove("string_value");
    assert(!config.has("string_value"));
    
    // Test getKeys function
    auto keys = config.getKeys();
    assert(keys.size() == 3);
    assert(std::find(keys.begin(), keys.end(), "int_value") != keys.end());
    assert(std::find(keys.begin(), keys.end(), "double_value") != keys.end());
    assert(std::find(keys.begin(), keys.end(), "bool_value") != keys.end());
    
    // Test summary function
    std::string summary = config.summary();
    assert(!summary.empty());
    
    std::cout << "  ✓ Basic functionality tests passed" << std::endl;
}

void testTypeConversion() {
    std::cout << "Testing type conversion utilities..." << std::endl;
    
    // Test stringToConfigValue
    auto intVal = nlm::Config::stringToConfigValue("42", "int");
    assert(intVal.has_value());
    
    auto doubleVal = nlm::Config::stringToConfigValue("3.14", "double");
    assert(doubleVal.has_value());
    
    auto boolVal = nlm::Config::stringToConfigValue("true", "bool");
    assert(boolVal.has_value());
    
    auto stringVal = nlm::Config::stringToConfigValue("hello", "string");
    assert(stringVal.has_value());
    
    // Test configValueToString
    std::string intStr = nlm::Config::configValueToString(*intVal);
    assert(intStr == "42");
    
    std::string doubleStr = nlm::Config::configValueToString(*doubleVal);
    assert(doubleStr.find("3.14") != std::string::npos);
    
    // Test validateType
    bool intValid = nlm::Config::validateType(*intVal, "int");
    assert(intValid);
    
    bool stringValid = nlm::Config::validateType(*stringVal, "string");
    assert(stringValid);
    
    std::cout << "  ✓ Type conversion tests passed" << std::endl;
}

void testErrorHandling() {
    std::cout << "Testing error handling..." << std::endl;
    
    nlm::Config config;
    
    // Test error handling functions
    std::vector<nlm::ConfigError> errors = config.getValidationErrors();
    assert(errors.empty());
    
    std::string lastError = config.getLastError();
    assert(lastError.empty());
    
    std::cout << "  ✓ Error handling tests passed" << std::endl;
}

void testFileOperations() {
    std::cout << "Testing file operations..." << std::endl;
    
    nlm::Config config;
    
    // Test basic file operations
    config.set("test_key", "test_value", nlm::ConfigSource::File);
    assert(config.has("test_key"));
    
    // Test saveToFile (should create a JSON file)
    bool saveResult = config.saveToFile("/tmp/test_config.json", "json");
    assert(saveResult);
    
    // Test loadFromFile
    bool loadResult = config.loadFromFile("/tmp/test_config.json", false);
    assert(loadResult);
    
    std::cout << "  ✓ File operation tests passed" << std::endl;
}

void testMergeStrategies() {
    std::cout << "Testing merge strategies..." << std::endl;
    
    nlm::Config config1;
    config1.set("key1", 1, nlm::ConfigSource::File);
    config1.set("key2", 2, nlm::ConfigSource::CommandLine);
    
    nlm::Config config2;
    config2.set("key2", 99, nlm::ConfigSource::Runtime);
    config2.set("key3", 3, nlm::ConfigSource::Runtime);
    
    // Test mergeWith
    Config merged = config1.mergeWith(config2, nlm::ConfigSource::Runtime);
    
    assert(merged.get<int>("key1").has_value() && merged.get<int>("key1").value() == 1);
    assert(merged.get<int>("key2").has_value() && merged.get<int>("key2").value() == 99); // config2 overrides config1
    assert(merged.get<int>("key3").has_value() && merged.get<int>("key3").value() == 3);
    
    // Test isDefault
    assert(!merged.isDefault("key1"));
    assert(!merged.isDefault("key2"));
    assert(!merged.isDefault("key3"));
    
    std::cout << "  ✓ Merge strategy tests passed" << std::endl;
}

int main() {
    std::cout << "Running NLM Configuration System Tests" << std::endl;
    std::cout << "=====================================\n" << std::endl;
    
    try {
        testBasicFunctionality();
        testTypeConversion();
        testErrorHandling();
        testFileOperations();
        testMergeStrategies();
        
        std::cout << std::endl;
        std::cout << "All tests passed! ✓" << std::endl;
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Test failed with exception: " << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cerr << "Test failed with unknown exception" << std::endl;
        return 1;
    }
}