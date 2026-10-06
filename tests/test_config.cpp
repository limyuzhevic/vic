// Config Tests
// Enhanced with NLM Test Framework
#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>
#include <string>

// Test macros for NLM Test Framework
#define NLM_TEST_CASE_LEGACY(name, testFunc) \
    void name##TestLegacy() { testFunc; } \
    NLM_TEST_CASE(name, name##TestLegacy)

namespace test_config {

void testConfigCreation() {
    nlm::Config config;
    NLM_ASSERT_FALSE(config.has("test"));
    
    std::cout << "    testConfigCreation passed" << std::endl;
}

void testConfigSetAndGet() {
    nlm::Config config;
    
    config.set("int_value", 42, nlm::ConfigSource::Runtime);
    NLM_ASSERT_TRUE(config.has("int_value"));
    
    auto intVal = config.get<int>("int_value");
    NLM_ASSERT_TRUE(intVal.has_value());
    NLM_ASSERT_EQ(*intVal, 42);
    
    config.set("double_value", 3.14, nlm::ConfigSource::Runtime);
    auto doubleVal = config.get<double>("double_value");
    NLM_ASSERT_TRUE(doubleVal.has_value());
    NLM_ASSERT_TRUE(*doubleVal > 3.13 && *doubleVal < 3.15);
    
    config.set("string_value", std::string("hello"), nlm::ConfigSource::Runtime);
    auto stringVal = config.get<std::string>("string_value");
    NLM_ASSERT_TRUE(stringVal.has_value());
    NLM_ASSERT_EQ(*stringVal, "hello");
    
    config.set("bool_value", true, nlm::ConfigSource::Runtime);
    auto boolVal = config.get<bool>("bool_value");
    NLM_ASSERT_TRUE(boolVal.has_value());
    NLM_ASSERT_EQ(*boolVal, true);
    
    std::cout << "    testConfigSetAndGet passed" << std::endl;
}

void testConfigGetOr() {
    nlm::Config config;
    
    int val = config.getOr<int>("missing", 100);
    NLM_ASSERT_EQ(val, 100);
    
    config.set("existing", 50, nlm::ConfigSource::Runtime);
    val = config.getOr<int>("existing", 100);
    NLM_ASSERT_EQ(val, 50);
    
    std::cout << "    testConfigGetOr passed" << std::endl;
}

void testConfigRemove() {
    nlm::Config config;
    
    config.set("test", 123, nlm::ConfigSource::Runtime);
    NLM_ASSERT_TRUE(config.has("test"));
    
    config.remove("test");
    NLM_ASSERT_FALSE(config.has("test"));
    
    std::cout << "    testConfigRemove passed" << std::endl;
}

void testConfigKeys() {
    nlm::Config config;
    
    config.set("key1", 1, nlm::ConfigSource::Runtime);
    config.set("key2", 2, nlm::ConfigSource::Runtime);
    config.set("key3", 3, nlm::ConfigSource::Runtime);
    
    auto keys = config.getKeys();
    NLM_ASSERT_EQ(keys.size(), 3);
    
    std::cout << "    testConfigKeys passed" << std::endl;
}

// Modern test with enhanced config features
void testConfigAdvancedFeatures() {
    nlm::Config config;
    
    // Test advanced features with modern assertions
    config.set("int", 100, nlm::ConfigSource::Runtime);
    NLM_ASSERT_TRUE(config.has("int"));
    NLM_ASSERT_EQ(config.getOr<int>("int", 0), 100);
    
    config.set("string", "advanced", nlm::ConfigSource::Runtime);
    NLM_ASSERT_TRUE(config.has("string"));
    NLM_ASSERT_EQ(config.getOr<std::string>("string", ""), "advanced");
    
    config.set("double", 3.14159, nlm::ConfigSource::Runtime);
    NLM_ASSERT_TRUE(config.has("double"));
    NLM_ASSERT_TRUE(config.getOr<double>("double", 0.0) > 3.0 && config.getOr<double>("double", 0.0) < 4.0);
    
    config.set("bool", true, nlm::ConfigSource::Runtime);
    NLM_ASSERT_TRUE(config.has("bool"));
    NLM_ASSERT_TRUE(config.getOr<bool>("bool", false));
    
    // Test remove
    config.remove("int");
    NLM_ASSERT_FALSE(config.has("int"));
    
    std::cout << "    testConfigAdvancedFeatures passed" << std::endl;
}

void runAll() {
    std::cout << "Running Enhanced Config tests..." << std::endl;
    
    testConfigCreation();
    testConfigSetAndGet();
    testConfigGetOr();
    testConfigRemove();
    testConfigKeys();
    testConfigAdvancedFeatures();
}

} // namespace test_config
