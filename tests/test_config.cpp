// Config Tests
#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>
#include <unordered_map>

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

void testConfigErrorHandling() {
    nlm::Config config;
    
    // Add a schema to trigger validation errors
    nlm::ConfigSchema schema;
    schema.name = "strict_schema";
    
    nlm::ParameterBounds strictBounds;
    strictBounds.min = 1;
    strictBounds.max = 10;
    strictBounds.allowNegative = false;
    strictBounds.allowZero = false;
    strictBounds.unit = "count";
    
    schema.parameterBounds["strict_key"] = strictBounds;
    config.addSchema("strict_schema", schema);
    
    // Test that setting invalid value throws ConfigValidationError
    bool exceptionThrown = false;
    try {
        config.set("strict_key", 15, nlm::ConfigSource::Runtime);  // Too large
    } catch (const nlm::ConfigValidationError& e) {
        exceptionThrown = true;
        assert(e.key == "strict_key");
        assert(e.source == nlm::ConfigSource::Runtime);
    }
    assert(exceptionThrown);
    
    // Test setting negative value
    exceptionThrown = false;
    try {
        config.set("strict_key", -5, nlm::ConfigSource::Runtime);  // Negative
    } catch (const nlm::ConfigValidationError& e) {
        exceptionThrown = true;
        assert(e.key == "strict_key");
    }
    assert(exceptionThrown);
    
    // Test setting zero
    exceptionThrown = false;
    try {
        config.set("strict_key", 0, nlm::ConfigSource::Runtime);  // Zero not allowed
    } catch (const nlm::ConfigValidationError& e) {
        exceptionThrown = true;
        assert(e.key == "strict_key");
    }
    assert(exceptionThrown);
    
    // Test that setting valid value doesn't throw
    try {
        config.set("strict_key", 5, nlm::ConfigSource::Runtime);  // Valid
        assert(config.has("strict_key"));
    } catch (...) {
        assert(false); // Should not throw
    }
    
    // Test loadFromFile with non-existent file
    bool loadResult = config.loadFromFile("/nonexistent/file.txt");
    assert(!loadResult);
    
    // Test saveToFile (this should work if file can be created)
    // We'll create a temp file path
    std::string tempFile = "/tmp/test_config_save.txt";
    
    // Clean up if exists
    std::remove(tempFile.c_str());
    
    bool saveResult = config.saveToFile(tempFile);
    assert(saveResult);
    assert(std::ifstream(tempFile).good());
    
    // Clean up
    std::remove(tempFile.c_str());
    
    std::cout << "    testConfigErrorHandling passed" << std::endl;
}

void runAll() {
    std::cout << "Running Config tests..." << std::endl;
    testConfigCreation();
    testConfigSetAndGet();
    testConfigGetOr();
    testConfigRemove();
    testConfigKeys();
    testConfigValidation();
    testConfigBatchOperations();
    testConfigCopyAndMerge();
    testConfigPresets();
    testConfigExportImport();
    testConfigErrorHandling();
}

} // namespace test_config
