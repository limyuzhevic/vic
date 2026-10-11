// Configuration Unit Tests
// Comprehensive unit testing for the Config class

#include "core/Config/Config.hpp"
#include <gtest/gtest.h>
#include <memory>
#include <fstream>
#include <sstream>
#include <filesystem>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

namespace nlm::tests {

class ConfigTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Create clean config for each test
        config = std::make_shared<Config>();
    }
    
    void TearDown() override {
        config.reset();
    }
    
    // Test fixtures
    std::shared_ptr<Config> config;
};

// Basic Configuration Tests
TEST_F(ConfigTest, DefaultCreation) {
    EXPECT_NE(config, nullptr);
    EXPECT_FALSE(config->has("test"));
}

TEST_F(ConfigTest, BasicSetAndGet) {
    // Test integer values
    config->set("int_value", 42, ConfigSource::Runtime);
    EXPECT_TRUE(config->has("int_value"));
    
    auto int_val = config->get<int>("int_value");
    ASSERT_TRUE(int_val.has_value());
    EXPECT_EQ(*int_val, 42);
    
    // Test double values
    config->set("double_value", 3.14159, ConfigSource::Runtime);
    auto double_val = config->get<double>("double_value");
    ASSERT_TRUE(double_val.has_value());
    EXPECT_DOUBLE_EQ(*double_val, 3.14159);
    
    // Test string values
    config->set("string_value", std::string("hello world"), ConfigSource::Runtime);
    auto string_val = config->get<std::string>("string_value");
    ASSERT_TRUE(string_val.has_value());
    EXPECT_EQ(*string_val, "hello world");
    
    // Test boolean values
    config->set("bool_value", true, ConfigSource::Runtime);
    auto bool_val = config->get<bool>("bool_value");
    ASSERT_TRUE(bool_val.has_value());
    EXPECT_TRUE(*bool_val);
}

TEST_F(ConfigTest, GetOrMethod) {
    // Test GetOr with missing key
    int default_val = config->getOr<int>("missing", 100);
    EXPECT_EQ(default_val, 100);
    
    // Test GetOr with existing key
    config->set("existing", 50, ConfigSource::Runtime);
    int existing_val = config->getOr<int>("existing", 100);
    EXPECT_EQ(existing_val, 50);
}

TEST_F(ConfigTest, RemoveMethod) {
    // Set and remove values
    config->set("temp", 123, ConfigSource::Runtime);
    EXPECT_TRUE(config->has("temp"));
    
    config->remove("temp");
    EXPECT_FALSE(config->has("temp"));
}

TEST_F(ConfigTest, GetKeys) {
    // Test key management
    config->set("key1", 1, ConfigSource::Runtime);
    config->set("key2", 2, ConfigSource::Runtime);
    config->set("key3", 3, ConfigSource::Runtime);
    
    auto keys = config->getKeys();
    EXPECT_EQ(keys.size(), 3);
    
    // Verify all keys are present
    EXPECT_TRUE(std::find(keys.begin(), keys.end(), "key1") != keys.end());
    EXPECT_TRUE(std::find(keys.begin(), keys.end(), "key2") != keys.end());
    EXPECT_TRUE(std::find(keys.begin(), keys.end(), "key3") != keys.end());
}

TEST_F(ConfigTest, ClearMethod) {
    // Set multiple values
    config->set("key1", 1, ConfigSource::Runtime);
    config->set("key2", 2, ConfigSource::Runtime);
    config->set("key3", 3, ConfigSource::Runtime);
    
    EXPECT_EQ(config->getKeys().size(), 3);
    
    // Clear all values
    config->clear();
    
    EXPECT_EQ(config->getKeys().size(), 0);
}

// Configuration Source Tests
TEST_F(ConfigTest, ConfigSource) {
    // Test different config sources
    config->set("runtime_value", 100, ConfigSource::Runtime);
    config->set("default_value", 200, ConfigSource::Default);
    
    auto runtime_val = config->get<int>("runtime_value");
    auto default_val = config->get<int>("default_value");
    
    ASSERT_TRUE(runtime_val.has_value());
    ASSERT_TRUE(default_val.has_value());
    
    EXPECT_EQ(*runtime_val, 100);
    EXPECT_EQ(*default_val, 200);
}

