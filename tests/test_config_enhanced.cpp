// Enhanced Config Tests
// #include "core/Config/Config.hpp"
// #include <cassert>
// #include <iostream>
// #include <stdexcept>

namespace test_config_enhanced {

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

void testConfigValidation() {
    nlm::Config config;
    
    // Test valid value setting
    config.set("valid_int", 100, nlm::ConfigSource::Runtime);
    assert(config.has("valid_int"));
    
    // Test validation (in enhanced version, validation should be available)
    auto validation = config.validate("valid_int", nlm::ConfigValue(100));
    assert(validation.valid);
    
    std::cout << "    testConfigValidation passed" << std::endl;
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

void testConfigSchema() {
    nlm::Config config;
    
    // Test schema addition
    nlm::ConfigSchema schema("test_param", "Test parameter", nlm::ConfigSource::Default,
                           "0", "int", "units", -100, 100);
    config.addSchema(schema);
    
    assert(config.hasDescription("test_param"));
    assert(config.getType("test_param") == "int");
    assert(config.getUnits("test_param") == "units");
    
    // Test all schema keys
    auto allKeys = config.getAllSchemaKeys();
    assert(std::find(allKeys.begin(), allKeys.end(), "test_param") != allKeys.end());
    
    std::cout << "    testConfigSchema passed" << std::endl;
}

void testConfigSaveLoad() {
    nlm::Config config;
    
    // Set some values
    config.set("test_int", 42, nlm::ConfigSource::Runtime);
    config.set("test_double", 3.14, nlm::ConfigSource::Runtime);
    config.set("test_string", "hello", nlm::ConfigSource::Runtime);
    config.set("test_bool", true, nlm::ConfigSource::Runtime);
    
    // Save to file
    std::string tempFile = "test_config.txt";
    bool saveResult = config.saveToFile(tempFile);
    assert(saveResult);
    
    // Clear and load back
    config.clear();
    bool loadResult = config.loadFromFile(tempFile);
    assert(loadResult);
    
    // Verify some values were loaded
    assert(config.has("test_int"));
    
    std::cout << "    testConfigSaveLoad passed" << std::endl;
}

void testAll() {
    std::cout << "Running Enhanced Config tests..." << std::endl;
    testConfigCreation();
    testConfigSetAndGet();
    testConfigValidation();
    testConfigGetOr();
    testConfigRemove();
    testConfigKeys();
    testConfigSchema();
    testConfigSaveLoad();
}

} // namespace test_config_enhanced

// Test runner for enhanced tests
void runEnhancedConfigTests() {
    test_config_enhanced::testAll();
}