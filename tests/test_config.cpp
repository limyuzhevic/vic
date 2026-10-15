// Config Tests - GoogleTest Version
#include "core/Config/Config.hpp"
#include <gtest/gtest.h>
#include <string>

namespace {

TEST(ConfigTest, ConfigCreation) {
    nlm::Config config;
    EXPECT_FALSE(config.has("test"));
}

TEST(ConfigTest, ConfigSetAndGet) {
    nlm::Config config;
    
    config.set("int_value", 42, nlm::ConfigSource::Runtime);
    EXPECT_TRUE(config.has("int_value"));
    
    auto intVal = config.get<int>("int_value");
    EXPECT_TRUE(intVal.has_value());
    EXPECT_EQ(*intVal, 42);
    
    config.set("double_value", 3.14, nlm::ConfigSource::Runtime);
    auto doubleVal = config.get<double>("double_value");
    EXPECT_TRUE(doubleVal.has_value());
    EXPECT_GT(*doubleVal, 3.13);
    EXPECT_LT(*doubleVal, 3.15);
    
    config.set("string_value", std::string("hello"), nlm::ConfigSource::Runtime);
    auto stringVal = config.get<std::string>("string_value");
    EXPECT_TRUE(stringVal.has_value());
    EXPECT_EQ(*stringVal, "hello");
    
    config.set("bool_value", true, nlm::ConfigSource::Runtime);
    auto boolVal = config.get<bool>("bool_value");
    EXPECT_TRUE(boolVal.has_value());
    EXPECT_TRUE(*boolVal);
}

TEST(ConfigTest, ConfigGetOr) {
    nlm::Config config;
    
    int val = config.getOr<int>("missing", 100);
    EXPECT_EQ(val, 100);
    
    config.set("existing", 50, nlm::ConfigSource::Runtime);
    val = config.getOr<int>("existing", 100);
    EXPECT_EQ(val, 50);
}

TEST(ConfigTest, ConfigRemove) {
    nlm::Config config;
    
    config.set("test", 123, nlm::ConfigSource::Runtime);
    EXPECT_TRUE(config.has("test"));
    
    config.remove("test");
    EXPECT_FALSE(config.has("test"));
}

TEST(ConfigTest, ConfigKeys) {
    nlm::Config config;
    
    config.set("key1", 1, nlm::ConfigSource::Runtime);
    config.set("key2", 2, nlm::ConfigSource::Runtime);
    config.set("key3", 3, nlm::ConfigSource::Runtime);
    
    auto keys = config.getKeys();
    EXPECT_EQ(keys.size(), 3);
}

TEST(ConfigTest, ConfigAdvancedFeatures) {
    nlm::Config config;
    
    // Test multiple values with same key
    config.set("value", 1, nlm::ConfigSource::Default);
    config.set("value", 2, nlm::ConfigSource::Runtime);
    
    auto val = config.get<int>("value");
    EXPECT_TRUE(val.has_value());
    EXPECT_EQ(*val, 2); // Runtime should override Default
    
    // Test ConfigSource enum
    config.set("default_key", "default", nlm::ConfigSource::Default);
    config.set("runtime_key", "runtime", nlm::ConfigSource::Runtime);
    
    EXPECT_EQ(config.getSource("default_key"), nlm::ConfigSource::Default);
    EXPECT_EQ(config.getSource("runtime_key"), nlm::ConfigSource::Runtime);
}

TEST(ConfigTest, ConfigHasWithMultipleSources) {
    nlm::Config config;
    
    // Test that has works regardless of source
    config.set("test1", 1, nlm::ConfigSource::Default);
    EXPECT_TRUE(config.has("test1"));
    
    config.set("test2", 2, nlm::ConfigSource::Runtime);
    EXPECT_TRUE(config.has("test2"));
    
    config.set("test3", 3, nlm::ConfigSource::File);
    EXPECT_TRUE(config.has("test3"));
}

} // namespace

// Backward compatibility - keep the runAll function for existing test_main.cpp
namespace test_config {
    void runAll();
}