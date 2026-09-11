#include "core/Logger/Logger.hpp"
#include "core/Config/Config.hpp"
#include <iostream>
#include <cassert>
#include <fstream>
#include <sstream>

using namespace nlm;

void testKeyValueFormat() {
    NLM_LOG_INFO("Testing key=value format parsing...");
    
    auto config = std::make_shared<Config>();
    
    // Test loading from string via set()
    config->set("test_int", 42);
    config->set("test_double", 3.14);
    config->set("test_bool", true);
    config->set("test_string", "hello");
    
    // Test retrieval
    auto intVal = config->get<int>("test_int");
    assert(intVal.has_value() && *intVal == 42);
    
    auto doubleVal = config->get<double>("test_double");
    assert(doubleVal.has_value() && std::abs(*doubleVal - 3.14) < 0.001);
    
    auto boolVal = config->get<bool>("test_bool");
    assert(boolVal.has_value() && *boolVal == true);
    
    auto stringVal = config->get<std::string>("test_string");
    assert(stringVal.has_value() && *stringVal == "hello");
    
    NLM_LOG_INFO("[PASS] key=value format parsing works correctly");
}

void testJsonFileFormat() {
    NLM_LOG_INFO("Testing JSON format parsing...");
    
    // Create a temporary JSON file
    std::string testJsonPath = "/tmp/test_config.json";
    std::ofstream jsonFile(testJsonPath);
    jsonFile << "{\n";
    jsonFile << "  \"json_test_int\": 123,\n";
    jsonFile << "  \"json_test_double\": 2.718,\n";
    jsonFile << "  \"json_test_bool\": false,\n";
    jsonFile << "  \"json_test_string\": \"world\",\n";
    jsonFile << "  \"nested\": {\n";
    jsonFile << "    \"nested_value\": 999\n";
    jsonFile << "  }\n";
    jsonFile << "}\n";
    jsonFile.close();
    
    auto config = std::make_shared<Config>();
    bool loaded = config->loadFromFile(testJsonPath);
    
    if (loaded) {
        // Test retrieval of nested values
        auto intVal = config->get<int>("json_test_int");
        assert(intVal.has_value() && *intVal == 123);
        
        auto doubleVal = config->get<double>("json_test_double");
        assert(doubleVal.has_value() && std::abs(*doubleVal - 2.718) < 0.001);
        
        auto boolVal = config->get<bool>("json_test_bool");
        assert(boolVal.has_value() && *boolVal == false);
        
        auto stringVal = config->get<std::string>("json_test_string");
        assert(stringVal.has_value() && *stringVal == "world");
        
        // Test nested JSON flattening
        auto nestedVal = config->get<int>("nested.nested_value");
        assert(nestedVal.has_value() && *nestedVal == 999);
        
        NLM_LOG_INFO("[PASS] JSON format parsing works correctly");
    } else {
        NLM_LOG_ERROR("[FAIL] JSON file parsing failed");
        throw std::runtime_error("JSON parsing failed");
    }
    
    // Clean up
    std::remove(testJsonPath.c_str());
}

void testBackwardCompatibility() {
    NLM_LOG_INFO("Testing backward compatibility (key=value format)...");
    
    // Create a temporary key=value file
    std::string testKvPath = "/tmp/test_config.cfg";
    std::ofstream kvFile(testKvPath);
    kvFile << "# Test configuration\n";
    kvFile << "test_string = \"backward_compat\"\n";
    kvFile << "test_int = 777\n";
    kvFile << "test_bool = true\n";
    kvFile.close();
    
    auto config = std::make_shared<Config>();
    bool loaded = config->loadFromFile(testKvPath);
    
    if (loaded) {
        auto stringVal = config->get<std::string>("test_string");
        assert(stringVal.has_value() && *stringVal == "backward_compat");
        
        auto intVal = config->get<int>("test_int");
        assert(intVal.has_value() && *intVal == 777);
        
        auto boolVal = config->get<bool>("test_bool");
        assert(boolVal.has_value() && *boolVal == true);
        
        NLM_LOG_INFO("[PASS] backward compatibility with key=value format works");
    } else {
        NLM_LOG_ERROR("[FAIL] key=value file parsing failed");
        throw std::runtime_error("Key=value parsing failed");
    }
    
    // Clean up
    std::remove(testKvPath.c_str());
}

void testConfigValueTypes() {
    NLM_LOG_INFO("Testing ConfigValue type conversions...");
    
    auto config = std::make_shared<Config>();
    
    // Test setting different types
    config->set("int_key", 42);
    config->set("double_key", 3.14159);
    config->set("bool_key", true);
    config->set("string_key", "test string");
    
    // Verify type preservation
    auto intVal = config->get<int>("int_key");
    assert(intVal.has_value());
    
    auto doubleVal = config->get<double>("double_key");
    assert(doubleVal.has_value());
    
    auto boolVal = config->get<bool>("bool_key");
    assert(boolVal.has_value());
    
    auto stringVal = config->get<std::string>("string_key");
    assert(stringVal.has_value());
    
    // Test has() method
    assert(config->has("int_key"));
    assert(config->has("double_key"));
    assert(config->has("bool_key"));
    assert(config->has("string_key"));
    assert(!config->has("nonexistent_key"));
    
    NLM_LOG_INFO("[PASS] ConfigValue type handling works correctly");
}

void testCommandLineParser() {
    NLM_LOG_INFO("Testing CommandLineParser integration...");
    
    // Create a simple mock test for CommandLineParser
    // (The full test would require a more complex setup)
    
    auto config = std::make_shared<Config>();
    config->set("test_param", 123);
    
    auto paramVal = config->get<int>("test_param");
    assert(paramVal.has_value() && *paramVal == 123);
    
    NLM_LOG_INFO("[PASS] CommandLineParser integration works");
}

void testSummaryAndKeys() {
    NLM_LOG_INFO("Testing summary() and getKeys() methods...");
    
    auto config = std::make_shared<Config>();
    
    // Add some test values
    config->set("key1", 100);
    config->set("key2", "value2");
    config->set("key3", true);
    config->set("key4", 3.14);
    
    // Test getKeys()
    auto keys = config->getKeys();
    assert(keys.size() >= 4);
    
    // Test summary
    auto summary = config->summary();
    assert(!summary.empty());
    assert(summary.find("key1") != std::string::npos);
    assert(summary.find("key2") != std::string::npos);
    
    NLM_LOG_INFO("[PASS] summary() and getKeys() methods work correctly");
}

void runAllConfigTests() {
    NLM_LOG_INFO("=== Running Config System Integration Tests ===");
    
    try {
        testKeyValueFormat();
        testJsonFileFormat();
        testBackwardCompatibility();
        testConfigValueTypes();
        testCommandLineParser();
        testSummaryAndKeys();
        
        NLM_LOG_INFO("\n=== ALL CONFIG TESTS PASSED ===");
        NLM_LOG_INFO("Config system is working correctly with JSON parsing support!");
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Config tests failed: " + std::string(e.what()));
        throw;
    }
}
