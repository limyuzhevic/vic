// Configuration Tests - Production Unit Tests
// These tests verify core configuration functionality using the production test framework

#include "NLMTestFramework.hpp"
#include "core/Config/Config.hpp"

namespace test_config_production {

void testConfigBasicCreation() {
    nlm::Config config;
    ASSERT_FALSE(config.has("test"));
}

void testConfigSetAndGetInt() {
    nlm::Config config;
    
    config.set("int_value", 42, nlm::ConfigSource::Runtime);
    ASSERT_TRUE(config.has("int_value"));
    
    auto intVal = config.get<int>("int_value");
    ASSERT_TRUE(intVal.has_value());
    ASSERT_EQ(*intVal, 42);
}

void testConfigSetAndGetDouble() {
    nlm::Config config;
    
    config.set("double_value", 3.14, nlm::ConfigSource::Runtime);
    auto doubleVal = config.get<double>("double_value");
    ASSERT_TRUE(doubleVal.has_value());
    ASSERT_TRUE(*doubleVal > 3.13 && *doubleVal < 3.15);
}

void testConfigSetAndGetString() {
    nlm::Config config;
    
    config.set("string_value", std::string("hello"), nlm::ConfigSource::Runtime);
    auto stringVal = config.get<std::string>("string_value");
    ASSERT_TRUE(stringVal.has_value());
    ASSERT_EQ(*stringVal, "hello");
}

void testConfigSetAndGetBool() {
    nlm::Config config;
    
    config.set("bool_value", true, nlm::ConfigSource::Runtime);
    auto boolVal = config.get<bool>("bool_value");
    ASSERT_TRUE(boolVal.has_value());
    ASSERT_TRUE(*boolVal == true);
}

void testConfigGetOrWithMissingKey() {
    nlm::Config config;
    
    int val = config.getOr<int>("missing", 100);
    ASSERT_EQ(val, 100);
}

void testConfigGetOrWithExistingKey() {
    nlm::Config config;
    
    config.set("existing", 50, nlm::ConfigSource::Runtime);
    int val = config.getOr<int>("existing", 100);
    ASSERT_EQ(val, 50);
}

void testConfigRemove() {
    nlm::Config config;
    
    config.set("test", 123, nlm::ConfigSource::Runtime);
    ASSERT_TRUE(config.has("test"));
    
    config.remove("test");
    ASSERT_FALSE(config.has("test"));
}

void testConfigGetKeys() {
    nlm::Config config;
    
    config.set("key1", 1, nlm::ConfigSource::Runtime);
    config.set("key2", 2, nlm::ConfigSource::Runtime);
    config.set("key3", 3, nlm::ConfigSource::Runtime);
    
    auto keys = config.getKeys();
    ASSERT_EQ(keys.size(), 3);
    
    bool key1Found = false, key2Found = false, key3Found = false;
    for (const auto& key : keys) {
        if (key == "key1") key1Found = true;
        if (key == "key2") key2Found = true;
        if (key == "key3") key3Found = true;
    }
    ASSERT_TRUE(key1Found);
    ASSERT_TRUE(key2Found);
    ASSERT_TRUE(key3Found);
}

void testConfigClean() {
    nlm::Config config;
    
    config.set("temp1", 1, nlm::ConfigSource::Runtime);
    config.set("temp2", 2, nlm::ConfigSource::Runtime);
    
    config.clear();
    ASSERT_FALSE(config.has("temp1"));
    ASSERT_FALSE(config.has("temp2"));
    
    auto keys = config.getKeys();
    ASSERT_EQ(keys.size(), 0);
}

void testConfigSourceTracking() {
    nlm::Config config;
    
    config.set("default_key", 1, nlm::ConfigSource::Default);
    config.set("file_key", 2, nlm::ConfigSource::File);
    config.set("cli_key", 3, nlm::ConfigSource::CommandLine);
    config.set("runtime_key", 4, nlm::ConfigSource::Runtime);
    
    auto defaultVal = config.get<int>("default_key");
    auto fileVal = config.get<int>("file_key");
    auto cliVal = config.get<int>("cli_key");
    auto runtimeVal = config.get<int>("runtime_key");
    
    ASSERT_TRUE(defaultVal.has_value());
    ASSERT_TRUE(fileVal.has_value());
    ASSERT_TRUE(cliVal.has_value());
    ASSERT_TRUE(runtimeVal.has_value());
}

void runAllProductionTests() {
    // Register production unit tests
    auto& testRunner = getTestCases("production");
    
    testRunner.push_back({
        "Basic Creation",
        "production",
        testConfigBasicCreation
    });
    
    testRunner.push_back({
        "Set and Get Int",
        "production",
        testConfigSetAndGetInt
    });
    
    testRunner.push_back({
        "Set and Get Double",
        "production",
        testConfigSetAndGetDouble
    });
    
    testRunner.push_back({
        "Set and Get String",
        "production",
        testConfigSetAndGetString
    });
    
    testRunner.push_back({
        "Set and Get Bool",
        "production",
        testConfigSetAndGetBool
    });
    
    testRunner.push_back({
        "Get Or With Missing Key",
        "production",
        testConfigGetOrWithMissingKey
    });
    
    testRunner.push_back({
        "Get Or With Existing Key",
        "production",
        testConfigGetOrWithExistingKey
    });
    
    testRunner.push_back({
        "Remove Key",
        "production",
        testConfigRemove
    });
    
    testRunner.push_back({
        "Get All Keys",
        "production",
        testConfigGetKeys
    });
    
    testRunner.push_back({
        "Clean Configuration",
        "production",
        testConfigClean
    });
    
    testRunner.push_back({
        "Source Tracking",
        "production",
        testConfigSourceTracking
    });
}

} // namespace test_config_production
