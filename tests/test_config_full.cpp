// Comprehensive test for Config class functionality
// This test verifies all the completed implementations

#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>
#include <vector>

void testBasicConfig() {
    std::cout << "Running basic Config tests...\n";
    
    nlm::Config config;
    
    // Test initial state
    assert(!config.has("test"));
    
    // Test setting different types
    config.set("int_value", 42);
    config.set("double_value", 3.14);
    config.set("string_value", "hello");
    config.set("bool_value", true);
    
    // Test getting values
    auto intVal = config.get<int>("int_value");
    assert(intVal.has_value());
    assert(*intVal == 42);
    
    auto doubleVal = config.get<double>("double_value");
    assert(doubleVal.has_value());
    assert(*doubleVal > 3.13 && *doubleVal < 3.15);
    
    auto stringVal = config.get<std::string>("string_value");
    assert(stringVal.has_value());
    assert(*stringVal == "hello");
    
    auto boolVal = config.get<bool>("bool_value");
    assert(boolVal.has_value());
    assert(*boolVal == true);
    
    std::cout << "  Basic config tests passed\n";
}

void testGetOr() {
    std::cout << "Running getOr tests...\n";
    
    nlm::Config config;
    
    int val = config.getOr<int>("missing", 100);
    assert(val == 100);
    
    config.set("existing", 50);
    val = config.getOr<int>("existing", 100);
    assert(val == 50);
    
    std::cout << "  getOr tests passed\n";
}

void testRemoveAndKeys() {
    std::cout << "Running remove and getKeys tests...\n";
    
    nlm::Config config;
    
    config.set("key1", 1);
    config.set("key2", 2);
    config.set("key3", 3);
    
    assert(config.has("key1"));
    
    config.remove("key2");
    assert(!config.has("key2"));
    
    auto keys = config.getKeys();
    assert(keys.size() == 2);
    assert(std::find(keys.begin(), keys.end(), "key1") != keys.end());
    assert(std::find(keys.begin(), keys.end(), "key3") != keys.end());
    
    std::cout << "  remove and getKeys tests passed\n";
}

void testClear() {
    std::cout << "Running clear test...\n";
    
    nlm::Config config;
    
    config.set("key1", 1);
    config.set("key2", 2);
    
    assert(config.has("key1"));
    assert(config.has("key2"));
    
    config.clear();
    
    assert(!config.has("key1"));
    assert(!config.has("key2"));
    
    std::cout << "  clear test passed\n";
}

void testStringParsing() {
    std::cout << "Running string parsing tests...\n";
    
    nlm::Config config;
    
    // Test boolean parsing
    config.set("bool_true", "true");
    auto boolTrue = config.get<bool>("bool_true");
    assert(boolTrue.has_value() && *boolTrue == true);
    
    config.set("bool_false", "false");
    auto boolFalse = config.get<bool>("bool_false");
    assert(boolFalse.has_value() && *boolFalse == false);
    
    // Test integer parsing
    config.set("int_str", "123");
    auto intVal = config.get<int>("int_str");
    assert(intVal.has_value() && *intVal == 123);
    
    // Test double parsing
    config.set("double_str", "3.14159");
    auto doubleVal = config.get<double>("double_str");
    assert(doubleVal.has_value());
    assert(*doubleVal > 3.14 && *doubleVal < 3.15);
    
    // Test numeric alternatives
    config.set("num_str", "456");
    auto numVal = config.get<int>("num_str");
    assert(numVal.has_value() && *numVal == 456);
    
    std::cout << "  string parsing tests passed\n";
}

void testJSONRoundtrip() {
    std::cout << "Running JSON roundtrip tests...\n";
    
    nlm::Config config;
    
    // Set up test data
    config.set("int_val", 42);
    config.set("double_val", 3.14);
    config.set("string_val", "test");
    config.set("bool_val", true);
    config.set("vector_int", std::vector<int>{1, 2, 3});
    
    // Convert to JSON
    nlohmann::json json;
    for (const auto& key : config.getKeys()) {
        json[key] = config.configValueToJSON(config.get<nlm::ConfigValue>(key).value_or(nlm::ConfigValue()));
    }
    
    // Convert back to Config
    nlm::Config config2;
    config2.loadFromJSON(json.dump());
    
    // Verify values
    assert(config2.get<int>("int_val").has_value() && *config2.get<int>("int_val") == 42);
    assert(config2.get<double>("double_val").has_value());
    assert(config2.get<std::string>("string_val").has_value() && *config2.get<std::string>("string_val") == "test");
    assert(config2.get<bool>("bool_val").has_value() && *config2.get<bool>("bool_val") == true);
    
    // Vector test
    auto vecVal = config2.get<std::vector<int>>("vector_int");
    assert(vecVal.has_value());
    assert(vecVal.value() == std::vector<int>{1, 2, 3});
    
    std::cout << "  JSON roundtrip tests passed\n";
}

void testKeyValueFormat() {
    std::cout << "Running key=value format tests...\n";
    
    nlm::Config config;
    
    // Save to key=value format
    config.set("name", "Test Config");
    config.set("version", 1.0);
    config.set("enabled", true);
    config.set("count", 5);
    
    std::string summary = config.summary();
    assert(summary.find("Configuration Summary:") != std::string::npos);
    assert(summary.find("name = \"Test Config\"") != std::string::npos);
    assert(summary.find("version = 1.0") != std::string::npos);
    assert(summary.find("enabled = true") != std::string::npos);
    assert(summary.find("count = 5") != std::string::npos);
    
    std::cout << "  key=value format tests passed\n";
}

void testValidation() {
    std::cout << "Running validation tests...\n";
    
    nlm::Config config;
    
    // Set values according to schema
    config.set("random_seed", 42);
    config.set("neuron_count", 100);
    config.set("region_count", 2);
    config.set("simulation_timestep", 0.001);
    config.set("connection_probability", 0.1);
    config.set("log_to_file", true);
    config.set("visualization_enabled", false);
    config.set("environment_name", "GridWorld");
    
    // Validate all
    auto errors = config.validate();
    
    // All should be valid
    assert(errors.empty());
    
    std::cout << "  validation tests passed\n";
}

void testDefaultSchema() {
    std::cout << "Running default schema tests...\n";
    
    nlm::Config config;
    
    // Check that default schema exists
    auto schema = config.getSchema();
    assert(!schema.empty());
    
    // Check key fields exist
    assert(schema.find("random_seed") != schema.end());
    assert(schema.find("neuron_count") != schema.end());
    assert(schema.find("region_count") != schema.end());
    assert(schema.find("simulation_timestep") != schema.end());
    assert(schema.find("connection_probability") != schema.end());
    assert(schema.find("log_to_file") != schema.end());
    assert(schema.find("visualization_enabled") != schema.end());
    assert(schema.find("environment_name") != schema.end());
    
    // Check required fields
    assert(schema.at("random_seed").required);
    assert(schema.at("neuron_count").required);
    assert(schema.at("region_count").required);
    assert(schema.at("environment_name").required);
    
    std::cout << "  default schema tests passed\n";
}

void runAllTests() {
    std::cout << "=== Config Class Tests ===\n\n";
    
    testBasicConfig();
    testGetOr();
    testRemoveAndKeys();
    testClear();
    testStringParsing();
    testJSONRoundtrip();
    testKeyValueFormat();
    testValidation();
    testDefaultSchema();
    
    std::cout << "\n=== All tests passed! ===\n";
}

int main() {
    runAllTests();
    return 0;
}
