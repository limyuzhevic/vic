// Config Tests - Updated to Google Test Framework
// This demonstrates migration from custom test framework to Google Test

#include "core/Config/Config.hpp"
#include <gtest/gtest.h>

class ConfigTest : public ::testing::Test {
protected:
    void SetUp() override {
        config = std::make_shared<nlm::Config>();
    }
    
    std::shared_ptr<nlm::Config> config;
};

TEST_F(ConfigTest, Creation) {
    EXPECT_FALSE(config->has("test"));
}

TEST_F(ConfigTest, SetAndGet) {
    config->set("int_value", 42, nlm::ConfigSource::Runtime);
    EXPECT_TRUE(config->has("int_value"));
    
    auto intVal = config->get<int64_t>("int_value");
    EXPECT_TRUE(intVal.has_value());
    EXPECT_EQ(*intVal, 42);
    
    config->set("double_value", 3.14, nlm::ConfigSource::Runtime);
    auto doubleVal = config->get<double>("double_value");
    EXPECT_TRUE(doubleVal.has_value());
    EXPECT_GT(*doubleVal, 3.13);
    EXPECT_LT(*doubleVal, 3.15);
    
    config->set("string_value", std::string("hello"), nlm::ConfigSource::Runtime);
    auto stringVal = config->get<std::string>("string_value");
    EXPECT_TRUE(stringVal.has_value());
    EXPECT_EQ(*stringVal, "hello");
    
    config->set("bool_value", true, nlm::ConfigSource::Runtime);
    auto boolVal = config->get<bool>("bool_value");
    EXPECT_TRUE(boolVal.has_value());
    EXPECT_TRUE(*boolVal);
}

TEST_F(ConfigTest, GetOrWithMissingKey) {
    int val = config->getOr<int64_t>("missing", 100);
    EXPECT_EQ(val, 100);
}

TEST_F(ConfigTest, GetOrWithExistingKey) {
    config->set("existing", 50, nlm::ConfigSource::Runtime);
    int val = config->getOr<int64_t>("existing", 100);
    EXPECT_EQ(val, 50);
}

TEST_F(ConfigTest, Remove) {
    config->set("test", 123, nlm::ConfigSource::Runtime);
    EXPECT_TRUE(config->has("test"));
    
    config->remove("test");
    EXPECT_FALSE(config->has("test"));
}

TEST_F(ConfigTest, GetKeys) {
    config->set("key1", 1, nlm::ConfigSource::Runtime);
    config->set("key2", 2, nlm::ConfigSource::Runtime);
    config->set("key3", 3, nlm::ConfigSource::Runtime);
    
    auto keys = config->getKeys();
    EXPECT_EQ(keys.size(), 3);
}

// Old runAll() function removed - Google Test handles test execution