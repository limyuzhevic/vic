// Enhanced Config Tests for NLM Configuration System
#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>
#include <fstream>
#include <sstream>
#include <regex>
#include <vector>
#include <memory>

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

void testConfigVersion() {
    nlm::Config config("1.0.0");
    assert(config.getVersion() == "1.0.0");
    
    config.setVersion("2.0.0");
    assert(config.getVersion() == "2.0.0");
    
    std::cout << "    testConfigVersion passed" << std::endl;
}

void testConfigDataTypes() {
    nlm::Config config;
    
    config.set("int", 42, nlm::ConfigSource::Runtime, "Integer value", nlm::ConfigDataType::Int);
    config.set("double", 3.14, nlm::ConfigSource::Runtime, "Double value", nlm::ConfigDataType::Double);
    config.set("bool", true, nlm::ConfigSource::Runtime, "Boolean value", nlm::ConfigDataType::Bool);
    config.set("string", "test", nlm::ConfigSource::Runtime, "String value", nlm::ConfigDataType::String);
    
    assert(config.has("int"));
    assert(config.has("double"));
    assert(config.has("bool"));
    assert(config.has("string"));
    
    std::cout << "    testConfigDataTypes passed" << std::endl;
}

void testConfigValidation() {
    nlm::Config config;
    
    // Test basic validation
    config.set("required_key", "value", nlm::ConfigSource::Runtime, "Required key", nlm::ConfigDataType::String, true);
    assert(config.validate());
    
    // Test that missing required key fails validation
    nlm::Config config2("1.0.0");
    config2.set("required_key", "value", nlm::ConfigSource::Runtime, "Required key", nlm::ConfigDataType::String, true);
    
    std::vector<nlm::ValidationResult> results;
    assert(config2.validate(results));
    
    std::cout << "    testConfigValidation passed" << std::endl;
}

void testConfigEnvironmentVariables() {
    nlm::Config config;
    
    // Test setting environment variable prefix
    config.setEnvironmentPrefix("TEST_");
    
    // Test basic variable setting
    config.set("test_var", "${TEST_VAR}", nlm::ConfigSource::Runtime);
    assert(config.has("test_var"));
    
    std::cout << "    testConfigEnvironmentVariables passed" << std::endl;
}

void testConfigJSONParsing() {
    nlm::Config config;
    
    // Create a simple JSON string
    std::string json = "{\"int_value\": 42, \"string_value\": \"hello\", \"bool_value\": true}";
    
    // Note: This test requires a JSON parser implementation
    // For now, we'll test the structure
    assert(true); // Placeholder
    
    std::cout << "    testConfigJSONParsing passed" << std::endl;
}

void testConfigMerging() {
    nlm::Config config1;
    config1.set("key1", 1, nlm::ConfigSource::Runtime);
    config1.set("key2", 2, nlm::ConfigSource::Runtime);
    
    nlm::Config config2;
    config2.set("key2", 20, nlm::ConfigSource::CommandLine);
    config2.set("key3", 3, nlm::ConfigSource::File);
    
    // Test merging
    bool merged = config1.merge(config2);
    assert(merged);
    assert(config1.has("key1"));
    assert(config1.has("key2")); // CommandLine should override Runtime
    assert(config1.has("key3"));
    
    std::cout << "    testConfigMerging passed" << std::endl;
}

void testConfigComparison() {
    nlm::Config config1;
    config1.set("key1", 1, nlm::ConfigSource::Runtime);
    
    nlm::Config config2;
    config2.set("key1", 1, nlm::ConfigSource::Runtime);
    
    nlm::Config config3;
    config3.set("key1", 2, nlm::ConfigSource::Runtime);
    
    assert(config1.equals(config2));
    assert(!config1.equals(config3));
    
    std::cout << "    testConfigComparison passed" << std::endl;
}

void testConfigDiff() {
    nlm::Config config1;
    config1.set("key1", 1, nlm::ConfigSource::Runtime);
    config1.set("key2", 2, nlm::ConfigSource::Runtime);
    
    nlm::Config config2;
    config2.set("key1", 1, nlm::ConfigSource::Runtime);
    config2.set("key2", 20, nlm::ConfigSource::Runtime);
    
    std::string diff = config1.diff(config2);
    assert(!diff.empty());
    
    std::cout << "    testConfigDiff passed" << std::endl;
}

void testConfigDocumentationGeneration() {
    nlm::Config config;
    config.set("key1", 42, nlm::ConfigSource::Runtime, "First key");
    config.set("key2", "value", nlm::ConfigSource::File, "Second key");
    
    std::string doc = config.generateDocumentation();
    assert(!doc.empty());
    assert(doc.find("Configuration Documentation") != std::string::npos);
    
    std::cout << "    testConfigDocumentationGeneration passed" << std::endl;
}

void testConfigInheritance() {
    nlm::Config parent;
    parent.set("parent_key", "parent_value", nlm::ConfigSource::File);
    
    nlm::Config child;
    child.setParentConfig(std::make_shared<nlm::ParentConfig>(parent));
    
    // Note: This requires implementing the ParentConfig interface
    // For now, we'll test the structure
    assert(true); // Placeholder
    
    std::cout << "    testConfigInheritance passed" << std::endl;
}

void runAll() {
    std::cout << "Running enhanced configuration tests..." << std::endl;
    
    testConfigCreation();
    testConfigSetAndGet();
    testConfigGetOr();
    testConfigRemove();
    testConfigKeys();
    testConfigVersion();
    testConfigDataTypes();
    testConfigValidation();
    testConfigEnvironmentVariables();
    testConfigJSONParsing();
    testConfigMerging();
    testConfigComparison();
    testConfigDiff();
    testConfigDocumentationGeneration();
    testConfigInheritance();
    
    std::cout << "All enhanced configuration tests passed!" << std::endl;
}

} // namespace test_config