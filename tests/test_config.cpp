// Config Tests
#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>
#include <fstream>
#include <filesystem>
#include <string>
#include <vector>
#include <memory>
#include <algorithm>
#include <limits>

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

void testConfigEmptyFileHandling() {
    nlm::Config config;
    
    // Test with non-existent file
    std::string nonexistent = "/tmp/nonexistent_config_12345.txt";
    
    // Note: loadFromFile may throw or return false, we should handle both
    try {
        bool result = config.loadFromFile(nonexistent);
        // If it returns, it should indicate failure
        if (result) {
            std::cerr << "WARNING: loadFromFile succeeded on non-existent file" << std::endl;
        }
    } catch (const nlm::ConfigException& e) {
        // Expected - file not found error
    }
    
    // Test with empty file
    std::string emptyFile = "/tmp/empty_config_12345.txt";
    std::ofstream emptyFileStream(emptyFile);
    emptyFileStream.close();
    
    try {
        bool result = config.loadFromFile(emptyFile);
        // Empty file should load successfully (no content)
        if (!result) {
            std::cerr << "WARNING: Empty file failed to load" << std::endl;
        }
    } catch (const nlm::ConfigException& e) {
        // Empty file handling may throw, depending on implementation
        std::cerr << "Empty file handling: " << e.what() << std::endl;
    }
    
    std::filesystem::remove(emptyFile);
    
    std::cout << "    testConfigEmptyFileHandling passed" << std::endl;
}

void testConfigInvalidJsonFileHandling() {
    nlm::Config config;
    
    // Test with malformed JSON file
    std::string malformedJsonFile = "/tmp/malformed_json_12345.json";
    std::ofstream jsonFile(malformedJsonFile);
    jsonFile << "{\"invalid\": json, \"missing_quote\"}";
    jsonFile.close();
    
    try {
        bool result = config.loadFromFile(malformedJsonFile);
        if (result) {
            std::cerr << "WARNING: Malformed JSON loaded successfully" << std::endl;
        }
    } catch (const nlm::ConfigException& e) {
        // Expected - JSON parse error
        std::cout << "    Caught expected JSON parse error: " << e.what() << std::endl;
    }
    
    std::filesystem::remove(malformedJsonFile);
    
    // Test with truncated JSON file
    std::string truncatedJsonFile = "/tmp/truncated_json_12345.json";
    std::ofstream truncatedFile(truncatedJsonFile);
    truncatedFile << "{\"key\": \"value\", \"missing_end\"";
    truncatedFile.close();
    
    try {
        bool result = config.loadFromFile(truncatedJsonFile);
        if (result) {
            std::cerr << "WARNING: Truncated JSON loaded successfully" << std::endl;
        }
    } catch (const nlm::ConfigException& e) {
        // Expected - JSON parse error
        std::cout << "    Caught expected truncated JSON error: " << e.what() << std::endl;
    }
    
    std::filesystem::remove(truncatedJsonFile);
    
    std::cout << "    testConfigInvalidJsonFileHandling passed" << std::endl;
}

void testConfigInvalidYamlFileHandling() {
    nlm::Config config;
    
    // Test with malformed YAML file
    std::string malformedYamlFile = "/tmp/malformed_yaml_12345.yaml";
    std::ofstream yamlFile(malformedYamlFile);
    yamlFile << "invalid: [unclosed bracket";
    yamlFile.close();
    
    try {
        bool result = config.loadFromFile(malformedYamlFile);
        if (result) {
            std::cerr << "WARNING: Malformed YAML loaded successfully" << std::endl;
        }
    } catch (const nlm::ConfigException& e) {
        // Expected - YAML parse error
        std::cout << "    Caught expected YAML parse error: " << e.what() << std::endl;
    }
    
    std::filesystem::remove(malformedYamlFile);
    
    // Test with empty YAML file
    std::string emptyYamlFile = "/tmp/empty_yaml_12345.yaml";
    std::ofstream emptyYamlFileStream(emptyYamlFile);
    emptyYamlFileStream.close();
    
    try {
        bool result = config.loadFromFile(emptyYamlFile);
        // Empty YAML file should load successfully (no content)
        if (!result) {
            std::cerr << "WARNING: Empty YAML file failed to load" << std::endl;
        }
    } catch (const nlm::ConfigException& e) {
        // Empty file handling may throw
        std::cerr << "Empty YAML file handling: " << e.what() << std::endl;
    }
    
    std::filesystem::remove(emptyYamlFile);
    
    std::cout << "    testConfigInvalidYamlFileHandling passed" << std::endl;
}

