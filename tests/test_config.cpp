// Config Tests - Google Test Version
#include "core/Config/Config.hpp"
#include <gtest/gtest.h>
#include <iostream>

namespace nlm {

class ConfigTest : public ::testing::Test {
protected:
    void SetUp() override {
        config = Config();
    }
    
    Config config;
};

TEST_F(ConfigTest, ConfigCreation) {
    ASSERT_FALSE(config.has("test"));
}

TEST_F(ConfigTest, ConfigSetAndGet) {
    config.set("int_value", 42, ConfigSource::Runtime);
    ASSERT_TRUE(config.has("int_value"));
    
    auto intVal = config.get<int>("int_value");
    ASSERT_TRUE(intVal.has_value());
    ASSERT_EQ(*intVal, 42);
    
    config.set("double_value", 3.14, ConfigSource::Runtime);
    auto doubleVal = config.get<double>("double_value");
    ASSERT_TRUE(doubleVal.has_value());
    ASSERT_NEAR(*doubleVal, 3.14, 0.01);
    
    config.set("string_value", std::string("hello"), ConfigSource::Runtime);
    auto stringVal = config.get<std::string>("string_value");
    ASSERT_TRUE(stringVal.has_value());
    ASSERT_EQ(*stringVal, "hello");
    
    config.set("bool_value", true, ConfigSource::Runtime);
    auto boolVal = config.get<bool>("bool_value");
    ASSERT_TRUE(boolVal.has_value());
    ASSERT_TRUE(*boolVal);
}

TEST_F(ConfigTest, ConfigGetOr) {
    int val = config.getOr<int>("missing", 100);
    ASSERT_EQ(val, 100);
    
    config.set("existing", 50, ConfigSource::Runtime);
    val = config.getOr<int>("existing", 100);
    ASSERT_EQ(val, 50);
}

TEST_F(ConfigTest, ConfigRemove) {
    config.set("test", 123, ConfigSource::Runtime);
    ASSERT_TRUE(config.has("test"));
    
    config.remove("test");
    ASSERT_FALSE(config.has("test"));
}

TEST_F(ConfigTest, ConfigKeys) {
    config.set("key1", 1, ConfigSource::Runtime);
    config.set("key2", 2, ConfigSource::Runtime);
    config.set("key3", 3, ConfigSource::Runtime);
    
    auto keys = config.getKeys();
    ASSERT_EQ(keys.size(), 3);
    
    ASSERT_TRUE(config.has("key1"));
    ASSERT_TRUE(config.has("key2"));
    ASSERT_TRUE(config.has("key3"));
}

TEST_F(ConfigTest, ConfigLoadFromFile) {
    // Create a simple JSON file for testing
    std::string testFile = "/tmp/test_config.json";
    
    std::ofstream file(testFile);
    file << "{\"int_value\": 42, \"string_value\": \"test\"}";
    file.close();
    
    bool loaded = config.loadFromFile(testFile);
    ASSERT_TRUE(loaded);
    ASSERT_TRUE(config.has("int_value"));
    ASSERT_TRUE(config.has("string_value"));
    
    auto intVal = config.get<int>("int_value");
    ASSERT_TRUE(intVal.has_value());
    ASSERT_EQ(*intVal, 42);
    
    // Clean up
    std::remove(testFile.c_str());
}

TEST_F(ConfigTest, ConfigSaveToFile) {
    config.set("test_key", "test_value", ConfigSource::Runtime);
    config.set("number", 123, ConfigSource::Runtime);
    
    std::string testFile = "/tmp/test_save_config.json";
    bool saved = config.saveToFile(testFile);
    ASSERT_TRUE(saved);
    
    // Load and verify
    Config loadedConfig;
    ASSERT_TRUE(loadedConfig.loadFromFile(testFile));
    ASSERT_TRUE(loadedConfig.has("test_key"));
    ASSERT_TRUE(loadedConfig.has("number"));
    
    auto value = loadedConfig.get<std::string>("test_key");
    ASSERT_TRUE(value.has_value());
    ASSERT_EQ(*value, "test_value");
    
    // Clean up
    std::remove(testFile.c_str());
}

TEST_F(ConfigTest, ConfigClear) {
    config.set("key1", 1, ConfigSource::Runtime);
    config.set("key2", 2, ConfigSource::Runtime);
    
    config.clear();
    
    ASSERT_FALSE(config.has("key1"));
    ASSERT_FALSE(config.has("key2"));
    
    auto keys = config.getKeys();
    ASSERT_TRUE(keys.empty());
}

TEST_F(ConfigTest, ConfigSummary) {
    config.set("int_value", 42, ConfigSource::Runtime);
    config.set("string_value", "test", ConfigSource::Runtime);
    
    std::string summary = config.summary();
    ASSERT_FALSE(summary.empty());
    ASSERT_NE(summary.find("int_value"), std::string::npos);
    ASSERT_NE(summary.find("string_value"), std::string::npos);
}

TEST_F(ConfigTest, ConfigGetSourceTracking) {
    config.set("runtime_key", 1, ConfigSource::Runtime);
    config.set("file_key", 2, ConfigSource::File);
    config.set("default_key", 3, ConfigSource::Default);
    
    // All should be accessible
    ASSERT_TRUE(config.has("runtime_key"));
    ASSERT_TRUE(config.has("file_key"));
    ASSERT_TRUE(config.has("default_key"));
    
    // Different sources may have different precedence
    // Just verify they're all present and can be retrieved
    ASSERT_TRUE(config.get<int>("runtime_key").has_value());
    ASSERT_TRUE(config.get<int>("file_key").has_value());
    ASSERT_TRUE(config.get<int>("default_key").has_value());
}

} // namespace nlm

int main(int argc, char **argv) {
    std::cout << "=== NLM Config Tests (Google Test) ===" << std::endl;
    std::cout << "Testing Configuration Management:" << std::endl;
    std::cout << "  - Configuration creation and basic operations" << std::endl;
    std::cout << "  - Set/get with different data types" << std::endl;
    std::cout << "  - File I/O operations" << std::endl;
    std::cout << "  - Key management and cleanup" << std::endl;
    std::cout << std::endl;
    
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
