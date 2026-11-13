// Config Tests
#include "core/Config/Config.hpp"
#include <gtest/gtest.h>
#include <iostream>

TEST(ConfigTest, Creation) {
    nlm::Config config;
    EXPECT_FALSE(config.has("test"));
    
    std::cout << "    testConfigCreation passed" << std::endl;
}

TEST(ConfigTest, SetAndGet) {
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
    EXPECT_TRUE(*boolVal == true);
    
    std::cout << "    testConfigSetAndGet passed" << std::endl;
}

TEST(ConfigTest, GetOr) {
    nlm::Config config;
    
    int val = config.getOr<int>("missing", 100);
    EXPECT_EQ(val, 100);
    
    config.set("existing", 50, nlm::ConfigSource::Runtime);
    val = config.getOr<int>("existing", 100);
    EXPECT_EQ(val, 50);
    
    std::cout << "    testConfigGetOr passed" << std::endl;
}

TEST(ConfigTest, Remove) {
    nlm::Config config;
    
    config.set("test_key", 123, nlm::ConfigSource::Runtime);
    EXPECT_TRUE(config.has("test_key"));
    
    config.remove("test_key");
    EXPECT_FALSE(config.has("test_key"));
    
    std::cout << "    testConfigRemove passed" << std::endl;
}

TEST(ConfigTest, Clear) {
    nlm::Config config;
    
    config.set("key1", 1, nlm::ConfigSource::Runtime);
    config.set("key2", 2.0, nlm::ConfigSource::Runtime);
    config.set("key3", "value", nlm::ConfigSource::Runtime);
    
    EXPECT_TRUE(config.has("key1"));
    EXPECT_TRUE(config.has("key2"));
    EXPECT_TRUE(config.has("key3"));
    
    config.clear();
    EXPECT_FALSE(config.has("key1"));
    EXPECT_FALSE(config.has("key2"));
    EXPECT_FALSE(config.has("key3"));
    
    std::cout << "    testConfigClear passed" << std::endl;
}

TEST(ConfigTest, Summary) {
    nlm::Config config;
    
    config.set("test_key", 42, nlm::ConfigSource::Runtime);
    
    std::string summary = config.summary();
    EXPECT_FALSE(summary.empty());
    EXPECT_TRUE(summary.find("test_key") != std::string::npos);
    
    std::cout << "    testConfigSummary passed" << std::endl;
}

int main(int argc, char** argv) {
    std::cout << "=== NLM Configuration Tests ===" << std::endl;
    
    ::testing::InitGoogleTest(&argc, argv);
    int result = RUN_ALL_TESTS();
    
    std::cout << "=== Configuration Tests Complete ===" << std::endl;
    return result;
}