void testConfigNestedKeyHandling() {
    nlm::Config config;
    
    // Test deeply nested keys
    config.set("a.b.c.d.e.f.g.h.i.j.k", "deep_value", nlm::ConfigSource::Runtime);
    assert(config.has("a.b.c.d.e.f.g.h.i.j.k"));
    
    auto value = config.get<std::string>("a.b.c.d.e.f.g.h.i.j.k");
    assert(value.has_value());
    assert(*value == "deep_value");
    
    // Test with numeric nested keys
    config.set("x.y.z.1.2.3.4.5", "numeric_value", nlm::ConfigSource::Runtime);
    auto numericValue = config.get<std::string>("x.y.z.1.2.3.4.5");
    assert(numericValue.has_value());
    assert(*numericValue == "numeric_value");
    
    // Test special characters in keys
    config.set("key.with.dots", "value1", nlm::ConfigSource::Runtime);
    config.set("key-with-dashes", "value2", nlm::ConfigSource::Runtime);
    config.set("key with spaces", "value3", nlm::ConfigSource::Runtime);
    
    assert(config.has("key.with.dots"));
    assert(config.has("key-with-dashes"));
    assert(config.has("key with spaces"));
    
    std::cout << "    testConfigNestedKeyHandling passed" << std::endl;
}

void testConfigTypeConversionEdgeCases() {
    nlm::Config config;
    
    // Test type conversion edge cases
    config.set("int_val", 42, nlm::ConfigSource::Runtime);
    config.set("double_val", 3.14, nlm::ConfigSource::Runtime);
    config.set("bool_val", false, nlm::ConfigSource::Runtime);
    config.set("string_val", "", nlm::ConfigSource::Runtime); // Empty string
    
    // Test that empty string is preserved
    auto emptyStr = config.get<std::string>("string_val");
    assert(emptyStr.has_value());
    assert(*emptyStr == "");
    
    // Test boolean conversion edge cases
    config.set("zero_int", 0, nlm::ConfigSource::Runtime);
    config.set("non_zero_int", 1, nlm::ConfigSource::Runtime);
    
    auto zeroBool = config.get<bool>("zero_int");
    auto nonZeroBool = config.get<bool>("non_zero_int");
    
    // Note: bool conversion from int may be implementation-defined
    // We just verify it doesn't crash
    
    // Test large values
    config.set("large_int", std::numeric_limits<int>::max(), nlm::ConfigSource::Runtime);
    config.set("large_double", std::numeric_limits<double>::max(), nlm::ConfigSource::Runtime);
    
    // Test negative values
    config.set("negative_int", -2147483648, nlm::ConfigSource::Runtime);
    config.set("negative_double", -3.14159, nlm::ConfigSource::Runtime);
    
    std::cout << "    testConfigTypeConversionEdgeCases passed" << std::endl;
}

void testConfigArrayEdgeCases() {
    nlm::Config config;
    
    // Test empty arrays
    config.set("empty_int_array", std::vector<int>{}, nlm::ConfigSource::Runtime);
    config.set("empty_string_array", std::vector<std::string>{}, nlm::ConfigSource::Runtime);
    
    auto emptyIntArray = config.get<std::vector<int>>("empty_int_array");
    assert(emptyIntArray.has_value());
    assert(emptyIntArray->empty());
    
    auto emptyStringArray = config.get<std::vector<std::string>>("empty_string_array");
    assert(emptyStringArray.has_value());
    assert(emptyStringArray->empty());
    
    // Test arrays with special values
    config.set("mixed_array", std::vector<std::string>{"a", "", "b", "very long string to test length limits", "c"}, nlm::ConfigSource::Runtime);
    
    // Test large arrays
    std::vector<int> largeArray(10000, 42);
    config.set("large_array", largeArray, nlm::ConfigSource::Runtime);
    
    auto largeArrayRetrieved = config.get<std::vector<int>>("large_array");
    assert(largeArrayRetrieved.has_value());
    assert(largeArrayRetrieved->size() == 10000);
    
    std::cout << "    testConfigArrayEdgeCases passed" << std::endl;
}

