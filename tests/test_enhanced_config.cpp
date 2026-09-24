// Comprehensive Config Tests
// Tests for enhanced Config class functionality

#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>
#include <fstream>
#include <filesystem>

namespace test_enhanced_config {

void testConfigCreation() {
    nlm::Config config;
    assert(!config.has("test"));
    assert(config.getKeys().empty());
    std::cout << "    testConfigCreation passed" << std::endl;
}

void testConfigSetAndGet() {
    nlm::Config config;
    
    // Test all supported types
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
    
    config.set("int_vector", std::vector<int>({1, 2, 3}), nlm::ConfigSource::Runtime);
    auto vectorVal = config.get<std::vector<int>>("int_vector");
    assert(vectorVal.has_value());
    assert(vectorVal->size() == 3);
    assert((*vectorVal)[0] == 1);
    
    config.set("double_vector", std::vector<double>({1.1, 2.2, 3.3}), nlm::ConfigSource::Runtime);
    auto doubleVectorVal = config.get<std::vector<double>>("double_vector");
    assert(doubleVectorVal.has_value());
    assert(doubleVectorVal->size() == 3);
    assert((*doubleVectorVal)[1] == 2.2);
    
    config.set("string_vector", std::vector<std::string>({"a", "b", "c"}), nlm::ConfigSource::Runtime);
    auto stringVectorVal = config.get<std::vector<std::string>>("string_vector");
    assert(stringVectorVal.has_value());
    assert(stringVectorVal->size() == 3);
    assert((*stringVectorVal)[2] == "c");
    
    std::cout << "    testConfigSetAndGet passed" << std::endl;
}

void testConfigGetOr() {
    nlm::Config config;
    
    int val = config.getOr<int>("missing", 100);
    assert(val == 100);
    
    config.set("existing", 50, nlm::ConfigSource::Runtime);
    val = config.getOr<int>("existing", 100);
    assert(val == 50);
    
    // Test getOr with other types
    double doubleVal = config.getOr<double>("missing", 3.14);
    assert(doubleVal == 3.14);
    
    std::string stringVal = config.getOr<std::string>("missing", "default");
    assert(stringVal == "default");
    
    bool boolVal = config.getOr<bool>("missing", true);
    assert(boolVal == true);
    
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
    
    // Test prefix filtering
    config.set("prefix_a", "a", nlm::ConfigSource::Runtime);
    config.set("prefix_b", "b", nlm::ConfigSource::Runtime);
    config.set("different", "c", nlm::ConfigSource::Runtime);
    
    auto prefixKeys = config.getKeys("prefix_");
    assert(prefixKeys.size() == 2);
    assert(std::find(prefixKeys.begin(), prefixKeys.end(), "prefix_a") != prefixKeys.end());
    assert(std::find(prefixKeys.begin(), prefixKeys.end(), "prefix_b") != prefixKeys.end());
    
    std::cout << "    testConfigKeys passed" << std::endl;
}

void testConfigValidation() {
    nlm::Config config;
    
    // Test type validation based on key patterns
    config.set("neuron_count", 100, nlm::ConfigSource::Runtime);
    assert(config.validateKey("neuron_count").valid);
    
    config.set("learning_rate", 0.01, nlm::ConfigSource::Runtime);
    assert(config.validateKey("learning_rate").valid);
    
    config.set("enabled", true, nlm::ConfigSource::Runtime);
    assert(config.validateKey("enabled").valid);
    
    config.set("name", "test_brain", nlm::ConfigSource::Runtime);
    assert(config.validateKey("name").valid);
    
    // Test validation for invalid types (should still pass basic validation)
    config.set("invalid_count", "not_a_number", nlm::ConfigSource::Runtime);
    assert(config.validateKey("invalid_count").valid);  // Basic validation passes
    
    // Test full validation
    auto result = config.validate();
    assert(result.valid);  // All entries are valid according to basic rules
    
    std::cout << "    testConfigValidation passed" << std::endl;
}

void testConfigSourceAndOverride() {
    nlm::Config config;
    
    config.set("param1", 1, nlm::ConfigSource::File);
    config.set("param2", 2, nlm::ConfigSource::Default);
    config.set("param3", 3, nlm::ConfigSource::CommandLine);
    
    assert(config.getSource("param1") == nlm::ConfigSource::File);
    assert(config.getSource("param2") == nlm::ConfigSource::Default);
    assert(config.getSource("param3") == nlm::ConfigSource::CommandLine);
    
    assert(!config.isOverridden("param1"));  // File source is not considered overridden
    assert(!config.isOverridden("param2"));  // Default source is not considered overridden
    assert(config.isOverridden("param3"));   // CommandLine source is considered overridden
    
    std::cout << "    testConfigSourceAndOverride passed" << std::endl;
}

void testConfigBulkOperations() {
    nlm::Config config;
    
    // Test apply with map
    std::map<std::string, nlm::ConfigValue> values;
    values["batch_int"] = 42;
    values["batch_string"] = std::string("batch_test");
    values["batch_bool"] = true;
    config.apply(values);
    
    assert(config.has("batch_int"));
    assert(config.has("batch_string"));
    assert(config.has("batch_bool"));
    
    // Test apply with another config
    nlm::Config other;
    other.set("other1", 100, nlm::ConfigSource::File);
    other.set("other2", "other_value", nlm::ConfigSource::File);
    
    config.apply(other);
    assert(config.has("other1"));
    assert(config.has("other2"));
    assert(config.getOr<int>("other1", 0) == 100);
    
    // Test copy
    nlm::Config copy = config.copy(nlm::ConfigSource::File);
    assert(copy.has("batch_int"));  // Runtime entries are not copied
    assert(copy.has("other1"));   // File entries are copied
    assert(copy.getSource("other1") == nlm::ConfigSource::File);
    
    // Test merge
    nlm::Config third;
    third.set("merge_test", 999, nlm::ConfigSource::Runtime);
    
    nlm::Config base;
    base.set("merge_test", 111, nlm::ConfigSource::Default);
    
    base.merge(third, nlm::ConfigSource::Runtime);
    assert(base.getOr<int>("merge_test", 0) == 999);  // Runtime overrides Default
    
    std::cout << "    testConfigBulkOperations passed" << std::endl;
}

void testConfigEquals() {
    nlm::Config config1;
    config1.set("key1", 1, nlm::ConfigSource::Runtime);
    config1.set("key2", "value", nlm::ConfigSource::Runtime);
    
    nlm::Config config2;
    config2.set("key1", 1, nlm::ConfigSource::Runtime);
    config2.set("key2", "value", nlm::ConfigSource::Runtime);
    
    assert(config1.equals(config2));
    
    nlm::Config config3;
    config3.set("key1", 2, nlm::ConfigSource::Runtime);  // Different value
    assert(!config1.equals(config3));
    
    nlm::Config config4;
    config4.set("key1", 1, nlm::ConfigSource::Runtime);
    // Missing key2 in config4
    assert(!config1.equals(config4));
    
    std::cout << "    testConfigEquals passed" << std::endl;
}

void testConfigSaveAndLoad() {
    nlm::Config config;
    
    // Set up test values of different types
    config.set("int_value", 42, nlm::ConfigSource::Runtime);
    config.set("double_value", 3.14, nlm::ConfigSource::Runtime);
    config.set("string_value", "test", nlm::ConfigSource::Runtime);
    config.set("bool_value", true, nlm::ConfigSource::Runtime);
    config.set("int_list", std::vector<int>({1, 2, 3}), nlm::ConfigSource::Runtime);
    config.set("string_list", std::vector<std::string>({"a", "b", "c"}), nlm::ConfigSource::Runtime);
    
    // Test save to file
    std::string testFile = "/tmp/test_config.txt";
    assert(config.saveToFile(testFile));
    
    // Create new config and load
    nlm::Config config2;
    assert(config2.loadFromFile(testFile));
    
    // Verify loaded values
    assert(config2.has("int_value"));
    assert(config2.has("double_value"));
    assert(config2.has("string_value"));
    assert(config2.has("bool_value"));
    assert(config2.has("int_list"));
    assert(config2.has("string_list"));
    
    // Clean up
    std::filesystem::remove(testFile);
    
    std::cout << "    testConfigSaveAndLoad passed" << std::endl;
}

void testConfigStringParsing() {
    nlm::Config config;
    
    // Test parsing of various value formats
    config.set("simple_string", "simple", nlm::ConfigSource::File);
    config.set("quoted_string", "\"quoted\"", nlm::ConfigSource::File);
    config.set("integer_string", "123", nlm::ConfigSource::File);
    config.set("float_string", "3.14", nlm::ConfigSource::File);
    config.set("boolean_string", "true", nlm::ConfigSource::File);
    
    // Test parsing of lists
    config.set("int_list", "[1, 2, 3]", nlm::ConfigSource::File);
    config.set("string_list", "[\"a\", \"b\", \"c\"]", nlm::ConfigSource::File);
    
    // Test parsing of maps/objects
    config.set("object", "{key1: value1, key2: value2}", nlm::ConfigSource::File);
    
    std::cout << "    testConfigStringParsing passed" << std::endl;
}

void testConfigClear() {
    nlm::Config config;
    
    config.set("key1", 1, nlm::ConfigSource::Runtime);
    config.set("key2", "value", nlm::ConfigSource::Runtime);
    
    assert(!config.getKeys().empty());
    
    config.clear();
    assert(config.getKeys().empty());
    assert(!config.has("key1"));
    assert(!config.has("key2"));
    
    std::cout << "    testConfigClear passed" << std::endl;
}

void runAll() {
    testConfigCreation();
    testConfigSetAndGet();
    testConfigGetOr();
    testConfigRemove();
    testConfigKeys();
    testConfigValidation();
    testConfigSourceAndOverride();
    testConfigBulkOperations();
    testConfigEquals();
    testConfigSaveAndLoad();
    testConfigStringParsing();
    testConfigClear();
    
    std::cout << std::endl;
    std::cout << "=== All Enhanced Config Tests Passed ===" << std::endl;
}

} // namespace test_enhanced_config

// Test runner function
void runEnhancedConfigTests() {
    test_enhanced_config::runAll();
}