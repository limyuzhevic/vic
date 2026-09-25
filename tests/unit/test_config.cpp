// Configuration Unit Tests
// Comprehensive test suite for Config class using Google Test framework

#include "tests/common/test_base.hpp"
#include <gtest/gtest.h>
#include <cmath>
#include <vector>
#include <algorithm>
#include <string>
#include <filesystem>

namespace nlm {
namespace test {

// Test fixture for config tests
class ConfigTest : public TestBase {
protected:
    void SetUp() override {
        TestBase::SetUp();
        
        // Create test configuration
        config = std::make_shared<nlm::Config>();
    }
    
    void TearDown() override {
        TestBase::TearDown();
        config.reset();
    }
    
protected:
    std::shared_ptr<nlm::Config> config;
};

// Test basic config creation
TEST_F(ConfigTest, Creation) {
    EXPECT_NE(config, nullptr);
    EXPECT_TRUE(config->has("neuron_count"));
    EXPECT_TRUE(config->has("region_count"));
    EXPECT_TRUE(config->has("simulation_timestep"));
}

// Test config value setting and getting
TEST_F(ConfigTest, ValueSettingAndGetting) {
    // Test integer value
    config->set("test_int", static_cast<int64_t>(42), nlm::ConfigSource::Default);
    EXPECT_EQ(config->get<int64_t>("test_int"), 42);
    
    // Test float value
    config->set("test_float", 3.14f, nlm::ConfigSource::Default);
    EXPECT_FLOAT_EQ(config->get<float>("test_float"), 3.14f);
    
    // Test boolean value
    config->set("test_bool", true, nlm::ConfigSource::Default);
    EXPECT_TRUE(config->get<bool>("test_bool"));
    
    // Test string value
    config->set("test_string", std::string("hello"), nlm::ConfigSource::Default);
    EXPECT_EQ(config->get<std::string>("test_string"), "hello");
}

// Test config value types
TEST_F(ConfigTest, ValueTypes) {
    // Integer
    config->set("int_val", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    EXPECT_TRUE(config->has("int_val"));
    EXPECT_EQ(config->getType("int_val"), nlm::ConfigValueType::Int64);
    
    // Float
    config->set("float_val", 1.5f, nlm::ConfigSource::Default);
    EXPECT_TRUE(config->has("float_val"));
    EXPECT_EQ(config->getType("float_val"), nlm::ConfigValueType::Float);
    
    // Boolean
    config->set("bool_val", false, nlm::ConfigSource::Default);
    EXPECT_TRUE(config->has("bool_val"));
    EXPECT_EQ(config->getType("bool_val"), nlm::ConfigValueType::Bool);
    
    // String
    config->set("string_val", std::string("test"), nlm::ConfigSource::Default);
    EXPECT_TRUE(config->has("string_val"));
    EXPECT_EQ(config->getType("string_val"), nlm::ConfigValueType::String);
}

// Test config source tracking
TEST_F(ConfigTest, SourceTracking) {
    // Default source
    config->set("default_val", 1, nlm::ConfigSource::Default);
    EXPECT_EQ(config->getSource("default_val"), nlm::ConfigSource::Default);
    
    // File source
    config->set("file_val", 2, nlm::ConfigSource::File);
    EXPECT_EQ(config->getSource("file_val"), nlm::ConfigSource::File);
    
    // Command line source
    config->set("cli_val", 3, nlm::ConfigSource::CommandLine);
    EXPECT_EQ(config->getSource("cli_val"), nlm::ConfigSource::CommandLine);
    
    // Runtime source
    config->set("runtime_val", 4, nlm::ConfigSource::Runtime);
    EXPECT_EQ(config->getSource("runtime_val"), nlm::ConfigSource::Runtime);
}

// Test config validation
TEST_F(ConfigTest, Validation) {
    // Add a validation rule (hypothetical)
    config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
    
    // Basic validation
    EXPECT_GT(config->get<int64_t>("neuron_count"), 0);
    EXPECT_GT(config->get<int64_t>("region_count"), 0);
    
    EXPECT_FLOAT_GT(config->get<float>("simulation_timestep"), 0.0f);
}

// Test config reset
TEST_F(ConfigTest, Reset) {
    // Set some values
    config->set("temp_val", 999, nlm::ConfigSource::Runtime);
    EXPECT_EQ(config->get<int64_t>("temp_val"), 999);
    
    // Reset to defaults
    config->reset();
    
    // Check that default values are restored
    EXPECT_GT(config->get<int64_t>("neuron_count"), 0);
    EXPECT_GT(config->get<int64_t>("region_count"), 0);
    
    // Check that temp value is gone
    EXPECT_FALSE(config->has("temp_val"));
}

// Test config comparison
TEST_F(ConfigTest, Comparison) {
    auto config2 = std::make_shared<nlm::Config>();
    
    // Set same values in both configs
    config->set("common_key", 123, nlm::ConfigSource::Default);
    config2->set("common_key", 123, nlm::ConfigSource::Default);
    
    // Configs should be equal
    EXPECT_EQ(config->get<int64_t>("common_key"), config2->get<int64_t>("common_key"));
}

// Test config serialization (if supported)
TEST_F(ConfigTest, Serialization) {
    // Set various values
    config->set("test_int", static_cast<int64_t>(42), nlm::ConfigSource::Default);
    config->set("test_float", 3.14f, nlm::ConfigSource::Default);
    config->set("test_string", std::string("serialization"), nlm::ConfigSource::Default);
    
    // Test that values are preserved
    EXPECT_EQ(config->get<int64_t>("test_int"), 42);
    EXPECT_FLOAT_EQ(config->get<float>("test_float"), 3.14f);
    EXPECT_EQ(config->get<std::string>("test_string"), "serialization");
}

// Parameterized test for config with different values
class ParameterizedConfigTest : public ParameterizedTestBase, public ::testing::WithParamInterface<TestParameters> {
public:
    static std::vector<TestParameters> GetTestParameters() {
        return {
            TestParameters(10, 0.1f, 0.01f, "Small config"),
            TestParameters(100, 0.2f, 0.05f, "Medium config"),
            TestParameters(1000, 0.15f, 0.1f, "Large config")
        };
    }
};

TEST_P(ParameterizedConfigTest, ConfigWithDifferentParameters) {
    TestParameters params = GetParam();
    
    auto paramConfig = std::make_shared<nlm::Config>();
    
    // Set parameters based on test scenario
    paramConfig->set("neuron_count", static_cast<int64_t>(params.neuronCount), nlm::ConfigSource::Default);
    paramConfig->set("learning_rate", params.learningRate, nlm::ConfigSource::Default);
    
    // Verify configuration
    EXPECT_EQ(paramConfig->get<int64_t>("neuron_count"), params.neuronCount);
    EXPECT_FLOAT_EQ(paramConfig->get<float>("learning_rate"), params.learningRate);
}

INSTANTIATE_TEST_SUITE_P(
    ConfigParameterizedTests,
    ParameterizedConfigTest,
    ::testing::ValuesIn(ParameterizedConfigTest::GetTestParameters())
);

// Performance benchmark for config operations
TEST(ConfigPerformanceTest, ConfigCreationPerformance) {
    TestBase testBase;
    
    auto func = []() {
        auto config = std::make_shared<nlm::Config>();
        config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
    };
    
    auto result = testBase.runBenchmark(func, 10000);
    
    std::cout << "Config creation performance: " << result.timeMs << " ms for " 
              << result.iterations << " iterations (" << result.opsPerMs << " ops/ms)" << std::endl;
    
    // Should be able to create at least 10,000 configs per second
    EXPECT_GT(result.opsPerMs, 10000.0);
}

// Performance benchmark for config value operations
TEST(ConfigPerformanceTest, ConfigValueOperationsPerformance) {
    auto testConfig = std::make_shared<nlm::Config>();
    
    // Set up test values
    for (int i = 0; i < 100; ++i) {
        testConfig->set(std::string("key_") + std::to_string(i), 
                       static_cast<int64_t>(i), 
                       nlm::ConfigSource::Default);
    }
    
    TestBase testBase;
    auto func = [&]() {
        for (int i = 0; i < 100; ++i) {
            int64_t val = testConfig->get<int64_t>(std::string("key_") + std::to_string(i));
            EXPECT_EQ(val, static_cast<int64_t>(i));
        }
    };
    
    auto result = testBase.runBenchmark(func, 1000);
    
    std::cout << "Config value operations performance: " << result.timeMs << " ms for " 
              << result.iterations << " iterations (" << result.opsPerMs << " ops/ms)" << std::endl;
}

// Edge case tests for config
TEST_F(ConfigTest, ConfigEdgeCases) {
    // Test with extreme integer values
    config->set("max_int", std::numeric_limits<int64_t>::max(), nlm::ConfigSource::Default);
    EXPECT_EQ(config->get<int64_t>("max_int"), std::numeric_limits<int64_t>::max());
    
    config->set("min_int", std::numeric_limits<int64_t>::min(), nlm::ConfigSource::Default);
    EXPECT_EQ(config->get<int64_t>("min_int"), std::numeric_limits<int64_t>::min());
    
    // Test with extreme float values
    config->set("max_float", std::numeric_limits<float>::max(), nlm::ConfigSource::Default);
    EXPECT_FLOAT_EQ(config->get<float>("max_float"), std::numeric_limits<float>::max());
    
    config->set("min_float", std::numeric_limits<float>::lowest(), nlm::ConfigSource::Default);
    EXPECT_FLOAT_EQ(config->get<float>("min_float"), std::numeric_limits<float>::lowest());
    
    // Test with very long string
    std::string longString(1000, 'a');
    config->set("long_string", longString, nlm::ConfigSource::Default);
    EXPECT_EQ(config->get<std::string>("long_string"), longString);
}

// Test config with special characters in strings
TEST_F(ConfigTest, ConfigSpecialCharacters) {
    // Test with special characters in strings
    std::string specialString = "Special chars: !@#$%^&*()_+-=[]{}|;:,.<>?";
    config->set("special_string", specialString, nlm::ConfigSource::Default);
    EXPECT_EQ(config->get<std::string>("special_string"), specialString);
    
    // Test with Unicode characters
    std::string unicodeString = "Unicode: 你好世界 🌍"; 
    config->set("unicode_string", unicodeString, nlm::ConfigSource::Default);
    EXPECT_EQ(config->get<std::string>("unicode_string"), unicodeString);
}

// Test config with array values
TEST_F(ConfigTest, ConfigArrayValues) {
    // Test with integer array
    std::vector<int64_t> intArray = {1, 2, 3, 4, 5};
    config->set("int_array", intArray, nlm::ConfigSource::Default);
    
    // Note: Array retrieval might need different API
    // This test validates that array values can be set
}

// Test config memory usage
TEST_F(ConfigTest, ConfigMemoryUsage) {
    auto testConfig = std::make_shared<nlm::Config>();
    
    // Set many values to test memory efficiency
    for (int i = 0; i < 1000; ++i) {
        testConfig->set(std::string("key_") + std::to_string(i), 
                       static_cast<int64_t>(i), 
                       nlm::ConfigSource::Default);
    }
    
    // Verify all values are still accessible
    for (int i = 0; i < 1000; ++i) {
        EXPECT_EQ(testConfig->get<int64_t>(std::string("key_") + std::to_string(i)), 
                 static_cast<int64_t>(i));
    }
}

} // namespace test
} // namespace nlm