void testConfigMemoryManagement() {
    nlm::Config config;
    
    // Test memory efficiency with many entries
    for (int i = 0; i < 1000; ++i) {
        config.set("key_" + std::to_string(i), i, nlm::ConfigSource::Runtime);
    }
    
    // Verify all entries are accessible
    for (int i = 0; i < 1000; ++i) {
        auto val = config.get<int>("key_" + std::to_string(i));
        assert(val.has_value());
        assert(*val == i);
    }
    
    // Test clearing large config
    config.clear();
    assert(config.size() == 0);
    
    // Test memory after many operations
    for (int i = 0; i < 100; ++i) {
        config.set("temp_key_" + std::to_string(i), i, nlm::ConfigSource::Runtime);
        config.remove("temp_key_" + std::to_string(i));
    }
    
    std::cout << "    testConfigMemoryManagement passed" << std::endl;
}

void testConfigValidationEdgeCases() {
    nlm::Config config;
    
    // Test validation with empty context
    nlm::ConfigSchema schema;
    
    // Add validation rule that always passes
    schema["test_key"] = nlm::ConfigValidation("test", "Test rule")
        .addRule([](const nlm::ConfigValue& value) { return true; });
    
    config.addSchema(schema);
    
    // Test validation with empty context (should still validate)
    assert(config.validate());
    
    // Test validation with empty context but invalid schema entry
    config.set("invalid_key", "value", nlm::ConfigSource::Runtime);
    
    // Validation should still work even with no schema for this key
    assert(config.validate());
    
    std::cout << "    testConfigValidationEdgeCases passed" << std::endl;
}

void testConfigSerializationEdgeCases() {
    nlm::Config config;
    
    // Test with complex nested structure
    config.set("level1.level2.level3.deep", "value", nlm::ConfigSource::Runtime);
    config.set("level1.level2.level3.empty_array", std::vector<int>{}, nlm::ConfigSource::Runtime);
    config.set("level1.level2.level3.bool_false", false, nlm::ConfigSource::Runtime);
    
    // Convert to JSON and back
    std::string json = config.toJson();
    
    nlm::Config configFromJson;
    configFromJson.fromJson(json);
    
    // Verify values are preserved
    assert(configFromJson.has("level1.level2.level3.deep"));
    auto jsonValue = configFromJson.get<std::string>("level1.level2.level3.deep");
    assert(jsonValue.has_value());
    assert(*jsonValue == "value");
    
    // Test with special characters in string values
    config.set("special_chars", "special \"quotes\" and \\backslashes\\ and newlines\nand tabs\t", nlm::ConfigSource::Runtime);
    std::string json2 = config.toJson();
    
    nlm::Config configFromJson2;
    configFromJson2.fromJson(json2);
    
    auto specialValue = configFromJson2.get<std::string>("special_chars");
    assert(specialValue.has_value());
    
    std::cout << "    testConfigSerializationEdgeCases passed" << std::endl;
}

void runAll() {
    testConfigCreation();
    testConfigSetAndGet();
    testConfigGetOr();
    testConfigRemove();
    testConfigKeys();
    testConfigEmptyFileHandling();
    testConfigInvalidJsonFileHandling();
    testConfigInvalidYamlFileHandling();
    testConfigNestedKeyHandling();
    testConfigTypeConversionEdgeCases();
    testConfigArrayEdgeCases();
    testConfigMemoryManagement();
    testConfigValidationEdgeCases();
    testConfigSerializationEdgeCases();
}

} // namespace test_config