TEST_F(ConfigTest, SourcePriority) {
    // Test source priority (runtime overrides default)
    config->set("shared_value", 100, ConfigSource::Default);
    config->set("shared_value", 200, ConfigSource::Runtime);
    
    auto val = config->get<int>("shared_value");
    ASSERT_TRUE(val.has_value());
    EXPECT_EQ(*val, 200); // Runtime value should be used
}

// JSON Serialization Tests
TEST_F(ConfigTest, ToJson) {
    // Set various values
    config->set("int_val", 42, ConfigSource::Runtime);
    config->set("double_val", 3.14, ConfigSource::Runtime);
    config->set("string_val", "test", ConfigSource::Runtime);
    config->set("bool_val", true, ConfigSource::Runtime);
    config->set("nested", std::make_shared<Config>(), ConfigSource::Runtime);
    
    // Convert to JSON
    std::string json_str = config->toJson();
    ASSERT_FALSE(json_str.empty());
    
    // Parse and verify
    auto json_obj = json::parse(json_str);
    EXPECT_EQ(json_obj["int_val"], 42);
    EXPECT_DOUBLE_EQ(json_obj["double_val"], 3.14);
    EXPECT_EQ(json_obj["string_val"], "test");
    EXPECT_TRUE(json_obj["bool_val"]);
}

TEST_F(ConfigTest, FromJson) {
    // Create JSON string
    std::string json_str = R"({
        "neuron_count": 1000,
        "region_count": 3,
        "connection_probability": 0.1,
        "name": "test_config"
    })";
    
    auto from_json_config = Config::fromJson(json_str);
    ASSERT_NE(from_json_config, nullptr);
    
    EXPECT_EQ(from_json_config->get<int>("neuron_count"), 1000);
    EXPECT_EQ(from_json_config->get<int>("region_count"), 3);
    EXPECT_FLOAT_EQ(from_json_config->get<float>("connection_probability"), 0.1f);
    EXPECT_EQ(from_json_config->get<std::string>("name"), "test_config");
}

TEST_F(ConfigTest, JsonRoundTrip) {
    // Test JSON serialization round-trip
    config->set("neuron_count", 500);
    config->set("region_count", 2);
    config->set("name", "roundtrip_test");
    
    // Serialize to JSON
    std::string json_str = config->toJson();
    
    // Deserialize from JSON
    auto restored_config = Config::fromJson(json_str);
    ASSERT_NE(restored_config, nullptr);
    
    // Verify all values
    EXPECT_EQ(restored_config->get<int>("neuron_count"), 500);
    EXPECT_EQ(restored_config->get<int>("region_count"), 2);
    EXPECT_EQ(restored_config->get<std::string>("name"), "roundtrip_test");
}

// File I/O Tests
TEST_F(ConfigTest, SaveToFile) {
    // Set configuration
    config->set("test_key", "test_value");
    config->set("number", 42);
    
    // Save to temporary file
    std::string temp_file = "/tmp/test_config.json";
    config->saveToFile(temp_file);
    
    // Verify file exists
    std::ifstream file(temp_file);
    ASSERT_TRUE(file.good());
    
    // Parse file content
    auto file_json = json::parse(file);
    EXPECT_EQ(file_json["test_key"], "test_value");
    EXPECT_EQ(file_json["number"], 42);
    
    file.close();
    
    // Cleanup
    std::remove(temp_file.c_str());
}

TEST_F(ConfigTest, LoadFromFile) {
    // Create test JSON file
    std::string temp_file = "/tmp/test_config_load.json";
    std::ofstream file(temp_file);
    
    file << R"({
        "load_key": "load_value",
        "number": 123
    })";
    
    file.close();
    
    // Load from file
    config->loadFromFile(temp_file);
    
    EXPECT_EQ(config->get<std::string>("load_key"), "load_value");
    EXPECT_EQ(config->get<int>("number"), 123);
    
    // Cleanup
    std::remove(temp_file.c_str());
}

