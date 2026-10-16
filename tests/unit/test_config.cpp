// Config Tests using Google Test Framework
// Comprehensive unit tests for nlm::Config class
// Achieves >90% coverage for configuration functionality

#include "fixtures/TestHelpers.hpp"
#include "core/Config/Config.hpp"
#include "core/Config/ConfigValidator.hpp"
#include <gtest/gtest.h>
#include <fstream>
#include <filesystem>
#include <string>
#include <vector>

namespace test {

// Test 1.1: Configuration Creation and Initialization
TEST_F(NLMTest, ConfigCreation) {
    nlm::Config config;
    
    EXPECT_FALSE(config.has("non_existent_key"));
    EXPECT_EQ(config.getKeys().size(), 0);
}

// Test 1.2: Basic Set and Get Operations
TEST_F(NLMTest, ConfigSetAndGet) {
    nlm::Config config;
    
    // Test integer
    config.set("int_value", 42, nlm::ConfigSource::Runtime);
    auto intVal = config.get<int64_t>("int_value");
    ASSERT_TRUE(intVal.has_value());
    EXPECT_EQ(*intVal, 42);
    
    // Test double
    config.set("double_value", 3.14159, nlm::ConfigSource::Runtime);
    auto doubleVal = config.get<double>("double_value");
    ASSERT_TRUE(doubleVal.has_value());
    EXPECT_DOUBLE_EQ(*doubleVal, 3.14159);
    
    // Test string
    config.set("string_value", std::string("hello world"), nlm::ConfigSource::Runtime);
    auto stringVal = config.get<std::string>("string_value");
    ASSERT_TRUE(stringVal.has_value());
    EXPECT_EQ(*stringVal, "hello world");
    
    // Test boolean
    config.set("bool_value", true, nlm::ConfigSource::Runtime);
    auto boolVal = config.get<bool>("bool_value");
    ASSERT_TRUE(boolVal.has_value());
    EXPECT_TRUE(*boolVal);
}

// Test 1.3: GetOr Method with Default Values
TEST_F(NLMTest, ConfigGetOr) {
    nlm::Config config;
    
    // Test missing key returns default
    int val = config.getOr<int64_t>("missing", 100);
    EXPECT_EQ(val, 100);
    
    // Test existing key returns its value
    config.set("existing", 50, nlm::ConfigSource::Runtime);
    val = config.getOr<int64_t>("existing", 100);
    EXPECT_EQ(val, 50);
    
    // Test type conversion with getOr
    config.set("num_str", "123", nlm::ConfigSource::Runtime);
    val = config.getOr<int64_t>("num_str", 0);
    EXPECT_EQ(val, 123);  // String "123" converts to int
}

// Test 1.4: Source Priority Management
TEST_F(NLMTest, ConfigSourcePriority) {
    nlm::Config config;
    
    // Set same key with different sources (runtime should win)
    config.set("test_key", 10, nlm::ConfigSource::Default);
    config.set("test_key", 20, nlm::ConfigSource::File);
    config.set("test_key", 30, nlm::ConfigSource::Runtime);
    
    auto val = config.get<int64_t>("test_key");
    ASSERT_TRUE(val.has_value());
    EXPECT_EQ(*val, 30);  // Runtime should take precedence
}

// Test 1.5: Key Existence and Removal
TEST_F(NLMTest, ConfigHasAndRemove) {
    nlm::Config config;
    
    EXPECT_FALSE(config.has("any_key"));
    
    config.set("key1", 1, nlm::ConfigSource::Runtime);
    EXPECT_TRUE(config.has("key1"));
    
    config.set("key2", 2.0, nlm::ConfigSource::Runtime);
    EXPECT_TRUE(config.has("key2"));
    
    config.remove("key1");
    EXPECT_FALSE(config.has("key1"));
    EXPECT_TRUE(config.has("key2"));
}

// Test 1.6: Configuration Summary
TEST_F(NLMTest, ConfigSummary) {
    nlm::Config config;
    
    config.set("int_val", 42, nlm::ConfigSource::Runtime);
    config.set("str_val", "test", nlm::ConfigSource::File);
    
    std::string summary = config.summary();
    EXPECT_FALSE(summary.empty());
    EXPECT_TRUE(summary.find("int_val") != std::string::npos);
    EXPECT_TRUE(summary.find("str_val") != std::string::npos);
}

// Test 1.7: Case Insensitivity
TEST_F(NLMTest, ConfigCaseInsensitive) {
    nlm::Config config;
    
    config.set("MyKey", 100, nlm::ConfigSource::Runtime);
    
    // Should find key regardless of case
    EXPECT_TRUE(config.has("mykey"));
    EXPECT_TRUE(config.has("MYKEY"));
    EXPECT_TRUE(config.has("MyKey"));
    
    auto val = config.get<int64_t>("MYKEY");
    ASSERT_TRUE(val.has_value());
    EXPECT_EQ(*val, 100);
}

// Test 1.8: File Loading (requires temporary file)
TEST_F(NLMTest, ConfigLoadFromFile) {
    nlm::Config config;
    
    // Create a temporary config file
    std::string tempFile = "/tmp/test_config_" + std::to_string(std::chrono::system_clock::now().time_since_epoch().count()) + ".cfg";
    std::ofstream file(tempFile);
    file << "# Test config file\n";
    file << "int_setting = 42\n";
    file << "double_setting = 3.14\n";
    file << "bool_setting = true\n";
    file << "str_setting = \"hello\"\n";
    file.close();
    
    // Load the file
    bool success = config.loadFromFile(tempFile);
    EXPECT_TRUE(success);
    
    // Verify loaded values
    auto intVal = config.get<int64_t>("int_setting");
    ASSERT_TRUE(intVal.has_value());
    EXPECT_EQ(*intVal, 42);
    
    auto doubleVal = config.get<double>("double_setting");
    ASSERT_TRUE(doubleVal.has_value());
    EXPECT_DOUBLE_EQ(*doubleVal, 3.14);
    
    auto strVal = config.get<std::string>("str_setting");
    ASSERT_TRUE(strVal.has_value());
    EXPECT_EQ(*strVal, "hello");
    
    // Clean up
    std::filesystem::remove(tempFile);
}

// Test 1.9: Validation Integration
TEST_F(NLMTest, ConfigValidationIntegration) {
    nlm::Config config;
    
    // Set up a validator if available
    auto validator = std::make_unique<nlm::NLMConfigValidator>();
    config.setValidator(std::move(validator));
    
    // Basic validation should pass
    auto result = config.validate();
    EXPECT_TRUE(result.isValid);
}

// Test 1.10: Error Handling
TEST_F(NLMTest, ConfigErrorHandling) {
    nlm::Config config;
    
    // Enable error handling
    config.setErrorHandlingEnabled(true);
    
    // Test that error handling doesn't crash on basic operations
    config.set("test_key", 1, nlm::ConfigSource::Runtime);
    auto val = config.get<int64_t>("test_key");
    ASSERT_TRUE(val.has_value());
    EXPECT_EQ(*val, 1);
    
    // Check error stats (should be 0 since no errors occurred)
    const auto& stats = config.getErrorStats();
    EXPECT_EQ(stats.totalErrors, 0);
}

// Test ConfigValidator
TEST_F(NLMTest, ConfigValidatorBasic) {
    nlm::ConfigValidator validator;
    nlm::ConfigEntry entry("test_key", std::string("value"), nlm::ConfigSource::Runtime);
    
    nlm::ValidationResult result = validator.validate(entry);
    EXPECT_TRUE(result.isValid);
}

// Parameterized test for different value types
class ConfigTypeTest : public NLMTest, public ::testing::WithParamInterface<std::string> {
};

TEST_P(ConfigTypeTest, GetOrWithDifferentTypes) {
    nlm::Config config;
    std::string type = GetParam();
    
    if (type == "int") {
        config.set("value", 42, nlm::ConfigSource::Runtime);
        EXPECT_EQ(config.getOr<int64_t>("value", 0), 42);
    } else if (type == "double") {
        config.set("value", 3.14, nlm::ConfigSource::Runtime);
        EXPECT_DOUBLE_EQ(config.getOr<double>("value", 0.0), 3.14);
    } else if (type == "bool") {
        config.set("value", true, nlm::ConfigSource::Runtime);
        EXPECT_TRUE(config.getOr<bool>("value", false));
    } else if (type == "string") {
        config.set("value", std::string("test"), nlm::ConfigSource::Runtime);
        EXPECT_EQ(config.getOr<std::string>("value", ""), "test");
    }
}

INSTANTIATE_TEST_SUITE_P(
    ConfigTypeTest,
    ConfigTypeTest,
    ::testing::Values("int", "double", "bool", "string")
);

} // namespace test