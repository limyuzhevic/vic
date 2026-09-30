// Configuration Tests
// Test Config class with JSON/YAML parsing

#include <gtest/gtest.h>
#include "core/Config/Config.hpp"

namespace nlm {

class ConfigTest : public ::testing::Test {
protected:
    void SetUp() override {
        config = std::make_shared<Config>();
    }
    
    void TearDown() override {
        config.reset();
    }
    
    std::shared_ptr<Config> config;
};

TEST_F(ConfigTest, BasicFunctionality) {
    config->set("test_int", static_cast<int64_t>(42), ConfigSource::User);
    config->set("test_float", 3.14f, ConfigSource::User);
    config->set("test_string", "hello", ConfigSource::User);
    
    EXPECT_EQ(config->get<int64_t>("test_int"), 42);
    EXPECT_NEAR(config->get<float>("test_float"), 3.14f, 0.001f);
    EXPECT_EQ(config->get<std::string>("test_string"), "hello");
}

TEST_F(ConfigTest, DefaultValues) {
    EXPECT_EQ(config->getOrDefault<std::string>("missing", "default"), "default");
    EXPECT_EQ(config->getOrDefault<int64_t>("missing", 0), 0);
    EXPECT_NEAR(config->getOrDefault<float>("missing", 1.0f), 1.0f, 0.001f);
}

TEST_F(ConfigTest, ErrorHandling) {
    EXPECT_THROW(config->get<std::string>("missing"), std::runtime_error);
}

} // namespace nlm