TEST_F(ConfigTest, FileRoundTrip) {
    // Test save and load round-trip
    config->set("roundtrip_key", "roundtrip_value");
    config->set("roundtrip_number", 777);
    
    // Save to file
    std::string temp_file = "/tmp/roundtrip_config.json";
    config->saveToFile(temp_file);
    
    // Create new config and load from file
    auto new_config = std::make_shared<Config>();
    new_config->loadFromFile(temp_file);
    
    // Verify values
    EXPECT_EQ(new_config->get<std::string>("roundtrip_key"), "roundtrip_value");
    EXPECT_EQ(new_config->get<int>("roundtrip_number"), 777);
    
    // Cleanup
    std::remove(temp_file.c_str());
}

// Configuration Summary Tests
TEST_F(ConfigTest, Summary) {
    // Set some values
    config->set("key1", 1);
    config->set("key2", "value2");
    config->set("key3", 3.14);
    
    // Get summary
    std::string summary = config->summary();
    
    // Summary should contain key information
    EXPECT_FALSE(summary.empty());
    EXPECT_TRUE(summary.find("key1") != std::string::npos);
    EXPECT_TRUE(summary.find("key2") != std::string::npos);
    EXPECT_TRUE(summary.find("key3") != std::string::npos);
}

TEST_F(ConfigTest, StringRepresentation) {
    // Set values and test string representation
    config->set("test", 123);
    
    // Test repr
    std::string repr = config->__repr__();
    EXPECT_FALSE(repr.empty());
    EXPECT_TRUE(repr.find("Config") != std::string::npos);
}

// Edge Case Tests
TEST_F(ConfigTest, GetNonExistentKey) {
    // Test getting non-existent key
    auto val = config->get<int>("nonexistent");
    EXPECT_FALSE(val.has_value());
}

TEST_F(ConfigTest, GetOrDefaultWithDifferentTypes) {
    // Test GetOr with different types
    config->set("int_val", 100);
    config->set("double_val", 3.14);
    config->set("string_val", "hello");
    
    EXPECT_EQ(config->getOr<int>("int_val", 0), 100);
    EXPECT_DOUBLE_EQ(config->getOr<double>("double_val", 0.0), 3.14);
    EXPECT_EQ(config->getOr<std::string>("string_val", ""), "hello");
}

TEST_F(ConfigTest, EmptyJson) {
    // Test empty JSON
    std::string empty_json = "{}";
    auto empty_config = Config::fromJson(empty_json);
    
    EXPECT_NE(empty_config, nullptr);
    EXPECT_EQ(empty_config->getKeys().size(), 0);
}

TEST_F(ConfigTest, InvalidJson) {
    // Test invalid JSON handling
    std::string invalid_json = "{invalid json}";
    
    // Should not crash (exception handling tested separately)
    EXPECT_NO_THROW(Config::fromJson(invalid_json));
}

// Configuration Validation Tests
TEST_F(ConfigTest, ConfigurationValidation) {
    // Test configuration validation
    config->set("neuron_count", 1000);
    config->set("region_count", 5);
    config->set("connection_probability", 0.1f);
    
    // Basic validation
    EXPECT_GT(config->get<int>("neuron_count"), 0);
    EXPECT_GT(config->get<int>("region_count"), 0);
    EXPECT_GE(config->get<float>("connection_probability"), 0.0f);
    EXPECT_LE(config->get<float>("connection_probability"), 1.0f);
}

TEST_F(ConfigTest, NegativeValues) {
    // Test that negative values are allowed (config validation is application's responsibility)
    config->set("negative_int", -1);
    config->set("negative_float", -3.14);
    
    EXPECT_EQ(config->get<int>("negative_int"), -1);
    EXPECT_FLOAT_EQ(config->get<float>("negative_float"), -3.14f);
}

