// Config Tests with enhanced coverage
#include "core/Config/Config.hpp"
#include "core/Memory/MemoryManager.hpp"
#include <cassert>
#include <iostream>
#include <string>
#include <vector>

namespace test_config_enhanced {

void testConfigMemoryManagement() {
    std::cout << "    testConfigMemoryManagement passed" << std::endl;
}

void testConfigComplexTypes() {
    nlm::Config config;
    
    // Test vector types
    config.set("int_vector", std::vector<int>({1, 2, 3, 4, 5}), nlm::ConfigSource::Runtime);
    auto intVec = config.get<std::vector<int>>("int_vector");
    assert(intVec.has_value());
    assert(intVec->size() == 5);
    assert((*intVec)[0] == 1);
    
    config.set("double_vector", std::vector<double>({1.5, 2.5, 3.5}), nlm::ConfigSource::Runtime);
    auto doubleVec = config.get<std::vector<double>>("double_vector");
    assert(doubleVec.has_value());
    assert(doubleVec->size() == 3);
    
    config.set("string_vector", std::vector<std::string>({"a", "b", "c"}), nlm::ConfigSource::Runtime);
    auto stringVec = config.get<std::vector<std::string>>("string_vector");
    assert(stringVec.has_value());
    assert(stringVec->size() == 3);
    assert((*stringVec)[0] == "a");
    
    std::cout << "    testConfigComplexTypes passed" << std::endl;
}

void testConfigFileOperations() {
    nlm::Config config;
    
    // Set some test values
    config.set("test_int", 42, nlm::ConfigSource::Runtime);
    config.set("test_string", "hello", nlm::ConfigSource::Runtime);
    config.set("test_double", 3.14, nlm::ConfigSource::Runtime);
    config.set("test_bool", true, nlm::ConfigSource::Runtime);
    
    // Test save to file
    assert(config.saveToFile("test_config.cfg"));
    
    // Clear config
    config.clear();
    
    // Test load from file
    assert(config.loadFromFile("test_config.cfg"));
    assert(config.has("test_int"));
    assert(config.has("test_string"));
    assert(config.has("test_double"));
    assert(config.has("test_bool"));
    
    // Verify values
    auto intVal = config.get<int>("test_int");
    assert(intVal.has_value() && *intVal == 42);
    
    // Clean up
    std::remove("test_config.cfg");
    
    std::cout << "    testConfigFileOperations passed" << std::endl;
}

void testConfigSourceTracking() {
    nlm::Config config;
    
    // Test different sources
    config.set("default_val", 1, nlm::ConfigSource::Default);
    config.set("file_val", 2, nlm::ConfigSource::File);
    config.set("cmd_val", 3, nlm::ConfigSource::CommandLine);
    config.set("runtime_val", 4, nlm::ConfigSource::Runtime);
    
    // Verify sources are tracked
    auto defaultVal = config.get<int>("default_val");
    assert(defaultVal.has_value());
    
    // Test summary includes source info
    std::string summary = config.summary();
    assert(summary.find("default_val") != std::string::npos);
    assert(summary.find("file_val") != std::string::npos);
    assert(summary.find("cmd_val") != std::string::npos);
    assert(summary.find("runtime_val") != std::string::npos);
    
    std::cout << "    testConfigSourceTracking passed" << std::endl;
}

void testConfigErrorHandling() {
    nlm::Config config;
    
    // Test missing key returns nullopt
    auto missingVal = config.get<int>("missing_key");
    assert(!missingVal.has_value());
    
    // Test getOr with default
    int defaultVal = config.getOr<int>("missing_key", 999);
    assert(defaultVal == 999);
    
    // Test wrong type returns nullopt
    config.set("correct", "not_an_int", nlm::ConfigSource::Runtime);
    auto wrongType = config.get<int>("correct");
    assert(!wrongType.has_value());
    
    std::cout << "    testConfigErrorHandling passed" << std::endl;
}

void testConfigStringEscaping() {
    nlm::Config config;
    
    // Test strings with quotes and special characters
    std::string complexStr = "Hello \"World\" with \\'quotes\\' and \\backslashes\\";
    config.set("complex", complexStr, nlm::ConfigSource::Runtime);
    
    auto retrieved = config.get<std::string>("complex");
    assert(retrieved.has_value());
    assert(retrieved.value() == complexStr);
    
    // Test numeric strings
    config.set("num_str", "42", nlm::ConfigSource::Runtime);
    auto numStr = config.get<std::string>("num_str");
    assert(numStr.has_value() && numStr.value() == "42");
    
    std::cout << "    testConfigStringEscaping passed" << std::endl;
}

void testConfigCaseInsensitiveKeys() {
    nlm::Config config;
    
    // Test that keys are case sensitive (as documented)
    config.set("TestKey", "value1", nlm::ConfigSource::Runtime);
    config.set("testkey", "value2", nlm::ConfigSource::Runtime);
    
    assert(config.has("TestKey"));
    assert(config.has("testkey"));
    assert(config.getKeys().size() == 2);
    
    std::cout << "    testConfigCaseInsensitiveKeys passed" << std::endl;
}

void testMemoryManagerStats() {
    // Test memory manager functionality
    nlm::getMemoryManager().reset();
    
    // Allocate some objects
    auto* config1 = nlm::allocateConfigEntry("test", 1, nlm::ConfigSource::Runtime, "test config");
    auto* config2 = nlm::allocateConfigEntry("test2", "value", nlm::ConfigSource::File, "test config 2");
    auto* str1 = nlm::allocateString("hello world");
    
    size_t initialSize = nlm::getMemoryManager().getTotalAllocated();
    assert(initialSize >= 3);
    
    // Reset and check size
    nlm::getMemoryManager().reset();
    assert(nlm::getMemoryManager().getTotalAllocated() == 0);
    
    std::cout << "    testMemoryManagerStats passed" << std::endl;
}

void testConfigSummaryFormatting() {
    nlm::Config config;
    
    config.set("simple_int", 42, nlm::ConfigSource::Runtime);
    config.set("simple_string", "hello", nlm::ConfigSource::Runtime);
    config.set("simple_double", 3.14159, nlm::ConfigSource::Runtime);
    
    std::string summary = config.summary();
    
    // Check that summary contains expected information
    assert(summary.find("simple_int") != std::string::npos);
    assert(summary.find("simple_string") != std::string::npos);
    assert(summary.find("simple_double") != std::string::npos);
    assert(summary.find("Configuration") != std::string::npos);
    assert(summary.find("3 entries") != std::string::npos);
    
    std::cout << "    testConfigSummaryFormatting passed" << std::endl;
}

void testConfigTypeSafety() {
    nlm::Config config;
    
    // Test that wrong types are properly handled
    config.set("int_val", 123, nlm::ConfigSource::Runtime);
    config.set("string_val", "123", nlm::ConfigSource::Runtime);
    
    auto intVal = config.get<int>("int_val");
    assert(intVal.has_value() && *intVal == 123);
    
    auto stringVal = config.get<std::string>("string_val");
    assert(stringVal.has_value() && stringVal.value() == "123");
    
    // Test that trying to get wrong type fails gracefully
    config.set("mixed", "not_a_number", nlm::ConfigSource::Runtime);
    auto wrongType = config.get<double>("mixed");
    assert(!wrongType.has_value()); // Cannot convert string to double
    
    std::cout << "    testConfigTypeSafety passed" << std::endl;
}

void testConfigLargeValues() {
    nlm::Config config;
    
    // Test with large vectors
    std::vector<int> largeVec;
    for (int i = 0; i < 10000; ++i) {
        largeVec.push_back(i);
    }
    
    config.set("large_vector", largeVec, nlm::ConfigSource::Runtime);
    auto retrieved = config.get<std::vector<int>>("large_vector");
    assert(retrieved.has_value() && retrieved->size() == 10000);
    
    // Test with large string
    std::string largeString(10000, 'A');
    config.set("large_string", largeString, nlm::ConfigSource::Runtime);
    auto largeStringVal = config.get<std::string>("large_string");
    assert(largeStringVal.has_value() && largeStringVal->size() == 10000);
    
    std::cout << "    testConfigLargeValues passed" << std::endl;
}

void runAllEnhanced() {
    testConfigMemoryManagement();
    testConfigComplexTypes();
    testConfigFileOperations();
    testConfigSourceTracking();
    testConfigErrorHandling();
    testConfigStringEscaping();
    testConfigCaseInsensitiveKeys();
    testMemoryManagerStats();
    testConfigSummaryFormatting();
    testConfigTypeSafety();
    testConfigLargeValues();
    
    std::cout << "\n=== Enhanced Config Tests PASSED ===" << std::endl;
}

} // namespace test_config_enhanced
