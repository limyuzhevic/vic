// Config Tests - Updated with modern testing framework
#include "tests/nlm_test_framework.hpp"
#include "core/Config/Config.hpp"

namespace test_config {

void testConfigCreation() {
    nlm::Config config;
    ASSERT(!config.has("test"), "Config should not have test key initially");
}

void testConfigSetAndGet() {
    nlm::Config config;
    
    config.set("int_value", 42, nlm::ConfigSource::Runtime);
    ASSERT(config.has("int_value"), "Config should have int_value");
    
    auto intVal = config.get<int>("int_value");
    ASSERT(intVal.has_value(), "int_value should have a value");
    ASSERT(*intVal == 42, "int_value should be 42");
    
    config.set("double_value", 3.14, nlm::ConfigSource::Runtime);
    auto doubleVal = config.get<double>("double_value");
    ASSERT(doubleVal.has_value(), "double_value should have a value");
    ASSERT(*doubleVal > 3.13 && *doubleVal < 3.15, "double_value should be approximately 3.14");
    
    config.set("string_value", std::string("hello"), nlm::ConfigSource::Runtime);
    auto stringVal = config.get<std::string>("string_value");
    ASSERT(stringVal.has_value(), "string_value should have a value");
    ASSERT(*stringVal == "hello", "string_value should be 'hello'");
    
    config.set("bool_value", true, nlm::ConfigSource::Runtime);
    auto boolVal = config.get<bool>("bool_value");
    ASSERT(boolVal.has_value(), "bool_value should have a value");
    ASSERT(*boolVal == true, "bool_value should be true");
}

void testConfigGetOr() {
    nlm::Config config;
    
    int val = config.getOr<int>("missing", 100);
    ASSERT(val == 100, "Missing key should return default value");
    
    config.set("existing", 50, nlm::ConfigSource::Runtime);
    val = config.getOr<int>("existing", 100);
    ASSERT(val == 50, "Existing key should return its value");
}

void testConfigRemove() {
    nlm::Config config;
    
    config.set("test", 123, nlm::ConfigSource::Runtime);
    ASSERT(config.has("test"), "Config should have test key");
    
    config.remove("test");
    ASSERT(!config.has("test"), "Config should not have test key after removal");
}

void testConfigKeys() {
    nlm::Config config;
    
    config.set("key1", 1, nlm::ConfigSource::Runtime);
    config.set("key2", 2, nlm::ConfigSource::Runtime);
    config.set("key3", 3, nlm::ConfigSource::Runtime);
    
    auto keys = config.getKeys();
    ASSERT(keys.size() == 3, "Config should have 3 keys");
    
    // Check that all expected keys are present
    bool hasKey1 = std::find(keys.begin(), keys.end(), std::string("key1")) != keys.end();
    bool hasKey2 = std::find(keys.begin(), keys.end(), std::string("key2")) != keys.end();
    bool hasKey3 = std::find(keys.begin(), keys.end(), std::string("key3")) != keys.end();
    
    ASSERT(hasKey1 && hasKey2 && hasKey3, "Config should contain key1, key2, and key3");
}

void testConfigSaveLoad() {
    nlm::Config config1;
    config1.set("test_key", "test_value", nlm::ConfigSource::Runtime);
    config1.set("number", 42, nlm::ConfigSource::Runtime);
    config1.set("enabled", true, nlm::ConfigSource::Runtime);
    
    // Save to file
    std::string tempFile = "test_config_save.tmp";
    bool saveSuccess = config1.saveToFile(tempFile);
    ASSERT(saveSuccess, "Config save should succeed");
    
    // Load from file
    nlm::Config config2;
    bool loadSuccess = config2.loadFromFile(tempFile);
    ASSERT(loadSuccess, "Config load should succeed");
    
    // Verify loaded values
    auto testKey = config2.get<std::string>("test_key");
    ASSERT(testKey.has_value(), "Loaded config should have test_key");
    ASSERT(*testKey == "test_value", "Loaded test_key should equal 'test_value'");
    
    auto number = config2.get<int>("number");
    ASSERT(number.has_value(), "Loaded config should have number");
    ASSERT(*number == 42, "Loaded number should be 42");
    
    auto enabled = config2.get<bool>("enabled");
    ASSERT(enabled.has_value(), "Loaded config should have enabled");
    ASSERT(*enabled == true, "Loaded enabled should be true");
    
    // Clean up
    std::remove(tempFile.c_str());
}

void runAllTests() {
    testConfigCreation();
    testConfigSetAndGet();
    testConfigGetOr();
    testConfigRemove();
    testConfigKeys();
    testConfigSaveLoad();
}

} // namespace test_config

// Auto-register test suite
NLM_TEST_MAIN()