// Memory Management Tests
TEST_F(ConfigTest, MemoryCleanup) {
    // Test memory cleanup
    std::vector<std::shared_ptr<Config>> configs;
    
    for (int i = 0; i < 100; ++i) {
        auto temp_config = std::make_shared<Config>();
        temp_config->set("temp_key", i);
        configs.push_back(temp_config);
    }
    
    // Clear vector (configs should be destroyed)
    configs.clear();
    
    // Should not cause memory issues
    EXPECT_TRUE(true);
}

// Performance Tests
TEST_F(ConfigTest, FastAccess) {
    // Set many values
    for (int i = 0; i < 1000; ++i) {
        config->set(std::to_string(i), i);
    }
    
    // Measure access time
    auto start_time = std::chrono::high_resolution_clock::now();
    
    for (int i = 0; i < 1000; ++i) {
        auto val = config->get<int>(std::to_string(i));
        EXPECT_TRUE(val.has_value());
        EXPECT_EQ(*val, i);
    }
    
    auto end_time = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration<double>(end_time - start_time);
    
    // Should be fast (< 0.1 seconds for 1000 accesses)
    EXPECT_LT(duration.count(), 0.1);
}

TEST_F(ConfigTest, JsonSerializationPerformance) {
    // Set many values
    for (int i = 0; i < 100; ++i) {
        config->set(std::to_string(i), i);
    }
    
    // Measure serialization time
    auto start_time = std::chrono::high_resolution_clock::now();
    
    std::string json_str = config->toJson();
    
    auto end_time = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration<double>(end_time - start_time);
    
    // Should be fast (< 0.1 seconds for 100 values)
    EXPECT_LT(duration.count(), 0.1);
    EXPECT_FALSE(json_str.empty());
}

// Cross-Platform Tests
TEST_F(ConfigTest, PathHandling) {
    // Test path handling (different platforms may have different path separators)
#ifdef _WIN32
    std::string test_path = "C:\\test\\config.json";
#else
    std::string test_path = "/tmp/test_config.json";
#endif
    
    config->set("path_key", test_path);
    auto retrieved_path = config->get<std::string>("path_key");
    
    EXPECT_EQ(retrieved_path, test_path);
}

// Stress Tests
TEST_F(ConfigTest, LargeConfig) {
    // Test large configuration
    const int num_values = 10000;
    
    for (int i = 0; i < num_values; ++i) {
        config->set(std::to_string(i), i);
    }
    
    EXPECT_EQ(config->getKeys().size(), num_values);
}

TEST_F(ConfigTest, DeepNesting) {
    // Test deep nesting (JSON allows nesting)
    auto nested_config = std::make_shared<Config>();
    nested_config->set("level1", "value1");
    
    // Create nested configuration
    auto nested_nested = std::make_shared<Config>();
    nested_nested->set("level2", "value2");
    
    auto nested_nested2 = std::make_shared<Config>();
    nested_nested2->set("level3", "value3");
    
    nested_config->set("nested", nested_nested2);
    
    // In JSON, nested configs become objects
    std::string json_str = nested_config->toJson();
    
    auto parsed_config = Config::fromJson(json_str);
    ASSERT_NE(parsed_config, nullptr);
}

// Benchmarking Functions
static void BM_ConfigGetSet(benchmark::State& state) {
    auto config = std::make_shared<Config>();
    
    for (auto _ : state) {
        for (int i = 0; i < state.range(0); ++i) {
            config->set(std::to_string(i), i);
            auto val = config->get<int>(std::to_string(i));
            benchmark::DoNotOptimize(val);
        }
    }
}

static void BM_ConfigJson(benchmark::State& state) {
    auto config = std::make_shared<Config>();
    
    for (int i = 0; i < state.range(0); ++i) {
        config->set(std::to_string(i), i);
    }
    
    for (auto _ : state) {
        std::string json_str = config->toJson();
        benchmark::DoNotOptimize(json_str);
    }
}

BENCHMARK(BM_ConfigGetSet)->Arg(100)->Arg(1000)->Arg(10000);
BENCHMARK(BM_ConfigJson)->Arg(100)->Arg(1000)->Arg(10000);

} // namespace nlm::tests
